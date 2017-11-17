//---------------------------------------------------------------------------
#include "InstructionCalibrate.h"
#include "assets_pg.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
using namespace ProfiledGame;

//---------------------------------------------------------------------------
__fastcall TiInstructionCalibrate::TiInstructionCalibrate(TiAnimationManager* aManager,
		TiSize aScreenSize, TiSize aViewport) :
		TiScene(aManager, aScreenSize, aViewport)
{
	TiAnimation* background = new TiAnimation(false, false);
	background->addFrames(IDR_INSTRUCTIONS_CALIBRATE, aViewport.Width, aViewport.Height);
	background->placeTo(aScreenSize.Width / 2, aScreenSize.Height / 2);
	aManager->add(background);

	iDynamicAssets->add(background);
}

