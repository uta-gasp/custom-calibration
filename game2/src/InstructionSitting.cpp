//---------------------------------------------------------------------------
#include "InstructionSitting.h"
#include "assets_pg.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
using namespace ProfiledGame;

//---------------------------------------------------------------------------
__fastcall TiInstructionSitting::TiInstructionSitting(TiAnimationManager* aManager,
		TiSize aScreenSize, TiSize aViewport) :
		TiScene(aManager, aScreenSize, aViewport)
{
	TiAnimation* background = new TiAnimation(false);
	background->addFrames(IDR_INSTRUCTIONS_SITTING, aViewport.Width, aViewport.Height);
	background->placeTo(aScreenSize.Width / 2, aScreenSize.Height / 2);
	aManager->add(background);

	iStaticAssets->add(background);

	iButtonContinue = TiRect(Offset.x + 595, Offset.y + 517, 200, 60);
}

//---------------------------------------------------------------------------
void __fastcall TiInstructionSitting::mouseClick(TMouseButton aButton, TShiftState aShift, int aX, int aY)
{
	if (iButtonContinue.hits(aX, aY))
	{
		if (FOnDone)
			FOnDone(this);
	}
}
