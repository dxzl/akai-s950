//---------------------------------------------------------------------------
#ifndef RegHelperH
#define RegHelperH

#include <Windows.h>
#include <Registry.hpp>
#include <Classes.hpp>
//---------------------------------------------------------------------------
class TRegHelper
{
private:
  TRegistry* Reg;

  bool bOpened;

protected:
public:
  __fastcall TRegHelper(bool bCreate);
  virtual __fastcall ~TRegHelper();

  void __fastcall ReadSetting(String keyName, bool &bVal, bool bDef);
  void __fastcall ReadSetting(String keyName, int &iVal, int iDef);
  String __fastcall ReadSetting(String keyName);
  void __fastcall WriteSetting(String keyName, bool bVal);
  void __fastcall WriteSetting(String keyName, int iVal);
  void __fastcall WriteSetting(String keyName, String sIn);
};
//---------------------------------------------------------------------------
#endif
