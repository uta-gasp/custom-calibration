//---------------------------------------------------------------------------
#ifndef GAME1_CalibPointsH
#define GAME1_CalibPointsH

//---------------------------------------------------------------------------
#include <dynarray.h>

//---------------------------------------------------------------------------
#include "Lamp.h"

#ifndef RET_SUCCESS
#include "myGazeAPI.h"
#endif

//---------------------------------------------------------------------------
namespace FireflyAndPoints {

//---------------------------------------------------------------------------
class TiLamps
{
	private:
		typedef TiDynArray<TiLamp> TiPoints;

		TiAnimationManager* iAnimationManager;

		TiPoints iPoints;
		int iCurrentPointIndex;
		bool iIsSinglePointMode;

		int __fastcall GetCount();
		TiLamp* __fastcall GetCurrent();
		void __fastcall Rearrange();

	public:
		__fastcall TiLamps(TiAnimationManager* aManager, int aWindowWidth, int aWindowHeight);

		void __fastcall lightOn();
		void __fastcall lightOff();
		void __fastcall lightOnCurrent();
		void __fastcall lightOffCurrent();
		void __fastcall fadeOut();

		void __fastcall clear();
		void __fastcall add(CalibrationPointStruct& aPoint);
		void __fastcall prepare(int aCalibPointNumber = -1);

    void __fastcall accept();
		TiLamp* __fastcall next(int aPointNumber = 0);

		void __fastcall paintTo(Gdiplus::Graphics* aGraphics, EiUpdateType aUpdateType);

		bool __fastcall canRecalibratePoint(int aIndex);
		int __fastcall pointRecalibrationCount(int aIndex);

		__property int Count = {read = GetCount};
		__property TiLamp* Current = {read = GetCurrent};
		__property int CurrentPointIndex = {read = iCurrentPointIndex};
		__property bool IsSinglePointMode = {read = iIsSinglePointMode};
};

//---------------------------------------------------------------------------
};	// namespace

#endif
