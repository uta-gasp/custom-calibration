//---------------------------------------------------------------------------
#include "Lamp.h"
#include "assets_ffap.h"
#include "utils.h"

//---------------------------------------------------------------------------
#include <math.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
using namespace FireflyAndPoints;

//---------------------------------------------------------------------------
const int KOlioCountMin = 0;
const int KOlioCountMax = 0;
const int KOlioDistanceMin = 120;
const int KOlioDistanceMax = 150;
const int KOlioIdleSpeedMin = 6;
const int KOlioIdleSpeedMax = 10;
const int KOlioFadeSpeed = 12;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
double __fastcall TiLamp::TiOlio::getDirectionFromLamp(TiOlios* aOthers, int aMaxCount)
{
	double maxAngleBetween = 360.0 / aMaxCount / 2;

	double result;
	bool isTooCloseToOthers = true;

	while (isTooCloseToOthers) {
		result = random(360);
		isTooCloseToOthers = false;
		for (int i = 0; i < aOthers->Count; i++) {
			TiOlio* other = aOthers->get(i);
			double dist = fabs( other->Direction - result );
			if (dist > 180) {
				dist = 360 - dist;
			}
			isTooCloseToOthers = isTooCloseToOthers || dist < maxAngleBetween;
		}
	}

	return result;
}

//---------------------------------------------------------------------------
__fastcall TiLamp::TiOlio::TiOlio(int aLampX, int aLampY, double aDirectionFromLamp) :
		TiAnimation(true, false),
		iLampX(aLampX),
		iLampY(aLampY)
{
	addFrames(IDR_OLIO_IDLE, 90, 143);
	addFrames(IDR_OLIO_FADEIN, 90, 143);
	addFrames(IDR_OLIO_FADEOUT, 90, 143);

	LoopAnimation = true;
	AnimationSpeed = Utils::randInRange(KOlioIdleSpeedMin, KOlioIdleSpeedMax);

	iDistanceToLamp = Utils::randInRange(KOlioDistanceMin, KOlioDistanceMax);

	iDirection = aDirectionFromLamp < 0 ? random(360) : aDirectionFromLamp;

	double dx = iDistanceToLamp * cos(iDirection * M_PI / 180.0);
	double dy = iDistanceToLamp * sin(iDirection * M_PI / 180.0);

	placeTo(iLampX + dx, iLampY + dy);

	OnAnimationFinished = onAnimationFinished;

	hide();
}

//---------------------------------------------------------------------------
void __fastcall TiLamp::TiOlio::idle()
{
	if (AnimationIndex == 0)
		return;

	show();
	StartIdle(false);
}

//---------------------------------------------------------------------------
void __fastcall TiLamp::TiOlio::appear()
{
	if (AnimationIndex == 1)
	{
//		StartIdle();
		return;
	}

	LoopAnimation = false;
	AnimationSpeed = KOlioFadeSpeed;
	AnimationIndex = 1;

	show();

	startAnimation();
}

//---------------------------------------------------------------------------
void __fastcall TiLamp::TiOlio::disappear()
{
	if (Opacity == 0.0)
		return;

	stopAnimation();

	LoopAnimation = false;
	AnimationSpeed = KOlioFadeSpeed;
	AnimationIndex = 2;

	startAnimation();
}

//---------------------------------------------------------------------------
void __fastcall TiLamp::TiOlio::onAnimationFinished(TObject* aSender)
{
	if (AnimationIndex == 1) // has just appeared
	{
		StartIdle();
	}
	else if (AnimationIndex == 2) // has just disappeared
	{
		stopAnimation();
		hide();
	}
}

//---------------------------------------------------------------------------
void __fastcall TiLamp::TiOlio::StartIdle(bool aNewSpeed)
{
	LoopAnimation = true;
	AnimationIndex = 0;

	if (aNewSpeed)
		AnimationSpeed = Utils::randInRange(KOlioIdleSpeedMin, KOlioIdleSpeedMax);

	startAnimation();
}

//---------------------------------------------------------------------------
/*
void __fastcall TiLamp::TiOlio::moveAway()
{
	int dx = X - iLampX;
	int dy = Y - iLampY;

	moveTo(
			X + KWormDistanceMax * dx / iDistanceToLamp,
			Y + KWormDistanceMax * dy / iDistanceToLamp
	);
}

//---------------------------------------------------------------------------
void __fastcall TiLamp::TiWorm::moveClose()
{
	moveTo(
			iLampX + iDistanceToLamp * cos(iDirection * M_PI / 180.0),
			iLampY + iDistanceToLamp * sin(iDirection * M_PI / 180.0)
	);
}
*/

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall TiLamp::TiLamp(TiAnimationManager* aManager, int aX, int aY) :
		iX(aX),
		iY(aY),
		iEnabled(true)
{
	iOlios.DeleteContent = false;

	iLamp = new TiAnimation();
	iLamp->Center = TPoint(100, 100);
	iLamp->addFrames(IDR_LAMP, 200, 275);
	iLamp->LoopAnimation = false;
	iLamp->RewindAnimationAfterStop = false;
	iLamp->AllowAnimationReversion = true;
	iLamp->placeTo(iX, iY);
	iLamp->hide();

	int olioCount = Utils::randInRange(KOlioCountMin, KOlioCountMax);
	for (int i = 0; i < olioCount; i++)
	{
		double direction = TiOlio::getDirectionFromLamp(&iOlios, olioCount);
		TiOlio* olio = new TiOlio(iX, iY, direction);
		//olio->OnFadingFinished = onOlioFadingFinished;

		iOlios.add(olio);
		if (aManager)
			aManager->add(olio);
	}

	if (aManager)
		aManager->add(iLamp);
}

//---------------------------------------------------------------------------
void __fastcall TiLamp::show(bool aSetOliosOnIdle)
{
	iLamp->fadeIn();
	for (int i = 0; i < iOlios.Count; i++)
		if (aSetOliosOnIdle)
			iOlios[i]->idle();
		else
			iOlios[i]->appear();

	//TiTimeout::run(500, onWormsHideRequest);
}

//---------------------------------------------------------------------------
void __fastcall TiLamp::hide()
{
	iLamp->fadeOut();
	for (int i = 0; i < iOlios.Count; i++)
		iOlios[i]->disappear();
}

//---------------------------------------------------------------------------
void __fastcall TiLamp::lightOn()
{
	iLamp->startAnimation();
	for (int i = 0; i < iOlios.Count; i++)
		iOlios[i]->disappear();

	//TiTimeout::run(500, onWormsHideRequest);
}

//---------------------------------------------------------------------------
void __fastcall TiLamp::lightOff()
{
	iLamp->resetAnimation();

	for (int i = 0; i < iOlios.Count; i++)
	{
		TiOlio* olio = iOlios[i];
		olio->appear();
	}
}

//---------------------------------------------------------------------------
void __fastcall TiLamp::paintTo(Gdiplus::Graphics* aGraphics, EiUpdateType aUpdateType)
{
	if (aUpdateType & updStatic)
		iLamp->paintTo(aGraphics);

	if (aUpdateType & updNonStatic)
		for (int i = 0; i < iOlios.Count; i++)
			iOlios[i]->paintTo(aGraphics);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/*
void __fastcall TiLamp::onWormsHideRequest(TObject* aSender)
{
	for (int i = 0; i < iWorms.Count; i++)
		iWorms[i]->fadeOut();
}

//---------------------------------------------------------------------------
void __fastcall TiLamp::onWormFadingFinished(TObject* aSender)
{
	TiAnimation* worm = (TiAnimation*)aSender;
	worm->toggleAnimation();
}
*/
