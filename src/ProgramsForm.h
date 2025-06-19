//---------------------------------------------------------------------------
#ifndef ProgramsFormH
#define ProgramsFormH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Grids.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.Dialogs.hpp>

//#include "MainForm.h"

#define PADROWS 3
#define PADCOLS 1

#define KEY_MIDDLE_C (24+12+12+12) // 60

#define S_SELECTED " (selected)"
//---------------------------------------------------------------------------
// Columns in grid
#define C_KEYGROUP          0
#define C_SS_COARSEPITCH    (3+PADCOLS)
#define C_SS_FINEPITCH      (4+PADCOLS)
#define C_MIDIOFFSET        (5+PADCOLS)
#define C_LOWKEY            (6+PADCOLS)
#define C_HIGHKEY           (7+PADCOLS)

//---------------------------------------------------------------------------

// Program header
#define PRNAME   7  // 10 bytes
#define PRUNDEF1 27 // DD
#define PRUNDEF2 35 // DW
#define KTILT    39 // DW (+/- 50)
#define PRUNDEF3 43 // DW
#define PRUNDEF4 47 // DB
#define POSXEN   49 // DB positional crossfade 0=disable, 1=enable
#define PRRESER1 51 // DB 51-52 reserved constant set to 255
#define NOKG     53 // DB number of keygroups 1-31
#define PRUNDEF5 55 // DW
#define MDPGNM   59 // DB (S950 only) Midi program # 0-127
#define MPLVD    61 // DB (S950 only) Enable MDPGNM (255), 0 = disable
#define PRRESER2 63 // DW 63-66
#define PRRESER3 67 // DD 67-74
#define PRRESER4 75 // DD 75-82

// Keygroups start at offset 83 (not including the 7-byte transmit header)

// Keygroup
#define UMK     0
#define LMK     2
#define VST     4
#define ATK     6
#define DCY     8
#define SSTN    10
#define RLSE    12
#define FVI     14
#define FKI     16
#define AVI     18
#define RVI     20
#define LVI     22
#define PVI     24
#define PAO     26
#define PST     28
#define VBDLY   30
#define VBRATE  32
#define VBDPTH  34
#define KBITS   36
#define OPVOICE 38
#define KMDCHN  40
#define AFDI    42
#define MWDI    44
#define VCFAMNT 46
#define NAMEFS  48
#define VCFAK   68
#define VCFDY   70
#define VCFST   72
#define VCFRL   74
#define VTMX    76
#define KYUDEF1 78 // DB 2 bytes
#define KYUDEF2 80 // DW
#define TROFFS  84
#define FLTFS   88
#define LOUDFS  90
#define NAMESS  92
#define KYUDEF3 112 // DD 8 bytes
#define KYUDEF4 120 // DD
#define TROFSS  128
#define FLTSS   132
#define LOUDSS  134
#define KYUDEF5 136 // DW 4 bytes

#define NEWPROGNAME "NewProg " // a ## number will be added...
#define NEWSAMPNAME "NewSamp " // a ## number will be added...
#define COPYMAGIC "83059138S950" // magic # used to validate clipboard keygroup
//---------------------------------------------------------------------------
// Scott Swift's .PRG file-format:

// d2 c3 20 26 (magic number)
// 01 00 00 00 (number of programs)

// Program 1
// e1 00 00 00 (program-size (including BEX, checksum and EEX))
// f0 47 00 09 40 00 00 (*0 sys ex, akai id, midi chan, PRGM code, S900 Id, Prg #, Reserved)
// 54 00 4f 00 4e 00 45 00 20 00 50 00 52 00 47 00 52 00 4d 00 (*7 prog name)
// 00 00 00 00 00 00 00 00 (*27 undefined)
// 00 00 00 00 (* 35 undefined)
// 00 00 00 00 (*39 KTILT +/- 50)
// 7e 00 44 01 (*43 undefined on S900)
// 00 00 (*47 undefined on S900)
// 00 00 (*49 positional xfade)
// 7f 01 (*51 reserved)
// 01 00 (*53 # keygroups in program)
// 00 00 00 00 (*55 undefined for S900)
// 00 00 (*59 program number for midi program change command (S950 only))
// 7f 01 (*61 0 means MDPGNM, above, is invalid (old S900 format), 255 enables it for S950)
// 00 00 00 00 (*63 reserved)
// 00 00 00 00 00 00 00 00 (*67 reserved)
// 00 00 00 00 00 00 00 00 (*75 reserved)

