//---------------------------------------------------------------------------
#include "Animation.h"
#include "utils.h"

//---------------------------------------------------------------------------
#include <math.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
__fastcall TiAnimation::TiAnimation(bool aVisible, bool aIsStatic) :
		TiAnimationBase(aIsStatic),
		iAnimationFrames(NULL),
		iScale(1.0),
		iAngle(0.0),
		iAnimationIndex(-1),
		iMoveSpeed(300),
		iRotationSpeed(300),
		iFadingDirection(0),
		iFadingStepCount(10),
		FOnMoveFinished(NULL),
		FOnRotationFinished(NULL),
		FOnFadingFinished(NULL),
		FOnFadingTransition(NULL)
{
	iMoveTimer = new TTimer(NULL);
	iMoveTimer->Enabled = false;
	iMoveTimer->Interval = 20;
	iMoveTimer->OnTimer = onMoveTimer;

	iRotationTimer = new TTimer(NULL);
	iRotationTimer->Enabled = false;
	iRotationTimer->Interval = 20;
	iRotationTimer->OnTimer = onRotationTimer;

	iFadingTimer = new TTimer(NULL);
	iFadingTimer->Enabled = false;
	iFadingTimer->Interval = 35;
	iFadingTimer->OnTimer = onFadingTimer;

	iAlpha = aVisible ? 1.0 : 0.0;
}

//---------------------------------------------------------------------------
__fastcall TiAnimation::~TiAnimation()
{
	delete iMoveTimer;
	delete iRotationTimer;
	delete iFadingTimer;
}

//---------------------------------------------------------------------------
void __fastcall TiAnimation::addFrames(const wchar_t* aPath, const wchar_t* aFileName, int aWidth, int aHeight)
{
	size_t bufferLength = wcslen(aPath) + wcslen(aFileName) + 1;
	wchar_t* filename = new wchar_t[bufferLength];
	wchar_t* p = filename;
	wcscpy(p, aPath);
	p += wcslen(aPath);
	wcscpy(p, aFileName);

	AddFrames(new Gdiplus::Bitmap(filename), aWidth, aHeight);

	delete filename;
}

//---------------------------------------------------------------------------
void __fastcall TiAnimation::addFrames(const wchar_t* aName, int aWidth, int aHeight)
{
	AddFrames(new Gdiplus::Bitmap(HInstance, aName), aWidth, aHeight);
}

//---------------------------------------------------------------------------
void __fastcall TiAnimation::addFrames(UINT aID, int aWidth, int aHeight)
{
	Gdiplus::Bitmap* frames;
	if (!Utils::loadBitmapFromPNG(aID, &frames))
		return;

	AddFrames(frames, aWidth, aHeight);
}

//---------------------------------------------------------------------------
bool __fastcall TiAnimation::fadeOut()
{
	if (iAlpha == 0.0)
	{
		iFadingTimer->Enabled = false;
		return false;
	}

	iFadingDirection = -1;
	if (!iFadingTimer->Enabled)
		iFadingTimer->Enabled = true;

	return true;
}

//---------------------------------------------------------------------------
bool __fastcall TiAnimation::fadeIn()
{
	if (iAlpha == 1.0)
	{
		iFadingTimer->Enabled = false;
		return false;
	}

	iFadingDirection = 1;
	if (!iFadingTimer->Enabled)
		iFadingTimer->Enabled = true;

	return true;
}

//---------------------------------------------------------------------------
void __fastcall TiAnimation::show()
{
	if (iAlpha < 1.0)
	{
		iAlpha = 1.0;
		iIsPaintRequested = true;
	}
}

//---------------------------------------------------------------------------
void __fastcall TiAnimation::hide()
{
	if (iAlpha > 0.0)
	{
		iAlpha = 0.0;
		iIsPaintRequested = true;
	}
}

//---------------------------------------------------------------------------
bool __fastcall TiAnimation::moveTo(int aX, int aY)
{
	if (X == aX && Y == aY)
	{
		iMoveTimer->Enabled = false;
		return false;
	}

	iDestinationLocation = TPoint(aX, aY);
	iStartX = iMoveX = X;
	iStartY = iMoveY = Y;

	if (!iMoveTimer->Enabled)
		iMoveTimer->Enabled = true;

	return true;
}

//---------------------------------------------------------------------------
void __fastcall TiAnimation::stopMove()
{
	iMoveTimer->Enabled = false;
}

