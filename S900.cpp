//---------------------------------------------------------------------------
//Copyright 2008 Scott Swift - This program is distributed under the
//terms of the GNU General Public License.
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
USERES("S900.res");
USEFORM("MainForm.cpp", FormS900);
USEUNIT("RegHelper.cpp");
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    try
    {
        Application->Initialize();
        Application->CreateForm(__classid(TFormS900), &FormS900);
     Application->Run();
    }
    catch (Exception &exception)
    {
        Application->ShowException(&exception);
    }

    return 0;
}
//---------------------------------------------------------------------------
