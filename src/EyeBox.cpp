//---------------------------------------------------------------------------
#include "EyeBox.h"
#include "assets.h"

//---------------------------------------------------------------------------
#include <math.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
const double KMinDistance = 30.0;
const double KMinEyeScale2 = 0.2 * 0.2;

//---------------------------------------------------------------------------
__fastcall TiEyeBox::TiEyeBox(TiAnimationManager* aManager, TRect aBox) :
		TObject(),
		iVisible(true),
		iInstabilityCounter(0),
		FOnHidden(NULL)
{
	iBox = aBox;

	iBackground = new TiAnimation();
	iBackground->addFrames(IDR_EYEBOX, 160, 120);
	iBackground->placeTo(aBox.Left + aBox.Width()/2, aBox.Top + aBox.Height()/2);
	iBackground->OnFadingFinished = onBackgroundFadingFinished;

	iLeft = new TiAnimation(false);
	iLeft->addFrames(IDR_EYE, 32);

	iRight = new TiAnimation(false);
	iRight->addFrames(IDR_EYE, 32);

	iWarning = new TiAnimation(false);
	iWarning->addFrames(IDR_WARNING, 144, 20);
	iWarning->placeTo(aBox.Left + aBox.Width()/2, aBox.Top + iWarning->Height);
	iWarning->FadingDuration = 200;

	iStart = new TiAnimation();
	iStart->addFrames(IDR_START, 120, 48);
	iStart->placeTo(
			iBackground->X - iStart->Width/2 - 4,
			iBackground->Y + iBackground->Height/2 + 4 + iStart->Height/2
	);

	iClose = new TiAnimation();
	iClose->addFrames(IDR_CLOSE, 120, 48);
	iClose->placeTo(
			iBackground->X + iClose->Width/2 + 4,
			iBackground->Y + iBackground->Height/2 + 4 + iClose->Height/2
	);

	if (aManager)
	{
		aManager->add(iBackground);
		aManager->add(iLeft);
		aManager->add(iRight);
		aManager->add(iWarning);
		aManager->add(iStart);
		aManager->add(iClose);
	}

	SetEyeLocation(iLeft, -1, -1);
	SetEyeLocation(iRight, -1, -1);
}

//---------------------------------------------------------------------------
__fastcall TiEyeBox::~TiEyeBox()
{
}

//---------------------------------------------------------------------------
void __fastcall TiEyeBox::left(EyeDataStruct& aEyeData)
{
	if (!iVisible)
		return;

	SetEyeLocation(iLeft, aEyeData.eyePositionX, aEyeData.eyePositionY);
	SetEyeScale(iLeft, aEyeData.eyePositionZ);
}

//---------------------------------------------------------------------------
void __fastcall TiEyeBox::right(EyeDataStruct& aEyeData)
{
	if (!iVisible)
		return;

	SetEyeLocation(iRight, aEyeData.eyePositionX, aEyeData.eyePositionY);
	SetEyeScale(iRight, aEyeData.eyePositionZ);
}

//---------------------------------------------------------------------------
void __fastcall TiEyeBox::setTrackingStability(bool aStable)
{
	iInstabilityCounter = min(15, max(0, iInstabilityCounter + (aStable ? -1 : 1)));

	if (iInstabilityCounter > 10 && (!iWarning->IsVisible && iWarning->FadingDirection != 1))
		iWarning->fadeIn();
	else if (iInstabilityCounter < 5 && iWarning->IsVisible && iWarning->FadingDirection != -1)
		iWarning->fadeOut();
}

//---------------------------------------------------------------------------
void __fastcall TiEyeBox::paintTo(Gdiplus::Graphics* aGraphics)
{
	iBackground->paintTo(aGraphics);
	iLeft->paintTo(aGraphics);
	iRight->paintTo(aGraphics);
	iWarning->paintTo(aGraphics);
	iStart->paintTo(aGraphics);
	iClose->paintTo(aGraphics);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TiEyeBox::onBackgroundFadingFinished(TObject* aSender)
{
	if (!iBackground->IsVisible && FOnHidden)
		FOnHidden(this);
}

//---------------------------------------------------------------------------
void __fastcall TiEyeBox::SetEyeLocation(TiAnimation* aEye, double aX, double aY)
{
	if (!aEye)
		return;

	int fadingDirection = aEye->FadingDirection;
	bool visible = false;

	if (aX >= 0 && aY >= 0)
	{
		visible = true;
		aEye->placeTo(
			iBox.Left + aX * iBox.Width(),
			iBox.Top + aY * iBox.Height()
		);
	}

	if (!visible && (aEye->IsVisible && fadingDirection != -1))
		aEye->fadeOut();
	else if (visible && (!aEye->IsVisible || fadingDirection == -1))
		aEye->fadeIn();
}

//---------------------------------------------------------------------------
void __fastcall TiEyeBox::SetEyeScale(TiAnimation* aEye, double aDist)
{
	if (aDist <= KMinDistance)
		return;

	aEye->Scale = sqrt(max(KMinEyeScale2, 1.0 - (aDist - KMinDistance)/50.0));
}

//---------------------------------------------------------------------------
void __fastcall TiEyeBox::SetVisible(bool aValue)
{
	if (iVisible && !aValue)
	{
		iBackground->fadeOut();
		iLeft->fadeOut();
		iRight->fadeOut();
		iWarning->fadeOut();
		iStart->fadeOut();
		iClose->fadeOut();
	}
	else if (!iVisible && aValue)
	{
		iBackground->fadeIn();
		iStart->fadeIn();
		iClose->fadeIn();
	}

	iVisible = aValue;
}
