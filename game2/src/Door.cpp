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
	iDoorsDown = new TiAnimationSimple(false);
	for (int i = 0; i < IDR_DOORS_FRAME_COUNT; i++)
		iDoorsDown->addFrames(IDR_DOORS_FRAMES + i);
	iDoorsDown->addFrames(IDR_DOORS);
	iDynamicAssets->add(iDoorsDown);

	iDoorsUp = new TiAnimationSimple(false);
	iDoorsUp->addFrames(IDR_DOORS);
	for (int i = IDR_DOORS_FRAME_COUNT; i > 0; i--)
		iDoorsUp->addFrames(IDR_DOORS_FRAMES + i - 1);
	iDynamicAssets->add(iDoorsUp);

	for (int i = 0; i < iDynamicAssets->Count; i++)
	{
		TiAnimationBase* doors = iDynamicAssets->get(i);
		doors->placeTo(aScreenSize.Width / 2, aScreenSize.Height / 2);
		doors->LoopAnimation = false;
		doors->RewindAnimationAfterStop = false;
		doors->OnAnimationFinished = onAnimationDone;
		aManager->add(doors);
	}
}

//---------------------------------------------------------------------------
void __fastcall TiDoor::show(EiMovement aMovement)
{
	switch (aMovement)
	{
		case dmUp: 		iDoorVisible = iDoorsUp; 		break;
		case dmDown: 	iDoorVisible = iDoorsDown; 	break;
	}

	iIsClosed = aMovement == dmDown;;

	iDoorVisible->show();

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

