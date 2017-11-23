//---------------------------------------------------------------------------
#include "CalibQuality.h"

//---------------------------------------------------------------------------
#include <math.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
__fastcall TiCalibQuality::TiCalibQuality() :
		TObject()
{
	iCalibQualityDataLeft.DeleteContent = true;
	iCalibQualityDataRight.DeleteContent = true;
}

//---------------------------------------------------------------------------
__fastcall TiCalibQuality::~TiCalibQuality()
{
}

//---------------------------------------------------------------------------
void __fastcall TiCalibQuality::reset()
{
	iCalibQualityDataLeft.clear();
	iCalibQualityDataRight.clear();
}

//---------------------------------------------------------------------------
void __fastcall TiCalibQuality::add(int aNumber,
		CalibrationPointQualityStruct& aLeft,
		CalibrationPointQualityStruct& aRight)
{
	CalibrationPointQualityStruct* left = new CalibrationPointQualityStruct();
	*left = aLeft;
	CalibrationPointQualityStruct* right = new CalibrationPointQualityStruct();
	*right = aRight;

	iCalibQualityDataLeft.add(left);
	iCalibQualityDataRight.add(right);
}

//---------------------------------------------------------------------------
void __fastcall TiCalibQuality::log(TStrings* aList)
{
	for (int i = 0; i < iCalibQualityDataLeft.Count; i++)
	{
		CalibrationPointQualityStruct* point = iCalibQualityDataLeft[i];
		aList->Add(String().sprintf("%d\t%d %d\t%d %d\t%d %d\t%d\t%.2f",
			point->number - 1, point->positionX, point->positionY,
			int(point->correctedPorX), int(point->correctedPorY),
			int(point->standardDeviationX), int(point->standardDeviationY),
			int(point->usageStatus), point->qualityIndex));
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TiCalibQuality::Point __fastcall TiCalibQuality::GetWorstPoint()
{
	TiCalibQuality::Point result;

	for (int i = 0; i < iCalibQualityDataLeft.Count; i++)
	{
		CalibrationPointQualityStruct* point = iCalibQualityDataLeft[i];
		if (point->usageStatus == calibrationPointUsed)
		{
			double dx = point->positionX - point->correctedPorX;
			double dy = point->positionY - point->correctedPorY;
			double offset = 999999999;
			try {
				offset = sqrt(dx * dx + dy * dy);
			} catch (...) {
				//MessageBox(NULL, "Oops1", "KC", MB_OK);
			}

			if (offset > result.Offset)
			{
				result.X = point->positionX;
				result.Y = point->positionY;
				result.ID = point->number;
				result.Quality = point->qualityIndex;
				result.Offset = offset;
			}
		}
		else
		{
			result.X = point->positionX;
			result.Y = point->positionY;
			result.ID = point->number;
			result.Quality = point->qualityIndex;
			result.Offset = 10000;
			return result;
		}
	}

	return result;
}

