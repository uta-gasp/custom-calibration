//---------------------------------------------------------------------------
#ifndef ControllerH
#define ControllerH

//---------------------------------------------------------------------------
#include "utils.h"

#include "FireflyAndPoints.h"
#include "ProfiledGame.h"

#include "PreInstruction.h"

//---------------------------------------------------------------------------
#define DAY_COUNT 6
#define SESSION_COUNT 3

//---------------------------------------------------------------------------
class TiController : public TObject
{
	public:
		enum EiCalibType
		{
			ctStandard = 's',
			ctFirefly = 'f',
			ctProfiledGame = 'p'
		};

		struct SiCalibrationQualityPoint
		{
			int ID;
			CalibrationPointQualityStruct Left;
			CalibrationPointQualityStruct Right;

			SiCalibrationQualityPoint() {}
			SiCalibrationQualityPoint(int aID, CalibrationPointQualityStruct& aLeft, CalibrationPointQualityStruct& aRight) :
					ID(aID), Left(aLeft), Right(aRight) {}
		};

		typedef TiDynArray<SiCalibrationQualityPoint> TiCalibrationQualityPoints;
		typedef TiDynArray<CalibrationPointStruct> TiCalibrationPoints;

		typedef void __fastcall (__closure *FiOnRecalibrateRequest)(System::TObject* aSender, int aPointID);
		typedef void __fastcall (__closure *FiOnPointEvent)(System::TObject* aSender, int aPointID, bool aIsRecalibrating);
		typedef void __fastcall (__closure *FiOnDebug)(System::TObject* aSender, const String& aMsg);

	private:
		struct SiUser
		{
			String Name;
			EiCalibType Sessions[DAY_COUNT][SESSION_COUNT];

			SiUser(const String& aName) : Name(aName) { }
		};

		typedef TiDynArray<SiUser> TiUsers;

	private:
		TfrmPreInstruction* iPreInstructionForm;
		TiFireflyAndPoints* iFireflyAndPoints;
		TiProfiledGame* iProfiledGame;
		TForm* iCalibrationForm;
		TiUsers* iUsers;

		TiTimestamp* iTimestamp;
		TiLogger* iEvents;
		TiLogger* iSamples;

		bool iDebug;
		String iSettingsFileName;

		SiUser* iCurrentUser;
		EiCalibType* iCurrentDaySessions;
		int iCurrentSessionIndex;
		int iCurrentDayIndex;

		TNotifyEvent FOnCalibrationStarted;
		TNotifyEvent FOnCalibrationDisplayReady;
		FiOnPointEvent FOnPointAborted;
		FiOnPointEvent FOnPointAccepted;
		FiOnRecalibrateRequest FOnRecalibrateRequest;
		TNotifyEvent FOnCalibrationFinished;
		TNotifyEvent FOnVerificationStarted;
		TNotifyEvent FOnVerificationFinished;
		TNotifyEvent FOnAborted;
		TNotifyEvent FOnFinished;
		FiOnDebug FOnDebug;

		void __fastcall CreateCalibration(EiCalibType aType);
		void __fastcall DestroyCalibration(TObject* aSender = NULL);

		void __fastcall RunNextSession(TObject* aSender = NULL);
		void __fastcall ShowPreInstruction(TfrmPreInstruction::EiInstruction aInstruction);

		TStrings* __fastcall GetUsers();

		void __fastcall onCalib_Event(TObject* aSender, const String& aMsg);
		void __fastcall onCalib_Sample(TObject* aSender, double aX, double aY);
		void __fastcall onCalib_Start(TObject* aSender);
		void __fastcall onCalib_ReadyToCalibrate(TObject* aSender);
		void __fastcall onCalib_RecalibPoint(TObject* aSender, int aPointNumber, bool aIsSinglePointMode);
		void __fastcall onCalib_PointReady(TObject* aSender, int aPointIndex, bool aIsSinglePointMode);
		void __fastcall onCalib_PointAborted(TObject* aSender, int aPointIndex, bool aIsSinglePointMode);
		void __fastcall onCalib_PointAccepted(TObject* aSender, int aPointIndex, bool aIsSinglePointMode);
		void __fastcall onCalib_Finished(TObject* aSender);
		void __fastcall onCalib_Aborted(TObject* aSender);
		void __fastcall onCalib_VerifStarted(TObject* aSender);
		void __fastcall onCalib_VerifFinished(TObject* aSender);
		void __fastcall onCalib_BeforeExit(TObject* aSender);

		void __fastcall onCalib_MouseMove(TObject* aSender, TShiftState Shift, int X, int Y);

		void __fastcall onCalibForm_Closed(TObject* aSender, TCloseAction& Action);

	public:
		__fastcall TiController(bool aDebug, String& aSettingsFileName);
		virtual __fastcall ~TiController();

		void __fastcall loadInstructions(String& aFile);
		void __fastcall run(String& aStudentName, int aDay);
		void __fastcall run(EiCalibType aCalibType);

		void __fastcall setPoints(TiCalibrationPoints* aPoints);
		void __fastcall nextPoint(int aPointID);
		void __fastcall verify(TiCalibrationQualityPoints* aPoints);
		void __fastcall feedSample(SampleStruct& aSample);

		__property TStrings* Users = {read = GetUsers};

		__property TNotifyEvent OnCalibrationStarted = {read = FOnCalibrationStarted, write = FOnCalibrationStarted};
		__property TNotifyEvent OnCalibrationDisplayReady = {read = FOnCalibrationDisplayReady, write = FOnCalibrationDisplayReady};
		__property FiOnPointEvent OnPointAborted = {read = FOnPointAborted, write = FOnPointAborted};
		__property FiOnPointEvent OnPointAccepted = {read = FOnPointAccepted, write = FOnPointAccepted};
		__property FiOnRecalibrateRequest OnRecalibrateRequest = {read = FOnRecalibrateRequest, write = FOnRecalibrateRequest};
		__property TNotifyEvent OnCalibrationFinished = {read = FOnCalibrationFinished, write = FOnCalibrationFinished};
		__property TNotifyEvent OnVerificationStarted = {read = FOnVerificationStarted, write = FOnVerificationStarted};
		__property TNotifyEvent OnVerificationFinished = {read = FOnVerificationFinished, write = FOnVerificationFinished};
		__property TNotifyEvent OnAborted = {read = FOnAborted, write = FOnAborted};
		__property TNotifyEvent OnFinished = {read = FOnFinished, write = FOnFinished};
		__property FiOnDebug OnDebug = {read = FOnDebug, write = FOnDebug};
};

//---------------------------------------------------------------------------
#endif
