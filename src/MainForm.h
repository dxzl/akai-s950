//---------------------------------------------------------------------------
//Software written by Scott Swift 2017 - This program is distributed under the
//terms of the GNU General Public License.
//---------------------------------------------------------------------------
#ifndef MainFormH
#define MainFormH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Clipbrd.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.Menus.hpp>
#include "AdPort.hpp"
#include "OoMisc.hpp"

// defines
//---------------------------------------------------------------------------
// Note: Use String() to wrap this for the overloaded RegHelper write method!
// (set MainForm Height to 350 when help is clicked)
#define FORM_HEIGHT 350
#define VERSION_STR "Version 2.4 ALPHA_TEST A"
#define DIAGNOSTIC_MODE false
//---------------------------------------------------------------------------

#define REGISTRY_KEY "\\Software\\Discrete-Time Systems\\AkaiS950"

#define DEFSAVENAME ("akai_progs")

// Registry entries we save in HKEY_CURRENT_USER
#define S9_REGKEY_VERSION "Version"
#define S9_REGKEY_BAUD "Baud"
#define S9_REGKEY_USE_RIGHT_CHAN "UseRightChan"
#define S9_REGKEY_AUTO_RENAME "AutoRename"
#define S9_REGKEY_FORCE_HWFLOW "ForceHWFlowControl"
#define S9_REGKEY_SEND_AS_12_BITS "SendAs12Bits"

#define LOAD 1
#define SAVE 2

#define NUMITEMS 4
#define GETFILE  0
#define PUTFILE  1
#define SETTINGS 2
#define CONNECT  3

#define ONESECTIMEOUT 1000 // 1 second
#define RXTIMEOUT 3 // 3 seconds
#define TXTIMEOUT 3 // 3 seconds

// NOTE: increasing this will cause much more memory to be used because many
// arrays are not dynamically allocated (they should be dynamic and use "new"
// instead)... TODO!
#define MAX_SAMPS 100 // S900 is 32 but S950 is "99" (I think it's 100 with TONE)
#define MAX_PROGS 100 // S900 is 32 but S950 is "99"?????

#define DATA_PACKET_SIZE 120 // 120 bytes representing 60, 40 or 30 samples
#define DATA_PACKET_OVERHEAD 2  // block-counter (7-bits) and checksum
#define SDS_DATA_PACKET_OVERHEAD 7  // BEX, 7E, cc, 02, block-counter (7-bits), checksum and EEX

#define MAX_SERIAL_BITS_PER_WORD 14

// S950 sample parameters size
#define PARMSIZ (122+7)

// .AKI file header info. (Note: sizeof(PSTOR) must evaluate to 72!)
// (otherwise, old .aki files will not be compatible)
#define AKI_FILE_HEADER_SIZE (sizeof(PSTOR)) // parameter storage for .AKI file
#define MAGIC_NUM_AKI 453782108 // magic number identifies .aki file

#define AKIEXT ".aki"

// add this to saved wav file-names. then strip it off again when drag-dropping
// or sending wav files
#define WAV_NAME_WARNING " (12-bit mono)"

// S950 sample header storage
#define HEDRSIZ 19  // starts with BEX but no EEX
#define ACKSIZ 4

// minimum points in a loop
#define MINLOOP 5

// packet retry count
#define PACKET_RETRY_COUNT 3

#define UINT16SIZE ((int)sizeof(uint16_t))
#define UINT32SIZE ((int)sizeof(uint32_t))

// system exclusive
#define BEX  (0xf0)  // beginning of exclusive
#define EEX  (0xf7)  // end of EXCLUSIVE

// sysex acknowledge block
#define PSD   (0x7c)  // pause until next packet (SDS only!)
#define ASD   (0x7d)  // abort sample dump
#define NAKS  (0x7e)  // not-acknowledge (request re-trans.)*/
#define ACKS  (0x7f)  // acknowledge sample block or header

// system common commands (enable system common reception to use these!)
#define RSD       0  // request sample dump
#define SD        1  // sample dump
#define SDS_DATA  2  // data-packet
#define SDS_RSD   3  // request sample-dump

