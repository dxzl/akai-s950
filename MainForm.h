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

//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
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
private:	// User declarations
public:		// User declarations
    __fastcall TForm1(TComponent* Owner);

// functions prototypes
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
