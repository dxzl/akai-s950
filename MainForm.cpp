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
TForm1 *Form1;

#define TEMPCATBUFSIZ   (sizeof(S900CAT)*MAX_SAMPS_S900*2 + 9)

typedef struct
  {
  char name[MAX_NAME_S900+1];
  int sampidx;
  int select; /* user select flag */
  } CAT;

typedef struct
  {
  char type; /* ASCII: "P" = program, "S" = sample */
  char sampidx; /* program/sample number (0-31) */
  char name[MAX_NAME_S900]; /* program/sample name */
  } S900CAT;

// Note: sizeof(PSTOR) is 72, but the original
// program stored the header as 68 bytes

typedef struct
  {
  /* parameter storage, file storage header */
  unsigned int startidx; /* start of play index (4 bytes) */
  unsigned int endidx; /* end of play index (4 bytes) */
  unsigned int loopidx; /* end of play index (4 bytes) */
  unsigned int freq; /* sample freq. in Hz (4 bytes) */
  unsigned int pitch; /* pitch - units = 1/16 semitone (4 bytes) */
  unsigned int totalct; /* total number of words in sample (4 bytes) */
  double period; /* sample period in nanoseconds (8 bytes) */
  short unsigned int bits_per_sample; /* (2 bytes) */

  char name[MAX_SAMP_NAME+1]; /* ASCII sample name (17 bytes) */
  char spares[16]; /* unused bytes (16 bytes) */
  unsigned char looping; /* (1 byte) */
  } PSTOR;

// Local vars
int ByteCount = 0;
int SampEntries = 0;
int ProgEntries = 0;
unsigned char TempArray[TEMPCATBUFSIZ];
unsigned char PermSampArray[sizeof(CAT)*MAX_SAMPS_S900];
unsigned char PermProgArray[sizeof(CAT)*MAX_SAMPS_S900];

/* global arrays */
unsigned char samp_parms[PARMSIZ];
unsigned char samp_hedr[HEDRSIZ];

// function prototypes
/*************************************************************************/
void encode_parameters( int samp, PSTOR * ps );
void encode_parmsDB( unsigned char * source, unsigned char * dest, int numchars );
void encode_parmsDD( unsigned int value, unsigned char * tp );
void encode_parmsDW( unsigned int value, unsigned char * tp );
void encode_hedrTB( unsigned int value, unsigned char * tp );
void compute_checksum( void );
int findidx( int * samp, char * name );
void exmit( int samp, int mode );
void comws( int count, unsigned char * ptr );
unsigned char send_data( short int * intptr, PSTOR * ps );
unsigned char wav_send_data( unsigned short * intptr, PSTOR * ps );
void send_samp_parms( unsigned int index );
void printerror( String message );
void print_ps_info( PSTOR * ps );
int receive( int count );
void catalog( bool print );
int GetAck( void );
void trim( char * trimstr, char * sourcestr);

int get_sample( int samp, String Name );
int get_samp_hedr( int samp );
void decode_parameters( PSTOR * ps );
int get_samp_data( PSTOR * ps, int handle );
int getdata1( short int * bufptr, int max_bytes, PSTOR * ps );
int getdata2( unsigned char * bufptr, int max_bytes );
void decode_parmsDB( unsigned char * source, unsigned char * dest, int numchars );
unsigned int decode_parmsDD( unsigned char * tp );
unsigned int decode_parmsDW( unsigned char * tp );
unsigned int decode_hedrTB( unsigned char * tp );
void chandshake( int mode );
void cxmit( int samp, int mode );

/*************************************************************************/

// Get sample routines

/*************************************************************************/

int get_sample( int samp, String Name )
{

    unsigned int my_magic;
    PSTOR ps;
    int iFileHandle;

	if ( !FileExists( Name ) )
    	iFileHandle = FileCreate( Name );
    else
        {
        iFileHandle = FileOpen( Name, fmOpenReadWrite );
        FileSeek( iFileHandle, 0, 0 ); // Back to beginning of the file
        }

    my_magic = MAGIC_NUM;

    exmit( 0, SECRE ); /* request common reception enable */

    /* first interrogate S900 to get sample parameters... */

    /* request sample parms */
    exmit( samp, RSPRM );

    if ( getdata2( samp_parms, sizeof( samp_parms ) ) )
        {
        exmit( 0, SECRD ); /* request common reception disable */
        FileClose( iFileHandle );
        return( 1 ); /* could not get header data */
        }

    // copy into samp_parms
    memcpy( samp_parms, TempArray, PARMSIZ );

    if ( get_samp_hedr( samp ) )
        {
        exmit( 0, SECRD ); /* request common reception disable */
        FileClose( iFileHandle );
        return( 2 ); /* could not get header data */
        }

    // copy into samp_hedr
    memcpy( samp_hedr, TempArray, HEDRSIZ );

    decode_parameters( &ps ); /* fill new sample's PSTOR struct... */

    print_ps_info( &ps );

    /* write the magic number and header... */
	FileWrite( iFileHandle, (char *)&my_magic, sizeof(my_magic) );
	FileWrite( iFileHandle, (char *)&ps, sizeof(PSTOR) );

    if ( get_samp_data( &ps, iFileHandle ) != 0 )
        {
        exmit(0,SECRD); /* request common reception disable */
        FileClose( iFileHandle );
        return( 3 ); /* could not get sample data */
        }

    exmit(0,SECRD); /* request common reception disable */

    FileClose( iFileHandle );

    return( 0 );

/*
1=no parms, 2=no header, 3=no samp data
*/
}


