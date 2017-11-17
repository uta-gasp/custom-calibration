//---------------------------------------------------------------------------
#include "GameResult.h"
#include "assets_pg.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
using namespace ProfiledGame;

//---------------------------------------------------------------------------
__fastcall TiGameResult::TiGameResult(TiAnimationManager* aManager,
		TiSize aScreenSize, TiSize aViewport) :
		TiScene(aManager, aScreenSize, aViewport)
{
	iButtonContinue = TiRect(Offset.x + 583, Offset.y + 505, 200, 60);
}

//---------------------------------------------------------------------------
void __fastcall TiGameResult::show(TiProfile* aProfile, int aLevel)
{
	iBackground = new TiAnimation(false, false);
	switch (aLevel)
	{
		case 0: iBackground->addFrames(IDR_INSTRUCTIONS_LEVEL1_RESULT, iViewport.Width, iViewport.Height); break;
		case 1: iBackground->addFrames(IDR_INSTRUCTIONS_LEVEL2_RESULT, iViewport.Width, iViewport.Height); break;
	}
	iBackground->placeTo(iScreenSize.Width / 2, iScreenSize.Height / 2);
	iManager->add(iBackground);
	iDynamicAssets->add(iBackground);

	iGameScore = aProfile->GameScore;

	iBackground->setFrame(aProfile->IsSucceeded ? 1 : 0);

	TiScene::show();
}

//---------------------------------------------------------------------------
void __fastcall TiGameResult::mouseClick(TMouseButton aButton, TShiftState aShift, int aX, int aY)
{
	if (iButtonContinue.hits(aX, aY))
	{
		if (FOnDone)
			FOnDone(this);
	}
}

//---------------------------------------------------------------------------
void __fastcall TiGameResult::paintTo(Gdiplus::Graphics* aGraphics, EiUpdateType aUpdateType)
{
	TiScene::paintTo(aGraphics, aUpdateType);
	
	if (iIsVisible && aUpdateType & updNonStatic)
	{
		DrawNumber(aGraphics, iGameScore.Success, 598);
		DrawNumber(aGraphics, iGameScore.Failure, 750);
	}
}

//---------------------------------------------------------------------------
void __fastcall TiGameResult::DrawNumber(Gdiplus::Graphics* aGraphics, int aNumber, int aX)
{
	 WCHAR string[256];
	 wcscpy(string, WideString(String(aNumber)).c_bstr());

	 Gdiplus::Font font(L"Arial", 65, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
	 Gdiplus::PointF origin(Offset.x + aX, Offset.y + 380);
	 Gdiplus::SolidBrush brush(Gdiplus::Color(255, 255, 255, 255));
	 Gdiplus::StringFormat format;
	 format.SetAlignment(Gdiplus::StringAlignmentFar);

	 aGraphics->DrawString(string, wcslen(string), &font, origin,	&format, &brush);
}