// akai system exclusive commands
#define RDRS  0     // request drum settings
#define ROVS  1     // request overall settings
#define RPRGM 2     // request a program and its keygroups
#define RCAT  3     // request prg/samp names
#define RSPRM 4     // request sample parms
#define SECRE 5     // system exclusive common reception enable
#define SECRD 6     // system exclusive common reception disable
#define DRS   7     // drum settings
#define OVS   8     // overall settings
#define PRGM  9     // program
#define SPRM  10    // sample parameters
#define DCAT  11    // sample/program name catalog

#define AKAI_ID   (0x47)  // AKAI I.D.
#define S900_ID   (64)    // S900/S950 I.D.

// system common non-realtime I.D.
#define SYSTEM_COMMON_NONREALTIME_ID (0x7e)
#define SYSTEM_COMMON_REALTIME_ID (0x7f)

// the number of chars the S950 can handle, is 10 chars max
#define MAX_NAME_S900 10

#define DAC12UNSIGNED 4095 // 2^12 / 2 - 1
#define DAC12 2047 // 2^12 / 2 - 1
#define DAC16 32767 // 2^16 / 2 - 1

// .PRG file header info.
// a single program has PRG_FILE_HEADER_SIZE + (X*PROGKEYGROUPSIZ) + 2 for checksum
// and EEX bytes
#define PROGHEADERSIZ (76+7) // S950 program header size
// magic number identifies .prg file
#define MAGIC_NUM_PRG 639681490
// S950 program keygroup size
// in a .prg file, we also have the 4-byte prog size at the top of each program
#define PROGKEYGROUPSIZ 140
// max keygroups in a program (S950 is 32, (we need to limit this because of TEMPARRAYSIZ!)
#define MAX_KEYGROUPS 32

// this TempArray is used for all receive() data and needs to be big enough
// for the largest single transmission from the target machine
// (we need 122 bytes for a data packet)
// (we need 127 bytes for a sample-dump-standard data packet)
// 2400 bytes for the largest possible catalog...
//#define TEMPARRAYSIZ ((int)(sizeof(S950CAT)*(MAX_SAMPS+MAX_PROGS) + 9))
//
// Since we now can edit a program with 32 keygroups, we need a larger array...
#define TEMPARRAYSIZ ((int)(2+PROGHEADERSIZ+(PROGKEYGROUPSIZ*MAX_KEYGROUPS)))


#define DEF_RS232_BAUD_RATE (38400)

// when we transmit individual S900/S950 programs from a file to the
// target machine, there is a delay needed between them - from long-distance
// trial-and-error reports, this does not work at 25ms and does work at 100ms,
// so I'm setting it to 125ms.
#define DELAY_BETWEEN_EACH_PROGRAM_TX 125

#define STEPSPERSEMITONE 16 // 16 steps per semitone for Akai S900/S950

// this struct can be modified if you need to rearrange it or add fields...
typedef struct
{
  char name[MAX_NAME_S900+1]; // ASCII
  int index;
} CAT;

// this struct MUST be 12 bytes and always be in this order!
// (it reflects the cat data returned by the target machine)
typedef struct
{
  uint8_t type; // ASCII: "P" = program, "S" = sample
  uint8_t index; // program/sample number (0-31)
  char name[MAX_NAME_S900]; // program/sample name (in ASCII)
} S950CAT;

// Note: sizeof(PSTOR) is 72, but the original
// program stored the header as 68 bytes (in an AKI file...)

#define PSTOR_NAME_COUNT (MAX_NAME_S900+4)