int get_samp_hedr( int samp )
{

    cxmit( samp, RSD ); /* request sample dump */

    /* see if chars waiting from serial port... */
    if ( receive( HEDRSIZ ) )
        {
        printerror( "error receiving data" );
        return( 1 );
        }

    return( 0 ); /* 0=OK, 1=com error */
}


void decode_parameters( PSTOR * ps )
{
    /* number of sample words */
    ps->totalct = decode_hedrTB( (unsigned char *)&samp_hedr[9+2] );

    /* start time */
    ps->startidx = decode_parmsDD( (unsigned char *)&samp_parms[71+(7-1)] );

    /* end time */
    ps->endidx = decode_hedrTB( (unsigned char *)&samp_hedr[15+2] ) - 1;

    /* loop time */
    ps->loopidx = decode_hedrTB( (unsigned char *)&samp_hedr[12+2] ) - 1;

    /* sampling frequency */
    ps->freq = decode_parmsDW( (unsigned char *)&samp_parms[47+(3-1)] );

    /* sampling period */
    /* for S900, we'll calculate our own value for period */
    ps->period = 1.0e9/(double)ps->freq;

    /* pitch */
    ps->pitch = decode_parmsDW( (unsigned char *)&samp_parms[51+(3-1)] );

    /* ASCII sample name */
    decode_parmsDB( (unsigned char *)&samp_parms[7], (unsigned char *)ps->name, MAX_NAME_S900 );
    trim( ps->name, ps->name ); /* trim off the blanks */

    /* looping mode */
    decode_parmsDB( (unsigned char *)&samp_parms[59], &ps->looping, 1 );

    ps->bits_per_sample = (short unsigned int)samp_hedr[5];
}


int get_samp_data( PSTOR * ps, int handle )
{
    char rbuf[WORDS_PER_BLOCK*2];

    unsigned int count = 0;
    int writct, retstat;
    String dots = "";
    int blockct = 0;
    int status = 0;

    while( !status )
        {
        chandshake(ACKS);

        if ( count >= ps->totalct )
            break;

        if ( ( retstat = getdata1( (short int *)rbuf, WORDS_PER_BLOCK*2, ps ) ) != 0 )
            {
            if (retstat == 1)
                status = 2; /* comm error */
            else if (retstat == 2)
                status = 4; /* checksum error */

            break; // timeout or error
            }

        blockct++;

        if ( ps->totalct <= 19200 )
            {
            if ( blockct % 8 == 0 )
                {
                dots += ".";
                Form1->Memo1->Lines->Add( dots );
                }
            }
        else
            {
            if ( blockct % 32 == 0 )
                {
                dots += ".";
                Form1->Memo1->Lines->Add( dots );
                }
            }

        count += WORDS_PER_BLOCK;
        writct = WORDS_PER_BLOCK*2;

        /* write only up to totalct words... */
        if ( count > ps->totalct )
            writct -= (int)(count-ps->totalct) * 2;

        /* write to file */
        if ( FileWrite( handle, rbuf, writct ) < 0 )
            {
            chandshake(ASD);
            status = 1;
            }
        }

    if ( count < ps->totalct )
        status |= 8; /* wrong number of words received */

    return( status );

    /* 0=OK, 1=writerror, 2=com error, 4=bad chksm, 8=wrong # words */
}


int getdata1( short int * bufptr, int max_bytes, PSTOR * ps )
{

    int ii;
    unsigned char *cp;
    unsigned char checksum;
    short int baseline, temp;

    baseline = (short int)(1 << (ps->bits_per_sample-1));

    /* see if chars waiting from serial port... */
    if ( receive( max_bytes+2 ) )
        return( 1 );

    cp = TempArray+1;
    checksum = 0;

    for ( ii = 0 ; ii < max_bytes/2 ; ii++ )
        {
        temp = (short int)(*cp << 5);
        checksum ^= *cp++;
        temp |= (short int)(*cp >> 2);
        checksum ^= *cp++;
        temp -= baseline; /* convert to two's compliment */
        *bufptr++ = temp;
        }

    if ( checksum != *cp )
        return( 2 ); /* bad checksum */

    return( 0 );
/*
    0=OK, 1=receive error, 2=bad checksum
*/
}


int getdata2( unsigned char * bufptr, int max_bytes )
{

    int ii;
    unsigned char *cp, checksum;

    /* see if chars waiting from serial port... */
    if ( receive( 0 ) )
        return( 1 );

    if ( ByteCount != max_bytes  )
        return( 2 );

    cp = TempArray+7;
    checksum = 0;

    for ( ii = 0 ; ii < max_bytes-9 ; ii++ )
        checksum ^= *cp++;

    if ( checksum != *cp++ )
        return( 3 ); /* bad checksum */

    return( 0 );
/*
    0=OK, 1=receive error, 2=wrong bytes, 3=bad checksum
*/
}


