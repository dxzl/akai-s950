//---------------------------------------------------------------------------

#ifndef OverallSettingsFormH
#define OverallSettingsFormH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
//---------------------------------------------------------------------------
#define OSSIZ (7+80+2)

// NOTE: there are many reserved fields so we need to preserve them
// as read when writing overall setting back to the machine!
#define PRONAME  7   // DB*10
#define MDXTCH   39  // DB Midi transmit channel for Akai exclusive
#define RSCHNL   47  // DW Reception simulator Midi channel (1-16)
#define RSKEY    51  // DW Reception simulator midi key (0-127)
#define RSVEL    55  // DW Reception simulator velocity (0-127)
#define BASMCH   61  // DB bit 7 (128) set is omni mode (all channels) (0-15)
#define MLEN     63  // DB reception of loudness on continuous controller 7 0=disable
#define M1RS2    65  // DB 1=midi, 2=RS232
#define MPEN     67  // DB 0=disable, anything else = enable
#define OSCONST1 69  // DW 20727
#define OSCONST2 73  // DW 7238
#define PWRANGE  77  // DB (7) pitch wheel range (either up or down) (0-12)
#define RSBAUD   79  // DW Nominal RS-232 baud rate in 1/10 (e.g. 960 means 9600 baud)
//---------------------------------------------------------------------------
typedef struct
{
  char ProgName[MAX_NAME_S900+1]; // (DEFAULT PR) 10 chars "TONE_PRGM_"
  Byte MidiTxEx; // (0) Midi transmit channel for midi exclusive
  Byte RxSimChan; // (1) Reception simulator
  Byte RxSimKey; // (60) " Midi Key
  Byte RxSimVel; // (64) " Velocity
  Byte MidiChan; // 0
  bool bOmniOn; // (this flag is from the MSB of BSAMCH)
  bool bRxLoudCtrl7; // (0)
  bool bCtrlByRs232; // (0) Midi/Rs232 select
  Byte PitchWheelRange; // (7) semitones up or down +/- 7 is default (0-127)
  uint32_t BaudRate; // (960) 9600
} OS;
//---------------------------------------------------------------------------
class TFormOverallSettings : public TForm
{
__published:  // IDE-managed Components
  TPanel *Panel1;
  TEdit *EditRxSimKey;
  TEdit *EditRxSimVel;
  TEdit *EditPitchWheelRange;
  TCheckBox *CheckBoxOmni;
  TCheckBox *CheckBoxLoudnessOnMidi7;
  TCheckBox *CheckBoxMidiProgChange;
  TComboBox *ComboBoxProgNames;
  TComboBox *ComboBoxMidiExTx;
  TLabel *LabelMidiExTx;
  TLabel *LabelProgName;
  TComboBox *ComboBoxRxSimChan;
  TLabel *LabelRxSimChan;
  TLabel *LabelRxSimKey;
  TLabel *LabelRxSimVel;
  TComboBox *ComboBoxBaseMidiChan;
  TLabel *LabelBaseMidiChan;
  TComboBox *ComboBoxBaudRate;
  TLabel *LabelBaudRate;
  TRadioGroup *RadioGroupMidiRs232;
  TLabel *LabelPitchWheelRange;
  TButton *ButtonClose;
  TButton *ButtonSend;
  TButton *ButtonRefresh;
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall FormDestroy(TObject *Sender);
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall ButtonSendClick(TObject *Sender);
  void __fastcall ButtonCloseClick(TObject *Sender);
  void __fastcall ButtonRefreshClick(TObject *Sender);
  void __fastcall FormShow(TObject *Sender);
private:  // User declarations

  int __fastcall RefreshAll(void);
  int __fastcall RefreshCatalog(void);
  int __fastcall LoadFromMachine(void);
  int __fastcall SendToMachine(void);
  int __fastcall FromArray(void);
  int __fastcall ToArray(void);
  int __fastcall LoadDefaults(void);
  int __fastcall ToGui(void);
  int __fastcall FromGui(void);

  TStringList* ProgramData;

  // Overall settings info
  Byte os[OSSIZ]; // encoded array for overall settings
  OS OverallSettings; // current working overall settings struct

public:    // User declarations
  __fastcall TFormOverallSettings(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormOverallSettings *FormOverallSettings;
//---------------------------------------------------------------------------
#endif
