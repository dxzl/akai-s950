//---------------------------------------------------------------------------
#include <vcl.h>
#include "MainForm.h"
#pragma hdrstop

#include "ProgramsForm.h"
#include "ListBoxForm.h"
#include "S9Utils.h"
//#include <System.SysUtils.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormProgram *FormProgram;
using namespace S9Utils;

//---------------------------------------------------------------------------
__fastcall TFormProgram::TFormProgram(TComponent* Owner)
  : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormProgram::FormClose(TObject *Sender, TCloseAction &Action)
{
  Release();
}
//---------------------------------------------------------------------------
void __fastcall TFormProgram::FormDestroy(TObject *Sender)
{
  if (SampleData) delete SampleData;
  if (ProgramData) delete ProgramData;
}
//---------------------------------------------------------------------------
void __fastcall TFormProgram::FormCreate(TObject *Sender)
{
  ButtonDelKg->Enabled = false;
  ButtonAddKg->Enabled = true;

  ButtonSend->Enabled = false;
  MenuSendToMachine->Enabled = false;
  MenuSaveIntoPrgFile->Enabled = true;

  m_progIndex = -1;
  m_selectCount = 0;

  SampleData = NULL;
  ProgramData = NULL;

  // Add columns (we start with 1 for keygroup #)
  SG->ColCount = 45;

  // SG->Cells[col][row]

  SG->Cells[0][0] = "Keygroup";
  SG->Cells[0][1] = "";
  SG->Cells[0][2] = "";
  SG->Cells[1][0] = "Soft Sample";
  SG->Cells[1][1] = "Name";
  SG->Cells[1][2] = "(10 chars max)";
  SG->Cells[2][0] = "SS Filter";
  SG->Cells[2][1] = "";
  SG->Cells[2][2] = "(0-99)";
  SG->Cells[3][0] = "SS Loudness";
  SG->Cells[3][1] = "";
  SG->Cells[3][2] = "(+/- 50)";
  SG->Cells[4][0] = "SS Coarse Pitch";
  SG->Cells[4][1] = "(+/- 127";
  SG->Cells[4][2] = "semitones)";
  SG->Cells[5][0] = "SS Fine Pitch";
  SG->Cells[5][1] = "(+/- 99";
  SG->Cells[5][2] = "1/16 semitone)";
  SG->Cells[6][0] = "Midi Offset (0-15)";
  SG->Cells[6][1] = "(added to base";
  SG->Cells[6][2] = "channel)";
  SG->Cells[C_LOWKEY][0] = "Lower Key";
  SG->Cells[C_LOWKEY][1] = "(-2C to +8G,";
  SG->Cells[C_LOWKEY][2] = "sharp-# flat-b)";
  SG->Cells[C_HIGHKEY][0] = "Upper Key";
  SG->Cells[C_HIGHKEY][1] = "(-2C to +8G,";
  SG->Cells[C_HIGHKEY][2] = "sharp-# flat-b)";
  SG->Cells[9][0] = "Attack";
  SG->Cells[9][1] = "(0-99)";
  SG->Cells[9][2] = "";
  SG->Cells[10][0] = "Decay";
  SG->Cells[10][1] = "(0-99)";
  SG->Cells[10][2] = "";
  SG->Cells[11][0] = "Sustain";
  SG->Cells[11][1] = "(0-99)";
  SG->Cells[11][2] = "";
  SG->Cells[12][0] = "Release";
  SG->Cells[12][1] = "(0-99)";
  SG->Cells[12][2] = "";
  SG->Cells[13][0] = "Voice Out";
  SG->Cells[13][1] = "0-7 Mono 255 ALL";
  SG->Cells[13][2] = "8=L(0-3) 9=R(4-7)";

  SG->Cells[14][0] = "Soft-to-Loud";
  SG->Cells[14][1] = "Vel. Threshold";
  SG->Cells[14][2] = "0-127, 128=OFF";
  SG->Cells[15][0] = "Filter-velocity";
  SG->Cells[15][1] = "Interaction";
  SG->Cells[15][2] = "0-99";
  SG->Cells[16][0] = "Filter-key";
  SG->Cells[16][1] = "Tracking";
  SG->Cells[16][2] = "0-99";
  SG->Cells[17][0] = "Attack-velocity";
  SG->Cells[17][1] = "Interaction";
  SG->Cells[17][2] = "0-99";
  SG->Cells[18][0] = "Velocity Release";
  SG->Cells[18][1] = "Interaction";
  SG->Cells[18][2] = "+/-50";
  SG->Cells[19][0] = "Loudness-velocity";
  SG->Cells[19][1] = "Interaction";
  SG->Cells[19][2] = "0-99, 0=OFF";
  SG->Cells[20][0] = "Pitch-warp Vel.";
  SG->Cells[20][1] = "Interaction";
  SG->Cells[20][2] = "0-99";
  SG->Cells[21][0] = "Pitch-warp";
  SG->Cells[21][1] = "Initial Offset";
  SG->Cells[21][2] = "+/-50";
  SG->Cells[22][0] = "Pitch-warp";
  SG->Cells[22][1] = "Recovery Time";
  SG->Cells[22][2] = "0-99";

  SG->Cells[23][0] = "LFO Build-up";
  SG->Cells[23][1] = "Time";
  SG->Cells[23][2] = "0-99";
  SG->Cells[24][0] = "LFO";
  SG->Cells[24][1] = "Rate";
  SG->Cells[24][2] = "0-99";
  SG->Cells[25][0] = "LFO";
  SG->Cells[25][1] = "Depth";
  SG->Cells[25][2] = "0-99";

  SG->Cells[26][0] = "Aftertouch";
  SG->Cells[26][1] = "Depth Mod.";
  SG->Cells[26][2] = "0-99";
  SG->Cells[27][0] = "Mod. Wheel LFO";
  SG->Cells[27][1] = "Depth Mod.";
  SG->Cells[27][2] = "0-99";
  SG->Cells[28][0] = "ADSR Envelope";
  SG->Cells[28][1] = "Applied to VCF";
  SG->Cells[28][2] = "+/-50";
  SG->Cells[29][0] = "Filter ADSR";
  SG->Cells[29][1] = "Attack Time";
  SG->Cells[29][2] = "0-99";
  SG->Cells[30][0] = "Filter ADSR";
  SG->Cells[30][1] = "Decay Time";
  SG->Cells[30][2] = "0-99";
  SG->Cells[31][0] = "Filter ADSR";
  SG->Cells[31][1] = "Sustain Level";
  SG->Cells[31][2] = "0-99";
  SG->Cells[32][0] = "Filter ADSR";
  SG->Cells[32][1] = "Release Time";
  SG->Cells[32][2] = "0-99";

  SG->Cells[33][0] = "Vel. Crossfade";
  SG->Cells[33][1] = "50%-point";
  SG->Cells[33][2] = "0-127";

  SG->Cells[34][0] = "Loud Sample";
  SG->Cells[34][1] = "Name";
  SG->Cells[34][2] = "(10 chars max)";

  SG->Cells[35][0] = "LS Filter";
  SG->Cells[35][1] = "";
  SG->Cells[35][2] = "(0-99)";
  SG->Cells[36][0] = "LS Loudness";
  SG->Cells[36][1] = "";
  SG->Cells[36][2] = "(+/- 50)";
  SG->Cells[37][0] = "LS Coarse Pitch";
  SG->Cells[37][1] = "(+/- 127";
  SG->Cells[37][2] = "semitones)";
  SG->Cells[38][0] = "LS Fine Pitch";
  SG->Cells[38][1] = "(+/- 99";
  SG->Cells[38][2] = "1/16 semitone)";

  // KBITS
  SG->Cells[39][0] = "Vel. Crossfade";
  SG->Cells[39][1] = "Curve Modification";
  SG->Cells[39][2] = "(0=Off, 1=On)";
  SG->Cells[40][0] = "Vel. Release";
  SG->Cells[40][1] = "Mode (0=Note";
  SG->Cells[40][2] = "Off, 1=Note On)";
  SG->Cells[41][0] = "One-shot Trigger";
  SG->Cells[41][1] = "(0=Off";
  SG->Cells[41][2] = "1=On)";
  SG->Cells[42][0] = "Vibrato Desync.";
  SG->Cells[42][1] = "(0=Off";
  SG->Cells[42][2] = "1=On)";
  SG->Cells[43][0] = "Velocity";
  SG->Cells[43][1] = "Crossfade";
  SG->Cells[43][2] = "(0=Off, 1=On)";
  SG->Cells[44][0] = "Transpose"; // Inverse!
  SG->Cells[44][1] = "(0=Off";
  SG->Cells[44][2] = "1=On)";

  ButtonSelectAll->Caption = "Select All";

  ButtonRefreshProgramsListClick(NULL);
  RefreshKeygroupsFromSampleList();
//  SG->Row = SG->RowCount-1;
}
//---------------------------------------------------------------------------
void __fastcall TFormProgram::FormShow(TObject *Sender)
{
  // Drop the program-name list down
  if (ComboBoxProgNames->Items->Count && !ComboBoxProgNames->DroppedDown)
    ComboBoxProgNames->DroppedDown = true;
}
//---------------------------------------------------------------------------
// writes the working KeyGroup struct to one row in the grid
// index is 0-based
int __fastcall TFormProgram::KeygroupToGui(int index, bool bSkipNames)
{
  try
  {
    if (index < 0)
      return -2;

    int iRow = index + PADROWS;

    if (iRow >= SG->RowCount)
      return -3;

    if (!bSkipNames)
      SG->Cells[0+PADCOLS][iRow] = String(KeyGroup.SoftSampleName).TrimRight(); // soft sample name

    SG->Cells[1+PADCOLS][iRow] = String(KeyGroup.SoftSampFilter); // soft-sample filter (0-99)
    SG->Cells[2+PADCOLS][iRow] = String(KeyGroup.SoftSampLoudness); // soft-sample velocity loudness (0-99)
    SG->Cells[3+PADCOLS][iRow] = GetCoarsePitch(KeyGroup.SoftSampTransposeOffset); // course pitch offset (+/- 127)
    SG->Cells[4+PADCOLS][iRow] = GetFinePitch(KeyGroup.SoftSampTransposeOffset); // fine pitch offset (+/- 99)
    SG->Cells[5+PADCOLS][iRow] = String(KeyGroup.MidiOffset); // midi offset (0-15)
    SG->Cells[C_LOWKEY][iRow] = KeyToString(KeyGroup.LowerMidiKey); // low key (0-127)
    SG->Cells[C_HIGHKEY][iRow] = KeyToString(KeyGroup.UpperMidiKey); // high key (0-127)
    SG->Cells[8+PADCOLS][iRow] = String(KeyGroup.AttackTime); // attack (0-99)
    SG->Cells[9+PADCOLS][iRow] = String(KeyGroup.DecayTime); // decay (0-99)
    SG->Cells[10+PADCOLS][iRow] = String(KeyGroup.SustLevel); // sustain (0-99)
    SG->Cells[11+PADCOLS][iRow] = String(KeyGroup.RelTime); // release (0-99)
    SG->Cells[12+PADCOLS][iRow] = String(KeyGroup.VoiceOutAssign); // voice out 0-7, 8, 9, 255

    SG->Cells[13+PADCOLS][iRow] = String(KeyGroup.VelSwThresh); // (128) Velocity switch threshold. Velocity >= this, then switch to loud sample
    SG->Cells[14+PADCOLS][iRow] = String(KeyGroup.FilterVelInter); // (10) Filter-velocity interaction
    SG->Cells[15+PADCOLS][iRow] = String(KeyGroup.FilterKeyTracking); // (50) Filter-key tracking. 50 = 1 octave/octave
    SG->Cells[16+PADCOLS][iRow] = String(KeyGroup.AttackVelInter); // (0) Attack-velocity interaction
    SG->Cells[17+PADCOLS][iRow] = String(KeyGroup.VelRelInter); // (0) Velocity-release interaction (+/- 50). If positive, greater note-off velocity gives faster release.
    SG->Cells[18+PADCOLS][iRow] = String(KeyGroup.LoudnessVelInter); // (30) Loudness-velocity interaction 0=No dynamics
    SG->Cells[19+PADCOLS][iRow] = String(KeyGroup.PitchWarpVelInter); // (0) Pitch-warp velocity interaction
    SG->Cells[20+PADCOLS][iRow] = String(KeyGroup.PitchWarpOffset); // (0) Pitch-warp initial offset (+/-50)
    SG->Cells[21+PADCOLS][iRow] = String(KeyGroup.PitchWarpRecoveryTime); // (99) Pitch-warp recovery time. 99 is the slowest.

    SG->Cells[22+PADCOLS][iRow] = String(KeyGroup.LfoBuildTime); // (64) LFO Build-up time
    SG->Cells[23+PADCOLS][iRow] = String(KeyGroup.LfoRate); // (42) LFO Rate
    SG->Cells[24+PADCOLS][iRow] = String(KeyGroup.LfoDepth); // (0) LFO Depth

    SG->Cells[25+PADCOLS][iRow] = String(KeyGroup.AftertouchDepthMod); // (0) Aftertough depth modulation
    SG->Cells[26+PADCOLS][iRow] = String(KeyGroup.ModWheelLfoDepthMod); // (50) Mod. wheel LFO depth modulation
    SG->Cells[27+PADCOLS][iRow] = String(KeyGroup.AdsrEnvToVcfFilter); // (0) ADSR envelope applied to VCF
    SG->Cells[28+PADCOLS][iRow] = String(KeyGroup.FilterAdsrAttackTime); // (20) Filter ADSR attack time
    SG->Cells[29+PADCOLS][iRow] = String(KeyGroup.FilterAdsrDecayTime); // (20) Filter ADSR decay time
    SG->Cells[30+PADCOLS][iRow] = String(KeyGroup.FilterAdsrSustainLevel); // (20) Filter ADSR sustain level
    SG->Cells[31+PADCOLS][iRow] = String(KeyGroup.FilterAdsrReleaseTime); // (20) Filter ADSR release time
    // (64) Velocity value at which loud-soft mixture is 50% in velocity
    // crossfade type sample. Ignores if ENVTMX bit in KBITS is 0.
    SG->Cells[32+PADCOLS][iRow] = String(KeyGroup.VelXfadeFiftyPercent);

    if (!bSkipNames)
      SG->Cells[33+PADCOLS][iRow] = String(KeyGroup.LoudSampleName).TrimRight(); // Loud sample name

    SG->Cells[34+PADCOLS][iRow] = String(KeyGroup.LoudSampFilter); // (99) Loud-sample filter. 99 gives highest cut-off
    SG->Cells[35+PADCOLS][iRow] = String(KeyGroup.LoudSampLoudness); // (0) Loud-sample loudness. +/-50 units of .375 dB
    SG->Cells[36+PADCOLS][iRow] = GetCoarsePitch(KeyGroup.LoudSampTransposeOffset); // (0) Loud-sample coarse pitch (semitones)
    SG->Cells[37+PADCOLS][iRow] = GetFinePitch(KeyGroup.LoudSampTransposeOffset); // (0) Loud-sample fine pitch (1/16 semitone)

    // KBITS
    SG->Cells[38+PADCOLS][iRow] = String((KeyGroup.ControlBits & 32) ? 1 : 0); // Velocity crossfade curve modification 0=off, 1=on
    SG->Cells[39+PADCOLS][iRow] = String((KeyGroup.ControlBits & 16) ? 1 : 0); // Velocity release mode 0=note-off, 1=note-on
    SG->Cells[40+PADCOLS][iRow] = String((KeyGroup.ControlBits & 8) ? 1 : 0); // One-shot trigger mode 0=off, 1=on
    SG->Cells[41+PADCOLS][iRow] = String((KeyGroup.ControlBits & 4) ? 1 : 0); // Vibrato Desync 0=off, 1=on
    SG->Cells[42+PADCOLS][iRow] = String((KeyGroup.ControlBits & 2) ? 1 : 0); // Velocity crossfade 0=off, 1=on
    SG->Cells[43+PADCOLS][iRow] = String((KeyGroup.ControlBits & 1) ? 0 : 1); // Transpose 0=on, 1=off

    if (MenuShowUndefined->Checked)
    {
      // Diagnostic: Undefined and reserved fields
      String s = "\r\nUndefined/Reserved Fields, Keygroup " + String(index+1) + "\r\n"
      "\r\nUndefined1 (DB 78): " + String(KeyGroup.KyUndef1) +
      "\r\nUndefined2 (DW 80): " + String(KeyGroup.KyUndef2) +
      "\r\nUndefined3 (DD 112): " + String(KeyGroup.KyUndef3) +
      "\r\nUndefined4 (DD 120): " + String(KeyGroup.KyUndef4) +
      "\r\r\nUndefined5 (DW 136): " + String(KeyGroup.KyUndef5);
      printm(s);
    }


    return 0;
  }
  catch(...)
  {
    return -100;
  }
}
//---------------------------------------------------------------------------
// writes one keygroup row of the grid to the working KeyGroup struct
// index is 0-based
// returns 0 if ok, negative if fail
int __fastcall TFormProgram::KeygroupFromGui(int index)
{
  try
  {
    if (index < 0)
      return -2;

    int iRow = index + PADROWS;

    if (iRow >= SG->RowCount)
      return -3;

    // If the grid does not let the user set all possible parameters,
    // then we need some defaults!
    int iError = KeygroupLoadDefaults();
    if (iError < 0)
    {
      if (iError != -1)
        ShowMessage("Error in KeygroupLoadDefaults(), (code=" +
                                              String(iError) + ")");
      return -1;
    }

    // Soft sample name... Trim blanks and write back...
    AnsiString sTemp = SG->Cells[0+PADCOLS][iRow].TrimRight();
    SG->Cells[0+PADCOLS][iRow] = sTemp;
    if (!NameOk(sTemp))
    {
      ShowMessage("Name must be 1-10 ASCII characters!");
      return -1;
    }
    strncpy(&KeyGroup.SoftSampleName[0], sTemp.c_str(), MAX_NAME_S900);
    KeyGroup.SoftSampleName[MAX_NAME_S900] = '\0';

    int iTemp = SG->Cells[1+PADCOLS][iRow].ToIntDef(-1000);
    if (iTemp >= 0 && iTemp <= 99)
      KeyGroup.SoftSampFilter = iTemp; // soft-sample filter (99 = highest cut-off)
    else
    {
      SG->Cells[1+PADCOLS][iRow] = "99";
      KeyGroup.SoftSampFilter = 99;
    }

    iTemp = SG->Cells[2+PADCOLS][iRow].ToIntDef(-1000);
    if (iTemp >= -50 && iTemp <= 50)
      KeyGroup.SoftSampLoudness = iTemp; // loudness (+/- 50)
    else
    {
      SG->Cells[2+PADCOLS][iRow] = "+0";
      KeyGroup.SoftSampLoudness = 0;
    }

    // coarse pitch, semitones
    iTemp = SG->Cells[3+PADCOLS][iRow].ToIntDef(-1000);
    if (iTemp >= -127 && iTemp <= 127)
      KeyGroup.SoftSampTransposeOffset = iTemp*16; // +/- course transpose in semitone steps
    else
    {
      SG->Cells[3+PADCOLS][iRow] = "+0";
      KeyGroup.SoftSampTransposeOffset = 0;
    }

    // add the fine-tune pitch transpose
    iTemp = SG->Cells[4+PADCOLS][iRow].ToIntDef(-1000);
    if (iTemp >= -99 && iTemp <= 99)
      KeyGroup.SoftSampTransposeOffset += iTemp; // +/- fine transpose in 1/16 octave steps
    else
      SG->Cells[4+PADCOLS][iRow] = "+0";

    iTemp = SG->Cells[5+PADCOLS][iRow].ToIntDef(-1000); // midi offset (0-15)
    if (iTemp >= 0 && iTemp <= 15)
      KeyGroup.MidiOffset = iTemp;
    else
    {
      SG->Cells[5+PADCOLS][iRow] = "0";
      KeyGroup.MidiOffset = 0;
    }

    iTemp = StringToKey(SG->Cells[C_LOWKEY][iRow]);
    if (iTemp >= 0 && iTemp <= 127)
      KeyGroup.LowerMidiKey = iTemp; // low key (0-127)
    else
    {
      SG->Cells[C_LOWKEY][iRow] = "+0C";
      KeyGroup.LowerMidiKey = 24;
    }

    iTemp = StringToKey(SG->Cells[C_HIGHKEY][iRow]);
    if (iTemp >= 0 && iTemp <= 127)
      KeyGroup.UpperMidiKey = iTemp; // high key (0-127)
    else
    {
      SG->Cells[C_HIGHKEY][iRow] = "+8G";
      KeyGroup.UpperMidiKey = 127;
    }

    // attack
    iTemp = SG->Cells[8+PADCOLS][iRow].ToIntDef(-1000);
    if (iTemp >= 0 && iTemp <= 99)
      KeyGroup.AttackTime = iTemp;
    else
    {
      SG->Cells[8+PADCOLS][iRow] = "0";
      KeyGroup.AttackTime = 0;
    }

    // decay
    iTemp = SG->Cells[9+PADCOLS][iRow].ToIntDef(-1000);
    if (iTemp >= 0 && iTemp <= 99)
      KeyGroup.DecayTime = iTemp;
    else
    {
      SG->Cells[9+PADCOLS][iRow] = "80";
      KeyGroup.DecayTime = 80;
    }

    // sustain
    iTemp = SG->Cells[10+PADCOLS][iRow].ToIntDef(-1000);
    if (iTemp >= 0 && iTemp <= 99)
      KeyGroup.SustLevel = iTemp;
    else
    {
      SG->Cells[10+PADCOLS][iRow] = "99";
      KeyGroup.SustLevel = 99;
    }

    // release
    iTemp = SG->Cells[11+PADCOLS][iRow].ToIntDef(-1000);
    if (iTemp >= 0 && iTemp <= 99)
      KeyGroup.RelTime = iTemp;
    else
    {
      SG->Cells[11+PADCOLS][iRow] = "30";
      KeyGroup.RelTime = 30;
    }

    // voice-out assign
    iTemp = SG->Cells[12+PADCOLS][iRow].ToIntDef(-1000);
    if (iTemp == 255 || (iTemp >= 0 && iTemp <= 9))
      KeyGroup.VoiceOutAssign = iTemp;
    else
    {
      SG->Cells[12+PADCOLS][iRow] = "255";
      KeyGroup.VoiceOutAssign = 255;
    }

    // velocity threshold at which we transition from soft to loud sample.
    // 128 is off
    iTemp = SG->Cells[13+PADCOLS][iRow].ToIntDef(-1000);
    if (iTemp >= 0 && iTemp <= 128)
      KeyGroup.VelSwThresh = iTemp;
    else
    {
      SG->Cells[13+PADCOLS][iRow] = "128";
      KeyGroup.VelSwThresh = 128;
    }

    // Filter-velocity interaction
    // 128 is off
    iTemp = SG->Cells[14+PADCOLS][iRow].ToIntDef(-1000);
    if (iTemp >= 0 && iTemp <= 99)
      KeyGroup.FilterVelInter = iTemp;
    else
    {
      SG->Cells[14+PADCOLS][iRow] = "10";
      KeyGroup.FilterVelInter = 10;
    }

    // Filter-key tracking. 50 gives 1 Octave/Octave
    iTemp = SG->Cells[15+PADCOLS][iRow].ToIntDef(-1000);
    if (iTemp >= 0 && iTemp <= 99)
      KeyGroup.FilterKeyTracking = iTemp;
    else
    {
      SG->Cells[15+PADCOLS][iRow] = "50";
      KeyGroup.FilterKeyTracking = 50;
    }

    // Attack-velocity interaction
    iTemp = SG->Cells[16+PADCOLS][iRow].ToIntDef(-1000);
    if (iTemp >= 0 && iTemp <= 99)
      KeyGroup.AttackVelInter = iTemp;
    else
    {
      SG->Cells[16+PADCOLS][iRow] = "0";
      KeyGroup.AttackVelInter = 0;
    }

    // Velocity-release interaction +/-50, if positive, greater note-off
    // velocity gives faster release
    iTemp = SG->Cells[17+PADCOLS][iRow].ToIntDef(-1000);
    if (iTemp >= -50 && iTemp <= 50)
      KeyGroup.VelRelInter = iTemp;
    else
    {
      SG->Cells[17+PADCOLS][iRow] = "0";
      KeyGroup.VelRelInter = 0;
    }

    // Loudness-velocity interaction, 0 = no dynamics
    iTemp = SG->Cells[18+PADCOLS][iRow].ToIntDef(-1000);
    if (iTemp >= 0 && iTemp <= 99)
      KeyGroup.LoudnessVelInter = iTemp;
    else
    {
      SG->Cells[18+PADCOLS][iRow] = "30";
      KeyGroup.LoudnessVelInter = 30;
    }

    // Pitch-warp velocity interaction
    iTemp = SG->Cells[19+PADCOLS][iRow].ToIntDef(-1000);
    if (iTemp >= 0 && iTemp <= 99)
      KeyGroup.PitchWarpVelInter = iTemp;
    else
    {
      SG->Cells[19+PADCOLS][iRow] = "0";
      KeyGroup.PitchWarpVelInter = 0;
    }

    // Pitch-warp velocity interaction
    iTemp = SG->Cells[20+PADCOLS][iRow].ToIntDef(-1000);
    if (iTemp >= -50 && iTemp <= 50)
      KeyGroup.PitchWarpOffset = iTemp;
    else
    {
      SG->Cells[20+PADCOLS][iRow] = "0";
      KeyGroup.PitchWarpOffset = 0;
    }

    // Pitch-warp recovery time, 99 is the slowest
    iTemp = SG->Cells[21+PADCOLS][iRow].ToIntDef(-1000);
    if (iTemp >= 0 && iTemp <= 99)
      KeyGroup.PitchWarpRecoveryTime = iTemp;
    else
    {
      SG->Cells[21+PADCOLS][iRow] = "99";
      KeyGroup.PitchWarpRecoveryTime = 99;
    }

    // LFO build-up time
    iTemp = SG->Cells[22+PADCOLS][iRow].ToIntDef(-1000);
    if (iTemp >= 0 && iTemp <= 99)
      KeyGroup.LfoBuildTime = iTemp;
    else
    {
      SG->Cells[22+PADCOLS][iRow] = "64";
      KeyGroup.LfoBuildTime = 64;
    }

    // LFO rate
    iTemp = SG->Cells[23+PADCOLS][iRow].ToIntDef(-1000);
    if (iTemp >= 0 && iTemp <= 99)
      KeyGroup.LfoRate = iTemp;
    else
    {
      SG->Cells[23+PADCOLS][iRow] = "42";
      KeyGroup.LfoRate = 42;
    }

    // LFO depth
    iTemp = SG->Cells[24+PADCOLS][iRow].ToIntDef(-1000);
    if (iTemp >= 0 && iTemp <= 99)
      KeyGroup.LfoDepth = iTemp;
    else
    {
      SG->Cells[24+PADCOLS][iRow] = "0";
      KeyGroup.LfoDepth = 0;
    }

    // Aftertouch depth modulation
    iTemp = SG->Cells[25+PADCOLS][iRow].ToIntDef(-1000);
    if (iTemp >= 0 && iTemp <= 99)
      KeyGroup.AftertouchDepthMod = iTemp;
    else
    {
      SG->Cells[25+PADCOLS][iRow] = "0";
      KeyGroup.AftertouchDepthMod = 0;
    }

    // Modulation wheel LFO depth modulation
    // Note: LFO depth modulation is the sum of VBDPTH (LFO depth) +
    // AFDI (aftertouch) + MWDI (Modwheel). A maximum of +/- 3 semitones
    iTemp = SG->Cells[26+PADCOLS][iRow].ToIntDef(-1000);
    if (iTemp >= 0 && iTemp <= 99)
      KeyGroup.ModWheelLfoDepthMod = iTemp;
    else
    {
      SG->Cells[26+PADCOLS][iRow] = "50";
      KeyGroup.ModWheelLfoDepthMod = 50;
    }

    // Amount of ADSR envelope applied to VCF filter frequency. Signed +/-50
    iTemp = SG->Cells[27+PADCOLS][iRow].ToIntDef(-1000);
    if (iTemp >= -50 && iTemp <= 50)
      KeyGroup.AdsrEnvToVcfFilter = iTemp;
    else
    {
      SG->Cells[27+PADCOLS][iRow] = "0";
      KeyGroup.AdsrEnvToVcfFilter = 0;
    }

    // Filter ADSR attack time
    iTemp = SG->Cells[28+PADCOLS][iRow].ToIntDef(-1000);
    if (iTemp >= 0 && iTemp <= 99)
      KeyGroup.FilterAdsrAttackTime = iTemp;
    else
    {
      SG->Cells[28+PADCOLS][iRow] = "20";
      KeyGroup.FilterAdsrAttackTime = 20;
    }

    // Filter ADSR decay time
    iTemp = SG->Cells[29+PADCOLS][iRow].ToIntDef(-1000);
    if (iTemp >= 0 && iTemp <= 99)
      KeyGroup.FilterAdsrDecayTime = iTemp;
    else
    {
      SG->Cells[29+PADCOLS][iRow] = "20";
      KeyGroup.FilterAdsrDecayTime = 20;
    }

    // Filter ADSR sustain level
    iTemp = SG->Cells[30+PADCOLS][iRow].ToIntDef(-1000);
    if (iTemp >= 0 && iTemp <= 99)
      KeyGroup.FilterAdsrSustainLevel = iTemp;
    else
    {
      SG->Cells[30+PADCOLS][iRow] = "20";
      KeyGroup.FilterAdsrSustainLevel = 20;
    }

    // Filter ADSR release time
    iTemp = SG->Cells[31+PADCOLS][iRow].ToIntDef(-1000);
    if (iTemp >= 0 && iTemp <= 99)
      KeyGroup.FilterAdsrReleaseTime = iTemp;
    else
    {
      SG->Cells[31+PADCOLS][iRow] = "20";
      KeyGroup.FilterAdsrReleaseTime = 20;
    }

    // Velocity value at which loud-soft mixture is 50% in velocity
    // crossfade type sample. Ignores if ENVTMX bit in KBITS is 0.
    iTemp = SG->Cells[32+PADCOLS][iRow].ToIntDef(-1000);
    if (iTemp >= 0 && iTemp <= 127)
      KeyGroup.VelXfadeFiftyPercent = iTemp;
    else
    {
      SG->Cells[32+PADCOLS][iRow] = "64";
      KeyGroup.VelXfadeFiftyPercent = 64;
    }

    // Loud sample name... Trim blanks and write back...
    sTemp = SG->Cells[33+PADCOLS][iRow].TrimRight();
    SG->Cells[33+PADCOLS][iRow] = sTemp;
    if (!NameOk(sTemp))
    {
      ShowMessage("Name must be 1-10 ASCII characters!");
      return -1;
    }
    strncpy(&KeyGroup.LoudSampleName[0], sTemp.c_str(), MAX_NAME_S900);
    KeyGroup.LoudSampleName[MAX_NAME_S900] = '\0';

    // Loud-sample filter. 99 gives highest cut-off
    iTemp = SG->Cells[34+PADCOLS][iRow].ToIntDef(-1000);
    if (iTemp >= 0 && iTemp <= 127)
      KeyGroup.LoudSampFilter = iTemp;
    else
    {
      SG->Cells[34+PADCOLS][iRow] = "99";
      KeyGroup.LoudSampFilter = 99;
    }

    // Loud-sample loudness. +/-50 units of .375 dB
    iTemp = SG->Cells[35+PADCOLS][iRow].ToIntDef(-1000);
    if (iTemp >= -50 && iTemp <= 50)
      KeyGroup.LoudSampLoudness = iTemp;
    else
    {
      SG->Cells[35+PADCOLS][iRow] = "0";
      KeyGroup.LoudSampLoudness = 0;
    }

    // Loud-sample coarse pitch (semitones)
    iTemp = SG->Cells[36+PADCOLS][iRow].ToIntDef(-1000);
    if (iTemp >= -127 && iTemp <= 127)
      KeyGroup.LoudSampTransposeOffset = iTemp*16; // +/- course transpose in semitone steps
    else
    {
      SG->Cells[36+PADCOLS][iRow] = "+0";
      KeyGroup.LoudSampTransposeOffset = 0;
    }

    // add the fine-tune pitch transpose
    iTemp = SG->Cells[37+PADCOLS][iRow].ToIntDef(-1000);
    if (iTemp >= -99 && iTemp <= 99)
      KeyGroup.LoudSampTransposeOffset += iTemp; // +/- fine transpose in 1/16 octave steps
    else
      SG->Cells[37+PADCOLS][iRow] = "+0";


    KeyGroup.ControlBits = 4; // set to default state (desync on, transpose enabled)

    // KBITS(bit 5) Velocity crossfade curve modification 0=off, 1=on
    iTemp = SG->Cells[38+PADCOLS][iRow].ToIntDef(-1000);
    if (iTemp >= 0 && iTemp <= 1)
      KeyGroup.ControlBits |= (iTemp << 5);
    else
      SG->Cells[38+PADCOLS][iRow] = "0";

    // KBITS(bit 4) Velocity release mode 0=note-off, 1=note-on
    iTemp = SG->Cells[39+PADCOLS][iRow].ToIntDef(-1000);
    if (iTemp >= 0 && iTemp <= 1)
      KeyGroup.ControlBits |= (iTemp << 4);
    else
      SG->Cells[39+PADCOLS][iRow] = "0";

    // KBITS(bit 3) One-shot trigger mode 0=off, 1=on
    iTemp = SG->Cells[40+PADCOLS][iRow].ToIntDef(-1000);
    if (iTemp >= 0 && iTemp <= 1)
      KeyGroup.ControlBits |= (iTemp << 3);
    else
      SG->Cells[40+PADCOLS][iRow] = "0";

    // KBITS(bit 2) Vibrato Desync 0=off, 1=on
    iTemp = SG->Cells[41+PADCOLS][iRow].ToIntDef(-1000);
    if (iTemp >= 0 && iTemp <= 1)
      KeyGroup.ControlBits |= (iTemp << 2);
    else
      SG->Cells[41+PADCOLS][iRow] = "1"; // default set above is On

    // KBITS(bit 1) Velocity crossfade 0=off, 1=on
    iTemp = SG->Cells[42+PADCOLS][iRow].ToIntDef(-1000);
    if (iTemp >= 0 && iTemp <= 1)
      KeyGroup.ControlBits |= (iTemp << 1);
    else
      SG->Cells[42+PADCOLS][iRow] = "0";

    // KBITS(bit 0) Transpose 0=on, 1=off
    iTemp = SG->Cells[43+PADCOLS][iRow].ToIntDef(-1000);
    if (iTemp >= 0 && iTemp <= 1)
    {
      iTemp = (iTemp == 0) ? 1 : 0; // invert
      KeyGroup.ControlBits |= (iTemp << 0);
    }
    else
      SG->Cells[43+PADCOLS][iRow] = "1"; // default set above is On

    return 0;
  }
  catch(...)
  {
    return -100;
  }
}
//---------------------------------------------------------------------------
// Loads ProgramHeader with default values
int __fastcall TFormProgram::HeaderLoadDefaults(int numKeyGroups)
{
  try
  {
    strcpy(&ProgramHeader.ProgName[0], "NewProg 00");
    ProgramHeader.PrUndef1 = 0;
    ProgramHeader.PrUndef2 = 0;
    ProgramHeader.KeyTilt = 0;
    ProgramHeader.PrUndef3 = 50302; // S950 TONE program has 50302 in this! ????
    ProgramHeader.PrUndef4 = 0;
    ProgramHeader.PosXfade = 0;
    ProgramHeader.PrReser1 = 255;
    ProgramHeader.NumKeygroups = numKeyGroups;
    ProgramHeader.PrUndef5 = 0;
    ProgramHeader.MidiPgmNumber = 0;
    ProgramHeader.EnableMidiPgmNumber = 0;
    ProgramHeader.PrReser2 = 0;
    ProgramHeader.PrReser3 = 0;
    ProgramHeader.PrReser4 = 0;

    return 0;
  }
  catch(...)
  {
    return -100;
  }
}
//---------------------------------------------------------------------------
// Put ProgramHeader info to GUI
int __fastcall TFormProgram::HeaderToGui(void)
{
  try
  {
    ComboBoxProgNames->Text = String(ProgramHeader.ProgName).TrimRight();
    EditKeyTilt->Text = String(ProgramHeader.KeyTilt);
    EditMidiPrgNum->Text = String(ProgramHeader.MidiPgmNumber);
    CheckBoxPosXfade->Checked = ProgramHeader.PosXfade == 0 ? false : true;
    CheckBoxEnableProgNumber->Checked = ProgramHeader.EnableMidiPgmNumber == 0 ? false : true;

    if (MenuShowUndefined->Checked)
    {
      // Undefined and reserved fields
      String s = "\r\nUndefined/Reserved Fields, Header\r\n"
      "\r\nUndefined1 (DD 27): " + String(ProgramHeader.PrUndef1) +
      "\r\nUndefined2 (DW 35): " + String(ProgramHeader.PrUndef2) +
      "\r\nUndefined3 (DW 43): " + String(ProgramHeader.PrUndef3) + // has 50302 in it for the TONE program! ???
      "\r\nUndefined4 (DB 47): " + String(ProgramHeader.PrUndef4) +
      "\r\nUndefined5 (DW 55): " + String(ProgramHeader.PrUndef5) +
      "\r\nReserved1 (DB 51): " + String(ProgramHeader.PrReser1) +
      "\r\nReserved2 (DW 63): " + String(ProgramHeader.PrReser2) +
      "\r\nReserved3 (DD 67): " + String(ProgramHeader.PrReser3) +
      "\r\nReserved4 (DD 75): " + String(ProgramHeader.PrReser4);
      printm(s);
     }

    return 0;
  }
  catch(...)
  {
    return -100;
  }
}
//---------------------------------------------------------------------------
// Put ProgramHeader info to GUI
int __fastcall TFormProgram::HeaderFromGui(void)
{
  try
  {
    int NumKeyGroups = SG->RowCount - PADROWS;

    // If the grid does not let the user set all possible parameters,
    // then we need some defaults!
    int iError = HeaderLoadDefaults(NumKeyGroups);
    if (iError < 0)
    {
      if (iError != -1)
        ShowMessage("Error in HeaderLoadDefaults(), (code=" +
                                              String(iError) + ")");
      return -1;
    }

    AnsiString sTemp = ComboBoxProgNames->Text.TrimRight();

    if (!NameOk(sTemp))
    {
      ShowMessage("Name must be 1-10 ASCII characters!");
      ComboBoxProgNames->SetFocus();
      return -1;
    }

    memset(&ProgramHeader.ProgName[0], 0, MAX_NAME_S900+1);
    strcpy(&ProgramHeader.ProgName[0], sTemp.c_str());

    int iTemp = EditKeyTilt->Text.ToIntDef(-1000);
    if (iTemp < -50 || iTemp > 50)
    {
      EditKeyTilt->Text = "0";
      iTemp = 0;
    }
    ProgramHeader.KeyTilt = iTemp; // +/- 50

    iTemp = EditMidiPrgNum->Text.ToIntDef(-1000);
    if (iTemp < 0 || iTemp > 127)
    {
      EditMidiPrgNum->Text = "0";
      iTemp = 0;
    }
    ProgramHeader.MidiPgmNumber = iTemp;

    ProgramHeader.PosXfade = CheckBoxPosXfade->Checked ? 1 : 0;
    ProgramHeader.EnableMidiPgmNumber = CheckBoxEnableProgNumber->Checked ? 255 : 0;

    // Not defined for S900 but used in some way for S950... ?????
    //ProgramHeader.Unknown1
    //ProgramHeader.Unknown2
    //ProgramHeader.Unknown3
    return 0;
  }
  catch(...)
  {
    return -100;
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormProgram::ButtonKgDefaultsClick(TObject *Sender)
{
  try
  {
    int NumKeyGroups = SG->RowCount - PADROWS;

    if (NumKeyGroups == 0) return;

    int iError = KeygroupLoadDefaults(); // load defaults into working KeyGroup

    if (iError < 0)
    {
      if (iError != -1)
        ShowMessage("Unable to load keygroup defaults KeygroupLoadDefaults()! (code=" + String(iError) + ")");
      return;
    }

    if (CountSelected() == 0) // nothing selected, restore defaults on current item
    {
      int iRow = SG->Row;

      if (iRow >= PADROWS && iRow < SG->RowCount)
      {
        iError = KeygroupToGui(iRow-PADROWS, true);

        if (iError < 0)
        {
          if (iError != -1)
            ShowMessage("Unable to load keygroup defaults KeyGroupToGui()! (code=" + String(iError) + ")");
          return;
        }
      }
    }
    else // restore defaults to selected items
    {
      for (int ii = 0; ii < NumKeyGroups; ii++)
      {
        int iRow = ii+PADROWS;

        // restore defaults if selected... (we use Objects[0] as select flag)
        if ((int)SG->Rows[iRow]->Objects[C_KEYGROUP] != 0)
        {
          iError = KeygroupToGui(iRow-PADROWS, true);

          if (iError < 0)
          {
            if (iError != -1)
              ShowMessage("Unable to load keygroup defaults KeyGroupToGui()! (code=" + String(iError) + ")");
            return;
          }
        }
      }
    }
  }
  catch(...)
  {
    ShowMessage("Exception thrown in ButtonKgDefaultsClick()!");
  }
}
//---------------------------------------------------------------------------
// Loads KeyGroup with default values
int __fastcall TFormProgram::KeygroupLoadDefaults(void)
{
  try
  {
    KeyGroup.UpperMidiKey = 127;
    KeyGroup.LowerMidiKey = 24;
    KeyGroup.VelSwThresh = 128;
    KeyGroup.AttackTime = 0;
    KeyGroup.DecayTime = 80;
    KeyGroup.SustLevel = 99;
    KeyGroup.RelTime = 30;
    KeyGroup.FilterVelInter = 10;
    KeyGroup.FilterKeyTracking = 50;
    KeyGroup.AttackVelInter = 0;
    KeyGroup.VelRelInter = 0;
    KeyGroup.LoudnessVelInter = 30;
    KeyGroup.PitchWarpVelInter = 0;
    KeyGroup.PitchWarpOffset = 0;
    KeyGroup.PitchWarpRecoveryTime = 99;
    KeyGroup.LfoBuildTime = 64;
    KeyGroup.LfoRate = 42;
    KeyGroup.LfoDepth = 0;
    KeyGroup.ControlBits = 4;
    KeyGroup.VoiceOutAssign = 255;
    KeyGroup.MidiOffset = 0;
    KeyGroup.AftertouchDepthMod = 0;
    KeyGroup.ModWheelLfoDepthMod = 50;
    KeyGroup.AdsrEnvToVcfFilter = 0;

    // ASCII sample name
    strcpy(&KeyGroup.SoftSampleName[0], "SoftSamp00"); // max 10 chars!

    KeyGroup.FilterAdsrAttackTime = 20;
    KeyGroup.FilterAdsrDecayTime = 20;
    KeyGroup.FilterAdsrSustainLevel = 20;
    KeyGroup.FilterAdsrReleaseTime = 20;
    KeyGroup.VelXfadeFiftyPercent = 64;

    // undefined for S900
    KeyGroup.KyUndef1 = 0;
    KeyGroup.KyUndef2 = 50372; // offset 80, DW TONE program has this.... (????)

    KeyGroup.SoftSampTransposeOffset = 0;
    KeyGroup.SoftSampFilter = 99;
    KeyGroup.SoftSampLoudness = 0;

    // ASCII sample name
    strcpy(&KeyGroup.LoudSampleName[0], "LoudSamp00"); // max 10 chars!

    // undefined for S900
    KeyGroup.KyUndef3 = 0;
    KeyGroup.KyUndef4 = 0;

    KeyGroup.LoudSampTransposeOffset = 0;
    KeyGroup.LoudSampFilter = 99;
    KeyGroup.LoudSampLoudness = 0;

    // undefined for S900
    KeyGroup.KyUndef5 = 0;
    return 0;
  }
  catch(...)
  {
    return -100;
  }
}
//---------------------------------------------------------------------------
// encodes Program Header into the pg[76+7] (akai-formatted) array
int __fastcall TFormProgram::HeaderToArray(int progIdx)
{
  try
  {
    // Clear pg array to all 0
    memset(pg, 0, PROGHEADERSIZ);

    pg[0] = BEX;
    pg[1] = AKAI_ID;
    pg[2] = 0;
    pg[3] = PRGM;
    pg[4] = S900_ID;
    pg[5] = progIdx;
    pg[6] = 0;

    // copy ASCII sample name, pad with blanks and terminate
    AsciiStrEncode(&pg[PRNAME], ProgramHeader.ProgName);

    encodeDD(ProgramHeader.PrUndef1, &pg[PRUNDEF1]);
    encodeDW(ProgramHeader.PrUndef2, &pg[PRUNDEF2]);

    encodeDW(ProgramHeader.KeyTilt, &pg[KTILT]);

    encodeDW(ProgramHeader.PrUndef3, &pg[PRUNDEF3]);
    encodeDB(ProgramHeader.PrUndef4, &pg[PRUNDEF4]);

    encodeDB(ProgramHeader.PosXfade, &pg[POSXEN]);

    // reserved location with 255 (for S900)
    encodeDB(ProgramHeader.PrReser1, &pg[PRRESER1]);

    encodeDB(ProgramHeader.NumKeygroups, &pg[NOKG]);

    encodeDW(ProgramHeader.PrUndef5, &pg[PRUNDEF5]);

    encodeDB(ProgramHeader.MidiPgmNumber, &pg[MDPGNM]); // S950 only
    encodeDB(ProgramHeader.EnableMidiPgmNumber, &pg[MPLVD]); // 0 for S900, 255 for S950

    // TODO: find out what's encoded here for S950... not used for S900
    encodeDW(ProgramHeader.PrReser2, &pg[PRRESER2]);
    encodeDD(ProgramHeader.PrReser3, &pg[PRRESER3]);
    encodeDD(ProgramHeader.PrReser4, &pg[PRRESER4]);

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
int __fastcall TFormProgram::HeaderFromArray(void)
{
  try
  {
    // ASCII sample name
    AsciiStrDecode(ProgramHeader.ProgName, &pg[PRNAME]);

    ProgramHeader.PrUndef1 = decodeDD(&pg[PRUNDEF1]);
    ProgramHeader.PrUndef2 = decodeDW(&pg[PRUNDEF2]);

    ProgramHeader.KeyTilt = decodeDW(&pg[KTILT]);

    ProgramHeader.PrUndef3 = decodeDW(&pg[PRUNDEF3]);
    ProgramHeader.PrUndef4 = decodeDB(&pg[PRUNDEF4]);

    ProgramHeader.PosXfade = decodeDB(&pg[POSXEN]);

    // reserved location with 255 (for S900)
    ProgramHeader.PrReser1 = decodeDB(&pg[PRRESER1]);

    ProgramHeader.NumKeygroups = decodeDB(&pg[NOKG]);

    ProgramHeader.PrUndef5 = decodeDW(&pg[PRUNDEF5]);

    ProgramHeader.MidiPgmNumber = decodeDB(&pg[MDPGNM]); // S950 only
    ProgramHeader.EnableMidiPgmNumber = decodeDB(&pg[MPLVD]); // 0 for S900, 255 for S950

    ProgramHeader.PrReser2 = decodeDW(&pg[PRRESER2]);
    ProgramHeader.PrReser3 = decodeDD(&pg[PRRESER3]);
    ProgramHeader.PrReser4 = decodeDD(&pg[PRRESER4]);

    return 0;
  }
  catch(...)
  {
    return -100;
  }
}
//---------------------------------------------------------------------------
// encodes KeyGroup (the working-set KEYGROUP struct) into the
// kg[140] (akai-formatted) array
// returns 0 if ok, negative if fail
int __fastcall TFormProgram::KeygroupToArray(void)
{
  try
  {
    // Clear kg array to all 0
    memset(kg, 0, PROGKEYGROUPSIZ);

    encodeDB(KeyGroup.UpperMidiKey, &kg[UMK]);
    encodeDB(KeyGroup.LowerMidiKey, &kg[LMK]);
    encodeDB(KeyGroup.VelSwThresh, &kg[VST]);
    encodeDB(KeyGroup.AttackTime, &kg[ATK]);
    encodeDB(KeyGroup.DecayTime, &kg[DCY]);
    encodeDB(KeyGroup.SustLevel, &kg[SSTN]);
    encodeDB(KeyGroup.RelTime, &kg[RLSE]);
    encodeDB(KeyGroup.FilterVelInter, &kg[FVI]);
    encodeDB(KeyGroup.FilterKeyTracking, &kg[FKI]);
    encodeDB(KeyGroup.AttackVelInter, &kg[AVI]);
    encodeDB(KeyGroup.VelRelInter, &kg[RVI]);
    encodeDB(KeyGroup.LoudnessVelInter, &kg[LVI]);
    encodeDB(KeyGroup.PitchWarpVelInter, &kg[PVI]);
    encodeDB(KeyGroup.PitchWarpOffset, &kg[PAO]);
    encodeDB(KeyGroup.PitchWarpRecoveryTime, &kg[PST]);
    encodeDB(KeyGroup.LfoBuildTime, &kg[VBDLY]);
    encodeDB(KeyGroup.LfoRate, &kg[VBRATE]);
    encodeDB(KeyGroup.LfoDepth, &kg[VBDPTH]);
    encodeDB(KeyGroup.ControlBits, &kg[KBITS]);
    encodeDB(KeyGroup.VoiceOutAssign, &kg[OPVOICE]);
    encodeDB(KeyGroup.MidiOffset, &kg[KMDCHN]);
    encodeDB(KeyGroup.AftertouchDepthMod, &kg[AFDI]);
    encodeDB(KeyGroup.ModWheelLfoDepthMod, &kg[MWDI]);
    encodeDB(KeyGroup.AdsrEnvToVcfFilter, &kg[VCFAMNT]);

    // copy ASCII sample name, pad with blanks and terminate
    AsciiStrEncode(&kg[NAMEFS], KeyGroup.SoftSampleName);

    encodeDB(KeyGroup.FilterAdsrAttackTime, &kg[VCFAK]);
    encodeDB(KeyGroup.FilterAdsrDecayTime, &kg[VCFDY]);
    encodeDB(KeyGroup.FilterAdsrSustainLevel, &kg[VCFST]);
    encodeDB(KeyGroup.FilterAdsrReleaseTime, &kg[VCFRL]);
    encodeDB(KeyGroup.VelXfadeFiftyPercent, &kg[VTMX]);

    // undefined for S900
    encodeDB(KeyGroup.KyUndef1, &kg[KYUDEF1]);
    encodeDW(KeyGroup.KyUndef2, &kg[KYUDEF2]);

    encodeDW(KeyGroup.SoftSampTransposeOffset, &kg[TROFFS]);
    encodeDB(KeyGroup.SoftSampFilter, &kg[FLTFS]);
    encodeDB(KeyGroup.SoftSampLoudness, &kg[LOUDFS]);

    // copy ASCII sample name, pad with blanks and terminate
    AsciiStrEncode(&kg[NAMESS], KeyGroup.LoudSampleName);

    // undefined for S900
    encodeDD(KeyGroup.KyUndef3, &kg[KYUDEF3]);
    encodeDD(KeyGroup.KyUndef4, &kg[KYUDEF4]);

    encodeDW(KeyGroup.LoudSampTransposeOffset, &kg[TROFSS]);
    encodeDB(KeyGroup.LoudSampFilter, &kg[FLTSS]);
    encodeDB(KeyGroup.LoudSampLoudness, &kg[LOUDSS]);

    // undefined for S900
    encodeDW(KeyGroup.KyUndef5, &kg[KYUDEF5]);

    return 0;
  }
  catch(...)
  {
    return -100;
  }
}
//---------------------------------------------------------------------------
// decodes KeyGroup (the working-set KEYGROUP struct) from the
// kg[140] (akai-formatted) array
int __fastcall TFormProgram::KeygroupFromArray(void)
{
  try
  {
    KeyGroup.UpperMidiKey = decodeDB(&kg[UMK]);
    KeyGroup.LowerMidiKey = decodeDB(&kg[LMK]);
    KeyGroup.VelSwThresh = decodeDB(&kg[VST]);
    KeyGroup.AttackTime = decodeDB(&kg[ATK]);
    KeyGroup.DecayTime = decodeDB(&kg[DCY]);
    KeyGroup.SustLevel = decodeDB(&kg[SSTN]);
    KeyGroup.RelTime = decodeDB(&kg[RLSE]);
    KeyGroup.FilterVelInter = decodeDB(&kg[FVI]);
    KeyGroup.FilterKeyTracking = decodeDB(&kg[FKI]);
    KeyGroup.AttackVelInter = decodeDB(&kg[AVI]);
    KeyGroup.VelRelInter = decodeDB(&kg[RVI]);
    KeyGroup.LoudnessVelInter = decodeDB(&kg[LVI]);
    KeyGroup.PitchWarpVelInter = decodeDB(&kg[PVI]);
    KeyGroup.PitchWarpOffset = decodeDB(&kg[PAO]);
    KeyGroup.PitchWarpRecoveryTime = decodeDB(&kg[PST]);
    KeyGroup.LfoBuildTime = decodeDB(&kg[VBDLY]);
    KeyGroup.LfoRate = decodeDB(&kg[VBRATE]);
    KeyGroup.LfoDepth = decodeDB(&kg[VBDPTH]);
    KeyGroup.ControlBits = decodeDB(&kg[KBITS]);
    KeyGroup.VoiceOutAssign = decodeDB(&kg[OPVOICE]);
    KeyGroup.MidiOffset = decodeDB(&kg[KMDCHN]);
    KeyGroup.AftertouchDepthMod = decodeDB(&kg[AFDI]);
    KeyGroup.ModWheelLfoDepthMod = decodeDB(&kg[MWDI]);
    KeyGroup.AdsrEnvToVcfFilter = decodeDB(&kg[VCFAMNT]);

    // ASCII sample name
    AsciiStrDecode(KeyGroup.SoftSampleName, &kg[NAMEFS]);

    KeyGroup.FilterAdsrAttackTime = decodeDB(&kg[VCFAK]);
    KeyGroup.FilterAdsrDecayTime = decodeDB(&kg[VCFDY]);
    KeyGroup.FilterAdsrSustainLevel = decodeDB(&kg[VCFST]);
    KeyGroup.FilterAdsrReleaseTime = decodeDB(&kg[VCFRL]);
    KeyGroup.VelXfadeFiftyPercent = decodeDB(&kg[VTMX]);

    // undefined for S900
    KeyGroup.KyUndef1 = decodeDB(&kg[KYUDEF1]);
    KeyGroup.KyUndef2 = decodeDW(&kg[KYUDEF2]);

    KeyGroup.SoftSampTransposeOffset = decodeDW(&kg[TROFFS]);
    KeyGroup.SoftSampFilter = decodeDB(&kg[FLTFS]);
    KeyGroup.SoftSampLoudness = decodeDB(&kg[LOUDFS]);

    // ASCII sample name
    AsciiStrDecode(KeyGroup.LoudSampleName, &kg[NAMESS]);

    // undefined for S900
    KeyGroup.KyUndef3 = decodeDD(&kg[KYUDEF3]);
    KeyGroup.KyUndef4 = decodeDD(&kg[KYUDEF4]);

    KeyGroup.LoudSampTransposeOffset = decodeDW(&kg[TROFSS]);
    KeyGroup.LoudSampFilter = decodeDB(&kg[FLTSS]);
    KeyGroup.LoudSampLoudness = decodeDB(&kg[LOUDSS]);

    // undefined for S900
    KeyGroup.KyUndef5 = decodeDW(&kg[KYUDEF5]);
    return 0;
  }
  catch(...)
  {
    return -100;
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormProgram::ButtonRefreshProgramsListClick(TObject *Sender)
{
  // don't allow multiple button presses
  if (IsBusy())
    return;

  try
  {
    FormMain->BusyCount++;

    // refresh the catalog
    int iError = RefreshCatalog();
    if (iError < 0 && iError != -1)
      ShowMessage("Error: unable to allocate SampleData/ProgramData! (code=" + String(iError) + ")");

    RefreshProgramsInComboBox(0);
  }
  __finally
  {
    FormMain->BusyCount--;
  }
}
//---------------------------------------------------------------------------
// return 0 if success
int __fastcall TFormProgram::RefreshCatalog(void)
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
    if (SampleData)
      delete SampleData;
    if (ProgramData)
      delete ProgramData;

    // Create new TStringList for catalog data
    SampleData = FormMain->CatalogSampleData;
    ProgramData = FormMain->CatalogProgramData;

    if (!ProgramData || !SampleData)
      return -2;

    return 0;
  }
  catch(...)
  {
    return -100;
  }
}
//---------------------------------------------------------------------------
// Returns 0 if success, negative if fail, -1 if error message already shown
int __fastcall TFormProgram::RefreshProgramsInComboBox(int iSampIdx)
{
  try
  {
    m_progIndex = -1;

    ComboBoxProgNames->Clear();

    if (!ProgramData)
    {
      ComboBoxProgNames->Text = String(NEWPROGNAME) + "00"; // new program name
      return -2;
    }

    // add programs to combo box with index in the objects list
    ComboBoxProgNames->Items->Assign(ProgramData);

    String ProgName = GetNewProgName(NEWPROGNAME);

    if (ProgName.IsEmpty())
    {
      // This should never happen...
      ShowMessage("\"NewProg XX\" out of names - delete some on target machine!");
      return -1;
    }

    ComboBoxProgNames->Text = ProgName; // new program name

    return 0;
  }
  catch(...)
  {
    ShowMessage("Unexpected error, RefreshProgramsInComboBox(), please tell author!");
    return -1;
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormProgram::RefreshKeygroupsFromSampleList(void)
{
  // Add a keygroup for each sample on machine
  SG->OnRowMoved = NULL;

  // Add rows to the grid
  if (!SampleData || SampleData->Count == 0)
    SG->RowCount = PADROWS+1;
  else
    SG->RowCount = PADROWS+SampleData->Count;

  InitKeygroupCells();

  UpdateKeygroupNumbers();

  SG->OnRowMoved = SGRowMoved;
}
//---------------------------------------------------------------------------
// When we click the Send/Save button, we need to check to see if the
// program name in the combo box is in the list of programs. If it is,
// We need to prompt the user to either change the name or overwrite
// that program.

// This routine looks for "NewProg 00", 01,02,...99 in the list of programs
// and returns the first name that's not in the list.
//---------------------------------------------------------------------------
String __fastcall TFormProgram::GetNewProgName(String sName)
{
    String s;

    // search for a NewProg ## from 00-99 that's not in the list
    int jj;
    for (jj=0; jj < MAX_PROGS; jj++)
    {
        s = sName;

        if (jj < 10)
            s += "0";

        s += String(jj);

        if (!IsNameInList(ComboBoxProgNames, s))
            break;
    }

  if (jj >= 99)
    s = "";

    return s;
}
//---------------------------------------------------------------------------
void __fastcall TFormProgram::ButtonCloseClick(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormProgram::ButtonSendClick(TObject *Sender)
{
  SendProgramToMachine();
}
//---------------------------------------------------------------------------
void __fastcall TFormProgram::MenuSendToMachineClick(TObject *Sender)
{
  SendProgramToMachine();
}
//---------------------------------------------------------------------------
void __fastcall TFormProgram::SendProgramToMachine(void)
{
  // don't allow multiple button presses
  if (IsBusy())
    return;

  try
  {
    FormMain->BusyCount++;

    try
    {
      AnsiString sProgName = ComboBoxProgNames->Text.TrimRight();
      ComboBoxProgNames->Text = sProgName;

      if (!NameOk(sProgName))
      {
        ShowMessage("Name must be 1-10 ASCII characters!");
        ComboBoxProgNames->SetFocus();
        return;
      }

      // iProgIdx, we have the indices of names on the target machine from
      // the catalog stored both in the drop-down combo-box and the
      // ProgramData TStringList. The actual index is stored in the Items->Objects[]
      // field. We also have the count of programs on the machine. So if we
      // have a new program name in ComboBoxProgNames->Text, that's not in the
      // drop-down list, set iProgIdx to ComboBoxProgNames->Items->Count. Otherwise,
      // set it to the matching Items->Objects[].
      int iProgIdx;

      int iItemIdx = ComboBoxProgNames->Items->IndexOf(sProgName);

      if (iItemIdx < 0)
      {
        // if already MAX_PROGS in list and the new name isn't one of them
        // print an error
        if (ComboBoxProgNames->Items->Count >= MAX_PROGS)
        {
          ShowMessage(String(MAX_PROGS) +
                " programs exist on machine... can't add"
                " more. You must delete a program on the"
                " machine or edit an existing program!");
          return;
        }
        iProgIdx = ComboBoxProgNames->Items->Count; // we have a new program
      }
      else
      {
        String s = "\r\n\r\n\"" + sProgName +
          "\" exists on the machine already, overwrite it?";
        int button = MessageBox(Handle, s.w_str(), L"S900/S950",
                  MB_ICONQUESTION + MB_YESNO + MB_DEFBUTTON2);

        if (button == IDNO) return;

        iProgIdx = (int)ComboBoxProgNames->Items->Objects[iItemIdx]; // overwrite old program
      }

      int progSize = GridToTempArray(iProgIdx);

      if (progSize < 2+PROGHEADERSIZ+PROGKEYGROUPSIZ)
      {
        if (progSize != -1)
          ShowMessage("problem converting grid values! (code=" + String(progSize) + ")");
        return;
      }

      // send program to Akai S950/S900, with delay
      if (!FormMain->comws(progSize, FormMain->TempArray, true))
      {
        ShowMessage("Unable to send... try again or close...");
        return;
      }

      // better delay a bit...
      FormMain->DelayGpTimer(DELAY_BETWEEN_EACH_PROGRAM_TX);

      // refresh the catalog
      int iError = RefreshCatalog();
      if (iError < 0)
      {
        if (iError != -1)
        ShowMessage("Error: unable to allocate SampleData/ProgramData! (code=" + String(iError) + ")");
        return;
      }

      // find our name in the catalog
      iItemIdx = ProgramData->IndexOf(sProgName);

      if (iItemIdx < 0)
      {
        ShowMessage("Error: The program we just tried to write does not appear in the catalog! \"" + sProgName + "\"");
        iItemIdx = 0;
      }

      RefreshProgramsInComboBox(iItemIdx);
    }
    catch(...)
    {
      ShowMessage("Unknown problem in SendProgramToMachine()!");
      return;
    }
  }
  __finally
  {
    FormMain->BusyCount--;
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormProgram::InitKeygroupCells(void)
{
  // int FormMain->NumSampEntries, NumProgEntries, SampIdx[index], ProgIdx[index]
  // String FormMain->SampName[index], ProgName[index]

  // SG->Cells[col][row]
  int NumKeyGroups = SG->RowCount - PADROWS;

  if (KeygroupLoadDefaults() < 0 || HeaderLoadDefaults(NumKeyGroups) < 0)
  {
    ShowMessage("Error loading defaults!");
    return;
  }

  for (int ii = 0; ii < NumKeyGroups; ii++)
  {
    // Fill in a name if no samples (working offline)
    AnsiString sName;
    if (!SampleData || SampleData->Count == 0)
    {
      sName = NEWSAMPNAME;
      if (ii < 10)
        sName += "0";
      sName += String(ii);
    }
    else
      sName = SampleData->Strings[ii].TrimRight();

    strncpy(KeyGroup.SoftSampleName, sName.c_str(), MAX_NAME_S900);
    KeyGroup.SoftSampleName[MAX_NAME_S900] = '\0';

    strncpy(KeyGroup.LoudSampleName, sName.c_str(), MAX_NAME_S900);
    KeyGroup.LoudSampleName[MAX_NAME_S900] = '\0';

    if (KeygroupToGui(ii) < 0)
      return;
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormProgram::UpdateKeygroupNumbers(void)
{
  // SG->Cells[col][row]
  int NumKeyGroups = SG->RowCount - PADROWS;

  if (NumKeyGroups <= 1)
  {
    if (ButtonDelKg->Enabled)
      ButtonDelKg->Enabled = false;
  }
  else
  {
    if (!ButtonDelKg->Enabled)
      ButtonDelKg->Enabled = true;
  }

  if (NumKeyGroups >= MAX_KEYGROUPS)
  {
    if (ButtonAddKg->Enabled)
      ButtonAddKg->Enabled = false;
  }
  else
  {
    if (!ButtonAddKg->Enabled)
      ButtonAddKg->Enabled = true;
  }

  int iOldCount = m_selectCount;
  m_selectCount = 0;

  for (int ii = 0; ii < NumKeyGroups; ii++)
  {
    String s = String(ii+1); // keygroup #

    int row = ii+PADROWS;

    // Increment select-count if selected... (we use Objects[0] as select flag)
    if ((int)SG->Rows[row]->Objects[0] != 0)
    {
      m_selectCount++;
      s += S_SELECTED;
    }

    SG->Cells[PADCOLS-1][row] = s;
  }

  SetButtonText(iOldCount, m_selectCount);

  // this seems to make some "vanishing cell text" issues go away...
  SG->EditorMode = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormProgram::SGRowMoved(TObject *Sender, int FromIndex, int ToIndex)

{
  UpdateKeygroupNumbers();
}
//---------------------------------------------------------------------------
void __fastcall TFormProgram::ButtonAddKgClick(TObject *Sender)
{
  CopyKeygroup1Click(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TFormProgram::ButtonDelKgClick(TObject *Sender)
{
  DeleteKeygroup1Click(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TFormProgram::DeleteKeygroup1Click(TObject *Sender)
{
  DeleteKeygroup(SG->Row);
}
//---------------------------------------------------------------------------
// delete all selected keygroups
void __fastcall TFormProgram::DeleteSelectedKeygroups(void)
{
  for (int iRow = PADROWS; iRow < SG->RowCount; iRow++)
  {
    if ((int)SG->Rows[iRow]->Objects[0] != 0)
      if (DeleteKeygroup(iRow) == 0)
        iRow--;
  }
}
//---------------------------------------------------------------------------
int __fastcall TFormProgram::DeleteKeygroup(int iRow)
{
  SG->OnRowMoved = NULL;
  int iOldCount = m_selectCount;

  try
  {
    int iRowCount = SG->RowCount;

    // don't delete header rows or 1st row!
    if (iRow < PADROWS || iRowCount <= PADROWS+1)
      return -1;

    // To delete, we move the data up than subtract a row
    for (int ii = iRow; ii < iRowCount-1; ii++)
      SG->Rows[ii]->Assign(SG->Rows[ii + 1]);

    SG->Rows[SG->RowCount-1]->Clear();
    SG->RowCount--; // delete last row

    if (SG->Row > SG->RowCount-1)
      SG->Row = SG->RowCount-1;
  }
  __finally
  {
    UpdateKeygroupNumbers();
    SetButtonText(iOldCount, m_selectCount);
    SG->OnRowMoved = SGRowMoved;
  }
  return 0;
}
//---------------------------------------------------------------------------
void __fastcall TFormProgram::CopyKeygroup1Click(TObject *Sender)
{
  SG->OnRowMoved = NULL;

  // Add a keygroup for each sample on machine
  try
  {
    int iRow = SG->Row;
    int iRowCount = SG->RowCount;

    // append a new row to the end
    if (iRowCount < MAX_KEYGROUPS + PADROWS)
    {
      SG->RowCount++;

      // copy selected row to it
      SG->Rows[iRowCount]->Assign(SG->Rows[iRow]);
    }
    else
      ShowMessage("Keygroup limit: " + String(MAX_KEYGROUPS));
  }
  __finally
  {
    UpdateKeygroupNumbers();
    SG->OnRowMoved = SGRowMoved;
  }
}
//---------------------------------------------------------------------------
// Given a +/- pitch offset in 1/16 semitone units,
// returns the course pitch in semitones
String __fastcall TFormProgram::GetCoarsePitch(Int16 rawPitch)
{
  return String(rawPitch/16);
}
//---------------------------------------------------------------------------
// Given a +/- pitch offset in 1/16 semitone units,
// returns the fine pitch in 1/16 semitones
String __fastcall TFormProgram::GetFinePitch(Int16 rawPitch)
{

  return String(rawPitch%16);
}
//---------------------------------------------------------------------------
// load program from machine
void __fastcall TFormProgram::MenuLoadFromMachineClick(TObject *Sender)
{
  // don't allow multiple button presses
  if (IsBusy())
    return;

  // Add the names to FormListBox and get the selected program
  TFormListBox* pLB = NULL;

  try
  {
    FormMain->BusyCount++;

    try
    {
      m_progIndex = -1;

      // refresh the catalog
      int iError = RefreshCatalog();
      if (iError < 0)
      {
        if (iError != -1)
          ShowMessage("unable to allocate SampleData/ProgramData! (code=" + String(iError) + ")");
        return;
      }

      if (!ProgramData->Count)
      {
        ShowMessage("no programs on machine!");
        return;
      }

      int iProgIdx;

      if (ProgramData->Count == 1)
        iProgIdx = (int)ProgramData->Objects[0];
      else // more than one need to choose...
      {
        // display program names in list box
        // resulting index will be in
        Application->CreateForm(__classid(TFormListBox), &pLB);
        if (!pLB) return;

        pLB->Topic = "Load Program";

        pLB->ListBox1->Assign(ProgramData);

        if (pLB->ShowModal() == mrCancel)
          return;

        iProgIdx = (int)ProgramData->Objects[pLB->ItemOffset];
      }

      iError = LoadProgramFromMachine(iProgIdx);
      if (iError < 0)
      {
        if (iError != -1)
          ShowMessage("Error in LoadProgramFromMachine(), (code=" +
                                                  String(iError) + ")");
      }

      // refresh combo-box too!
      iError = RefreshProgramsInComboBox(pLB->ItemOffset);
      if (iError < 0)
      {
        if (iError != -1)
          ShowMessage("unable to refresh programs in ComboBox! (code=" + String(iError) + ")");
        return;
      }

      m_progIndex = iProgIdx; // global index we can use for sending the program back...
    }
    catch(...)
    {
      ShowMessage("Unable to load program!");
    }
  }
  __finally
  {
    if (pLB) pLB->Release();

    FormMain->BusyCount--;
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormProgram::ComboBoxProgNamesSelect(TObject *Sender)
{
  // don't allow multiple button presses
  if (IsBusy())
    return;

  try
  {
    FormMain->BusyCount++;

    try
    {
      m_progIndex = -1;

      // here, the user has selected a program from the drop-down list
      int idx = (int)ComboBoxProgNames->Items->Objects[ComboBoxProgNames->ItemIndex];
      int iError = LoadProgramFromMachine(idx);
      if (iError < 0)
      {
        if (iError != -1)
          ShowMessage("Error in LoadProgramFromMachine(), (code=" +
                                                  String(iError) + ")");
      }

      m_progIndex = idx; // global index we can use for sending the program back...
    }
    catch(...)
    {
      ShowMessage("Error in ComboBoxProgNamesSelect()!");
    }
  }
  __finally
  {
    FormMain->BusyCount--;
  }
}
//---------------------------------------------------------------------------
// load program from machine
// returns -1 if error message displayed, 0 if success
int __fastcall TFormProgram::LoadProgramFromMachine(int iPrgIdx)
{
  ButtonSend->Enabled = false;
  MenuSendToMachine->Enabled = false;

  int progSize = FormMain->LoadProgramToTempArray(iPrgIdx); // get the program into TempArray

  if (progSize < 2+PROGHEADERSIZ+PROGKEYGROUPSIZ)
  {
    ShowMessage("Unable to load program... check RS232 cable, baud-rate or power!");
    return -1;
  }

  int iError = ProgFromTempArrayToGui();
  if (iError < 0)
  {
    if (iError != -1)
      ShowMessage("Error in ProgramFromTempArrayToGui(), (code=" +
                                              String(iError) + ")");
    return -1;
  }

  ButtonSend->Enabled = true;
  MenuSendToMachine->Enabled = true;

  return 0;
}
//---------------------------------------------------------------------------
void __fastcall TFormProgram::MenuSaveIntoPrgFileClick(TObject *Sender)
{
  // First, make sure the user's program name is ok...
  AnsiString sProgName = ComboBoxProgNames->Text.TrimRight();
  ComboBoxProgNames->Text = sProgName;

  if (!NameOk(sProgName))
  {
    ShowMessage("Name must be 1-10 ASCII characters!");
    ComboBoxProgNames->SetFocus();
    return;
  }

  String sFilePath;

  try
  {
    SaveDialog1->Title = "Write program into .prg file...";
    SaveDialog1->DefaultExt = "prg";
    SaveDialog1->Filter = "Programs file (*.prg)|*.prg|"
      "All files (*.*)|*.*";
    SaveDialog1->FilterIndex = 1; // start the dialog showing .prg files
    SaveDialog1->Options.Clear();
//    SaveDialog1->Options << ofHideReadOnly
//      << ofPathMustExist << ofOverwritePrompt << ofEnableSizing
//      << ofNoReadOnlyReturn;
    // Don't need overwrite prompt since we can add programs or delete them...
    SaveDialog1->Options << ofHideReadOnly
      << ofPathMustExist << ofEnableSizing << ofNoReadOnlyReturn;

    sFilePath = String(DEFSAVENAME) + String(".prg");

    SaveDialog1->FileName = sFilePath;

    if (!SaveDialog1->Execute()) return;

    // Assign the full file name
    sFilePath = SaveDialog1->FileName;

    TStringList* sl = NULL;

    long lFileHandle = 0;

    try
    {
      sl = new TStringList(); // list of programs in file and file position

      if (!sl) return;

      if (!FileExists(sFilePath))
        lFileHandle = FileCreate(sFilePath);
      else
        lFileHandle = FileOpen(sFilePath, fmShareDenyNone | fmOpenReadWrite);

      if (lFileHandle == 0)
      {
        ShowMessage("Unable to open file for writing: \"" + sFilePath + "\"");
        return;
      }

      // initialize a new, empty .prg file (it will have 4 bytes in it for EOF)
      int iFileSize = FileSeek(lFileHandle, 0, 2); // point to eof
      if (iFileSize == 0)
      {
        FileSeek(lFileHandle, 0, 0); // point to start
        Int32 magicNum = MAGIC_NUM_PRG;
        FileWrite(lFileHandle, &magicNum, UINT32SIZE);
        Int32 numProgs = 0; // will be incremented when we add program(s)
        FileWrite(lFileHandle, &numProgs, UINT32SIZE);
        iFileSize = UINT32SIZE*3;
        FileWrite(lFileHandle, &iFileSize, UINT32SIZE);
      }
      else // go ahead and read the program names in the file...
      {
        int iError = ReadNamesInPrgFile(lFileHandle, sl);

        if (iError < 0)
        {
           ShowMessage("Unable to read file:\r\r\n\"" +
              sFilePath + "\" (code=" + String(iError) + ")");
           return;
        }

        if (sl->Count)
        {
          // If sProgName is in the list of names in sl
          int idx = sl->IndexOf(sProgName);
          if (idx >= 0)
          {
            String s = "\r\n\r\n\"" + sProgName +
              "\" exists in the .prg file already, overwrite it?";
            int button = MessageBox(Handle, s.w_str(), L"S900/S950",
                      MB_ICONQUESTION + MB_YESNO + MB_DEFBUTTON2);

            if (button == IDNO) return;

            iError = DeletePrgFromFile(sFilePath, lFileHandle, (int)sl->Objects[idx]);
            if (iError < 0)
            {
              if (iError != -1)
                ShowMessage("problem deleting file..."
                      " (code=" + String(iError) + ")");
                return;
            }
          }
        }
      }

      // Append to end of existing .prg file
      int iError = GridToFile(lFileHandle);
      if (iError < 0)
      {
        if (iError != -1)
          ShowMessage("problem writing to file... (code=" + String(iError) + ")");
        return;
      }
    }
    __finally
    {
      if (sl) delete sl;
      if (lFileHandle) FileClose(lFileHandle);
    }
  }
  catch (...)
  {
    ShowMessage("Error, can't save file: \"" + SaveDialog1->FileName + "\"");
  }
}
//---------------------------------------------------------------------------
// delete program from file at iFileOffset
// returns new file-size if success or negative error code
int __fastcall TFormProgram::DeletePrgFromFile(String sFilePath, long &lFileHandle, int iFileOffset)
{
  Byte* buf = NULL;
  Int32 fileSize = 0;

  try
  {
    try
    {
      // Seek: Offset, Mode 0=beginning, 1=current pos, 2=end
      FileSeek(lFileHandle, 0, 0); // reset
      uint32_t magicNum;
      FileRead(lFileHandle, &magicNum, UINT32SIZE);
      if (magicNum != MAGIC_NUM_PRG)
        return -2; // not a .prg file
      uint32_t numProgs;
      FileRead(lFileHandle, &numProgs, UINT32SIZE);
      if (numProgs == 0)
        return -3; // no programs to delete

      String sTempPath = sFilePath + ".tmp"; // double extension

      if (FileExists(sTempPath))
        DeleteFile(sTempPath);

      long lTempHandle = FileCreate(sTempPath);
      if (lTempHandle == 0)
        return -4;

      // read/write first part of file
      FileSeek(lFileHandle, 0, 0); // reset
      buf = new Byte[iFileOffset];
      int iByteCount = FileRead(lFileHandle, buf, iFileOffset);
      if (iByteCount != iFileOffset)
        return -5;
      iByteCount = FileWrite(lTempHandle, buf, iFileOffset); // write first part to temp file
      if (iByteCount != iFileOffset)
        return -6;
      delete [] buf;
      buf = NULL;

      Int32 progSize;
      iByteCount = FileRead(lFileHandle, &progSize, UINT32SIZE);
      if (iByteCount != UINT32SIZE)
        return -7;

      int iStart = FileSeek(lFileHandle, progSize, 1); // point 1 prog beyond the one we are deleting
      int iEnd = FileSeek(lFileHandle, 0, 2); // point to eof

      if (iStart < 0 || iEnd < 0)
        return -8;

      // now we need to move data past this program into temp file...
      int iSize = iEnd-iStart;
      buf = new Byte[iSize];
      FileSeek(lFileHandle, iStart, 0);
      iByteCount = FileRead(lFileHandle, buf, iSize); // read second part to buf
      if (iByteCount != iSize)
        return -9;
      FileSeek(lFileHandle, iFileOffset, 0);
      iByteCount = FileWrite(lTempHandle, buf, iSize); // write second part to temp file
      if (iByteCount != iSize)
        return -10;
      delete buf;
      buf = 0;

      FileClose(lFileHandle);

      // point 4 bytes from end and write the file-size
      fileSize = FileSeek(lTempHandle, -UINT32SIZE, 2) + UINT32SIZE;
      iByteCount = FileWrite(lTempHandle, &fileSize, UINT32SIZE); // write new filesize
      if (iByteCount != UINT32SIZE)
        return -11;
      FileSeek(lTempHandle, UINT32SIZE, 0); // point to # programs, past magic #
      numProgs--; // 1 less program
      iByteCount = FileWrite(lTempHandle, &numProgs, UINT32SIZE); // write new filesize
      if (iByteCount != UINT32SIZE)
        return -12;

      // close both files, delete old and rename new to old
      FileClose(lTempHandle);

      try
      {
        DeleteFile(sFilePath);
      }
      catch(...) {}
      try
      {
        RenameFile(sTempPath, sFilePath);
      }
      catch (...) {}

      // reopen and assign handle to new file by reference
      lFileHandle = FileOpen(sFilePath, fmShareDenyNone | fmOpenReadWrite);
      if (lFileHandle == 0)
        return -13;
    }
    __finally
    {
      if (buf)
        delete [] buf;
    }

    return fileSize;
  }
  catch(...)
  {
    return -100;
  }
}
//---------------------------------------------------------------------------
// append program in TempArray to end of existing .prg file
// returns program size if success or negative error code
int __fastcall TFormProgram::GridToFile(long lFileHandle)
{
  try
  {
    // here, the user has selected an item from the drop-down list
    // and it's showing in the text-field...
//    int progIdx = (int)ComboBoxProgNames->Items->Objects[ComboBoxProgNames->ItemIndex];

    // In MainForm.cpp, in PutPrograms() (when user uploads all programs in a .prg file)
    // I'm now resequencing the program numbers on the fly 0, 1, 2... overwriting
    // programs on the machine - file-storage prog-number is not really useful,
    // could make it into a temporary "internal" field in-future...
    // Also - when sending a program from this editor to the machine, we should use
    // the index from the catalog if the program already exists, or 1 greater
    // then the number of programs if it does not.
    int progIdx = 0; // NOT SURE YET WHAT TO PUT HERE!!!!!!!!!!!!!!!!!!!!!!!!!!
    int progSize = GridToTempArray(progIdx);

    if (progSize < 2+PROGHEADERSIZ+PROGKEYGROUPSIZ)
    {
      if (progSize != -1)
      {
        ShowMessage("problem converting grid values! (code=" + String(progSize) + ")");
        return -1;
      }
      return -2;
    }

    // Seek -4 from end of file
    int oldFileSize = FileSeek(lFileHandle, -UINT32SIZE, 2) + UINT32SIZE;

    // write the new program's size
    int byteCount = FileWrite(lFileHandle, &progSize, UINT32SIZE);

    if (byteCount != UINT32SIZE)
      return -3;

    // write the new program
    byteCount = FileWrite(lFileHandle, FormMain->TempArray, progSize); // -1 if error

    if (byteCount != progSize)
      return -4;

    // write the new size of the .prg file
    int newFileSize = oldFileSize+progSize+UINT32SIZE;
    byteCount = FileWrite(lFileHandle, &newFileSize, UINT32SIZE); // -1 if error

    if (byteCount != UINT32SIZE)
      return -5;

    // Update # programs
    int numProgs = 0;
    FileSeek(lFileHandle, UINT32SIZE, 0); // seek past magic-num to #programs
    byteCount = FileRead(lFileHandle, &numProgs, UINT32SIZE);
    if (byteCount != UINT32SIZE)
      return -6;
    numProgs++;
    FileSeek(lFileHandle, UINT32SIZE, 0); // seek past magic-num to #programs
    byteCount = FileWrite(lFileHandle, &numProgs, UINT32SIZE);
    if (byteCount != UINT32SIZE)
      return -7;
    //ShowMessage(String(progSize) + ", " + String(newFileSize)); // 225, 241
    return progSize;
  }
  catch(...)
  {
    return -100;
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormProgram::MenuLoadFromPrgFileClick(TObject *Sender)
{
  LoadOrDeleteProgramFromFile(true);
}
//---------------------------------------------------------------------------
void __fastcall TFormProgram::MenuDeleteProgClick(TObject *Sender)
{
  LoadOrDeleteProgramFromFile(false);
}
//---------------------------------------------------------------------------
void __fastcall TFormProgram::LoadOrDeleteProgramFromFile(bool bLoad)
{
  if (bLoad)
  {
    ButtonSend->Enabled = false;
    MenuSendToMachine->Enabled = false;
  }

  try
  {
    if (bLoad)
      OpenDialog1->Title = "Load program from .prg file for editing";
    else
      OpenDialog1->Title = "Delete program from .prg file";
    OpenDialog1->DefaultExt = "prg";
    OpenDialog1->Filter = "Prg file (*.prg)|*.prg|" "All files (*.*)|*.*";
    OpenDialog1->FilterIndex = 1; // start the dialog showing .prg files
    OpenDialog1->Options.Clear();
    OpenDialog1->Options << ofHideReadOnly << ofPathMustExist <<
                    ofFileMustExist << ofEnableSizing;

    if (!OpenDialog1->Execute())
      return; // Cancel

    String sFilePath = OpenDialog1->FileName;

    TStringList* sl = NULL;
    TFormListBox* pLB = NULL;

    long lFileHandle = 0;

    try
    {
      sl = new TStringList(); // list of programs in file and file position

      if (!sl) return;

      lFileHandle = FileOpen(sFilePath, fmShareDenyNone | fmOpenRead);

      if (lFileHandle == 0)
        return;

      int iError = ReadNamesInPrgFile(lFileHandle, sl);

      if (iError < 0)
      {
        if (iError != -1)
           ShowMessage("Unable to read file:\r\r\n\"" +
              sFilePath + "\" ReadNamesInPrgFile() (code=" + String(iError) + ")");
         return;
      }

      if (!sl->Count)
      {
        ShowMessage("no programs in this file!");
        return;
      }

      int prgOffset; // offset into file where the program size is (4 bytes)

      if (sl->Count == 1 && bLoad)
        prgOffset = (int)sl->Objects[0];
      else // more than one (or deleting) need to choose...
      {
        // display program names in list box
        // resulting index will be in
        Application->CreateForm(__classid(TFormListBox), &pLB);
        if (!pLB) return;

        pLB->Topic = bLoad ? "Load Program" : "Delete Program";

        for (int ii = 0; ii < sl->Count; ii++)
          pLB->ListBox1->Items->AddObject(sl->Strings[ii], sl->Objects[ii]);

        if (pLB->ShowModal() == mrCancel)
          return;

        prgOffset = pLB->ItemOffset;
      }

      //ShowMessage(String(pLB->SelIndex));
      //ShowMessage(String(pLB->PrgFileOffset));

      if (bLoad)
      {
        iError = ProgFromFileToGui(lFileHandle, prgOffset);
        if (iError < 0)
        {
          if (iError != -1)
            ShowMessage("Unable to load program ProgFromFileToGui()\r\n"
                                  "(code=" + String(iError) + ")");
        }
      }
      else
      {
        iError = DeletePrgFromFile(sFilePath, lFileHandle, prgOffset);
        if (iError < 0)
        {
          if (iError != -1)
            ShowMessage("Unable to delete program (code=" +
                                              String(iError) + ")");
        }
      }

      if (bLoad)
      {
        ButtonSend->Enabled = true;
        MenuSendToMachine->Enabled = true;
      }
    }
    __finally
    {
      if (sl) delete sl;
      if (pLB) pLB->Release();
      if (lFileHandle) FileClose(lFileHandle);
    }
  }
  catch(...)
  {
    ShowMessage("Exception thrown in LoadOrDeleteProgramFromFile()!");
  }
}
//---------------------------------------------------------------------------
// Reads program from file and writes it to the string-grid
// returns program's size or negative if error
// (-1 means we already displayed the error message)
int __fastcall TFormProgram::ProgFromFileToGui(long lFileHandle, int iFileOffset)
{
  if (lFileHandle == 0)
    return -2;

  try
  {
    // Read program into TempArray
    Int32 progSize = ProgFromFileToTempArray(lFileHandle, iFileOffset);
    if (progSize <= 0)
      return -3;

    int iError = ProgFromTempArrayToGui();
    if (iError < 0)
    {
      if (iError != -1)
        ShowMessage("Error in ProgramFromTempArrayToGui() (code=" +
                                                String(iError) + ")");
      return -1;
    }

    return progSize;
  }
  catch(...)
  {
    return -100;
  }
}
//---------------------------------------------------------------------------
// Put a program in TempArray to the grid
// returns 0 if success, negative if fail
int __fastcall TFormProgram::ProgFromTempArrayToGui(void)
{
  try
  {
    // Clear string-grid
    ClearStringGrid();

    // Copy program header info
    memcpy(pg, FormMain->TempArray, PROGHEADERSIZ);

     // read user controls into ProgramHeader struct
    int iError = HeaderFromArray();
    if (iError < 0)
    {
      if (iError != -1)
        ShowMessage("Error in HeaderFromArray(), (code=" +
                                              String(iError) + ")");
      return -1;
    }

    // move info in pg[] into GUI controls
    iError = HeaderToGui();
    if (iError < 0)
    {
      if (iError != -1)
        ShowMessage("Error in HeaderToGui(), (code=" +
                                              String(iError) + ")");
      return -1;
    }

    if (ProgramHeader.NumKeygroups > MAX_KEYGROUPS)
      return -2;

    int offset = PROGHEADERSIZ;

    for (int ii = 0; ii < ProgramHeader.NumKeygroups; ii++)
    {
      // Copy keygroup info
      memcpy(kg, &FormMain->TempArray[offset], PROGKEYGROUPSIZ);
      offset += PROGKEYGROUPSIZ;

      iError = KeygroupFromArray();
      if (iError < 0)
      {
        if (iError != -1)
          ShowMessage("Error in KeygroupFromArray(), (code=" +
                                                String(iError) + ")");
        return -1;
      }

      if (ii > 0)
        SG->RowCount++;

      iError = KeygroupToGui(ii);
      if (iError < 0)
      {
        if (iError != -1)
          ShowMessage("Error in KeygroupToGui(), (code=" +
                                                String(iError) + ")");
        return -1;
      }

      UpdateKeygroupNumbers();
    }
    return 0;
  }
  catch(...)
  {
    return -100;
  }
}
//---------------------------------------------------------------------------
// read a program from a .prg file into TempArray, given its offset
// returns file-size or negative number if error
int __fastcall TFormProgram::ProgFromFileToTempArray(long lFileHandle, int iFileOffset)
{
  if (lFileHandle == 0)
    return -2;

  try
  {
    FileSeek(lFileHandle, iFileOffset, 0); // point to program

    Int32 progSize;
    int bytesRead = FileRead(lFileHandle, &progSize, UINT32SIZE);

    if (bytesRead != UINT32SIZE)
      return -3;

    // sanity check...
    if (progSize > TEMPARRAYSIZ)
      return -4;

    // copy program in file to buf
    bytesRead = FileRead(lFileHandle, FormMain->TempArray, progSize);

    if (bytesRead != progSize)
      return -5;

    return progSize;
  }
  catch(...)
  {
    return -100;
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormProgram::ClearStringGrid(void)
{
  if (!SG) return;
  SG->RowCount = PADROWS+1;
}
//---------------------------------------------------------------------------
// open a .prg file and return a string list of program names
// with the file-offset of the program size in the TObject* field
// returns negative if failure
int __fastcall TFormProgram::ReadNamesInPrgFile(long lFileHandle, TStringList* sl)
{
  if (lFileHandle == 0)
    return -2;

  Byte* buf = NULL;
  uint32_t numProgs = 0; // need this in __finally

  try
  {
    try
    {
      uint32_t iFileLength = FileSeek(lFileHandle, 0, 2); // seek to end

      // seek/read the stored file-length (int32_t at end of the file)
      FileSeek(lFileHandle, -UINT32SIZE, 2);
      uint32_t storedFileLength;
      uint32_t bytesRead = FileRead(lFileHandle, &storedFileLength, UINT32SIZE);

      if (bytesRead != UINT32SIZE)
        return -3;

      if (storedFileLength != iFileLength)
        return -4;

      FileSeek(lFileHandle, 0, 0); // back to start

      // read the magic number
      uint32_t my_magic; // uniquely identifies a .prg file
      bytesRead = FileRead(lFileHandle, &my_magic, UINT32SIZE);

      if (bytesRead != UINT32SIZE)
        return -5;

      if (my_magic != MAGIC_NUM_PRG)
        return -6;

      // read the number of programs
      bytesRead = FileRead(lFileHandle, &numProgs, UINT32SIZE);

      if (bytesRead != UINT32SIZE)
        return -7;

      if (numProgs > MAX_PROGS)
        return -8;

      if (numProgs == 0)
        return 0; // return "no error" - allow 0 programs...

      // read each program in file
      for (uint32_t ii = 0; ii < numProgs; ii++)
      {
        Application->ProcessMessages();

        // save file-offset for origin of the program
        int progPos = FileSeek(lFileHandle,0,1); // 0 bytes from current position

        // read program-size from file.
        // a program in the file is already formatted for the S950 and
        // includes the BEX, checksum and EEX.
        uint32_t progSize;
        bytesRead = FileRead(lFileHandle, &progSize, UINT32SIZE);
        if (bytesRead != UINT32SIZE)
          return -9;

        // sanity check!
        // ShowMessage("progsize: " + String(progSize) + ", TEMPARRAYSIZ: " + String(TEMPARRAYSIZ));
        if (progSize > TEMPARRAYSIZ)
          return -10;

        buf = new Byte[progSize];

        if (buf == NULL)
          return -11;

        // copy program in file to buf
        bytesRead = FileRead(lFileHandle, buf, progSize);

        if (bytesRead != progSize)
          return -12;

        if (buf[0] != BEX)
          return -13;

        if (buf[progSize - 1] != EEX)
          return -14;

        // add program name to list
        char bufName[MAX_NAME_S900+1];
        AsciiStrDecode(bufName, &buf[PRNAME]);

        delete [] buf; // finished with program and its buffer
        buf = NULL; // need this because of __finally!

        // program name
        String sName = String(bufName).TrimRight();

        sl->AddObject(sName, (TObject*)progPos);
      }
    }
    __finally
    {
      if (buf != NULL)
        delete [] buf;
    }
  }
  catch(...)
  {
    return -100;
  }

  return 0;
}
//---------------------------------------------------------------------------
// take program in grid and format it as a program ready to transmit
// or save into a .prg file
// returns the program's size or negative if error
// returns -1 if error message printed here or progSize
int __fastcall TFormProgram::GridToTempArray(int progIdx)
{
  try
  {
    ProgramHeader.NumKeygroups = SG->RowCount - PADROWS;

    int iError = HeaderFromGui();
    if (iError < 0) // read user controls into ProgramHeader struct
    {
      if (iError != -1)
        ShowMessage("Error in HeaderFromGui(), (code=" +
                                              String(iError) + ")");
      return -1;
    }

    // Copy program header info
    iError = HeaderToArray(progIdx);
    if (iError < 0) // encode from ProgramHeader struct into pg array
    {
      if (iError != -1)
        ShowMessage("Error in HeaderToArray(), (code=" +
                                              String(iError) + ")");
      return -1;
    }

    memcpy(FormMain->TempArray, pg, PROGHEADERSIZ);

    // encode the keygroups to kg then move to TempArray
    int taIdx = PROGHEADERSIZ;
    for (int ii = 0; ii < ProgramHeader.NumKeygroups; ii++)
    {
      iError = KeygroupFromGui(ii);

      if (iError < 0)
      {
        if (iError != -1)
          ShowMessage("Error in KeygroupFromGui(), (code=" +
                                                String(iError) + ")");
        return -1;
      }

      iError = KeygroupToArray(); // encode to kg[]
      if (iError < 0)
      {
        if (iError != -1)
          ShowMessage("Error in KeygroupToArray(), (code=" +
                                                String(iError) + ")");
        return -1;
      }

      memcpy(&FormMain->TempArray[taIdx], kg, PROGKEYGROUPSIZ);
      taIdx += PROGKEYGROUPSIZ;
    }

    // checksum is exclusive or of 7-(taIdx-1)
    // and the value is put in taIdx
    compute_checksum(7, taIdx, FormMain->TempArray);
    taIdx++;

    FormMain->TempArray[taIdx] = EEX;
    taIdx++; // need this! (we return the program-size)

    // diagnostic printout of hex values in main window
    if (MenuShowUndefined->Checked)
      display_hex(FormMain->TempArray, taIdx);

    return taIdx; // program size
  }
  catch(...)
  {
    return -100;
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormProgram::MenuShowUndefinedClick(TObject *Sender)
{
  MenuShowUndefined->Checked = !MenuShowUndefined->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TFormProgram::MenuDrumifyClick(TObject *Sender)
{
  int NumKeyGroups = SG->RowCount - PADROWS;

  bool bFirst = true;
  int iKey = -1;
  int coarsePitch = -1;
  int midiOffset = -1;
  int iSelectCount = CountSelected();

  if (iSelectCount == 0)
  {
    ShowMessage("Nothing selected...\n"
          "Press Ctrl and left-click keygroups to select them.");
    return;
  }

  for (int ii = 0; ii < NumKeyGroups; ii++)
  {
    int row = ii+PADROWS;

    // Drumify if selected... (we use Objects[0] as select flag)
    if ((int)SG->Rows[row]->Objects[C_KEYGROUP] != 0)
    {
      if (bFirst)
      {
        String s = SG->Cells[C_LOWKEY][row];

        // Read the low-key of the 1st selected keygroup as our base key
        // then set the high key the same and iterate, advancing to the next black key
        iKey = StringToKey(s);

        if (iKey < 0)
        {
          ShowMessage("Invalid low-key in first selected row: \"" + s + "\"");
          SG->Row = row;
          SG->Col = C_LOWKEY;
          return;
        }

        if (iKey + iSelectCount > 128)
        {
          ShowMessage("Can't drumify all selected keygroups, max key is \"8B\"!"
              " Reduce Low Key by " + String(iKey + iSelectCount - 128) + " semitone(s)...");
          SG->Row = row;
          SG->Col = C_LOWKEY;
          return;
        }

        coarsePitch = SG->Cells[C_SS_COARSEPITCH][row].ToIntDef(0);
        midiOffset = SG->Cells[C_MIDIOFFSET][row].ToIntDef(0);

        bFirst = false;
      }
      else if (iKey > 127)
      {
        ShowMessage("Error: iKey > 127!");
        SG->Row = row;
        SG->Col = C_LOWKEY;
        return;
      }


      String sKey = KeyToString(iKey);

      // now, we need to offset the pitch, up if we've lowered the key and
      // down if we've raised the key. we assume a base raw pitch of 960 (C3)
      // we won't be changing the fine pitch at all. Units are 1/16 semitone.
      // So really, I think all we need to do is change the Coarse Pitch field
      // up or down, in semitones. So if the base key here is C3, there is
      // no change to that pitch. if it's C2, we need to add +12 to the coarse pitch field.
      // Also, we should "make no change" if coarse pitch is anything other than 0, initially.
      //
      // 24 is C0 so C3 is 24+12+12+12 = 60
      // so we want to set the coarse pitch to 60 - iKey

      // Adjust the pitch...
      // (NOTE: for now, we make a huge presumption, namely that the sample's
      // base pitch is middle-C, the default for all WAV samples uploaded to
      // the machine. In future, ideally, you need to load the sample
      // parameters from the machine, for each drum sample, and use the "actual"
      // base pitch, not KEY_MIDDLE_C)
      int newPitch = coarsePitch + (KEY_MIDDLE_C-iKey);

      SG->Cells[C_LOWKEY][row] = sKey;
      SG->Cells[C_HIGHKEY][row] = sKey;
      SG->Cells[C_SS_COARSEPITCH][row] = String(newPitch);
      SG->Cells[C_MIDIOFFSET][row] = String(midiOffset);

      iKey++;
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormProgram::MenuDrumifyBlackClick(TObject *Sender)
{
  int NumKeyGroups = SG->RowCount - PADROWS;

  bool bFirst = true;
  int iKey = -1;
  int coarsePitch = -1;
  int midiOffset = -1;
  int iSelectCount = CountSelected();

  if (iSelectCount == 0)
  {
    ShowMessage("Nothing selected...\n"
          "Press Ctrl and left-click keygroups to select them.");
    return;
  }

  for (int ii = 0; ii < NumKeyGroups; ii++)
  {
    int row = ii+PADROWS;

    // Drumify if selected... (we use Objects[0] as select flag)
    if ((int)SG->Rows[row]->Objects[C_KEYGROUP] != 0)
    {
      if (bFirst)
      {
        String s = SG->Cells[C_LOWKEY][row];

        // Read the low-key of the 1st selected keygroup as our base key
        // then set the high key the same and iterate, advancing to the next black key
        iKey = StringToKey(s);

        if (iKey < 0)
        {
          ShowMessage("Invalid low-key in first selected row: \"" + s + "\"");
          SG->Row = row;
          SG->Col = C_LOWKEY;
          return;
        }

        if (!IsBlackKey(iKey))
        {
          ShowMessage("\"" + s + "\" is not a half-note (black) key!\n"
              "Type in a semitone key for the first selected keygroup,\n"
              "such as \"0C#\" or \"3Eb\"");

          // focus
          SG->Row = row;
          SG->Col = C_LOWKEY;
          return;
        }

        if (iKey + iSelectCount > 128)
        {
          ShowMessage("Can't drumify all selected keygroups, max key is \"8B\"!"
              " Reduce Low Key by " + String(iKey + iSelectCount - 128) + " semitone(s)...");
          SG->Row = row;
          SG->Col = C_LOWKEY;
          return;
        }

        coarsePitch = SG->Cells[C_SS_COARSEPITCH][row].ToIntDef(0);
        midiOffset = SG->Cells[C_MIDIOFFSET][row].ToIntDef(0);

        bFirst = false;
      }
      else if (iKey > 127)
      {
        ShowMessage("Error: iKey > 127!");
        return;
      }

      String sKey = KeyToString(iKey);

      // Adjust the pitch...
      int newPitch = coarsePitch + (KEY_MIDDLE_C-iKey);

      SG->Cells[C_LOWKEY][row] = sKey;
      SG->Cells[C_HIGHKEY][row] = sKey;
      SG->Cells[C_SS_COARSEPITCH][row] = String(newPitch);
      SG->Cells[C_MIDIOFFSET][row] = String(midiOffset);

      iKey = NextBlackKey(iKey); // next key
    }
  }
}
//---------------------------------------------------------------------------
// Column
// Lower key column = C_LOWKEY,
// Upper key column = C_HIGHKEY
void __fastcall TFormProgram::MenuDrumifyWhiteClick(TObject *Sender)
{
  int NumKeyGroups = SG->RowCount - PADROWS;

  bool bFirst = true;
  int iKey = -1;
  int coarsePitch = -1;
  int midiOffset = -1;
  int iSelectCount = CountSelected();

  if (iSelectCount == 0)
  {
    ShowMessage("Nothing selected...\n"
          "Press Ctrl and left-click keygroups to select them.");
    return;
  }

  for (int ii = 0; ii < NumKeyGroups; ii++)
  {
    int row = ii+PADROWS;

    // Drumify if selected... (we use Objects[0] as select flag)
    if ((int)SG->Rows[row]->Objects[C_KEYGROUP] != 0)
    {
      if (bFirst)
      {
        String s = SG->Cells[C_LOWKEY][row];

        // Read the low-key of the 1st selected keygroup as our base key
        // then set the high key the same and iterate, advancing to the next white key
        iKey = StringToKey(s);

        if (iKey < 0)
        {
          ShowMessage("Invalid low-key in first selected row: \"" + s + "\"");
          SG->Row = row;
          SG->Col = C_LOWKEY;
          return;
        }

        if (!IsWhiteKey(iKey))
        {
          ShowMessage("\"" + s + "\" is not a whole-note (white) key!\n"
              "Type in a non-flat/non-sharp key for the first selected keygroup,\n"
              "such as \"0C\" or \"3E\"");

          // focus
          SG->Row = row;
          SG->Col = C_LOWKEY;
          return;
        }

        if (iKey + iSelectCount > 128)
        {
          ShowMessage("Can't drumify all selected keygroups, max key is \"8B\"!"
              " Reduce Low Key by " + String(iKey + iSelectCount - 128) + " semitone(s)...");
          SG->Row = row;
          SG->Col = C_LOWKEY;
          return;
        }

        coarsePitch = SG->Cells[C_SS_COARSEPITCH][row].ToIntDef(0);
        midiOffset = SG->Cells[C_MIDIOFFSET][row].ToIntDef(0);

        bFirst = false;
      }
      else if (iKey > 127)
      {
        ShowMessage("Error: iKey > 127!");
        SG->Row = row;
        SG->Col = C_LOWKEY;
        return;
      }

      String sKey = KeyToString(iKey);

      // Adjust the pitch...
      int newPitch = coarsePitch + (KEY_MIDDLE_C-iKey);

      SG->Cells[C_LOWKEY][row] = sKey;
      SG->Cells[C_HIGHKEY][row] = sKey;
      SG->Cells[C_SS_COARSEPITCH][row] = String(newPitch);
      SG->Cells[C_MIDIOFFSET][row] = String(midiOffset);

      iKey = NextWhiteKey(iKey); // next key
    }
  }
}
//---------------------------------------------------------------------------
// assign 1 octave to each selected sample starting at
// first selected sample's Low Key
void __fastcall TFormProgram::MenuOctifyKGClick(TObject *Sender)
{
  int NumKeyGroups = SG->RowCount - PADROWS;

  bool bFirst = true;
  int iKey = -1;
  int iSelectCount = CountSelected();

  if (iSelectCount == 0)
  {
    ShowMessage("Nothing selected...\n"
          "Press Ctrl and left-click keygroups to select them.");
    return;
  }

  for (int ii = 0; ii < NumKeyGroups; ii++)
  {
    int row = ii+PADROWS;

    // Drumify if selected... (we use Objects[0] as select flag)
    if ((int)SG->Rows[row]->Objects[C_KEYGROUP] != 0)
    {
      if (bFirst)
      {
        String s = SG->Cells[C_LOWKEY][row];

        // Read the low-key of the 1st selected keygroup as our base key
        // then set the high key the same and iterate, advancing to the next white key
        iKey = StringToKey(s);

        if (iKey < 0)
        {
          ShowMessage("Invalid low-key in first selected row: \"" + s + "\"");
          SG->Row = row;
          SG->Col = C_LOWKEY;
          return;
        }

        if (iKey + (iSelectCount*12) > 128)
        {
          ShowMessage("Can't octify all selected keygroups, max key is \"8B\"!"
              " Reduce Low Key by " + String(iKey + (iSelectCount*12) - 128) + " semitone(s)...");
          SG->Row = row;
          SG->Col = C_LOWKEY;
          return;
        }

        bFirst = false;
      }
      else if (iKey > 127)
      {
        ShowMessage("Error: iKey > 127!");
        SG->Row = row;
        SG->Col = C_LOWKEY;
        return;
      }

      SG->Cells[C_LOWKEY][row] = KeyToString(iKey);
      SG->Cells[C_HIGHKEY][row] = KeyToString(iKey+12-1);

      iKey += 12; // next octave
    }
  }
}
//---------------------------------------------------------------------------
// set midi channel for all selected keygroups to the midi channel of
// the first selected keygroup
void __fastcall TFormProgram::MenuMidifyKeygroupsClick(TObject *Sender)
{
  int NumKeyGroups = SG->RowCount - PADROWS;

  bool bFirst = true;
  int midiOffset = -1;
  int iSelectCount = CountSelected();

  if (iSelectCount == 0)
  {
    ShowMessage("Nothing selected...\n"
          "Press Ctrl and left-click keygroups to select them.");
    return;
  }

  for (int ii = 0; ii < NumKeyGroups; ii++)
  {
    int row = ii+PADROWS;

    // Drumify if selected... (we use Objects[0] as select flag)
    if ((int)SG->Rows[row]->Objects[C_KEYGROUP] != 0)
    {
      if (bFirst)
      {
        String s = SG->Cells[C_MIDIOFFSET][row];

        // Read the low-key of the 1st selected keygroup as our base key
        // then set the high key the same and iterate, advancing to the next white key
        midiOffset = s.ToIntDef(-1);

        if (midiOffset < 0 || midiOffset > 15)
        {
          ShowMessage("Invalid midi-offset in first selected row: \"" + s + "\"\n"
                      "Must be 0-15!");
          SG->Row = row;
          SG->Col = C_MIDIOFFSET;
          return;
        }

        bFirst = false;
      }

      SG->Cells[C_MIDIOFFSET][row] = String(midiOffset);

      //if (midiOffset < 15)
      //  midiOffset++; // next channel
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormProgram::MenuClearSelectedClick(TObject *Sender)
{
  SelectAllKeygroups(false);
}
//---------------------------------------------------------------------------
void __fastcall TFormProgram::MenuSelectAllClick(TObject *Sender)
{
  SelectAllKeygroups(true);
}
//---------------------------------------------------------------------------
void __fastcall TFormProgram::MenuDeleteSelectedKeygroupsClick(TObject *Sender)
{
  DeleteSelectedKeygroups();
}
//---------------------------------------------------------------------------
void __fastcall TFormProgram::MenuCopyHilightedKeygroupClick(TObject *Sender)
{
  // Put the comma separated keygroup text on the clipboard replacing
  // the Keygroup # with a COPYMAGIC so we can identify it upon paste...
  try
  {
    if (SG->RowCount > PADROWS)
    {
      String s = SG->Rows[SG->Row]->CommaText;

      // replace the row number with a "magic number" for the clipboard
      // so that we can identify it for pasting
      int iCommaPos = s.Pos(",");
      if (iCommaPos > 0)
        Clipboard()->AsText = COPYMAGIC + s.SubString(iCommaPos, s.Length()-iCommaPos+1);
    }
  }
  catch(...){}
}
//---------------------------------------------------------------------------
void __fastcall TFormProgram::MenuPasteHilightedKeygroupClick(TObject *Sender)
{
  // Check the text on the clipboard for a leading string COPYMAGIC.
  // If it's our data - paste it into the current keygroup.
  try
  {
    if (SG->RowCount > PADROWS)
    {
      String s = Clipboard()->AsText;

      int iCommaPos = s.Pos(",");
      if (iCommaPos > 0)
      {
        String sMagic = s.SubString(1, iCommaPos-1);
        if (sMagic == String(COPYMAGIC))
        {
          String sRemaining = s.SubString(iCommaPos, s.Length()-iCommaPos+1);
          SG->Rows[SG->Row]->CommaText = String(SG->Row-PADROWS+1) + sRemaining;
        }
      }
    }
  }
  catch(...){}
}
//---------------------------------------------------------------------------
void __fastcall TFormProgram::ButtonSelectAllClick(TObject *Sender)
{
  if (ButtonSelectAll->Tag == 0)
    SelectAllKeygroups(true);
  else
    SelectAllKeygroups(false);
}
//---------------------------------------------------------------------------
// select/deselect all keygroups
void __fastcall TFormProgram::SelectAllKeygroups(bool bSelect)
{
  int iOldCount = m_selectCount;

  if (bSelect)
  {
    for (int iRow = PADROWS; iRow < SG->RowCount; iRow++)
    {
      if ((int)SG->Rows[iRow]->Objects[0] == 0)
      {
        SG->Rows[iRow]->Strings[0] = String(iRow-PADROWS+1) + S_SELECTED;
        SG->Rows[iRow]->Objects[0] = (TObject*)1; // write selected
      }
    }
    m_selectCount = SG->RowCount;
  }
  else // clear all selections
  {
    for (int iRow = PADROWS; iRow < SG->RowCount; iRow++)
    {
      if ((int)SG->Rows[iRow]->Objects[0] != 0)
      {
        SG->Rows[iRow]->Strings[0] = String(iRow-PADROWS+1);
        SG->Rows[iRow]->Objects[0] = (TObject*)0; // write deselected
      }
    }
    m_selectCount = 0;
  }

  SetButtonText(iOldCount, m_selectCount);
}
//---------------------------------------------------------------------------
void __fastcall TFormProgram::SetButtonText(int iOldCount, int iNewCount)
{
  if (iOldCount && !iNewCount)
  {
    ButtonSelectAll->Caption = "Select All";
    ButtonSelectAll->Tag = 0;
  }
  else if (!iOldCount && iNewCount)
  {
    ButtonSelectAll->Caption = "Clear Selected";
    ButtonSelectAll->Tag = 1;
  }
}
//---------------------------------------------------------------------------
// Count # of selected rows
int __fastcall TFormProgram::CountSelected(void)
{
  int NumKeyGroups = SG->RowCount - PADROWS;

  int iCount = 0;

  for (int ii = 0; ii < NumKeyGroups; ii++)
  {
    int row = ii+PADROWS;

    // Drumify if selected... (we use Objects[0] as select flag)
    if ((int)SG->Rows[row]->Objects[C_KEYGROUP] != 0)
      iCount++;
  }

  SetButtonText(m_selectCount, iCount);

  m_selectCount = iCount;

  return iCount;
}
//---------------------------------------------------------------------------
void __fastcall TFormProgram::SGMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y)
{
  if (Shift.Contains(ssCtrl))
  {
    int iCol, iRow;
    SG->MouseToCell(X, Y, iCol, iRow);
    int iSelected = (int)SG->Rows[iRow]->Objects[0]; // read
    iSelected = (iSelected == 0) ? 1 : 0; // toggle selection

    String s = String(iRow-PADROWS+1);

    int iOldCount = m_selectCount;

    if (iSelected)
    {
      s += S_SELECTED;
      m_selectCount++;
    }
    else
      m_selectCount--;

    SetButtonText(iOldCount, m_selectCount);

    SG->Rows[iRow]->Strings[0] = s;
    SG->Rows[iRow]->Objects[0] = (TObject*)iSelected; // write
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormProgram::MenuHelpClick(TObject *Sender)
{
  String s =
  "Program editor for the Akai S900/S950 digital sampler."
  " Program(s) are saved in a file with a .prg extension and"
  " can be added to or deleted from a .prg file. A program can be"
  " created or edited here.\r\n\r\n"
  "To copy and add a keygroup, click on the keygroup you want to copy"
  " and click the \"+\" button.\r\n\r\n"
  "To delete a keygroup, click on it and click the \"-\" button.\r\n\r\n"
  "You can reposition a keygroup by clicking to its left and dragging it up or down.\r\n\r\n"
  "The key-range field can have a number (-2 to +8) for the octave,"
  " a capital letter (A-G) for the note and a # (plus half-step) or"
  " b (minus half-step). Optionally, you can enter the raw midi key number (0-127).\r\n\r\n"
  "The menu has options to add/read/delete programs in a .prg file.\r\n\r\n"
  "Programs on the target machine appear in the drop-down list."
  " Click on a program name to load it into the editor.\r\n\r\n"
  "When the editor first opens, it will automatically add a keygroup"
  " for each sample on the target machine (if the machine is connected).\r\n\r\n"
  "Press \"Send Program to Machine\" to transmit the current program.\r\n\r\n"
  "Press \"Refresh Programs List\" to refresh the drop-down programs list.\r\n\r\n"
  "To Select/Deselect a keygroup, hold down the Ctrl key and click any cell in the row.\r\n\r\n"
  "\"Drumify\" will auto-set both the midi key and pitch offset for all of your drum samples.\r\n"
  "  1) Select the keygroups you want to drumify (hold Crtl and click the keygroups).\r\n"
  "  2) In the first selected keygroup, set the starting note (3C, -4Db, Etc.) in the \"Low Key\" column.\r\n"
  "  3) In the first selected keygroup, set the starting pitch (usually you want to keep this 0) in the \"Coarse Pitch\" column.\r\n"
  "  4) In the first selected keygroup, set the midi-offset you want all of the drums to be on in the \"Midi Offset\" column.\r\n"
  "  5) Click Tools->Drumify.\r\n"
  "Now you can tap notes on your midi keyboard to play your drum-samples in real-time! (Note: "
  "Presently, the base sample pitch is fixed at 3C (middle C), which is the default for uploaded WAVs).\r\n\r\n"
  "Sample parameters cannot be edited in the program editor, use the sample-parameters editor for that.";
  ShowMessage(s);
}
//---------------------------------------------------------------------------