void decode_parmsDB( unsigned char * source, unsigned char * dest, int numchars )
{

    int ii;

    for ( ii = 0 ; ii < numchars ; ii++ )
        {
        *dest = *source++;
        *dest |= (unsigned char)(*source++ << 7);
        dest++;
        }

    if ( numchars > 1 )
        *dest = '\0';
}


unsigned int decode_parmsDD( unsigned char * tp )
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

    return( acc );
}


unsigned int decode_parmsDW( unsigned char * tp )
{

    unsigned int acc;

    acc = 0;
    acc |= *tp | (*(tp+1) << 7);

    acc <<= 8;
    tp -= 2;
    acc |= *tp | (*(tp+1) << 7);

    return( acc );
}


unsigned int decode_hedrTB( unsigned char * tp )
{

    unsigned int acc;

    acc = (unsigned int)*tp--;
    acc = (acc << 7) | (unsigned int)*tp--;
    acc = (acc << 7) | (unsigned int)*tp;

    return( acc );
}

void cxmit( int samp, int mode )
{

    unsigned char midistr[6];

    midistr[0] = BEX;
    midistr[1] = COMMON_ID;
    midistr[2] = (unsigned char)mode;
    midistr[3] = (unsigned char)samp;
    midistr[4] = 0;
    midistr[5] = EEX;

    comws( 6, midistr );
}


void chandshake( int mode )
{

    unsigned char midistr[4];

    midistr[0] = BEX;
    midistr[1] = COMMON_ID;
    midistr[2] = (unsigned char)mode;
    midistr[3] = EEX;

    comws( 4, midistr );
}


/*************************************************************************/

void trim( char * trimstr, char * sourcestr)
{

while ( *sourcestr != '\0' && *sourcestr == ' ' )
  sourcestr++;

strcpy( trimstr, sourcestr );

/* point to NUL */
sourcestr = trimstr + strlen(trimstr);

while ( *(sourcestr-1) == ' ' && sourcestr != trimstr )
  {
  *(sourcestr-1) = '\0';
  sourcestr--;
  }
}

void catalog( bool print )
{
    // 7 hedr bytes + chksum byte + EEX = 9...
    if ( ByteCount < 9 || TempArray[3] != DCAT )
        {
        printerror( "check cable, is sampler on and configured\r\n"
          "for RS232, 38400 baud?" );
        return;
        }

    int entries = (ByteCount-9)/sizeof(S900CAT);

    if ( !entries )
        {
        printerror( "No Samples or programs in S900" );
        return;
        }

    S900CAT * tempptr = (S900CAT *)&TempArray[7]; // Skip header
    CAT * permsampptr = (CAT *)&PermSampArray[0];
    CAT * permprogptr = (CAT *)&PermProgArray[0];

    SampEntries = 0;
    ProgEntries = 0;

    for ( int ii = 0 ; ii < entries ; ii++ )
        {
        if ( tempptr->type == 'S' )
            {
            sprintf( permsampptr->name, "%.*s", MAX_NAME_S900, tempptr->name );
            trim( permsampptr->name, permsampptr->name );

            permsampptr->sampidx = tempptr->sampidx;

            SampEntries++; /* increment counter */
            permsampptr++; // next structure
            }
        else if ( tempptr->type == 'P' )
            {
            sprintf( permprogptr->name, "%.*s", MAX_NAME_S900, tempptr->name );
            trim( permprogptr->name, permprogptr->name );

            permprogptr->sampidx = tempptr->sampidx;

            ProgEntries++; /* increment counter */
            permprogptr++; // next structure
            }

        tempptr++; // next structure
        }


    if ( print )
        {
        permsampptr = (CAT *)&PermSampArray[0];
        permprogptr = (CAT *)&PermProgArray[0];

        Form1->Memo1->Lines->Add("Programs:");

        for ( int ii = 0 ; ii < ProgEntries ; ii++ )
            {
            Form1->Memo1->Lines->Add(String(permprogptr->sampidx+1) + ':' + String(permprogptr->name));
            permprogptr++;
            }

        Form1->Memo1->Lines->Add("Samples:");

        for ( int ii = 0 ; ii < SampEntries ; ii++ )
            {
            Form1->Memo1->Lines->Add(String(permsampptr->sampidx+1) + ':' + String(permsampptr->name));
            permsampptr++;
            }
        }
}

int GetAck( void )
{
    if ( receive( 0 ) )
        {
        printerror( "error receiving ack" );
        return(1);
        }

    if ( ByteCount == ACKSIZ && TempArray[ByteCount-2] == NAKS )
        {
        printerror( "packet NAKed" );

        return(1);
        }

    if ( ByteCount != ACKSIZ || TempArray[ByteCount-2] != ACKS )
        {
        printerror( "bad ACK packet" );

        return(1);
        }

    return(0);
}

int receive( int count )
// set count to 0 to receive a complete message
// set to "count" to receive a partial message.
{
    unsigned char tempc;
    bool have_bex = false;

    ByteCount = 0;

    int timer = 0;

    for(;;)
        {
        if( ++timer >= ACKTIMEOUT )
            return(1); // timeout

          if ( Form1->ApdComPort1->CharReady() )
              {
              tempc = Form1->ApdComPort1->GetChar();

              if ( !have_bex )
                  {
                  if ( tempc == BEX || count != 0 )
                      {
                      have_bex = true;
                      TempArray[ByteCount++] = tempc;
                      }
                  }
              else
                  {
                  TempArray[ByteCount++] = tempc;

                  if ( count != 0 && ByteCount == count )
                      return(0);

                  if ( tempc == EEX )
                      return(0);

                  if ( ByteCount == TEMPCATBUFSIZ ) // at buffer capacity... error
                      return(2);
                  }
              }
        }
}