// AKI_FILE_HEADER_SIZE (sizeof(PSTOR))(MUST BE 72 bytes!!!!)
// make sure it is always 72 because we directly encode/decode
// it in a .AKI sample file!
// NOTE: Reason you can't change the order and size of things is that there are many users of this
// program through the years who have .aki sample files that will break if this is changed!!!!
// (otherwise I would move freq/pitch together and make a DD for m_u32_undef95!)
typedef struct
{
  // parameter storage, file storage header
  uint32_t startpoint; // first replay (loop) index (4 bytes)
  uint32_t endpoint; // end of play point (4 bytes)
  uint32_t looplen; // loop length relative to startidx (4 bytes)
  uint16_t freq; // sample freq. in Hz (2 bytes)
  uint16_t reservedDW_87; // DW at offset 87 in samp_parms (2 bytes)
  uint16_t pitch; // pitch - units = 1/16 semitone (2 bytes)
  uint16_t undefinedDW_35; // DW at offset 35 in samp_parms (2 bytes)
  uint32_t sampleCount; // total number of words in sample (4 bytes)
  uint32_t period; // sample period in nanoseconds (4 bytes)
  uint32_t undefinedDD_95;
  uint16_t bits_per_word; // (2 bytes)
  char name[PSTOR_NAME_COUNT]; // ASCII sample name (14 bytes)
  int32_t undefinedDD_27; // DD at offset 27 in samp_parms (4 bytes)
  int32_t undefinedDD_103; // DD at offset 103 in samp_parms (4 bytes)
  int32_t undefinedDD_111; // DD at offset 111 in samp_parms (4 bytes)
  int32_t undefinedDD_119; // DD at offset 119 in samp_parms (4 bytes)
  int32_t loudnessoffset; // signed value (4 bytes)
  uint8_t spareDB1; // (1 byte)
  uint8_t reservedDB_61; // (1 byte)
  uint8_t flags; // (1 byte) (bit 0 = vel. xfade, bit 1 = reverse-waveform)
  uint8_t loopmode; // (1 byte) 'O' one-shot, 'A' alternateing, 'L' looping
} PSTOR; // sizeof(PSTOR) must always be 72 bytes!

// 44 bytes
typedef struct
{
  char chunkTag[4]; // "RIFF"
  uint32_t fileSizeMinusEight;
  char wave[4]; // "WAVE"
  char fmt[4]; // "fmt "
  uint32_t fmtSectionSize; // 16
  uint16_t audioFormat; // 1 (umcompressed pcm)
  uint16_t numChannels; // 1 (mono)
  uint32_t sampleFreq;
  uint32_t frameRate; // (SampleFreq * BitsPerSample * NumChannels) / 8
  uint16_t bytesPerFrame; // 2
  uint16_t bitsPerSample; // 12 for the S900/S950
  char data[4]; // "data"
  uint32_t dataSize; // # sample words * bytesPerFrame
} WAVHEDR; // 44 bytes

// a SMPLCHUNK can have 0-N of these... (we need only one)
typedef struct
{
  uint32_t cuePointId; // 0 (could refer to a particular cue point Id), "loop"
  uint32_t type; // 0=forward, 1=alternate, 2=reverse (for the loop, not the sample)
  uint32_t start; // (sample's end-index - loop-length) * 2 bytes per frame
  uint32_t end; // sample's end-index * 2 bytes per frame
  uint32_t fraction; // 0
  uint32_t playCount; // 0 (endless)
} SMPLLOOP; // 24 bytes

// we tack this on in a "smpl" chunk, after any loops (same size as a 24-byte SMPLLOOP)
typedef struct
{
  uint32_t magic; // set this to MAGIC_NUM_AKI (identifies it as "uniquely ours")
  uint8_t flags;
  uint8_t loudnessoffset;
  uint16_t spare1;
  uint32_t spare2;
  uint32_t spare3;
  uint32_t spare4;
  uint32_t spare5;
} SMPLDATA; // 24 bytes

// generic digital-sampler chunk
typedef struct
{
  char chunkTag[4]; // "smpl"
  uint32_t chunkSize; // 60
  uint32_t Manufacturer; // 0
  uint32_t Product; // 0
  uint32_t SamplePeriod; // 0
  uint32_t MIDIUnityNote; // (60 for middle-C)
  uint32_t MIDIPitchFraction; // 0
  uint32_t SMPTEFormat; // 0
  uint32_t SMPTEOffset; // 0
  uint32_t loopCount; // 1
  uint32_t samplerDataSize; // sizeof(SMPLOOP) + sizeof(uint32_t) + sizeof(PSTOR)
} SMPLCHUNKHEDR; // 44 bytes

