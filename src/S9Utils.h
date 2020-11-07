//---------------------------------------------------------------------------
#ifndef S9UtilsH
#define S9UtilsH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
//---------------------------------------------------------------------------
namespace S9Utils
{
  bool __fastcall IsBusy(void);
  bool __fastcall IsBlackKey(int iKey);
  bool __fastcall IsWhiteKey(int iKey);
  int __fastcall NextWhiteKey(int iKey);
  int __fastcall NextBlackKey(int iKey);
  String __fastcall KeyToString(Byte key);
  int __fastcall StringToKey(String s);
  bool __fastcall CharIsNote(char c);
  bool __fastcall NameOk(AnsiString sProgName);
  void __fastcall trimright(Byte* pStr);
  bool __fastcall StrCmpCaseInsens(char* sA, char* sB, int len);
  bool __fastcall IsNameInList(TComboBox* p, String sName);

  // used by ProgramForm.cpp, and MainForm.cpp and EditSampParmsForm.cpp
  void __fastcall encodeDB(Byte c, Byte* dest);
  void __fastcall encodeDB(Byte* dest, char* source, int numchars);
  Byte __fastcall decodeDB(Byte* source);
  void __fastcall decodeDB(char* dest, Byte* source, int numchars);

  void __fastcall encodeDD(UInt32 value, Byte* tp);
  UInt32 __fastcall decodeDD(Byte* tp);

  void __fastcall encodeDW(UInt32 value, Byte* tp);
  UInt32 __fastcall decodeDW(Byte* tp);

  void __fastcall encodeTB(UInt32 value, Byte* tp);
  UInt32 __fastcall decodeTB(Byte* tp);

  void __fastcall compute_checksum(int min_index, int max_index, BYTE* buf);
  void __fastcall display_hex(Byte buf[], int count);
  void __fastcall printm(String message);

  void __fastcall AsciiStrEncode(Byte* dest, char* source);
  void __fastcall AsciiStrDecode(char* dest, Byte* source);
  void __fastcall AsciiStrCpyTrimRight(char* dest, char* source);
}
//---------------------------------------------------------------------------
extern const char* KEYS[];
#endif