void encode_parameters( int samp, PSTOR* ps )
{

    char locstr[MAX_NAME_S900+1];
    char temp;

    samp_parms[0] = BEX;
    samp_parms[1] = AKAI_ID;
    samp_parms[3] = SPRM;
    samp_parms[4] = S900_ID;
    samp_parms[6] = 0;
    samp_parms[128] = EEX;

    encode_parmsDW( 0, &samp_parms[55] ); /* loudness offset */
    encode_parmsDW( 0, &samp_parms[87] ); /* reserved */
    temp = 0;
    encode_parmsDB( (unsigned char *)&temp, (unsigned char *)&samp_parms[61], 1 ); /* reserved */
    encode_parmsDB( (unsigned char *)&temp, (unsigned char *)&samp_parms[91], 1 ); /* type = norm */
    temp = 'N';
    encode_parmsDB( (unsigned char *)&temp, (unsigned char *)&samp_parms[93], 1 ); /* waveform = 'N' */

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
    encode_parmsDD( ps->totalct, &samp_parms[39] );
    encode_hedrTB( ps->totalct, &samp_hedr[9] );

    /* start time */
    encode_parmsDD( ps->startidx, &samp_parms[71] );

    /* end time */
    encode_parmsDD( ps->endidx + 1, &samp_parms[63] );
    encode_hedrTB( ps->endidx + 1, &samp_hedr[15] );

    /* loop time */
    encode_parmsDD( ps->endidx-ps->loopidx, &samp_parms[79] );
    encode_hedrTB( ps->loopidx + 1, &samp_hedr[12] );

    /** temporary code!!! until Loop/Alt user switch added **/
    if ( ps->loopidx >= ps->totalct - 5 )
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
    encode_parmsDB( (unsigned char *)&ps->looping, (unsigned char *)&samp_parms[59], 1 );

    /* sampling frequency */
    encode_parmsDW( ps->freq, &samp_parms[47] );

    /* sampling period */
    encode_hedrTB( (unsigned int)ps->period, &samp_hedr[6] );

    /* pitch */
    encode_parmsDW(ps->pitch,&samp_parms[51]);

    // truncate to fit S900
    ps->name[MAX_NAME_S900] = '\0';

    /* ASCII sample name */
    strcpy( (char *)locstr, (char *)ps->name );

    int size = strlen( locstr );

    // Pad with blanks
    while ( size < MAX_NAME_S900 )
        locstr[size++] = ' ';

    encode_parmsDB( (unsigned char *)locstr, (unsigned char *)&samp_parms[7], MAX_NAME_S900 );

    compute_checksum();
}

void encode_parmsDB( unsigned char * source, unsigned char * dest, int numchars )
{

int ii;

for ( ii = 0 ; ii < numchars ; ii++ )
    {
    *dest++ = *source & (unsigned char)0x7f;

    if ( *source & (unsigned char)0x80 )
        *dest = 1;
    else
        *dest = 0;

    dest++;
    source++;
    }
}

void encode_parmsDD( unsigned int value, unsigned char * tp )
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

void encode_parmsDW( unsigned int value, unsigned char * tp )
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

void encode_hedrTB( unsigned int value, unsigned char * tp )
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

void compute_checksum( void )
{
    unsigned char checksum;
    int ii;

    // checksum and store in transmission array
    checksum = 0;

    for ( ii = 7 ; ii < 127 ; ii++ )
        checksum ^= samp_parms[ii];

    samp_parms[ii] = checksum;
}

int findidx( int * samp, char * name )
// 0=samples present, match
// 16=error
// 32=no samples
// 64=samples present, but no match
{
    // Request S900 Catalog
    exmit( 0, RCAT );

    if ( receive( 0 ) )
        {
        printerror( "error receiving catalog" );
        return( 16 );
        }

    catalog( false ); // populate sample and program structs (no printout)

    if ( SampEntries == 0 )
        {
        *samp = 0;
        return( 32 ); /* no samples... we are the only one */
        }

    CAT * catptr = (CAT * )PermSampArray;

    for ( int ii = 0 ; ii < SampEntries ; ii++ )
        {
        if ( strncmp( catptr->name, name, MAX_NAME_S900 ) == 0 )
            {
            *samp = catptr->sampidx;

            return( 0 ); /* match! */
            }

        catptr++;
        }

    *samp = SampEntries;

    return( 64 );
}

void exmit( int samp, int mode )
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

    comws( 8, midistr );
}

void comws( int count, unsigned char * ptr )
{
    for ( int ii = 0 ; ii < count ; ii++ )
        {
          while ( Form1->ApdComPort1->OutBuffFree < 1 );
          Form1->ApdComPort1->PutChar( *ptr++ );
        }
}

