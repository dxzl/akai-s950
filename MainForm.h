//---------------------------------------------------------------------------
//Software written by Scott Swift 2016 - This program is distributed under the
//terms of the GNU General Public License.
//---------------------------------------------------------------------------
#ifndef MainFormH
#define MainFormH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Menus.hpp>
#include "AdPort.hpp"
#include "OoMisc.hpp"

// defines
//---------------------------------------------------------------------------
#define VERSION_STR "Version 1.39: June 11, 2017"
//---------------------------------------------------------------------------

#define REGISTRY_KEY "\\Software\\Discrete-Time Systems\\AkaiS900"

// Registry entries we save in HKEY_CURRENT_USER
#define S9_REGKEY_VERSION "Version"
#define S9_REGKEY_BAUD "Baud"
#define S9_REGKEY_USE_RIGHT_CHAN "UseRightChan"
#define S9_REGKEY_AUTO_RENAME "AutoRename"
#define S9_REGKEY_FORCE_HWFLOW "ForceHWFlowControl"
//#define S9_REGKEY_TARGET_S950 "TargetS950"
//#define S9_REGKEY_USE_SMOOTH_QUANTIZATION "UseSmoothQuantization"

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
#define MAX_SAMPS 100 // S900 is 32 but S950 is "99" (I think it's 100 with TONE)
#define MAX_PROGS 100 // S900 is 32 but S950 is "99" (I think it's 100 with TONE)

#define DATA_PACKET_SIZE 120
#define S900_BITS_PER_WORD 14
#define S950_BITS_PER_WORD 16

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

#define UINT16SIZE ((int)sizeof(unsigned __int16))
#define UINT32SIZE ((int)sizeof(unsigned __int32))

#define BEX  ((Byte)0xF0)  // beginning of exclusive
#define EEX  ((Byte)0xF7)  // end of EXCLUSIVE

#define AKAI_ID   ((Byte)0x47)  // AKAI I.D.
#define S900_ID   ((Byte)64)    // S900 I.D.
#define COMMON_ID ((Byte)0x7e)  // system common I.D.

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
#define ACKS  ((Byte)0x7f)  // acknowledge sample block or header
#define ASD   ((Byte)0x7d)  // abort sample dump
#define NAKS  ((Byte)0x7e)  // not-acknowledge (request re-trans.)*/

// the number of chars the S900 can handle, is 10 chars max
#define MAX_NAME_S900 10

#define DAC12UNSIGNED 4095 // 2^12 / 2 - 1
#define DAC12 2047 // 2^12 / 2 - 1
#define DAC16 32767 // 2^16 / 2 - 1

// this TempArray is used for all receive() data and needs to be big enough
// for the largest single transmission from the target machine
// (we need 122 bytes for a data packet)
// 2400 bytes for the largest possible catalog...
#define TEMPCATBUFSIZ (sizeof(S900CAT)*(MAX_SAMPS+MAX_PROGS) + 9)

// Delay (in ms) between sending individual programs from a .prg file
#define DELAY_BETWEEN_PROGRAMS 125

// this struct can be modified if you need to rearrange it or add fields...
typedef struct
{
  char name[MAX_NAME_S900+1];
  int sampidx;
} CAT;

// this struct MUST be 12 bytes and always be in this order!
// (it reflects the cat data returned by the target machine)
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
  unsigned __int16 bits_per_word; // (2 bytes)
  char name[PSTOR_NAME_COUNT]; // ASCII sample name (14 bytes)
  char spares[PSTOR_SPARE_COUNT]; // unused bytes (12 bytes)
  __int32 spareint2; // signed value (4 bytes)
  __int32 loudnessoffset; // signed value (4 bytes)
  Byte sparechar1; // (1 byte)
  Byte sparechar2; // (1 byte)
  Byte flags; // (1 byte)
  Byte loopmode; // (1 byte)
} PSTOR; // sizeof(PSTOR) must always be 72 bytes!

//---------------------------------------------------------------------------
class TFormS900 : public TForm
{
__published:  // IDE-managed Components
	TMainMenu *MainMenu1;
	TTimer *Timer1;
	TOpenDialog *OpenDialog1;
	TSaveDialog *SaveDialog1;
	TApdComPort *ApdComPort1;
	TPanel *Panel1;
	TPanel *Panel2;
	TComboBox *ComboBox1;
	TListBox *ListBox1;
	TMemo *Memo1;
	TMenuItem *S9001;
	TMenuItem *MenuGetCatalog;
	TMenuItem *N4;
	TMenuItem *MenuGetSample;
	TMenuItem *MenuPutSample;
	TMenuItem *N3;
	TMenuItem *MenuGetPrograms;
	TMenuItem *MenuPutPrograms;
	TMenuItem *N1;
	TMenuItem *MenuUseRightChanForStereoSamples;
	TMenuItem *MenuAutomaticallyRenameSample;
	TMenuItem *N2;
	TMenuItem *MenuUseHWFlowControlBelow50000Baud;
	TMenuItem *Help1;
	void __fastcall MenuGetCatalogClick(TObject *Sender);
	void __fastcall MenuGetSampleClick(TObject *Sender);
	void __fastcall MenuPutSampleClick(TObject *Sender);
	void __fastcall MenuGetProgramsClick(TObject *Sender);
	void __fastcall MenuPutProgramsClick(TObject *Sender);
	void __fastcall MenuUseRightChanForStereoSamplesClick(TObject *Sender);
	void __fastcall MenuAutomaticallyRenameSampleClick(TObject *Sender);
	void __fastcall MenuUseHWFlowControlBelow50000BaudClick(TObject *Sender);
	void __fastcall Help1Click(TObject *Sender);