// a CUECHUNK can have 0-N of these... (we need only one)
typedef struct {
  char cuePointID[4]; // a unique ID for the Cue Point. "strt" "end "
  uint32_t playOrderPosition; // Unsigned 4-byte little endian int: If a Playlist chunk is present in the Wave file, this the sample number at which this cue point will occur during playback of the entire play list as defined by the play list's order.  **Otherwise set to same as sample offset??***  Set to 0 when there is no playlist.
  char dataChunkID[4]; // Unsigned 4-byte little endian int: The ID of the chunk containing the sample data that corresponds to this cue point.  If there is no playlist, this should be 'data'.
  uint32_t chunkStart; // Unsigned 4-byte little endian int: The byte offset into the Wave List Chunk of the chunk containing the sample that corresponds to this cue point. This is the same chunk described by the Data Chunk ID value. If no Wave List Chunk exists in the Wave file, this value is 0.
  uint32_t blockStart; // Unsigned 4-byte little endian int: The byte offset into the "data" or "slnt" Chunk to the start of the block containing the sample. The start of a block is defined as the first byte in uncompressed PCM wave data or the last byte in compressed wave data where decompression can begin to find the value of the corresponding sample value.
  uint32_t frameOffset; // Unsigned 4-byte little endian int: The offset into the block (specified by Block Start) for the sample that corresponds to the cue point.
} CUEPOINT; // 24 bytes

// holds our start-point
typedef struct
{
  char chunkID[4]; // String: Must be "cue " (0x63756520).
  uint32_t chunkDataSize; // Unsigned 4-byte little endian int: Byte count for the remainder of the chunk: 4 (size of cuePointsCount) + (24 (size of CuePoint struct) * number of CuePoints).
  uint32_t cuePointsCount; // Unsigned 4-byte little endian int: Length of cuePoints[].
  CUEPOINT cuePoints[2];
} CUECHUNK; // 60 bytes

//---------------------------------------------------------------------------
class TFormMain : public TForm
{
__published:  // IDE-managed Components
  TPanel *Panel2;
	TComboBox *ComboBoxBaudRate;
    TListBox *ListBox1;
    TOpenDialog *OpenDialog1;
    TSaveDialog *SaveDialog1;
    TMainMenu *MainMenu1;
    TPanel *Panel1;
    TMemo *Memo1;
    TTimer *Timer1;
    TMenuItem *MenuGetCatalog;
    TMenuItem *N1;
    TMenuItem *MenuGetSample;
    TMenuItem *MenuPutSample;
    TMenuItem *N2;
    TMenuItem *MenuGetPrograms;
    TMenuItem *N3;
    TMenuItem *MenuUseRightChanForStereoSamples;
    TMenuItem *MenuAutomaticallyRenameSample;
    TMenuItem *MenuUseHWFlowControlBelow50000Baud;
    TMenuItem *MenuMax12BitsPerSample;
    TMenuItem *N4;
    TMenuItem *MenuAbout;
    TMenuItem *MenuMakeOrEditProgram;
    TMenuItem *N5;
    TMenuItem *MenuEditSampleParameters;
    TMenuItem *MainMenuHelp;
    TMenuItem *MenuEditOverallSettings;
    TApdComPort *ApdComPort1;
	  TMenuItem *MenuChangeComPort;
  TMenuItem *MenuClear;

    void __fastcall MenuGetCatalogClick(TObject *Sender);
    void __fastcall MenuPutSampleClick(TObject *Sender);
    void __fastcall MenuGetSampleClick(TObject *Sender);
    void __fastcall MenuAutomaticallyRenameSampleClick(TObject *Sender);
    void __fastcall MenuGetProgramsClick(TObject *Sender);
    void __fastcall MenuUseHWFlowControlBelow50000BaudClick(TObject *Sender);
    void __fastcall MenuUseRightChanForStereoSamplesClick(TObject *Sender);

