//---------------------------------------------------------------------------
#include "AnimationBase.h"

//---------------------------------------------------------------------------
#include <math.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
__fastcall TiAnimationBase::TiAnimationBase(bool aIsStatic) :
		iParent(NULL),
		iIsStatic(aIsStatic),
		iIsPaintRequested(false),
		iX(0),
		iY(0),
		iWidth(16),
		iHeight(16),
		iCenter(-1, -1),
		iAnimationDirection(1),
		iFrameIndex(-1),
		iFrameCount(0),
		iLoopAnimation(true),
		iRewindAnimationAfterStop(true),
		iAllowAnimationReversion(false),
		FOnPaint(NULL),
		FOnAnimationFinished(NULL)
{
	iBitmaps.DeleteContent = true;

	iAnimationTimer = new TTimer(NULL);
	iAnimationTimer->Enabled = false;
	iAnimationTimer->Interval = 50;
	iAnimationTimer->OnTimer = onAnimationTimer;

	iUpdateTimer = new TTimer(NULL);
	iUpdateTimer->Enabled = false;
	iUpdateTimer->Interval = 10;
	iUpdateTimer->OnTimer = onUpdateTimer;
}

//---------------------------------------------------------------------------
__fastcall TiAnimationBase::~TiAnimationBase()
{
	delete iAnimationTimer;
	delete iUpdateTimer;
}

//---------------------------------------------------------------------------
void __fastcall TiAnimationBase::invalidate()
{
	if (IsVisible)
		iIsPaintRequested = true;
}

//---------------------------------------------------------------------------
void __fastcall TiAnimationBase::startAnimation()
{
	iAnimationTimer->Enabled = true;
}

//---------------------------------------------------------------------------
void __fastcall TiAnimationBase::stopAnimation()
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
void __fastcall TiAnimationBase::toggleAnimation()
{
	if (iAnimationTimer->Enabled)
		stopAnimation();
	else
		startAnimation();
}

//---------------------------------------------------------------------------
void __fastcall TiAnimationBase::resetAnimation()
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
void __fastcall TiAnimationBase::setFrame(int aIndex)
{
	if (aIndex < 0 || aIndex >= iFrameCount)
		aIndex = random(iFrameCount);

	iFrameIndex = aIndex;
	if (OnPaint)
		OnPaint(this);
}

//---------------------------------------------------------------------------
void __fastcall TiAnimationBase::placeTo(int aX, int aY)
{
	iX = aX;
	iY = aY;

	if (IsVisible)
		iIsPaintRequested = true;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TiAnimationBase::SetCenter(TPoint aCenter)
{
	iCenter = aCenter;
	iIsPaintRequested = true;
}

//---------------------------------------------------------------------------
void __fastcall TiAnimationBase::SetParent(TiAnimationManager* aValue)
{
	iParent = aValue;
	iUpdateTimer->Enabled = !iParent;
}

//---------------------------------------------------------------------------
void __fastcall TiAnimationBase::SetOnPaint(TNotifyEvent aValue)
{
	FOnPaint = aValue;
	iUpdateTimer->Enabled = FOnPaint != NULL;
}

//---------------------------------------------------------------------------
bool __fastcall TiAnimationBase::GetIsPaintRequested()
{
	bool result = iIsPaintRequested;
	iIsPaintRequested = false;
	return result;
}

//---------------------------------------------------------------------------
bool __fastcall TiAnimationBase::GetIsAnimating()
{
	return iAnimationTimer->Enabled;
}

//---------------------------------------------------------------------------
double __fastcall TiAnimationBase::GetAnimationSpeed()
{
	return 1000.0 / iAnimationTimer->Interval;
}

//---------------------------------------------------------------------------
void __fastcall TiAnimationBase::SetAnimationSpeed(double aValue)
{
	iAnimationTimer->Interval = floor(1000.0 / aValue + 0.5);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TiAnimationBase::SetSize(int aWidth, int aHeight)
{
	iWidth = aWidth;
	iHeight = aHeight;

	if (iCenter.x < 0 || iCenter.y < 0)
		iCenter = TPoint(Width / 2, Height / 2);
}

//---------------------------------------------------------------------------
void __fastcall TiAnimationBase::SetLocation(int aX, int aY)
{
	iX = aX;
	iY = aY;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TiAnimationBase::onAnimationTimer(TObject* aSender)
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
void __fastcall TiAnimationBase::onUpdateTimer(TObject* aSender)
{
	if (!FOnPaint || !iIsPaintRequested)
		return;

	FOnPaint(this);
	iIsPaintRequested = false;
}

