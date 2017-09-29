//---------------------------------------------------------------------------
#include "Animation.h"
#include "utils.h"

//---------------------------------------------------------------------------
#include <math.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
const int KFadingStepCount = 10;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall TiAnimation::TiAnimation(bool aVisible, bool aIsStatic) :
	TObject(),
	iWidth(16),
	iHeight(16),
	iParent(NULL),
	iAnimationFrames(NULL),
	iX(0),
	iY(0),
	iScale(1.0),
	iAngle(0.0),
	iCenter(-1, -1),
	iAnimationIndex(-1),
	iFrameIndex(-1),
	iIsStatic(aIsStatic),
	iLoopAnimation(true),
	iRewindAnimationAfterStop(true),
	iAllowAnimationReversion(false),
	iIsPaintRequested(false),
	iMoveSpeed(300),
	iRotationSpeed(300),
	iFadingDirection(0),
	iAnimationDirection(1),
	FOnPaint(NULL),
	FOnAnimationFinished(NULL),
	FOnMoveFinished(NULL),
	FOnRotationFinished(NULL),
	FOnFadingFinished(NULL)
{
	iAnimations.DeleteContent = true;

	iAnimationTimer = new TTimer(NULL);
	iAnimationTimer->Enabled = false;
	iAnimationTimer->Interval = 50;
	iAnimationTimer->OnTimer = onAnimationTimer;

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

	iUpdateTimer = new TTimer(NULL);
	iUpdateTimer->Enabled = false;
	iUpdateTimer->Interval = 10;
	iUpdateTimer->OnTimer = onUpdateTimer;

	iAlpha = aVisible ? 1.0 : 0.0;
}

//---------------------------------------------------------------------------
__fastcall TiAnimation::~TiAnimation()
{
	delete iAnimationTimer;
	delete iMoveTimer;
	delete iRotationTimer;
	delete iFadingTimer;
	delete iUpdateTimer;
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
	if (!loadBitmapFromPNG(aID, &frames))
		return;

	AddFrames(frames, aWidth, aHeight);
}

//---------------------------------------------------------------------------
void __fastcall TiAnimation::startAnimation()
{
	iAnimationTimer->Enabled = true;
}

//---------------------------------------------------------------------------
void __fastcall TiAnimation::stopAnimation()
{
	iAnimationTimer->Enabled = false;

	if (iRewindAnimationAfterStop)
	{
		iFrameIndex = 0;
		iAnimationDirection = 1;

		if (IsVisible)
			iIsPaintRequested = true;
	}
}

//---------------------------------------------------------------------------
void __fastcall TiAnimation::toggleAnimation()
{
	if (iAnimationTimer->Enabled)
		stopAnimation();
	else
		startAnimation();
}

