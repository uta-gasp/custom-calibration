//---------------------------------------------------------------------------
#include "CalibPoint.h"
#include "assets_pg.h"
#include "utils.h"

//---------------------------------------------------------------------------
#include <math.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
using namespace ProfiledGame;

//---------------------------------------------------------------------------
const TPoint KButtonSize[] = {
	TPoint(45, 45),
	TPoint(49, 46)
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall TiCalibPoint::TiCalibPoint(TiAnimationManager* aManager, int aX, int aY, int aLevel) :
		iX(aX),
		iY(aY),
		iEnabled(true),
		iAcceptanceCount(0)
{
	int levelOffset = IDR_CALIB_LEVEL + (1 << 8) * (aLevel + 1);
	TPoint buttonSize = KButtonSize[aLevel];

	iCalibTarget = new TiAnimation(false, false);
	iCalibTarget->addFrames(IDR_CALIB_ITEM_OFF + levelOffset, buttonSize.x, buttonSize.y);
	iCalibTarget->addFrames(IDR_CALIB_ITEMS + levelOffset, buttonSize.x, buttonSize.y);
	iCalibTarget->AnimationSpeed = 1.5;
	iCalibTarget->LoopAnimation = true;
	iCalibTarget->RewindAnimationAfterStop = false;
	iCalibTarget->AllowAnimationReversion = false;
	iCalibTarget->placeTo(iX, iY);

	if (aManager)
		aManager->add(iCalibTarget);
}

//---------------------------------------------------------------------------
void __fastcall TiCalibPoint::show()
{
	iCalibTarget->AnimationIndex = 0;
	iCalibTarget->fadeIn();
}

//---------------------------------------------------------------------------
void __fastcall TiCalibPoint::hide()
{
	iCalibTarget->fadeOut();
}

//---------------------------------------------------------------------------
void __fastcall TiCalibPoint::start()
{
	iCalibTarget->AnimationIndex = 1;
	iCalibTarget->setFrame();
	iCalibTarget->startAnimation();
}

//---------------------------------------------------------------------------
void __fastcall TiCalibPoint::stop(bool aIsAccepted)
{
	if (aIsAccepted)
		iAcceptanceCount++;
		
	iCalibTarget->stopAnimation();
}

//---------------------------------------------------------------------------
void __fastcall TiCalibPoint::paintTo(Gdiplus::Graphics* aGraphics, EiUpdateType aUpdateType)
{
	if (aUpdateType & updNonStatic)
		iCalibTarget->paintTo(aGraphics);
}

