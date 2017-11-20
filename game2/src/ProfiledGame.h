//---------------------------------------------------------------------------
#ifndef ProfilesGameH
#define ProfilesGameH

//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>

//---------------------------------------------------------------------------
#include "Login.h"
#include "InstructionSitting.h"
#include "InstructionStart.h"
#include "Profile.h"
#include "ProfileEditor.h"
#include "Animation.h"
#include "EyeBox.h"
#include "Factory.h"
#include "GhostBox.h"
#include "Door.h"
#include "InstructionColor.h"
#include "InstructionShape.h"
#include "InstructionCalibrate.h"
#include "CalibPoints.h"
#include "GameResult.h"
#include "Rewards.h"
#include "Status.h"
#include "PositionTracker.h"

#include "CalibQuality.h"
#include "CalibQualityEstimator.h"
#include "utils.h"

#include "XML.h"

//---------------------------------------------------------------------------
class TiProfiledGame : public TForm
{
	__published:
		TTimer *tmrKostyl;
	TPanel *pnlNameContainer;
	TMemo *memName;
		void __fastcall FormCreate(TObject *Sender);
		void __fastcall FormDestroy(TObject *Sender);
		void __fastcall FormMouseUp(TObject *Sender, TMouseButton Button,
					TShiftState Shift, int X, int Y);
		void __fastcall FormKeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
		void __fastcall tmrKostylTimer(TObject *Sender);
	void __fastcall memNameKeyDown(TObject *Sender, WORD &Key,
					TShiftState Shift);
	void __fastcall memNameKeyPress(TObject *Sender, char &Key);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
					TShiftState Shift);

	public:
		typedef void __fastcall (__closure *FiOnEvent)(System::TObject* aSender, const String& aMsg);
		typedef void __fastcall (__closure *FiOnSample)(System::TObject* aSender, double aX, double aY);
		typedef void __fastcall (__closure *FiOnCalibrationPoint)(System::TObject* aSender,
				int aPointIndex, bool aIsSinglePointMode = false);

	private:
		Gdiplus::Graphics* iGraphics;
		Gdiplus::Bitmap* iStaticBitmap;

		TiAnimation* iBackground;

		ProfiledGame::TiLogin* iLogin;
		ProfiledGame::TiProfileEditor* iProfileEditor;
		ProfiledGame::TiInstructionSitting* iInstructionSitting;
		ProfiledGame::TiInstructionStart* iInstructionStart;
		ProfiledGame::TiEyeBox* iEyeBox;
		ProfiledGame::TiFactory* iFactory;
		ProfiledGame::TiGhostBox* iGhostBox;
		ProfiledGame::TiDoor* iDoor;
		ProfiledGame::TiInstructionColor* iInstructionColor;
		ProfiledGame::TiInstructionShape* iInstructionShape;
		ProfiledGame::TiInstructionCalibrate* iInstructionCalibrate;
		ProfiledGame::TiCalibPoints* iCalibPoints;
		ProfiledGame::TiGameResult* iGameResult;
		ProfiledGame::TiRewards* iRewards;
		ProfiledGame::TiStatus* iStatus;
		ProfiledGame::TiPositionTracker* iPositionTracker;

		ProfiledGame::TiLevelInstruction* iLevelInstruction;
		ProfiledGame::TiLevelLegend* iLevelLegend;

		TiAnimationManager* iObjects;

		TiTimeout* iTimeout;
		bool iIsVerifying;
		bool iIsRecalibrating;
		int iLastPointID;
		int iNextPointID;
		int iTargetID;
		int iCalibrationLevel;

		ProfiledGame::TiProfile* iProfile;

		TiCalibQuality* iCalibQuality;
		TiCalibQualityEstimator* iCalibQualityEstimator;

		FiOnEvent FOnEvent;
		FiOnSample FOnSample;
		TNotifyEvent FOnStart;
		TNotifyEvent FOnReadyToCalibrate;
		FiOnCalibrationPoint FOnRecalibratePoint;
		FiOnCalibrationPoint FOnPointReady;
		FiOnCalibrationPoint FOnPointAccepted;
		TNotifyEvent FOnFinished;
		TNotifyEvent FOnAborted;
		TNotifyEvent FOnVerifStarted;
		TNotifyEvent FOnVerifFinished;

		void __fastcall onObjectPaint(TObject* aSender, EiUpdateType aUpdateType);
		void __fastcall onCalibPointReady(TObject* aSender);

		void __fastcall onLoginDone(TObject* aSender = NULL);
		void __fastcall onProfileEditorDone(TObject* aSender = NULL);
		void __fastcall onInstructionSittingDone(TObject* aSender = NULL);
		void __fastcall onEyeBoxDone(TObject* aSender = NULL);
		void __fastcall AfterEyeBox(TObject* aSender = NULL);
		void __fastcall onInstructionStartDone(TObject* aSender = NULL);
		void __fastcall onLevelLegendDone(TObject* aSender = NULL);
		void __fastcall AfterLevelLegend(TObject *Sender);
		void __fastcall onDoorDone(TObject* aSender);
		void __fastcall AfterDoor(TObject *Sender);
		void __fastcall onLevelInstructionDone(TObject* aSender = NULL);
		void __fastcall onInstructionCalibrateDone(TObject* aSender = NULL);
		void __fastcall onCalibPointsDone(TObject* aSender = NULL);
		void __fastcall AfterCalibPoints(TObject *Sender);
		void __fastcall onGameResultDone(TObject* aSender = NULL);
		void __fastcall DelayedRewards(TObject *Sender);
		void __fastcall onRewardsDone(TObject* aSender = NULL);
		void __fastcall onStatusDone(TObject* aSender = NULL);
		void __fastcall onPositionTrackerDone(TObject* aSender = NULL);

		void __fastcall StartCalibration(TObject* aSender = NULL);
		void __fastcall StartVerification(TObject* aSender = NULL);
		void __fastcall NextPoint(int aPointNumber);
		void __fastcall RecalibratePoint(int aPointNumber);
		void __fastcall PointDone(TObject* aSender = NULL);
		void __fastcall AfterPointDone(TObject* aSender = NULL);
		void __fastcall PointAbort(TObject* aSender = NULL);
		void __fastcall Abort(String aMessage = "");

		void __fastcall ReportPointAcceptance();
		void __fastcall AddVerificationPoints();
		void __fastcall EstimateVerificationQuality();

		void __fastcall Done(TObject* aSender = NULL);

		void __fastcall SetOnEvent(FiOnEvent aFOnEvent);

	public:
		__fastcall TiProfiledGame(TComponent* aOwner);
		__fastcall ~TiProfiledGame();

		void __fastcall setSample(SampleStruct& aSample);

		int  __fastcall showModal(String& aName = String());
		void __fastcall showLogin(String& aName = String());			 		// debug only

		void __fastcall clearPoints();
		void __fastcall addPoint(CalibrationPointStruct& aPoint);
		void __fastcall nextPoint(int aPointID);
		void __fastcall reportCalibrationResult(int aNumber,
				CalibrationPointQualityStruct& aLeft,
				CalibrationPointQualityStruct& aRight);
		bool __fastcall processCalibrationResult();

		void __fastcall loadSettings(TiXML_INI* aStorage);
		void __fastcall saveSettings(TiXML_INI* aStorage);

		__property FiOnEvent OnEvent = {read = FOnEvent, write = SetOnEvent};
		__property FiOnSample OnSample = {read = FOnSample, write = FOnSample};
		__property TNotifyEvent OnStart = {read = FOnStart, write = FOnStart};
		__property TNotifyEvent OnReadyToCalibrate = {read = FOnReadyToCalibrate, write = FOnReadyToCalibrate};
		__property FiOnCalibrationPoint OnRecalibratePoint = {read = FOnRecalibratePoint, write = FOnRecalibratePoint};
		__property FiOnCalibrationPoint OnPointReady = {read = FOnPointReady, write = FOnPointReady};
		__property FiOnCalibrationPoint OnPointAccepted = {read = FOnPointAccepted, write = FOnPointAccepted};
		__property TNotifyEvent OnFinished = {read = FOnFinished, write = FOnFinished};
		__property TNotifyEvent OnAborted = {read = FOnAborted, write = FOnAborted};
		__property TNotifyEvent OnVerifStarted = {read = FOnVerifStarted, write = FOnVerifStarted};
		__property TNotifyEvent OnVerifFinished = {read = FOnVerifFinished, write = FOnVerifFinished};
};

//---------------------------------------------------------------------------
#endif
