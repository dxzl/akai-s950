//---------------------------------------------------------------------------
//Software written by Scott Swift 2017 - This program is distributed under the
//terms of the GNU General Public License.
//---------------------------------------------------------------------------
#include <vcl.h>
#include "MainForm.h"
#pragma hdrstop

#include "S9Utils.h"
#include "ProgramsForm.h"
#include "SampParmsForm.h"
#include "OverallSettingsForm.h"
#include "RegHelper.h"
//#include <Clipbrd.hpp>
#include <stdio.h>
#include <math.h>
//---------------------------------------------------------------------------
#pragma link "AdPort"
#pragma link "OoMisc"
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormMain *FormMain;
using namespace S9Utils;

//---------------------------------------------------------------------------
__fastcall TFormMain::TFormMain(TComponent* Owner)
  : TForm(Owner)
{
#if (AKI_FILE_HEADER_SIZE != 72)
  printm("\r\nWARNING: sizeof(PSTOR) != 72\r\n"
    "TO DEVELOPER: sizeof(PSTOR) MUST be " + String(72) + " bytes\r\n"
    " to maintain compatibility with old .AKI files!");
#endif
#if (sizeof(S950CAT) != 12)
  printm("\r\nWARNING: sizeof(S950CAT) != 12\r\n"
    "TO DEVELOPER: sizeof(S950CAT) MUST be " + String(12) + " bytes!");
#endif
  // use the following # for PSTOR_STRUCT_SIZ in MainForm.h!
  //printm("sizeof(PSTOR):" + String(sizeof(PSTOR)));
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormCreate(TObject *Sender)
{
  Timer1->OnTimer = NULL;
  Timer1->Enabled = false;

  m_rxByteCount = 0;
  m_gpTimeout = false;
  m_busyCount = 0;
  m_numSampEntries = 0;
  m_numProgEntries = 0;
  m_elapsedSeconds = 0;
  m_inBufferFull = false;
  m_abort = false;

  // read settings from registry HKEY_CURRENT_USER
  // \\Software\\Discrete-Time Systems\\AkaiS950
  TRegHelper* pReg = NULL;

  try
  {
    try
    {
      pReg = new TRegHelper(true);

      if (pReg != NULL)
      {
        // tell user how to delete reg key if this is first use...
        if (pReg->ReadSetting(S9_REGKEY_VERSION).IsEmpty())
        {
          pReg->WriteSetting(S9_REGKEY_VERSION, String(VERSION_STR));

          // cmd reg delete "HKCU\Software\Discrete-Time Systems\AkaiS950" /f
          printm("This app stores its settings in the windows registry. "
            "To delete settings, go to Start => Run and type \"cmd\". "
            "In the window type the line below and press enter:\r\n\r\n"
            "reg delete \"HKCU\\Software\\Discrete-Time Systems\\AkaiS950\" /f\r\n"
            "(or: Start => Run, \"regedit\" and search for \"AkaiS950\")\r\n\r\n"
            "Author: Scott Swift, Test Engineer: Oliver Tann");
        }

        pReg->ReadSetting(S9_REGKEY_BAUD, m_baud, DEF_RS232_BAUD_RATE);
        pReg->ReadSetting(S9_REGKEY_USE_RIGHT_CHAN, m_use_right_chan, true);
        pReg->ReadSetting(S9_REGKEY_AUTO_RENAME, m_auto_rename, true);
        pReg->ReadSetting(S9_REGKEY_FORCE_HWFLOW, m_force_hwflow, false);
        pReg->ReadSetting(S9_REGKEY_SEND_AS_12_BITS, m_send_as_12_bits, false);
      }
      else
      {
        ShowMessage("Unable to read settings from the registry!");
        m_baud = DEF_RS232_BAUD_RATE;
        m_use_right_chan = true;
        m_auto_rename = true;
        m_force_hwflow = false;
        m_send_as_12_bits=false;
      }
    }
    catch (...)
    {
      ShowMessage("Unable to read settings from the registry!");
      m_baud = DEF_RS232_BAUD_RATE;
      m_use_right_chan = true;
      m_auto_rename = true;
      m_force_hwflow = false;
      m_send_as_12_bits=false;
    }

    ComboBoxBaudRate->Enabled = true;

    m_data_size = DATA_PACKET_SIZE;
    m_data_size += DATA_PACKET_OVERHEAD;
    m_hedr_size = HEDRSIZ;
    m_ack_size = ACKSIZ;

    //printm(VERSION_STR);
    printm("For connection tips, click \"Menu\" and select \"About\"...");
  }
  __finally
  {
    try { if (pReg != NULL) delete pReg; }
    catch (...) {}
  }

  // in the ApdComPort1 component, set:
  //HWFlowOptions:
  //hwUseDTR = false;
  //hwUseRTS = false;
  //hwRequireDSR = false;
  //hwRequireCTS = false;
  //SWFlowOptions = swfNone;

  MenuUseRightChanForStereoSamples->Checked = m_use_right_chan;
  MenuAutomaticallyRenameSample->Checked = m_auto_rename;
  MenuUseHWFlowControlBelow50000Baud->Checked = m_force_hwflow;
  MenuMax12BitsPerSample->Checked = m_send_as_12_bits;
  SetMenuItems();

  Caption = "Akai S900/S950 (rs232) " + String(VERSION_STR);

  SetComPort(m_baud);

  // create list for drag-dropped file-names
  m_slFilePaths = new TStringList();

  //enable drag&drop files
  ::DragAcceptFiles(this->Handle, true);

  //printm(String(sizeof(WAVHEDR)));
  //printm(String(sizeof(SMPLLOOP)));
  //printm(String(sizeof(SMPLDATA)));
  //printm(String(sizeof(SMPLCHUNKHEDR)));
  //printm(String(sizeof(SMPLCHUNKOUT)));
  //printm(String(sizeof(CUEPOINT)));
  //printm(String(sizeof(CUECHUNK)));
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormDestroy(TObject *Sender)
{
  if (m_slFilePaths)
  {
    delete m_slFilePaths;
    m_slFilePaths = NULL;
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormShow(TObject *Sender)
{
  Memo1->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormClose(TObject *Sender, TCloseAction &Action)
{
  // save settings to registry HKEY_CURRENT_USER
  // \\Software\\Discrete-Time Systems\\AkaiS950
  TRegHelper* pReg = NULL;

  try
  {
    pReg = new TRegHelper(false);

    if (pReg != NULL)
    {
      pReg->WriteSetting(S9_REGKEY_BAUD, m_baud);
      pReg->WriteSetting(S9_REGKEY_USE_RIGHT_CHAN, m_use_right_chan);
      pReg->WriteSetting(S9_REGKEY_AUTO_RENAME, m_auto_rename);
      pReg->WriteSetting(S9_REGKEY_FORCE_HWFLOW, m_force_hwflow);
      pReg->WriteSetting(S9_REGKEY_SEND_AS_12_BITS, m_send_as_12_bits);
    }
  }
  __finally
  {
    try { if (pReg != NULL) delete pReg; }
    catch (...) {}
  }
}
//---------------------------------------------------------------------------
// Get sample routines
//---------------------------------------------------------------------------
// All you need to call to receive a sample into a file...
// Returns 0 if success, negative if fail (-1 if error message printed).
// Pass in the index from sampler's catalog and full file path and name
int __fastcall TFormMain::GetSample(String sPath, int iSamp)
{
  long lTempHandle = 0;
  int iError = -1;

  if (sPath.IsEmpty())
  {
    printm("sPath is empty in GetSample()!");
    return -1;
  }

  if (FileExists(sPath))
  {
    ShowMessage("File already exists, delete or move it first!\n"
                    "(\"" + sPath + "\")");
    return -1;
  }

  String sTempPath = sPath + ".tmp"; // double extension

  if (FileExists(sTempPath))
  {
    try
    {
      DeleteFile(sTempPath);
    }
    catch (...)
    {
      printm("unable to delete temp-file: \"" + sTempPath + "\"");
    }
  }

  lTempHandle = FileCreate(sTempPath);
  if (lTempHandle == 0)
  {
    printm("Unable to create temp file: \"" + sTempPath + "\"!");
    return -1;
  }

  try
  {
    try
    {
      // populate global samp_parms array
      iError = LoadSampParmsToTempArray(iSamp);
      if (iError < 0)
      {
        switch(iError)
        {
          case -2:
              printm("transmit timeout!");
          break;
          case -3:
              printm("timeout receiving sample parameters!");
          break;
          case -4:
              printm("received wrong bytecount for sample parameters!");
          break;
          case -5:
              printm("bad checksum for sample parameters!");
          break;
          case -6:
            printm("exception in LoadSampParmsToTempArray()!");
          break;
          default:
          break;
        };
        return -1; // could not get sample parms
      }

      // copy m_temp_array into samp_parms
      memcpy(samp_parms, m_temp_array, PARMSIZ);

      // delay, then request common reception enable
      if (!exmit(0, SECRE, true))
        return -1; // tx timeout

      // doesn't hurt to give ye olde S900 processor some time!
      DelayGpTimer(25);

      if (get_comm_samp_hedr(iSamp) < 0)
        return -1; // could not get header data

      PSTOR ps = { 0 };

      // fill PSTOR struct using info in samp_parms[] and samp_hedr[]...
      decode_sample_info(&ps);

      print_ps_info(&ps); // display info in window
      
#if DIAGNOSTIC_MODE
      printm("\nsamp_parms[" + String(PARMSIZ) + "]:\n");
      display_hex(samp_parms, PARMSIZ);
      printm("");
#endif

      String sExt = ExtractFileExt(sPath).LowerCase();

      if (sExt != AKIEXT)
        iError = GetAsWavFile(lTempHandle, iSamp, &ps);
      else
        iError = GetAsAkiFile(lTempHandle, iSamp, &ps);
    }
    catch(...)
    {
      printm("exception thrown in GetSample()!");
      return -1;
    }
  }
  __finally
  {
    if (lTempHandle != 0)
      FileClose(lTempHandle);

    if (iError < 0)
    {
      chandshake(ASD); // abort dump
      printm("aborted sample-dump: " + String(iError));
    }

    // request common reception disable (after 25ms delay)
    exmit(0, SECRD, true);
  }

  if (FileExists(sTempPath))
  {
    if (iError < 0)
    {
      try
      {
        DeleteFile(sTempPath);
        printm("deleted temporary file...");
      }
      catch (...)
      {
        printm("unable to delete temporary file: \"" + sTempPath + "\"");
      }
    }
    else
    {
      // Rename temp-file
      try
      {
        RenameFile(sTempPath, sPath);
        printm("temporary file renamed to: \"" + sPath + "\"");
      }
      catch (...)
      {
        printm("unable to rename temporary file: \"" + sTempPath + "\"");
        printm("to: \"" + sPath + "\"");
      }
    }
  }

  return iError; // bug 11/14/2020 was returning 0
}
//---------------------------------------------------------------------------
// returns the file-size or negative if error
int __fastcall TFormMain::GetAsAkiFile(long lFileHandle, int iSamp, PSTOR* ps)
{
  uint32_t my_magic = MAGIC_NUM_AKI;
  int fileSize = 0;

  try
  {
    // write the magic number and header...
    if (FileWrite(lFileHandle, &my_magic, UINT32SIZE) < 0)
      return -1;
    fileSize += UINT32SIZE;

    // write the full 72 byte struct
    if (FileWrite(lFileHandle, ps, AKI_FILE_HEADER_SIZE) < 0)
      return -1;
    fileSize += AKI_FILE_HEADER_SIZE;

    // receive, verify and write sample data blocks to file
    int dataBytesWritten = get_samp_data(ps, lFileHandle, false);

    if (dataBytesWritten < 0)
      return -1;
    fileSize += dataBytesWritten;
  }
  catch(...)
  {
    printm("exception thrown in GetAsAkiFile()!");
    return -1; // exception
  }
  printm("AKAI file-size (in bytes): " + String(fileSize));
  return fileSize;
}
//---------------------------------------------------------------------------
// returns the file-size or negative if error
int __fastcall TFormMain::GetAsWavFile(long lFileHandle, int iSamp, PSTOR* ps)
{
  int fileSize = 0;

  try
  {
    // Output WAV format:
    // 0-3 "RIFF" (little-endian) "RIFX" big-endian
    // 4-7 filesize-8
    // 8-11 "WAVE"
    // 12-15 "fmt "
    // 16-19 size of fmt block - 16
    // 20-21 AudioFormat 1 (uncompressed pcm) (int16_t)
    // 22-23 NumChannels 1 (mono) (int16_t)
    // 24-27 SampleFreq ???? (int32_t)
    // 28-31 FrameRate (SampleRate * BitsPerSample * NumChannels) / 8 (int32_t)
    // 32-33 BytesPerFrame (NumChannels * BitsPerSample/8), 2 (int16_t)
    // 34-35 BitsPerSample 16 (int16_t)
    // 36-39 "data"
    // 40-43 # bytes of data (sample words * 2)
    // 44-45 (sample word 1 of N)
    // 46-? sample data (on even byte-boundary because we have 2 bytes/frame for 16-bit mono)
    // SMPLCHUNKOUT struct

    WAVHEDR wh = {0};
    strncpy(wh.chunkTag, "RIFF", 4);
    strncpy(wh.wave, "WAVE", 4);
    strncpy(wh.fmt, "fmt ", 4);
    strncpy(wh.data, "data", 4);
    wh.fmtSectionSize = 16;
    wh.audioFormat = 1; // uncompressed pcm
    wh.numChannels = 1; // mono
    wh.sampleFreq = ps->freq;
    wh.bitsPerSample = ps->bits_per_word; // sampler sends 12-bits (TODO: algorithm to convert to 16-bit wav)
    wh.bytesPerFrame = wh.bitsPerSample/8;
    if (wh.bitsPerSample % 8)
      wh.bytesPerFrame++;
    wh.bytesPerFrame *= wh.numChannels;
    wh.frameRate = wh.sampleFreq*wh.bytesPerFrame;
    wh.dataSize = ps->sampleCount*wh.bytesPerFrame;
    int expectedDataFileBytes = wh.dataSize;
    if (expectedDataFileBytes & 1)
      expectedDataFileBytes++; // if odd # data bytes we have a "phantom" padding byte added
    wh.fileSizeMinusEight = 0; // fill this in at the end!
    // ps.loopmode, ps.startpoint, ps.endpoint, ps.looplen, ps.pitch, ps.loudnessoffset, ps.flags

    if (FileWrite(lFileHandle, &wh, sizeof(WAVHEDR)) < 0)
    {
      printm("error writing wave-header structure!");
      return -1;
    }
    fileSize += sizeof(WAVHEDR);

    // receive, verify and write sample data blocks to file
    int dataBytesWritten = get_samp_data(ps, lFileHandle, true);

    if (dataBytesWritten != expectedDataFileBytes)
    {
      if (dataBytesWritten >= 0)
        printm("expected " + String(expectedDataFileBytes) + " data bytes, got " + String(dataBytesWritten));
      return -1; // could not get sample data
    }
    fileSize += dataBytesWritten;

    CUECHUNK cc = {0};
    strncpy(cc.chunkID, "cue ", 4);
    cc.chunkDataSize = sizeof(CUECHUNK)-8;
    cc.cuePointsCount = 2;
    // next come the cue-points - (we have two, start and end)
    // start
    strncpy(cc.cuePoints[0].cuePointID, "strt", 4);
    cc.cuePoints[0].playOrderPosition = 0;
    strncpy(cc.cuePoints[0].dataChunkID, "data", 4);
    cc.cuePoints[0].chunkStart = 0;
    cc.cuePoints[0].blockStart = 0;
    cc.cuePoints[0].frameOffset = ps->startpoint; // start-point in frames
    // end
    strncpy(cc.cuePoints[1].cuePointID, "end ", 4);
    cc.cuePoints[1].playOrderPosition = 0;
    strncpy(cc.cuePoints[1].dataChunkID, "data", 4);
    cc.cuePoints[1].chunkStart = 0;
    cc.cuePoints[1].blockStart = 0;
    cc.cuePoints[1].frameOffset = ps->endpoint; // end-point in frames

    if (FileWrite(lFileHandle, &cc, sizeof(CUECHUNK)) < 0)
    {
      printm("error writing cue-points structure!");
      return -1;
    }
    fileSize += sizeof(CUECHUNK);

    SMPLCHUNKHEDR sch = {0};
    strncpy(sch.chunkTag, "smpl", 4);
    sch.Manufacturer = 0;
    sch.Product = 0;
    sch.SamplePeriod = ps->period; // in nanoseconds
    sch.MIDIUnityNote = ps->pitch/STEPSPERSEMITONE; // 960/16 = 60 = middle C
    // the fraction of a semitone up from the specified MIDIUnityNote
    // 0x80000000 = 1/2 semitone (50 cents)
    sch.MIDIPitchFraction =
      (ps->pitch%STEPSPERSEMITONE)*((unsigned)0x80000000/(STEPSPERSEMITONE/2));
    sch.SMPTEFormat = 0;
    sch.SMPTEOffset = 0;
    sch.loopCount = (ps->loopmode != 'O' && ps->looplen >= MINLOOP) ? 1 : 0;
    // sc.samplerDataSize is 1 SMPLLOOP, plus a SMPLDATA struct
    sch.samplerDataSize = sizeof(SMPLDATA);
    if (sch.loopCount > 0)
      sch.samplerDataSize += sch.loopCount*sizeof(SMPLLOOP);
    sch.chunkSize = sizeof(SMPLCHUNKHEDR)+sch.samplerDataSize-8;

    if (FileWrite(lFileHandle, &sch, sizeof(SMPLCHUNKHEDR)) < 0)
    {
      printm("error writing sample-info structure!");
      return -1;
    }
    fileSize += sizeof(SMPLCHUNKHEDR);

    // don't even write a loop if in one-shot looping mode (off)
    if (sch.loopCount == 1)
    {
      SMPLLOOP sl = {0};
      // loop
      sl.cuePointId = 0; // 4-character ID (could refers to a particular cue-point)
      // ps.loopmode O=one-shot (no looping), A=alternating, L=loop
      if (ps->loopmode == 'A')
        sl.type = 1; // alternating
      else
        sl.type = 0; // loop forward
      // going to write as frame-indices rather than bytes
      sl.start = (ps->endpoint > ps->looplen) ? ps->endpoint-ps->looplen : 0;
      sl.end = ps->endpoint;
      sl.fraction = 0;
      sl.playCount = 0; // endless

      if (FileWrite(lFileHandle, &sl, sizeof(SMPLLOOP)) < 0)
      {
        printm("error writing sample-loop structure!");
        return -1;
      }
      fileSize += sizeof(SMPLLOOP);
    }

    SMPLDATA sd = {0};
    // write extra data, our SMPLDATA struct with a "magic number" for verification
    sd.magic = MAGIC_NUM_AKI;
    sd.flags = ps->flags;
    sd.loudnessoffset = ps->loudnessoffset;

    if (FileWrite(lFileHandle, &sd, sizeof(SMPLDATA)) < 0)
    {
      printm("error writing sample-data structure!");
      return -1;
    }
    fileSize += sizeof(SMPLDATA);

    FileSeek(lFileHandle, (int)sizeof(uint32_t), 0); // point to fileSizeMinusEight
    uint32_t iTemp = fileSize - 8;
    if (FileWrite(lFileHandle, &iTemp, sizeof(uint32_t)) < 0)
    {
      printm("error writing file-size!");
      return -1;
    }
  }
  catch(...)
  {
    printm("exception thrown in GetAsWavFile()!");
    return -1; // exception
  }
  printm("WAVE file-size (in bytes): " + String(fileSize));
  return fileSize;
}
//---------------------------------------------------------------------------
// puts samp_parms[] and samp_hedr[] arrays info into PSTOR struct (ps)
void __fastcall TFormMain::decode_sample_info(PSTOR* ps)
{
  // FROM AKAI EXCLUSIVE SAMPLE PARAMETERS... (do this before decoding header)

  // clear spare-byte fields
  ps->spareDB1 = 0; // 1 byte

  // ASCII sample name
  AsciiStrDecode(ps->name, &samp_parms[7]); // 10 bytes => 14 byte null terminated string

  // undefined
  ps->undefinedDD_27 = decodeDD((uint8_t*)&samp_parms[27]); // 8=>4

  // undefined
  ps->undefinedDW_35 = decodeDW((uint8_t*)&samp_parms[35]); // 4=>2

  // number of words in sample (for velocity-crossfade it's the sum of soft and loud parts)
  ps->sampleCount = decodeDD(&samp_parms[39]); // 8=>4

  // original sample rate in Hz
  ps->freq = decodeDW(&samp_parms[47]); // 4=>2
  
  // nominal pitch in 1/16 semitone, C3=960
  ps->pitch = decodeDW(&samp_parms[51]); // 4=>2

  // loudness offset (signed) nominal pitch in 1/16 semitone, C3=960
  ps->loudnessoffset = decodeDW(&samp_parms[55]); // 4=>2

  // replay mode 'A', 'L' or 'O' (alternating, looping or one-shot)
  // (samp_hedr[18] also has sample's loop mode: 0=looping, 1=alternating
  // or one-shot if loop-length < MINLOOP)
  ps->loopmode = decodeDB(&samp_parms[59]); // 2=>1

  // 61,62 DB reserved
  ps->reservedDB_61 = decodeDB(&samp_parms[61]); // 2=>1

  // 63-70 DD end point relative to start of sample (1800 default for TONE sample)
  ps->endpoint = decodeDD(&samp_parms[63]); // 8=>4

  // 71-78 DD first replay point relative to start of sample (default is 0)
  ps->startpoint = decodeDD(&samp_parms[71]); // 8=>4

  // 79-86 DD length of looping or alternating part
  ps->looplen = decodeDD(&samp_parms[79]); // 8=>4

  // 87-90 DW reserved
  ps->reservedDW_87 = decodeDW(&samp_parms[87]); // 4=>2

  // 91,92 DB type of sample 0=normal, 255=velocity crossfade
  bool xfade = (decodeDB(&samp_parms[91]) == 255); // 2=>1

  // 93,94 DB sample waveform 'N'=normal, 'R'=reversed
  bool reversed = (decodeDB(&samp_parms[93]) == 'R'); // 2=>1

  ps->flags = 0;
  if (xfade)
    ps->flags |= 1;
  if (reversed)
    ps->flags |= 2;
  //if (???)
  //  ps->flags |= 4;
  //if (???)
  //  ps->flags |= 8;
  //if (???)
  //  ps->flags |= 16;
  //if (???)
  //  ps->flags |= 32;
  //if (???)
  //  ps->flags |= 64;
  //if (???)
  //  ps->flags |= 128;

  // 95-126 (4 DDs) undefined
  ps->undefinedDD_95 = decodeDD((uint8_t*)&samp_parms[95]); // 8=>4
  
  // these two have data (for the S950) but - it is not documented...
  ps->undefinedDD_103 = decodeDD((uint8_t*)&samp_parms[103]); // 8=>4
  ps->undefinedDD_111 = decodeDD((uint8_t*)&samp_parms[111]); // 8=>4
  ps->undefinedDD_119 = decodeDD((uint8_t*)&samp_parms[119]); // 8=>4

  // FROM SAMPLE HEADER...

  // bits per sample-word (S900 transmits 12 but can accept 8-14)
  // S950 can accept 16 but not serially, only from S1000 disk????
  ps->bits_per_word = (uint16_t)samp_hedr[5]; // 4=>2

  // sampling period in nS 15259-500000
  ps->period = decodeTB(&samp_hedr[6]); // 3=>(21-bits)(stored here as 32 bit int)

  // NOW USING VALUES IN samp_parms FOR ITEMS BELOW!!!!!!!!!!!!!!!!!!

  // number of sample words 200-475020
  // ps->sampleCount = decodeTB((uint8_t*)&samp_hedr[9]); // 3

  // loop start point (non-looping mode if >= endidx-5)
  // int startpoint = decodeTB((uint8_t*)&samp_hedr[12]); // 3

  // loop end point (S950/S900 takes this as end point of the sample)
  // int loopend = decodeTB((uint8_t*)&samp_hedr[15]); // 3

  // ps->startidx = startpoint;
  // ps->endidx = loopend;
  // ps->looplen = loopend - startpoint;

  // we directly get this from samp_parms[59] instead!
  // samp_hedr[18] is mode 0=looping, 1=alternating (one-shot if loop-length < MINLOOP)
  //if (ps->endidx-ps->loopidx < MINLOOP)
  //    ps->looping = 'O';
  //else
  //    ps->looping = (samp_hedr[18] & (uint8_t)0x01) : 'A' : 'L';
}
//---------------------------------------------------------------------------
// get a sample's header info into the global m_temp_array
// returns 0 if success -1 if fail
int __fastcall TFormMain::get_comm_samp_hedr(int samp)
{
  // request sample dump (after 25ms delay)
  int mode = RSD;
  if (!cxmit(samp, mode, true))
    return -1; // tx timeout

  // wait for data
  int iError = receive(m_hedr_size);
  if (iError < 0)
  {
    if (iError == -2)
      printm("timeout receiving sample header!");
    return -1;
  }

  // copy first 19 bytes of m_temp_array into samp_hedr buffer
  memcpy(samp_hedr, m_temp_array, m_hedr_size);

  return 0;
}
//---------------------------------------------------------------------------
// receive sample data blocks and write them to a file, print a progress display all functions
// here print their own error messages ao you can just look for a return value of 0 for success.
// returns -1 if error or if 0 bytes received, returns the # bytes written if success
int __fastcall TFormMain::get_samp_data(PSTOR* ps, long lFileHandle, bool bIsWavFile)
{
  // always 120 byte packets (plus block-count and checksum), S900 is max 60 2-byte words in 14-bits
  // and S950 "should be" max 40 3-byte words in 16-bits (but the S950 seems NOT to be able to receive
  // 16-bit samples as it turns out though the manual says it can process 16-bits from disk???)(Confirmed
  // June/2025 S950, like the S900 sends max 12-bits serially! It can receive max 14-bits serially))
  // (The S900/S950 protocol was designed to be compliant with the Prophet 2000 which is supposed to handle
  // 16-bit sample packets as 40 3-byte words or 20 4-byte words... (only the lower 7-bits of each byte is usable))
  //
  // we receive any # bits per word and properly format it as 40 or 60 16-bit 2's compliment sample-words in rbuf
  uint8_t* rbuf = NULL;

  int bytesWritten = 0;

  try
  {
    try
    {
      unsigned count = 0;
      String dots = "";
      int blockct = 0;

      int bits_per_word = ps->bits_per_word;

      int sampler_bytes_per_word = bits_per_word / 7;
      if (bits_per_word % 7)
        sampler_bytes_per_word++;

      if (DATA_PACKET_SIZE % sampler_bytes_per_word)
      {
        printm("can't fit expected samples into 120 byte packets: " +
                              String(sampler_bytes_per_word));
        return -1;
      }

      // will be 2 words (120/60 for 8-14 bit samples) for S900/S950, [but we
      // could also handle 3 words (120/40 15-21 bit samples) or
      // 4 words (120/20 22-28 bit samples)]
      int sampler_words_per_packet = DATA_PACKET_SIZE / sampler_bytes_per_word;

      unsigned total_words = ps->sampleCount;

      int target_bytes_per_word = bits_per_word / 8;
      if (bits_per_word % 8)
        target_bytes_per_word++;
      rbuf = new uint8_t[sampler_words_per_packet*target_bytes_per_word];

      printm("bits per sample-word: " + String(bits_per_word));
      printm("raw bytes per word from sampler: " + String(sampler_bytes_per_word));
      printm("target bytes per word: " + String(target_bytes_per_word));
      printm("words per sampler packet: " + String(sampler_words_per_packet));

      for (;;)
      {
        if (!chandshake(ACKS, blockct))
          break;

        if (count >= ps->sampleCount)
          break;

        // (blockct passed is only used for displaying messages...)
        if (get_data_block(rbuf, sampler_bytes_per_word, target_bytes_per_word,
             sampler_words_per_packet, bits_per_word, blockct, bIsWavFile) < 0)
          return -1;

        if (ps->sampleCount <= 19200)
        {
          if (blockct % 8 == 0)
          {
            dots += ".";
            printm(dots);
          }
        }
        else
        {
          if (blockct % 32 == 0)
          {
            dots += ".";
            printm(dots);
          }
        }

        count += sampler_words_per_packet;
        int writct = sampler_words_per_packet * target_bytes_per_word;

        // write only up to sampleCount words...
        if (count > total_words)
          writct -= (int)(count - total_words) * target_bytes_per_word;

        // write to file (samples in rbuf are little-endian)
        if (FileWrite(lFileHandle, rbuf, writct) < 0)
        {
          printm("error writing sample data to file! (block=" +
                             String(blockct) + ")");
          return -1;
        }

        bytesWritten += writct;

        blockct++;
      }

      if (count < total_words)
      {
        if (count == 0)
        {
          printm("no sample data received!");
          return -1;
        }

        printm("expected " + String(total_words) +
          " bytes, but received " + String(count) + "!");
      }

      // if an odd # bytes was written, must pad with 0
      if (bIsWavFile && (bytesWritten & 1))
      {
        uint8_t temp = 0;
        if (FileWrite(lFileHandle, &temp, 1) < 0)
        {
          printm("error writing padding-byte to file!");
          return -1;
        }
        bytesWritten++;
      }
    }
    catch(...)
    {
      printm("exception thrown in get_samp_data()!");
      return -1;
    }
  }
  __finally
  {
    if (rbuf)
      delete [] rbuf;
  }

  return bytesWritten;
}
//---------------------------------------------------------------------------
// Returns: 0=OK, -1=error
// Rreceive 8-16-bit sample data into 16-bit words
// Converts a block of raw sample-data from the machine in 7-bit midi-format,
// and moves it from m_temp_array into dest. Also validates the checksum.
//
// # words per 120 byte block is generally 60 8-14 bit samples
// or 40 15-21 bit samples... could be 20 22-28 bit samples
int __fastcall TFormMain::get_data_block(uint8_t* dest, int sampler_bytes_per_word, int target_bytes_per_word,
    int sampler_words_per_packet, int bits_per_word, int packet_count, bool bIsWavFile)
{
  int bc = packet_count+1; // display 1 greater...

  try
  {
    // receive data sample data block from serial port
    // and store in m_temp_array
    int iError = receive(m_data_size);
    if (iError < 0)
    {
      if (iError == -2)
      {
        printm("receive timeout at block " + String(bc) + "... expected " +
              String(m_data_size) + " bytes, got " + String(m_rxByteCount));
      }
      return -1;
    }

    uint8_t *cp;
    uint8_t checksum = 0;

    cp = m_temp_array + 1;

    int32_t baseline = (int32_t)(1 << (bits_per_word - 1));

    // process m_temp_array into a data buffer and validate checksum
    // (NOTE: tricky little algorithm [Scott M. Swift] came up with that handles
    // data bytes that come in like the following:
    //
    // ps->bits_per_word == 16 bits
    // byte 1 = 0 d15 d14 d13 d12 d11 d10 d09 (shift_count = 16-7 = 9)
    // byte 2 = 0 d08 d07 d06 d05 d04 d03 d02 (shift_count = 16-14 = 2)
    // byte 3 = 0 d01 d00  0   0   0   0   0  (shift_count = 16-21 = -5)
    //
    // ps->bits_per_word == 14 bits
    // byte 1 = 0 d13 d12 d11 d10 d09 d08 d07 (shift_count = 14-7 = 7)
    // byte 2 = 0 d06 d05 d04 d03 d02 d01 d00 (shift_count = 14-14 = 0)
    //
    // ps->bits_per_word == 12 bits
    // byte 1 = 0 d11 d10 d09 d08 d07 d06 d05 (shift_count = 12-7 = 5)
    // byte 2 = 0 d04 d03 d02 d01 d00  0   0  (shift_count = 12-14 = -2)
    //
    // ps->bits_per_word == 8 bits
    // byte 1 = 0 d07 d06 d05 d04 d03 d02 d01 (shift_count = 8-7 = 1)
    // byte 2 = 0 d00  0   0   0   0   0   0  (shift_count = 8-14 = -6)
    //
    // ...and turns it into an N-bit two's compliment sample-point
    // to store in a wav file.
    for (int ii = 0; ii < sampler_words_per_packet; ii++)
    {
      // Samples sent by S900/S950 are only 12-bits. This algorithm can handle
      // up to 32-bit decoding as-is...
      int32_t acc = 0;

      for (int jj = 1; jj <= sampler_bytes_per_word; jj++)
      {
        int shift_count = bits_per_word - (jj * 7); // (each midi-byte has only 7 bits used)

        uint8_t b = *cp++;
        uint32_t val = (uint32_t)b;
        checksum ^= b;

        uint32_t or_val;
        if (shift_count >= 0)
          or_val = (uint32_t)(val << shift_count);
        else
          or_val = (uint32_t)(val >> -shift_count);

        acc |= or_val;
      }

      if (bits_per_word > 8 || !bIsWavFile)
        acc -= baseline; // convert to two's compliment

      // wav file has data left-shifted within byte-field...
      if (bIsWavFile)
        acc <<= 8-(bits_per_word % 8);

      // write the sample-word in little-endian order for RIFF
      for (int jj = 0; jj < target_bytes_per_word; jj++)
      {
        *dest++ = (uint8_t)acc;
        acc >>= 8;
      }
    }

    if (checksum != *cp)
    {
      printm("bad checksum! (block=" + String(bc) + ")");
      return -1; // bad checksum
    }

    return 0;
  }
  catch(...)
  {
    printm("exception thrown in get_comm_samp_data()!");
    return -1;
  }
}
//---------------------------------------------------------------------------
// Send sample routines
//---------------------------------------------------------------------------
bool __fastcall TFormMain::PutSample(String sFilePath)
{
  // don't allow multiple button presses
  if (IsBusy())
    return false;

  // vars released in __finally block
  uint8_t *fileBuf = NULL;
  uint8_t *tbuf = NULL;
  int iFileHandle = 0;

  try
  {
    this->BusyCount++;;

    try
    {
      ListBox1->Clear();
      Memo1->Clear();

      int iFileLength;
      int iBytesRead;
      int sampIndex;
      PSTOR ps = { 0 };
      uint8_t newname[MAX_NAME_S900 + 1];

      if (sFilePath.IsEmpty() || !FileExists(sFilePath))
      {
        printm("file does not exist!");
        return false;
      }

      // Load file
      printm("file path: \"" + sFilePath + "\"");

      // allow file to be opened for reading by another program at the same time
      iFileHandle = FileOpen(sFilePath, fmShareDenyNone | fmOpenRead);

      if (iFileHandle == 0)
      {
        printm("unable to open file, handle is 0!");
        return false;
      }

      // get file length
      iFileLength = FileSeek(iFileHandle, 0, 2);
      FileSeek(iFileHandle, 0, 0); // back to start

      if (iFileLength == 0)
      {
        FileClose(iFileHandle);
        printm("unable to open file, length is 0!");
        return false;
      }

      fileBuf = new uint8_t[iFileLength + 1];

      if (fileBuf == NULL)
      {
        FileClose(iFileHandle);
        printm("unable to allocate " + String(iFileLength + 1) +
                             " bytes of memory!");
        return false;
      }

      try
      {
        iBytesRead = FileRead(iFileHandle, fileBuf, iFileLength);
        //printm("read " + String(iBytesRead) + " bytes...");
      }
      catch (...)
      {
        printm("unable to read file into buffer...");
        return false;
      }

      // finished with the file...
      FileClose(iFileHandle);
      iFileHandle = 0;

      // copy up to the first 10 chars of file-name without extension
      // into newname and pad the rest with spaces, terminate
      String sName = ExtractFileName(sFilePath);

      String Ext = ExtractFileExt(sName).LowerCase();

      if (Ext.IsEmpty() || (Ext != AKIEXT && Ext != ".wav"))
      {
        ShowMessage("File \"" + sName + "\" must have a\n"
               + String(AKIEXT) + " or .wav file-extension!");
        return false;
      }

      // strip out " (12-bit mono)" if it was added when the wav file was
      // saved from the machine...
      int pos = sName.Pos(WAV_NAME_WARNING);
      if (pos > 1)
      {
        int len = strlen(WAV_NAME_WARNING);
        sName = sName.SubString(1, pos-1) + sName.SubString(pos+len, sName.Length());
      }

      // Form a max 10 char sample name from the file's name
      // (up to the .ext) and pad out with spaces
      int lenName = sName.Length();
      bool bStop = false;
      for (int ii = 1; ii <= MAX_NAME_S900; ii++)
      {
        if (!bStop && (ii > lenName || sName[ii] == '.'))
          bStop = true;

        newname[ii - 1] = bStop ? ' ' : sName[ii];
      }
      // terminate it
      newname[MAX_NAME_S900] = '\0';

      printm("rs232 baud-rate: " + String(m_baud));

      int machine_max_bits_per_word = m_send_as_12_bits ? 12 : MAX_SERIAL_BITS_PER_WORD;

      int sampler_bytes_per_word = machine_max_bits_per_word / 7;

      if (machine_max_bits_per_word % 7)
        sampler_bytes_per_word++;

      // # sample-words is 120/2 = 60 for 8-14 bit samples,
      // 120/3 = 40 for 15-21 bit samples and 120/4 = 20
      // for 22-28 bit samples. there should be no remainder!
      if (DATA_PACKET_SIZE % sampler_bytes_per_word)
      {
        printm("sampler_bytes_per_word of " + String(sampler_bytes_per_word) +
          " does not fit evenly into 120 byte packet!");
        return false;
      }

      int words_per_block = DATA_PACKET_SIZE / sampler_bytes_per_word;

      // transmit buffer (allow for checksum and block number, 122 bytes)
      // (127 bytes if sample-dump standard mode)
      tbuf = new uint8_t[m_data_size];

      // preload the data packet buffer with sample-dump-standard
      // constants if SDS was selected from menu...
      // For SDS, the checksum is the running XOR of all the data after
      // the SYSEX byte, up to but not including the checksum itself.
      // F0 7E cc 02 kk <120 bytes> mm F7
      uint8_t initial_checksum = 0;

      uint8_t *ptbuf; // transmit buffer pointer
      uint8_t checksum;
      int blockct;

      // flag set if we receive a not-acknowledge on any data-packet
      // after PACKET_RETRY_COUNT attempts
      bool bSendAborted;

      m_abort = false; // user can press ESC a key to quit

      if (Ext.IsEmpty() || (Ext != AKIEXT && Ext != ".wav"))
      {
        ShowMessage("File \"" + sName + "\" must have a\n"
               + String(AKIEXT) + " or .wav file-extension!");
        return false;
      }

      if (Ext != AKIEXT) // Not an AKI file? (try WAV...)
      {
        if (iBytesRead < 45)
        {
          printm("bad file (1)");
          return false;
        }

        if (!StrCmpCaseInsens((char*)&fileBuf[0], "RIFF", 4))
        {
          printm("bad file (2) [no \'RIFF\' preamble!]");
          return false;
        }

        int file_size = *(int32_t*)&fileBuf[4];
        if (file_size + 8 != iBytesRead)
        {
          printm("bad file (3), (file_size = " +
            String(file_size + 8) + ", iBytesRead = " +
            String(iBytesRead) + ")!");
          return false;
        }

        if (!StrCmpCaseInsens((char*)&fileBuf[8], "WAVE", 4))
        {
          printm("bad file (4) [no \'WAVE\' preamble!]");
          return false;
        }

        // Search file for "fmt " block
        uint8_t* headerPtr = fileBuf;
        // NOTE: the FindSubsection will return headerPtr by reference!
        int32_t headerSize = FindSubsection(headerPtr, "fmt ", iBytesRead);
        if (headerSize < 0)
        {
          printm("bad file (4) [no \'fmt \' sub-section!]");
          return false;
        }

        // Length of the format data in bytes is a four-byte int at
        // offset 16. It should be at least 16 bytes of sample-info...
        if (headerSize < 16)
        {
          printm("bad file (6) [\'fmt \' sub-section is < 16 bytes!]");
          return false;
        }

        // Search file for "data" block
        // (Left-channel data typically begins after the header
        // at 12 + 4 + 4 + 16 + 4 + 4 - BUT - could be anywhere...)
        uint8_t* dataPtr = fileBuf;
        // NOTE: the FindSubsection will return dataPtr by reference!
        int32_t dataLength = FindSubsection(dataPtr, "data", iBytesRead);
        if (dataLength < 0)
        {
          printm("bad file (4) [no \'data\' sub-section!]");
          return false;
        }

        // NOTE: Metadata tags will appear at the end of the file,
        // after the data. You will see "LIST". I've also seen
        // "JUNK" and "smpl", etc.

        // Bytes of data following "data"
        ///*** Data must end on an even byte boundary!!! (explains
        // why the file may appear 1 byte over-size for 8-bit mono)
        printm("data-section length (in bytes): " + String(dataLength));

        int AudioFormat = *(int16_t*)(&headerPtr[0]);
        if (AudioFormat != 1) // we can't handle compressed WAV-files
        {
          printm("cannot read this WAV file-type " +
                        String(AudioFormat) + "!");
          return false;
        }

        int NumChannels = *(int16_t*)(&headerPtr[2]);
        printm("number of channels: " + String(NumChannels));

        int SampleRate = *(int32_t*)(&headerPtr[4]);
        printm("sample rate: " + String(SampleRate));

        // BytesPerFrame: Number of bytes per frame
        int BytesPerFrame = *(int16_t*)(&headerPtr[12]);
        printm("bytes per frame: " + String(BytesPerFrame));

        int BitsPerWord = *(int16_t*)(&headerPtr[14]);
        printm("bits per sample: " + String(BitsPerWord));
        if (BitsPerWord < 8 || BitsPerWord > 64)
        {
          printm("bits per sample out of range: " + String(BitsPerWord));
          return false;
        }

        int BytesPerWord = BitsPerWord / 8;
        if (BitsPerWord % 8) // remaining bits?
          BytesPerWord++; // round up

        if (BytesPerWord > 8)
        {
          printm("error: can't handle samples over 64-bits!");
          return false;
        }

        if (NumChannels * BytesPerWord != BytesPerFrame)
        {
          printm("error: (NumChannels * BytesPerWord != BytesPerFrame)");
          return false;
        }

        // there should be no "remainder" bytes...
        if (dataLength % (NumChannels * BytesPerWord))
        {
          printm("error: incomplete data-block!");
          return false;
        }

        printm("bytes per sample: " + String(BytesPerWord));

        int TotalFrames = dataLength / (NumChannels * BytesPerWord);
        printm("number of frames: " + String(TotalFrames));

        // make sure we have a file-length that accomodates the expected data-length!
        if (dataPtr + dataLength > fileBuf + iBytesRead)
        {
          printm("error: [dataPtr+dataLength > fileBuf+iBytesRead!]");
          return false;
        }

        // Need to populate a PSTOR structure for S950
        
        //---------- set spare/reserved/undefined fields (they default to 0 above...) --------------
        // clear spare-byte fields
        //ps.spareDB1 = 0; // 1 byte
        //ps.spareDW1 = 0; // 2 bytes
        // these fields are reserved/undefined in the S900, unknown for the S950
        //ps.undefinedDW_35 = 0;
        //ps.reservedDB_61 = 0;
        //ps.reservedDW_87 = 0;
        //ps.undefinedDD_27 = 0;
        //ps.undefinedDD_95 = 0;
        //ps.undefinedDD_103 = 0;
        //ps.undefinedDD_111 = 0; // 0x80000000 1/2 semitone (50 cents) (semitone pitch for what?)
        //ps.undefinedDD_119 = 0; // 1 (fine pitch for what?)
        //------------------------------------------------------------------------------------------
        
        ps.startpoint = 0; // first replay point for looping (4 bytes)
        ps.endpoint = TotalFrames; // end of play index (4 bytes)
        ps.looplen = ps.endpoint; // loop length (4 bytes)

        uint32_t tempfreq = SampleRate;
        if (tempfreq > 49999)
          tempfreq = 49999;
        ps.freq = (uint16_t)tempfreq; // sample freq. in Hz (4 bytes)

        ps.pitch = 960; // pitch - units = 1/16 semitone (4 bytes) (middle C)
        ps.sampleCount = TotalFrames; // total number of words in sample (4 bytes)
        ps.period = 1.0e9 / (double)ps.freq; // sample period in nanoseconds (8 bytes)

        // 8-14 bits S900 or 8-16-bits S950
        // (this will be the bits-per-word of the sample residing on the machine)
        // (DO THIS BEFORE SETTING shift_count!)
        ps.bits_per_word =
          (uint16_t)((BitsPerWord > machine_max_bits_per_word) ?
            machine_max_bits_per_word : BitsPerWord);

        // positive result is the amount of right shift needed (if any)
        // to down-convert the wav's # bits to the desired # bits
        // (example: Wave-file's BitsPerWord is 16 and ps.bits_per_word
        // max is 14 bits for S900, result is "need to right-shift 2")
        // (DO THIS AFTER LIMITING ps.bits_per_word TO machine_max_bits_per_word!)
        int shift_count = BitsPerWord - ps.bits_per_word;

        AsciiStrCpyTrimRight(ps.name, newname);

        ps.loudnessoffset = 0;

        // set alternating/reverse flags "normal"
        ps.flags = 0;

        ps.loopmode = 'O'; // (1 byte) (one-shot)

        // Search file for "cue " block
        uint8_t* tempPtr = fileBuf;
        // NOTE: the FindSubsection will return tempPtr by reference!
        // cueChunkSize does not include the first 8 bytes!
        int32_t cueChunkSize = FindSubsection(tempPtr, "cue ", iBytesRead);
        // we allow a CUECHUNK with just one CUEPOINT, hence subtracting off
        // one of the 2 CUEPOINT structs defined in CUECHUNK
        if (cueChunkSize >= (int32_t)(sizeof(CUECHUNK)-sizeof(CUEPOINT)-8))
        {
          CUECHUNK* pCue = (CUECHUNK*)(tempPtr-8); // obtain a pointer to the struct

          if (pCue->cuePointsCount > 0)
          {
            // do this before start-point to get ps.endpoint
            if (pCue->cuePointsCount > 1)
            {
              printm("(found " + String(pCue->cuePointsCount) + " cue-points)");

              uint32_t iEnd = pCue->cuePoints[pCue->cuePointsCount-1].frameOffset;

              if (iEnd <= ps.sampleCount)
              {
                printm("cue-point 2 (end): " + String(iEnd));
                ps.endpoint = iEnd;
              }
            }
            else
              printm("(found 1 cue-point)");

            // this uses ps.endpoint to do limit-checking... so do after endpoint
            uint32_t iStart = pCue->cuePoints[0].frameOffset;

            if (iStart <= ps.sampleCount && iStart <= ps.endpoint)
            {
              printm("cue-point 1 (start): " + String(iStart));
              ps.startpoint = iStart;
            }
          }
        }

        // Search file for "smpl" block
        tempPtr = fileBuf;
        // NOTE: the FindSubsection will return tempPtr by reference!
        // smplChunkSize does not include the first 8 bytes!
        int32_t smplChunkSize = FindSubsection(tempPtr, "smpl", iBytesRead);

        // we will look at the data if it's got at least the regular chunk info + one loop...
        if (smplChunkSize >= (int32_t)(sizeof(SMPLCHUNKHEDR)+sizeof(SMPLLOOP)-8))
        {
          SMPLCHUNKHEDR* pSmpl = (SMPLCHUNKHEDR*)(tempPtr-8); // obtain a pointer to the struct

          // the fraction of a semitone up from the specified MIDIUnityNote
          // 0x80000000 = 1/2 semitone (50 cents)
          ps.pitch = (pSmpl->MIDIUnityNote*STEPSPERSEMITONE) +
                (pSmpl->MIDIPitchFraction/((unsigned)0x80000000/(STEPSPERSEMITONE/2)));

          // unused...
          //pSmpl->Manufacturer;
          //pSmpl->Product;
          //pSmpl->SamplePeriod;
          //pSmpl->SMPTEFormat
          //pSmpl->SMPTEOffset

          // is there at least one loop following the normal "smpl" chunk?
          if (pSmpl->loopCount >= 1 && pSmpl->samplerDataSize >= sizeof(SMPLLOOP))
          {
            // get a pointer to the first loop-point
            SMPLLOOP* pLoop = (SMPLLOOP*)(&pSmpl->samplerDataSize + sizeof(uint32_t));

            uint32_t iStart = pLoop->start;

            if (iStart > ps.sampleCount)
              iStart = ps.sampleCount;

            uint32_t iEnd = pLoop->end;

            if (iEnd > ps.sampleCount)
              iEnd = ps.sampleCount;

            if (pSmpl->loopCount == 1)
              printm("(found 1 loop)");
            else
              printm("(found " + String(pSmpl->loopCount) + " loops (we only use the first one))");
             printm("loop start: " + String(iStart) + ", loop end: " + String(iEnd));

            // swap if reversed
            if (iStart > iEnd)
            {
              uint32_t iTemp = iStart;
              iStart = iEnd;
              iEnd = iTemp;
            }

            uint32_t iLoopLen = (iEnd > iStart) ? iEnd-iStart : 0;

            // 5-point minimum on S900 loop-length...
            if (iLoopLen >= MINLOOP && ps.sampleCount >= MINLOOP && iLoopLen <= ps.sampleCount)
            {
              ps.looplen = iLoopLen;
              // 0=forward, 1=alternate, 2=reverse (for the loop, not the sample)
              // S9xx can't do reverse loop (not sure if S950 has an 'R'????)
              ps.loopmode = (pLoop->type == 1) ? 'A' : 'L';
              // fraction is meaningless for us... pSmpl->loopPoints[0].fraction
              // playCount is meaningless for us... pSmpl->loopPoints[0].playCount
            }

            // now let's see if there's a SMPLDATA struct following the loop(s)...
            if (pSmpl->samplerDataSize == (pSmpl->loopCount*sizeof(SMPLLOOP)) +
                                                                  sizeof(SMPLDATA))
            {
              // additional check
              if (smplChunkSize == (int32_t)(sizeof(SMPLCHUNKHEDR)+pSmpl->samplerDataSize-8))
              {
                SMPLDATA* pSmplData = (SMPLDATA*)((uint8_t*)pSmpl +
                        sizeof(SMPLCHUNKHEDR) + (pSmpl->loopCount*sizeof(SMPLLOOP)));

                if (pSmplData->magic == MAGIC_NUM_AKI)
                {
                  printm("detected custom S900/S950 data embedded in WAV!");

                  ps.flags = pSmplData->flags;
                  ps.loudnessoffset = pSmplData->loudnessoffset;
                }
              }
            }
          }
        }

        // find the sample's index on the target machine
        if ((sampIndex = FindIndex(ps.name)) < 0)
          return false;

        // encode samp_hedr and samp_parms arrays
        encode_sample_info(sampIndex, &ps);

        print_ps_info(&ps);

         // if Stereo and User checked "send right channel"
         // point to first right-channel sample-datum
        if (NumChannels > 1 && m_use_right_chan)
          dataPtr += BytesPerWord;

        int64_t wav_baseline = (1L << (BitsPerWord - 1L));
        int64_t target_baseline = (1L << (ps.bits_per_word - 1L));
        int64_t ob_target_max = (1L << ps.bits_per_word) - 1L;
        int64_t signed_target_max = target_baseline - 1;
        int64_t signed_target_min = -signed_target_max;

        int64_t my_max = signed_target_min;
        int64_t my_min = signed_target_max;

        // the largest magnitude (pos or neg) point in this wav
        // (needed to compute a scale-factor to map the wav to
        // the target # bits)
        uint64_t maxAbsAcc = 0;

        // init progress display
        int divisor = (ps.sampleCount <= 19200) ? 8 : 32;
        String dots;

        bool bUseScaling = (BytesPerWord < 2) ? false : true;
        int passes = bUseScaling ? 2 : 1;

        // Make two passes... first to get max absolute value of the original
        // WAV sample-point to compute a scaling factor to normalize to 14-bits.
        while (passes > 0)
        {
          blockct = 0;

          // when passes == 1, that's the "really send the sample" pass...
          // (as opposed to merely  finding the max for the scale-factor)
          if (passes == 1)
          {
            if (bUseScaling)
            {
              double scale_factor = (double)signed_target_max / (double)maxAbsAcc;
              printm("\r\nscale factor (targ max/file max): " +
                String(signed_target_max) + "/" + String(maxAbsAcc) + " = " +
                      Format("%.3f\r\n", ARRAYOFCONST((scale_factor))));
            }

            // request common reception enable (after 25ms delay)
            if (!exmit(0, SECRE, true))
              return false; // tx timeout

            DelayGpTimer(25);

            // transmit sample header info
            if (!comws(m_hedr_size, samp_hedr, false))
              return false;

            // wait for acknowledge
            // (for midi - the data sent is initially buffered by
            // between 229 and 247 bytes so we CANNOT get
            // synchronized ack-packets! [Unless it is SDS])
            if (get_ack(0) < 0)
              return false;
          }
          else
            printm("computing largest absolute sample value (wait...)");

          int FrameCounter = 0;
          uint8_t* dp = dataPtr;

          // for each frame in WAV-file (one-sample of 1-8 bytes...)
          for (;;)
          {
            // End of file?
            if (FrameCounter >= TotalFrames)
              break;

            // read and encode block of 20, 30 or 60 frames (samples)...

            // Strategy: encode WAV samples of 8-64 bits
            // as unsigned 14 (or 16-bit) (two-byte S950 "SW" format)
            // S900 is 12-bits but can receive 14-bits.
            // 8-bit wav-format is already un-signed, but
            // over 8-bits we need to convert from signed to
            // unsigned.  All values need to be converted into
            // a 14-bit form for S950...

            // For both S900 and S950, the data are encoded into
            // 2 bytes per sample - 60 samples per 120 byte block.

            // (For the Prophet 2000 data are encoded into 3 bytes per
            // sample - 40 samples per 120 byte block. And this format
            // also handles 30 samples encoded 4-bytes * 30 = 120,
            // but that capability is unrealized for the S900/S950)

            checksum = initial_checksum; // init checksum

            if (passes == 1)
            {
              // reset transmit buffer pointer
              tbuf[0] = (uint8_t)(blockct & 0x7f); // queue the block #
              ptbuf = &tbuf[1];
            }

            register int64_t acc; // sample accumulator (must be signed!)

            for (int ii = 0; ii < words_per_block; ii++)
            {
              if (FrameCounter >= TotalFrames)
                acc = 0;
              else
              {
                // one-byte wave samples are un-signed by default
                if (BytesPerWord < 2) // one byte?
                {
                  acc = *dp;

                  if (passes == 2)
                    if (acc > maxAbsAcc)
                      maxAbsAcc = acc;
                }
                else // between 2-7 bytes per sample (signed)
                {
                  // Allowed BitsPerWord => 9-56
                  // Stored as MSB then LSB in "dp" buffer.
                  // Left-justified...
                  //
                  // From the WAV file. RIFF is little-endian (RIFX is
                  // big-endian) bit-field is left-shifted within # bytes
                  // per sample:
                  //
                  // example (24-bits):
                  // byte *dp    : D07 D06 D05 D04 D03 D02 D01 D00
                  // byte *(dp+1): D15 D14 D13 D12 D11 D10 D09 D08
                  // byte *(dp+2): D23 D22 D21 D20 D19 D18 D17 D16
                  //
                  // example (16-bits):
                  // byte *dp    : D07 D06 D05 D04 D03 D02 D01 D00
                  // byte *(dp+1): D15 D14 D13 D12 D11 D10 D09 D08
                  //
                  // example (9-bits):
                  // byte *dp    : D00  0   0   0   0   0   0   0
                  // byte *(dp+1): D08 D07 D06 D05 D04 D03 D02 D01
                  //
                  // example (12-bits):
                  // byte *dp    : D03 D02 D01 D00   0   0   0   0
                  // byte *(dp+1): D11 D10 D09 D08 D07 D06 D05 D04

                  // init 64-bit acc with -1 (all 1) if msb of
                  // most-signifigant byte is 1 (negative value)
                  if (dp[BytesPerWord - 1] & 0x80)
                    acc = -1; // set all 1s
                  else
                    acc = 0;

                  // load accumulator with 8-64 bit sample
                  // (Microsoft WAV sample-data are in Intel little-endian
                  // byte-order. left-channel sample appears first for a
                  // stereo WAV file, then the right-channel.)
                  for (int ii = BytesPerWord - 1; ii >= 0; ii--)
                  {
                    acc <<= 8; // zero-out new space and shift previous
                    acc |= dp[ii];
                  }

                  // right-justify so we can add baseline
                  // (the bits in a wav are left-justified
                  // so we must scoot them to the right!)
                  //
                  // NOTE: since acc is signed, the sign-bit
                  // is preserved during the right-shift!
                  acc >>= (8 * BytesPerWord) - BitsPerWord;

                  if (passes == 2)
                  {
                    // find our max +/- sample value
                    if (acc < 0)
                    {
                      if (-acc > maxAbsAcc)
                        maxAbsAcc = -acc;
                    }
                    else
                    {
                      if (acc > maxAbsAcc)
                        maxAbsAcc = acc;
                    }
                  }
                  else // here, we have computed a scale_factor!
                  {
                    if (bUseScaling)
                    {
                      acc *= signed_target_max;
                      double rem = fmod((double)acc, (double)maxAbsAcc);
                      acc = (double)acc / (double)maxAbsAcc;

                      if (rem >= 0.5)
                        acc++;
                      else if (rem <= -0.5)
                        acc--;

                      // convert from 2's compliment to offset-binary
                      acc += signed_target_max;

                      if (acc > my_max)
                        my_max = acc;
                      if (acc < my_min)
                        my_min = acc;
                    }
                    else
                    {
                      // convert from 2's compliment to offset-binary:
                      acc += wav_baseline;

                      // convert down to 14 or 16-bits if over...
                      if (shift_count > 0)
                      {
                        // shift msb of discarded bits to lsb of val
                        acc >>= shift_count - 1;

                        bool bRoundUp = acc & 1; // need to round up?

                        // discard msb of discarded bits...
                        acc >>= 1;

                        if (bRoundUp && acc != ob_target_max)
                          acc++;
                      }
                    }
                  }
                }
              }

              // save sample in 122-byte tbuf
              if (passes == 1)
                queue((int32_t)acc, ptbuf, sampler_bytes_per_word,
                  ps.bits_per_word, checksum); // returns ptbuf by-reference!

              dp += BytesPerFrame; // Next frame
              FrameCounter++;
            }

            // user can press ESC to quit
            Application->ProcessMessages();

            if (m_abort)
            {
              printm("user aborted transmit!");
              return false;
            }

            if (passes == 1)
            {
              *ptbuf++ = checksum; // queue checksum

              if (ptbuf - tbuf != m_data_size)
                printm("detected tbuf overrun (should never happen!)");

              bSendAborted = send_packet(tbuf, blockct);

              if (bSendAborted)
              {
                // sample send failed!
                unsigned countSent = blockct*words_per_block;
                printm("sent " + String(countSent) + " of " +
                  String(ps.sampleCount) + " sample words!");

                // limits
                if (countSent < ps.sampleCount)
                {
                  ps.sampleCount = countSent;
                  ps.endpoint = ps.sampleCount;
                  ps.looplen = ps.endpoint;
                  ps.startpoint = 0;

                  // re-encode modified values
                  encode_sample_info(sampIndex, &ps);
                  printm("sample length was truncated!");
                }

                break; // break out of sample transmit loop
              }

              // do the ..... progress indicator
              if (blockct % divisor == 0)
              {
                dots += ".";
                printm(dots);
              }
            }

            blockct++;
          }

          // print max and min if computing scale-factor and we have
          // applied it in this, second (and last) pass - just before
          // we exit...
          if (bUseScaling && passes == 1)
            printm("target_max: " + String(ob_target_max) +
                  ", scaled-max: " + String(my_max) +
                  ", scaled-min: " + String(my_min));

          passes--;

        } // end while
      }
      else // .AKI file (my custom format)
      {
        // File format: (little-endian storage format, LSB then MSB)
        // 1) 4 byte unsigned, magic number to identify type of file
        // 2) 72 byte PSTOR struct with sample's info
        // 3) At offset 20 into PSTOR is the 4-byte number of sample-words
        // 4) X sample words in 16-bit 2's compliment little endian format
        // Offset 32 has a 16-bit int with the # bits per sample-word.

        // min size is one 60-byte block of data plus the magic-number and
        // PSTOR (program info) array

        if (iBytesRead < (int)(DATA_PACKET_SIZE +
                    AKI_FILE_HEADER_SIZE + UINT32SIZE))
        {
          printm("file is corrupt");
          return false;
        }

        printm("Read " + String(iBytesRead) + " bytes");

        uint32_t my_magic;

        memcpy(&my_magic, &fileBuf[0], UINT32SIZE);

        //printm("aki-file magic #: " + String(my_magic));
        if (my_magic != MAGIC_NUM_AKI)
        {
          printm("File is not the right kind!");
          return false;
        }

        // load ps (the sample-header info) from fileBuffer
        memcpy(&ps, &fileBuf[0 + UINT32SIZE], AKI_FILE_HEADER_SIZE);

        // terminate and right-trim the name field
        ps.name[MAX_NAME_S900] = '\0';
        trimright(ps.name);

        // get the +/- shift_count before we limit ps.bits_per_word (below)
        // (should either be 16-16 = 0, 14-14 = 0, 14-16 = -2 or 16-14 = +2
        // the only case we care about is +2 (or any positive number) because
        // we will have to down-convert by right-shifting 2 to target an S900
        // with a 16-bit sample saved from an S950)
        int shift_count = ps.bits_per_word - machine_max_bits_per_word;

        // 2 bytes 14-bits S900 or 3 bytes 16-bits S950
        // If the .aki file has 16-bit samples and we are sending to
        // an S900, we need to lower the PSTOR bits_per_word to 14!
        if (ps.bits_per_word > machine_max_bits_per_word)
        {
          printm("reducing bits per word in " + String(AKIEXT) +
           " file from " + String(ps.bits_per_word) + " to " +
            String(machine_max_bits_per_word) + "!");
          ps.bits_per_word = (uint16_t)machine_max_bits_per_word;
        }

        // find the sample's index on the target machine
        if ((sampIndex = FindIndex(ps.name)) < 0)
          return false;

        // encode samp_hedr and samp_parms arrays
        encode_sample_info(sampIndex, &ps);

        print_ps_info(&ps);

        // request common reception enable (after 25ms delay)
        if (!exmit(0, SECRE, true))
          return false; // tx timeout

        DelayGpTimer(25);

        // transmit sample header info
        if (!comws(m_hedr_size, samp_hedr, false))
          return false;

        // wait for acknowledge
        // (for midi - the data sent is initially buffered by
        // between 229 and 247 bytes so we CANNOT get
        // synchronized ack-packets!)
        if (get_ack(0) < 0)
          return false;

        int32_t baseline = (int16_t)(1 << (ps.bits_per_word - 1));

        uint16_t max_val = (uint16_t)((1 << ps.bits_per_word) - 1);

        uint8_t* ptr = (uint8_t*)&fileBuf[AKI_FILE_HEADER_SIZE + UINT32SIZE];
        int ReadCounter = AKI_FILE_HEADER_SIZE + UINT32SIZE; // We already processed the header

        blockct = 0;

        // init progress display
        int divisor = (ps.sampleCount <= 19200) ? 8 : 32;
        String dots;

        int file_bytes_per_word = ps.bits_per_word/8;
        if (ps.bits_per_word % 8)
          file_bytes_per_word++;

        for (;;)
        {
          // End of file?
          if (ReadCounter >= iBytesRead)
            break;

          checksum = initial_checksum; // init checksum

          // reset transmit buffer pointer
          tbuf[0] = (uint8_t)(blockct & 0x7f); // queue the block #
          ptbuf = &tbuf[1];

          // Send data and checksum (send 0 if at end-of-file to pad
          // out the data packet to 120 bytes!)
          // Read block of 120 bytes (40 16-bit sample words or
          // 60 14-bit sample words) from fileBuffer...
          // pad last block with 0's if end of file
          for (int ii = 0; ii < words_per_block; ii++)
          {
            register int32_t acc = 0;
            if (ReadCounter < iBytesRead)
            {
              // read little-endian bytes_per_word bytes of sample-word
              for (int jj = file_bytes_per_word-1; jj >= 0; jj--)
              {
                acc <<= 8;
                acc |= ptr[jj];
              }
              // convert back to full-range rather than two's compliment
              acc += baseline;
            }

            // positive shift_count means the sample in the file has
            // more bits in each sample-word than the S900/S950 can accept
            // (14-bits max) - we should do a nice, two-pass scaling (like
            // we do above if it's a wav file), but for now, we just
            // shift out the superfluous bits, rounding up... in truth,
            // an aki file should never have any samples in it over 14 bits
            // but you never know the future!
            if (shift_count > 0)
            {
              // shift msb of discarded bits to lsb of val
              acc >>= shift_count - 1;

              bool bRoundUp = acc & 1; // need to round up?

              // discard msb of discarded bits...
              acc >>= 1;

              if (bRoundUp && acc != max_val)
                acc++;
            }

            // convert acc (sample-word) into Akai S900/S950 raw transmit format
            // and store in tbuf for later transmission
            queue(acc, ptbuf, sampler_bytes_per_word,
                  ps.bits_per_word, checksum); // returns ptbuf by-reference!

            ReadCounter += file_bytes_per_word;
            ptr += file_bytes_per_word; // point to next sample-word
          }

          // user can press ESC to quit
          Application->ProcessMessages();

          if (m_abort)
          {
            printm("user aborted transmit!");
            return false;
          }

          *ptbuf++ = checksum; // queue checksum

          if (ptbuf - tbuf != m_data_size)
            printm("detected tbuf overrun (should never happen!)");

          bSendAborted = send_packet(tbuf, blockct);

          if (bSendAborted)
          {
            // sample send failed!
            unsigned countSent = blockct*words_per_block;
            printm("sent " + String(countSent) + " of " +
              String(ps.sampleCount) + " sample words!");

            // limits
            if (countSent < ps.sampleCount)
            {
              ps.sampleCount = countSent;
              ps.endpoint = ps.sampleCount;
              ps.looplen = ps.endpoint;
              ps.startpoint = 0;

              // re-encode modified values
              encode_sample_info(sampIndex, &ps);
              printm("sample length was truncated!");
            }

            break; // break out of sample transmit loop
          }

          // do the ..... progress indicator
          if (blockct % divisor == 0)
          {
            dots += ".";
            printm(dots);
          }

          blockct++;
        }
      }

      // write the EEX (no delay)
      uint8_t temp = EEX;
      if (!comws(1, &temp, false))
        return false;

      // do renaming unless NAK packet was received while in midi Tx
      // (in that case we have no way of knowing exactly which packet
      // failed so we can't change samp_parms correctly... oh well...)
      if (m_auto_rename && !bSendAborted)
      {
        // look up new sample in catalog, when you write a new sample it
        // shows up as "00", "01", "02"

        uint8_t locstr[3];
        sprintf(locstr, "%02d", sampIndex);

        sampIndex = findidx(locstr);

        // returns the 0-based sample index if a match is found
        // -1 = error
        // -2 = no samples on machine
        // -3 = samples on machine, but no match
        if (sampIndex == -1)
        {
          printm("catalog search error for: \"" + String((char*)locstr) + "\"");
          return false; // catalog error
        }

        if (sampIndex == -2)
          sampIndex = 0; // we will be the only sample...

        if (sampIndex < 0)
        {
          printm("index string \"" + String((char*)locstr) + "\" not found!");
          return false;
        }

        send_samp_parms(sampIndex); // write samp_parms to midi or com port

        if (!bSendAborted)
          printm("sample written ok! (index=" + String(sampIndex) + ")");
      }
      else
      {
        if (!bSendAborted)
          printm("sample written ok!");
      }
    }
    catch (...)
    {
      printm("unable to process file: \"" + sFilePath + "\"");
      return false;
    }
  }
  __finally
  {
    // request common reception disable (after 25ms delay)
    exmit(0, SECRD, true);

    if (fileBuf)
      delete[] fileBuf;

    if (tbuf)
      delete[] tbuf;

    if (iFileHandle != 0)
      FileClose(iFileHandle);

    this->BusyCount--;
  }

  return true; // success
}
//---------------------------------------------------------------------------
// return true if send fails
bool __fastcall TFormMain::send_packet(uint8_t *tbuf, int blockct)
{
  for(int ii = 0; ii < PACKET_RETRY_COUNT; ii++)
  {
    // write the data block (no delay)
    if (!comws(m_data_size, tbuf, false))
      return true;

    // wait for acknowledge
    if (get_ack(blockct) == 0)
      return false; // success

    ApdComPort1->FlushOutBuffer();
    ApdComPort1->FlushInBuffer();
    if (ii == 0)
      printm("retrying " + String(PACKET_RETRY_COUNT) + " times...");
    DelayGpTimer(25); // delay and try again
  }

  return true; // failed
}
//---------------------------------------------------------------------------
// prev = dcRemoval(signal, prev.w, 0.9);
//double __fastcall TFormMain::dcRemoval(double input, double &prevHistory, double alpha)
//{
//    double newHistory = input + alpha*prevHistory;
//    double output = newHistory - prevHistory;
//    prevHistory = newHistory; // return by-reference
//    return output;
//}
//---------------------------------------------------------------------------
// Tricky algorithm I came up with (the inverse of the one in RxSamp.cpp)
// builds and sends only the required number of 7-bit bytes representing one
// sample-word to the S900/S950 - S.S.
//
// Checksum in/out is by-reference as is the ptbuf pointer
// Algorithm handles 32-bit samples as-is but we will only ever feed it
// 14-bits (how sad!)
void __fastcall TFormMain::queue(int32_t acc, uint8_t* &ptbuf,
         int sampler_bytes_per_word, int bits_per_word, uint8_t &checksum)
{
  for (int ii = 1; ii <= sampler_bytes_per_word; ii++)
  {
    int shift_count = bits_per_word - (ii * 7);

    uint8_t out_val;

    if (shift_count >= 0)
      out_val = (uint8_t)(acc >> shift_count);
    else
      out_val = (uint8_t)(acc << -shift_count);

    out_val &= 0x7f; // mask msb to 0

    // update reference in/out vars
    checksum ^= out_val;
    *ptbuf++ = out_val;
  }
}
//---------------------------------------------------------------------------
// puts PSTOR struct (ps) info into samp_parms[] and samp_hedr[] arrays
void __fastcall TFormMain::encode_sample_info(uint16_t index, PSTOR* ps)
{
  //
  // SAMPLE PARAMETERS 129 bytes (do this before encoding header)
  //

  // Clear array to all 0
  memset(samp_parms, 0, PARMSIZ);

  samp_parms[0] = BEX;
  samp_parms[1] = AKAI_ID;
  samp_parms[2] = 0; // midi chan
  samp_parms[3] = SPRM;
  samp_parms[4] = S900_ID;
  samp_parms[5] = (uint8_t)(index & 0x7f);
  samp_parms[6] = (uint8_t)((index>>7) & 0x7f); // reserved

  // copy ASCII sample name, pad with blanks and terminate
  AsciiStrEncode(&samp_parms[7], ps->name); // 20

  // undefined, 27-38, 12 bytes
  encodeDD(ps->undefinedDD_27, &samp_parms[27]); // 27-34  DD Undefined
  encodeDW(ps->undefinedDW_35, &samp_parms[35]); // 35-38  DW Undefined

  // number of sample words
  encodeDD(ps->sampleCount, &samp_parms[39]); // 8

  // sampling frequency
  encodeDW(ps->freq, &samp_parms[47]); // 4

  // pitch
  encodeDW(ps->pitch, &samp_parms[51]); // 4

  // loudness offset
  encodeDW(ps->loudnessoffset, &samp_parms[55]); // 4

  // replay mode 'A', 'L' or 'O' (alternating, looping or one-shot)
  // (samp_hedr[18] also has sample's loop mode: 0=looping, 1=alternating
  // or one-shot if loop-length < MINLOOP)
  encodeDB(ps->loopmode, &samp_parms[59]); // 2

  encodeDB(ps->reservedDB_61, &samp_parms[61]); // reserved 2

  // end point relative to start of sample
  encodeDD(ps->endpoint, &samp_parms[63]); // 8

  // loop start point
  encodeDD(ps->startpoint, &samp_parms[71]); // 8

  // limit loop length (needed below when setting samp_hedr[15])
  //if (ps->looplen > ps->endpoint+1)
  //  ps->looplen = ps->endpoint+1;
  if (ps->looplen > ps->endpoint)
    ps->looplen = ps->endpoint;

  // length of looping or alternating part (default is 45 for TONE sample)
  encodeDD(ps->looplen, &samp_parms[79]); // 8

  encodeDW(ps->reservedDW_87, &samp_parms[87]); // reserved 4

  // 91,92 DB type of sample 0=normal, 255=velocity crossfade
  uint8_t c_temp = (uint8_t)((ps->flags & 1) ? 255 : 0);
  encodeDB(c_temp, &samp_parms[91]);

  // 93,94 DB waveform type 'N'=normal, 'R'=reversed
  c_temp = (ps->flags & 2) ? 'R' : 'N';
  encodeDB(c_temp, &samp_parms[93]); // 2

  // 95-102, 1 DDs Undefined, 8 bytes (no space to save in PSTOR so putting in member variables)
  encodeDD(ps->undefinedDD_95, &samp_parms[95]); // 95-102 DD Undefined (8 bytes)

  // 103-126, 3 DDs Undefined, 24 bytes (saving in PSTOR)
  encodeDD(ps->undefinedDD_103, &samp_parms[103]); // 119-126 DD Undefined (8 bytes)
  encodeDD(ps->undefinedDD_111, &samp_parms[111]); // 111-118 DD Undefined (8 bytes)
  encodeDD(ps->undefinedDD_119, &samp_parms[119]); // 119-126 DD Undefined (8 bytes)

  // checksum is exclusive or of 7-126 (120 bytes)
  // and the value is put in index 127
  compute_checksum(7, 127, &samp_parms[0]);

  samp_parms[128] = EEX;

  //
  // SAMPLE HEADER, 19 bytes (21 bytes for sample-dump-standard (SDS))
  //

  // Clear array to all 0
  memset(samp_hedr, 0, HEDRSIZ);

  // encode excl, syscomid, sampdump
  samp_hedr[0] = BEX;
  samp_hedr[1] = SYSTEM_COMMON_NONREALTIME_ID;
  int idx = 2;
  samp_hedr[idx++] = SD;
  samp_hedr[idx++] = (uint8_t)(index & 0x7f);
  samp_hedr[idx++] = (uint8_t)((index>>7) & 0x7f); // MSB samp idx always 0 for S900

  // bits per word
  samp_hedr[idx++] = (uint8_t)ps->bits_per_word;

  // sampling period
  encodeTB(ps->period, &samp_hedr[idx]); // 3
  idx += 3;

  // number of sample words
  encodeTB(ps->sampleCount, &samp_hedr[idx]); // 3
  idx += 3;

  // loop start point
  //encodeTB(ps->endpoint-ps->looplen+1, &samp_hedr[idx]); // 3
  encodeTB(ps->endpoint-ps->looplen, &samp_hedr[idx]); // 3
  idx += 3;

  // loop end, S950 takes this as the end point
  encodeTB(ps->endpoint, &samp_hedr[idx]); // 3
  idx += 3;

  // use ps->looping mode 'A', 'L' or 'O' (alternating, looping or one-shot)
  // to set samp_hedr[18], loop mode: 0=looping, 1=alternating
  // NOTE: one-shot if loop-length is smaller than 5
  samp_hedr[idx] = (uint8_t)((ps->loopmode == 'A') ? 1 : 0);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::send_samp_parms(unsigned index)
{
  samp_parms[5] = (uint8_t)(index & 0x7f);
  samp_parms[6] = (uint8_t)((index>>7) & 0x7f);

  // transmit sample parameters (after 25ms delay)
  if (!comws(PARMSIZ, samp_parms, true))
    return;
}
//---------------------------------------------------------------------------
// Catalog routines
//---------------------------------------------------------------------------
bool __fastcall TFormMain::catalog(bool print)
{
  // 7 hedr bytes + chksum byte + EEX = 9...
  if (m_rxByteCount < 9 || m_temp_array[3] != DCAT)
    return false;

  int entries = (m_rxByteCount - 9) / sizeof(S950CAT);

  if (!entries)
  {
    printm("no Samples or programs in S950!");
    return true;
  }

  S950CAT *tempptr = (S950CAT *)&m_temp_array[7]; // Skip header
  CAT *permsampptr = PermSampArray;
  CAT *permprogptr = PermProgArray;

  m_numSampEntries = 0;
  m_numProgEntries = 0;

  for (int ii = 0; ii < entries; ii++)
  {
    if (tempptr->type == 'S')
    {
      if (m_numSampEntries < MAX_SAMPS)
      {
        AsciiStrCpyTrimRight(permsampptr->name, tempptr->name);

        permsampptr->index = tempptr->index;

        m_numSampEntries++; // increment counter
        permsampptr++; // next structure
      }
    }
    else if (tempptr->type == 'P')
    {
      if (m_numProgEntries < MAX_PROGS)
      {
        AsciiStrCpyTrimRight(permprogptr->name, tempptr->name);

        permprogptr->index = tempptr->index;

        m_numProgEntries++; // increment counter
        permprogptr++; // next structure
      }
    }

    tempptr++; // next structure
  }

  if (print)
  {
    printm("Programs:");

    for (int ii = 0; ii < m_numProgEntries; ii++)
      printm(String(PermProgArray[ii].index) + ":\"" +
        String(PermProgArray[ii].name) + "\"");

    printm("Samples:");

    for (int ii = 0; ii < m_numSampEntries; ii++)
      printm(String(PermSampArray[ii].index) + ":\"" +
        String(PermSampArray[ii].name) + "\"");
  }

  return true;
}
//---------------------------------------------------------------------------
// Receive data routines
//---------------------------------------------------------------------------
int __fastcall TFormMain::receive(int count)
// set count 0 to receive a complete message (EEX required)
// set count -1 to receive a partial message of unknown size
// set "count" to return either when a specific # bytes is received
//  or when EEX is received
//
// returns 0 if success, -1 if failure message printed, -2 if timeout
{
  uint8_t tempc;
  bool have_bex = false;

  m_rxByteCount = 0;

  m_abort = false;
  m_inBufferFull = false;

  try
  {
    // add timeout seconds for slower baud-rate...
    int iTimeoutTime = m_baud < 19200 ? TXTIMEOUT+2 : TXTIMEOUT;

    // wait until all but last 3 bytes sent
    if (WaitTxCom(iTimeoutTime, 3) < 0)
    {
      printm("unable to verify empty transmit buffer (receive())...");
      return false;
    }

    StartElapsedSecondsTimer();

    for (;;)
    {
      if (m_abort)
      {
        printm("receive aborted by user!");
        return -1;
      }

      if (ApdComPort1->CharReady())
      {
        if (m_rxByteCount >= TEMPARRAYSIZ) // at buffer capacity... error
        {
          m_inBufferFull = true;
          printm("receive buffer overrun!");
          return -1;
        }

        // keep ressetting timer to hold off timeout
        ResetElapsedSecondsTimer();

        tempc = ApdComPort1->GetChar();

        if (!have_bex)
        {
          if (tempc == BEX || count != 0)
          {
            have_bex = true;
            m_temp_array[m_rxByteCount++] = tempc;
          }
          else
          {
            printm("expected BEX but got: 0x" + IntToHex((int)tempc, 2));
            printm("machine seems to be sending to us as a master... cycle its power and try again!");
            return -1;
          }
        }
        else
        {
          m_temp_array[m_rxByteCount++] = tempc;

          if (tempc == EEX ||
              (count > 0 && m_rxByteCount >= (uint32_t)count))
            return 0;
        }
        ApdComPort1->ProcessCommunications(); // keep the serial communications library running...
      }
      else
      {
        Application->ProcessMessages(); // need this to detect the timeout

        if (IsTimeElapsed(iTimeoutTime))
        {
          // return whatever we have if count == -1
          if (count == -1 && m_rxByteCount > 0)
            return 0;

          break;
        }
      }
    }
  }
  __finally
  {
    StopElapsedSecondsTimer();
  }

  return -2; // timeout
}
//---------------------------------------------------------------------------
// Misc methods
//---------------------------------------------------------------------------
int __fastcall TFormMain::FindIndex(uint8_t* pName)
{
  // now we need to set "samp index"
  int sampIndex = findidx(pName);

  // retval = the 0-based sample index if a match is found
  // -1 = error
  // -2 = no samples on machine
  // -3 = samples on machine, but no match
  if (sampIndex == -1)
  {
    printm("catalog search error for: \"" + String((char*)pName).TrimRight() + "\"");
    printm("check RS232 connection and/or baud-rate: " + String(m_baud));
  }
  else if (sampIndex >= 0) // sample we are about to write is already on machine
    printm("found sample \"" + String((char*)pName).TrimRight() + "\" at index " + String(sampIndex));
  else if (sampIndex == -3) // samples on machine, but not the one we are about to write
    sampIndex = m_numSampEntries;
  else // no samples on machine
    sampIndex = 0;

  return sampIndex;
}
//---------------------------------------------------------------------------
int __fastcall TFormMain::findidx(uint8_t* sampName)
// returns the 0-based sample index if a match is found
// -1 = catalog error
// -2 = no samples on machine
// -3 = samples on machine, but no match
//
// ignores spaces to right of name in comparison
{
  try
  {
    // Request S950 Catalog (no printout, delay)
    if (GetCatalog(false, true) < 0)
      return -1; // catalog error

    if (m_numSampEntries == 0)
      return -2; // no samples on machine

    // make a right-trimmed copy of name
    uint8_t newName[MAX_NAME_S900 + 1];

    AsciiStrCpyTrimRight(newName, sampName);

    for (int ii = 0; ii < m_numSampEntries; ii++)
      if (bytewisecompare(PermSampArray[ii].name, newName, MAX_NAME_S900)) // names match?
        return PermSampArray[ii].index; // found match!
  }
  catch(...)
  {
    return -1; // exception
  }

  return -3; // samples on machine, but no match
}
//---------------------------------------------------------------------------
// return true if match
bool __fastcall TFormMain::bytewisecompare(uint8_t* buf1, uint8_t* buf2, int maxLen)
{
  for (int ii = 0; ii < maxLen; ii++)
  {
    if (buf1[ii] != buf2[ii])
      return false; // not the same!

    if (buf1[ii] == 0)
      break;
  }

  return true; // byte-wise compare the same!
}
//---------------------------------------------------------------------------
// Return count of files selected
int __fastcall TFormMain::GetFileNames(void)
{
  OpenDialog1->Title = "Send .wav " + String(AKIEXT) + " .prg file(s) to S900/S950";
  OpenDialog1->DefaultExt = "aki";
  OpenDialog1->Filter = "Akai files (*" + String(AKIEXT) + ")|*" +
              String(AKIEXT) + "|Wav files (*.wav)|*.wav|"
            "Prg file (*.prg)|*.prg|" "All files (*.*)|*.*";
  OpenDialog1->FilterIndex = 4; // start the dialog showing all files
  OpenDialog1->Options.Clear();
  OpenDialog1->Options << ofHideReadOnly << ofPathMustExist <<
          ofFileMustExist << ofEnableSizing << ofAllowMultiSelect;

  if (!OpenDialog1->Execute())
    return 0; // Cancel

  int count = OpenDialog1->Files->Count;

  m_slFilePaths->Clear();

  if (count)
    m_slFilePaths->Assign(OpenDialog1->Files);

  return count;
}
//---------------------------------------------------------------------------
// Search file for named chunk
//
// Returns:
// -2 = not found
// -3 = exception thrown
// 0-N length of chunk
//
// Returns a byte-pointer to the start of data in the chunk
// as a reference: fileBuffer
//
// On entry, set fileBuffer to the start of the file-buffer
int32_t __fastcall TFormMain::FindSubsection(uint8_t* &fileBuffer, char* chunkName, UINT fileLength)
{
  try
  {
    // bypass the first 12-bytes "RIFFnnnnWAVE" at the file's beginning...
    uint8_t* chunkPtr = fileBuffer + 12;

    uint8_t* pMax = fileBuffer + fileLength;

    uint32_t chunkLength;

    // Search file for named chunk
    for (;;)
    {
      // Chunks are 4-bytes ANSI followed by a 4-byte length, lsb-to-msb
      if (chunkPtr + 8 >= pMax)
        return -2;

      // chunkLength does not include the first 8 bytes of the chunk!
      chunkLength = *(uint32_t*)(chunkPtr + 4);

      // WAV file chuncks are on even-byte boundaries. if the # bytes is
      // odd, a padding byte should have been added that's not included
      // in the length!
      bool bOddLength = (chunkLength & 1);

      // look for the 4-byte chunk-name
      if (StrCmpCaseInsens((char*)chunkPtr, chunkName, 4))
      {
        fileBuffer = chunkPtr + 8; // return pointer to data, by reference
        return chunkLength;
      }

      chunkPtr += chunkLength + 8;

      if (bOddLength)
        chunkPtr++; // add 1 if there's a padding byte
    }
  }
  catch (...)
  {
    return -3;
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MenuPutSampleClick(TObject *Sender)
{
  if (IsBusy())
    return;

  // don't set/ clear busy flag here because PutSample() in PutFiles() does it!

  // put list of file-paths in m_slFilePaths
  GetFileNames();

  // send them to machine
  PutFiles();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MenuGetCatalogClick(TObject *Sender)
{
  if (IsBusy())
    return;

  try
  {
    this->BusyCount++;

    ListBox1->Clear();
    Memo1->Clear();

    int iError = GetCatalog(true);

    if (iError == -2)
      printm("transmit timeout requesting catalog!");
    else if (iError == -3)
      printm("timeout receiving catalog!");
    else if (iError == -4)
      printm("incorrect catalog received!");
  }
  __finally
  {
    this->BusyCount--;
  }

  return;
}
//---------------------------------------------------------------------------
// This fills the list-box (at the far left) with sample-names, you then
// click a name to receive that particular sample from the Akai.
void __fastcall TFormMain::MenuGetSampleClick(TObject *Sender)
{
  if (IsBusy())
    return;

  ListBox1->Clear();
  Memo1->Clear();

  try
  {
    this->BusyCount++;

    try
    {
      // Request S950 Catalog (no printout)
      int iError = GetCatalog();

      if (iError < 0)
      {
        if (iError == -2)
          printm("transmit timeout requesting catalog!");
        else if (iError == -3)
          printm("timeout receiving catalog!");
        else if (iError == -4)
          printm("incorrect catalog received!");
        return;
      }

      if (!m_numSampEntries)
      {
        printm("no samples in machine!");
        return;
      }

      // (we save the actual sample index in the object-field...)
      for (int ii = 0; ii < m_numSampEntries; ii++)
        ListBox1->Items->AddObject(PermSampArray[ii].name,
                    (TObject*)PermSampArray[ii].index);

      printm("\r\n<--- ***Click a sample at the left to receive\r\n"
        "and save it to a .AKI or .WAV file***");
    }
    catch (...)
    {
      ShowMessage("Can't get catalog!");
    }
  }
  __finally
  {
    this->BusyCount--;
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ListBox1Click(TObject *Sender)
{
  if (IsBusy())
    return;

  try
  {
    this->BusyCount++;

    String sFile = ListBox1->Items->Strings[ListBox1->ItemIndex];
    if (DoSaveDialog(sFile)) // get new, complete file-path back in sFile...
    {
      ListBox1->Repaint();

      // we stored the actual index from the catalog in the object-field
      int index = (int)ListBox1->Items->Objects[ListBox1->ItemIndex];

      int iError = GetSample(sFile, index);

      if (iError < 0)
        printm("not able to save sample! (code=" + String(iError) + ")");
      else
        printm("sample saved as: \"" + sFile + "\"");
    }
  }
  __finally
  {
    this->BusyCount--;
  }
}
//---------------------------------------------------------------------------
// pass in sName of a simple file-name like "MyFile1"
// returns, by-reference, the full file-path
bool __fastcall TFormMain::DoSaveDialog(String &sName)
{
  try
  {
    SaveDialog1->Title = "Save a sample as .wav or " + String(AKIEXT) + " file...";
    SaveDialog1->DefaultExt = "wav";
    SaveDialog1->Filter = "Akai files (*" + String(AKIEXT) + ")|*" +
                        String(AKIEXT) + "|WAV files (*.wav)|*.wav" +
                                      "|All files (*.*)|*.*";
    SaveDialog1->FilterIndex = 3; // start the dialog showing all files
    SaveDialog1->Options.Clear();
    SaveDialog1->Options << ofHideReadOnly
      << ofPathMustExist << ofOverwritePrompt << ofEnableSizing
      << ofNoReadOnlyReturn;

    // Use the sample-name in the list as the file-name
    // add WAV_NAME_WARNING for a .wav file - we then strip it off
    // files sent to the machine
    SaveDialog1->FileName = sName.TrimRight() + WAV_NAME_WARNING;

    if (SaveDialog1->Execute())
    {
      // Assign the full file name to reference var sName!)
      sName = SaveDialog1->FileName;

      return true;
    }
  }
  catch (...)
  {
    printm("error, can't save file: \"" + SaveDialog1->FileName + "\"");
  }
  sName = "";
  return false;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MenuAboutClick(TObject *Sender)
{
  String hText =
    "Connect a quality USB to RS232 cable from your computer to "
    "the DB-25 female connector on the Akai S950/S900 sampler.\r\n\r\n"
    "Select your desired baud rate (38400 is the default).\r\n"
    "On the S950/S900 push the MIDI button.\r\n"
    "Push the DOWN button and scroll to menu 5.\r\n"
    "Push the RIGHT button and select \"2\" control by RS232.\r\n"
    "Push the RIGHT button again and enter \"3840\".\r\n"
    "This will set the machine to 38400 baud\r\n"
    "(\"5760\" will show 55555 and set 57600 baud).\r\n\r\n"
    "To test, select \"Get list of samples and programs\" from "
    "the menu. A box may appear asking for the com port. Select "
    "a port (usually COM1) and click OK. You should see the samples "
    "and programs listed in this window.\r\n\r\n"
    "Author: Scott Swift dxzl@live.com\r\n"
    "Testing: Oliver Tann optann@gmail.com\r\n\r\n"
    "Happy sampling!";

  Height = FORM_HEIGHT; // allow help to fit
  Memo1->Lines->Clear();
  Memo1->Lines->Add(hText);
//    ShowMessage(hText + " - Cheers, Scott Swift dxzl@live.com");
//    Clipboard()->AsText = hText;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::SetMenuItems(void)
{
  m_data_size = DATA_PACKET_SIZE + DATA_PACKET_OVERHEAD;
  m_hedr_size = HEDRSIZ;
  MenuGetCatalog->Enabled = true;
  MenuGetPrograms->Enabled = true;
  MenuAutomaticallyRenameSample->Enabled = true;
  ListBox1->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MenuUseRightChanForStereoSamplesClick(TObject *Sender)
{
  MenuUseRightChanForStereoSamples->Checked = !MenuUseRightChanForStereoSamples->Checked;
  m_use_right_chan = MenuUseRightChanForStereoSamples->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MenuAutomaticallyRenameSampleClick(
  TObject *Sender)
{
  MenuAutomaticallyRenameSample->Checked = !MenuAutomaticallyRenameSample->Checked;
  m_auto_rename = MenuAutomaticallyRenameSample->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MenuMax12BitsPerSampleClick(TObject *Sender)
{
  // Toggle checked state on click
  MenuMax12BitsPerSample->Checked = !MenuMax12BitsPerSample->Checked;
  m_send_as_12_bits = MenuMax12BitsPerSample->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MenuUseHWFlowControlBelow50000BaudClick(
  TObject *Sender)
{
  MenuUseHWFlowControlBelow50000Baud->Checked = !MenuUseHWFlowControlBelow50000Baud->Checked;
  m_force_hwflow = MenuUseHWFlowControlBelow50000Baud->Checked;

  // reset port if below 50000
  if (m_baud < 50000)
    SetComPort(m_baud);
}
//---------------------------------------------------------------------------
// custom .prg file-format:
// magic number 639681490             4 byte int32_t
// number of programs                 4 byte int32_t
//
// next is a variable number of programs:
// number of bytes in program N       4 byte int32_t
// program N data including BEX, checksum and EEX
// (each program has a 83 byte header including BEX, then a variable number of
// keygroups, then a checksum byte and an EEX)
// (each keygroup is 140 bytes)
// (the number of keygroups in a program is a DB at header index 53, 54)
//
// this file's size in bytes          4 byte int32_t
void __fastcall TFormMain::MenuGetProgramsClick(TObject *Sender)
{
  // don't allow multiple button presses
  if (IsBusy())
    return;

  try
  {
    this->BusyCount++;

    ListBox1->Clear();
    Memo1->Clear();

    SaveDialog1->Title = "Save all programs to .pgm file...";
    SaveDialog1->DefaultExt = "pgm";
    SaveDialog1->Filter = "Programs (*.pgm)|*.pgm|"
      "All files (*.*)|*.*";
    SaveDialog1->FilterIndex = 2; // start the dialog showing all files
    SaveDialog1->Options.Clear();
    SaveDialog1->Options << ofHideReadOnly
      << ofPathMustExist << ofOverwritePrompt << ofEnableSizing
      << ofNoReadOnlyReturn;

    SaveDialog1->FileName = String(DEFSAVENAME) + String(".prg");

    if (!SaveDialog1->Execute())
      return;

    String sFilePath = SaveDialog1->FileName.TrimRight();

    int iNumProgs = GetPrograms(sFilePath);

    if (iNumProgs > 0)
      printm(String(iNumProgs) + " programs successfully saved!");
    else
    {
      if (iNumProgs != -1) // already printed message?
        printm("unable to save programs... (code: " + String(iNumProgs) + ")");

      try { DeleteFile(sFilePath); }
      catch (...) {}
    }
  }
  __finally
  {
    this->BusyCount--;
  }
}
//---------------------------------------------------------------------------
// Retrieves all programs from Akai sampler and stores them in a .prg file
// Returns the number of programs saved or a negative error-code
int __fastcall TFormMain::GetPrograms(String sFilePath)
{
  int iFileHandle;

  if (FileExists(sFilePath) == 0)
    iFileHandle = FileCreate(sFilePath);
  else
    iFileHandle = FileOpen(sFilePath, fmShareDenyNone | fmOpenReadWrite);

  if (iFileHandle == 0)
    return -2;

  m_numProgEntries = 0; // need this in __finally

  try
  {
    try
    {
      // Request S950 Catalog (no printout)
      int iError = GetCatalog();

      if (iError < 0)
      {
        if (iError == -2)
          printm("transmit timeout requesting catalog!");
        else if (iError == -3)
          printm("timeout receiving catalog!");
        else if (iError == -4)
          printm("incorrect catalog received!");
        else
          printm("catalog error!");
        return -1;
      }

      if (!m_numProgEntries)
        return -5;

      printm("reading " + String(m_numProgEntries) +
                      " programs from S950/S900...");

      int32_t totalBytesWritten = 0;
      int32_t bytesWritten;

      // write the magic number
      uint32_t my_magic = MAGIC_NUM_PRG; // uniquely identifies a .prg file
      totalBytesWritten += UINT32SIZE;
      bytesWritten = FileWrite(iFileHandle, &my_magic, UINT32SIZE);

      if (bytesWritten != UINT32SIZE)
        return -6;

      // write the number of programs
      totalBytesWritten += UINT32SIZE;
      bytesWritten = FileWrite(iFileHandle, &m_numProgEntries, UINT32SIZE);

      if (bytesWritten != UINT32SIZE)
        return -7;

      String dots;

      // request each program and add it to file on the fly
      for (int ii = 0; ii < m_numProgEntries; ii++)
      {
        int progSize = LoadProgramToTempArray(PermProgArray[ii].index);

        if (progSize == -1)
          return -1; // already printed error
        else if (progSize < 2+PROGHEADERSIZ+PROGKEYGROUPSIZ)
          return -8;

        // write program-size (including BEX, checksum and EEX) to file.
        // (this helps when we read a .prg file back, to seperate
        // the individual programs, buffer them and transmit to the
        // machine)
        totalBytesWritten += UINT32SIZE;
        bytesWritten = FileWrite(iFileHandle, &progSize, UINT32SIZE);

        if (bytesWritten != UINT32SIZE)
          return -9;

        // copy program in m_temp_array to file
        totalBytesWritten += progSize;
        bytesWritten = FileWrite(iFileHandle, m_temp_array, progSize);

        if (bytesWritten != progSize)
          return -10;

        dots += '.';
        printm(dots);
      }

      // lastly, write the file's total size
      totalBytesWritten += UINT32SIZE;
      bytesWritten = FileWrite(iFileHandle, &totalBytesWritten, UINT32SIZE);

      if (bytesWritten != UINT32SIZE)
        return -11;
    }
    __finally
    {
      if (iFileHandle)
        try {FileClose(iFileHandle);} catch(...){}
    }

    return m_numProgEntries;
  }
  catch (...)
  {
    return -100;
  }
}
//---------------------------------------------------------------------------
// Returns program-size or negative error-code
int __fastcall TFormMain::LoadProgramToTempArray(int progIndex)
{
  try
  {
    // request next program and its keygroups (after 25ms delay)
    // (clears m_rxByteCount before PutLong, if midi)
    if (!exmit(progIndex, RPRGM, true))
    {
      printm("transmit timeout!");
      return -1;
    }

    // receive entire program into m_temp_array
    int iError = receive(0);
    if (iError < 0)
    {
      if (iError == -2)
        printm("receive timeout!");

      return -1;
    }

    // size of this program
    int32_t progSize = m_rxByteCount;

    if (progSize < PROGHEADERSIZ + PROGKEYGROUPSIZ + 2)
    {
      printm("insufficient bytes received: " + String(progSize));
      return -1;
    }

    if (m_temp_array[3] != PRGM || m_temp_array[4] != S900_ID)
    {
      printm("invalid programs header! (1)");
      return -1;
    }

    if (m_temp_array[5] != progIndex)
    {
      m_temp_array[5] = progIndex;
// this happens all the time - the machine always sends 0 for program #
// so we just have to write-in a value that will work when we send this
// file back to the machine...
//                    printm("forcing offset 5 (program index) to be " +
//                                        String(progIndex) + "\r\n" +
//                    "(received index was " + String((int)m_temp_array[5]) + ")");
    }

    if (m_temp_array[progSize - 1] != EEX)
    {
      printm("expected EEX, got: " + String((int)m_temp_array[progSize-1]));
      return -1;
    }

    // compute checksum
    uint8_t checksum = 0;
    for (int jj = 7; jj < progSize - 2; jj++)
      checksum ^= m_temp_array[jj];

    if (m_temp_array[progSize - 2] != checksum)
    {
      printm("bad checksum for program " + String(progIndex) + "!");
      return -1;
    }

    // get number of keygroups in one program (1-31)
    int numKeygroups = decodeDB(&m_temp_array[53]); // 53, 54
    if (numKeygroups == 1)
      printm("program " + String(progIndex) + " has " +
              String(numKeygroups) + " keygroup...");
    else
      printm("program " + String(progIndex) + " has " +
              String(numKeygroups) + " keygroups...");

    return progSize;
  }
  catch(...)
  {
    return -100;
  }
}
//---------------------------------------------------------------------------
// custom .prg file-format:
// magic number 639681490             4 byte int32_t
// number of programs                 4 byte int32_t
//
// next is a variable number of programs:
// number of bytes in program N       4 byte int32_t
// program N data including BEX, checksum and EEX
// (each program has a 83 byte header including BEX, then a variable number of
// keygroups, then a checksum byte and an EEX)
// (each keygroup is 140 bytes)
// (the number of keygroups in a program is a DB at header index 53, 54)
//
// this file's size in bytes          4 byte int32_t
void __fastcall TFormMain::PutPrograms(String sFilePath)
{
  // don't allow multiple button presses
  if (IsBusy())
    return;

  long lFileHandle = FileOpen(sFilePath, fmShareDenyNone | fmOpenRead);

  if (lFileHandle == 0)
  {
    printm("can't open file to read: \"" + sFilePath + "\"");
    return;
  }

  bool bError = false;
  uint8_t* buf = NULL;
  uint32_t numProgs = 0; // need this in __finally

  try
  {
    this->BusyCount++;

    try
    {
      // one way transfer, no incoming handshake
      ApdComPort1->FlushOutBuffer();

      uint32_t iFileLength = FileSeek(lFileHandle, 0, 2); // seek to end

      // seek/read the stored file-length (int32_t at end of the file)
      FileSeek(lFileHandle, (int)(iFileLength - UINT32SIZE), 0);
      uint32_t storedFileLength;
      uint32_t bytesRead = FileRead(lFileHandle, &storedFileLength, UINT32SIZE);

      if (bytesRead != UINT32SIZE)
      {
        printm("file is corrupt or not the right kind...");
        bError = true;
        return;
      }

      if (storedFileLength != iFileLength)
      {
        printm("file is either corrupt or not a valid .prg file!");
        bError = true;
        return;
      }

      FileSeek(lFileHandle, 0, 0); // back to start

      // read the magic number
      uint32_t my_magic; // uniquely identifies a .prg file
      bytesRead = FileRead(lFileHandle, &my_magic, UINT32SIZE);

      if (bytesRead != UINT32SIZE)
      {
        printm("file is corrupt or not the right kind...");
        bError = true;
        return;
      }

      if (my_magic != MAGIC_NUM_PRG)
      {
        printm("file is not a .prg programs file for the S950/S900!");
        bError = true;
        return;
      }

      // read the number of programs
      bytesRead = FileRead(lFileHandle, &numProgs, UINT32SIZE);

      if (bytesRead != UINT32SIZE)
      {
        printm("file is corrupt or not the right kind...");
        bError = true;
        return;
      }

      if (numProgs == 0)
      {
        printm("no programs in file!");
        bError = true;
        return;
      }

      printm("sending " + String(numProgs) + " programs to S950/S900...");

      // allocate memory for largest program with up to 64 keygroups
      // a single program has PRGHEADERSIZ + (X*PROGKEYGROUPSIZ) + 2 for
      // checksum and EEX bytes
      uint32_t bufSize = PROGHEADERSIZ + (MAX_KEYGROUPS*PROGKEYGROUPSIZ) + 2;
      buf = new uint8_t[bufSize];

      if (buf == NULL)
      {
        printm("can't allocate buffer for program and keygroups!");
        bError = true;
        return;
      }

      String dots; // progress display

             // request each program and add it to file on the fly
      for (uint32_t ii = 0; ii < numProgs; ii++)
      {
        Application->ProcessMessages();

        // read program-size from file.
        // a program in the file is already formatted for the S950 and
        // includes the BEX, checksum and EEX.
        uint32_t progSize;
        bytesRead = FileRead(lFileHandle, &progSize, UINT32SIZE);

        if (bytesRead != UINT32SIZE)
        {
          printm("file is corrupt or not the right kind...");
          bError = true;
          return;
        }

        if (progSize > bufSize)
        {
          printm("error, progSize > bufSize at index: " + String(ii));
          bError = true;
          return;
        }

        // copy program in file to buf
        bytesRead = FileRead(lFileHandle, buf, progSize);

        if (bytesRead != progSize)
        {
          printm("file is corrupt... (index=" + String(ii) + ")");
          bError = true;
          return;
        }

        if (buf[0] != BEX)
        {
          printm("buffer does not contain BEX! (index=" + String(ii) + ")");
          bError = true;
          return;
        }

        if (buf[progSize - 1] != EEX)
        {
          printm("buffer does not contain EEX! (index=" + String(ii) + ")");
          bError = true;
          return;
        }

        // We want to resequence the program numbers since they
        // can be anything in the file, due to user deleting or
        // adding programs... (offset 5 in the header)
        buf[5] = ii;

        // Delay for target machine's processing...
        DelayGpTimer(DELAY_BETWEEN_EACH_PROGRAM_TX);

        // send program to Akai S950/S900
        if (!comws(progSize, buf, false))
        {
          printm("transmit error!");
          bError = true;
          return;
        }

        dots += '.';
        printm(dots);
      }
    }
    catch(...)
    {
      printm("exception thrown while sending programs!");
      bError = true;
    }
  }
  __finally
  {
    if (lFileHandle)
      FileClose(lFileHandle);

    if (buf != NULL)
      delete[] buf;

    if (!bError)
      printm(String(numProgs) + " programs successfully sent!");
    else
      printm("unable to write programs...");

    this->BusyCount--;
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::WMDropFile(TWMDropFiles &Msg)
{
  // don't process these drag-drop files until previous ones are complete
  if (!m_slFilePaths || m_slFilePaths->Count)
    return;

  wchar_t* wBuf = NULL;

  try
  {
    //get dropped files count
    int cnt = ::DragQueryFileW((HDROP)Msg.Drop, -1, NULL, 0);

    wBuf = new wchar_t[MAX_PATH];

    for (int ii = 0; ii < cnt; ii++)
    {
      // Get next file-name
      if (::DragQueryFileW((HDROP)Msg.Drop, ii, wBuf, MAX_PATH) > 0)
      {
        // Load and convert file as per the file-type (either plain or rich text)
        WideString wFile(wBuf);

        // don't process this drag-drop until previous one sets m_DragDropFilePath = ""
        if (!wFile.IsEmpty())
        {
          String sFile = String(wFile); // convert to utf-8 internal string

          if (FileExists(sFile))
            m_slFilePaths->Add(sFile);
        }
      }
    }

    // kick off file processing...
    if (m_slFilePaths && m_slFilePaths->Count)
    {
      Timer1->Enabled = false; // stop timer (just in-case!)
      Timer1->Interval = 50;
      Timer1->OnTimer = Timer1FileDropTimeout; // set handler
      Timer1->Enabled = true; // fire event to send file
    }
  }
  __finally
  {
    try { if (wBuf != NULL) delete [] wBuf; } catch(...) {}
  }
}
//---------------------------------------------------------------------------
// Process the files drag/dropped and saved in m_slFilePaths
void __fastcall TFormMain::Timer1FileDropTimeout(TObject *Sender)
{
  Timer1->Enabled = false;
  PutFiles();
}
//---------------------------------------------------------------------------
// send m_slFilePaths list of .prg or .aki or .wav files
void __fastcall TFormMain::PutFiles(void)
{
  if (!m_slFilePaths || !m_slFilePaths->Count) return;

  int count = m_slFilePaths->Count;

  // flag to print warning if more than one programs file
  int progFileCounter = 0;

  String sPrgFilePath;

  for (int ii = 0; ii < count; ii++)
  {
    String sFile = m_slFilePaths->Strings[ii];

    String Ext = ExtractFileExt(sFile).LowerCase();

    if (Ext == ".prg")
    {
      sPrgFilePath = sFile;
      progFileCounter++;
    }
    else // .wav or .aki
    {
      if (!PutSample(sFile))
        break;
    }
  }

  m_slFilePaths->Clear();

  // save the .prg file for last
  if (!sPrgFilePath.IsEmpty())
  {
    PutPrograms(sPrgFilePath);

    if (progFileCounter > 1)
    ShowMessage("Warning: You chose " + String(progFileCounter) +
        " .prg files! Only one should be chosen...\n"
        "Sent programs file \"" + sPrgFilePath + "\"\n"
        "Each .prg file contains multiple Akai programs.");
  }
}
//---------------------------------------------------------------------------
// chan defaults to 0 (midi channel 1)
bool __fastcall TFormMain::exmit(int samp, int mode, bool bDelay)
{
  uint8_t midistr[8];

  midistr[0] = BEX;
  midistr[1] = AKAI_ID;
  midistr[2] = 0; // midi channel
  midistr[3] = (uint8_t)mode;
  midistr[4] = S900_ID;
  midistr[5] = (uint8_t)samp;
  midistr[6] = 0;
  midistr[7] = EEX;

  return comws(8, midistr, bDelay);
}
//---------------------------------------------------------------------------
// SDS: F0 7E cc 03 ss ss F7
bool __fastcall TFormMain::cxmit(int samp, int mode, bool bDelay)
{
  uint8_t midistr[7];

  midistr[0] = BEX;
  midistr[1] = SYSTEM_COMMON_NONREALTIME_ID;
  midistr[2] = (uint8_t)mode;
  midistr[3] = (uint8_t)samp; // lsb samp
  midistr[4] = (uint8_t)(samp >> 8); // msb samp
  midistr[5] = EEX;

  return comws(6, midistr, bDelay);
}
//---------------------------------------------------------------------------
// SDS: F0 7E cc 7F pp F7
// blockct is only used for sample-dump standard to ACK the block just received
bool __fastcall TFormMain::chandshake(int mode, int blockct)
{
  uint8_t midistr[ACKSIZ];

  midistr[0] = BEX;
  midistr[1] = SYSTEM_COMMON_NONREALTIME_ID;
  midistr[2] = (uint8_t)mode;
  midistr[ACKSIZ-1] = EEX;

  return comws(ACKSIZ, midistr, false); // no delay
}
//---------------------------------------------------------------------------
// opens midi In/Out automatically or flushes RS232 In/Out buffers
// returns true if success, false if fail
bool __fastcall TFormMain::comws(int count, uint8_t* ptr, bool bDelay)
{
  // this delay is necessary to give the older-technology S900/S950 time
  // to digest any previous commands such as sending a program, a sample
  // data-block, request sysex on, catalog, etc. Without it, all programs
  // may not get assimilated by the target-machine (for certain!)
  if (bDelay)
    DelayGpTimer(25); // add 25ms between transmits

  try
  {
    // add timeout seconds for slower baud-rate...
    int iTimeoutTime = m_baud < 19200 ? TXTIMEOUT+2 : TXTIMEOUT;

    // there should not be any bytes in output buffer when we call comws,
    // wait for tx to complete if there are...
    if (WaitTxCom(iTimeoutTime) < 0)
    {
      printm("unable to verify empty transmit buffer...");
      return false;
    }

    // we never expect received data unless we transmit something first...
    // the S950 is always a slave.
    // (# bytes currently available for reading > 0 ? flush)
    if (ApdComPort1->InBuffUsed > 0)
      ApdComPort1->FlushInBuffer();

    // if out data block fits, add it all to the com-driver's buffer
    if (ApdComPort1->OutBuffFree >= count)
      ApdComPort1->PutBlock(ptr, count);
    else // send byte-by-byte
    {
      StartElapsedSecondsTimer();

      for (int ii = 0; ii < count; ii++)
      {
        // wait for at least 1 byte available in the output buffer (OutBuffFree)
        while (ApdComPort1->OutBuffFree < 1)
        {
          Application->ProcessMessages();

          if (IsTimeElapsed(iTimeoutTime))
          {
            printm("rs232 transmit timeout!");
            return false;
          }
        }
        // add one char to the com-driver's buffer
        ApdComPort1->PutChar(*ptr++);
        ApdComPort1->ProcessCommunications();

        // reset timeout
        ResetElapsedSecondsTimer();
      }
    }
  }
  __finally
  {
    StopElapsedSecondsTimer();
  }

  return true;
}
//---------------------------------------------------------------------------
// returns 0 if acknowledge received ok
// returns -1 if error was printed
int __fastcall TFormMain::get_ack(int blockct)
{
  int iError = receive(0);
  if (iError < 0)
  {
    if (iError == -2)
      printm("timeout receiving acknowledge (ACK) at data-block " + String(blockct));

    return -1;
  }

  if (m_rxByteCount == (unsigned int)m_ack_size)
  {
    int idx = 2; // point to the ACKS/NAKS/ASD
    uint8_t c = m_temp_array[idx];

    if (c == ACKS)
      return 0; // ok!

    if (c == NAKS)
    {
      printm("packet \"not-acknowledge\" (NAK) received! memory full? (block=" + String(blockct) + ")");
      return -1;
    }

    if (c == ASD)
    {
      printm("packet \"abort sample dump\" (ASD) received! memory full? (block=" + String(blockct) + ")");
      return -1;
    }

    if (c == PSD)
    {
      printm("packet \"pause sample dump\" (PSD) received! memory full? (block=" + String(blockct) + ")");
      return -1;
    }

    printm("bad acknowledge, unknown code! (block=" + String(blockct) + ", code=" + String((uint32_t)c) + ")");
    return -1;
  }

  printm("bad acknowledge, wrong size! (block=" + String(blockct) + ", size=" + String(m_rxByteCount) + ")");
  return -1;
}
//---------------------------------------------------------------------------
// event hook
// this timer increments a one-second timer var that is
// used for Rx/Tx timeouts
void __fastcall TFormMain::Timer1OneSecTimeout(TObject *Sender)
{
  m_elapsedSeconds++;
}
//---------------------------------------------------------------------------
// event hook
void __fastcall TFormMain::Timer1GpTimeout(TObject *Sender)
{
  // used for midi-diagnostic function
  Timer1->Enabled = false;
  m_gpTimeout = true;
}
//---------------------------------------------------------------------------
bool __fastcall TFormMain::IsTimeElapsed(int time)
{
  return m_elapsedSeconds >= time;
}
//---------------------------------------------------------------------------
bool __fastcall TFormMain::IsGpTimeout(void)
{
  return m_gpTimeout;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::StopGpTimer(void)
{
  Timer1->Enabled = false;
  Timer1->OnTimer = NULL;
  m_gpTimeout = false;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::StartGpTimer(int iTime)
{
  Timer1->Enabled = false;
  m_gpTimeout = false;
  Timer1->Interval = iTime;
  Timer1->OnTimer = Timer1GpTimeout;
  Timer1->Enabled = true;
}
//---------------------------------------------------------------------------
// public method!
// hard delay, iTime is in milliseconds
void __fastcall TFormMain::DelayGpTimer(int iTime)
{
  StartGpTimer(iTime);
  while (!IsGpTimeout())
    Application->ProcessMessages();
  StopGpTimer();
}
//---------------------------------------------------------------------------
// starts a repeating, one-second up-timer for global member var m_elapsedSeconds
void __fastcall TFormMain::StartElapsedSecondsTimer(void)
{
  Timer1->Enabled = false; // stop timer (just in-case!)
  m_elapsedSeconds = 0;
  Timer1->Interval = ONESECTIMEOUT; // set time in ms TO 1 SECOND
  Timer1->OnTimer = Timer1OneSecTimeout; // set handler
  Timer1->Enabled = true; // start 1-second repeating timer
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::StopElapsedSecondsTimer(void)
{
  Timer1->Enabled = false; // stop timer
  m_elapsedSeconds = 0;
  Timer1->OnTimer = NULL; // clear handler
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ResetElapsedSecondsTimer(void)
{
  if (m_elapsedSeconds)
    m_elapsedSeconds = 0;
}
//---------------------------------------------------------------------------
// public method!

// waits for tx com to be empty
// returns 0 if success, negative if fail
// (this function stops the timer!)
// iTime is the max time to wait in seconds
// set count 0 to wait for all bytes sent (the default)
// set count > 0 to wait for all bytes except count sent (used in receive()).
int __fastcall TFormMain::WaitTxCom(int iTime, int count)
{
  try
  {
    if (ApdComPort1->OutBuffUsed > count)
    {
      try
      {
          StartElapsedSecondsTimer();

          while (ApdComPort1->OutBuffUsed > count)
          {
            Application->ProcessMessages();

            if (IsTimeElapsed(iTime))
            {
              printm("rs232 transmit timeout (previous bytes in Tx buffer!)");
              return -1;
            }
          }

          StopElapsedSecondsTimer();
      }
      __finally
      {
        StopElapsedSecondsTimer();
      }
    }
  }
  catch(...)
  {
    printm("unexpected error in WaitTxCom()!");
  }
  return 0;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MenuChangeComPortClick(TObject *Sender)
{
  if (IsBusy()){
    ShowMessage("Aborting... try \"Change COM Port\" again!");
    m_abort = true;
    return;
  }

  Timer1->Enabled = false;
  Timer1->OnTimer = NULL;
  m_gpTimeout = false;
  m_busyCount = 0;
  SetComPort(m_baud);
  ApdComPort1->Open = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ComboBoxBaudRateChange(TObject *Sender)
{
  if (IsBusy())
    return;

  SetComPort(ComboBoxBaudRate->Text.ToIntDef(DEF_RS232_BAUD_RATE));
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ComboBoxBaudRateSelect(TObject *Sender)
{
  if (IsBusy())
    return;

  SetComPort(ComboBoxBaudRate->Text.ToIntDef(DEF_RS232_BAUD_RATE));
  Memo1->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
  if (Key == VK_ESCAPE)
    m_abort = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MenuEditOverallSettingsClick(TObject *Sender)
{
  // don't allow com conflicts (on creation - these new forms
  // access the machine...)
  if (IsBusy())
    return;

  // don't set/clear busy flag here because methods in the CreateForm hook
  // set clear the flag already!

  TFormOverallSettings* pOS;
  Application->CreateForm(__classid(TFormOverallSettings), &pOS);
  if (!pOS) return;

  // go ahead - here, we have catalog info
  pOS->Show();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MenuEditSampleParametersClick(TObject *Sender)
{
  // don't allow com conflicts
  if (IsBusy())
    return;

  // don't set/clear busy flag here because methods in the CreateForm hook
  // set clear the flag already!

    // TEST (Comment out)!!!!!!!!!!!!!!!!!!!!!!
  //  m_numSampEntries = 4;
  //  strncpy(PermSampArray[0].name, "MyTestName", MAX_NAME_S900);
  //  PermSampArray[0].name[MAX_NAME_S900] = '\0';
  //  PermSampArray[0].index = 0;
  //  strncpy(PermSampArray[1].name, "New Samp 0", MAX_NAME_S900);
  //  PermSampArray[1].name[MAX_NAME_S900] = '\0';
  //  PermSampArray[1].index = 1;
  //  strncpy(PermSampArray[2].name, "New Samp 2", MAX_NAME_S900);
  //  PermSampArray[2].name[MAX_NAME_S900] = '\0';
  //  PermSampArray[2].index = 2;
  //  strncpy(PermSampArray[3].name, "New Samp 3", MAX_NAME_S900);
  //  PermSampArray[3].name[MAX_NAME_S900] = '\0';
  //  PermSampArray[3].index = 3;
  // TEST!!!!!!!!!!!!!!!!!!!!!!

  TFormEditSampParms* pFS;
  Application->CreateForm(__classid(TFormEditSampParms), &pFS);
  if (!pFS) return;

  // go ahead - here, we have catalog info
  pFS->Show();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MenuMakeOrEditProgramClick(TObject *Sender)
{
  // don't allow com conflicts
  if (IsBusy())
    return;

  // don't set/clear busy flag here because methods in the CreateForm hook
  // set clear the flag already!

  //TEST (comment out and uncomment code in ProgramForm to get catalog)
  //  m_numProgEntries = 4;
  //  strncpy(PermProgArray[0].name, "TONE PRGRM", MAX_NAME_S900);
  //  PermProgArray[0].name[MAX_NAME_S900] = '\0';
  //  PermProgArray[0].index = 0;
  //  strncpy(PermProgArray[1].name, "NewProg 00", MAX_NAME_S900);
  //  PermProgArray[1].name[MAX_NAME_S900] = '\0';
  //  PermProgArray[1].index = 1;
  //  strncpy(PermProgArray[2].name, "NewProg 01", MAX_NAME_S900);
  //  PermProgArray[2].name[MAX_NAME_S900] = '\0';
  //  PermProgArray[2].index = 2;
  //  strncpy(PermProgArray[3].name, "        x", MAX_NAME_S900);
  //  PermProgArray[3].name[MAX_NAME_S900] = '\0';
  //  PermProgArray[3].index = 3;
  //
  //  m_numSampEntries = 4;
  //  strncpy(PermSampArray[0].name, "TONE", MAX_NAME_S900);
  //  PermSampArray[0].name[MAX_NAME_S900] = '\0';
  //  PermSampArray[0].index = 0;
  //  strncpy(PermSampArray[1].name, "New Samp 0", MAX_NAME_S900);
  //  PermSampArray[1].name[MAX_NAME_S900] = '\0';
  //  PermSampArray[1].index = 0;
  //  strncpy(PermSampArray[2].name, "New Samp 2", MAX_NAME_S900);
  //  PermSampArray[2].name[MAX_NAME_S900] = '\0';
  //  PermSampArray[2].index = 0;
  //  strncpy(PermSampArray[3].name, "New Samp 3", MAX_NAME_S900);
  //  PermSampArray[3].name[MAX_NAME_S900] = '\0';
  //  PermSampArray[3].index = 0;

  // in FormProgram's Create hook, we invoke property getters
  // that convert PermSampArray and PermProgArray to TStringList
  TFormProgram* pFP;
  Application->CreateForm(__classid(TFormProgram), &pFP);
  if (!pFP) return;

  // go ahead - here, we have catalog info
  pFP->Show();
}
//---------------------------------------------------------------------------
// Public
// Get the catalog of samples and programs from the machine.
// The samples are in PermSampArray[] (access as TStringList using the SampleData property
// Count of samples is in m_numSampEntries (access using NumProgEntries property)
// The programs are in PermProgArray[] (access as TStringList using the ProgramData property
// Count of programs is in m_numProgEntries (access using NumSampEntries property)
// Returns 0 if success, negative number if error
// Returns 0 if success, -2 is tx error, -3 is rx error, -4 is other error
int __fastcall TFormMain::GetCatalog(bool bPrint, bool bDelay)
{
  // Request S950 Catalog (no delay)
  if (!exmit(0, RCAT, bDelay))
    return -2;

  if (receive(0) < 0)
    return -3;

  if (!catalog(bPrint))
    return -4;

  return 0;
}
//---------------------------------------------------------------------------
int __fastcall TFormMain::SetComPort(int baud)
{
  try
  {
    ComboBoxBaudRate->OnChange = NULL;

    try
    {
      // From Akai protocol: "Hardware handshake using CTS/RTS. Handshake need not
      // be used on rates below 50000 baud, in which case RTS should be tied high.”
      ApdComPort1->DonePort();

      THWFlowOptionSet hwflow;
      bool rts;
      if (baud >= 50000 || m_force_hwflow)
      {
        // hwfUseDTR, hwfUseRTS, hwfRequireDSR, hwfRequireCTS
        hwflow = (THWFlowOptionSet() << hwfUseRTS << hwfRequireCTS);
        rts = false; // state of the RTS line low
      }
      else
      {
        hwflow.Clear();
        rts = true; // state of the RTS line high
      }

      // need TEMPARRAYSIZ buffers if we use PutBlock() instead of PutChar() in comws()
      // default buffers are 4096
      ApdComPort1->OutSize = 4096;
      ApdComPort1->InSize = 4096;
      ApdComPort1->RS485Mode = false;
      ApdComPort1->PromptForPort = true;
      ApdComPort1->Baud = baud;
      ApdComPort1->HWFlowOptions = hwflow;
      ApdComPort1->RTS = rts;
      ApdComPort1->ComNumber = 0; // COM1
      ApdComPort1->DataBits = 8;
      ApdComPort1->StopBits = 1;
      ApdComPort1->Parity = pNone;
      ApdComPort1->AutoOpen = true;

      ComboBoxBaudRate->Text = String(baud);

      m_baud = baud;
    }
    catch(...)
    {
      return -1;
    }
  }
  __finally
  {
    ComboBoxBaudRate->OnChange = ComboBoxBaudRateChange;
  }
    return 0;
}
//---------------------------------------------------------------------------
// returns: 0=OK, -1 if error printed
// gets a sample's sysex extended parameters and puts them in the
// global m_temp_array... also validates the checksum
// (this method is public and is used by EditSampParmsForm.cpp)
int __fastcall TFormMain::LoadSampParmsToTempArray(int iSampIdx)
{
  try
  {
    // request sample parms (after 25ms delay)
    // (NOTE: this will open both in/out ports as needed!)
    if (!exmit(iSampIdx, RSPRM, true))
      return -2; // tx timeout

    // receive complete SYSEX message from serial port..., put into m_temp_array
    if (receive(0) < 0)
      return -3;

    if (m_rxByteCount != PARMSIZ)
      return -4;

    uint8_t* cp = m_temp_array + 7; // point past header to actual parameters
    uint8_t checksum = 0;

    // checksum of buffer minus 7-byte header and checksum and EEX
    for (int ii = 0; ii < PARMSIZ - 9; ii++)
      checksum ^= *cp++;

    if (checksum != *cp)
      return -5; // bad checksum
  }
  catch(...)
  {
    return -6;
  }

  return 0;
}
//---------------------------------------------------------------------------
// returns: 0=OK, -1 if error printed
// gets the machine's overall settings and puts them in the
// global m_temp_array... also validates the checksum
// (this method is public and is used by OverallSettingsForm.cpp)
int __fastcall TFormMain::LoadOverallSettingsToTempArray(void)
{
  try
  {
    // request overall settings (after 25ms delay)
    // (NOTE: this will open both in/out ports as needed!)
    if (!exmit(0, ROVS, true))
      return -2; // tx timeout

    // receive complete SYSEX message from serial port..., put into m_temp_array
    if (receive(0) < 0)
      return -3;

    if (m_rxByteCount != OSSIZ)
      return -4;

    uint8_t* cp = m_temp_array + 7; // point past header to actual settings
    uint8_t checksum = 0;

    // checksum of buffer minus 7-byte header and checksum and EEX
    for (int ii = 0; ii < OSSIZ - 9; ii++)
      checksum ^= *cp++;

    if (checksum != *cp)
      return -5; // bad checksum
  }
  catch(...)
  {
    return -100;
  }

  return 0;
}
//---------------------------------------------------------------------------
// Property getters/setters
//---------------------------------------------------------------------------
// value is the desired baud-rate,  9600, 57600, Etc.
void __fastcall TFormMain::SetBaudRate(uint32_t value)
{
  SetComPort(value);
}
//---------------------------------------------------------------------------
// Return the actual baud rate from the hardware
uint32_t __fastcall TFormMain::GetBaudRate(void)
{
  return ApdComPort1->Baud;
}
//---------------------------------------------------------------------------
uint8_t* __fastcall TFormMain::GetTempArray(void)
{
  return &m_temp_array[0];
}
//---------------------------------------------------------------------------
TStringList* __fastcall TFormMain::GetCatalogSampleData(void)
{
  TStringList* sl = new TStringList();

  if (sl)
  {
    CAT* p = &PermSampArray[0];

    for (int ii = 0; ii < m_numSampEntries; ii++, p++)
      sl->AddObject(p->name, (TObject*)p->index);
  }

  return sl;
}
//---------------------------------------------------------------------------
TStringList* __fastcall TFormMain::GetCatalogProgramData(void)
{
    TStringList* sl = new TStringList();

    if (sl)
    {
      CAT* p = &PermProgArray[0];

      for (int ii = 0; ii < m_numProgEntries; ii++, p++)
        sl->AddObject(p->name, (TObject*)p->index);
    }

    return sl;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MainMenuHelpClick(TObject *Sender)
{
  String s =
  "WAV File uploader for the Akai S900/S950 digital sampler.\n\n"
  "Select multiple .wav or .aki sample files and a single .prg programs file"
  " and drag-drop them as a group onto the main window to upload"
  " them to your Akai S900/S950 digital sampler.\n\n"
  "You can generate a program to go with your .wav files by first uploading"
  " all .wav files to the machine, then choose \"Make/Edit Program\" from the"
  " menu. The editor adds a keygroup for each sample currently on the"
  " machine.\n\n"
  "A single .prg file can hold multiple S900/S950 programs.\n\n"
  "Akai-specific sample parameters can now be stored in a .wav"
  " file. You may read your samples from the machine and store them"
  " as either 12-bit mono .wav files or as custom-format .aki files.\n\n"
  "This program automatically adjusts the number of bits per sample and"
  " normalizes the sample.\n\n"
  "The S900/S950 samplers record at 12 bits resolution and play back at 14 bits"
  " resolution, so some clarity may be lost for 16 or 24 bit wav files.\n\n";
  ShowMessage(s);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MenuClearClick(TObject *Sender)
{
  Memo1->Clear();
}
//---------------------------------------------------------------------------

