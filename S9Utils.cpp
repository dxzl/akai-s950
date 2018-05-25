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
void __fastcall S9Utils::encodeDD(UInt32 value, Byte* tp)
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
void __fastcall S9Utils::encodeDW(UInt32 value, Byte* tp)
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
void __fastcall S9Utils::encodeTB(UInt32 value, Byte* tp)
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
// decode a 32-bit value in 8-bytes into an __int32
UInt32 __fastcall S9Utils::decodeDD(Byte* tp)
{
	UInt32 acc;

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
// decode a 16-bit value in 4-bytes into an __int32
UInt32 __fastcall S9Utils::decodeDW(Byte* tp)
{
	UInt32 acc;

	tp += 2;

	acc = 0;
	acc |= *tp | (*(tp + 1) << 7);

	acc <<= 8;
	tp -= 2;
	acc |= *tp | (*(tp + 1) << 7);

	return acc;
}
//---------------------------------------------------------------------------
// decode a 21-bit value in 3-bytes into an __int32
UInt32 __fastcall S9Utils::decodeTB(Byte* tp)
{
	UInt32 acc;

	tp += 2;

	acc = (UInt32)*tp--;
	acc = (acc << 7) | (UInt32)*tp--;
	acc = (acc << 7) | (UInt32)*tp;

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