	void __fastcall ListBox1Click(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall ComboBox1Change(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);

private:  // User declarations
	void __fastcall Timer1FileDropTimeout(TObject *Sender);
    void __fastcall Timer1RxTimeout(TObject *Sender);
	void __fastcall Timer1GpTimeout(TObject *Sender);

    bool __fastcall IsGpTimeout(void);
    void __fastcall StopGpTimer(void);
    void __fastcall DelayGpTimer(int time);
    void __fastcall StartGpTimer(int time);

    void __fastcall PutSample(String sFilePath);
    int __fastcall GetSample(int samp, String Name);
    void __fastcall SetComPort(int baud);
    int __fastcall FindIndex(char* pName);
    String __fastcall GetFileName(void);
    bool __fastcall StrCmpCaseInsens(char* sA, char* sB, int len);
    __int32 __fastcall FindSubsection(Byte* &fileBuffer, char* chunkName, UINT maxBytes);

	void __fastcall encode_sample_info(int samp, PSTOR* ps);
    void __fastcall decode_sample_info(PSTOR* ps);

    void __fastcall encode_parmsDB(Byte c, Byte* dest);
    void __fastcall encode_parmsDB(Byte* source, Byte* dest, int numchars);
    Byte __fastcall decode_parmsDB(Byte* source);
    void __fastcall decode_parmsDB(Byte* dest, Byte* source, int numchars);

    void __fastcall encode_parmsDD(unsigned __int32 value, Byte* tp);
    unsigned __int32 __fastcall decode_parmsDD(Byte* tp);

    void __fastcall encode_parmsDW(unsigned __int32 value, Byte* tp);
    unsigned __int32 __fastcall decode_parmsDW(Byte* tp);

    void __fastcall encode_hedrTB(unsigned __int32 value, Byte* tp);
    unsigned __int32 __fastcall decode_hedrTB(Byte* tp);

    void __fastcall compute_checksum(int min_index, int max_index);
    int __fastcall findidx(char* sampName);
    void __fastcall xmit(unsigned __int16 val, int bytes_per_word,
                               int bits_per_word, Byte &checksum);
    void __fastcall send_samp_parms(unsigned int index);
    void __fastcall printm(String message);
    void __fastcall print_ps_info(PSTOR* ps);
    int __fastcall receive(int count);
    void __fastcall catalog(bool print);
    void __fastcall trimright(char* pStr);

    void __fastcall chandshake(int mode);
    int __fastcall get_ack(int blockct);
    void __fastcall exmit(int samp, int mode, bool bDelay);
    void __fastcall cxmit(int samp, int mode, bool bDelay);
    void __fastcall comws(int count, Byte* ptr, bool bDelay);

	int __fastcall get_samp_data(PSTOR * ps, int handle);
    int __fastcall get_comm_samp_data(__int16* bufptr, int bytes_per_word,
                 int samples_per_block, int bits_per_word, int blockct);
    int __fastcall get_comm_samp_parms(int samp);
    int __fastcall get_comm_samp_hedr(int samp);
    void __fastcall WMDropFile(TWMDropFiles &Msg);

    // receive data buffer, needs to be big enough for a catalog of a
    // sampler with the max samples allowed OR large enough for
    // the largest sysex dump you expect, sample parameters, drum settings, etc.
    Byte TempArray[TEMPCATBUFSIZ];

    // holds the processed catalog info
    Byte PermSampArray[sizeof(CAT)*MAX_SAMPS];
    Byte PermProgArray[sizeof(CAT)*MAX_PROGS];

    // holds the Rx/Tx sample information
    Byte samp_parms[PARMSIZ];
    Byte samp_hedr[HEDRSIZ];

    // settings vars
    int m_baud;
    bool m_use_right_chan, m_auto_rename, m_force_hwflow;
    bool m_target_S950, m_use_smooth_quantization;

    int m_byteCount;
    int m_numSampEntries;
    int m_numProgEntries;
    bool m_rxTimeout, m_gpTimeout, m_systemBusy;

    String m_DragDropFilePath;

protected:

BEGIN_MESSAGE_MAP
    //add message handler for WM_DROPFILES
    // NOTE: All of the TWM* types are in messages.hpp!
    VCL_MESSAGE_HANDLER(WM_DROPFILES, TWMDropFiles, WMDropFile)
END_MESSAGE_MAP(TComponent)

public:    // User declarations
    __fastcall TFormS900(TComponent* Owner);

    __property bool SystemBusy = {read = m_systemBusy};
};
//---------------------------------------------------------------------------
extern PACKAGE TFormS900 *FormS900;
//---------------------------------------------------------------------------
#endif
