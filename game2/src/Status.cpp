//---------------------------------------------------------------------------
#include "Status.h"
#include "assets_pg.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
using namespace ProfiledGame;

//---------------------------------------------------------------------------
const WideString KFontName = L"Arial";
const int KFontSize = 36;
const Gdiplus::FontStyle KFontStyle = Gdiplus::FontStyleRegular;
const Gdiplus::Unit KFontUnits = Gdiplus::UnitPixel;

const Gdiplus::Color KColorName(255, 255, 255, 255);
const Gdiplus::Color KColorScore(255, 0, 128, 192);
const Gdiplus::Color KColorSaldo(255, 255, 255, 255);
const int KLevelLabelsPositionY = 44;

const Gdiplus::Color KScoreBackgroundColor(255, 127, 208, 255);
const TiRect KScoreBackgroundRect(567, KLevelLabelsPositionY - 3, 310, 47);

//---------------------------------------------------------------------------
__fastcall TiStatus::TiStatus(TiAnimationManager* aManager,
		TiSize aScreenSize, TiSize aViewport) :
		TiScene(aManager, aScreenSize, aViewport)
{
	iTiles = new TiTiles(true);

	iBackground = new TiAnimation(false);
	iBackground->addFrames(IDR_STATUS_BACKGROUND, aViewport.Width, aViewport.Height);
	iBackground->placeTo(aScreenSize.Width / 2, aScreenSize.Height / 2);
	aManager->add(iBackground);

	iPanel = new TiAnimation(false);
	iPanel->addFrames(IDR_STATUS_PANEL, aViewport.Width, aViewport.Height);
	iPanel->placeTo(aScreenSize.Width/2, aScreenSize.Height/2);
	aManager->add(iPanel);

	iPrizes = new TiAnimation(false);
	iPrizes->addFrames(IDR_STATUS_PRIZES, aViewport.Width, aViewport.Height);
	iPrizes->placeTo(aScreenSize.Width/2, aScreenSize.Height/2);
	aManager->add(iPrizes);

	iButtonContinue = new TiAnimation(false);
	iButtonContinue->addFrames(IDR_STATUS_BUTTON_CONTINUE, 209, 70);
	iButtonContinue->placeTo(Offset.x + aViewport.Width/4*3, Offset.y + aViewport.Height/3*2);
	aManager->add(iButtonContinue);

	iButtonPrizes = new TiAnimation(false);
	iButtonPrizes->addFrames(IDR_STATUS_BUTTON_PRIZES, 160, 68);
	iButtonPrizes->placeTo(Offset.x + 1050, Offset.y + 66);
	aManager->add(iButtonPrizes);

	for (int i = 0; i < IDR_PRIZE_COUNT + 1; i++)       // +1 because of IDR_STATUS_TILE_PRIZE_REST
	{
		int row = i / 4;
		int col = i % 4;
		TiRect rc(Offset.x + 795 + col * 115, Offset.y + 293 + row * 115, 107, 104);

		TiAnimation* tile = new TiAnimation(false, false);
		tile->addFrames(IDR_STATUS_TILE_PRIZE + i, aViewport.Width, aViewport.Height);
		tile->placeTo(aScreenSize.Width/2, aScreenSize.Height/2);
		aManager->add(tile);

		TiAnimation* selected = new TiAnimation(false, false);
		selected->addFrames(IDR_STATUS_PRIZE_SELECTED, 79, 71);
		selected->placeTo(rc.X + 107/2, rc.Y + 104/2);
		aManager->add(selected);

		iTiles->add(new SiTile(tile, selected, rc));
	}

	iButtonContinueRect = TiRect(iButtonContinue->X - iButtonContinue->Width / 2,
			iButtonContinue->Y - iButtonContinue->Height / 2,
			iButtonContinue->Width, iButtonContinue->Height);

	iButtonPrizesRect = TiRect(iButtonPrizes->X - iButtonPrizes->Width / 2,
			iButtonPrizes->Y - iButtonPrizes->Height / 2,
			iButtonPrizes->Width, iButtonPrizes->Height);

	iButtonPrizesReadyRect = TiRect(Offset.x + 936, Offset.y + 662, 200, 60);
}

//---------------------------------------------------------------------------
__fastcall TiStatus::~TiStatus()
{
	delete iTiles;
}

//---------------------------------------------------------------------------
void __fastcall TiStatus::show(TiProfile* aProfile)
{
	if (iIsVisible)
		return;

	iProfile = aProfile;

	iProfile->Avatar->placeTo(iScreenSize.Width / 3, iScreenSize.Height / 2);
	iProfile->Avatar->show();

	iBackground->show();
	iPanel->show();
	iButtonContinue->show();
	iButtonPrizes->show();

	TiScene::show();

}
//---------------------------------------------------------------------------
void __fastcall TiStatus::hide()
{
	if (!iIsVisible)
		return;

	iBackground->hide();
	iPanel->hide();
	iPrizes->hide();
	iButtonContinue->hide();
	iButtonPrizes->hide();

	iProfile->Avatar->hide();

	TiScene::hide();
}

