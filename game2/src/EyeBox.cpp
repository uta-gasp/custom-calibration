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
const double KMinEyeDistance = 300.0;	// mm
const double KMinEyeScale = 0.2 * 0.2;
const double KCamWidth = 320;    // px
const double KCamHeight = 240;   // px
const double KInvalidValue = 0.0;

const int KEyeSize = 36;								// px
const int KEyeCircleWidth = 8;

const int KEyeBoxWidth = 800;
const int KEyeBoxHeight = 600;

const int KThresholdZ = 80;
const int KThresholdXY = 30;

const double KIdealDistance = 480.0;    // mm
const double KIdealEyeX 		= -28.0;    // px, left eye
const double KIdealEyeY 		=   2.0;    // px, left eye

//---------------------------------------------------------------------------
__fastcall TiEyeBox::TiEyeBox(TiAnimationManager* aManager, TiSize aScreenSize, TiSize aViewport) :
		TiScene(aManager, aScreenSize, aViewport),
		iProfile(NULL),
		iEstimateUserPositionQuality(false)
{
	iEyeArea = TiRect(
		(aScreenSize.Width - KEyeBoxWidth) / 2, (aScreenSize.Height - KEyeBoxHeight) / 2,
		KEyeBoxWidth, KEyeBoxHeight
	);

	iBackground = new TiAnimation(false);
	aManager->add(iBackground);

	iLeft = new TiAnimation(false, false);
	aManager->add(iLeft);

	iRight = new TiAnimation(false, false);
	aManager->add(iRight);

	iUserPositionQualityEstimatorLeft = new TiUserPositionQualityEstimator(KThresholdZ, KThresholdXY);
	iUserPositionQualityEstimatorRight = new TiUserPositionQualityEstimator(KThresholdZ, KThresholdXY);

	SetEyeLocation(iLeft, KInvalidValue, KInvalidValue);
	SetEyeLocation(iRight, KInvalidValue, KInvalidValue);
}

//---------------------------------------------------------------------------
__fastcall TiEyeBox::~TiEyeBox()
{
	delete iUserPositionQualityEstimatorLeft;
	delete iUserPositionQualityEstimatorRight;
}

//---------------------------------------------------------------------------
void __fastcall TiEyeBox::show(TiProfile* aProfile)
{
	if (iIsVisible)
		return;

	iEstimateUserPositionQuality = true;
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

	iLastLeftEye = aEyeData;

	SetEyeLocation(iLeft, aEyeData.eyePositionX, aEyeData.eyePositionY);
	SetEyeScale(iLeft, aEyeData.eyePositionZ);

	if (!iEstimateUserPositionQuality)
		return;

	TiUserPositionQualityEstimator::PositionQuality quality = iUserPositionQualityEstimatorLeft->feed(aEyeData);
	if (quality == TiUserPositionQualityEstimator::pqOK)
	{
		iEstimateUserPositionQuality = false;
		if (FOnDone)
			FOnDone(this);
	}
}

//---------------------------------------------------------------------------
void __fastcall TiEyeBox::right(EyeDataStruct& aEyeData)
{
	if (!iIsVisible)
		return;

	iLastRightEye = aEyeData;

	SetEyeLocation(iRight, aEyeData.eyePositionX, aEyeData.eyePositionY);
	SetEyeScale(iRight, aEyeData.eyePositionZ);
/*
	if (!iEstimateUserPositionQuality)
		return;

	TiUserPositionQualityEstimator::PositionQuality quality = iUserPositionQualityEstimatorRight->feed(aEyeData);
	if (quality == TiUserPositionQualityEstimator::pqOK)
	{
		iEstimateUserPositionQuality = false;
		if (FOnDone)
			FOnDone(this);
	}
	*/
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
#ifdef _DEBUG
			DrawDebugInfo(aGraphics, iLastLeftEye, 0);
#endif

			double size = iLeft->Scale * KEyeSize;
			Gdiplus::Rect idealSizeLeft(iLeft->X - (size - KEyeCircleWidth/2)/2, iLeft->Y - (size - KEyeCircleWidth/2)/2, size, size);
			aGraphics->DrawEllipse(&idealSizePen, idealSizeLeft);
		}

		if (iRight->IsVisible)
		{
#ifdef _DEBUG
			DrawDebugInfo(aGraphics, iLastRightEye, iScreenSize.Height / 2);
#endif

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
	if (aDist <= KMinEyeDistance)
		return;

	aEye->Scale = GetScale(aDist);
}

//---------------------------------------------------------------------------
double __fastcall TiEyeBox::GetScale(double aDist)
{
	return sqrt(max(KMinEyeScale, 3.0 - 2*(aDist - KMinEyeDistance)/(TiUserPositionQualityEstimator::getIdealUserDistance() - KMinEyeDistance)));
}

//---------------------------------------------------------------------------
void __fastcall TiEyeBox::DrawDebugInfo(Gdiplus::Graphics* aGraphics, const EyeDataStruct& aEyeData, int aOffsetY)
{
	float fontSize = 28;
	WideString text;
	Gdiplus::Font font(L"Arial", fontSize);
	Gdiplus::PointF origin(10.0f, aOffsetY + 10.0f);
	Gdiplus::SolidBrush textBrush(Gdiplus::Color(255, 255, 255, 255));

	aGraphics->DrawString(text.c_bstr(), text.Length(), &font, origin, &textBrush); origin.Y += 1.25 * fontSize;
	double dx = aEyeData.eyePositionX - KIdealEyeX;
	double dy = aEyeData.eyePositionY - KIdealEyeY;
	text = String().sprintf("%.0f", sqrt(dx*dx + dy*dy));
	aGraphics->DrawString(text.c_bstr(), text.Length(), &font, origin, &textBrush); origin.Y += 1.25 * fontSize;
	text = String().sprintf("%.0f", aEyeData.eyePositionZ - KIdealDistance);
	aGraphics->DrawString(text.c_bstr(), text.Length(), &font, origin, &textBrush); origin.Y += 1.25 * fontSize;
}
