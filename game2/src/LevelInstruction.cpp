//---------------------------------------------------------------------------
#include "LevelInstruction.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
using namespace ProfiledGame;

//---------------------------------------------------------------------------
__fastcall TiLevelInstruction::TiLevelInstruction(TiAnimationManager* aManager,
		TiSize aScreenSize, TiSize aViewport) :
		TiScene(aManager, aScreenSize, aViewport),
		iLevelRcID(0)		// must be set in descendant ctor
{
	iButtonContinue = TiRect(Offset.x + 583, Offset.y + 575, 200, 60);
}

//---------------------------------------------------------------------------
void __fastcall TiLevelInstruction::show(int aTargetID)
{
	TiAnimation* background = new TiAnimation(false, false);
	background->addFrames(iLevelRcID + aTargetID, iViewport.Width, iViewport.Height);
	background->placeTo(iScreenSize.Width / 2, iScreenSize.Height / 2);
	iManager->add(background);
	iDynamicAssets->add(background);

	TiScene::show();
}

//---------------------------------------------------------------------------
void __fastcall TiLevelInstruction::mouseClick(TMouseButton aButton, TShiftState aShift, int aX, int aY)
{
	if (iButtonContinue.hits(aX, aY))
	{
		if (FOnDone)
			FOnDone(this);
	}
}
