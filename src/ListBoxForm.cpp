//---------------------------------------------------------------------------
#include <vcl.h>
#include "MainForm.h"
#pragma hdrstop

#include "ListBoxForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormListBox *FormListBox;
//---------------------------------------------------------------------------
__fastcall TFormListBox::TFormListBox(TComponent* Owner)
  : TForm(Owner)
{
  m_selIndex = -1;
  m_itemOffset = -1;
}
//---------------------------------------------------------------------------
void __fastcall TFormListBox::ListBox1Click(TObject *Sender)
{
  m_selIndex = ListBox1->ItemIndex;
  if (m_selIndex >= 0)
    m_itemOffset = (int)ListBox1->Items->Objects[m_selIndex];
  Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormListBox::FormClose(TObject *Sender, TCloseAction &Action)
{
  if (m_selIndex >= 0)
    ModalResult = mrOk;
  else
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TFormListBox::FormShow(TObject *Sender)
{
  this->Caption = sTopic;
}
//---------------------------------------------------------------------------

