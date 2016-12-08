//---------------------------------------------------------------------------
#pragma package(smart_init)
#include "MainForm.h"
#pragma hdrstop

#include "RegHelper.h"

//---------------------------------------------------------------------------
//  constructor
__fastcall TRegHelper::TRegHelper(bool bCreate)
{
  Reg = new TRegistry();

  if (Reg == NULL)
  {
#if DEBUG_ON
    MainForm->CWrite( "\r\nUnable to create TRegistry()!\r\n");
#endif
    bOpened = false;
    return;
  }

  if (!Reg->OpenKey(REGISTRY_KEY, bCreate))
  {
#if DEBUG_ON
    MainForm->CWrite( "\r\nUnable to open Registry()!\r\n");
#endif
    bOpened = false;
    return;
  }

  bOpened = true;
}
//------------------- destructor -------------------------
__fastcall TRegHelper::~TRegHelper()
{
  try
  {
    if (Reg != NULL)
    {
      if (bOpened)
        Reg->CloseKey();

      delete Reg;
    }
  }
  catch(...) {}
}
//---------------------------------------------------------------------------
String __fastcall TRegHelper::ReadSetting(String keyName)
{
  String sOut;

  // Read key from registry at HKEY_CURRENT_USER
  if (!bOpened)
    return "";

  try
  {
    sOut = Reg->ReadString(keyName);
  }
  catch (...)
  {
    sOut = "";
  }

  return sOut;
}
//---------------------------------------------------------------------------
void __fastcall TRegHelper::ReadSetting(String keyName, bool &bVal, bool bDef)
{
  // Read key from registry at HKEY_CURRENT_USER
  if (!bOpened)
    return;

  try
  {
    bVal = Reg->ReadBool(keyName);
  }
  catch(...)
  {
    bVal = bDef;
  }
}
//---------------------------------------------------------------------------
void __fastcall TRegHelper::ReadSetting(String keyName, int &iVal, int iDef)
{
  // Read key from registry at HKEY_CURRENT_USER
  if (!bOpened)
    return;

  try
  {
    iVal = Reg->ReadInteger(keyName);
  }
  catch(...)
  {
    iVal = iDef;
  }
}
//---------------------------------------------------------------------------
void __fastcall TRegHelper::WriteSetting(String keyName, String sIn)
{
  // Read key from registry at HKEY_CURRENT_USER
  if (!bOpened)
    return;

  try
  {
    Reg->WriteString(keyName, sIn);
  }
  catch(...) {}
}
//---------------------------------------------------------------------------
void __fastcall TRegHelper::WriteSetting(String keyName, bool bVal)
{
  // Read key from registry at HKEY_CURRENT_USER
  if (!bOpened)
    return;

  try
  {
    Reg->WriteBool(keyName, bVal);
  }
  catch(...) {}
}
//---------------------------------------------------------------------------
void __fastcall TRegHelper::WriteSetting(String keyName, int iVal)
{
  // Read key from registry at HKEY_CURRENT_USER
  if (!bOpened)
    return;

  try
  {
    Reg->WriteInteger(keyName, iVal);
  }
  catch(...) {}
}
//---------------------------------------------------------------------------

