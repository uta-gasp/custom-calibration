//---------------------------------------------------------------------------
#include "EyeBoxWithInstructions.h"
#include "assets_ffap.h"

//---------------------------------------------------------------------------
#include <math.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
using namespace FireflyAndPoints;

//---------------------------------------------------------------------------
const double KMinDistance = 300.0;
const double KMinEyeScale2 = 0.2 * 0.2;
const double KCamWidth = 320;
const double KCamHeight = 240;
const double KInvalidValue = 0.0;
const double KMinStartDistance = 450.0;
const double KMaxStartDistance = 650.0;
const double KIdealDistance = 530.0;
const int KEyeSize = 32;

const int KInstructionWidth = 650;
const int KInstructionHeight = 170;

//---------------------------------------------------------------------------
__fastcall TiEyeBoxWithInstructions::TiEyeBoxWithInstructions(TiAnimationManager* aManager,
		TRect aBox, TiSize aScreenSize) :
		TObject(),
		iVisible(false),
		iInstabilityCounter(0),
		FOnShown(NULL),
		FOnHidden(NULL)
{
	iBox = aBox;

	iBackground = new TiAnimation(iVisible);
	iBackground->addFrames(IDR_EYEBOX, 160, 120);
	iBackground->placeTo(aBox.Left + aBox.Width()/2, aBox.Top + aBox.Height()/2);
	iBackground->OnFadingFinished = onBackgroundFadingFinished;

	iLeft = new TiAnimation(false, false);
	iLeft->addFrames(IDR_EYE, KEyeSize);

	iRight = new TiAnimation(false, false);
	iRight->addFrames(IDR_EYE, KEyeSize);

	iWarning = new TiAnimation(false);
	iWarning->addFrames(IDR_WARNING, 144, 20);
	iWarning->placeTo(aBox.Left + aBox.Width()/2, aBox.Top + iWarning->Height);
	iWarning->FadingDuration = 200;

	iStart = new TiAnimation(iVisible, false);
	iStart->addFrames(IDR_START, 160, 48);
	iStart->addFrames(IDR_START_HOVER, 160, 48);
	iStart->FadingDuration = 500;
	iStart->placeTo(
			iBackground->X,
			iBackground->Y + iBackground->Height/2 + 4 + iStart->Height/2
	);

	iInstruction = new TiAnimation(iVisible);
	iInstruction->addFrames(IDR_EYEBOX_INSTRUCTION, KInstructionWidth, KInstructionHeight);
	iInstruction->placeTo(aScreenSize.Width / 2, iStart->Y + iStart->Height / 2 + 28 + iInstruction->Height / 2 );

	if (aManager)
	{
		aManager->add(iBackground);
		aManager->add(iLeft);
		aManager->add(iRight);
		aManager->add(iWarning);
		aManager->add(iInstruction);
		aManager->add(iStart);
	}

	SetEyeLocation(iLeft, KInvalidValue, KInvalidValue);
	SetEyeLocation(iRight, KInvalidValue, KInvalidValue);
}

//---------------------------------------------------------------------------
__fastcall TiEyeBoxWithInstructions::~TiEyeBoxWithInstructions()
{
}

//---------------------------------------------------------------------------
void __fastcall TiEyeBoxWithInstructions::left(EyeDataStruct& aEyeData)
{
	if (!iVisible)
		return;

	SetEyeLocation(iLeft, aEyeData.eyePositionX, aEyeData.eyePositionY);
	SetEyeScale(iLeft, aEyeData.eyePositionZ);

	setTrackingStability(aEyeData.eyePositionZ >= KMinStartDistance && aEyeData.eyePositionZ <= KMaxStartDistance);
}

//---------------------------------------------------------------------------
void __fastcall TiEyeBoxWithInstructions::right(EyeDataStruct& aEyeData)
{
	if (!iVisible)
		return;

	SetEyeLocation(iRight, aEyeData.eyePositionX, aEyeData.eyePositionY);
	SetEyeScale(iRight, aEyeData.eyePositionZ);
}

