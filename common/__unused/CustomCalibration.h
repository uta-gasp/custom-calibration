//---------------------------------------------------------------------------
#ifndef CustomCalibrationH
#define CustomCalibrationH

//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>

//---------------------------------------------------------------------------
#include "XML.h"

//---------------------------------------------------------------------------
class TfrmCustomCalibration : public TForm
{
	__published:
		TTimer *tmrKostyl;
		void __fastcall FormCreate(TObject *Sender);
		void __fastcall FormDestroy(TObject *Sender);
		void __fastcall FormMouseUp(TObject *Sender, TMouseButton Button,
					TShiftState Shift, int X, int Y);
		void __fastcall FormKeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
		void __fastcall tmrKostylTimer(TObject *Sender);
		void __fastcall FormMouseMove(TObject *Sender, TShiftState Shift, int X,
          int Y);

	public:
		typedef void __fastcall (__closure *FiOnEvent)(System::TObject* aSender, const String& aMsg);
		typedef void __fastcall (__closure *FiOnSample)(System::TObject* aSender, double aX, double aY);
		typedef void __fastcall (__closure *FiOnCalibrationPoint)(System::TObject* aSender,
				int aPointIndex, bool aIsSinglePointMode = false);

	private:
		Gdiplus::Graphics* iGraphics;

		TiAnimationManager* iObjects;

		TiTimeout* iTimeout;

		FiOnEvent FOnEvent;
		FiOnSample FOnSample;
		TNotifyEvent FOnStart;
		TNotifyEvent FOnReadyToCalibrate;
		FiOnCalibrationPoint FOnRecalibrateSinglePoint;
		FiOnCalibrationPoint FOnPointReady;
		FiOnCalibrationPoint FOnPointAccepted;
		FiOnCalibrationPoint FOnPointAborted;
		TNotifyEvent FOnFinished;
		TNotifyEvent FOnAborted;
		TNotifyEvent FOnGameStarted;
		TNotifyEvent FOnGameFinished;

		void __fastcall onObjectPaint(TObject* aSender, EiUpdateType aUpdateType);
		void __fastcall onEyeBoxFadingTransition(TObject* aSender, double iAlpha);
		void __fastcall onEyeBoxHidden(TObject* aSender);
		void __fastcall onAttractorFadingFisnihed(TObject* aSender);
		void __fastcall onAttractorMoveFisnihed(TObject* aSender);
		void __fastcall onCalibPointTimeout(TObject* aSender);
		void __fastcall onBackgroundFadingFisnihed(TObject* aSender);
		void __fastcall onGameSelect(TObject* aSender, int aTargetX, int aTargetY);
		void __fastcall onGameFisnihed(TObject* aSender);

		void __fastcall StartCalibration();
		void __fastcall RestartCalibration(int aRecalibrationPointNumber = -1);
		void __fastcall PointDone(TObject* aSender = NULL);
		void __fastcall PointAbort(TObject* aSender = NULL);
		void __fastcall MoveToNextPoint(int aPointNumber);
		void __fastcall Abort();
		void __fastcall Finish();

		void __fastcall ShowGameInstructions(TObject* aSender = NULL);
		void __fastcall StartGame(TObject* aSender = NULL);
		void __fastcall FadeOut(TObject* aSender = NULL);
		void __fastcall Done(TObject* aSender = NULL);

		void __fastcall SetOnEvent(FiOnEvent aFOnEvent);

	public:
		__fastcall TfrmCustomCalibration(TComponent* aOwner);
		__fastcall ~TfrmCustomCalibration();

		void __fastcall setSample(SampleStruct& aSample);
		
		int  __fastcall showModal(bool aSkipCalibration = false);

		void __fastcall clearPoints();
		void __fastcall addPoint(CalibrationPointStruct& aPoint);
		void __fastcall nextPoint(int aPointNumber);
		void __fastcall reportCalibrationResult(int aNumber,
				CalibrationPointQualityStruct& aLeft,
				CalibrationPointQualityStruct& aRight);
		bool __fastcall processCalibrationResult();

		void __fastcall loadSettings(TiXML_INI* aStorage);
		void __fastcall saveSettings(TiXML_INI* aStorage);

		__property FiOnEvent OnEvent = {read = FOnEvent, write = SetOnEvent};
		__property FiOnSample OnSample = {read = FOnSample, write = FOnSample};
};

//---------------------------------------------------------------------------
#endif
