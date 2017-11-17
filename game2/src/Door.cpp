//---------------------------------------------------------------------------
#include "Door.h"
#include "assets_pg.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
using namespace ProfiledGame;

//---------------------------------------------------------------------------
__fastcall TiDoor::TiDoor(TiAnimationManager* aManager,
		TiSize aScreenSize, TiSize aViewport) :
		TiScene(aManager, aScreenSize, aViewport),
		iDoorVisible(NULL)
{
	iDoorsClose = new TiAnimation(false, false);
	iDoorsClose->addFrames(IDR_DOORS_CLOSE, aViewport.Width, aViewport.Height);
	iDynamicAssets->add(iDoorsClose);

	iDoorsOpen = new TiAnimation(false, false);
	iDoorsOpen->addFrames(IDR_DOORS_OPEN, aViewport.Width, aViewport.Height);
	iDynamicAssets->add(iDoorsOpen);

	iDoorsDown = new TiAnimation(false, false);
	iDoorsDown->addFrames(IDR_DOORS_DOWN, aViewport.Width, aViewport.Height);
	iDynamicAssets->add(iDoorsDown);

	iDoorsUp = new TiAnimation(false, false);
	iDoorsUp->addFrames(IDR_DOORS_UP, aViewport.Width, aViewport.Height);
	iDynamicAssets->add(iDoorsUp);

	for (int i = 0; i < iDynamicAssets->Count; i++)
	{
		TiAnimation* doors = iDynamicAssets->get(i);
		doors->placeTo(aScreenSize.Width / 2, aScreenSize.Height / 2);
		doors->LoopAnimation = false;
		doors->RewindAnimationAfterStop = false;
		doors->FadingStepCount = 5;
		doors->OnAnimationFinished = onAnimationDone;
		aManager->add(doors);
	}
}

//---------------------------------------------------------------------------
void __fastcall TiDoor::show(EiMovement aMovement, int aDuration)
{
	switch (aMovement)
	{
		case dmUp: 		iDoorVisible = iDoorsUp; 		break;
		case dmDown: 	iDoorVisible = iDoorsDown; 	break;
		case dmOpen: 	iDoorVisible = iDoorsOpen; 	break;
		case dmClose: iDoorVisible = iDoorsClose; break;
	}

	iIsClosed = aMovement == dmDown || aMovement == dmClose;

	if (!aDuration)
	{
		iDoorVisible->show();
	}
	else
	{
		iDoorVisible->FadingDuration = aDuration;
		iDoorVisible->fadeIn();
	}

	iIsVisible = true;
}

//---------------------------------------------------------------------------
void __fastcall TiDoor::run()
{
	if (iDoorVisible)
		iDoorVisible->startAnimation();
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TiDoor::onAnimationDone(TObject* aSender)
{
	if (FOnDone)
		FOnDone(this);
}

