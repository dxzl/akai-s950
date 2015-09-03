//---------------------------------------------------------------------------
//Copyright 2008 Scott Swift - This program is distributed under the
//terms of the GNU General Public License.
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
USERES("S900.res");
USEFORM("MainForm.cpp", Form1);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
        try
        {
                 Application->Initialize();
                 Application->CreateForm(__classid(TForm1), &Form1);
     Application->Run();
        }
        catch (Exception &exception)
        {
                 Application->ShowException(&exception);
        }
        return 0;
}
//---------------------------------------------------------------------------
