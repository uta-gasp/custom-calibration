//---------------------------------------------------------------------------
#ifndef CalibPlotH
#define CalibPlotH

//---------------------------------------------------------------------------
#include <dynarray.h>

//---------------------------------------------------------------------------
#include "Animation.h"
#include "fakeMyGaze.h"

//---------------------------------------------------------------------------
class TiCalibPlot : public TObject
{
	public:
		typedef TiDynArray<CalibrationPointQualityStruct> TiCalibQualityData;

	private:
		TRect iBox;
		TiAnimation* iBackground;
		TiAnimation* iPlot;
		TiAnimation* iRestart;
		TiAnimation* iClose;

		float iScaleX;
		float iScaleY;

		TiCalibQualityData iCalibQualityData;
		CalibrationPointQualityStruct* iFocused;

		bool iVisible;

		TNotifyEvent FOnHidden;

		void __fastcall onBackgroundFadingFinished(TObject* aSender);

		void __fastcall SetVisible(bool aValue);
		void __fastcall DrawQualityData(Gdiplus::Graphics* aGraphics);

	public:
		__fastcall TiCalibPlot(TiAnimationManager* aManager, TRect aBox, int aWindowWidth, int aWindowHeight);
		__fastcall ~TiCalibPlot();

		void __fastcall reset();
		void __fastcall add(CalibrationPointQualityStruct* aCalibPointQuality);

		CalibrationPointQualityStruct* __fastcall calibPointHitTest(int aX, int aY);

		void __fastcall paintTo(Gdiplus::Graphics* aGraphics);

		__property TRect Rect = {read = iBox};
		__property TiAnimation* Background = {read = iBackground};
		__property TiAnimation* Plot = {read = iPlot};
		__property TiAnimation* Restart = {read = iRestart};
		__property TiAnimation* Close = {read = iClose};

		__property bool IsVisible = {read = iVisible, write = SetVisible};

		__property TNotifyEvent OnHidden = {read = FOnHidden, write = FOnHidden};
};

//---------------------------------------------------------------------------
#endif
