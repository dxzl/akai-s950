//---------------------------------------------------------------------------
#include <vcl.h>
#include "MainForm.h"
#pragma hdrstop

#include "S9Utils.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
// maps note 0-11 to the note-string
const char* KEYS[] =
{
  "C",
  "C#",
  "D",
  "Eb",
  "E",
  "F",
  "F#",
  "G",
  "G#",
  "A",
  "Bb",
  "B",
};
//---------------------------------------------------------------------------
const int WHITEKEYS[] =
{
  0, // C
  2, // D
  4, // E
  5, // F
  7, // G
  9, // A
  11 // B
};
#define WK_COUNT 7
//---------------------------------------------------------------------------
const int BLACKKEYS[] =
{
  1, // C#
  3, // Eb
  6, // F#
  8, // G#
  10 // Bb
};
#define BK_COUNT 5
//---------------------------------------------------------------------------
bool __fastcall S9Utils::IsWhiteKey(int iKey)
{
  iKey = iKey % 12; // convert from ovtave+note to to a note
  for (int ii = 0; ii < WK_COUNT; ii++)
    if (iKey == WHITEKEYS[ii])
      return true;
  return false;
}
//---------------------------------------------------------------------------
int __fastcall S9Utils::NextWhiteKey(int iKey)
{
  do { iKey++; } while(IsBlackKey(iKey));
  return iKey;
}
//---------------------------------------------------------------------------
bool __fastcall S9Utils::IsBlackKey(int iKey)
{
  iKey = iKey % 12; // convert from ovtave+note to to a note
  for (int ii = 0; ii < BK_COUNT; ii++)
    if (iKey == BLACKKEYS[ii])
      return true;
  return false;
}
//---------------------------------------------------------------------------
int __fastcall S9Utils::NextBlackKey(int iKey)
{
  do { iKey++; } while(IsWhiteKey(iKey));
  return iKey;
}
//---------------------------------------------------------------------------
// Given a key in the range 0-127, returns the -2C-+8G equavalent string
// (11octaves*12notes)-4 = 128 total midi notes
String __fastcall S9Utils::KeyToString(Byte key)
{
  // get the octave
  int octave = key/12 - 2;

  // get the note (index 0-11)
  int keyTableIdx = key%12;

  String sSign;

  if (octave >= 0)
    sSign = "+";

  return String (sSign) + String(octave) + String(KEYS[keyTableIdx]);
}
//---------------------------------------------------------------------------
// Given a string from a grid cell in the form "-2G" or "+3Bb"
// returns a midi key index 0-127, -1 if fail
int __fastcall S9Utils::StringToKey(String s)
{
    try
    {
      s = s.Trim(); // trim spaces

      // first see if the user entered a midi key index (0-127)
      // if so, return that...
      int note = s.ToIntDef(-1);

      if (note >= 0 && note < 128)
          return note;

      // otherwise look for -2C to +8G (can have +5Bb, -2F#, Eb, etc.)

      // at a minimum need a capital note A-G
      char c_note = '\0';
      char c_digit = '\0';
      int NoteCount = 0;
      int FlatCount = 0;
      int SharpCount = 0;
      int PlusCount = 0;
      int MinusCount = 0;
      int DigitCount = 0;
      int OtherCount = 0;

      int len = s.Length();
      for (int ii = 1; ii <= len; ii++)
      {
        if (S9Utils::CharIsNote(s[ii]))
        {
          NoteCount++;
          c_note = s[ii];
        }
        else if (s[ii] == 'b')
          FlatCount++;
        else if (s[ii] == '#')
          SharpCount++;
        else if (s[ii] == '+')
          PlusCount++;
        else if (s[ii] == '-')
          MinusCount++;
        else if (s[ii] >= '0' && s[ii] <= '9')
        {
          DigitCount++;
          c_digit = s[ii];
        }
      else
        OtherCount++;
    }

    // ok, let's see what we have...

    // Must have one note A-G and no meaningless chars
    if (NoteCount != 1 || OtherCount > 0) return -1;

    // can't have more than one sharp, flat, plus, minus, octive-digit
    if (FlatCount > 1 || SharpCount > 1 || DigitCount > 1 ||
      PlusCount > 1 || MinusCount > 1) return -1;

    // can't have both + and -
    if (PlusCount > 0 && MinusCount > 0) return -1;

    // can't have both # and b
    if (SharpCount > 0 && FlatCount > 0) return -1;

    // ok, let's determine the note
    int octave;
    if (c_digit != '\0')
      octave = String(c_digit).ToIntDef(-1);
    else
      octave = 0;

    note = 0;

    // Can't have octave below -2
    if (MinusCount > 0)
    {
      if (octave > 2) return -1;

      // octave -0, -1, -2 = 0, 12, 24
      note += octave * 12; // notes
    }
    else // has to be positive octive
    {
      if (octave > 8) return -1;

      // octave number of 0 will give a note of 24 (+0C)
      note += (octave+2) * 12;
    }

    // now add the note offset (C is 0)
    if (c_note == 'C')
      note += 0;
    else if (c_note == 'D')
      note += 2;
    else if (c_note == 'E')
      note += 4;
    else if (c_note == 'F')
      note += 5;
    else if (c_note == 'G')
      note += 7;
    else if (c_note == 'A')
      note += 9;
    else if (c_note == 'B')
      note += 11;

    // now offset if sharp/flat
    if (SharpCount)
      note++;
    else if (FlatCount)
      note--;

    return note;
    }
    catch(...)
    {
      return -100;
    }
}
//---------------------------------------------------------------------------
bool __fastcall S9Utils::CharIsNote(char c)
{
  return (c >= 'A' && c <= 'G');
}
//---------------------------------------------------------------------------
bool __fastcall S9Utils::NameOk(AnsiString sProgName)
{
  int len = sProgName.Length();
  if (len > MAX_NAME_S900 || len < 1)
    return false;
  for (int ii = 1; ii <= len; ii++)
    if (sProgName[ii] < 32 || sProgName[ii] > 126)
      return false;
  return true;
}
//---------------------------------------------------------------------------
// return true if sName is in the list of programs
bool __fastcall S9Utils::IsNameInList(TComboBox* p, String sName)
{
  for (int ii=0; ii < p->Items->Count; ii++)
      if (p->Items->Strings[ii] == sName)
          return true;
  return false;
}
//---------------------------------------------------------------------------
// returns TRUE if strings match. case-insensitive, no trimming
// (Strings in the S900/S950 are in ASCII)
bool __fastcall S9Utils::StrCmpCaseInsens(char* sA, char* sB, int len)
{
  return AnsiString(sA, len).LowerCase() == AnsiString(sB, len).LowerCase();
}
//---------------------------------------------------------------------------
void __fastcall S9Utils::encodeDB(Byte c, Byte* dest)
{
  *dest++ = (c & (Byte)0x7f);
  *dest = (Byte)((c & (Byte)0x80) ? 1 : 0);
}
//---------------------------------------------------------------------------
void __fastcall S9Utils::encodeDB(Byte* dest, char* source, int numchars)
{
  for (int ii = 0; ii < numchars; ii++)
  {
    *dest++ = (*source & (Byte)0x7f);
    *dest++ = (Byte)((*source & (Byte)0x80) ? 1 : 0);
    source++;
  }
}
//---------------------------------------------------------------------------
void __fastcall S9Utils::encodeDD(uint32_t value, Byte* tp)
{
  for (int ii = 0; ii < 4; ii++)
  {
    *tp++ = (Byte)(value & 0x7f);
    value >>= 7;
    *tp++ = (Byte)(value & 1);
    value >>= 1;
  }
}
//---------------------------------------------------------------------------
void __fastcall S9Utils::encodeDW(uint32_t value, Byte* tp)
{
  for (int ii = 0; ii < 2; ii++)
  {
    *tp++ = (Byte)(value & 0x7f);
    value >>= 7;
    *tp++ = (Byte)(value & 1);
    value >>= 1;
  }
}
//---------------------------------------------------------------------------
void __fastcall S9Utils::encodeTB(uint32_t value, Byte* tp)
{
  *tp++ = (Byte)(value & 0x7f);
  value >>= 7;
  *tp++ = (Byte)(value & 0x7f);
  value >>= 7;
  *tp = (Byte)(value & 0x7f);
}
//---------------------------------------------------------------------------
Byte __fastcall S9Utils::decodeDB(Byte* source)
{
  Byte c = *source++;
  c |= (Byte)(*source << 7);
  return c;
}
//---------------------------------------------------------------------------
// make sure sizeof dest buffer >= numchars+1!
void __fastcall S9Utils::decodeDB(char* dest, Byte* source, int numchars)
{
  for (int ii = 0; ii < numchars; ii++)
  {
    *dest = *source++;
    *dest |= (Byte)(*source++ << 7);
    dest++;
  }

  if (numchars > 1)
    *dest = '\0';
}
//---------------------------------------------------------------------------
// decode a 32-bit value in 8-bytes into an int32_t
uint32_t __fastcall S9Utils::decodeDD(Byte* tp)
{
  uint32_t acc;

  tp += 6;

  acc = 0;
  acc |= *tp | (*(tp + 1) << 7);

  for (int ii = 0; ii < 3; ii++)
  {
    acc <<= 8;
    tp -= 2;
    acc |= *tp | (*(tp + 1) << 7);
  }

  return acc;
}
//---------------------------------------------------------------------------
// decode a 16-bit value in 4-bytes into an int32_t
uint32_t __fastcall S9Utils::decodeDW(Byte* tp)
{
  uint32_t acc;

  tp += 2;

  acc = 0;
  acc |= *tp | (*(tp + 1) << 7);

  acc <<= 8;
  tp -= 2;
  acc |= *tp | (*(tp + 1) << 7);

  return acc;
}
//---------------------------------------------------------------------------
// decode a 21-bit value in 3-bytes into an int32_t
uint32_t __fastcall S9Utils::decodeTB(Byte* tp)
{
  uint32_t acc;

  tp += 2;

  acc = (uint32_t)*tp--;
  acc = (acc << 7) | (uint32_t)*tp--;
  acc = (acc << 7) | (uint32_t)*tp;

  return acc;
}
//---------------------------------------------------------------------------
void __fastcall S9Utils::compute_checksum(int min_index, int max_index, BYTE* buf)
{
  Byte checksum;
  int ii;

  // checksum and store in transmission array
  checksum = 0;

  for (ii = min_index; ii < max_index; ii++)
    checksum ^= buf[ii];

  buf[max_index] = checksum; // offset 126 is checksum
}
//---------------------------------------------------------------------------
// display data obtained via receive()
void __fastcall S9Utils::display_hex(Byte buf[], int count)
{
  String sTemp;
  for (int ii = 0; ii < count; ii++)
  {
    if ((ii % 16) == 0)
    {
      if (!sTemp.IsEmpty())
      {
        printm(sTemp);
        sTemp = "";
      }
    }
    sTemp += IntToHex((int)buf[ii], 2) + " ";
  }
  // print what remains...
  if (!sTemp.IsEmpty())
    printm(sTemp);
}
//---------------------------------------------------------------------------
void __fastcall S9Utils::printm(String message)
{
  FormMain->Memo1->Lines->Add(message);
}
//---------------------------------------------------------------------------
void __fastcall S9Utils::AsciiStrEncode(Byte* dest, char* source)
{
  char locstr[MAX_NAME_S900 + 1];
  strncpy(locstr, source, MAX_NAME_S900);
  locstr[MAX_NAME_S900] = '\0';
  int size = strlen(locstr);
  while (size < MAX_NAME_S900)
    locstr[size++] = ' ';

  encodeDB(dest, locstr, MAX_NAME_S900);
}
//---------------------------------------------------------------------------
void __fastcall S9Utils::AsciiStrDecode(char* dest, Byte* source)
{
  decodeDB(dest, source, MAX_NAME_S900); // 20
  trimright(dest); // trim off the blanks
}
//---------------------------------------------------------------------------
void __fastcall S9Utils::AsciiStrCpyTrimRight(char* dest, char* source)
{
  sprintf(dest, "%.*s", MAX_NAME_S900, source);
  trimright(dest);
}
//---------------------------------------------------------------------------
void __fastcall S9Utils::trimright(Byte* pStr)
{
  int len = 0;
  for (; len < MAX_NAME_S900; len++)
    if (pStr[len] == 0)
      break;

  while (len > 0)
  {
    if (pStr[len - 1] != ' ')
      break;

    len--;
  }

  pStr[len] = 0;
}
//---------------------------------------------------------------------------
// prints a message if the main "busy" flag is set.
// The flag is used at high-level UI menu/button handlers to avoid
// com-conflicts...
bool __fastcall S9Utils::IsBusy(void)
{
  if (FormMain->BusyCount < 0)
  {
    printm("repairing busy-counter - please tell dxzl@live.com! (" + String(FormMain->BusyCount) + ")");
    FormMain->BusyCount = 0; // repair
    return false;
  }

  if (FormMain->BusyCount > 0)
  {
    if (FormMain->BusyCount > 1)
    {
      printm("repairing busy-counter - please tell dxzl@live.com! (" + String(FormMain->BusyCount) + ")");
      FormMain->BusyCount = 1; // repair
    }

    ShowMessage("Communications busy. Try Again...");
    return true;
  }

  return false;
}
//---------------------------------------------------------------------------
void __fastcall S9Utils::print_ps_info(PSTOR* ps, bool bPrintUnused)
{
  printm("");
  printm("Output Parameters:");
  printm("sample length (in words): " + String(ps->sampleCount));
  printm("end point: " + String(ps->endpoint));
  printm("frequency (hertz): " + String(ps->freq));
  printm("pitch: " + String(ps->pitch));
  printm("period (nanoseconds): " + String((uint32_t)ps->period));
  printm("bits per word: " + String(ps->bits_per_word));
  printm("start point: " + String(ps->startpoint));
  printm("loop length: " + String(ps->looplen));

  if (ps->flags & (Byte)1)
    printm("velocity crossfade: on");
  else
    printm("velocity crossfade: off");

  if (ps->flags & (Byte)2)
    printm("reverse waveform: yes");
  else
    printm("reverse waveform: no");

  if (ps->loopmode == 'O')
    printm("looping mode: one-shot");
  else if (ps->loopmode == 'L')
    printm("looping mode: looping");
  else if (ps->loopmode == 'A')
    printm("looping mode: alternating");
  else
    printm("looping mode: unknown");

  printm("sample name: \"" + String(ps->name) + "\""); // show any spaces

  if (bPrintUnused){
    printm("");
    printm("Undefined Parameters (saved in .aki file):");
    printm("undefinedDD_27=" + String(ps->undefinedDD_27));
    printm("undefinedDW_35=" + String(ps->undefinedDW_35));
    printm("undefinedDD_95=" + String(ps->undefinedDD_95));
    printm("undefinedDD_103=" + String(ps->undefinedDD_103));
    printm("undefinedDD_111=" + String(ps->undefinedDD_111));
    printm("undefinedDD_119=" + String(ps->undefinedDD_119));
    printm("");
    printm("Reserved Parameters (saved in .aki file):");
    printm("reservedDB_61=" + String(ps->reservedDB_61));
    printm("reservedDW_87=" + String(ps->reservedDW_87));
  }
}
//---------------------------------------------------------------------------

