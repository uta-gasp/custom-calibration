//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

USERES("KidCalibTest.res");
USEFORM("MainForm.cpp", frmMainForm);
USERC("..\assets\assets.rc");
USEUNIT("..\src\Animation.cpp");
USELIB("GdiPlus.lib");
USEUNIT("..\src\utils.cpp");
USEFORM("..\src\CustomCalibration.cpp", frmCustomCalibration);
USEUNIT("..\src\EyeBox.cpp");
USEUNIT("..\src\CalibPlot.cpp");
USEUNIT("..\src\CalibPoint.cpp");
USEUNIT("..\src\CalibPoints.cpp");
USEUNIT("..\src\Game.cpp");
USELIB("..\..\XML\XML.lib");
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	try
	{
		Application->Initialize();
		Application->CreateForm(__classid(TfrmMainForm), &frmMainForm);
		Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	return 0;
}

//---------------------------------------------------------------------------
