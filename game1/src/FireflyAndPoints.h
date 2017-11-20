//---------------------------------------------------------------------------
#ifndef FireflyAndPointsH
#define FireflyAndPointsH

//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>

#include "Animation.h"
#include "EyeBoxWithInstructions.h"
#include "CalibQuality.h"
#include "Lamps.h"
#include "OlioHunting.h"
#include "CalibQualityEstimator.h"
#include "utils.h"

#include "XML.h"
#include <ExtCtrls.hpp>

//---------------------------------------------------------------------------
class TiFireflyAndPoints : public TForm
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

		enum EiAttractorType {
			atFirefly = 0,
			atCircle
		};

	private:
		Gdiplus::Graphics* iGraphics;
		//Gdiplus::Bitmap* iBackground;
		Gdiplus::Bitmap* iStaticBitmap;

		FireflyAndPoints::TiEyeBoxWithInstructions* iEyeBox;
		FireflyAndPoints::TiOlioHunting* iGame;

		TiAnimation* iBackground;
		TiAnimation* iTarget;

		TiAnimation* iFireFly;
		TiAnimation* iCircle;
		TiAnimation* iAttractor;

		TiAnimationManager* iObjects;

		Gdiplus::Color iBackgroundColor;
		Gdiplus::Color iSetupBackgroundColor;
		Gdiplus::Color iCalibrationBackgroundColor;

		TiTimeout* iTimeout;
		TiTimeout* iPointAcceptTimeout;
		bool iIsWaitingToAcceptPoint;
		bool iLastPointAborted;
		bool iGameAfterCalibration;
		bool iGazeControlInGame;
		TPoint iMouseInitialPosition;

		FireflyAndPoints::TiLamps* iCalibPoints;

		TiCalibQuality* iCalibQuality;
		TiCalibQualityEstimator* iCalibQualityEstimator;

		EiAttractorType iAttractorType;

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
		__fastcall TiFireflyAndPoints(TComponent* aOwner);
		__fastcall ~TiFireflyAndPoints();

		void __fastcall setSample(SampleStruct& aSample);
		void __fastcall setTrackingStability(bool aStable);

		int  __fastcall showModal(bool aSkipCalibration = false);

		void __fastcall showEyeBox();
		void __fastcall clearPoints();
		void __fastcall addPoint(CalibrationPointStruct& aPoint);
		void __fastcall nextPoint(int aPointNumber);
		void __fastcall reportCalibrationResult(int aNumber,
				CalibrationPointQualityStruct& aLeft,
				CalibrationPointQualityStruct& aRight);
		bool __fastcall processCalibrationResult();
		void __fastcall playGame(TObject* aSender = NULL);

		void __fastcall loadSettings(TiXML_INI* aStorage);
		void __fastcall saveSettings(TiXML_INI* aStorage);

		__property bool GameAfterCalibration = {read = iGameAfterCalibration, write = iGameAfterCalibration};
		__property bool GazeControlInGame = {read = iGazeControlInGame, write = iGazeControlInGame};
		__property EiAttractorType AttractorType = {read = iAttractorType, write = iAttractorType};

		__property FiOnEvent OnEvent = {read = FOnEvent, write = SetOnEvent};
		__property FiOnSample OnSample = {read = FOnSample, write = FOnSample};
		__property TNotifyEvent OnStart = {read = FOnStart, write = FOnStart};
		__property TNotifyEvent OnReadyToCalibrate = {read = FOnReadyToCalibrate, write = FOnReadyToCalibrate};
		__property FiOnCalibrationPoint OnRecalibrateSinglePoint = {read = FOnRecalibrateSinglePoint, write = FOnRecalibrateSinglePoint};
		__property FiOnCalibrationPoint OnPointReady = {read = FOnPointReady, write = FOnPointReady};
		__property FiOnCalibrationPoint OnPointAccepted = {read = FOnPointAccepted, write = FOnPointAccepted};
		__property FiOnCalibrationPoint OnPointAborted = {read = FOnPointAborted, write = FOnPointAborted};
		__property TNotifyEvent OnFinished = {read = FOnFinished, write = FOnFinished};
		__property TNotifyEvent OnAborted = {read = FOnAborted, write = FOnAborted};
		__property TNotifyEvent OnGameStarted = {read = FOnGameStarted, write = FOnGameStarted};
		__property TNotifyEvent OnGameFinished = {read = FOnGameStarted, write = FOnGameFinished};
};

//---------------------------------------------------------------------------
#endif
