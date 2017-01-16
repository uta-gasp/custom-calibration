//---------------------------------------------------------------------------
#ifndef CalibPointsH
#define CalibPointsH

//---------------------------------------------------------------------------
#include <dynarray.h>

//---------------------------------------------------------------------------
#include "CalibPoint.h"

//---------------------------------------------------------------------------
class TiCalibPoints
{
	private:
		typedef TiDynArray<TiCalibPoint> TiPoints;

		TiPoints iPoints;

		int iCurrentPointIndex;
		bool iIsSinglePointMode;

		int __fastcall GetCount();
		void __fastcall Rearrange();

	public:
		__fastcall TiCalibPoints(TiAnimationManager* aManager, int aWindowWidth, int aWindowHeight);

		void __fastcall lightOn();
		void __fastcall lightOff();
		void __fastcall lightOnCurrent();
		void __fastcall lightOffCurrent();
		void __fastcall prepare(int aCalibPointIndex = -1);

		TiCalibPoint* __fastcall TiCalibPoints::next();

		void __fastcall paintTo(Gdiplus::Graphics* aGraphics);

		TiCalibPoint* operator[](int aIndex);

		__property int Count = {read = GetCount};
		__property int CurrentPointIndex = {read = iCurrentPointIndex};
		__property bool IsSinglePointMode = {read = iIsSinglePointMode};
};

//---------------------------------------------------------------------------
#endif
