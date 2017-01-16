//---------------------------------------------------------------------------
#include "CalibPlot.h"
#include "assets.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
const float KTargetSize = 6.f;
const int KPointHitSize = 16;

const Gdiplus::Color KDeviationColor(208,208,208);
const Gdiplus::Color KDeviationColorFocused(230,224,224);
const Gdiplus::Color KTargetColor(0,0,0);
const Gdiplus::Color KGazeColor(208,64,0);

//---------------------------------------------------------------------------
__fastcall TiCalibPlot::TiCalibPlot(TiAnimationManager* aManager, TRect aBox,
		int aWindowWidth, int aWindowHeight) :
		TObject(),
		iFocused(NULL),
		iVisible(false),
		FOnHidden(NULL)
{
	iBox = aBox;

	iScaleX = double(iBox.Width()) / aWindowWidth;
	iScaleY = double(iBox.Height()) / aWindowHeight;

	iBackground = new TiAnimation(false);
	iBackground->addFrames(IDR_CALIBPLOT, 160, 120);
	iBackground->placeTo(aBox.Left + aBox.Width()/2, aBox.Top + aBox.Height()/2);
	iBackground->OnFadingFinished = onBackgroundFadingFinished;

	iPlot = new TiAnimation(false);
	iPlot->LoopAnimation = false;

	iRestart = new TiAnimation(false);
	iRestart->addFrames(IDR_RESTART, 120, 48);
	iRestart->addFrames(IDR_RESTART_HOVER, 120, 48);
	iRestart->placeTo(
			iBackground->X - iRestart->Width/2 - 4,
			iBackground->Y + iBackground->Height/2 + 4 + iRestart->Height/2
	);

	iClose = new TiAnimation(false);
	iClose->addFrames(IDR_CLOSE, 120, 48);
	iClose->addFrames(IDR_CLOSE_HOVER, 120, 48);
	iClose->placeTo(
			iBackground->X + iClose->Width/2 + 4,
			iBackground->Y + iBackground->Height/2 + 4 + iClose->Height/2
	);

	if (aManager)
	{
		aManager->add(iBackground);
		aManager->add(iPlot);
		aManager->add(iRestart);
		aManager->add(iClose);
	}

	iCalibQualityData.DeleteContent = true;
}

//---------------------------------------------------------------------------
__fastcall TiCalibPlot::~TiCalibPlot()
{
}

//---------------------------------------------------------------------------
void __fastcall TiCalibPlot::reset()
{
	iCalibQualityData.clear();
}

//---------------------------------------------------------------------------
void __fastcall TiCalibPlot::add(CalibrationPointQualityStruct* aCalibPointQuality)
{
	iCalibQualityData.add(aCalibPointQuality);
	iPlot->invalidate();
}

//---------------------------------------------------------------------------
CalibrationPointQualityStruct* __fastcall TiCalibPlot::calibPointHitTest(int aX, int aY)
{
	if (!iVisible)
		return NULL;
		
	CalibrationPointQualityStruct* result = NULL;
	for (int i = 0; i < iCalibQualityData.Count; i++)
	{
		CalibrationPointQualityStruct* point = iCalibQualityData[i];
		int x = iBox.Left + iScaleX * point->positionX;
		int y = iBox.Top + iScaleY * point->positionY;
		if (aX > x - KPointHitSize && aX < x + KPointHitSize &&
				aY > y - KPointHitSize && aY < y + KPointHitSize)
		{
			result = point;
			break;
		}
	}

	if (iFocused != result)
	{
		iFocused = result;
		iPlot->invalidate();
	}

	return iFocused;
}

//---------------------------------------------------------------------------
void __fastcall TiCalibPlot::paintTo(Gdiplus::Graphics* aGraphics)
{
	iBackground->paintTo(aGraphics);
	iPlot->paintTo(aGraphics);
	iRestart->paintTo(aGraphics);
	iClose->paintTo(aGraphics);

	DrawQualityData(aGraphics);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TiCalibPlot::onBackgroundFadingFinished(TObject* aSender)
{
	if (!iBackground->IsVisible && FOnHidden)
		FOnHidden(this);
}

//---------------------------------------------------------------------------
void __fastcall TiCalibPlot::SetVisible(bool aValue)
{
	if (iVisible && !aValue)
	{
		iBackground->fadeOut();
		iPlot->fadeOut();
		iRestart->fadeOut();
		iClose->fadeOut();
	}
	else if (!iVisible && aValue)
	{
		iBackground->fadeIn();
		iPlot->fadeIn();
		iRestart->fadeIn();
		iClose->fadeIn();
	}

	iVisible = aValue;
}

//---------------------------------------------------------------------------
void __fastcall TiCalibPlot::DrawQualityData(Gdiplus::Graphics* aGraphics)
{
	if (!iPlot->IsVisible)
		return;

	Gdiplus::SolidBrush target(Gdiplus::Color(255 * iPlot->Opacity,
			KTargetColor.GetR(), KTargetColor.GetG(), KTargetColor.GetB())
	);
	Gdiplus::SolidBrush gaze(Gdiplus::Color(255 * iPlot->Opacity,
			KGazeColor.GetR(), KGazeColor.GetG(), KGazeColor.GetB())
	);

	for (int i = 0; i < iCalibQualityData.Count; i++)
	{
		CalibrationPointQualityStruct* point = iCalibQualityData[i];
		if (point->usageStatus == 0)
		{
			Gdiplus::SolidBrush* deviation;
			if (point == iFocused)
					deviation = new Gdiplus::SolidBrush(Gdiplus::Color(255 * iPlot->Opacity,
							KDeviationColorFocused.GetR(),
							KDeviationColorFocused.GetG(),
							KDeviationColorFocused.GetB()
					));
			else
					deviation = new Gdiplus::SolidBrush(Gdiplus::Color(255 * iPlot->Opacity,
							KDeviationColor.GetR(),
							KDeviationColor.GetG(),
							KDeviationColor.GetB()
					));

			aGraphics->FillEllipse(deviation,
					float(iBox.Left + iScaleX * point->positionX - point->standardDeviationX),
					float(iBox.Top + iScaleY * point->positionY - point->standardDeviationY),
					2.f * point->standardDeviationX,
					2.f * point->standardDeviationY
			);
			aGraphics->FillEllipse(&gaze,
					iBox.Left + iScaleX * point->correctedPorX - KTargetSize / 2,
					iBox.Top + iScaleY * point->correctedPorY - KTargetSize / 2,
					KTargetSize, KTargetSize
			);

			delete deviation;
		}
		aGraphics->FillEllipse(&target,
				iBox.Left + iScaleX * point->positionX - KTargetSize / 2,
				iBox.Top + iScaleY * point->positionY - KTargetSize / 2,
				KTargetSize, KTargetSize
		);
	}
}

