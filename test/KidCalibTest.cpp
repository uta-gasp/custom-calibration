//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

//---------------------------------------------------------------------------
USERES("KidCalibTest.res");
USEFORM("MainForm.cpp", frmMainForm);
USELIB("GdiPlus.lib");
USELIB("..\..\XML\XML.lib");
USEUNIT("..\common\Animation.cpp");
USEUNIT("..\common\CalibQuality.cpp");
USEUNIT("..\common\CalibQualityEstimator.cpp");
USEUNIT("..\common\utils.cpp");
USERC("..\game1\assets\assets_ffap.rc");
USEFORM("..\game1\src\FireflyAndPoints.cpp", iFireflyAndPoints);
USEUNIT("..\game1\src\EyeBoxWithInstructions.cpp");
USEUNIT("..\game1\src\Lamp.cpp");
USEUNIT("..\game1\src\Lamps.cpp");
USEUNIT("..\game1\src\OlioHunting.cpp");
USERC("..\game2\assets\assets_pg.rc");
USEFORM("..\game2\src\ProfiledGame.cpp", iProfiledGame);
USEUNIT("..\game2\src\EyeBox.cpp");
USEUNIT("..\game2\src\CalibPoint.cpp");
USEUNIT("..\game2\src\CalibPoints.cpp");
USEUNIT("..\game2\src\Profile.cpp");
USEUNIT("..\game2\src\ProfileEditor.cpp");
USEUNIT("..\game2\src\Avatar.cpp");
USEUNIT("..\game2\src\InstructionSitting.cpp");
USEUNIT("..\game2\src\Scene.cpp");
USEUNIT("..\game2\src\InstructionStart.cpp");
USEUNIT("..\game2\src\Factory.cpp");
USEUNIT("..\game2\src\InstructionColor.cpp");
USEUNIT("..\game2\src\Door.cpp");
USEUNIT("..\game2\src\InstructionCalibrate.cpp");
USEUNIT("..\game2\src\Login.cpp");
USEUNIT("..\game2\src\GameResult.cpp");
USEUNIT("..\game2\src\Rewards.cpp");
USEUNIT("..\game2\src\Status.cpp");
USEUNIT("..\game2\src\InstructionShape.cpp");
USEUNIT("..\game2\src\LevelInstruction.cpp");
USEUNIT("..\game2\src\LevelLegend.cpp");
USEUNIT("..\game2\src\GhostBox.cpp");
USEUNIT("Controller.cpp");
USEUNIT("..\game2\src\PositionTracker.cpp");
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