unsigned char send_data( short int * intptr, PSTOR * ps )
{
    short int val, baseline;
    unsigned char temp, checksum;

    baseline = (short int)(1 << (ps->bits_per_sample-1));

    checksum = 0;

    for ( int ii = 0 ; ii < WORDS_PER_BLOCK ; ii++ )
        {
        /* past totalct, read routine pumps out junk until */
        /* required 60 sample words have been transmitted. */

        val = *intptr++;
        val += baseline; /* convert out of two's complement format */
        temp = (unsigned char)(((unsigned short int)val >> 5) & 0x7f);
        checksum ^= temp;

        comws( 1, &temp );

        temp = (unsigned char)(((unsigned short int)val << 2) & 0x7c);
        checksum ^= temp;

        comws( 1, &temp );
        }

    return( checksum );
}

unsigned char wav_send_data( unsigned short * intptr, PSTOR * ps )
// S900 is a 12-bit sampler but can receive 14-bit samples
{
    unsigned short val;
    unsigned char temp, checksum;

    checksum = 0;

    // We expect a buffer of 14-bit, right-justified, UNSIGNED
    // values in a 16-bit, unsigned short format!!!

    for ( int ii = 0 ; ii < WORDS_PER_BLOCK ; ii++ )
        {
        // past totalct, read routine pumps out junk until
        // required 60 sample words have been transmitted.

        val = *intptr++;

        // byte 1 = 0 d13 d12 d11 d10 d9 d8 d7
        temp = (unsigned char)((val >> 7) & 0x7f); // bits 7-13

        checksum ^= temp;
        comws( 1, &temp );

        // byte 2 = 0 d6 d5 d4 d3 d2 d1 d0
        temp = (unsigned char)(val & 0x7f); // bits 0-6

        checksum ^= temp;
        comws( 1, &temp );
        }

    return( checksum );
}

void send_samp_parms( unsigned int index )
{
    compute_checksum();

    /* transmit sample parameters */
    samp_parms[5] = (unsigned char)(index);
    comws( PARMSIZ, samp_parms );
}

void printerror( String message )
{
    Form1->Memo1->Lines->Add( message );
}

void print_ps_info( PSTOR * ps )
{
    Form1->Memo1->Lines->Add( "" );
    Form1->Memo1->Lines->Add( "Output Parameters:" );
    Form1->Memo1->Lines->Add( "start: " + String(ps->startidx) );
    Form1->Memo1->Lines->Add( "end: " + String(ps->endidx) );
    Form1->Memo1->Lines->Add( "loop: " + String(ps->loopidx) );
    Form1->Memo1->Lines->Add( "frequency: " + String(ps->freq) );
    Form1->Memo1->Lines->Add( "pitch: " + String(ps->pitch) );
    Form1->Memo1->Lines->Add( "size(in words): " + String(ps->totalct) );
    Form1->Memo1->Lines->Add( "period: " + String((unsigned int)ps->period) );
    Form1->Memo1->Lines->Add( "bits per word: " + String(ps->bits_per_sample) );
    Form1->Memo1->Lines->Add( "name: " + String(ps->name) );
    Form1->Memo1->Lines->Add( "looping: " + String((char)ps->looping) );
}

//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
    Memo1->Lines->Add( "Click the menu above and select \"Help\"..." );
}
//---------------------------------------------------------------------------