    void __fastcall ListBox1Click(TObject *Sender);
  void __fastcall MenuAboutClick(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall ComboBoxBaudRateChange(TObject *Sender);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall ComboBoxBaudRateSelect(TObject *Sender);
  void __fastcall FormDestroy(TObject *Sender);
    void __fastcall MenuMakeOrEditProgramClick(TObject *Sender);
    void __fastcall MenuEditSampleParametersClick(TObject *Sender);
  void __fastcall MainMenuHelpClick(TObject *Sender);
  void __fastcall MenuEditOverallSettingsClick(TObject *Sender);
	void __fastcall MenuChangeComPortClick(TObject *Sender);
  void __fastcall MenuMax12BitsPerSampleClick(TObject *Sender);
  void __fastcall MenuClearClick(TObject *Sender);

private:  // User declarations
  void __fastcall Timer1FileDropTimeout(TObject *Sender);
  void __fastcall Timer1GpTimeout(TObject *Sender);
  void __fastcall Timer1OneSecTimeout(TObject *Sender);

  bool __fastcall IsGpTimeout(void);
  void __fastcall StopGpTimer(void);
  void __fastcall StartGpTimer(int time);
  void __fastcall StartElapsedSecondsTimer(void);
  void __fastcall StopElapsedSecondsTimer(void);
  void __fastcall ResetElapsedSecondsTimer(void);
  bool __fastcall IsTimeElapsed(int iTime);

  bool __fastcall PutSample(String sFilePath);
  int __fastcall GetSample(String sPath, int iSamp);
  int __fastcall GetAsWavFile(long lFileHandle, int iSamp, PSTOR* ps);
  int __fastcall GetAsAkiFile(long lFileHandle, int iSamp, PSTOR* ps);
  void __fastcall PutPrograms(String sFilePath);
  void __fastcall PutFiles(void);
  int __fastcall GetPrograms(String sFilePath);

  void __fastcall SetMenuItems(void);
  int __fastcall FindIndex(uint8_t* pName);
  int __fastcall SetComPort(int baud);
  int __fastcall GetFileNames(void);
  bool __fastcall DoSaveDialog(String &sName);
  int32_t __fastcall FindSubsection(uint8_t* &fileBuffer, char* chunkName, UINT maxBytes);
  void __fastcall encode_samp_hedr(uint16_t index, PSTOR* ps);
  void __fastcall encode_samp_parms(uint16_t index, PSTOR* ps);
  void __fastcall decode_samp_hedr(PSTOR* ps);
  void __fastcall decode_samp_parms(PSTOR* ps);

  bool __fastcall bytewisecompare(uint8_t* buf1, uint8_t* buf2, int maxLen);
  int __fastcall findidx(uint8_t* sampName);
  void __fastcall queue(int64_t acc, uint8_t* &ptbuf, int sampler_bytes_per_word, int bits_per_word, uint8_t &checksum);
  void __fastcall send_samp_parms(unsigned index);
  bool __fastcall send_packet(uint8_t* tbuf, int blockct);
  int __fastcall receive(int count);
  bool __fastcall catalog(bool print);

  bool __fastcall chandshake(int mode, int blockct=0);
  int __fastcall get_ack(int blockct);
  bool __fastcall exmit(int samp, int mode, bool bDelay);
  bool __fastcall cxmit(int samp, int mode, bool bDelay);

  int __fastcall get_samp_data(PSTOR * ps, long lFileHandle, bool bIsWavFile);
  int __fastcall get_data_block(uint8_t* dest, int sampler_bytes_per_word, int target_bytes_per_word,
      int sampler_words_per_packet, int bits_per_word, int packet_count, bool bIsWavFile);

  int __fastcall get_comm_samp_hedr(int samp);
  void __fastcall WMDropFile(TWMDropFiles &Msg);

  // receive data buffer, needs to be big enough for a catalog of a
  // sampler with the max samples allowed OR large enough for
  // the largest sysex dump you expect, sample parameters, drum settings, etc.
  uint8_t m_temp_array[TEMPARRAYSIZ];

  //---------------------------------------------------------------------------
  // header for a floppy-disk is 1/2 the samp_parms[] array size:
  // (appears to mirror the samp_parms[] info. but in 60 bytes rather than 120!!!)
  //     10     ASCII       Filename
  //      4     undefined   0
  //      2     undefined   0
  //      4     unsigned    Number of sample words
  //      2     unsigned    Sample rate (Hz)
  //      2     unsigned    Tuning (16ths of a semitone, C3=960)
  //      2     signed      Loudness offset
  //      1     ASCII       Replay mode (O=one-shot, L=loop, A=alt)
  //      1     reserved    0
  //      4     unsigned    End marker
  //      4     unsigned    Start marker
  //      4     unsigned    Loop length
  //      2     reserved    (offset 87 in samp_parms[]) 36025 (other examples seen 46460)
  //      1     byte        0/255 Velocity crossfade/normal
  //      1     ASCII       Reverse/normal (R=reversed, N=normal)
  //      4     undefined   0
  //      4     undefined   0
  //      4     unknown     (offset 111 in samp_parms[]) 61483 (have seen 2147483648, 2481979392)
  //      4     unknown     (offset 119 in samp_parms[]) 637534208 (have seen 1, 16)
  //---------------------------------------------------------------------------

  // holds the raw Rx/Tx sample information
  //
  //SNAME   DB  'nnnnnnnnnn'   Name of sample 20
  //        DD  x   Undefined 8
  //        DW  x   Undefined 4
  //SLNGTH  DD  1800  Total number of words in sample. Note that for velocity-crossfade type this will be the sum of soft and loud parts 8
  //SMRATE  DW  11773   Original sample rate in Hz (=2.5*bandwidth) 4
  //SNOMP   DW  960   Nominal pitch, unsigned in 1/16 semitones (960=C3) 4
  //SDFLDO  DW  0   Signed loudness offset 4
  //SRPLMD  DB  'L'   Replay mode: 'O/L/A'=one shot/looping/alternating 2
  //        DB  0   Reserved 2
  //SEND    DD  1800  End point relative to start of sample 8
  //SSTART  DD  0   First replay point relative to start of sample 8
  //SLOOP   DD  45  Length of loop or alternative part 8
  //        DW  x   Reserved 4
  //VC      DB  0   0/255 Velocity crossfade/normal type sample 2
  //NOREV   DB  'N'   'R/N' Sample waveform has/has not been reversed 2
  //        DD  x   Undefined 8
  //        DD  x   Undefined 8
  //        DD  x   Undefined 8
  //        DD  x   Undefined 8
  uint8_t samp_parms[PARMSIZ];

  // header for the sample-data
  uint8_t samp_hedr[HEDRSIZ];

  unsigned m_rxByteCount;
  int m_numSampEntries, m_numProgEntries, m_elapsedSeconds, m_busyCount;
  bool m_gpTimeout, m_inBufferFull, m_abort;

  // holds the processed catalog info
  CAT PermSampArray[MAX_SAMPS];
  CAT PermProgArray[MAX_PROGS];

  TStringList* m_slFilePaths;

  // settings vars
  int m_baud, m_data_size, m_hedr_size, m_ack_size;
  bool m_use_right_chan, m_auto_rename, m_force_hwflow;
  bool m_send_as_12_bits, m_use_smooth_quantization;

protected:

BEGIN_MESSAGE_MAP
  //add message handler for WM_DROPFILES
  // NOTE: All of the TWM* types are in messages.hpp!
  VCL_MESSAGE_HANDLER(WM_DROPFILES, TWMDropFiles, WMDropFile)
END_MESSAGE_MAP(TComponent)

  TStringList* __fastcall GetCatalogSampleData(void);
  TStringList* __fastcall GetCatalogProgramData(void);
  uint8_t* __fastcall GetTempArray(void);
  uint32_t __fastcall GetBaudRate(void);
  void __fastcall SetBaudRate(uint32_t value);

public:    // User declarations
  __fastcall TFormMain(TComponent* Owner);

  void __fastcall DelayGpTimer(int iTime);
  int __fastcall WaitTxCom(int iTime, int count=0);

  bool __fastcall comws(int count, uint8_t* ptr, bool bDelay);

  int __fastcall LoadProgramToTempArray(int progIndex);
  int __fastcall LoadSampParmsToTempArray(int iSampIdx);
  void __fastcall PrintLoadSampParmsErrorMessage(int iError);
  int __fastcall LoadOverallSettingsToTempArray(void);
  int __fastcall GetCatalog(bool bPrint=false, bool bDelay=false);

  // this is a "high-level" busy flag - test, set and clear it from
  // the UI level... such as a menu or button click!
  // General rule: you can call SysUtil method IsBusy() for a high
  // level menu-hook, but don't set/clear the busy flag there if inner-levels
  // already set/clear the flag!
  __property int BusyCount = {read = m_busyCount, write = m_busyCount};

  __property TStringList* CatalogSampleData = {read = GetCatalogSampleData};
  __property TStringList* CatalogProgramData = {read = GetCatalogProgramData};
  __property uint8_t* TempArray = {read = GetTempArray};
  __property uint32_t BaudRate = {read = GetBaudRate, write = SetBaudRate};
};

extern PACKAGE TFormMain *FormMain;
//---------------------------------------------------------------------------
#endif

