//---------------------------------------------------------------------------
//Software written by Scott Swift 2016 - This program is distributed under the
//terms of the GNU General Public License.
//---------------------------------------------------------------------------
#ifndef MainFormH
#define MainFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Menus.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include "AdPort.hpp"
#include "OoMisc.hpp"

// defines
//---------------------------------------------------------------------------
#define VERSION_STR "Version 1.29: December 3, 2016"
//---------------------------------------------------------------------------

#define REGISTRY_KEY "\\Software\\Discrete-Time Systems\\AkaiS900"

// Registry entries we save in HKEY_CURRENT_USER
#define S9_REGKEY_VERSION "Version"
#define S9_REGKEY_BAUD "Baud"
#define S9_REGKEY_USE_RIGHT_CHAN "UseRightChan"
#define S9_REGKEY_AUTO_RENAME "AutoRename"
#define S9_REGKEY_FORCE_HWFLOW "ForceHWFlowControl"
#define S9_REGKEY_TARGET_S950 "TargetS950"

#define LOAD 1
#define SAVE 2

#define NUMITEMS 4
#define GETFILE  0
#define PUTFILE  1
#define SETTINGS 2
#define CONNECT  3

#define ACKINITIALTIMEOUT 3100 // 3.1 seconds
#define ACKTIMEOUT 2100 // 2.1 seconds

// NOTE: increasing this will cause much more memory to be used because many
// arrays are not dynamically allocated (they should be dynamic and use "new"
// instead)... TODO!
#define MAX_SAMPS_S900 32 // S900 is 32 but S950 is "99" (I think it's 100 with TONE)

#define WORDS_PER_BLOCK 60

// S900 sample parameters size
#define PARMSIZ 129

// .AKI file header info. (Note: sizeof(PSTOR) must evaluate to 72!)
// (otherwise, old .aki files will not be compatible)
#define AKI_FILE_HEADER_SIZE (sizeof(PSTOR)) // parameter storage for .AKI file
#define MAGIC_NUM_AKI 453782108 // magic number identifies .aki file

// .PRG file header info.
// a single program has PRG_FILE_HEADER_SIZE + (X*PROGKEYGROUPSIZ) + 2 for checksum
// and EEX bytes
#define PRG_FILE_HEADER_SIZE 83 // S900 program header size
// magic number identifies .prg file
#define MAGIC_NUM_PRG 639681490
// S900 program keygroup size
#define PROGKEYGROUPSIZ 140
// max keygroups in a program (S900 is 32, but we'll allow 100)
#define MAX_KEYGROUPS 100

// S900 sample header storage
#define HEDRSIZ 19  // starts with BEX but no EEX
#define ACKSIZ  4

#define UINT32SIZE ((int)sizeof(unsigned __int32))

#define BEX  ((unsigned char)0xF0)  // beginning of exclusive
#define EEX  ((unsigned char)0xF7)  // end of EXCLUSIVE

#define AKAI_ID   ((unsigned char)0x47)  // AKAI I.D.
#define S900_ID   ((unsigned char)64)    // S900 I.D.
#define COMMON_ID ((unsigned char)0x7e)  // system common I.D.

// akai exclusive
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

// system exclusive
#define RSD   0     // request sample dump
#define SD    1     // sample dump
#define ACKS  ((unsigned char)0x7f)  // acknowledge sample block or header
#define ASD   ((unsigned char)0x7d)  // abort sample dump
#define NAKS  ((unsigned char)0x7e)  // not-acknowledge (request re-trans.)*/

// the number of chars the S900 can handle, is 10 chars max
#define MAX_NAME_S900 10

#define S900_BITS_PER_SAMPLE 14

#define DAC12UNSIGNED 4095 // 2^12 / 2 - 1
#define DAC12 2047 // 2^12 / 2 - 1
#define DAC16 32767 // 2^16 / 2 - 1

#define TEMPCATBUFSIZ (sizeof(S900CAT)*MAX_SAMPS_S900*2 + 9)

typedef struct
{
  char name[MAX_NAME_S900+1];
  int sampidx;
  int select; // user select flag
} CAT;