//---------------------------------------------------------------------------
void __fastcall TiAnimation::rotateBy(double aAngle)
{
	if (iRotationTimer->Enabled)
		return;

	iDestinationAngle = iAngle + aAngle;
	iStartAngle = iRotationAngle = iAngle;

	iRotationTimer->Enabled = true;
}

//---------------------------------------------------------------------------
void __fastcall TiAnimation::stopRotation()
{
	iRotationTimer->Enabled = false;
}

//---------------------------------------------------------------------------
void __fastcall TiAnimation::setOrientation(double aDX, double aDY)
{
	double angle = 0;
	if (aDX == 0.0 || aDY == 0.0)
	{
		if (aDX == 0.0)
			angle = aDY > 0 ? 180 : 0;
		else if (aDY == 0.0)
			angle = aDX > 0 ? 90 : 270;
	}
	else
	{
		angle = 180 * atan2(aDY, aDX) / M_PI + 90;
		while (angle < 0)
			angle += 360;
	}

	Angle = angle;
}

//---------------------------------------------------------------------------
bool __fastcall TiAnimation::hitTest(int aX, int aY)
{
	if (!IsVisible)
		return false;

	double w = iScale * Width;
	double h = iScale * Height;
	double cx = iScale * Center.x;
	double cy = iScale * Center.y;
	return aX > X - cx && aX < X + (w - cx) &&
				 aY > Y - cy && aY < Y + (h - cy);
}

//---------------------------------------------------------------------------
double __fastcall TiAnimation::distanceTo(int aX, int aY)
{
	if (!IsVisible)
		return MaxInt;

	double w = iScale * Width;
	double h = iScale * Height;
	double cx = iScale * Center.x;
	double cy = iScale * Center.y;
	bool isInside = aX > X - cx && aX < X + (w - cx) &&
									aY > Y - cy && aY < Y + (h - cy);
	if (isInside)
	{
		return 0;
	}
	else
	{
		double dx = aX < X ? (X - cx) - aX : aX - (X + (w - cx));
		double dy = aY < Y ? (Y - cy) - aY : aY - (Y + (h - cy));
		return sqrt(dx*dx + dy*dy);
	}
}

//---------------------------------------------------------------------------
TPoint __fastcall TiAnimation::clientToScreen(int aX, int aY)
{
	double dx = iScale * (aX - Center.x);
	double dy = iScale * (aY - Center.y);
	return TPoint(X + dx, Y + dy);
}

