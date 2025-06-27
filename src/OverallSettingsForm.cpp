//---------------------------------------------------------------------------
#include <vcl.h>
#include "MainForm.h"
#pragma hdrstop

#include "OverallSettingsForm.h"
#include "S9Utils.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormOverallSettings *FormOverallSettings;
using namespace S9Utils;
//---------------------------------------------------------------------------
__fastcall TFormOverallSettings::TFormOverallSettings(TComponent* Owner)
  : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormOverallSettings::FormClose(TObject *Sender, TCloseAction &Action)

{
  Release();
}
//---------------------------------------------------------------------------
void __fastcall TFormOverallSettings::FormDestroy(TObject *Sender)
{
  if (ProgramData) delete ProgramData;
}
//---------------------------------------------------------------------------
void __fastcall TFormOverallSettings::FormCreate(TObject *Sender)
{
  ButtonSend->Enabled = false;

  ProgramData = NULL;

  int iError = RefreshAll();

  if (iError < 0)
  {
    if (iError != -1)
      ShowMessage("Unable to refresh settings! (code=" + String(iError) + ")");

    if (LoadDefaults() < 0)
      return;

    iError = ToGui(); // Display OverallSettings struct

    if (iError < 0)
    {
      if (iError != -1)
        ShowMessage("Unable to load from machine (ToGui())! (code=" + String(iError) + ")");
      return;
    }

    ComboBoxBaudRate->Hint = "Sets a new baud-rate at both the machine and in this program\n"
                             "when you click \"Send\". If you change this and communication\n"
                             "stops working, you must reset the baud rate manually at both the\n"
                             "machine and in the main window of this program!";
    ShowMessage("Defaults loaded. You need to press \"Refresh\" once the machine is connected...");
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormOverallSettings::FormShow(TObject *Sender)
{
  // Drop the program-name list down
  if (ComboBoxProgNames->Items->Count && !ComboBoxProgNames->DroppedDown)
    ComboBoxProgNames->DroppedDown = true;
}
//---------------------------------------------------------------------------
// return 0 if success
int __fastcall TFormOverallSettings::RefreshCatalog(void)
{
  try
  {
    int iError = FormMain->GetCatalog();

    if (iError < 0)
    {
      if (iError == -2)
        ShowMessage("Transmit timeout requesting catalog!");
      else if (iError == -3)
        ShowMessage("Timeout receiving catalog!");
      else if (iError == -4)
        ShowMessage("Incorrect catalog received!");
      else
        ShowMessage("Not able to get catalog, check power and cables... (code=" + String(iError) + ")");
      return -1;
    }

    // Invoke property getters in FormMain that create string-lists.
    // (MUST delete on destroy or prior to reinvoking!)
    if (ProgramData)
      delete ProgramData;
    ProgramData = FormMain->CatalogProgramData;
    if (!ProgramData)
      return -2;

    return 0;
  }
  catch(...)
  {
    return -100;
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormOverallSettings::ButtonCloseClick(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormOverallSettings::ButtonRefreshClick(TObject *Sender)
{
  if (IsBusy())
    return;

  try
  {
    FormMain->BusyCount++;

    int iError = RefreshAll();

    if (iError < 0)
    {
      if (iError != -1)
        ShowMessage("Unable to refresh settings! (code=" + String(iError) + ")");
    }
  }
  __finally
  {
    FormMain->BusyCount--;
  }
}
//---------------------------------------------------------------------------
// return 0 if success, negative if fail
int __fastcall TFormOverallSettings::RefreshAll(void)
{
  ButtonSend->Enabled = false;

  // load catalog into ProgramData
  int iError = RefreshCatalog();
  if (iError < 0)
  {
    if (iError != -1)
      ShowMessage("Unable to load catalog! (code=" + String(iError) + ")");

    return -1;
  }

  // add programs to combo box with index in the objects list
  ComboBoxProgNames->Items->Assign(ProgramData);

  // load overall settings
  iError = LoadFromMachine();
  if (iError < 0)
  {
    if (iError != -1)
      ShowMessage("Unable to load overall settings! (code=" + String(iError) + ")");

    return -1;
  }

  ButtonSend->Enabled = true;

  return 0;
}
//---------------------------------------------------------------------------
void __fastcall TFormOverallSettings::ButtonSendClick(TObject *Sender)
{
  int iError = SendToMachine();
  if (iError < 0)
  {
    if (iError != -1)
      ShowMessage("Unable to send overall settings! (code=" + String(iError) + ")");
    return;
  }
}
//---------------------------------------------------------------------------
int __fastcall TFormOverallSettings::SendToMachine(void)
{
  // don't allow multiple button presses
  if (IsBusy())
    return -1;

  try
  {
    FormMain->BusyCount++;

    try
    {
      // Save the machine's baud-rate that's working right now...
      uint32_t OldBaudRate = OverallSettings.BaudRate;

      int iError = FromGui(); // Load OverallSettings struct from GUI

      if (iError < 0)
      {
        if (iError != -1)
          ShowMessage("Unable to send to machine (FromGui())! (code=" + String(iError) + ")");
        return -1;
      }

      // convert OverallSettings struct to os[]
      iError = ToArray();

      if (iError < 0)
      {
        if (iError != -1)
          ShowMessage("Unable to send to machine (ToArray())! (code=" + String(iError) + ")");
        return -1;
      }

      // send overall settings to Akai S950/S900, with delay
      if (!FormMain->comws(OSSIZ, os, true))
      {
        ShowMessage("Unable to send... try again or close...");
        return -1;
      }

      // trigger baud rate change in FormMain if user just changed baud-rate on machine!
      if (OverallSettings.BaudRate != OldBaudRate)
      {
        // add timeout seconds for slower baud-rate...
        int iTimeoutTime = OldBaudRate < 19200 ? TXTIMEOUT+2 : TXTIMEOUT;

        // wait for comws() above to complete...
        if (FormMain->WaitTxCom(iTimeoutTime) < 0)
        {
          ShowMessage("Unable to verify empty transmit buffer...");
          return -1;
        }

        FormMain->BaudRate = OverallSettings.BaudRate;
      }
    }
    catch(...)
    {
      return -100;
    }
  }
  __finally
  {
    FormMain->BusyCount--;
  }

  return 0;
}
//---------------------------------------------------------------------------
int __fastcall TFormOverallSettings::LoadFromMachine(void)
{
  try
  {
    // receive overall settings from Akai S950/S900
    int iError = FormMain->LoadOverallSettingsToTempArray(); // get the overall settings into TempArray

    if (iError < 0)
    {
      switch(iError)
      {
        case -2:
            ShowMessage("transmit timeout!");
        break;
        case -3:
            ShowMessage("timeout receiving sample parameters!");
        break;
        case -4:
            ShowMessage("received wrong bytecount for overall settings!");
        break;
        case -5:
            ShowMessage("bad checksum for overall settings!");
        break;
        case -6:
          ShowMessage("exception in LoadOverallSettingsToTempArray()!");
        break;
        default:
        break;
      };
      return -1; // could not get sample parms
    }

    // copy TempArray to os[]
    memcpy(os, FormMain->TempArray, OSSIZ);

    // convert os[] to OverallSettings struct
    iError = FromArray();

    if (iError < 0)
    {
      if (iError != -1)
        ShowMessage("Unable to load from machine (FromArray())! (code=" + String(iError) + ")");
      return -1;
    }

    iError = ToGui(); // Display OverallSettings struct

    if (iError < 0)
    {
      if (iError != -1)
        ShowMessage("Unable to load from machine (ToGui())! (code=" + String(iError) + ")");
      return -1;
    }

    return 0;
  }
  catch(...)
  {
    return -100;
  }
}
//---------------------------------------------------------------------------
// encodes Program Header into the os[7+80+2] (akai-formatted) array
int __fastcall TFormOverallSettings::ToArray(void)
{
  try
  {
    // Clear array to all 0
    // in this case - we don't clear because we need to retain the reserved
    // data that we loaded from the machine when the form was created...
    //memset(os, 0, OSSIZ);

    os[0] = BEX;
    os[1] = AKAI_ID;
    os[2] = 0;
    os[3] = OVS; // overall settings
    os[4] = S900_ID;
    os[5] = 0; // undefined
    os[6] = 0;

    // copy ASCII sample name, pad with blanks and terminate
    AsciiStrEncode(&os[PRONAME], OverallSettings.ProgName);

    encodeDB(OverallSettings.MidiTxEx , &os[MDXTCH]);
    encodeDW(OverallSettings.RxSimChan, &os[RSCHNL]);
    encodeDW(OverallSettings.RxSimKey, &os[RSKEY]);
    encodeDW(OverallSettings.RxSimVel, &os[RSVEL]);
    uint8_t temp = OverallSettings.MidiChan | (OverallSettings.bOmniOn ? 128 : 0);
    encodeDB(temp, &os[BASMCH]);
    temp = OverallSettings.bRxLoudCtrl7 ? 1 : 0;
    encodeDB(temp, &os[MLEN]);
    temp = OverallSettings.bCtrlByRs232 ? 2 : 1;
    encodeDB(temp, &os[M1RS2]);

    // here, at least for the S900, we have two reserved constants
// Don't need to set these if not clearing os (above)
//    encodeDW(20727, &os[OSCONST1]);
//    encodeDW(7238, &os[OSCONST2]);

    encodeDB(OverallSettings.PitchWheelRange, &os[PWRANGE]);
    encodeDW(OverallSettings.BaudRate/10, &os[RSBAUD]);

    // checksum is exclusive or of 7-86 (80 bytes)
    // and the value is put in index 87
    compute_checksum(7, 87, os);

    os[88] = EEX;

    return 0;
  }
  catch(...)
  {
    return -100;
  }
}
//---------------------------------------------------------------------------
// decodes ProgHeader (the working-set ProgramHeader.ProgName struct) from the
// pg[76+7] (akai-formatted) array
int __fastcall TFormOverallSettings::FromArray(void)
{
  try
  {
    // ASCII sample name
    AsciiStrDecode(OverallSettings.ProgName, &os[PRONAME]);

    OverallSettings.MidiTxEx = decodeDB(&os[MDXTCH]);
    OverallSettings.RxSimChan = decodeDW(&os[RSCHNL]);
    OverallSettings.RxSimKey = decodeDW(&os[RSKEY]);
    OverallSettings.RxSimVel = decodeDW(&os[RSVEL]);

    uint8_t temp = decodeDB(&os[BASMCH]);
    OverallSettings.MidiChan = (temp & 0x7f); // mask msb (flag)
    OverallSettings.bOmniOn = (temp & 0x80);

    temp = decodeDB(&os[MLEN]);
    OverallSettings.bRxLoudCtrl7 = (temp == 0 ? false : true);

    temp = decodeDB(&os[M1RS2]);
    OverallSettings.bCtrlByRs232 = (temp == 2 ? true : false);

    OverallSettings.PitchWheelRange = decodeDB(&os[PWRANGE]);
    OverallSettings.BaudRate = decodeDW(&os[RSBAUD])*10;

    return 0;
  }
  catch(...)
  {
    return -100;
  }
}
//---------------------------------------------------------------------------
// Put ProgramHeader info to GUI
int __fastcall TFormOverallSettings::FromGui(void)
{
  try
  {
    // Selected program name
    AnsiString sTemp = ComboBoxProgNames->Text.TrimRight();

    if (!NameOk(sTemp))
    {
      ShowMessage("Name must be 1-10 ASCII characters!");
      ComboBoxProgNames->SetFocus();
      return -1;
    }

    memset(OverallSettings.ProgName, 0, MAX_NAME_S900+1);
    strcpy(OverallSettings.ProgName, sTemp.c_str());

    // (0) Midi transmit channel for midi exclusive
    int iTemp = ComboBoxMidiExTx->Text.ToIntDef(-1);
    if (iTemp < 1 || iTemp > 16)
    {
      ShowMessage("Invalid entry. Midi number is 1-16!");
      ComboBoxMidiExTx->SetFocus();
      return -1;
    }
    OverallSettings.MidiTxEx = iTemp-1; // 0-15 so subtract 1

    // (1) Reception simulator channel
    // (spec says this is 1-16, not 0-15 like the basic channel)
    iTemp = ComboBoxRxSimChan->Text.ToIntDef(-1);
    if (iTemp < 1 || iTemp > 16)
    {
      ShowMessage("Invalid entry. Midi number is 1-16!");
      ComboBoxRxSimChan->SetFocus();
      return -1;
    }
    OverallSettings.RxSimChan = iTemp; // 1-16

    // (60) Reception simulator key
    iTemp = EditRxSimKey->Text.ToIntDef(-1);
    if (iTemp < 0 || iTemp > 127)
    {
      ShowMessage("Invalid entry. Reception simulator key is 0-127!");
      EditRxSimKey->SetFocus();
      return -1;
    }
    OverallSettings.RxSimKey = iTemp;

    // (64) Reception simulator velocity
    iTemp = EditRxSimVel->Text.ToIntDef(-1);
    if (iTemp < 1 || iTemp > 127)
    {
      ShowMessage("Invalid entry. Reception simulator velocity is 1-127!");
      EditRxSimVel->SetFocus();
      return -1;
    }
    OverallSettings.RxSimVel = iTemp;

    // Basic (base) midi channel
    iTemp = ComboBoxBaseMidiChan->Text.ToIntDef(-1);
    if (iTemp < 1 || iTemp > 16)
    {
      ShowMessage("Invalid entry. Midi number is 1-16!");
      ComboBoxBaseMidiChan->SetFocus();
      return -1;
    }
    OverallSettings.MidiChan = iTemp-1; // 0-15 so subtract 1

    // Omni midi-mode (respond on any channel)
    OverallSettings.bOmniOn = CheckBoxOmni->Checked;
    OverallSettings.bRxLoudCtrl7 = CheckBoxLoudnessOnMidi7->Checked;
    OverallSettings.bCtrlByRs232 = (RadioGroupMidiRs232->ItemIndex == 1) ? true : false;

    iTemp = EditPitchWheelRange->Text.ToIntDef(-1);
    if (iTemp < 0 || iTemp > 12)
    {
      ShowMessage("Invalid entry. Pitch wheel range is 0-12!");
      EditPitchWheelRange->SetFocus();
      return -1;
    }
    OverallSettings.PitchWheelRange = iTemp;

    // Baud rate
    iTemp = ComboBoxBaudRate->Text.ToIntDef(-1);
    if (iTemp < 300 || iTemp > 115200)
    {
      ShowMessage("Invalid entry. Baud rate is 300-115200!");
      ComboBoxBaudRate->SetFocus();
      return -1;
    }
    OverallSettings.BaudRate = iTemp;

    return 0;
  }
  catch(...)
  {
    return -100;
  }
}
//---------------------------------------------------------------------------
// Put ProgramHeader info to GUI
int __fastcall TFormOverallSettings::ToGui(void)
{
  try
  {
    int iTemp;

    // Selected program
    ComboBoxProgNames->Text = String(OverallSettings.ProgName);

    // (0) Midi transmit channel for midi exclusive
    // (have to add 1 because MidiTxEx is 0-based)
    ComboBoxMidiExTx->Text = String(OverallSettings.MidiTxEx+1);

    // (1) Reception simulator channel
    // 1-16, not 0-15 like the basic channel!
    ComboBoxRxSimChan->Text = String(OverallSettings.RxSimChan);

    // (60) Reception simulator key
    EditRxSimKey->Text = String(OverallSettings.RxSimKey);

    // (64) Reception simulator velocity
    EditRxSimVel->Text = String(OverallSettings.RxSimVel);

    // Basic (base) midi channel
    // (have to add 1 because MidiChan is 0-based)
    ComboBoxBaseMidiChan->Text = String(OverallSettings.MidiChan+1);

    // Omni midi-mode (respond on any channel)
    CheckBoxOmni->Checked = OverallSettings.bOmniOn;
    CheckBoxLoudnessOnMidi7->Checked = OverallSettings.bRxLoudCtrl7;
    RadioGroupMidiRs232->ItemIndex = OverallSettings.bCtrlByRs232 ? 1 : 0;
    EditPitchWheelRange->Text = String(OverallSettings.PitchWheelRange);

    // Baud rate
    ComboBoxBaudRate->Text = String(OverallSettings.BaudRate);

    return 0;
  }
  catch(...)
  {
    return -100;
  }
}
//---------------------------------------------------------------------------
// Loads OverallSettings struct with default values
int __fastcall TFormOverallSettings::LoadDefaults(void)
{
  try
  {
    strcpy(OverallSettings.ProgName, "DEFAULT PR");
    OverallSettings.MidiTxEx = 0; // (0) Midi transmit channel for midi exclusive
    OverallSettings.RxSimChan = 1; // (1) Reception simulator channel
    OverallSettings.RxSimKey = 60; // (60) " Midi Key
    OverallSettings.RxSimVel = 64; // (64) " Velocity
    OverallSettings.MidiChan = 0; // 0
    OverallSettings.bOmniOn = true; // (128) (this flag is from the MSB of BSAMCH)
    OverallSettings.bRxLoudCtrl7 = false; // (0)
    OverallSettings.bCtrlByRs232 = true; // (0) Midi/Rs232 select
    OverallSettings.PitchWheelRange = 7; // (7) semitones up or down +/- 7 is default (0-127)
    OverallSettings.BaudRate = FormMain->BaudRate; // get main form's rate

    return 0;
  }
  catch(...)
  {
    return -100;
  }
}
//---------------------------------------------------------------------------