typedef struct
{
  char type; // ASCII: "P" = program, "S" = sample
  char sampidx; // program/sample number (0-31)
  char name[MAX_NAME_S900]; // program/sample name
} S900CAT;

// Note: sizeof(PSTOR) is 72, but the original
// program stored the header as 68 bytes (in an AKI file...)

#define PSTOR_SPARE_COUNT 12
#define PSTOR_NAME_COUNT (MAX_NAME_S900+4)

// AKI_FILE_HEADER_SIZE (sizeof(PSTOR))(MUST BE 72 bytes!!!!)
// make sure it is always 72 because we directly encode/decode
// it in a .AKI sample file!
typedef struct
{
  // parameter storage, file storage header
  unsigned __int32 loopstart; // first replay (loop) index (4 bytes)
  unsigned __int32 endpoint; // end of play point (4 bytes)
  unsigned __int32 looplen; // loop length relative to startidx (4 bytes)
  unsigned __int32 freq; // sample freq. in Hz (4 bytes)
  unsigned __int32 pitch; // pitch - units = 1/16 semitone (4 bytes)
  unsigned __int32 totalct; // total number of words in sample (4 bytes)
  unsigned __int32 period; // sample period in nanoseconds (4 bytes)
  unsigned __int32 spareint1; // (4 bytes)
  unsigned __int16 bits_per_sample; // (2 bytes)
  char name[PSTOR_NAME_COUNT]; // ASCII sample name (14 bytes)
  char spares[PSTOR_SPARE_COUNT]; // unused bytes (12 bytes)
  __int32 spareint2; // signed value (4 bytes)
  __int32 loudnessoffset; // signed value (4 bytes)
  unsigned char sparechar1; // (1 byte)
  unsigned char sparechar2; // (1 byte)
  unsigned char flags; // (1 byte)
  unsigned char loopmode; // (1 byte)
} PSTOR; // sizeof(PSTOR) must always be 72 bytes!

//---------------------------------------------------------------------------
class TFormS900 : public TForm
{
__published:  // IDE-managed Components
    TMainMenu *MainMenu1;
    TMenuItem *S9001;
  TMenuItem *MenuGetCatalog;
  TMenuItem *MenuGetSample;
  TMenuItem *MenuPutSample;
    TOpenDialog *OpenDialog1;
    TSaveDialog *SaveDialog1;
    TMenuItem *Help1;
    TTimer *Timer1;
    TMenuItem *N1;
  TMenuItem *MenuUseRightChanForStereoSamples;
    TMenuItem *N2;
    TApdComPort *ApdComPort1;
    TPanel *Panel1;
    TPanel *Panel2;
    TListBox *ListBox1;
    TComboBox *ComboBox1;
    TMemo *Memo1;
    TMenuItem *MenuAutomaticallyRenameSample;
  TMenuItem *MenuGetPrograms;
  TMenuItem *MenuPutPrograms;
  TMenuItem *N3;
  TMenuItem *N4;
  TMenuItem *MenuUseHWFlowControlBelow50000Baud;

    void __fastcall MenuGetCatalogClick(TObject *Sender);
    void __fastcall MenuPutSampleClick(TObject *Sender);
    void __fastcall MenuGetSampleClick(TObject *Sender);
    void __fastcall ListBox1Click(TObject *Sender);
    void __fastcall Help1Click(TObject *Sender);
    void __fastcall MenuUseRightChanForStereoSamplesClick(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall Timer1FileDrop(TObject *Sender);
    void __fastcall Timer1RxTimeout(TObject *Sender);
    void __fastcall ComboBox1Change(TObject *Sender);
    void __fastcall MenuAutomaticallyRenameSampleClick(TObject *Sender);
  void __fastcall MenuGetProgramsClick(TObject *Sender);
  void __fastcall MenuPutProgramsClick(TObject *Sender);
  void __fastcall MenuUseHWFlowControlBelow50000BaudClick(TObject *Sender);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);

private:  // User declarations
    void __fastcall SetComPort(int baud);
    int __fastcall FindIndex(char* pName);
    void __fastcall PutFile(String sFilePath);
    String __fastcall GetFileName(void);
    bool __fastcall StrCmpCaseInsens(char* sA, char* sB, int len);
    __int32 __fastcall FindSubsection(unsigned char* &fileBuffer, char* chunkName, UINT maxBytes);

