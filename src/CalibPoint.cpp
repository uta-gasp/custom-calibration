//---------------------------------------------------------------------------
#include "CalibPoint.h"
#include "assets.h"
#include "utils.h"

//---------------------------------------------------------------------------
#include <math.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
const int KWormCountMin = 1;
const int KWormCountMax = 3;
const int KWormDistanceMin = 50;
const int KWormDistanceMax = 200;
const int KWormAnimSpeedMin = 15;
const int KWormAnimSpeedMax = 25;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall TiCalibPoint::TiWorm::TiWorm(int aLampX, int aLampY) :
		TiAnimation(),
		iLampX(aLampX),
		iLampY(aLampY)
{
	addFrames(IDR_WORM, 32);
	LoopAnimation = true;
	AnimationSpeed = randInRange(KWormAnimSpeedMin, KWormAnimSpeedMax);

	iDistanceToLamp = randInRange(KWormDistanceMin, KWormDistanceMax);

	iOriginalAngle = random(360);
	double dx = iDistanceToLamp * cos(iOriginalAngle * M_PI / 180.0);
	double dy = iDistanceToLamp * sin(iOriginalAngle * M_PI / 180.0);

	placeTo(iLampX + dx, iLampY + dy);
	Angle = iOriginalAngle + 90;
	startAnimation();
}

//---------------------------------------------------------------------------
void __fastcall TiCalibPoint::TiWorm::moveAway()
{
	int dx = X - iLampX;
	int dy = Y - iLampY;

	moveTo(
			X + KWormDistanceMax * dx / iDistanceToLamp,
			Y + KWormDistanceMax * dy / iDistanceToLamp
	);
}

//---------------------------------------------------------------------------
void __fastcall TiCalibPoint::TiWorm::moveClose()
{
	moveTo(
			iLampX + iDistanceToLamp * cos(iOriginalAngle * M_PI / 180.0),
			iLampY + iDistanceToLamp * sin(iOriginalAngle * M_PI / 180.0)
	);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall TiCalibPoint::TiCalibPoint(TiAnimationManager* aManager, int aX, int aY) :
		iX(aX),
		iY(aY),
		iEnabled(true)
{
	iWorms.DeleteContent = false;

	iLamp = new TiAnimation();
	iLamp->addFrames(IDR_LAMP, 128);
	iLamp->LoopAnimation = false;
	iLamp->RewindAnimationAfterStop = false;
	iLamp->AllowAnimationReversion = true;
	iLamp->placeTo(iX, iY);

	int wormCount = randInRange(KWormCountMin, KWormCountMax);
	for (int i = 0; i < wormCount; i++)
	{
		TiWorm* worm = new TiWorm(iX, iY);
		worm->OnFadingFinished = onWormFadingFinished;

		iWorms.add(worm);
		aManager->add(worm);
	}

	aManager->add(iLamp);
}

//---------------------------------------------------------------------------
void __fastcall TiCalibPoint::lightOn()
{
	iLamp->startAnimation();
	for (int i = 0; i < iWorms.Count; i++)
		iWorms[i]->moveAway();

	TiTimeout::run(500, onWormsHideRequest);
}

//---------------------------------------------------------------------------
void __fastcall TiCalibPoint::lightOff()
{
	if (iLamp->FrameIndex == 0)
		return;

	iLamp->startAnimation();

	for (int i = 0; i < iWorms.Count; i++)
	{
		TiWorm* worm = iWorms[i];
		worm->moveClose();
		worm->fadeIn();
	}
}

//---------------------------------------------------------------------------
void __fastcall TiCalibPoint::paintTo(Gdiplus::Graphics* aGraphics)
{
	iLamp->paintTo(aGraphics);
	for (int i = 0; i < iWorms.Count; i++)
		iWorms[i]->paintTo(aGraphics);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TiCalibPoint::onWormsHideRequest(TObject* aSender)
{
	for (int i = 0; i < iWorms.Count; i++)
		iWorms[i]->fadeOut();
}

//---------------------------------------------------------------------------
void __fastcall TiCalibPoint::onWormFadingFinished(TObject* aSender)
{
	TiAnimation* worm = (TiAnimation*)aSender;
	worm->toggleAnimation();
}