//---------------------------------------------------------------------------
void __fastcall TiEyeBoxWithInstructions::setTrackingStability(bool aStable)
{
	iInstabilityCounter = min(15, max(0, iInstabilityCounter + (aStable ? -1 : 1)));

	//if (iInstabilityCounter > 10 && (!iWarning->IsVisible && iWarning->FadingDirection != 1))
	//	iWarning->fadeIn();
	//else if (iInstabilityCounter < 5 && iWarning->IsVisible && iWarning->FadingDirection != -1)
	//	iWarning->fadeOut();
	if (aStable)
	{
		if (iStart->Opacity < 1.0)
			iStart->fadeIn();
	}
	else
	{
		if (iStart->Opacity > 0.0)
			iStart->fadeOut();
	}
}

//---------------------------------------------------------------------------
void __fastcall TiEyeBoxWithInstructions::paintTo(Gdiplus::Graphics* aGraphics, EiUpdateType aUpdateType)
{
	if (aUpdateType & updStatic)
	{
		iBackground->paintTo(aGraphics);
		iInstruction->paintTo(aGraphics);
	}

	if (aUpdateType & updNonStatic)
	{
		iStart->paintTo(aGraphics);
		iLeft->paintTo(aGraphics);
		iRight->paintTo(aGraphics);

		double size = GetScale(KIdealDistance) * KEyeSize;
		double penSize = 3;
		Gdiplus::Pen idealSizePen(Gdiplus::Color(255 * iLeft->Opacity, 0, 192, 0), penSize);

		if (iLeft->IsVisible)
		{
			Gdiplus::Rect idealSizeLeft(iLeft->X - (size - penSize/2)/2, iLeft->Y - (size - penSize/2)/2, size, size);
			aGraphics->DrawEllipse(&idealSizePen, idealSizeLeft);
		}

		if (iRight->IsVisible)
		{
			Gdiplus::Rect idealSizeRight(iRight->X - (size - penSize/2)/2, iRight->Y - (size - penSize/2)/2, size, size);
			aGraphics->DrawEllipse(&idealSizePen, idealSizeRight);
		}
	}

	if (aUpdateType & updStatic)
	{
		iWarning->paintTo(aGraphics);
		//iClose->paintTo(aGraphics);
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TiEyeBoxWithInstructions::onBackgroundFadingFinished(TObject* aSender)
{
	if (iBackground->IsVisible && FOnShown)
		FOnShown(this);
	if (!iBackground->IsVisible && FOnHidden)
		FOnHidden(this);
}

//---------------------------------------------------------------------------
void __fastcall TiEyeBoxWithInstructions::SetEyeLocation(TiAnimation* aEye, double aX, double aY)
{
	if (!aEye)
		return;

	int fadingDirection = aEye->FadingDirection;
	bool visible = false;

	if (aX != KInvalidValue && aY != KInvalidValue)
	{
		visible = true;
		aEye->placeTo(
			iBox.Left + (KCamWidth/2 + aX) / KCamWidth * iBox.Width(),
			iBox.Bottom - (KCamHeight/2 + aY) / KCamHeight * iBox.Height()
		);
	}

	if (!visible && (aEye->IsVisible && fadingDirection != -1))
		aEye->fadeOut();
	else if (visible && (!aEye->IsVisible || fadingDirection == -1))
		aEye->fadeIn();
}

//---------------------------------------------------------------------------
void __fastcall TiEyeBoxWithInstructions::SetEyeScale(TiAnimation* aEye, double aDist)
{
	if (aDist <= KMinDistance)
		return;

	aEye->Scale = GetScale(aDist);
}

//---------------------------------------------------------------------------
double __fastcall TiEyeBoxWithInstructions::GetScale(double aDist)
{
	return sqrt(max(KMinEyeScale2, 1.0 - (aDist - KMinDistance)/350.0));
}

//---------------------------------------------------------------------------
void __fastcall TiEyeBoxWithInstructions::SetVisible(bool aValue)
{
	if (iVisible && !aValue)
	{
		iBackground->fadeOut();
		iLeft->fadeOut();
		iRight->fadeOut();
		iWarning->fadeOut();
		iInstruction->fadeOut();
		iStart->fadeOut();
	}
	else if (!iVisible && aValue)
	{
		iBackground->fadeIn();
		iInstruction->fadeIn();
		iStart->fadeIn();
	}

	iVisible = aValue;
}