//---------------------------------------------------------------------------
void __fastcall TiAnimation::paintTo(Gdiplus::Graphics* aGraphics)
{
	if (!IsVisible || iFrameIndex < 0)
		return;

	Gdiplus::ImageAttributes attributes;
	if (iAlpha < 1.0)
	{
		Gdiplus::ColorMatrix colorMatrix = {
				1.0, 0.0, 0.0, 0.0, 0.0,
				0.0, 1.0, 0.0, 0.0, 0.0,
				0.0, 0.0, 1.0, 0.0, 0.0,
				0.0, 0.0, 0.0, iAlpha, 0.0,
				0.0, 0.0, 0.0, 0.0, 1.0,
		};
		attributes.SetColorMatrix(&colorMatrix, Gdiplus::ColorMatrixFlagsDefault);
	}

	double w = iScale * Width;
	double h = iScale * Height;
	double cx = iScale * Center.x;
	double cy = iScale * Center.y;
	Gdiplus::RectF destRect = Gdiplus::RectF(X - cx, Y - cy, w, h);

	if (iAngle != 0.0)
	{
		Gdiplus::Matrix transformMatrix;
		transformMatrix.Translate(X, Y);
		transformMatrix.Rotate(Angle);

		aGraphics->SetTransform(&transformMatrix);
		destRect = Gdiplus::RectF(-cx, -cy, w, h);
	}

	aGraphics->DrawImage(iAnimationFrames, destRect,
					Width * iFrameIndex, 0, Width, Height,
					Gdiplus::UnitPixel,
					iAlpha < 1.0 ? &attributes : NULL);
					
	if (iAngle != 0.0)
	{
		aGraphics->ResetTransform();
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TiAnimation::AddFrames(Gdiplus::Bitmap* aFrames, int aWidth, int aHeight)
{
	if (!aFrames)
		return;

	if (!aFrames->GetWidth())
	{
		delete aFrames;
		return;
	}

	SetSize(aWidth, aHeight > 0 ? aHeight : aWidth);

	iBitmaps.add(aFrames);

	if (!iAnimationFrames)
		AnimationIndex = 0;
}

//---------------------------------------------------------------------------
bool __fastcall TiAnimation::GetIsMoving()
{
	return iMoveTimer->Enabled;
}

//---------------------------------------------------------------------------
bool __fastcall TiAnimation::GetIsRotating()
{
	return iRotationTimer->Enabled;
}

//---------------------------------------------------------------------------
bool __fastcall TiAnimation::GetIsVisible()
{
	return iAlpha > 0.0;
}

//---------------------------------------------------------------------------
int __fastcall TiAnimation::GetFadingDuration()
{
	return iFadingStepCount * iFadingTimer->Interval;
}

//---------------------------------------------------------------------------
void __fastcall TiAnimation::SetFadingDuration(int aValue)
{
	iFadingTimer->Interval = aValue / iFadingStepCount;
}

//---------------------------------------------------------------------------
void __fastcall TiAnimation::SetFadingStepCount(int aValue)
{
	if (aValue < 3)
		return;

	int currentFadingDuration = FadingDuration;
	iFadingStepCount = aValue;
	iFadingTimer->Interval = currentFadingDuration / iFadingStepCount;
}

//---------------------------------------------------------------------------
void __fastcall TiAnimation::SetAnimationIndex(int aValue)
{
	if (aValue < 0 || aValue >= iBitmaps.Count || aValue == iAnimationIndex)
		return;

	iAnimationIndex = aValue;
	iAnimationFrames = iBitmaps[iAnimationIndex];

	iFrameCount = iAnimationFrames->GetWidth() / Width;
	iFrameIndex = iFrameCount > 0 ? 0 : -1;
	iAnimationDirection = 1;

	if (IsVisible)
		iIsPaintRequested = true;
}

//---------------------------------------------------------------------------
void __fastcall TiAnimation::SetScale(double aValue)
{
	iScale = aValue;
	if (IsVisible)
		iIsPaintRequested = true;
}

//---------------------------------------------------------------------------
void __fastcall TiAnimation::SetAngle(double aValue)
{
	iAngle = aValue;
	if (IsVisible)
		iIsPaintRequested = true;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TiAnimation::onMoveTimer(TObject* aSender)
{
	double dx = iDestinationLocation.x - iStartX;
	double dy = iDestinationLocation.y - iStartY;
	double dist = sqrt(dx*dx + dy*dy);
	double pixelsToMove = iMoveSpeed * iMoveTimer->Interval / 1000;
	double fraction = pixelsToMove / dist;
	iMoveX += dx * fraction;
	iMoveY += dy * fraction;

	bool finished = !Utils::sameSign(dx, iDestinationLocation.x - iMoveX) ||
									!Utils::sameSign(dy, iDestinationLocation.y - iMoveY);
	if (finished)
	{
		iMoveX = iDestinationLocation.x;
		iMoveY = iDestinationLocation.y;
	}

	SetLocation(iMoveX, iMoveY);

	if (IsVisible)
		iIsPaintRequested = true;

	if (finished)
	{
		stopMove();
		if (FOnMoveFinished)
			FOnMoveFinished(this);
	}
}

//---------------------------------------------------------------------------
void __fastcall TiAnimation::onRotationTimer(TObject* aSender)
{
	double dist = iDestinationAngle - iStartAngle;
	double degreesToRotate = iRotationSpeed * iRotationTimer->Interval / 1000;
	double fraction = degreesToRotate / dist;
	iRotationAngle += dist * fraction;

	bool finished = !Utils::sameSign(dist, iDestinationAngle - iRotationAngle);
	if (finished)
		iRotationAngle = iDestinationAngle;

	iAngle = iRotationAngle;

	if (IsVisible)
		iIsPaintRequested = true;

	if (finished)
	{
		while (iAngle > 180) iAngle -= 360;
		while (iAngle < -180) iAngle -= 360;

		stopRotation();
		if (FOnRotationFinished)
			FOnRotationFinished(this);
	}
}

//---------------------------------------------------------------------------
void __fastcall TiAnimation::onFadingTimer(TObject* aSender)
{
	double step = 1.0 / iFadingStepCount * iFadingDirection;
	double alpha = iAlpha + step;
	if (alpha <= 0.0)
	{
		alpha = 0.0;
		iFadingTimer->Enabled = false;
	}
	else if (alpha >= 1.0)
	{
		alpha = 1.0;
		iFadingTimer->Enabled = false;
	}

	iAlpha = alpha;
	iIsPaintRequested = true;

	if (FOnFadingTransition)
		FOnFadingTransition(this, iAlpha);

	if (!iFadingTimer->Enabled)
	{
		iFadingDirection = 0;
		if (FOnFadingFinished)
			FOnFadingFinished(this);
	}
}



