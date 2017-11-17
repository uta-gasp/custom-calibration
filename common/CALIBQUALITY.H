//---------------------------------------------------------------------------
#ifndef GAME2_CalibQualityH
#define GAME2_CalibQualityH

//---------------------------------------------------------------------------
#include <classes.hpp>
#include <dynarray.h>

//---------------------------------------------------------------------------
#ifndef RET_SUCCESS
#include "myGazeAPI.h"
#endif

//---------------------------------------------------------------------------
class TiCalibQuality : public TObject
{
	public:
		typedef TiDynArray<CalibrationPointQualityStruct> TiCalibQualityData;

		struct Point
		{
			double Offset;
			double Quality;
			int X;
			int Y;
			int ID;

			Point() : Offset(0), Quality(1) { }
		};

	private:
		TiCalibQualityData iCalibQualityDataLeft;
		TiCalibQualityData iCalibQualityDataRight;

		Point __fastcall GetWorstPoint();

	public:
		__fastcall TiCalibQuality();
		__fastcall ~TiCalibQuality();

		void __fastcall reset();
		void __fastcall add(int aNumber,
				CalibrationPointQualityStruct& aLeft,
				CalibrationPointQualityStruct& aRight);

		void __fastcall log(TStrings* aList);

		__property TiCalibQuality::Point WorstPoint = {read = GetWorstPoint};
};

//---------------------------------------------------------------------------
#endif
