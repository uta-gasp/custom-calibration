//---------------------------------------------------------------------------
#include "Rewards.h"
#include "assets_pg.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
using namespace ProfiledGame;

//---------------------------------------------------------------------------
const WideString KFontName = L"Arial";
const int KFontSize = 36;
const Gdiplus::FontStyle KFontStyle = Gdiplus::FontStyleBold;
const Gdiplus::Unit KFontUnits = Gdiplus::UnitPixel;

const Gdiplus::Color KColorScore(255, 0, 128, 192);
const Gdiplus::Color KColorCoins(255, 255, 255, 255);
const int KPositionScoreY = 248;

const Gdiplus::Color KScoreBackgroundColor(255, 127, 208, 255);
const TiRect KScoreBackgroundRect(527, 245, 310, 47);

//---------------------------------------------------------------------------
__fastcall TiRewards::TiRewards(TiAnimationManager* aManager,
		TiSize aScreenSize, TiSize aViewport) :
		TiScene(aManager, aScreenSize, aViewport)
{
	iBackground = new TiAnimation(false);
	iBackground->addFrames(IDR_INSTRUCTIONS_REWARDS, aViewport.Width, aViewport.Height);
	iBackground->placeTo(aScreenSize.Width / 2, aScreenSize.Height / 2);
	aManager->add(iBackground);
	iStaticAssets->add(iBackground);

	iBonus = new TiAnimation(false);
	iBonus->addFrames(IDR_INSTRUCTIONS_REWARDS_BONUS, aViewport.Width, aViewport.Height);
	iBonus->placeTo(aScreenSize.Width / 2, aScreenSize.Height / 2);
	aManager->add(iBonus);
	iStaticAssets->add(iBonus);

	iButtonContinue = TiRect(Offset.x + 583, Offset.y + 482, 200, 60);
}

//---------------------------------------------------------------------------
void __fastcall TiRewards::show(TiProfile* aProfile)
{
	iLevel = aProfile->Level;
	iScore = aProfile->LevelScore;
	iLevelScoreMax = aProfile->LevelScoreMax;
	iCoins = aProfile->GameCoins;

	iBackground->setFrame(aProfile->IsSucceeded ? 1 : 0);

	TiScene::show();

	iBonus->hide();
}

//---------------------------------------------------------------------------
void __fastcall TiRewards::showBonus()
{
	if (iIsVisible)
		iBonus->show();
}

//---------------------------------------------------------------------------
void __fastcall TiRewards::mouseClick(TMouseButton aButton, TShiftState aShift, int aX, int aY)
{
	if (iButtonContinue.hits(aX, aY))
	{
		if (FOnDone)
			FOnDone(this);
	}
}

//---------------------------------------------------------------------------
void __fastcall TiRewards::paintTo(Gdiplus::Graphics* aGraphics, EiUpdateType aUpdateType)
{
	if (iIsVisible && aUpdateType & updStatic)
	{
		double rate = double(iScore) / iLevelScoreMax;
		int width = rate * KScoreBackgroundRect.Width;

		Gdiplus::Rect fullRect(
			Offset.x + KScoreBackgroundRect.X, Offset.y + KScoreBackgroundRect.Y,
			KScoreBackgroundRect.Width, KScoreBackgroundRect.Height);

		Gdiplus::SolidBrush whiteBrush(Gdiplus::Color(255, 255, 255, 255));
		aGraphics->FillRectangle(&whiteBrush, fullRect);

		Gdiplus::Rect scoreBackgroundRect(
			Offset.x + KScoreBackgroundRect.X, Offset.y + KScoreBackgroundRect.Y,
			width, KScoreBackgroundRect.Height);

		Gdiplus::SolidBrush scoreBackgroundBrush(KScoreBackgroundColor);
		aGraphics->FillRectangle(&scoreBackgroundBrush, scoreBackgroundRect);
	}

	TiScene::paintTo(aGraphics, aUpdateType);

	if (iIsVisible && aUpdateType & updNonStatic)
	{
		DrawNumber(aGraphics, iScore, 688, KPositionScoreY, KColorScore, Gdiplus::StringAlignmentFar);
		DrawString(aGraphics, String("/"), 680, KPositionScoreY, KColorScore);
		DrawNumber(aGraphics, iLevelScoreMax, 690, KPositionScoreY, KColorScore);
		DrawString(aGraphics, String().sprintf("Taso %d", iLevel + 1),
				KScoreBackgroundRect.X + KScoreBackgroundRect.Width / 2,
				KScoreBackgroundRect.Y + KScoreBackgroundRect.Height + 10,
				KColorCoins, Gdiplus::StringAlignmentCenter);
		DrawString(aGraphics, String().sprintf("+%d", iCoins), 685, 370, KColorCoins);
	}
}

//---------------------------------------------------------------------------
void __fastcall TiRewards::DrawNumber(Gdiplus::Graphics* aGraphics, int aNumber, int aX, int aY,
		const Gdiplus::Color& aColor, Gdiplus::StringAlignment aAlignment)
{
	WCHAR string[256];
	wcscpy(string, WideString(String(aNumber)).c_bstr());

	Gdiplus::Font font(KFontName, KFontSize, KFontStyle, KFontUnits);
	Gdiplus::PointF origin(Offset.x + aX, Offset.y + aY);
	Gdiplus::SolidBrush brush(aColor);
	Gdiplus::StringFormat format;
	format.SetAlignment(aAlignment);

	aGraphics->DrawString(string, wcslen(string), &font, origin,	&format, &brush);
}

//---------------------------------------------------------------------------
void __fastcall TiRewards::DrawString(Gdiplus::Graphics* aGraphics, String& aText, int aX, int aY,
		const Gdiplus::Color& aColor, Gdiplus::StringAlignment aAlignment)
{
	WCHAR string[256];
	wcscpy(string, WideString(aText).c_bstr());

	Gdiplus::Font font(KFontName, KFontSize, KFontStyle, KFontUnits);
	Gdiplus::PointF origin(Offset.x + aX, Offset.y + aY);
	Gdiplus::SolidBrush brush(aColor);
	Gdiplus::StringFormat format;
	format.SetAlignment(aAlignment);

	aGraphics->DrawString(string, wcslen(string), &font, origin,	&format, &brush);
}

