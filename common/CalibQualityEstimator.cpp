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
double TiQualityMetric::CM_PER_INCH = 2.54;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
String SiQuality::toString(const char* aHeader)
{
	return String().sprintf("%s\t%d %d\t%.3f %.3f %.3f\t%.3f %.3f %.3f", aHeader, X, Y,
					Precision.pixels(), Precision.cm(), Precision.deg(60),
					Accuracy.pixels(), Accuracy.cm(), Accuracy.deg(60)
	);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall TiCalibQualityEstimator::TiCalibQualityEstimator() :
		iBufferLength(12),
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
	FOnEvent(this, String().sprintf("add selection point\t%d %d",
			aTargetX, aTargetY
	));

	TiPointD fixation;
	if (!GetAverage(fixation, iGazePoints))
		return false;

	FOnEvent(this, String().sprintf("fixation:\t%.2f %.2f",
			fixation.X, fixation.Y
	));

	TiPointD offset = TiPointD(aTargetX, aTargetY) - fixation;

	FOnEvent(this, String().sprintf("offset:\t%.2f %.2f",
			offset.X, offset.Y
	));

	TiPointD sd;
	if (!GetStandardDeviation(sd, iGazePoints, &fixation))
		return false;

	FOnEvent(this, String().sprintf("sd:\t%.2f %.2f",
			sd.X, sd.Y
	));

	SiQuality* quality = new SiQuality(
			aTargetX, aTargetY,
			TiQualityMetric(sd.length()),
			TiQualityMetric(offset.length())
	);

	FOnEvent(this, String().sprintf("SUMMARY:\t%.2f\t%.2f",
			offset.length(), sd.length()
	));

	iQualities->add(quality);

	return true;
}

//---------------------------------------------------------------------------
int __fastcall TiCalibQualityEstimator::estimate(SiQuality& aQuality)
{
	for (int i = 0; i < iQualities->Count; i++)
	{
		SiQuality* q = iQualities->get(i);
		FOnEvent(this, q->toString("verification point quality"));
	}

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
	if (!iGazePoints.size())
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

	try {
		aResult = TiPointD(sqrt(x / count), sqrt(y / count));
	} catch (...) {
		MessageBox(NULL, "Oops2", "KC", MB_OK);
	}

	return true;
}

//---------------------------------------------------------------------------
TiCalibQualityEstimator::TiPointD __fastcall TiCalibQualityEstimator::GetPoint()
{
	TiPointD fixation;
	GetAverage(fixation, iGazePoints);

	return fixation;
}