void __fastcall TForm1::Cat1Click(TObject *Sender)
{
    ListBox1->Clear();

    // Request S900 Catalog
    exmit( 0, RCAT );

    if ( receive( 0 ) )
        {
        printerror( "error receiving catalog" );
        return;
        }

    catalog( true );
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Put1Click(TObject *Sender)
{
    ListBox1->Clear();

    char tbuf[WORDS_PER_BLOCK*2]; // 120 bytes
    char *pszBuffer;

    String sFileName = "";
    int iFileHandle;
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
    OpenDialog1->Title = "Open .WAV or .AKI File, Send To Akai";
    OpenDialog1->DefaultExt = "aki";
    OpenDialog1->Filter = "Aki files (*.aki)|*.aki|" "Wav files (*.wav)|*.wav|"
             "All files (*.*)|*.*";
    OpenDialog1->FilterIndex = 3; // start the dialog showing all files
    OpenDialog1->Options.Clear();
    OpenDialog1->Options << ofHideReadOnly
    << ofPathMustExist << ofFileMustExist << ofEnableSizing;

    if ( !OpenDialog1->Execute() )
      return; // Cancel

    Memo1->Clear();
    Memo1->Repaint();

    sFileName = OpenDialog1->FileName;

    // Load file
    Memo1->Lines->Add( "Loading File : \"" + sFileName + "\"" );

  	iFileHandle = FileOpen(sFileName, fmOpenRead);
  	iFileLength = FileSeek(iFileHandle,0,2);
  	FileSeek(iFileHandle,0,0);
  	pszBuffer = new char[iFileLength+1];
  	iBytesRead = FileRead(iFileHandle, pszBuffer, iFileLength);
  	FileClose(iFileHandle);

    strncpy( newname, ExtractFileName( sFileName ).c_str(), 10 );

    while ( strlen(newname) < 10 )
        newname[strlen(newname)] = ' ';

    newname[11] = '\0';

    bool exton = false;

    for ( int ii = 0 ; ii < (int)strlen(newname) ; ii++ )
        {
        if ( newname[ii] == '.' )
            exton = true;

        if ( !isalnum( newname[ii] ) || exton )
            newname[ii] = ' ';
        }

    strncpy( fileext, ExtractFileExt( sFileName ).c_str(), 4 );

    // set file extension (if any) lowercase
    for ( int ii = 0 ; ii < 4 ; ii++ )
      fileext[ii] = (char)tolower( fileext[ii] );

//    Memo1->Lines->Add( "ext: " + String(fileext) );

    if ( strncmp( ".aki", fileext, 4 ) ) // Not an AKI file? (try WAV...)
        {
        unsigned char *dp;
        char tempstr[5];
        int file_size, SampleRate, BytesPerSample;
        int AudioFormat, NumChannels, BytesPerFrame, BitsPerSample;
        int SubChunck1Size, DataSize, TotalFrames;

        if ( iBytesRead < 45 )
            {
            Memo1->Lines->Add( "bad file (1)" );
            return;
            }

        sprintf( tempstr, "%4s", &pszBuffer[0] );
        if ( !strcmp( tempstr, "RIFF" ) )
            {
            Memo1->Lines->Add( "bad file (2)" );
            return;
            }

        file_size = *(int *)&pszBuffer[4];
        if ( file_size != iBytesRead-8 )
            {
            Memo1->Lines->Add( "bad file (3)" );
            return;
            }

        sprintf( tempstr, "%4s", &pszBuffer[8] );
        if ( !strcmp( tempstr, "WAVE" ) )
            {
            Memo1->Lines->Add( "bad file (4)" );
            return;
            }

        sprintf( tempstr, "%4s", &pszBuffer[12] );
        if ( !strcmp( tempstr, "fmt " ) )
            {
            Memo1->Lines->Add( "bad file (5)" );
            return;
            }

        SubChunck1Size = *(int *)&pszBuffer[16];
        if ( SubChunck1Size < 16 )
            {
            Memo1->Lines->Add( "bad file (6)" );
            return;
            }

        sprintf( tempstr, "%4s", &pszBuffer[20+SubChunck1Size+0] );
        if ( !strcmp( tempstr, "data" ) )
            {
            Memo1->Lines->Add( "bad file (7)" );
            return;
            }

        // Bytes of data following "data"
        //**** Data must end on an even byte boundary!!! (explains
        // why the file may appear 1 byte over-size for 8-bit mono)
        DataSize = *(int *)&pszBuffer[20+SubChunck1Size+4];
        Memo1->Lines->Add( "DataSize: " + String( DataSize ) );

        AudioFormat = (int)*(short int *)&pszBuffer[20];
        if ( AudioFormat != 1 ) // we can't handle compressed WAV-files
            {
            Memo1->Lines->Add( "unknown wave format: " + String( AudioFormat ) );
            return;
            }

        NumChannels = (int)*(short int *)&pszBuffer[22];
        Memo1->Lines->Add( "NumChannels: " + String( NumChannels ) );

        SampleRate = *(int *)&pszBuffer[24];
        Memo1->Lines->Add( "SampleRate: " + String( SampleRate ) );

        // BytesPerFrame: Number of bytes per frame
        BytesPerFrame = (int)*(short int *)&pszBuffer[32];
        Memo1->Lines->Add( "BytesPerFrame: " + String( BytesPerFrame ) );

        BitsPerSample = (int)*(short int *)&pszBuffer[34];
        Memo1->Lines->Add( "BitsPerSample: " + String( BitsPerSample ) );
        if ( BitsPerSample < 8 || BitsPerSample > 64 )
            {
            Memo1->Lines->Add( "bits per sample out of range: " +
                                 String( BitsPerSample ) );
            return;
            }

        BytesPerSample = BitsPerSample/8;
        if ( BitsPerSample % 8 ) // remaining bits?
            BytesPerSample++; // round up

        if ( BytesPerSample > 8 )
            {
            Memo1->Lines->Add( "error: can't handle samples over 64-bits!" );
            return;
            }

        if ( NumChannels * BytesPerSample != BytesPerFrame )
            {
            Memo1->Lines->Add( "error: (NumChannels * BytesPerSample != BytesPerFrame)" );
            return;
            }

        if ( DataSize == 4 ) // Sound Recorder bug-fix
            {
            DataSize = iBytesRead - (20+SubChunck1Size+4+4);
            Memo1->Lines->Add( "Recomputed DataSize: " + String( DataSize ) );
            }

        if ( DataSize % (NumChannels * BytesPerSample) )
            {
            Memo1->Lines->Add( "data are corrupt" );
            return;
            }

        Memo1->Lines->Add( "BytesPerSample: " + String( BytesPerSample ) );

        TotalFrames = DataSize / (NumChannels * BytesPerSample);
        Memo1->Lines->Add( "TotalFrames: " + String( TotalFrames ) );

        // Need to populate a PSTOR structure for S900
        ps.startidx = 0; /* start of play index (4 bytes) */
        ps.endidx = (unsigned int)TotalFrames-1; /* end of play index (4 bytes) */
        ps.loopidx = ps.endidx; /* end of play index (4 bytes) */

        int tempfreq = SampleRate;
        if ( tempfreq > 49999 )
            tempfreq = 49999;
        ps.freq = (unsigned int)tempfreq; /* sample freq. in Hz (4 bytes) */

        ps.pitch = 960; /* pitch - units = 1/16 semitone (4 bytes) (middle C) */
        ps.totalct = (unsigned int)TotalFrames; /* total number of words in sample (4 bytes) */
        ps.period = 1.0e9/(double)ps.freq; /* sample period in nanoseconds (8 bytes) */

        ps.bits_per_sample = S900_BITS_PER_SAMPLE; /* (2 bytes, 14-bits) */

        strcpy( ps.name, newname ); /* ASCII sample name (17 bytes) */
        ps.looping = 'O'; /* (1 byte) */

        // now we need to set "samp index"
        retval = findidx( (int *)&index, ps.name );

        // 0=samples present, one matched
        // 16=error
        // 32=no samples
        // 64=samples present, but no match
        if ( retval == 16 )
            return; // catalog error

        if ( retval == 0 )
            Memo1->Lines->Add( "sample exists on S900, index: " +
                                           String( index+1 ) );

        // encode samp_hedr and samp_parms arrays
        encode_parameters( index, &ps );

        print_ps_info( &ps );

        // request common reception enable
        exmit( 0, SECRE );

        // transmit sample header info
        comws( HEDRSIZ, samp_hedr );

        // wait for acknowledge
        if ( GetAck() )
            return;

        int FrameCounter = 0; // We already processed the header

        blockct = 0;

        String dots = ".";
        Memo1->Lines->Add( dots ); // progress display

        // Left-channel data begins at 20 + SubChunck1Size + 4 + 4
        int InitialOffset = 20+SubChunck1Size+4+4;

        // if Stereo and User checked "send right channel"
        // point to first right-channel sample-datum
        if ( NumChannels > 1 && MenuSendRightChan->Checked )
          InitialOffset += BytesPerSample;

        dp = (unsigned char *)&pszBuffer[InitialOffset];

//        NS9dither16 * d = new NS9dither16();

        // for each frame in WAV-file (one-sample of 1-8 bytes...)
        for(;;)
            {
            // End of file?
            if ( FrameCounter >= TotalFrames )
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

            for ( int ii = 0 ; ii < WORDS_PER_BLOCK ; ii++ )
              {
              if ( FrameCounter >= TotalFrames )
                {
                tbuf[ii*2] = 0;
                tbuf[(ii*2)+1] = 0;
                }
              else
                {
                unsigned long acc;

                // one-byte wave samples are un-signed by default
                if ( BytesPerSample < 2 ) // one byte?
                  {
                  // convert byte to 14-bits and save in tbuf
                  // (left-channel sample)
                  acc = *dp; // don't add ii here!
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
                  for ( int ii = 0 ; ii < BytesPerSample ; ii++ )
                    acc |= *(dp+ii) << (8*ii);

                  acc >>= (8*BytesPerSample)-BitsPerSample; // right-justify so we can add

                  // convert from 2's compliment to offset-binary
                  acc += 1 << (BitsPerSample-1);

                  // convert to 14-bits
                  if ( BitsPerSample > 14 )
                    acc >>= BitsPerSample-14;
                  else if ( BitsPerSample < 14 )
                    acc <<= 14-BitsPerSample;
                  }

                // save converted sample in tbuf
                // a short int in memory is ordered LSB first, then MSB!
                tbuf[ii*2] = (unsigned char)acc; // LSB
                tbuf[(ii*2)+1] = (char)((unsigned short)acc >> 8); // MSB
                }

              dp += BytesPerFrame; // Next frame (only use left channel for stereo)
              FrameCounter++;
              }

            // Print progress-display
            blockct++;

            if ( ps.totalct <= 19200 )
                {
                if ( blockct % 8 == 0 )
                    {
                    dots += ".";
                    Form1->Memo1->Lines->Add( dots );
                    }
                }
            else
                {
                if ( blockct % 32 == 0 )
                    {
                    dots += ".";
                    Form1->Memo1->Lines->Add( dots );
                    }
                }

            unsigned char temp = (unsigned char)(blockct & 0x7f);
            comws( 1, &temp );

            // Send data and checksum
            temp = wav_send_data( (unsigned short *)tbuf, &ps );
            comws( 1, &temp );

            // wait for acknowledge
            if ( GetAck() )
                return;
            }

//        delete d;
        }
    else // AKI file (my custom format)
        {

        if ( iBytesRead < (WORDS_PER_BLOCK*2) + PSTOR_SIZ + MAGIC_SIZ )
            {
            printerror( "file is corrupt" );
            return;
            }

        Memo1->Lines->Add( "Read " + String(iBytesRead) + " bytes" );

        unsigned int my_magic;

        memcpy( &my_magic, &pszBuffer[0], sizeof(my_magic) );
//    Memo1->Lines->Add( "magic = " + String(my_magic) );

        if ( my_magic != MAGIC_NUM )
            {
            printerror( "File is not the right kind!" );
            return;
            }

        memcpy( &ps, &pszBuffer[0+MAGIC_SIZ], PSTOR_SIZ );

        trim( ps.name, ps.name );

        // encode new sample name if any entered
        //    char name[11] = "TEMPNAME00";
        //    sprintf( ps.name, "%.*s", MAX_NAME_S900, name );

        // now we need to set "samp index"
        retval = findidx( (int *)&index, ps.name );

        // 0=samples present, one matched
        // 16=error
        // 32=no samples
        // 64=samples present, but no match
        if ( retval == 16 )
            return; // catalog error

        if ( retval == 0 )
            Memo1->Lines->Add( "sample exists on S900, index: " +
                                     String( index+1 ) );

        // encode samp_hedr and samp_parms arrays
        encode_parameters( index, &ps );

        print_ps_info( &ps );

        // request common reception enable
        exmit( 0, SECRE );

        // transmit sample header info
        comws( HEDRSIZ, samp_hedr );

        // wait for acknowledge
        if ( GetAck() )
            return;

        char * ptr = &pszBuffer[PSTOR_SIZ + MAGIC_SIZ];
        int ReadCounter = PSTOR_SIZ + MAGIC_SIZ; // We already processed the header

        blockct = 0;

        String dots = ".";
        Memo1->Lines->Add( dots );

        for(;;)
            {
            // End of file?
            if ( ReadCounter >= iBytesRead )
                break;

            // read block of 60 words from buffer... */
            for ( int ii = 0 ; ii < WORDS_PER_BLOCK*2 ; ii++ )
                {
                if ( ReadCounter >= iBytesRead )
                    tbuf[ii] = 0; // pad last block if end of file
                else
                    tbuf[ii] = *ptr++;

                ReadCounter++;
                }

            blockct++;

            if ( ps.totalct <= 19200 )
                {
                if ( blockct % 8 == 0 )
                    {
                    dots += ".";
                    Form1->Memo1->Lines->Add( dots );
                    }
                }
            else
                {
                if ( blockct % 32 == 0 )
                    {
                    dots += ".";
                    Form1->Memo1->Lines->Add( dots );
                    }
                }

            unsigned char temp = (unsigned char)(blockct & 0x7f);
            comws( 1, &temp );

            // Send data and checksum
            temp = send_data( (short int *)tbuf, &ps );
            comws( 1, &temp );

            // wait for acknowledge
            if ( GetAck() )
                {
                Form1->Memo1->Lines->Add( "no ACK at block: " + String( blockct ) );
                break;
                }
            }

        }

    unsigned char temp = EEX;
    comws( 1, &temp );

    exmit( 0, SECRD ); /* request common reception disable */

    sprintf( locstr, "%02d", index );

    retval = findidx( (int *)&index, locstr );

    // 0=samples present, one matched
    // 16=error
    // 32=no samples
    // 64=samples present, but no match
    if ( retval == 16 )
        return; // catalog error

    if ( retval == 0 )
        Memo1->Lines->Add( "sample written, index: " + String( index+1 ) );
    else
        {
        Memo1->Lines->Add( "index string not found, index: " + String( index+1 ) );
        return;
        }

    send_samp_parms( index );

    delete [] pszBuffer;
    }
  catch(...)
    {
    ShowMessage("Can't load file: \"" + sFileName + "\"" );
    }
}
//---------------------------------------------------------------------------


