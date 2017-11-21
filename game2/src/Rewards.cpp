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

const Gdiplus::Color KColorScore(255, 64, 32, 128);
const Gdiplus::Color KColorCoins(255, 255, 255, 255);
const int KPositionScoreY = 248;

const Gdiplus::Color KColorBackgroundScoreAll(255, 255, 255, 255);
const Gdiplus::Color KColorBackgroundScoreNew(255, 127, 208, 255);
const Gdiplus::Color KColorBackgroundScoreOld(255, 32, 128, 224);
const TiRect KScoreBackgroundRect(527, 245, 310, 47);

//---------------------------------------------------------------------------
__fastcall TiRewards::TiRewards(TiAnimationManager* aManager,
		TiSize aScreenSize, TiSize aViewport) :
		TiScene(aManager, aScreenSize, aViewport),
		iGainedPrizeIndex(-1)
{
	iPrizes = new TiAnimations(false);

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

	for (int i = 0; i < TiProfile::getBonusCountMax(); i++)
	{
		TiAnimation* prize = new TiAnimation(false);
		prize->addFrames(IDR_PRIZE_ICON + i, aViewport.Width, aViewport.Height);
		prize->placeTo(aScreenSize.Width / 2, aScreenSize.Height / 2);

		aManager->add(prize);
		iPrizes->add(prize);
	}

	iButtonContinue = TiRect(Offset.x + 583, Offset.y + 482, 200, 60);
}

//---------------------------------------------------------------------------
__fastcall TiRewards::~TiRewards()
{
	delete iPrizes;
}

//---------------------------------------------------------------------------
void __fastcall TiRewards::show(TiProfile* aProfile)
{
	iLevel = aProfile->Level;
	iScore = aProfile->LevelScore;
	iLevelScoreMax = aProfile->LevelScoreMax;

	iIsNewLevel = aProfile->IsGainedNewLevel;
	iGainedScore = aProfile->GameScore;
	iGainedCoins = aProfile->GameCoins;
	iGainedPrizeIndex = aProfile->GameBonus - 1;

	iBackground->setFrame(aProfile->IsSucceeded ? 1 : 0);

	TiScene::show();

	iBonus->hide();
}

//---------------------------------------------------------------------------
void __fastcall TiRewards::hide()
{
	if (iGainedPrizeIndex >= 0 && iGainedPrizeIndex < iPrizes->Count)
		iPrizes->get(iGainedPrizeIndex)->hide();

	TiScene::hide();
}

//---------------------------------------------------------------------------
void __fastcall TiRewards::showBonus()
{
	if (!iIsVisible)
		return;

	iBonus->show();

	if (iGainedPrizeIndex >= 0 && iGainedPrizeIndex < iPrizes->Count)
		iPrizes->get(iGainedPrizeIndex)->show();
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
		if (iIsNewLevel)
		{
			DrawScoreBackground(aGraphics, iLevelScoreMax, KColorBackgroundScoreNew);
			DrawString(aGraphics, String().sprintf("Taso %d!", iLevel + 1),
					KScoreBackgroundRect.X + KScoreBackgroundRect.Width / 2,
					KPositionScoreY,
					KColorScore, Gdiplus::StringAlignmentCenter);
		}
		else
		{
			DrawScoreBackground(aGraphics, iLevelScoreMax, KColorBackgroundScoreAll);
			DrawScoreBackground(aGraphics, iScore, KColorBackgroundScoreNew);
			DrawScoreBackground(aGraphics, max(0, iScore - iGainedScore), KColorBackgroundScoreOld);
		}
	}

	TiScene::paintTo(aGraphics, aUpdateType);

	if (iGainedPrizeIndex >= 0 && iGainedPrizeIndex < iPrizes->Count)
		iPrizes->get(iGainedPrizeIndex)->paintTo(aGraphics);

	if (iIsVisible && aUpdateType & updNonStatic)
	{
		if (!iIsNewLevel)
		{
			DrawNumber(aGraphics, iScore, 688, KPositionScoreY, KColorScore, Gdiplus::StringAlignmentFar);
			DrawString(aGraphics, String("/"), 680, KPositionScoreY, KColorScore);
			DrawNumber(aGraphics, iLevelScoreMax, 690, KPositionScoreY, KColorScore);

			DrawString(aGraphics, String().sprintf("Taso %d", iLevel + 1),
					KScoreBackgroundRect.X + KScoreBackgroundRect.Width / 2,
					KScoreBackgroundRect.Y + KScoreBackgroundRect.Height + 10,
					KColorCoins, Gdiplus::StringAlignmentCenter);
		}

		DrawString(aGraphics, String().sprintf("+%d", iGainedCoins), 685, 370, KColorCoins);
	}
}

//---------------------------------------------------------------------------
void __fastcall TiRewards::DrawScoreBackground(Gdiplus::Graphics* aGraphics, double aValue, const Gdiplus::Color& aColor)
{
	double rate = aValue / iLevelScoreMax;
	int width = rate * KScoreBackgroundRect.Width;

	Gdiplus::Rect scoreBackgroundRect(
			Offset.x + KScoreBackgroundRect.X, Offset.y + KScoreBackgroundRect.Y,
			width, KScoreBackgroundRect.Height);

	Gdiplus::SolidBrush scoreBackgroundBrush(aColor);
	aGraphics->FillRectangle(&scoreBackgroundBrush, scoreBackgroundRect);
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

