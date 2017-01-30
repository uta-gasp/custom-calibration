//---------------------------------------------------------------------------
#include "CalibPlot.h"
#include "assets.h"

#include <math.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
const float KTargetSize = 6.f;
const int KPointHitSize = 16;

const Gdiplus::Color KDeviationColor(208,208,208);
const Gdiplus::Color KDeviationColorFocused(230,224,224);
const Gdiplus::Color KTargetColor(0,0,0);
const Gdiplus::Color KGazeColorLeft(208,64,0);
const Gdiplus::Color KGazeColorRight(0,64,208);

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

	iCalibQualityDataLeft.DeleteContent = true;
	iCalibQualityDataRight.DeleteContent = true;
}

//---------------------------------------------------------------------------
__fastcall TiCalibPlot::~TiCalibPlot()
{
}

//---------------------------------------------------------------------------
void __fastcall TiCalibPlot::reset()
{
	iCalibQualityDataLeft.clear();
	iCalibQualityDataRight.clear();
}

//---------------------------------------------------------------------------
void __fastcall TiCalibPlot::add(int aNumber,
		CalibrationPointQualityStruct* aLeft,
		CalibrationPointQualityStruct* aRight)
{
	iCalibQualityDataLeft.add(aLeft);
	iCalibQualityDataRight.add(aRight);
	iPlot->invalidate();
}

//---------------------------------------------------------------------------
CalibrationPointQualityStruct* __fastcall TiCalibPlot::calibPointHitTest(int aX, int aY)
{
	if (!iVisible)
		return NULL;

	CalibrationPointQualityStruct* result = NULL;
	for (int i = 0; i < iCalibQualityDataLeft.Count; i++)
	{
		CalibrationPointQualityStruct* point = iCalibQualityDataLeft[i];
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
	Gdiplus::SolidBrush gazeLeft(Gdiplus::Color(255 * iPlot->Opacity,
			KGazeColorLeft.GetR(), KGazeColorLeft.GetG(), KGazeColorLeft.GetB())
	);
	Gdiplus::SolidBrush gazeRight(Gdiplus::Color(255 * iPlot->Opacity,
			KGazeColorRight.GetR(), KGazeColorRight.GetG(), KGazeColorRight.GetB())
	);

	for (int i = 0; i < iCalibQualityDataLeft.Count; i++)
	{
		CalibrationPointQualityStruct* pointLeft = iCalibQualityDataLeft[i];
		CalibrationPointQualityStruct* pointRight = iCalibQualityDataRight[i];

		Gdiplus::SolidBrush* deviation;
		if (pointLeft == iFocused)
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
				float(iBox.Left + iScaleX * pointLeft->positionX - pointLeft->standardDeviationX),
				float(iBox.Top + iScaleY * pointLeft->positionY - pointLeft->standardDeviationY),
				2.f * pointLeft->standardDeviationX,
				2.f * pointLeft->standardDeviationY
		);
		aGraphics->FillEllipse(&gazeLeft,
				iBox.Left + iScaleX * pointLeft->correctedPorX - KTargetSize / 2,
				iBox.Top + iScaleY * pointLeft->correctedPorY - KTargetSize / 2,
				KTargetSize, KTargetSize
		);
		aGraphics->FillEllipse(&gazeRight,
				iBox.Left + iScaleX * pointRight->correctedPorX - KTargetSize / 2,
				iBox.Top + iScaleY * pointRight->correctedPorY - KTargetSize / 2,
				KTargetSize, KTargetSize
		);

		delete deviation;

		aGraphics->FillEllipse(&target,
				iBox.Left + iScaleX * pointLeft->positionX - KTargetSize / 2,
				iBox.Top + iScaleY * pointLeft->positionY - KTargetSize / 2,
				KTargetSize, KTargetSize
		);
	}
}

//---------------------------------------------------------------------------
TiCalibPlot::Point __fastcall TiCalibPlot::GetWorstPoint()
{
	TiCalibPlot::Point result;

	for (int i = 0; i < iCalibQualityDataLeft.Count; i++)
	{
		CalibrationPointQualityStruct* point = iCalibQualityDataLeft[i];
		if (point->usageStatus == 0)
		{
			double dx = point->positionX - point->correctedPorX;
			double dy = point->positionY - point->correctedPorY;
			double offset = sqrt(dx * dx + dy * dy);
			if (offset > result.Offset)
			{
				result.X = point->positionX;
				result.Y = point->positionY;
				result.ID = point->number;
				result.Offset = offset;
			}
		}
		else
		{
			result.X = point->positionX;
			result.Y = point->positionY;
			result.ID = point->number;
			result.Offset = 10000;
			return result;
		}
	}

	return result;
}