void __fastcall TForm1::Get1Click(TObject *Sender)
{
  try
    {
    ListBox1->Clear();

    // Request S900 Catalog
    exmit( 0, RCAT );

    if ( receive( 0 ) )
        {
        printerror( "error receiving catalog" );
        return;
        }

    catalog( false );

    if ( !SampEntries )
        {
        Memo1->Lines->Add( "No samples in S900!" );
        return;
        }

    CAT *catp = (CAT *)PermSampArray;

    for ( int ii = 0 ; ii < SampEntries ; ii++, catp++ )
        ListBox1->Items->Add( catp->name );

    Memo1->Lines->Add( "\r\n***Click a sample at the right to receive\r\n"
                       "and save it to a .AKI file*** --->");
    }
  catch(...)
    {
    ShowMessage("Can't get catalog" );
    }
}
//---------------------------------------------------------------------------


void __fastcall TForm1::ListBox1Click(TObject *Sender)
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

  	if ( SaveDialog1->Execute() )
    	{
        ListBox1->Repaint();

        if ( get_sample( ListBox1->ItemIndex, SaveDialog1->FileName ) != 0 )
            Memo1->Lines->Add( "Not able to save sample!" );
        else
            Memo1->Lines->Add( "Sample saved as: \"" + SaveDialog1->FileName + "\"" );
        }
    }
  catch(...)
    {
    ShowMessage("Can't save file: \"" + SaveDialog1->FileName + "\"" );
    }
}
//---------------------------------------------------------------------------


void __fastcall TForm1::Help1Click(TObject *Sender)
{
    Memo1->Lines->Clear();
    Memo1->Lines->Add( "Connect a cable between the S900's (or S950) RS232\r\n"
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
      "window. - Cheers, Scott Swift. dxzl@live.com" );
}
//---------------------------------------------------------------------------




void __fastcall TForm1::MenuUseRS232Click(TObject *Sender)
{
//  MenuUseRS232->Checked = !MenuUseRS232->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::MenuSendRightChanClick(TObject *Sender)
{
  MenuSendRightChan->Checked = !MenuSendRightChan->Checked;
}
//---------------------------------------------------------------------------

