//---------------------------------------------------------------------------
#ifndef ListBoxFormH
#define ListBoxFormH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
//---------------------------------------------------------------------------
class TFormListBox : public TForm
{
__published:	// IDE-managed Components
	TListBox *ListBox1;
	void __fastcall ListBox1Click(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormShow(TObject *Sender);
private:	// User declarations
	int m_selIndex, m_itemOffset;
	String sTopic;
public:		// User declarations
	__fastcall TFormListBox(TComponent* Owner);

	__property int SelIndex = {read = m_selIndex};
	__property int ItemOffset = {read = m_itemOffset};

	__property String Topic = {read = sTopic, write = sTopic };
};
//---------------------------------------------------------------------------
extern PACKAGE TFormListBox *FormListBox;
//---------------------------------------------------------------------------
#endif