// (*83 keygroup 1)
// 7f 00 (*83 UMK)
// 18 00
// 00 01
// 00 00
// 50 00
// 63 00
// 1e 00
// 0a 00
// 32 00
// 00 00
// 00 00
// 1e 00
// 00 00
// 00 00
// 63 00
// 40 00
// 2a 00
// 00 00
// 04 00
// 7f 01
// 00 00
// 00 00
// 32 00
// 00 00 (*46 VCFAMNT)
// 54 00 4f 00 4e 00 45 00 20 00 20 00 20 00 20 00 20 00 20 00 (*48 NAMEFS)
// 14 00
// 14 00
// 14 00
// 14 00
// 40 00
// 00 00 (*78 undefined)
// 44 01 44 01 (*80 undefined on S900) (Could this be BCD form for fine-pitch?)
// 00 00 00 00 (*84 TROFFS)
// 63 00
// 00 00
// 32 00 20 00 53 00 41 00 4d 00 50 00 4c 00 45 00 20 00 20 00 (*92 NAMESS)
// 00 00 00 00 00 00 00 00 (*112 undefined on S900)
// 00 00 00 00 00 00 00 00 (*120 undefined on S900)
// 00 00 00 00 (*128 TROFSS)
// 63 00
// 00 00
// 00 00 00 00 (*136 undefined)

// (...subsequent keygroups for program 1 here...)

// (...subsequent programs here...)