//---------------------------------------------------------------------------
void __fastcall TiAnimation::resetAnimation()
{
	if (iFrameIndex > 0)
	{
		iFrameIndex = 0;
		iAnimationDirection = 1;
		
		if (IsVisible)
			iIsPaintRequested = true;
	}

	iAnimationTimer->Enabled = false;
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
void __fastcall TiAnimation::placeTo(int aX, int aY)
{
	iX = aX;
	iY = aY;

	if (IsVisible)
		iIsPaintRequested = true;
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
	iStartX = iMoveX = iX;
	iStartY = iMoveY = iY;

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

	double w = iScale * iWidth;
	double h = iScale * iHeight;
	double cx = iScale * iCenter.x;
	double cy = iScale * iCenter.y;
	return aX > iX - cx && aX < iX + (w - cx) &&
				 aY > iY - cy && aY < iY + (h - cy);
}

//---------------------------------------------------------------------------
double __fastcall TiAnimation::distanceTo(int aX, int aY)
{
	if (!IsVisible)
		return MaxInt;

	double w = iScale * iWidth;
	double h = iScale * iHeight;
	double cx = iScale * iCenter.x;
	double cy = iScale * iCenter.y;
	bool isInside = aX > iX - cx && aX < iX + (w - cx) &&
									aY > iY - cy && aY < iY + (h - cy);
	if (isInside)
	{
		return 0;
	}
	else
	{
		double dx = aX < iX ? (iX - cx) - aX : aX - (iX + (w - cx));
		double dy = aY < iY ? (iY - cy) - aY : aY - (iY + (h - cy));
		return sqrt(dx*dx + dy*dy);
	}
}

//---------------------------------------------------------------------------
TPoint __fastcall TiAnimation::clientToScreen(int aX, int aY)
{
	double dx = iScale * (aX - iCenter.x);
	double dy = iScale * (aY - iCenter.y);
	return TPoint(iX + dx, iY + dy);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TiAnimation::invalidate()
{
	if (IsVisible)
		iIsPaintRequested = true;
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

	double w = iScale * iWidth;
	double h = iScale * iHeight;
	double cx = iScale * iCenter.x;
	double cy = iScale * iCenter.y;
	Gdiplus::RectF destRect = Gdiplus::RectF(iX - cx, iY - cy, w, h);

	if (iAngle != 0.0)
	{
		Gdiplus::Matrix transformMatrix;
		transformMatrix.Translate(iX, iY);
		transformMatrix.Rotate(Angle);

		aGraphics->SetTransform(&transformMatrix);
		destRect = Gdiplus::RectF(-cx, -cy, w, h);
	}

	aGraphics->DrawImage(iAnimationFrames, destRect,
					iWidth * iFrameIndex, 0, iWidth, iHeight,
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

	iWidth = aWidth;
	iHeight = aHeight > 0 ? aHeight : iWidth;

	if (iCenter.x < 0 && iCenter.y < 0)
		iCenter = TPoint(iWidth / 2, iHeight / 2);

	//iHeight = aFrames->GetHeight();

	iAnimations.add(aFrames);

	if (!iAnimationFrames)
		AnimationIndex = 0;
}

//---------------------------------------------------------------------------
void __fastcall TiAnimation::SetParent(TiAnimationManager* aValue)
{
	iParent = aValue;
	iUpdateTimer->Enabled = !iParent;
}

//---------------------------------------------------------------------------
bool __fastcall TiAnimation::GetIsAnimating()
{
	return iAnimationTimer->Enabled;
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
int __fastcall TiAnimation::GetAnimationSpeed()
{
	return floor(1000.0 / iAnimationTimer->Interval + 0.5);
}

//---------------------------------------------------------------------------
void __fastcall TiAnimation::SetAnimationSpeed(int aValue)
{
	iAnimationTimer->Interval = floor(1000.0 / aValue + 0.5);
}

//---------------------------------------------------------------------------
int __fastcall TiAnimation::GetFadingDuration()
{
	return KFadingStepCount * iFadingTimer->Interval;
}

//---------------------------------------------------------------------------
void __fastcall TiAnimation::SetFadingDuration(int aValue)
{
	iFadingTimer->Interval = aValue / KFadingStepCount;
}

//---------------------------------------------------------------------------
void __fastcall TiAnimation::SetAnimationIndex(int aValue)
{
	if (aValue < 0 || aValue >= iAnimations.Count || aValue == iAnimationIndex)
		return;

	iAnimationIndex = aValue;
	iAnimationFrames = iAnimations[iAnimationIndex];
	iFrameCount = iAnimationFrames->GetWidth() / iWidth;
	iFrameIndex = iFrameCount > 0 ? 0 : -1;
	iAnimationDirection = 1;

	if (IsVisible)
		iIsPaintRequested = true;
}

//---------------------------------------------------------------------------
bool __fastcall TiAnimation::GetIsPaintRequested()
{
	bool result = iIsPaintRequested;
	iIsPaintRequested = false;
	return result;
}

//---------------------------------------------------------------------------
void __fastcall TiAnimation::SetScale(double aValue)
{
	iScale = aValue;
	if (IsVisible)
		iIsPaintRequested = true;
}

//---------------------------------------------------------------------------
void __fastcall TiAnimation::SetCenter(TPoint aCenter)
{
	iCenter = aCenter;
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
void __fastcall TiAnimation::SetOnPaint(TNotifyEvent aValue)
{
	FOnPaint = aValue;
	iUpdateTimer->Enabled = FOnPaint != NULL;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TiAnimation::onAnimationTimer(TObject* aSender)
{
	if (!IsVisible)
		return;

	int nextFrameIndex = iFrameIndex + iAnimationDirection;
	if (nextFrameIndex == iFrameCount || nextFrameIndex == -1)
	{
		if (iAllowAnimationReversion)
			iAnimationDirection = -iAnimationDirection;

		if (!iLoopAnimation)
		{
			stopAnimation();
			if (FOnAnimationFinished)
				FOnAnimationFinished(this);
		}
		else
		{
			if (iAllowAnimationReversion)
				iFrameIndex = nextFrameIndex + 2 * iAnimationDirection;
			else
				iFrameIndex = iAnimationDirection > 0 ? 0 : iFrameCount - 1;
		}
	}
	else
	{
		iFrameIndex = nextFrameIndex;
	}

	iIsPaintRequested = true;
}

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

	bool finished = !sameSign(dx, iDestinationLocation.x - iMoveX) ||
									!sameSign(dy, iDestinationLocation.y - iMoveY);
	if (finished)
	{
		iMoveX = iDestinationLocation.x;
		iMoveY = iDestinationLocation.y;
	}

	iX = iMoveX;
	iY = iMoveY;

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

	bool finished = !sameSign(dist, iDestinationAngle - iRotationAngle);
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
	double step = 1.0 / KFadingStepCount * iFadingDirection;
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

	if (!iFadingTimer->Enabled)
	{
		iFadingDirection = 0;
		if (FOnFadingFinished)
			FOnFadingFinished(this);
	}
}

//---------------------------------------------------------------------------
void __fastcall TiAnimation::onUpdateTimer(TObject* aSender)
{
	if (!FOnPaint || !iIsPaintRequested)
		return;

	FOnPaint(this);
	iIsPaintRequested = false;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall TiAnimationManager::TiAnimationManager() :
	TObject(),
	FOnPaint(NULL)
{
	iAnimations.DeleteContent = true;

	iUpdateTimer = new TTimer(NULL);
	iUpdateTimer->Enabled = true;
	iUpdateTimer->Interval = 35;
	iUpdateTimer->OnTimer = onUpdateTimer;
}

//---------------------------------------------------------------------------
__fastcall TiAnimationManager::~TiAnimationManager()
{
	delete iUpdateTimer;
}

//---------------------------------------------------------------------------
void __fastcall TiAnimationManager::add(TiAnimation* aAnimation)
{
	aAnimation->Parent = this;
	iAnimations.add(aAnimation);
}

//---------------------------------------------------------------------------
void __fastcall TiAnimationManager::paintTo(Gdiplus::Graphics* aGraphics)
{
	for (int i = 0; i < iAnimations.Count; i++)
	{
		iAnimations[i]->paintTo(aGraphics);
	}
}

//---------------------------------------------------------------------------
void __fastcall TiAnimationManager::startAnimation()
{
	for (int i = 0; i < iAnimations.Count; i++)
		iAnimations[i]->startAnimation();
}

//---------------------------------------------------------------------------
void __fastcall TiAnimationManager::stopAnimation()
{
	for (int i = 0; i < iAnimations.Count; i++)
		iAnimations[i]->stopAnimation();
}

//---------------------------------------------------------------------------
void __fastcall TiAnimationManager::toggleAnimation()
{
	for (int i = 0; i < iAnimations.Count; i++)
		iAnimations[i]->toggleAnimation();
}

//---------------------------------------------------------------------------
void __fastcall TiAnimationManager::stopMove()
{
	for (int i = 0; i < iAnimations.Count; i++)
		iAnimations[i]->stopMove();
}

//---------------------------------------------------------------------------
void __fastcall TiAnimationManager::setLoopAnimation(bool aValue)
{
	for (int i = 0; i < iAnimations.Count; i++)
		iAnimations[i]->LoopAnimation = aValue;
}

//---------------------------------------------------------------------------
void __fastcall TiAnimationManager::setAnimationSpeed(int aValue)
{
	for (int i = 0; i < iAnimations.Count; i++)
		iAnimations[i]->AnimationSpeed = aValue;
}

//---------------------------------------------------------------------------
void __fastcall TiAnimationManager::setMoveSpeed(double aValue)
{
	for (int i = 0; i < iAnimations.Count; i++)
		iAnimations[i]->MoveSpeed = aValue;
}

//---------------------------------------------------------------------------
void __fastcall TiAnimationManager::setRotationSpeed(double aValue)
{
	for (int i = 0; i < iAnimations.Count; i++)
		iAnimations[i]->RotationSpeed = aValue;
}

//---------------------------------------------------------------------------
TiAnimation* TiAnimationManager::operator[](int aIndex)
{
	return aIndex < 0 || aIndex >= iAnimations.Count ? NULL : iAnimations[aIndex];
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int __fastcall TiAnimationManager::GetAnimationCount()
{
	return iAnimations.Count;
}

//---------------------------------------------------------------------------
void __fastcall TiAnimationManager::onUpdateTimer(TObject* aSender)
{
	if (!FOnPaint)
		return;

	bool updateRequested = false;
	bool onlyNonStatic = true;
	for (int i = 0; i < iAnimations.Count; i++)
	{
		TiAnimation* animation = iAnimations[i];
		bool animationToUpdate = animation->IsPaintRequested;
		updateRequested = animationToUpdate || updateRequested;

		if (animationToUpdate && onlyNonStatic)
			onlyNonStatic = !animation->IsStatic;
	}

	if (updateRequested)
		FOnPaint(this, onlyNonStatic ? updNonStatic : updAll);
}

