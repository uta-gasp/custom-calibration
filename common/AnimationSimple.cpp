//---------------------------------------------------------------------------
#include "AnimationSimple.h"
#include "utils.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
__fastcall TiAnimationSimple::TiAnimationSimple(bool aIsStatic) :
		TiAnimationBase(aIsStatic),
		iIsVisible(false)
{
}

//---------------------------------------------------------------------------
void __fastcall TiAnimationSimple::addFrames(UINT aID)
{
	Gdiplus::Bitmap* frame;
	if (!Utils::loadBitmapFromPNG(aID, &frame))
		return;

	int width = frame->GetWidth();
	int height = frame->GetHeight();
	if (!width || !height)
	{
		delete frame;
		return;
	}

	SetSize(width, height);

	iBitmaps.add(frame);
	iFrameCount++;

	if (iFrameIndex < 0)
		iFrameIndex = 0;
}

//---------------------------------------------------------------------------
void __fastcall TiAnimationSimple::paintTo(Gdiplus::Graphics* aGraphics)
{
	if (!IsVisible || iFrameIndex < 0)
		return;

	Gdiplus::Bitmap* bmp = iBitmaps[iFrameIndex];

	aGraphics->DrawImage(bmp, int(X - Center.x), int(Y - Center.y), bmp->GetWidth(), bmp->GetHeight());
}

//---------------------------------------------------------------------------
void __fastcall TiAnimationSimple::show()
{
	if (!iIsVisible)
	{
		iIsVisible = true;
		iIsPaintRequested = true;
	}
}

//---------------------------------------------------------------------------
void __fastcall TiAnimationSimple::hide()
{
	if (iIsVisible)
	{
		iIsVisible = false;
		iIsPaintRequested = true;
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool __fastcall TiAnimationSimple::GetIsVisible()
{
	return iIsVisible;
}
