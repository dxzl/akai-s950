//---------------------------------------------------------------------------
//Copyright 2008 Scott Swift - This program is distributed under the
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
//---------------------------------------------------------------------------
// defines

#define LOAD 1
#define SAVE 2

#define NUMITEMS 4
#define GETFILE  0
#define PUTFILE  1
#define SETTINGS 2
#define CONNECT  3

#define TEMP_BUF_SIZ 256
#define ACKTIMEOUT 9930000
#define FEXIST 2000
#define MAX_SAMPS_S900 32

#define WORDS_PER_BLOCK 60

/* magic number used to identify a file as compatible with this program */
#define MAGIC_NUM 453782108

/* S900 sample parameters storage */
#define PARMSIZ 129
/* S900 sample header storage */
#define HEDRSIZ 19  // starts with BEX but no EEX
#define ACKSIZ  4

/* File hearer info. */
#define MAGIC_SIZ   4
#define PSTOR_SIZ   68

#define BEX  ((unsigned char)0xF0)  /* beginning of exclusive */
#define EEX  ((unsigned char)0xF7)  /* end of EXCLUSIVE       */

#define AKAI_ID   ((unsigned char)0x47)  /* AKAI I.D.              */
#define S900_ID   ((unsigned char)64)    /* S900 I.D.              */
#define COMMON_ID ((unsigned char)0x7e)  /* system common I.D.     */

#define RSD   0     /* request sample dump                */
#define SD    1     /* sample dump                        */
#define RPRGM 2     /* request program/keygroups          */
#define RCAT  3     /* request prg/samp names             */
#define RSPRM 4     /* request sample parms               */
#define SECRE 5     /* request common reception enable    */
#define SECRD 6     /* request common reception disable   */
#define SPRM  10    /* sample parameters                  */
#define DCAT  11    /* catalog received                   */

#define ACKS  ((unsigned char)0x7f)  /* acknowledge sample block or header */
#define ASD   ((unsigned char)0x7d)  /* abort sample dump                  */
#define NAKS  ((unsigned char)0x7e)  /* not-acknowledge (request re-trans.)*/

#define MAX_NAME_S900 10

/* max, number of characters in sample menu title */
#define MAX_SAMP_NAME 16

#define S900_BITS_PER_SAMPLE 14

#define  DAC12UNSIGNED 4095 /* 2^12 / 2 - 1 */
#define  DAC12 2047 /* 2^12 / 2 - 1 */
#define  DAC16 32767 /* 2^16 / 2 - 1 */

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

//---------------------------------------------------------------------------
class TFormS900 : public TForm
{
__published:  // IDE-managed Components
    TMainMenu *MainMenu1;
    TMenuItem *S9001;
    TMenuItem *Cat1;
    TMenuItem *Get1;
    TMenuItem *Put1;
    TMemo *Memo1;
    TOpenDialog *OpenDialog1;
    TSaveDialog *SaveDialog1;
    TListBox *ListBox1;
    TMenuItem *Help1;
    TTimer *Timer1;
    TMenuItem *N1;
    TMenuItem *MenuSendRightChan;
    TMenuItem *N2;
    TApdComPort *ApdComPort1;
    
    void __fastcall Cat1Click(TObject *Sender);
    void __fastcall Put1Click(TObject *Sender);
    void __fastcall Get1Click(TObject *Sender);
    void __fastcall ListBox1Click(TObject *Sender);
    void __fastcall Help1Click(TObject *Sender);
    void __fastcall MenuUseRS232Click(TObject *Sender);
    void __fastcall MenuSendRightChanClick(TObject *Sender);
  void __fastcall FormCreate(TObject *Sender);

private:  // User declarations
    void __fastcall encode_parameters(int samp, PSTOR * ps);
    void __fastcall encode_parmsDB(unsigned char * source, unsigned char * dest, int numchars);
    void __fastcall encode_parmsDD(unsigned int value, unsigned char * tp);
    void __fastcall encode_parmsDW(unsigned int value, unsigned char * tp);
    void __fastcall encode_hedrTB(unsigned int value, unsigned char * tp);
    void __fastcall compute_checksum(void);
    int __fastcall findidx(int * samp, char * name);
    void __fastcall exmit(int samp, int mode);
    void __fastcall comws(int count, unsigned char * ptr);
    unsigned char __fastcall send_data(short int * intptr, PSTOR * ps);
    unsigned char __fastcall wav_send_data(unsigned short * intptr, PSTOR * ps);
    void __fastcall send_samp_parms(unsigned int index);
    void __fastcall printerror(String message);
    void __fastcall print_ps_info(PSTOR * ps);
    int __fastcall receive(int count);
    void __fastcall catalog(bool print);
    int __fastcall get_ack(void);
    void __fastcall trim(char * trimstr, char * sourcestr);

    int __fastcall get_sample(int samp, String Name);
    int __fastcall get_samp_hedr(int samp);
    void __fastcall decode_parameters(PSTOR * ps);
    int __fastcall get_samp_data(PSTOR * ps, int handle);
    int __fastcall getdata1(short int * bufptr, int max_bytes, PSTOR * ps);
    int __fastcall getdata2(unsigned char * bufptr, int max_bytes);
    void __fastcall decode_parmsDB(unsigned char * source, unsigned char * dest, int numchars);
    unsigned int __fastcall decode_parmsDD(unsigned char * tp);
    unsigned int __fastcall decode_parmsDW(unsigned char * tp);
    unsigned int __fastcall decode_hedrTB(unsigned char * tp);
    void __fastcall chandshake(int mode);
    void __fastcall cxmit(int samp, int mode);

    int ByteCount;
    int SampEntries;
    int ProgEntries;

    unsigned char TempArray[TEMPCATBUFSIZ];
    unsigned char PermSampArray[sizeof(CAT)*MAX_SAMPS_S900];
    unsigned char PermProgArray[sizeof(CAT)*MAX_SAMPS_S900];

    unsigned char samp_parms[PARMSIZ];
    unsigned char samp_hedr[HEDRSIZ];

public:    // User declarations
    __fastcall TFormS900(TComponent* Owner);

};
//---------------------------------------------------------------------------
extern PACKAGE TFormS900 *FormS900;
//---------------------------------------------------------------------------
#endif
