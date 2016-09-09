//---------------------------------------------------------------------------
//Copyright 2008 Scott Swift - This program is distributed under the
//terms of the GNU General Public License.
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "MainForm.h"
#include "stdio.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "AdPort"
#pragma link "OoMisc"
#pragma resource "*.dfm"
TFormS900 *FormS900;
//---------------------------------------------------------------------------
__fastcall TFormS900::TFormS900(TComponent* Owner)
        : TForm(Owner)
{
    Memo1->Lines->Add("Version: September 9, 2016");
    Memo1->Lines->Add("Click the menu above and select \"Help\"...");
}
//---------------------------------------------------------------------------
void __fastcall TFormS900::FormCreate(TObject *Sender)
{
    this->g_byteCount = 0;
    this->g_numSampEntries = 0;
    this->g_numProgEntries = 0;

    g_DragDropFilePath = "";
    
    //enable drag&drop files
    ::DragAcceptFiles(this->Handle, true);
}
//---------------------------------------------------------------------------
// Get sample routines
//---------------------------------------------------------------------------
int __fastcall TFormS900::get_sample(int samp, String Name)
{
    unsigned int my_magic;
    PSTOR ps;
    int iFileHandle;

    if (!FileExists(Name))
        iFileHandle = FileCreate(Name);
    else
    {
        iFileHandle = FileOpen(Name, fmOpenReadWrite);
        FileSeek(iFileHandle, 0, 0); // Back to beginning of the file
    }

    my_magic = MAGIC_NUM;

    exmit(0, SECRE); /* request common reception enable */

    /* first interrogate S900 to get sample parameters... */

    /* request sample parms */
    exmit(samp, RSPRM);

    if (getdata2(samp_parms, sizeof(samp_parms)))
    {
        exmit(0, SECRD); /* request common reception disable */
        FileClose(iFileHandle);
        return(1); /* could not get header data */
    }

    // copy into samp_parms
    memcpy(samp_parms, TempArray, PARMSIZ);

    if (get_samp_hedr(samp))
    {
        exmit(0, SECRD); /* request common reception disable */
        FileClose(iFileHandle);
        return(2); /* could not get header data */
    }

    // copy into samp_hedr
    memcpy(samp_hedr, TempArray, HEDRSIZ);

    decode_parameters(&ps); /* fill new sample's PSTOR struct... */

    print_ps_info(&ps);

    /* write the magic number and header... */
    FileWrite(iFileHandle, (char *)&my_magic, sizeof(my_magic));
    FileWrite(iFileHandle, (char *)&ps, sizeof(PSTOR));

    if (get_samp_data(&ps, iFileHandle) != 0)
    {
        exmit(0,SECRD); /* request common reception disable */
        FileClose(iFileHandle);
        return(3); /* could not get sample data */
    }

    exmit(0,SECRD); /* request common reception disable */

    FileClose(iFileHandle);

    return(0);

    // 1=no parms, 2=no header, 3=no samp data
}
//---------------------------------------------------------------------------
int __fastcall TFormS900::get_samp_hedr(int samp)
{
    cxmit(samp, RSD); /* request sample dump */

    /* see if chars waiting from serial port... */
    if (receive(HEDRSIZ))
    {
        printerror("error receiving data");
        return(1);
    }

    return(0); /* 0=OK, 1=com error */
}
//---------------------------------------------------------------------------
void __fastcall TFormS900::decode_parameters(PSTOR * ps)
{
    /* number of sample words */
    ps->totalct = decode_hedrTB((unsigned char *)&samp_hedr[9+2]);

    /* start time */
    ps->startidx = decode_parmsDD((unsigned char *)&samp_parms[71+(7-1)]);

    /* end time */
    ps->endidx = decode_hedrTB((unsigned char *)&samp_hedr[15+2]) - 1;

    /* loop time */
    ps->loopidx = decode_hedrTB((unsigned char *)&samp_hedr[12+2]) - 1;

    /* sampling frequency */
    ps->freq = decode_parmsDW((unsigned char *)&samp_parms[47+(3-1)]);

    /* sampling period */
    /* for S900, we'll calculate our own value for period */
    ps->period = 1.0e9/(double)ps->freq;

    /* pitch */
    ps->pitch = decode_parmsDW((unsigned char *)&samp_parms[51+(3-1)]);

    /* ASCII sample name */
    decode_parmsDB((unsigned char *)&samp_parms[7], (unsigned char *)ps->name, MAX_NAME_S900);
    trim(ps->name, ps->name); /* trim off the blanks */

    /* looping mode */
    decode_parmsDB((unsigned char *)&samp_parms[59], &ps->looping, 1);

    ps->bits_per_sample = (short unsigned int)samp_hedr[5];
}
//---------------------------------------------------------------------------
int __fastcall TFormS900::get_samp_data(PSTOR * ps, int handle)
{
    char rbuf[WORDS_PER_BLOCK*2];

    unsigned int count = 0;
    int writct, retstat;
    String dots = "";
    int blockct = 0;
    int status = 0;

    while(!status)
    {
        chandshake(ACKS);

        if (count >= ps->totalct)
            break;

        if ((retstat = getdata1((short int *)rbuf, WORDS_PER_BLOCK*2, ps)) != 0)
        {
            if (retstat == 1)
                status = 2; /* comm error */
            else if (retstat == 2)
                status = 4; /* checksum error */

            break; // timeout or error
        }

        blockct++;

        if (ps->totalct <= 19200)
        {
            if (blockct % 8 == 0)
            {
                dots += ".";
                FormS900->Memo1->Lines->Add(dots);
            }
        }
        else
        {
            if (blockct % 32 == 0)
            {
                dots += ".";
                FormS900->Memo1->Lines->Add(dots);
            }
        }

        count += WORDS_PER_BLOCK;
        writct = WORDS_PER_BLOCK*2;

        /* write only up to totalct words... */
        if (count > ps->totalct)
            writct -= (int)(count-ps->totalct) * 2;

        /* write to file */
        if (FileWrite(handle, rbuf, writct) < 0)
        {
            chandshake(ASD);
            status = 1;
        }
    }

    if (count < ps->totalct)
        status |= 8; /* wrong number of words received */

    return(status);

    /* 0=OK, 1=writerror, 2=com error, 4=bad chksm, 8=wrong # words */
}
//---------------------------------------------------------------------------
int __fastcall TFormS900::getdata1(short int * bufptr, int max_bytes, PSTOR * ps)
{
    int ii;
    unsigned char *cp;
    unsigned char checksum;
    short int baseline, temp;

    baseline = (short int)(1 << (ps->bits_per_sample-1));

    /* see if chars waiting from serial port... */
    if (receive(max_bytes+2))
        return(1);

    cp = TempArray+1;
    checksum = 0;

    for (ii = 0 ; ii < max_bytes/2 ; ii++)
    {
        temp = (short int)(*cp << 5);
        checksum ^= *cp++;
        temp |= (short int)(*cp >> 2);
        checksum ^= *cp++;
        temp -= baseline; /* convert to two's compliment */
        *bufptr++ = temp;
    }

    if (checksum != *cp)
        return(2); /* bad checksum */

    return(0);
/*
    0=OK, 1=receive error, 2=bad checksum
*/
}
//---------------------------------------------------------------------------
int __fastcall TFormS900::getdata2(unsigned char * bufptr, int max_bytes)
{
    int ii;
    unsigned char *cp, checksum;

    /* see if chars waiting from serial port... */
    if (receive(0))
        return(1);

    if (g_byteCount != max_bytes)
        return(2);

    cp = TempArray+7;
    checksum = 0;

    for (ii = 0 ; ii < max_bytes-9 ; ii++)
        checksum ^= *cp++;

    if (checksum != *cp++)
        return(3); /* bad checksum */

    return(0);
/*
    0=OK, 1=receive error, 2=wrong bytes, 3=bad checksum
*/
}
//---------------------------------------------------------------------------
void __fastcall TFormS900::decode_parmsDB(unsigned char * source, unsigned char * dest, int numchars)
{
    int ii;

    for (ii = 0 ; ii < numchars ; ii++)
    {
        *dest = *source++;
        *dest |= (unsigned char)(*source++ << 7);
        dest++;
    }

    if (numchars > 1)
        *dest = '\0';
}
//---------------------------------------------------------------------------
unsigned int __fastcall TFormS900::decode_parmsDD(unsigned char * tp)
{
    unsigned int acc;
    int ii;

    acc = 0;
    acc |= *tp | (*(tp+1) << 7);

    for (ii = 0 ; ii < 3 ; ii++)
    {
        acc <<= 8;
        tp -= 2;
        acc |= *tp | (*(tp+1) << 7);
    }

    return(acc);
}
//---------------------------------------------------------------------------
unsigned int __fastcall TFormS900::decode_parmsDW(unsigned char * tp)
{
    unsigned int acc;

    acc = 0;
    acc |= *tp | (*(tp+1) << 7);

    acc <<= 8;
    tp -= 2;
    acc |= *tp | (*(tp+1) << 7);

    return(acc);
}
//---------------------------------------------------------------------------
unsigned int __fastcall TFormS900::decode_hedrTB(unsigned char * tp)
{
    unsigned int acc;

    acc = (unsigned int)*tp--;
    acc = (acc << 7) | (unsigned int)*tp--;
    acc = (acc << 7) | (unsigned int)*tp;

    return(acc);
}
//---------------------------------------------------------------------------
void __fastcall TFormS900::cxmit(int samp, int mode)
{
    unsigned char midistr[6];

    midistr[0] = BEX;
    midistr[1] = COMMON_ID;
    midistr[2] = (unsigned char)mode;
    midistr[3] = (unsigned char)samp;
    midistr[4] = 0;
    midistr[5] = EEX;

    comws(6, midistr);
}
//---------------------------------------------------------------------------
void __fastcall TFormS900::chandshake(int mode)
{
    unsigned char midistr[4];

    midistr[0] = BEX;
    midistr[1] = COMMON_ID;
    midistr[2] = (unsigned char)mode;
    midistr[3] = EEX;

    comws(4, midistr);
}
//---------------------------------------------------------------------------
void __fastcall TFormS900::trim(char * trimstr, char * sourcestr)
{
    while (*sourcestr != '\0' && *sourcestr == ' ')
      sourcestr++;

    strcpy(trimstr, sourcestr);

    /* point to NUL */
    sourcestr = trimstr + strlen(trimstr);

    while (*(sourcestr-1) == ' ' && sourcestr != trimstr)
    {
      *(sourcestr-1) = '\0';
      sourcestr--;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormS900::catalog(bool print)
{
    // 7 hedr bytes + chksum byte + EEX = 9...
    if (g_byteCount < 9 || TempArray[3] != DCAT)
    {
        printerror("check cable, is sampler on and configured\r\n"
          "for RS232, 38400 baud?");
        return;
    }

    int entries = (g_byteCount-9)/sizeof(S900CAT);

    if (!entries)
    {
        printerror("No Samples or programs in S900");
        return;
    }

    S900CAT * tempptr = (S900CAT *)&TempArray[7]; // Skip header
    CAT * permsampptr = (CAT *)&PermSampArray[0];
    CAT * permprogptr = (CAT *)&PermProgArray[0];

    g_numSampEntries = 0;
    g_numProgEntries = 0;

    for (int ii = 0 ; ii < entries ; ii++)
    {
        if (tempptr->type == 'S')
        {
            sprintf(permsampptr->name, "%.*s", MAX_NAME_S900, tempptr->name);
            trim(permsampptr->name, permsampptr->name);

            permsampptr->sampidx = tempptr->sampidx;

            g_numSampEntries++; /* increment counter */
            permsampptr++; // next structure
        }
        else if (tempptr->type == 'P')
        {
            sprintf(permprogptr->name, "%.*s", MAX_NAME_S900, tempptr->name);
            trim(permprogptr->name, permprogptr->name);

            permprogptr->sampidx = tempptr->sampidx;

            g_numProgEntries++; /* increment counter */
            permprogptr++; // next structure
        }

        tempptr++; // next structure
    }


    if (print)
    {
        permsampptr = (CAT *)&PermSampArray[0];
        permprogptr = (CAT *)&PermProgArray[0];

        FormS900->Memo1->Lines->Add("Programs:");

        for (int ii = 0 ; ii < g_numProgEntries ; ii++)
        {
            FormS900->Memo1->Lines->Add(String(permprogptr->sampidx+1) + ':' + String(permprogptr->name));
            permprogptr++;
        }

        FormS900->Memo1->Lines->Add("Samples:");

        for (int ii = 0 ; ii < g_numSampEntries ; ii++)
        {
            FormS900->Memo1->Lines->Add(String(permsampptr->sampidx+1) + ':' + String(permsampptr->name));
            permsampptr++;
        }
    }
}
//---------------------------------------------------------------------------
int __fastcall TFormS900::get_ack(void)
{
    if (receive(0))
    {
        printerror("error receiving ack");
        return(1);
    }

    if (g_byteCount == ACKSIZ && TempArray[g_byteCount-2] == NAKS)
    {
        printerror("packet \"not-acknowledge\" (NAK) received");

        return(1);
    }

    if (g_byteCount != ACKSIZ || TempArray[g_byteCount-2] != ACKS)
    {
        printerror("bad ACK (acknowledge) packet");

        return(1);
    }

    return(0);
}
//---------------------------------------------------------------------------
int __fastcall TFormS900::receive(int count)
// set count to 0 to receive a complete message
// set to "count" to receive a partial message.
{
    unsigned char tempc;
    bool have_bex = false;

    g_byteCount = 0;

    int timer = 0;

    for(;;)
    {
        if(++timer >= ACKTIMEOUT)
            return(1); // timeout

        if (FormS900->ApdComPort1->CharReady())
        {
            tempc = FormS900->ApdComPort1->GetChar();

            if (!have_bex)
            {
                if (tempc == BEX || count != 0)
                {
                    have_bex = true;
                    TempArray[g_byteCount++] = tempc;
                }
            }
            else
            {
                TempArray[g_byteCount++] = tempc;

                if (count != 0 && g_byteCount == count)
                    return(0);

                if (tempc == EEX)
                    return(0);

                if (g_byteCount == TEMPCATBUFSIZ) // at buffer capacity... error
                    return(2);
            }
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormS900::encode_parameters(int samp, PSTOR* ps)
{

    char locstr[MAX_NAME_S900+1];
    char temp;

    samp_parms[0] = BEX;
    samp_parms[1] = AKAI_ID;
    samp_parms[3] = SPRM;
    samp_parms[4] = S900_ID;
    samp_parms[6] = 0;
    samp_parms[128] = EEX;

    encode_parmsDW(0, &samp_parms[55]); /* loudness offset */
    encode_parmsDW(0, &samp_parms[87]); /* reserved */
    temp = 0;
    encode_parmsDB((unsigned char *)&temp, (unsigned char *)&samp_parms[61], 1); /* reserved */
    encode_parmsDB((unsigned char *)&temp, (unsigned char *)&samp_parms[91], 1); /* type = norm */
    temp = 'N';
    encode_parmsDB((unsigned char *)&temp, (unsigned char *)&samp_parms[93], 1); /* waveform = 'N' */

    samp_parms[5] = (unsigned char)(samp);
    samp_hedr[3] = (unsigned char)(samp);
    samp_parms[2] = 0;

    /* encode excl,syscomid,sampdump */
    samp_hedr[0] = (unsigned char)0xf0;
    samp_hedr[1] = (unsigned char)0x7e;
    samp_hedr[2] = (unsigned char)0x01;

    samp_hedr[4] = 0; /* MSB samp idx always 0 for S900 */
    samp_hedr[5] = (unsigned char)ps->bits_per_sample;

    /* number of sample words */
    encode_parmsDD(ps->totalct, &samp_parms[39]);
    encode_hedrTB(ps->totalct, &samp_hedr[9]);

    /* start time */
    encode_parmsDD(ps->startidx, &samp_parms[71]);

    /* end time */
    encode_parmsDD(ps->endidx + 1, &samp_parms[63]);
    encode_hedrTB(ps->endidx + 1, &samp_hedr[15]);

    /* loop time */
    encode_parmsDD(ps->endidx-ps->loopidx, &samp_parms[79]);
    encode_hedrTB(ps->loopidx + 1, &samp_hedr[12]);

    /** temporary code!!! until Loop/Alt user switch added **/
    if (ps->loopidx >= ps->totalct - 5)
    {
        ps->looping = 'O';
        samp_hedr[18] = 0;
    }
    else
    {
        ps->looping = 'L';
        samp_hedr[18] = 1;
    }

    /* looping mode */
    encode_parmsDB((unsigned char *)&ps->looping, (unsigned char *)&samp_parms[59], 1);

    /* sampling frequency */
    encode_parmsDW(ps->freq, &samp_parms[47]);

    /* sampling period */
    encode_hedrTB((unsigned int)ps->period, &samp_hedr[6]);

    /* pitch */
    encode_parmsDW(ps->pitch,&samp_parms[51]);

    // truncate to fit S900
    ps->name[MAX_NAME_S900] = '\0';

    /* ASCII sample name */
    strcpy((char *)locstr, (char *)ps->name);

    int size = strlen(locstr);

    // Pad with blanks
    while (size < MAX_NAME_S900)
        locstr[size++] = ' ';

    encode_parmsDB((unsigned char *)locstr, (unsigned char *)&samp_parms[7], MAX_NAME_S900);

    compute_checksum();
}
//---------------------------------------------------------------------------
void __fastcall TFormS900::encode_parmsDB(unsigned char * source, unsigned char * dest, int numchars)
{
    int ii;

    for (ii = 0 ; ii < numchars ; ii++)
    {
        *dest++ = *source & (unsigned char)0x7f;

        if (*source & (unsigned char)0x80)
            *dest = 1;
        else
            *dest = 0;

        dest++;
        source++;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormS900::encode_parmsDD(unsigned int value, unsigned char * tp)
{
    int ii;
    unsigned int acc;

    acc = value;

    for (ii = 0 ; ii < 4 ; ii++)
    {
        *tp++ = (unsigned char)(acc & (unsigned int)0x7f);
        acc >>= 7;
        *tp++ = (unsigned char)(acc & 1);
        acc >>= 1;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormS900::encode_parmsDW(unsigned int value, unsigned char * tp)
{
    int ii;
    unsigned int acc;

    acc = value;

    for (ii = 0 ; ii < 2 ; ii++)
    {
      *tp++ = (unsigned char)(acc & (unsigned int)0x7f);
      acc >>= 7;
      *tp++ = (unsigned char)(acc & 1);
      acc >>= 1;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormS900::encode_hedrTB(unsigned int value, unsigned char * tp)
{
    unsigned int acc;

    /* encode sample header buffer */
    acc = value;

    *tp++ = (unsigned char)(acc & (unsigned int)0x7f);
    acc >>= 7;
    *tp++ = (unsigned char)(acc & (unsigned int)0x7f);
    acc >>= 7;
    *tp = (unsigned char)(acc & (unsigned int)0x7f);
}
//---------------------------------------------------------------------------
void __fastcall TFormS900::compute_checksum(void)
{
    unsigned char checksum;
    int ii;

    // checksum and store in transmission array
    checksum = 0;

    for (ii = 7 ; ii < 127 ; ii++)
        checksum ^= samp_parms[ii];

    samp_parms[ii] = checksum;
}
//---------------------------------------------------------------------------
int __fastcall TFormS900::findidx(int * samp, char * name)
// 0=samples present, match
// 16=error
// 32=no samples
// 64=samples present, but no match
{
    // Request S900 Catalog
    exmit(0, RCAT);

    if (receive(0))
    {
        printerror("error receiving catalog");
        return 16;
    }

    catalog(false); // populate sample and program structs (no printout)

    if (g_numSampEntries == 0)
    {
        *samp = 0;
        return(32); /* no samples... we are the only one */
    }

    CAT * catptr = (CAT *)PermSampArray;

    for (int ii = 0 ; ii < g_numSampEntries ; ii++)
    {
        if (StrCmpCaseInsens(catptr->name, name, MAX_NAME_S900))
        {
            *samp = catptr->sampidx;

            return 0; /* match! */
        }

        catptr++;
    }

    *samp = g_numSampEntries;

    return 64;
}
//---------------------------------------------------------------------------
void __fastcall TFormS900::exmit(int samp, int mode)
{

    unsigned char midistr[8];

    midistr[0] = BEX;
    midistr[1] = AKAI_ID;
    midistr[2] = 0;
    midistr[3] = (unsigned char)mode;
    midistr[4] = S900_ID;
    midistr[5] = (unsigned char)samp;
    midistr[6] = 0;
    midistr[7] = EEX;

    comws(8, midistr);
}
//---------------------------------------------------------------------------
void __fastcall TFormS900::comws(int count, unsigned char * ptr)
{
    for (int ii = 0 ; ii < count ; ii++)
    {
          while (FormS900->ApdComPort1->OutBuffFree < 1);
          FormS900->ApdComPort1->PutChar(*ptr++);
    }
}
//---------------------------------------------------------------------------
unsigned char __fastcall TFormS900::send_data(short int * intptr, PSTOR * ps)
{
    short int val, baseline;
    unsigned char temp, checksum;

    baseline = (short int)(1 << (ps->bits_per_sample-1));

    checksum = 0;

    for (int ii = 0 ; ii < WORDS_PER_BLOCK ; ii++)
    {
        /* past totalct, read routine pumps out junk until */
        /* required 60 sample words have been transmitted. */

        val = *intptr++;
        val += baseline; /* convert out of two's complement format */
        temp = (unsigned char)(((unsigned short)val >> 5) & 0x7f);
        checksum ^= temp;

        comws(1, &temp);

        temp = (unsigned char)(((unsigned short)val << 2) & 0x7c);
        checksum ^= temp;

        comws(1, &temp);
    }

    return(checksum);
}
//---------------------------------------------------------------------------
unsigned char __fastcall TFormS900:: wav_send_data(unsigned short * intptr, PSTOR * ps)
// S900 is a 12-bit sampler but can receive 14-bit samples
{
    unsigned short val;
    unsigned char temp, checksum;

    checksum = 0;

    // We expect a buffer of 14-bit, right-justified, UNSIGNED
    // values in a 16-bit, unsigned short format!!!

    for (int ii = 0 ; ii < WORDS_PER_BLOCK ; ii++)
    {
        // past totalct, read routine pumps out junk until
        // required 60 sample words have been transmitted.

        val = *intptr++;

        // byte 1 = 0 d13 d12 d11 d10 d9 d8 d7
        temp = (unsigned char)((val >> 7) & 0x7f); // bits 7-13

        checksum ^= temp;
        comws(1, &temp);

        // byte 2 = 0 d6 d5 d4 d3 d2 d1 d0
        temp = (unsigned char)(val & 0x7f); // bits 0-6

        checksum ^= temp;
        comws(1, &temp);
    }

    return(checksum);
}
//---------------------------------------------------------------------------
void __fastcall TFormS900::send_samp_parms(unsigned int index)
{
    compute_checksum();

    /* transmit sample parameters */
    samp_parms[5] = (unsigned char)(index);
    comws(PARMSIZ, samp_parms);
}
//---------------------------------------------------------------------------
void __fastcall TFormS900::printerror(String message)
{
    FormS900->Memo1->Lines->Add(message);
}
//---------------------------------------------------------------------------
void __fastcall TFormS900::print_ps_info(PSTOR * ps)
{
    FormS900->Memo1->Lines->Add("");
    FormS900->Memo1->Lines->Add("Output Parameters:");
    FormS900->Memo1->Lines->Add("start index: " + String(ps->startidx));
    FormS900->Memo1->Lines->Add("end index: " + String(ps->endidx));
    FormS900->Memo1->Lines->Add("loop index: " + String(ps->loopidx));
    FormS900->Memo1->Lines->Add("frequency: " + String(ps->freq));
    FormS900->Memo1->Lines->Add("pitch: " + String(ps->pitch));
    FormS900->Memo1->Lines->Add("size(in words): " + String(ps->totalct));
    FormS900->Memo1->Lines->Add("period: " + String((unsigned int)ps->period));
    FormS900->Memo1->Lines->Add("bits per word: " + String(ps->bits_per_sample));
    FormS900->Memo1->Lines->Add("name: " + String(ps->name));
    FormS900->Memo1->Lines->Add("looping mode: \"" + String((char)ps->looping) + "\""); // prints "O" or "L"
}
//---------------------------------------------------------------------------
void __fastcall TFormS900::Put1Click(TObject *Sender)
{
  PutFile(GetFileName());
}
//---------------------------------------------------------------------------
void __fastcall TFormS900::PutFile(String sFilePath)
{
    ListBox1->Clear();

    unsigned char tbuf[WORDS_PER_BLOCK*2]; // 120 bytes
    unsigned char *pszBuffer = NULL;

    int iFileHandle = 0;
    int iFileLength;
    int iBytesRead;
    int retval;
    PSTOR ps;
    int index;
    int blockct;
    char locstr[3];
    char newname[11];
    char fileext[5];

    try
    {
        Memo1->Clear();
        Memo1->Repaint();

        if (sFilePath.IsEmpty() || !FileExists(sFilePath))
        {
          Memo1->Lines->Add("file does not exist!");
          return;
        }

        // Load file
        Memo1->Lines->Add("file path: \"" + sFilePath + "\"");

        iFileHandle = FileOpen(sFilePath, fmOpenRead);

        if (iFileHandle == 0)
        {
            Memo1->Lines->Add("unable to open file, handle is 0!");
            return;
        }

        iFileLength = FileSeek(iFileHandle,0,2);

        if (iFileLength == 0)
        {
            FileClose(iFileHandle);
            Memo1->Lines->Add("unable to open file, length is 0!");
            return;
        }

        pszBuffer = new unsigned char[iFileLength+1];

        if (pszBuffer == NULL)
        {
            FileClose(iFileHandle);
            Memo1->Lines->Add("unable to allocate " + String(iFileLength+1) + " bytes of memory!");
            return;
        }

        //Memo1->Lines->Add("allocated " + String(iFileLength+1) + " bytes...");

        try
        {
            FileSeek(iFileHandle,0,0); // back to start

            try
            {
                iBytesRead = FileRead(iFileHandle, pszBuffer, iFileLength);
                //Memo1->Lines->Add("read " + String(iBytesRead) + " bytes...");
            }
            catch(...)
            {
                Memo1->Lines->Add("unable to read file into buffer...");
                return;
            }

            FileClose(iFileHandle);
            iFileHandle = 0;

            strncpy(newname, ExtractFileName(sFilePath).c_str(), 10);

            while (strlen(newname) < 10)
                newname[strlen(newname)] = ' ';

            newname[11] = '\0';

            bool exton = false;

            for (int ii = 0 ; ii < (int)strlen(newname) ; ii++)
            {
                if (newname[ii] == '.')
                    exton = true;

                if (!isalnum(newname[ii]) || exton)
                    newname[ii] = ' ';
            }

            strncpy(fileext, ExtractFileExt(sFilePath).c_str(), 4);

            // set file extension (if any) lowercase
            for (int ii = 0 ; ii < 4 ; ii++)
                fileext[ii] = (char)tolower(fileext[ii]);

        //    Memo1->Lines->Add("ext: " + String(fileext));

            if (!StrCmpCaseInsens(".aki", fileext, 4)) // Not an AKI file? (try WAV...)
            {
                if (iBytesRead < 45)
                {
                    Memo1->Lines->Add("bad file (1)");
                    return;
                }

                if (!StrCmpCaseInsens((char*)&pszBuffer[0], "RIFF", 4))
                {
                    Memo1->Lines->Add("bad file (2) [no \'RIFF\' preamble!]");
                    return;
                }

                int file_size = *(__int32 *)&pszBuffer[4];
                if (file_size+8 != iBytesRead)
                {
                    Memo1->Lines->Add("bad file (3), (file_size = " +
                     String(file_size+8) + ", iBytesRead = " +
                        String(iBytesRead) + ")!");
                    return;
                }

                if (!StrCmpCaseInsens((char*)&pszBuffer[8], "WAVE", 4))
                {
                    Memo1->Lines->Add("bad file (4) [no \'WAVE\' preamble!]");
                    return;
                }

                // Search file for "fmt " block
                unsigned char* headerPtr = pszBuffer;
                // NOTE: the FindSubsection will return headerPtr by reference!
                __int32 headerSize = FindSubsection(headerPtr, "fmt ", iBytesRead);
                if (headerSize < 0)
                {
                    Memo1->Lines->Add("bad file (4) [no \'fmt \' sub-section!]");
                    return;
                }

                // Length of the format data in bytes is a four-byte int at
                // offset 16. It should be at least 16 bytes of sample-info...
                if (headerSize < 16)
                {
                    Memo1->Lines->Add("bad file (6) [\'fmt \' sub-section is < 16 bytes!]");
                    return;
                }

                // Search file for "data" block
                // (Left-channel data typically begins after the header
                // at 12 + 4 + 4 + 16 + 4 + 4 - BUT - could be anywhere...)
                unsigned char* dataPtr = pszBuffer;
                // NOTE: the FindSubsection will return dataPtr by reference!
                __int32 dataLength = FindSubsection(dataPtr, "data", iBytesRead);
                if (dataLength < 0)
                {
                    Memo1->Lines->Add("bad file (4) [no \'data\' sub-section!]");
                    return;
                }

                // NOTE: Metadata tags will appear at the end of the file,
                // after the data. You will see "LIST". I've also seen
                // "JUNK" and "smpl", etc.

                // Bytes of data following "data"
                //**** Data must end on an even byte boundary!!! (explains
                // why the file may appear 1 byte over-size for 8-bit mono)
                Memo1->Lines->Add("data-section length (in bytes): " + String(dataLength));

                int AudioFormat = *(__int16*)(&headerPtr[0]);
                if (AudioFormat != 1) // we can't handle compressed WAV-files
                {
                    Memo1->Lines->Add("cannot read this WAV file-type " +
                                      String(AudioFormat) + "!");
                    return;
                }

                int NumChannels = *(__int16*)(&headerPtr[2]);
                Memo1->Lines->Add("number of channels: " + String(NumChannels));

                int SampleRate = *(__int32*)(&headerPtr[4]);
                Memo1->Lines->Add("sample rate: " + String(SampleRate));

                // BytesPerFrame: Number of bytes per frame
                int BytesPerFrame = *(__int16*)(&headerPtr[12]);
                Memo1->Lines->Add("bytes per frame: " + String(BytesPerFrame));

                int BitsPerSample = *(__int16*)(&headerPtr[14]);
                Memo1->Lines->Add("bits per sample: " + String(BitsPerSample));
                if (BitsPerSample < 8 || BitsPerSample > 64)
                {
                    Memo1->Lines->Add("bits per sample out of range: " +
                                         String(BitsPerSample));
                    return;
                }

                int BytesPerSample = BitsPerSample/8;
                if (BitsPerSample % 8) // remaining bits?
                    BytesPerSample++; // round up

                if (BytesPerSample > 8)
                {
                    Memo1->Lines->Add("error: can't handle samples over 64-bits!");
                    return;
                }

                if (NumChannels * BytesPerSample != BytesPerFrame)
                {
                    Memo1->Lines->Add("error: (NumChannels * BytesPerSample != BytesPerFrame)");
                    return;
                }

                // there should be no "remainder" bytes...
                if (dataLength % (NumChannels * BytesPerSample))
                {
                    Memo1->Lines->Add("error: incomplete data-block!");
                    return;
                }

                Memo1->Lines->Add("bytes per sample: " + String(BytesPerSample));

                int TotalFrames = dataLength / (NumChannels * BytesPerSample);
                Memo1->Lines->Add("number of frames: " + String(TotalFrames));

                Memo1->Lines->Add("sample name: " + String(newname));

                // make sure we have a file-length that accomodates the expected data-length!
                if (dataPtr+dataLength > pszBuffer+iBytesRead)
                {
                    Memo1->Lines->Add("error: [dataPtr+dataLength > pszBuffer+iBytesRead!]");
                    return;
                }

                // Need to populate a PSTOR structure for S900
                ps.startidx = 0; /* start of play index (4 bytes) */
                ps.endidx = (unsigned int)TotalFrames-1; /* end of play index (4 bytes) */
                ps.loopidx = ps.endidx; /* end of play index (4 bytes) */

                int tempfreq = SampleRate;
                if (tempfreq > 49999)
                    tempfreq = 49999;
                ps.freq = (unsigned int)tempfreq; /* sample freq. in Hz (4 bytes) */

                ps.pitch = 960; /* pitch - units = 1/16 semitone (4 bytes) (middle C) */
                ps.totalct = (unsigned int)TotalFrames; /* total number of words in sample (4 bytes) */
                ps.period = 1.0e9/(double)ps.freq; /* sample period in nanoseconds (8 bytes) */

                ps.bits_per_sample = S900_BITS_PER_SAMPLE; /* (2 bytes, 14-bits) */

                strcpy(ps.name, newname); /* ASCII sample name (17 bytes) */
                ps.looping = 'O'; /* (1 byte) */

                // now we need to set "samp index"
                retval = findidx((int *)&index, ps.name);

                // 0=samples present, one matched
                // 16=error
                // 32=no samples
                // 64=samples present, but no match
                if (retval == 16)
                    return; // catalog error

                if (retval == 0)
                    Memo1->Lines->Add("sample exists on S900, index: " +
                                                   String(index+1));

                // encode samp_hedr and samp_parms arrays
                encode_parameters(index, &ps);

                print_ps_info(&ps);

                // request common reception enable
                exmit(0, SECRE);

                // transmit sample header info
                comws(HEDRSIZ, samp_hedr);

                // wait for acknowledge
                if (get_ack())
                  return;

                int FrameCounter = 0; // We already processed the header

                blockct = 0;

                String dots = ".";
                Memo1->Lines->Add(dots); // progress display

                // if Stereo and User checked "send right channel"
                // point to first right-channel sample-datum
                if (NumChannels > 1 && MenuSendRightChan->Checked)
                  dataPtr += BytesPerSample;

        //        NS9dither16 * d = new NS9dither16();

                // for each frame in WAV-file (one-sample of 1-8 bytes...)
                for(;;)
                {
                  // End of file?
                  if (FrameCounter >= TotalFrames)
                      break;

                  // read and encode block of 60 frames...

                  // Strategy: encode WAV samples of 8-64 bits
                  // as unsigned 14-bit (two-byte S900 "SW" format)
                  // S900 is 12-bits but can receive 14-bits.
                  // 8-bit wav-format is already un-signed, but
                  // over 8-bits we need to convert from signed to
                  // unsigned.  All values need to be converted into
                  // a 14-bit form...

                  // We generate a buffer (tbuf) of 14-bit,
                  // right-justified, UNSIGNED
                  // values in a 16-bit, unsigned short format!!!

                  for (int ii = 0 ; ii < WORDS_PER_BLOCK ; ii++)
                  {
                    if (FrameCounter >= TotalFrames)
                    {
                      tbuf[ii*2] = 0;
                      tbuf[(ii*2)+1] = 0;
                    }
                    else
                    {
                      unsigned long acc;

                      // one-byte wave samples are un-signed by default
                      if (BytesPerSample < 2) // one byte?
                      {
                        // convert byte to 14-bits and save in tbuf
                        // (left-channel sample)
                        acc = *dataPtr; // don't add ii here!
                        acc <<= 6; // 14-bit sample-conversion
                      }
                      else // between 2-7 bytes per sample (signed)
                      {
                        // Allowed BitsPerSample => 9-56
                        // Stored as MSB then LSB in "dp" buffer.
                        // Left-justified...
                        //
                        // From the WAV file:
                        //
                        // example (16-bits):
                        // byte *dp    : D15 D14 D13 D12 D11 D10 D09 D08
                        // byte *(dp+1): D07 D06 D05 D04 D03 D02 D01 D00
                        //
                        // example (9-bits):
                        // byte *dp    : D08 D07 D06 D05 D04 D03 D02 D01
                        // byte *(dp+1): D00  0   0   0   0   0   0   0

                        // to convert from two's compliment to offset-binary:
                        // take N + (1<<(BitsPerSample-1)), mask with
                        // ((1<<BitsPerSample)-1). store in signed long integer.

                        // load accumulator with left-justified sample
                        // (Microsoft WAV sample-data are in Intel
                        // littl-endian byte-order!)
                        // (left-channel sample is 1st if a stereo-WAV-file)

                        acc = 0;

                        // load accumulator with sample 8-64 bits
                        for (int ii = 0 ; ii < BytesPerSample ; ii++)
                          acc |= *(dataPtr+ii) << (8*ii);

                        acc >>= (8*BytesPerSample)-BitsPerSample; // right-justify so we can add

                        // convert from 2's compliment to offset-binary
                        acc += 1 << (BitsPerSample-1);

                        // convert to 14-bits
                        if (BitsPerSample > 14)
                          acc >>= BitsPerSample-14;
                        else if (BitsPerSample < 14)
                          acc <<= 14-BitsPerSample;
                      }

                      // save converted sample in tbuf
                      // a short int in memory is ordered LSB first, then MSB!
                      tbuf[ii*2] = (unsigned char)acc; // LSB
                      tbuf[(ii*2)+1] = (char)((unsigned short)acc >> 8); // MSB
                    }

                    dataPtr += BytesPerFrame; // Next frame
                    FrameCounter++;
                  }

                  // Print progress-display
                  blockct++;

                  if (ps.totalct <= 19200)
                  {
                    if (blockct % 8 == 0)
                    {
                      dots += ".";
                      FormS900->Memo1->Lines->Add(dots);
                    }
                  }
                  else
                  {
                    if (blockct % 32 == 0)
                    {
                      dots += ".";
                      FormS900->Memo1->Lines->Add(dots);
                    }
                  }

                  unsigned char temp = (unsigned char)(blockct & 0x7f);
                  comws(1, &temp);

                  // Send data and checksum
                  temp = wav_send_data((unsigned short *)tbuf, &ps);
                  comws(1, &temp);

                  // wait for acknowledge
                  if (get_ack())
                  {
                    FormS900->Memo1->Lines->Add("no acknowledge packet received! (block = " + String(blockct) + ")");
                    break;
                  }
                }
            }
            else // AKI file (my custom format)
            {
                if (iBytesRead < (WORDS_PER_BLOCK*2) + PSTOR_SIZ + MAGIC_SIZ)
                {
                  printerror("file is corrupt");
                  return;
                }

                Memo1->Lines->Add("Read " + String(iBytesRead) + " bytes");

                unsigned int my_magic;

                memcpy(&my_magic, &pszBuffer[0], sizeof(my_magic));
        //    Memo1->Lines->Add("magic = " + String(my_magic));

                if (my_magic != MAGIC_NUM)
                {
                  printerror("File is not the right kind!");
                  return;
                }

                memcpy(&ps, &pszBuffer[0+MAGIC_SIZ], PSTOR_SIZ);

                trim(ps.name, ps.name);

                // encode new sample name if any entered
                //    char name[11] = "TEMPNAME00";
                //    sprintf(ps.name, "%.*s", MAX_NAME_S900, name);

                // now we need to set "samp index"
                retval = findidx((int *)&index, ps.name);

                // 0=samples present, one matched
                // 16=error
                // 32=no samples
                // 64=samples present, but no match
                if (retval == 16)
                  return; // catalog error

                if (retval == 0)
                  Memo1->Lines->Add("sample exists on S900, index: " +
                                             String(index+1));

                // encode samp_hedr and samp_parms arrays
                encode_parameters(index, &ps);

                print_ps_info(&ps);

                // request common reception enable
                exmit(0, SECRE);

                // transmit sample header info
                comws(HEDRSIZ, samp_hedr);

                // wait for acknowledge
                if (get_ack())
                  return;

                unsigned char * ptr = &pszBuffer[PSTOR_SIZ + MAGIC_SIZ];
                int ReadCounter = PSTOR_SIZ + MAGIC_SIZ; // We already processed the header

                blockct = 0;

                String dots = ".";
                Memo1->Lines->Add(dots);

                for(;;)
                {
                  // End of file?
                  if (ReadCounter >= iBytesRead)
                      break;

                  // read block of 60 words from buffer... */
                  for (int ii = 0 ; ii < WORDS_PER_BLOCK*2 ; ii++)
                  {
                    if (ReadCounter >= iBytesRead)
                      tbuf[ii] = 0; // pad last block if end of file
                    else
                      tbuf[ii] = *ptr++;

                    ReadCounter++;
                  }

                  blockct++;

                  if (ps.totalct <= 19200)
                  {
                    if (blockct % 8 == 0)
                    {
                      dots += ".";
                      FormS900->Memo1->Lines->Add(dots);
                    }
                  }
                  else
                  {
                    if (blockct % 32 == 0)
                    {
                      dots += ".";
                      FormS900->Memo1->Lines->Add(dots);
                    }
                  }

                  unsigned char temp = (unsigned char)(blockct & 0x7f);
                  comws(1, &temp);

                  // Send data and checksum
                  temp = send_data((short int *)tbuf, &ps);
                  comws(1, &temp);

                  // wait for acknowledge
                  if (get_ack())
                  {
                    FormS900->Memo1->Lines->Add("no acknowledge packet received! (block = " + String(blockct) + ")");
                    break;
                  }
                }
            }

            unsigned char temp = EEX;
            comws(1, &temp);

            exmit(0, SECRD); /* request common reception disable */

            sprintf(locstr, "%02d", index);

            retval = findidx((int *)&index, locstr);

            // 0=samples present, one matched
            // 16=error
            // 32=no samples
            // 64=samples present, but no match
            if (retval == 16)
            {
                Memo1->Lines->Add("Error looking up new sample in catalog...");
                return; // catalog error
            }

            if (retval == 0)
                Memo1->Lines->Add("sample written, index: " + String(index+1));
            else
            {
                Memo1->Lines->Add("index string not found, index: " + String(index+1));
                return;
            }

            send_samp_parms(index);
        }
        __finally
        {
          if (pszBuffer != NULL)
              delete [] pszBuffer;

          if (iFileHandle != 0)
              FileClose(iFileHandle);
        }
    }
    catch(...)
    {
        ShowMessage("Can't load file: \"" + sFilePath + "\"");
    }
}
//---------------------------------------------------------------------------
String __fastcall TFormS900::GetFileName(void)
{
    OpenDialog1->Title = "Open .WAV or .AKI File, Send To Akai";
    OpenDialog1->DefaultExt = "aki";
    OpenDialog1->Filter = "Aki files (*.aki)|*.aki|" "Wav files (*.wav)|*.wav|"
                 "All files (*.*)|*.*";
    OpenDialog1->FilterIndex = 3; // start the dialog showing all files
    OpenDialog1->Options.Clear();
    OpenDialog1->Options << ofHideReadOnly
        << ofPathMustExist << ofFileMustExist << ofEnableSizing;

    if (!OpenDialog1->Execute())
      return ""; // Cancel

    String sFileName = OpenDialog1->FileName;

    return sFileName;
}
//---------------------------------------------------------------------------
// Search file for named chunk
//
// Returns:
// -2 = not found
// -1 = exception thrown
// 0-N length of chunk
//
// Returns a byte-pointer to the start of data in the chunk
// as a reference: fileBuffer
//
// On entry, set fileBuffer to the start of the file-buffer
__int32 __fastcall TFormS900::FindSubsection(unsigned char* &fileBuffer, char* chunkName, UINT fileLength)
{
  try
  {
    // bypass the first 12-bytes "RIFFnnnnWAVE" at the file's beginning...
    unsigned char* chunkPtr = fileBuffer+12;

    unsigned char* pMax = fileBuffer+fileLength;

    int chunkLength;

    // Search file for named chunk
    for(;;)
    {
      // Chunks are 4-bytes ANSI followed by a 4-byte length, lsb-to-msb
      if (chunkPtr+8 >= pMax)
        return -2;

      chunkLength = *(__int32*)(chunkPtr+4);

      // look for the 4-byte chunk-name
      if (StrCmpCaseInsens((char*)chunkPtr, (char*)chunkName, 4))
      {
        fileBuffer = chunkPtr+8; // return pointer to data, by reference
        return chunkLength;
      }

      chunkPtr += chunkLength+8;
    }
  }
  catch(...)
  {
    return -1;
  }
}
//---------------------------------------------------------------------------
// returns TRUE if strings match - insensitive to case
bool __fastcall TFormS900::StrCmpCaseInsens(char* sA, char* sB, int len)
{
  return String(sA, len).LowerCase() == String(sB, len).LowerCase();
}
//---------------------------------------------------------------------------
void __fastcall TFormS900::Cat1Click(TObject *Sender)
{
    ListBox1->Clear();

    // Request S900 Catalog
    exmit(0, RCAT);

    if (receive(0))
    {
        printerror("error receiving catalog");
        return;
    }

    catalog(true);
}
//---------------------------------------------------------------------------
void __fastcall TFormS900::Get1Click(TObject *Sender)
{
    try
    {
        ListBox1->Clear();

        // Request S900 Catalog
        exmit(0, RCAT);

        if (receive(0))
        {
            printerror("error receiving catalog");
            return;
        }

        catalog(false);

        if (!g_numSampEntries)
        {
            Memo1->Lines->Add("No samples in S900/S950!");
            return;
        }

        CAT *catp = (CAT *)PermSampArray;

        for (int ii = 0 ; ii < g_numSampEntries ; ii++, catp++)
            ListBox1->Items->Add(catp->name);

        Memo1->Lines->Add("\r\n***Click a sample at the right to receive\r\n"
                           "and save it to a .AKI file*** --->");
    }
    catch(...)
    {
        ShowMessage("Can't get catalog");
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormS900::ListBox1Click(TObject *Sender)
{
    try
    {
        SaveDialog1->Title = "Receive From Akai, Save .AKI File";
        SaveDialog1->DefaultExt = "aki";
        SaveDialog1->Filter = "Akai files (*.aki)|*.aki|"
               "All files (*.*)|*.*";
        SaveDialog1->FilterIndex = 2; // start the dialog showing all files
        SaveDialog1->Options.Clear();
        SaveDialog1->Options << ofHideReadOnly
         << ofPathMustExist << ofOverwritePrompt << ofEnableSizing
            << ofNoReadOnlyReturn;

        SaveDialog1->FileName = ExtractFilePath(SaveDialog1->FileName) +
                                    ListBox1->Items->Strings[ListBox1->ItemIndex];

        SaveDialog1->FileName.TrimRight();
        SaveDialog1->FileName.TrimLeft();

        if (SaveDialog1->Execute())
        {
            ListBox1->Repaint();

            if (get_sample(ListBox1->ItemIndex, SaveDialog1->FileName) != 0)
                Memo1->Lines->Add("Not able to save sample!");
            else
                Memo1->Lines->Add("Sample saved as: \"" + SaveDialog1->FileName + "\"");
        }
    }
    catch(...)
    {
        ShowMessage("Can't save file: \"" + SaveDialog1->FileName + "\"");
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormS900::Help1Click(TObject *Sender)
{
    Memo1->Lines->Clear();
    Memo1->Lines->Add("Connect a cable between the S900's (or S950) RS232\r\n"
      "PORT and COM1 (or other port) on your computer. This is an ordinary\r\n"
      "DB25-male to DB9-female null-modem cable.\r\n\r\n"
      "On the S900/S950 push the MIDI button.\r\n"
      "Push the DOWN button and scroll to menu 5.\r\n"
      "Push the RIGHT button and select \"2\" control by RS232.\r\n"
      "Push the RIGHT button again and enter \"3840\".\r\n"
      "This should set the S900 to 38400 baud.\r\n\r\n"
      "To test, pick the menu's CAT entry... you should get a box\r\n"
      " asking for the com port -- here if the message says there\r\n"
      " is an error... pay no attention - COM1 works fine for me.\r\n\r\n"
      "Click OK, you should see the samples and programs listed in this\r\n"
      "window. - Cheers, Scott Swift. dxzl@live.com");
}
//---------------------------------------------------------------------------
void __fastcall TFormS900::MenuUseRS232Click(TObject *Sender)
{
//  MenuUseRS232->Checked = !MenuUseRS232->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TFormS900::MenuSendRightChanClick(TObject *Sender)
{
  MenuSendRightChan->Checked = !MenuSendRightChan->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TFormS900::WMDropFile(TWMDropFiles &Msg)
{
  try
  {
    //get dropped files count
    int cnt = ::DragQueryFileW((HDROP)Msg.Drop, -1, NULL, 0);

    if (cnt != 1)
      return; // only one file!

    wchar_t wBuf[MAX_PATH];

    // Get first file-name
    if (::DragQueryFileW((HDROP)Msg.Drop, 0, wBuf, MAX_PATH) > 0)
    {
      // Load and convert file as per the file-type (either plain or rich text)
      WideString wFile(wBuf);

      if (!wFile.IsEmpty())
      {
          String sFile = String(wFile);
          if (FileExists(sFile))
          {
            g_DragDropFilePath = sFile;
            Timer1->Interval = 50;
            Timer1->Enabled = true; // fire event to send file
          }
      }
    }
  }
  catch(...){}
}
//---------------------------------------------------------------------------
void __fastcall TFormS900::Timer1Timer(TObject *Sender)
{
  Timer1->Enabled = false;
  PutFile(g_DragDropFilePath);
  g_DragDropFilePath = "";
}
//---------------------------------------------------------------------------