// 38 f7 (checksum, EEX)
// f1 00 00 00 (total file size)
//---------------------------------------------------------------------------
typedef struct
{
  // all the Bytes are actually encoded to DB (2-bytes)
  Byte LowerMidiKey; // 24-127, 24
  Byte UpperMidiKey; // 24-127, 127
  Byte VelSwThresh; // 0-128, 128
  Byte AttackTime; // 0-99, 0
  Byte DecayTime; //0-99, 80
  Byte SustLevel; // 0-99, 99
  Byte RelTime; // 0-99, 30
  Byte FilterVelInter; // 0-99, 10
  Byte FilterKeyTracking; // 0-99, 50
  Byte AttackVelInter; // 0-99, 0
  signed char VelRelInter; // 0-255, 0, +/-50
  Byte LoudnessVelInter; // 0-99, 30
  Byte PitchWarpVelInter; // 0-99, 0
  signed char PitchWarpOffset; // 0-255, 0, +/- 50
  Byte PitchWarpRecoveryTime; // 0-99, 99
  Byte LfoBuildTime; // 0-99, 64
  Byte LfoRate; // 0-99, 42
  Byte LfoDepth; // 0-99, 0
  Byte ControlBits; // 0-transpose, 1-vel xfade, 2-vibrato desynch, 3- one-shot trigger mode, 4-velocity-release mode, 5-vel xfade curve modify, 4
  Byte VoiceOutAssign; // 0-9 and 255, 0-mono, 8-left group (0-3), 9-right group (4-7), 255-ALL, 255
  Byte MidiOffset; // 0-15, 0
  Byte AftertouchDepthMod; // 0-99, 0
  Byte ModWheelLfoDepthMod; // 0-99, 50
  signed char AdsrEnvToVcfFilter; // 0-255, 0 +/- 50
  char SoftSampleName[MAX_NAME_S900+1]; // 10 chars "TONE______"
  Byte FilterAdsrAttackTime; // 0-99, 20
  Byte FilterAdsrDecayTime; // 0-99, 20
  Byte FilterAdsrSustainLevel; // 0-99, 20
  Byte FilterAdsrReleaseTime; // 0-99, 20
  Byte VelXfadeFiftyPercent; // 0-127, 64
  Byte KyUndef1;
  uint16_t KyUndef2;
  Int16 SoftSampTransposeOffset; // Signed, units of 1/16 semitone, 0
  Byte SoftSampFilter; // 0-99, 99 gives highest cuttoff, 99
  signed char SoftSampLoudness; // 0-255, signed +/- 50 units of .375dB, 0
  char LoudSampleName[MAX_NAME_S900+1]; // 10 chars
  uint32_t KyUndef3;
  uint32_t KyUndef4;
  Int16 LoudSampTransposeOffset; // Signed, units of 1/16 semitone, 0
  Byte LoudSampFilter; // 0-99, 99 gives highest cuttoff, 99
  signed char LoudSampLoudness; // 0-255, signed +/- 50 units of .375dB, 0
  uint16_t KyUndef5;
} KEYGROUP;
//---------------------------------------------------------------------------
typedef struct
{
  char ProgName[MAX_NAME_S900+1]; // 10 chars "DEFAULT PR"
  uint32_t PrUndef1;
  uint16_t PrUndef2;
  Int16 KeyTilt; // Key vs Loudness -50 to +50
  uint16_t PrUndef3;
  Byte PrUndef4;
  Byte PosXfade; // positional crossfade 0=disable 1=enable
  Byte PrReser1; // 255
  Byte NumKeygroups; // Number of keygroups 1-31
  uint16_t PrUndef5;
  Byte MidiPgmNumber; // Midi program number 0-127
  Byte EnableMidiPgmNumber; // Enable the above midi # if 255, disable with 0 (0 on S900)
  uint16_t PrReser2; // 0
  uint32_t PrReser3; // 0
  uint32_t PrReser4; // 0
} PRGHEDR;
//---------------------------------------------------------------------------
class TFormProgram : public TForm
{
__published:    // IDE-managed Components
  TPanel *Panel1;
  TPanel *Panel2;
  TPanel *Panel3;
  TLabel *LabelProgName;
  TLabel *LabelKeyTilt;
  TEdit *EditKeyTilt;
  TButton *ButtonSend;
  TButton *ButtonClose;
  TStringGrid *SG;
  TMainMenu *MainMenu1;
  TMenuItem *Menu1;
  TMenuItem *DeleteKeygroup1;
  TMenuItem *CopyKeygroup1;
  TComboBox *ComboBoxProgNames;
  TEdit *EditMidiPrgNum;
  TLabel *Label1;
  TCheckBox *CheckBoxPosXfade;
  TMenuItem *MenuSaveIntoPrgFile;
  TMenuItem *MenuLoadFromPrgFile;
  TMenuItem *N1;
  TOpenDialog *OpenDialog1;
  TSaveDialog *SaveDialog1;
  TCheckBox *CheckBoxEnableProgNumber;
  TMenuItem *MenuDeleteProg;
  TMenuItem *N2;
  TMenuItem *MenuLoadFromMachine;
  TMenuItem *MenuSendToMachine;
  TButton *ButtonRefreshProgramsList;
  TButton *ButtonAddKg;
  TButton *ButtonDelKg;
  TMenuItem *MenuHelp;
  TButton *ButtonKgDefaults;
  TMenuItem *N3;
  TMenuItem *MenuShowUndefined;
  TMenuItem *MenuTools;
  TMenuItem *MenuDrumifyWhite;
  TMenuItem *MenuOctifyKG;
  TMenuItem *MenuMidifyKeygroups;
  TButton *ButtonSelectAll;
  TMenuItem *N4;
  TMenuItem *MenuClearSelected;
  TMenuItem *MenuDrumifyBlack;
  TMenuItem *MenuDrumify;
  TMenuItem *MenuSelectAll;
  TMenuItem *MenuDeleteSelectedKeygroups;
  TMenuItem *N5;
  TMenuItem *N6;
  TMenuItem *MenuCopyHilightedKeygroup;
  TMenuItem *MenuPasteHilightedKeygroup;
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall ButtonCloseClick(TObject *Sender);
  void __fastcall ButtonSendClick(TObject *Sender);
  void __fastcall DeleteKeygroup1Click(TObject *Sender);
  void __fastcall CopyKeygroup1Click(TObject *Sender);
  void __fastcall SGRowMoved(TObject *Sender, int FromIndex, int ToIndex);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall FormDestroy(TObject *Sender);
  void __fastcall MenuSaveIntoPrgFileClick(TObject *Sender);
  void __fastcall ComboBoxProgNamesSelect(TObject *Sender);
  void __fastcall MenuLoadFromPrgFileClick(TObject *Sender);
  void __fastcall MenuDeleteProgClick(TObject *Sender);
  void __fastcall MenuSendToMachineClick(TObject *Sender);
  void __fastcall MenuLoadFromMachineClick(TObject *Sender);
  void __fastcall ButtonRefreshProgramsListClick(TObject *Sender);
  void __fastcall ButtonAddKgClick(TObject *Sender);
  void __fastcall ButtonDelKgClick(TObject *Sender);
  void __fastcall MenuHelpClick(TObject *Sender);
  void __fastcall ButtonKgDefaultsClick(TObject *Sender);
  void __fastcall MenuShowUndefinedClick(TObject *Sender);
  void __fastcall FormShow(TObject *Sender);
  void __fastcall MenuDrumifyWhiteClick(TObject *Sender);
  void __fastcall MenuOctifyKGClick(TObject *Sender);
  void __fastcall MenuMidifyKeygroupsClick(TObject *Sender);
  void __fastcall SGMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
  void __fastcall ButtonSelectAllClick(TObject *Sender);
  void __fastcall MenuDrumifyBlackClick(TObject *Sender);
  void __fastcall MenuDrumifyClick(TObject *Sender);
  void __fastcall MenuSelectAllClick(TObject *Sender);
  void __fastcall MenuClearSelectedClick(TObject *Sender);
  void __fastcall MenuDeleteSelectedKeygroupsClick(TObject *Sender);
  void __fastcall MenuCopyHilightedKeygroupClick(TObject *Sender);
  void __fastcall MenuPasteHilightedKeygroupClick(TObject *Sender);

private:    // User declarations
  void __fastcall InitKeygroupCells(void);
  void __fastcall UpdateKeygroupNumbers(void);

