//---------------------------------------------------------------------------
#include "EyeBox.h"
#include "assets_pg.h"

//---------------------------------------------------------------------------
#include <math.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
using namespace ProfiledGame;

//---------------------------------------------------------------------------
const double KMinDistance = 300.0;	// mm
const double KMinEyeScale = 0.2 * 0.2;
const double KCamWidth = 320;    // px
const double KCamHeight = 240;   // px
const double KInvalidValue = 0.0;

const double KMinStartDistance = 450.0; // mm
const double KMaxStartDistance = 650.0; // mm

const double KIdealDistance = 530.0;    // mm
const double KIdealEyeX 		= -36.0;    // px, left eye
const double KIdealEyeY 		=   2.0;    // px, left eye

const int KEyeSize = 36;								// px
const int KEyeCircleWidth = 8;

const int KEyeBoxWidth = 640;
const int KEyeBoxHeight = 480;

const int KProximityBufferSize = 20;
const int KThresholdZ = 80;
const int KThresholdXY = 7;

//---------------------------------------------------------------------------
__fastcall TiEyeBox::TiEyeBox(TiAnimationManager* aManager, TiSize aScreenSize, TiSize aViewport) :
		TiScene(aManager, aScreenSize, aViewport),
		iProfile(NULL)
{
	iEyeArea = TiRect(
		(aScreenSize.Width - KEyeBoxWidth) / 2, (aScreenSize.Height - KEyeBoxHeight) / 2,
		KEyeBoxWidth, KEyeBoxHeight
	);

	iBackground = new TiAnimation(false);
	//iBackground->addFrames(IDR_EYEBOX_BACKGROUND, 160, 120);
	//iBackground->placeTo(aScreenSize.Width/2, aScreenSize.Height/2);
	aManager->add(iBackground);

	iLeft = new TiAnimation(false, false);
	//iLeft->addFrames(IDR_EYEBOX_EYE, KEyeSize);
	aManager->add(iLeft);

	iRight = new TiAnimation(false, false);
	//iRight->addFrames(IDR_EYEBOX_EYE, KEyeSize);
	aManager->add(iRight);

	iProximities = new TiProximities(true);

	SetEyeLocation(iLeft, KInvalidValue, KInvalidValue);
	SetEyeLocation(iRight, KInvalidValue, KInvalidValue);
}

//---------------------------------------------------------------------------
__fastcall TiEyeBox::~TiEyeBox()
{
	delete iProximities;
}

//---------------------------------------------------------------------------
void __fastcall TiEyeBox::show(TiProfile* aProfile)
{
	if (iIsVisible)
		return;

	iAvgProximity = SiProximity();
	iProfile = aProfile;

	iBackground->show();

	iProfile->Avatar->placeTo(iScreenSize.Width / 2, iScreenSize.Height / 2);
	iProfile->Avatar->show();

	TiScene::show();
}

//---------------------------------------------------------------------------
void __fastcall TiEyeBox::hide()
{
	if (!iIsVisible)
		return;

	iBackground->hide();
	iProfile->Avatar->hide();
	iLeft->hide();
	iRight->hide();

	TiScene::hide();
}
//---------------------------------------------------------------------------
void __fastcall TiEyeBox::left(EyeDataStruct& aEyeData)
{
	if (!iIsVisible)
		return;

	SetEyeLocation(iLeft, aEyeData.eyePositionX, aEyeData.eyePositionY);
	SetEyeScale(iLeft, aEyeData.eyePositionZ);

	EstimatePositionQuality(aEyeData);
}

//---------------------------------------------------------------------------
void __fastcall TiEyeBox::right(EyeDataStruct& aEyeData)
{
	if (!iIsVisible)
		return;

	SetEyeLocation(iRight, aEyeData.eyePositionX, aEyeData.eyePositionY);
	SetEyeScale(iRight, aEyeData.eyePositionZ);
}

//---------------------------------------------------------------------------
void __fastcall TiEyeBox::paintTo(Gdiplus::Graphics* aGraphics, EiUpdateType aUpdateType)
{
	if (!iIsVisible)
		return;

	if (aUpdateType & updStatic)
	{
		iBackground->paintTo(aGraphics);
		iProfile->Avatar->paintTo(aGraphics);
	}

	if (aUpdateType & updNonStatic)
	{
		iLeft->paintTo(aGraphics);
		iRight->paintTo(aGraphics);

		Gdiplus::Pen idealSizePen(Gdiplus::Color(255 * iLeft->Opacity, 241, 9, 72), KEyeCircleWidth);

		if (iLeft->IsVisible)
		{
			double size = iLeft->Scale * KEyeSize;
			Gdiplus::Rect idealSizeLeft(iLeft->X - (size - KEyeCircleWidth/2)/2, iLeft->Y - (size - KEyeCircleWidth/2)/2, size, size);
			aGraphics->DrawEllipse(&idealSizePen, idealSizeLeft);
		}

		if (iRight->IsVisible)
		{
			double size = iRight->Scale * KEyeSize;
			Gdiplus::Rect idealSizeRight(iRight->X - (size - KEyeCircleWidth/2)/2, iRight->Y - (size - KEyeCircleWidth/2)/2, size, size);
			aGraphics->DrawEllipse(&idealSizePen, idealSizeRight);
		}
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TiEyeBox::SetEyeLocation(TiAnimation* aEye, double aX, double aY)
{
	if (!aEye)
		return;

	int fadingDirection = aEye->FadingDirection;
	bool visible = false;

	if (aX != KInvalidValue && aY != KInvalidValue)
	{
		visible = true;
		aEye->placeTo(
			iEyeArea.X + (KCamWidth/2 + aX) / KCamWidth * iEyeArea.Width,
			iEyeArea.Y + iEyeArea.Height - (KCamHeight/2 + aY) / KCamHeight * iEyeArea.Height
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

	aEye->Scale = GetScale(aDist);
}

//---------------------------------------------------------------------------
void __fastcall TiEyeBox::EstimatePositionQuality(EyeDataStruct& aEyeData)
{
	if (iAvgProximity.Z < 0)
		return;

	SiProximity* proximity = new SiProximity();

	double dx = aEyeData.eyePositionX - KIdealEyeX;
	double dy = aEyeData.eyePositionY - KIdealEyeY;
	proximity->XY = sqrt(dx*dx + dy*dy);
	proximity->Z = fabs(KIdealDistance - aEyeData.eyePositionZ);

	if (iProximities->Count == KProximityBufferSize)
		iProximities->remove(0l);

	iProximities->add(proximity);

	if (iProximities->Count == KProximityBufferSize)
	{
		double avgZ  = 0;
		double avgXY = 0;
		double weights = 0;
		for (int i = 0; i < iProximities->Count; i++)
		{
			proximity = iProximities->get(i);
			avgZ += proximity->Z;
			avgXY += proximity->XY;
			weights += 1;
		}
		iAvgProximity.Z = avgZ / weights;
		iAvgProximity.XY = avgXY / weights;

		if (iAvgProximity.Z < KThresholdZ && iAvgProximity.XY < KThresholdXY && FOnDone)
		{
			iAvgProximity.Z = -1;
			FOnDone(this);
		}
	}
}

//---------------------------------------------------------------------------
double __fastcall TiEyeBox::GetScale(double aDist)
{
	return sqrt(max(KMinEyeScale, 3.0 - 2*(aDist - KMinDistance)/(KIdealDistance - KMinDistance)));
}

