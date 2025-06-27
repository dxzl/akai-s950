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
  String __fastcall KeyToString(uint8_t key);
  int __fastcall StringToKey(String s);
  bool __fastcall CharIsNote(char c);
  bool __fastcall NameOk(AnsiString sProgName);
  void __fastcall trimright(uint8_t* pStr);
  bool __fastcall StrCmpCaseInsens(char* sA, char* sB, int len);
  bool __fastcall IsNameInList(TComboBox* p, String sName);

  // used by ProgramForm.cpp, and MainForm.cpp and EditSampParmsForm.cpp
  void __fastcall encodeDB(uint8_t c, uint8_t* dest);
  void __fastcall encodeDB(uint8_t* dest, char* source, int numchars);
  uint8_t __fastcall decodeDB(uint8_t* source);
  void __fastcall decodeDB(char* dest, uint8_t* source, int numchars);

  void __fastcall encodeDD(uint32_t value, uint8_t* tp);
  uint32_t __fastcall decodeDD(uint8_t* tp);

  void __fastcall encodeDW(uint32_t value, uint8_t* tp);
  uint32_t __fastcall decodeDW(uint8_t* tp);

  void __fastcall encodeTB(uint32_t value, uint8_t* tp);
  uint32_t __fastcall decodeTB(uint8_t* tp);

  void __fastcall compute_checksum(int min_index, int max_index, BYTE* buf);
  void __fastcall print_ps_info(PSTOR* ps, bool bPrintUnused= false);
  void __fastcall display_hex(uint8_t buf[], int count);
  void __fastcall printm(String message);

  void __fastcall AsciiStrEncode(uint8_t* dest, char* source);
  void __fastcall AsciiStrDecode(char* dest, uint8_t* source);
  void __fastcall AsciiStrCpyTrimRight(char* dest, char* source);
}
//---------------------------------------------------------------------------
extern const char* KEYS[];
#endif
