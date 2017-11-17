//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

//---------------------------------------------------------------------------
USERES("FireflyAndPointsTest.res");
USEFORM("MainForm.cpp", frmMainForm);
USELIB("GdiPlus.lib");
USELIB("..\..\..\XML\XML.lib");
USERC("..\assets\assets_ffap.rc");
USEFORM("..\src\FireflyAndPoints.cpp", iFireflyAndPoints);
USEUNIT("..\src\EyeBoxWithInstructions.cpp");
USEUNIT("..\src\Lamp.cpp");
USEUNIT("..\src\Lamps.cpp");
USEUNIT("..\src\OlioHunting.cpp");
USEUNIT("..\..\common\Animation.cpp");
USEUNIT("..\..\common\CalibQuality.cpp");
USEUNIT("..\..\common\CalibQualityEstimator.cpp");
USEUNIT("..\..\common\utils.cpp");

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