    void __fastcall encode_sample_info(int samp, PSTOR* ps);
    void __fastcall decode_sample_info(PSTOR* ps);

    void __fastcall encode_parmsDB(unsigned char c, unsigned char* dest);
    void __fastcall encode_parmsDB(unsigned char* source, unsigned char* dest, int numchars);
    unsigned char __fastcall decode_parmsDB(unsigned char* source);
    void __fastcall decode_parmsDB(unsigned char* dest, unsigned char* source, int numchars);

    void __fastcall encode_parmsDD(unsigned __int32 value, unsigned char* tp);
    unsigned __int32 __fastcall decode_parmsDD(unsigned char* tp);

    void __fastcall encode_parmsDW(unsigned __int32 value, unsigned char* tp);
    unsigned __int32 __fastcall decode_parmsDW(unsigned char* tp);

    void __fastcall encode_hedrTB(unsigned __int32 value, unsigned char* tp);
    unsigned __int32 __fastcall decode_hedrTB(unsigned char* tp);

    void __fastcall compute_checksum(int min_index, int max_index);
    int __fastcall findidx(char* sampName);
    void __fastcall exmit(int samp, int mode);
    void __fastcall comws(int count, unsigned char* ptr);
    unsigned char __fastcall send_data(__int16* intptr, PSTOR* ps);
    unsigned char __fastcall wav_send_data(unsigned __int16* ptr);
    void __fastcall send_samp_parms(unsigned int index);
    void __fastcall printm(String message);
    void __fastcall print_ps_info(PSTOR* ps);
    int __fastcall receive(int count);
    void __fastcall catalog(bool print);
    int __fastcall get_ack(int blockct);
    void __fastcall trimright(char* pStr);

    int __fastcall get_sample(int samp, String Name);
    int __fastcall get_samp_data(PSTOR * ps, int handle);
    int __fastcall get_comm_samp_data(__int16* bufptr, int max_bytes, PSTOR* ps, int blockct);
    int __fastcall get_comm_samp_parms(int samp);
    int __fastcall get_comm_samp_hedr(int samp);
    void __fastcall chandshake(int mode);
    void __fastcall cxmit(int samp, int mode);
    void __fastcall WMDropFile(TWMDropFiles &Msg);

    // receive data buffer, needs to be big enough for a catalog of a
    // sampler with the max samples allowed OR large enough for
    // the largest sysex dump you expect, sample parameters, drum settings, etc.
    unsigned char TempArray[TEMPCATBUFSIZ];

    // holds the processed catalog info
    unsigned char PermSampArray[sizeof(CAT)*MAX_SAMPS_S900];
    unsigned char PermProgArray[sizeof(CAT)*MAX_SAMPS_S900];

    // holds the Rx/Tx sample information
    unsigned char samp_parms[PARMSIZ];
    unsigned char samp_hedr[HEDRSIZ];

    // settings vars
    int g_baud;
    bool g_use_right_chan, g_auto_rename, g_force_hwflow;
//    bool g_target_S950;

    int g_byteCount;
    int g_numSampEntries;
    int g_numProgEntries;
    bool g_timeout;

    String g_DragDropFilePath;

protected:

BEGIN_MESSAGE_MAP
    //add message handler for WM_DROPFILES
    // NOTE: All of the TWM* types are in messages.hpp!
    VCL_MESSAGE_HANDLER(WM_DROPFILES, TWMDropFiles, WMDropFile)
END_MESSAGE_MAP(TComponent)

public:    // User declarations
    __fastcall TFormS900(TComponent* Owner);

};
//---------------------------------------------------------------------------
extern PACKAGE TFormS900 *FormS900;
//---------------------------------------------------------------------------
#endif
