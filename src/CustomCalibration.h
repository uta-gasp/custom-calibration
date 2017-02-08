//---------------------------------------------------------------------------
#ifndef CustomCalibrationH
#define CustomCalibrationH

//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>

#include "Animation.h"
#include "EyeBox.h"
#include "CalibPlot.h"
#include "CalibPoints.h"
#include "Game.h"
#include "utils.h"

#include "XML.h"
#include <ExtCtrls.hpp>

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
		typedef void __fastcall (__closure *FiOnDebug)(System::TObject* aSender, const char* aMsg);
		typedef void __fastcall (__closure *FiOnCalibrationPoint)(System::TObject* aSender,
				int aPointIndex, bool aIsSinglePointMode = false);

	private:
		Gdiplus::Graphics* iGraphics;
		//Gdiplus::Bitmap* iBackground;
		Gdiplus::Bitmap* iStaticBitmap;

		TiEyeBox* iEyeBox;
		TiCalibPlot* iCalibPlot;
		TiGame* iGame;
		TiAnimationManager* iObjects;
		TiAnimation* iBackground;
		TiAnimation* iTarget;
		TiAnimation* iFireFly;
		TiTimeout* iTimeout;
		TiTimeout* iPointAcceptTimeout;
		bool iIsWaitingToAcceptPoint;
		bool iLastPointAborted;
		bool iGameAfterCalibration;
		bool iGazeControlInGame;
		TPoint iMouseInitialPosition;

		TiCalibPoints* iCalibPoints;

		FiOnDebug FOnDebug;
		TNotifyEvent FOnStart;
		TNotifyEvent FOnReadyToCalibrate;
		FiOnCalibrationPoint FOnRecalibrateSinglePoint;
		FiOnCalibrationPoint FOnPointReady;
		FiOnCalibrationPoint FOnPointAccepted;
		FiOnCalibrationPoint FOnPointAborted;
		TNotifyEvent FOnFinished;
		TNotifyEvent FOnAborted;

		void __fastcall onObjectPaint(TObject* aSender, EiUpdateType aUpdateType);
		void __fastcall onEyeBoxHidden(TObject* aSender);
		void __fastcall onFireFlyFadingFisnihed(TObject* aSender);
		void __fastcall onFireFlyMoveFisnihed(TObject* aSender);
		void __fastcall onCalibPointTimeout(TObject* aSender);
		void __fastcall onBackgroundFadingFisnihed(TObject* aSender);
		void __fastcall onGameFisnihed(TObject* aSender);

		void __fastcall StartCalibration();
		void __fastcall RestartCalibration(int aRecalibrationPointNumber = -1);
		void __fastcall PointDone(TObject* aSender = NULL);
		void __fastcall PointAbort(TObject* aSender = NULL);
		void __fastcall MoveToNextPoint(int aPointNumber);
		void __fastcall Abort();
		void __fastcall Finish();
		void __fastcall StartGame(TObject* aSender = NULL);
		void __fastcall FadeOut(TObject* aSender = NULL);
		void __fastcall Done(TObject* aSender = NULL);

	public:
		__fastcall TfrmCustomCalibration(TComponent* aOwner);
		__fastcall ~TfrmCustomCalibration();

		void __fastcall setSample(SampleStruct& aSample);
		void __fastcall setTrackingStability(bool aStable);

		void __fastcall clearPoints();
		void __fastcall addPoint(CalibrationPointStruct& aPoint);
		void __fastcall nextPoint(int aPointNumber);
		void __fastcall reportCalibrationResult(int aNumber,
				CalibrationPointQualityStruct& aLeft,
				CalibrationPointQualityStruct& aRight);
		bool __fastcall processCalibrationResult();

		void __fastcall loadSettings(TiXML_INI* aStorage);
		void __fastcall saveSettings(TiXML_INI* aStorage);

		__property bool GameAfterCalibration = {read = iGameAfterCalibration, write = iGameAfterCalibration};
		__property bool GazeControlInGame = {read = iGazeControlInGame, write = iGazeControlInGame};

		__property FiOnDebug OnDebug = {read = FOnDebug, write = FOnDebug};
		__property TNotifyEvent OnStart = {read = FOnStart, write = FOnStart};
		__property TNotifyEvent OnReadyToCalibrate = {read = FOnReadyToCalibrate, write = FOnReadyToCalibrate};
		__property FiOnCalibrationPoint OnRecalibrateSinglePoint = {read = FOnRecalibrateSinglePoint, write = FOnRecalibrateSinglePoint};
		__property FiOnCalibrationPoint OnPointReady = {read = FOnPointReady, write = FOnPointReady};
		__property FiOnCalibrationPoint OnPointAccepted = {read = FOnPointAccepted, write = FOnPointAccepted};
		__property FiOnCalibrationPoint OnPointAborted = {read = FOnPointAborted, write = FOnPointAborted};
		__property TNotifyEvent OnFinished = {read = FOnFinished, write = FOnFinished};
		__property TNotifyEvent OnAborted = {read = FOnAborted, write = FOnAborted};
};

//---------------------------------------------------------------------------
#endif
