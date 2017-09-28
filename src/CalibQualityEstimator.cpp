//---------------------------------------------------------------------------
#include "CalibQualityEstimator.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
static HWND sDefaultWnd = ::GetDesktopWindow();
static HDC sDefaultDC = GetDC(sDefaultWnd);
int TiQualityMetric::DPI = GetDeviceCaps(sDefaultDC, LOGPIXELSX);
static _ = ::ReleaseDC(sDefaultWnd, sDefaultDC);

//---------------------------------------------------------------------------
__fastcall TiCalibQualityEstimator::TiCalibQualityEstimator() :
		iBufferLength(20),
		FOnEvent(NULL)
{
	iQualities = new TiQualities(true);
}

//---------------------------------------------------------------------------
__fastcall TiCalibQualityEstimator::~TiCalibQualityEstimator()
{
	delete iQualities;
}

//---------------------------------------------------------------------------
void __fastcall TiCalibQualityEstimator::reset()
{
	iGazePoints = TiPoints();
	iQualities->clear();
}

//---------------------------------------------------------------------------
void __fastcall TiCalibQualityEstimator::addSample(const TiPointI& aPoint)
{
	iGazePoints.push_back(aPoint);
	if (iGazePoints.size() > iBufferLength)
		iGazePoints.pop_front();
}

//---------------------------------------------------------------------------
bool __fastcall TiCalibQualityEstimator::addSelection(int aTargetX, int aTargetY)
{
	FOnEvent(this, String().sprintf("add selection point\t%d\t%d",
			aTargetX, aTargetY
	));

	TiPointD fixation;
	if (!GetAverage(fixation, iGazePoints))
		return false;

	FOnEvent(this, String().sprintf("   fixation:%.2f\t%.2f",
			fixation.X, fixation.Y
	));

	TiPointD offset = TiPointD(aTargetX, aTargetY) - fixation;

	FOnEvent(this, String().sprintf("   offset:%.2f\t%.2f",
			offset.X, offset.Y
	));

	TiPointD sd;
	if (!GetStandardDeviation(sd, iGazePoints, &fixation))
		return false;

	FOnEvent(this, String().sprintf("   sd:%.2f\t%.2f",
			sd.X, sd.Y
	));

	SiQuality* quality = new SiQuality(
			TiQualityMetric(offset.length()),
			TiQualityMetric(sd.length())
	);

	FOnEvent(this, String().sprintf("   SUMMARY:%.2f\t%.2f",
			offset.length(), sd.length()
	));

	iQualities->add(quality);

	return true;
}

//---------------------------------------------------------------------------
int __fastcall TiCalibQualityEstimator::estimate(SiQuality& aQuality)
{
	// debug
	for (int i = 0; i < iQualities->Count; i++)
	{
		SiQuality* q = iQualities->get(i);
		FOnEvent(this, String().sprintf("verification point quality\t%.3f\t%.3f",
				q->Precision.pixels(), q->Accuracy.pixels()
		));
	}
	// <---



	int count = iQualities->Count;
	if (!count)
		return 0;

	double precision = 0;
	double accuracy = 0;

	for (int i = 0; i < iQualities->Count; i++)
	{
		SiQuality* pointQuality = iQualities->get(i);
		precision += pointQuality->Precision.pixels();
		accuracy += pointQuality->Accuracy.pixels();
	}

	aQuality.Precision = TiQualityMetric(precision / count);
	aQuality.Accuracy = TiQualityMetric(accuracy / count);

	return count;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool __fastcall TiCalibQualityEstimator::GetAverage(TiPointD& aResult, const TiPoints& aPoints)
{
	if (iGazePoints.empty())
		return false;

	double x = 0;
	double y = 0;
	int count = iGazePoints.size();

	for (TiPoints::iterator it = iGazePoints.begin(); it != iGazePoints.end(); ++it)
	{
		x += it->X;
		y += it->Y;
	}

	aResult = TiPointD(x / count, y / count);
	return true;
}

//---------------------------------------------------------------------------
bool __fastcall TiCalibQualityEstimator::GetStandardDeviation(TiPointD& aResult,
	const TiPoints& aPoints, TiPointD* aMean)
{
	if (iGazePoints.empty())
		return false;

	TiPointD mean;
	if (aMean)
		mean = *aMean;
	else if (!GetAverage(mean, aPoints))
		return false;

	double x = 0;
	double y = 0;
	int count = iGazePoints.size();

	for (TiPoints::iterator it = iGazePoints.begin(); it != iGazePoints.end(); ++it)
	{
		x += pow(it->X - mean.X, 2);
		y += pow(it->Y - mean.Y, 2);
	}

	aResult = TiPointD(sqrt(x / count), sqrt(y / count));
	return true;
}