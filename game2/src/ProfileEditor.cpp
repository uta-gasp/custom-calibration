//---------------------------------------------------------------------------
#include "ProfileEditor.h"
#include "assets_pg.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
using namespace ProfiledGame;

//---------------------------------------------------------------------------
const int KSelectionOffsetX = 884;
const int KSelectionOffsetY = 200;
const int KSelectionCellWidth = 116;
const int KSelectionCellHeight = 116;
const int KSelectionWidth = 79;
const int KSelectionHeight = 71;

const WideString KFontName = L"Arial";
const int KFontSize = 36;
const Gdiplus::FontStyle KFontStyle = Gdiplus::FontStyleBold;
const Gdiplus::Unit KFontUnits = Gdiplus::UnitPixel;
const Gdiplus::Color KFontColor(255, 0, 128, 192);

//---------------------------------------------------------------------------
TiProfileEditor::TiProfileEditor(TiAnimationManager* aManager, TiSize aScreenSize, TiSize aViewport) :
		TiScene(aManager, aScreenSize, aViewport)
{
	iReadyButton = TiRect(Offset.x + 950, Offset.y + 667, 190, 50);

	iBackground = new TiAnimation(false, true);
	iBackground->addFrames(IDR_PROFILER_BACKGROUND, aViewport.Width, aViewport.Height);
	iBackground->placeTo(aScreenSize.Width/2, aScreenSize.Height/2);
	aManager->add(iBackground);

	iTitle = new TiAnimation(false, true);
	iTitle->addFrames(IDR_PROFILER_TITLE, aViewport.Width, aViewport.Height);
	iTitle->placeTo(aScreenSize.Width/2, aScreenSize.Height/2);
	aManager->add(iTitle);

	iButtons = new TiAnimation(false, true);
	iButtons->addFrames(IDR_PROFILER_BUTTONS, aViewport.Width, aViewport.Height);
	iButtons->placeTo(aScreenSize.Width/2, aScreenSize.Height/2);
	aManager->add(iButtons);

	for (int i = 0; i < pepCount; i++)
	{
		TiAnimation* selection = new TiAnimation(true, false);
		selection->addFrames(IDR_PROFILER_SELECTION, KSelectionWidth, KSelectionHeight);
		selection->placeTo(
				Offset.x + KSelectionOffsetX + KSelectionWidth/2,
				Offset.y + KSelectionOffsetY + KSelectionCellHeight * i + KSelectionHeight/2);
		iSelection[i] = selection;

		aManager->add(selection);

		for (int j = 0; j < pesCount; j++)
		{
			TiRect area(
					Offset.x + KSelectionOffsetX + j * KSelectionCellWidth,
					Offset.y + KSelectionOffsetY + i * KSelectionCellHeight,
					KSelectionWidth, KSelectionHeight
			);
			iCheckbox[i*pesCount + j] = new SiCheckbox(selection, area);
		}
	}
}

//---------------------------------------------------------------------------
void __fastcall TiProfileEditor::show(TiProfile* aProfile)
{
	if (iIsVisible)
		return;

	iProfile = aProfile;

	iProfile->Avatar->placeTo(iScreenSize.Width / 3, iScreenSize.Height / 2);
	iProfile->Avatar->show();

	iBackground->show();
	iTitle->show();
	iButtons->show();

	for (int i = 0; i < pepCount; i++)
		iSelection[i]->show();

	TiScene::show();
}

//---------------------------------------------------------------------------
void __fastcall TiProfileEditor::hide()
{
	if (!iIsVisible)
		return;

	iBackground->hide();
	iTitle->hide();
	iButtons->hide();
	iProfile->Avatar->hide();
	for (int i = 0; i < pepCount; i++)
		iSelection[i]->hide();

	TiScene::hide();
}

//---------------------------------------------------------------------------
void __fastcall TiProfileEditor::paintTo(Gdiplus::Graphics* aGraphics, EiUpdateType aUpdateType)
{
	if (!iIsVisible)
		return;

	if (aUpdateType & updStatic)
	{
		iBackground->paintTo(aGraphics);
		iTitle->paintTo(aGraphics);
		iButtons->paintTo(aGraphics);

		DrawName(aGraphics);
	}
	if (aUpdateType & updNonStatic)
	{
		iProfile->Avatar->paintTo(aGraphics);
		for (int i = 0; i < pepCount; i++)
			iSelection[i]->paintTo(aGraphics);
	}
}

//---------------------------------------------------------------------------
void __fastcall TiProfileEditor::mouseClick(TMouseButton aButton, TShiftState aShift, int aX, int aY)
{
	if (!iIsVisible)
		return;
		
	if (iReadyButton.hits(aX, aY))
	{
		if (FOnDone)
			FOnDone(this);
	}
	else
	{
		for (int i = 0; i < pepCount * pesCount; i++)
		{
			if (iCheckbox[i]->Area.hits(aX, aY))
			{
				int bodyPart = i / pesCount;
				int partType = i % pesCount;

				iSelection[bodyPart]->placeTo(
						Offset.x + KSelectionOffsetX + KSelectionCellWidth * partType + KSelectionWidth/2,
						iSelection[bodyPart]->Y);

				switch (bodyPart) {
					case pepSkin: iProfile->SkinColorID = partType; break;
					case pepHair: iProfile->HairColorID = partType; break;
					case pepEyes: iProfile->EyesColorID = partType; break;
					case pepShirt: iProfile->ShirtColorID = partType; break;
				}
				break;
			}
		}
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TiProfileEditor::DrawName(Gdiplus::Graphics* aGraphics)
{
	WCHAR string[256];
	wcscpy(string, iProfile->Name.c_bstr());

	Gdiplus::Font font(KFontName, KFontSize, KFontStyle, KFontUnits);
	Gdiplus::PointF origin(Offset.x + 1045, Offset.y + 104);
	Gdiplus::SolidBrush brush(KFontColor);
	Gdiplus::StringFormat format;
	format.SetAlignment(Gdiplus::StringAlignmentCenter);

	aGraphics->DrawString(string, wcslen(string), &font, origin,	&format, &brush);
}

