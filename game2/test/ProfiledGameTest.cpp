//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

//---------------------------------------------------------------------------
USERES("ProfiledGameTest.res");
USEFORM("MainForm.cpp", frmMainForm);
USELIB("GdiPlus.lib");
USELIB("..\..\..\XML\XML.lib");
USEUNIT("..\..\common\AnimationBase.cpp");
USEUNIT("..\..\common\AnimationManager.cpp");
USEUNIT("..\..\common\AnimationSimple.cpp");
USEUNIT("..\..\common\Animation.cpp");
USEUNIT("..\..\common\CalibQuality.cpp");
USEUNIT("..\..\common\CalibQualityEstimator.cpp");
USEUNIT("..\..\common\utils.cpp");
USERC("..\assets\assets_pg.rc");
USEFORM("..\src\ProfiledGame.cpp", iProfiledGame);
USEUNIT("..\src\EyeBox.cpp");
USEUNIT("..\src\CalibPoint.cpp");
USEUNIT("..\src\CalibPoints.cpp");
USEUNIT("..\src\Profile.cpp");
USEUNIT("..\src\ProfileEditor.cpp");
USEUNIT("..\src\Avatar.cpp");
USEUNIT("..\src\InstructionSitting.cpp");
USEUNIT("..\src\Scene.cpp");
USEUNIT("..\src\InstructionStart.cpp");
USEUNIT("..\src\Factory.cpp");
USEUNIT("..\src\InstructionColor.cpp");
USEUNIT("..\src\Door.cpp");
USEUNIT("..\src\InstructionCalibrate.cpp");
USEUNIT("..\src\Login.cpp");
USEUNIT("..\src\GameResult.cpp");
USEUNIT("..\src\Rewards.cpp");
USEUNIT("..\src\Status.cpp");
USEUNIT("..\src\InstructionShape.cpp");
USEUNIT("..\src\LevelInstruction.cpp");
USEUNIT("..\src\LevelLegend.cpp");
USEUNIT("..\src\GhostBox.cpp");
USEUNIT("..\src\PositionTracker.cpp");
USEUNIT("..\src\UserPositionQualityEstimator.cpp");

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
