//---------------------------------------------------------------------------

#ifndef SampParmsFormH
#define SampParmsFormH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.ExtCtrls.hpp>

#include "MainForm.h"
#include <Vcl.Dialogs.hpp>
#include <Vcl.ComCtrls.hpp>
//---------------------------------------------------------------------------
#define SOURCE_UNDEFINED 0
#define SOURCE_FILE 1
#define SOURCE_MACHINE 2

#define SOS_SEND "Send To Machine"
#define SOS_SAVE "Save To File"

#define ONSELECT_DELAY_TIME 500 // 500ms
//---------------------------------------------------------------------------
class TFormEditSampParms : public TForm
{
__published:  // IDE-managed Components
  TMainMenu *MainMenu1;
  TPanel *Panel1;
  TPanel *Panel2;
  TComboBox *ComboBoxSampNames;
  TButton *ButtonRefreshSamplesList;
  TLabel *LabelSampleName;
  TRadioGroup *RadioGroupWaveForm;
  TRadioGroup *RadioGroupReplayMode;
  TEdit *EditLoudness;
  TLabel *LabelLoudnessOffset;
  TEdit *EditEnd;
  TLabel *LabelEnd;
  TEdit *EditStart;
  TEdit *EditLoop;
  TLabel *LabelStart;
  TLabel *LabelLoop;
  TButton *ButtonSendOrSave;
  TButton *ButtonClose;
  TEdit *EditNominalPitch;
  TLabel *LabelNominalPitch;
  TMenuItem *MenuOptions;
  TMenuItem *MenuLoadFromAkiFile;
  TMenuItem *MenuSave;
  TMenuItem *Help;
  TEdit *EditFinePitch;
  TLabel *Label1;
  TOpenDialog *OpenDialog1;
  TTrackBar *TBStart;
  TTrackBar *TBEnd;
  TTrackBar *TBLoop;
  TLabel *LabelSampleRate;
  TLabel *LabelSampleLength;
  TUpDown *UpDownStart;
  TUpDown *UpDownEnd;
  TUpDown *UpDownLoop;
  TLabel *LabelInfo;
  TLabel *LabelStartTimeL;
  TLabel *LabelEndTimeL;
  TLabel *LabelLoopTimeL;
  TLabel *LabelStartTime;
  TLabel *LabelEndTime;
  TLabel *LabelLoopTime;
  TLabel *LabelLoopLength;
  TLabel *LabelSampleTime;
  TLabel *LabelBitsPerSample;
  TTimer *TimerTriggerOnSelect;
  void __fastcall HelpClick(TObject *Sender);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall ButtonCloseClick(TObject *Sender);
  void __fastcall FormDestroy(TObject *Sender);
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall ButtonRefreshSamplesListClick(TObject *Sender);
  void __fastcall MenuLoadFromAkiFileClick(TObject *Sender);
  void __fastcall ComboBoxSampNamesSelect(TObject *Sender);
  void __fastcall TBStartChange(TObject *Sender);
  void __fastcall TBEndChange(TObject *Sender);
  void __fastcall TBLoopChange(TObject *Sender);
  void __fastcall UpDownStartChangingEx(TObject *Sender, bool &AllowChange, int NewValue,
          TUpDownDirection Direction);
  void __fastcall UpDownEndChangingEx(TObject *Sender, bool &AllowChange, int NewValue,
          TUpDownDirection Direction);
  void __fastcall UpDownLoopChangingEx(TObject *Sender, bool &AllowChange, int NewValue,
          TUpDownDirection Direction);
  void __fastcall MenuSaveClick(TObject *Sender);
  void __fastcall FormShow(TObject *Sender);
  void __fastcall TimerTriggerOnSelectEvent(TObject *Sender);

private:  // User declarations
  int __fastcall RefreshCatalog(void);
  int __fastcall RefreshSamplesInComboBox(int iSampIdx);
  int __fastcall SendParmsToMachine(int iSampIdx);
  int __fastcall ParmsToGui(int iMode);
  int __fastcall ParmsFromGui(void);
  int __fastcall ParmsToArray(uint16_t index);
  int __fastcall ParmsFromArray(void);
  int __fastcall LoadSampParmsFromMachine(int iSampIdx);
  int __fastcall LoadSampParmsFromFile(void);
  int __fastcall SaveSampParmsToFile(void);
  void __fastcall ComputeTimes(void);
  void __fastcall DisableEvents(void);
  void __fastcall EnableEvents(void);

  TStringList* m_slSampleData;

  Byte m_samp_parms[PARMSIZ]; // encoded array for midi transmission
  PSTOR m_ps; // current working sample parameters struct

  int m_iSource; // 0=nothing, 1=file, 2=machine
  int m_sampIndex;

  String m_sFilePath;

public:    // User declarations
  __fastcall TFormEditSampParms(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormEditSampParms *FormEditSampParms;
//---------------------------------------------------------------------------
#endif
