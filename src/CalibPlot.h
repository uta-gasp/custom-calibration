//---------------------------------------------------------------------------
#ifndef CalibPlotH
#define CalibPlotH

//---------------------------------------------------------------------------
#include <dynarray.h>

//---------------------------------------------------------------------------
#include "Animation.h"

#ifndef RET_SUCCESS
#include "myGazeAPI.h"
#endif

//---------------------------------------------------------------------------
class TiCalibPlot : public TObject
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
		TRect iBox;
		TiAnimation* iBackground;
		TiAnimation* iPlot;
		TiAnimation* iRestart;
		TiAnimation* iClose;

		float iScaleX;
		float iScaleY;

		TiCalibQualityData iCalibQualityDataLeft;
		TiCalibQualityData iCalibQualityDataRight;
		CalibrationPointQualityStruct* iFocused;

		bool iVisible;

		TNotifyEvent FOnHidden;

		void __fastcall onBackgroundFadingFinished(TObject* aSender);

		void __fastcall SetVisible(bool aValue);
		void __fastcall DrawQualityData(Gdiplus::Graphics* aGraphics);
		Point __fastcall GetWorstPoint();

	public:
		__fastcall TiCalibPlot(TiAnimationManager* aManager, TRect aBox, int aWindowWidth, int aWindowHeight);
		__fastcall ~TiCalibPlot();

		void __fastcall reset();
		void __fastcall add(int aNumber,
				CalibrationPointQualityStruct& aLeft,
				CalibrationPointQualityStruct& aRight);

		CalibrationPointQualityStruct* __fastcall calibPointHitTest(int aX, int aY);

		void __fastcall paintTo(Gdiplus::Graphics* aGraphics);

		void __fastcall log(TStrings* aList);

		__property TRect Rect = {read = iBox};
		__property TiAnimation* Background = {read = iBackground};
		__property TiAnimation* Plot = {read = iPlot};
		__property TiAnimation* Restart = {read = iRestart};
		__property TiAnimation* Close = {read = iClose};

		__property TiCalibPlot::Point WorstPoint = {read = GetWorstPoint};

		__property bool IsVisible = {read = iVisible, write = SetVisible};

		__property TNotifyEvent OnHidden = {read = FOnHidden, write = FOnHidden};
};

//---------------------------------------------------------------------------
#endif