  int __fastcall HeaderFromArray(void);
  int __fastcall HeaderToArray(int progIdx);
  int __fastcall HeaderToGui(void);
  int __fastcall HeaderFromGui(void);
  int __fastcall KeygroupFromGui(int index);
  int __fastcall KeygroupToGui(int index, bool bSkipNames=false);
  int __fastcall KeygroupFromArray(void);
  int __fastcall KeygroupToArray(void);
  int __fastcall KeygroupLoadDefaults(void);
  int __fastcall HeaderLoadDefaults(int numKeyGroups);
  String __fastcall GetFinePitch(Int16 rawPitch);
  String __fastcall GetCoarsePitch(Int16 rawPitch);
  String __fastcall GetNewProgName(String sName);
  int __fastcall ReadNamesInPrgFile(long lFileHandle, TStringList* sl);
  int __fastcall ProgFromFileToTempArray(long lFileHandle, int iFileOffset);
  int __fastcall ProgFromTempArrayToGui(void);
  int __fastcall ProgFromFileToGui(long lFileHandle, int iFileOffset);
  void __fastcall LoadOrDeleteProgramFromFile(bool bLoad);
  int __fastcall LoadProgramFromMachine(int iPrgIdx);
  int __fastcall DeletePrgFromFile(String sFilePath, long &lFileHandle, int iFileOffset);
  int __fastcall GridToTempArray(int progIdx);
  int __fastcall GridToFile(long lFileHandle);
  void __fastcall ClearStringGrid(void);
  void __fastcall SendProgramToMachine(void);
  int __fastcall RefreshProgramsInComboBox(int iSampIdx);
  void __fastcall RefreshKeygroupsFromSampleList(void);
  int __fastcall RefreshCatalog(void);
  int __fastcall CountSelected(void);
  void __fastcall SetButtonText(int iOldCount, int iNewCount);
  void __fastcall SelectAllKeygroups(bool bSelect);
  void __fastcall DeleteSelectedKeygroups(void);
  int __fastcall DeleteKeygroup(int iRow);

  TStringList* SampleData;
  TStringList* ProgramData;

  // fixed size of one keygroup (one row in grid) is 140 bytes
  // DB is 2 bytes DW is 4 bytes, DD is 8 bytes, sample name is 10 DBs
  Byte kg[PROGKEYGROUPSIZ]; // encoded array for midi transmission
  KEYGROUP KeyGroup; // current working keygroup struct

  // Programs header info
  Byte pg[PROGHEADERSIZ]; // encoded array for midi transmission
  PRGHEDR ProgramHeader; // current working program header struct

  int m_progIndex, m_selectCount;

public:        // User declarations
    __fastcall TFormProgram(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormProgram *FormProgram;
//---------------------------------------------------------------------------
//class TGridCracker : public TStringGrid
//{
//public:
//    __property InplaceEditor;
//};
//---------------------------------------------------------------------------
#endif
