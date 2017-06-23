//---------------------------------------------------------------------------
#ifndef RegHelperH
#define RegHelperH

#include <Vcl.h>
#include <Registry.hpp>
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
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