//---------------------------------------------------------------------------
void __fastcall TiStatus::mouseClick(TMouseButton aButton, TShiftState aShift, int aX, int aY)
{
	if (iButtonContinue->IsVisible && iButtonContinueRect.hits(aX, aY))
	{
		if (FOnDone)
			FOnDone(this);
	}
	else if (!iPrizes->IsVisible && iButtonPrizesRect.hits(aX, aY))
	{
		iButtonContinue->hide();
		iPrizes->show();
		for (int i = 0; i < iTiles->Count; i++)
		{
			SiTile* tile = iTiles->get(i);
			int bit = 1 << i;
			if (!(iProfile->Bonus & bit))
				tile->Cover->show();
		}
	}
	else if (iPrizes->IsVisible)
	{
		if (iButtonPrizesReadyRect.hits(aX, aY))
		{
			iButtonContinue->show();
			iPrizes->hide();
			for (int i = 0; i < iTiles->Count; i++)
			{
				SiTile* tile = iTiles->get(i);
				tile->Cover->hide();
				tile->Selected->hide();
			}
		}
		else
		{
			for (int i = 0; i < iTiles->Count; i++)
			{
				SiTile* tile = iTiles->get(i);
				if (tile->Rect.hits(aX, aY))
				{
					if (!tile->Cover->IsVisible)
					{
						if (tile->Selected->IsVisible)
						{
							tile->Selected->hide();

							iProfile->SelectedPrizes = ~(~iProfile->SelectedPrizes | (1 << i));
						}
						else
						{
							tile->Selected->show();
							iProfile->SelectedPrizes = iProfile->SelectedPrizes | (1 << i);
						}
					}
					break;
				}
			}
		}
	}
}

//---------------------------------------------------------------------------
void __fastcall TiStatus::paintTo(Gdiplus::Graphics* aGraphics, EiUpdateType aUpdateType)
{
	if (!iIsVisible)
		return;

	if (aUpdateType & updStatic)
	{
		iBackground->paintTo(aGraphics);
		DrawScoreBackground(aGraphics);
		iPanel->paintTo(aGraphics);
		iButtonContinue->paintTo(aGraphics);
		iButtonPrizes->paintTo(aGraphics);
	}

	TiScene::paintTo(aGraphics, aUpdateType);

	if (aUpdateType & updNonStatic)
	{
		DrawString(aGraphics, iProfile->Name, 15, KLevelLabelsPositionY, KColorName);
		DrawString(aGraphics, String().sprintf("Taso %d", (iProfile->Level+1)), 400, KLevelLabelsPositionY, KColorName);
		DrawNumber(aGraphics, iProfile->LevelScore, 718, KLevelLabelsPositionY, KColorScore, Gdiplus::StringAlignmentFar);
		DrawString(aGraphics, WideString(L"/"), 710, KLevelLabelsPositionY, KColorScore);
		DrawNumber(aGraphics, iProfile->LevelScoreMax, 720, KLevelLabelsPositionY, KColorScore);
		DrawNumber(aGraphics, iProfile->Saldo, 1268, KLevelLabelsPositionY, KColorSaldo);

		iProfile->Avatar->paintTo(aGraphics);
		iPrizes->paintTo(aGraphics);

		for (int i = 0; i < iTiles->Count; i++)
		{
			SiTile* tile = iTiles->get(i);
			tile->Cover->paintTo(aGraphics);
			tile->Selected->paintTo(aGraphics);
		}
	}
}

//---------------------------------------------------------------------------
void __fastcall TiStatus::DrawNumber(Gdiplus::Graphics* aGraphics, int aNumber, int aX, int aY,
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
void __fastcall TiStatus::DrawString(Gdiplus::Graphics* aGraphics, const WideString& aText, int aX, int aY,
		const Gdiplus::Color& aColor, Gdiplus::StringAlignment aAlignment)
{
	WCHAR string[256];
	wcscpy(string, aText.c_bstr());

	Gdiplus::Font font(KFontName, KFontSize, KFontStyle, KFontUnits);
	Gdiplus::PointF origin(Offset.x + aX, Offset.y + aY);
	Gdiplus::SolidBrush brush(aColor);
	Gdiplus::StringFormat format;
	format.SetAlignment(aAlignment);

	aGraphics->DrawString(string, wcslen(string), &font, origin,	&format, &brush);
}

//---------------------------------------------------------------------------
void __fastcall TiStatus::DrawScoreBackground(Gdiplus::Graphics* aGraphics)
{
	double rate = double(iProfile->LevelScore) / iProfile->LevelScoreMax;
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

