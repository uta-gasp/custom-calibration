//---------------------------------------------------------------------------
#include "Controller.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
const String KDaySeparator = " ";
const String KSessionSeparator = ",";

//---------------------------------------------------------------------------
static Gdiplus::GdiplusStartupInput gdiplusStartupInput;
static ULONG_PTR m_gdiplusToken = NULL;

extern bool sMouseInput;

//---------------------------------------------------------------------------
__fastcall TiController::TiController(bool aDebug, String& aSettingsFileName) :
		TObject(),
		iDebug(aDebug),
		iSettingsFileName(aSettingsFileName),
		iFireflyAndPoints(NULL),
		iProfiledGame(NULL),
		iCalibrationForm(NULL),
		iTimestamp(NULL),
		iEvents(NULL),
		iSamples(NULL),
		iCurrentUser(NULL),
		iCurrentDaySessions(NULL),
		iCurrentSessionIndex(-1),
		iPreInstructionForm(NULL),
		FOnCalibrationStarted(NULL),
		FOnCalibrationDisplayReady(NULL),
		FOnPointAccepted(NULL),
		FOnPointAborted(NULL),
		FOnRecalibrateRequest(NULL),
		FOnCalibrationFinished(NULL),
		FOnVerificationStarted(NULL),
		FOnVerificationFinished(NULL),
		FOnAborted(NULL),
		FOnFinished(NULL),
		FOnDebug(NULL)
{
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	iUsers = new TiUsers(true);
}

//---------------------------------------------------------------------------
__fastcall TiController::~TiController()
{
	DestroyCalibration();

	delete iUsers;

	if (iPreInstructionForm)
	{
		delete iPreInstructionForm;
		iPreInstructionForm = NULL;
	}

	Gdiplus::GdiplusShutdown(m_gdiplusToken);
}

//---------------------------------------------------------------------------
void __fastcall TiController::loadInstructions(String& aFile)
{
	if (!FileExists(aFile))
		return;
		
	TStringList* lines = new TStringList();
	lines->LoadFromFile(aFile);

	for (int i = 0; i < lines->Count; i++)
	{
		String line = lines->Strings[i];
		TStrings* days = TiStrings::split(line, KDaySeparator);

		if (days->Count != 1 + DAY_COUNT)
			continue;

		SiUser* user = new SiUser(days->Strings[0]);
		iUsers->add(user);
		for (int di = 0; di < DAY_COUNT; di++)
		{
			TStrings* sessions = TiStrings::split(days->Strings[di+1], KSessionSeparator);
			if (sessions->Count != SESSION_COUNT)
				continue;

			for (int si = 0; si < sessions->Count; si++)
				user->Sessions[di][si] = (EiCalibType)sessions->Strings[si][1];

			delete sessions;
		}

		delete days;
	}

	delete lines;
}

//---------------------------------------------------------------------------
void __fastcall TiController::run(String& aStudentName, int aDay)
{
	if (iCalibrationForm)
		return;

	iCurrentUser = NULL;
	for (int i = 0; i < iUsers->Count; i++)
	{
		SiUser* user = iUsers->get(i);
		if (user->Name == aStudentName)
		{
			iCurrentUser = user;
			break;
		}
	}

	if (!iCurrentUser)
		return;

	iCurrentDaySessions = iCurrentUser->Sessions[aDay];
	iCurrentSessionIndex = 0;
	iCurrentDayIndex = aDay;

	if (iDebug)
	{
		RunNextSession();
	}
	else
	{
		iPreInstructionForm = new TfrmPreInstruction(NULL);
		iPreInstructionForm->SendToBack();
		iPreInstructionForm->Show();

		while (iCurrentDaySessions)
		{
			if (iCurrentSessionIndex < SESSION_COUNT)
				ShowPreInstruction((TfrmPreInstruction::EiInstruction)iCurrentDaySessions[iCurrentSessionIndex]);

			RunNextSession();
			iCurrentSessionIndex++;
		}
		DestroyCalibration();

		ShowPreInstruction(TfrmPreInstruction::instFinished);

		delete iPreInstructionForm;
		iPreInstructionForm = NULL;
	}
}

//---------------------------------------------------------------------------
void __fastcall TiController::run(EiCalibType aCalibType)
{
	if (iCalibrationForm)
		return;

	CreateCalibration(aCalibType);

	String userName = "";
	if (iCurrentUser)
		userName = iCurrentUser->Name;

	if (iDebug)
	{
		iCalibrationForm->Show();
		iCalibrationForm->OnClose = onCalibForm_Closed;

		if (iFireflyAndPoints)
			iFireflyAndPoints->showEyeBox();
		else if (iProfiledGame)
			iProfiledGame->showLogin(userName);
	}
	else
	{
		if (iFireflyAndPoints)
			iFireflyAndPoints->showModal();
		else if (iProfiledGame)
			iProfiledGame->showModal(userName);
	}
}

//---------------------------------------------------------------------------
void __fastcall TiController::setPoints(TiCalibrationPoints* aPoints)
{
	if (iFireflyAndPoints)
		iFireflyAndPoints->clearPoints();
	else if (iProfiledGame)
		iProfiledGame->clearPoints();

	for (int i = 0; i < aPoints->Count; i++)
	{
		CalibrationPointStruct* cp = aPoints->get(i);
		if (iFireflyAndPoints)
			iFireflyAndPoints->addPoint(*cp);
		else if (iProfiledGame)
			iProfiledGame->addPoint(*cp);
	}
}

//---------------------------------------------------------------------------
void __fastcall TiController::nextPoint(int aPointID)
{
	if (iFireflyAndPoints)
		iFireflyAndPoints->nextPoint(aPointID);
	else if (iProfiledGame)
		iProfiledGame->nextPoint(aPointID);
}

//---------------------------------------------------------------------------
void __fastcall TiController::verify(TiCalibrationQualityPoints* aPoints)
{
	if (!aPoints)
		return;

	for (int i = 0; i < aPoints->Count; i++)
	{
		SiCalibrationQualityPoint* pt = aPoints->get(i);
		if (iFireflyAndPoints)
			iFireflyAndPoints->reportCalibrationResult(pt->ID, pt->Left, pt->Right);
		else if (iProfiledGame)
			iProfiledGame->reportCalibrationResult(pt->ID, pt->Left, pt->Right);
	}

	if (iFireflyAndPoints)
		iFireflyAndPoints->processCalibrationResult();
	else if (iProfiledGame)
		iProfiledGame->processCalibrationResult();
}

//---------------------------------------------------------------------------
void __fastcall TiController::feedSample(SampleStruct& aSample)
{
	if (iFireflyAndPoints)
		iFireflyAndPoints->setSample(aSample);
	else if (iProfiledGame)
		iProfiledGame->setSample(aSample);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TiController::CreateCalibration(EiCalibType aType)
{
	DestroyCalibration();

	iTimestamp = new TiTimestamp();

	String fileNameSuffix;
	if (iCurrentUser)
		fileNameSuffix.printf("-%s-day%d", iCurrentUser->Name.c_str(), iCurrentDayIndex + 1);

	iEvents = new TiLogger(String("events") + fileNameSuffix);
	iSamples = new TiLogger(String("samples") + fileNameSuffix);

	if (iCurrentUser)
		iEvents->line(String().sprintf("user\t%s", iCurrentUser->Name.c_str()));

	if (aType == ctStandard || aType == ctFirefly)
	{
		TiFireflyAndPoints::EiAttractorType attractorType = aType == ctStandard ?
				TiFireflyAndPoints::atCircle :
				TiFireflyAndPoints::atFirefly;

		iFireflyAndPoints = new TiFireflyAndPoints(NULL, attractorType);
		iFireflyAndPoints->OnEvent = onCalib_Event;
		iFireflyAndPoints->OnSample = onCalib_Sample;
		iFireflyAndPoints->OnStart = onCalib_Start;
		iFireflyAndPoints->OnReadyToCalibrate = onCalib_ReadyToCalibrate;
		iFireflyAndPoints->OnRecalibrateSinglePoint = onCalib_RecalibPoint;
		iFireflyAndPoints->OnPointReady = onCalib_PointReady;
		iFireflyAndPoints->OnPointAccepted = onCalib_PointAccepted;
		iFireflyAndPoints->OnPointAborted = onCalib_PointAborted;
		iFireflyAndPoints->OnFinished = onCalib_Finished;
		iFireflyAndPoints->OnAborted = onCalib_Aborted;
		iFireflyAndPoints->OnGameStarted = onCalib_VerifStarted;
		iFireflyAndPoints->OnGameFinished = onCalib_VerifFinished;

		iFireflyAndPoints->GameAfterCalibration = true;

		if (sMouseInput)
			iFireflyAndPoints->OnMouseMove = onCalib_MouseMove;

		iCalibrationForm = iFireflyAndPoints;
	}
	else if (aType == ctProfiledGame)
	{
		iProfiledGame = new TiProfiledGame(NULL);
		iProfiledGame->OnEvent = onCalib_Event;
		iProfiledGame->OnSample = onCalib_Sample;
		iProfiledGame->OnStart = onCalib_Start;
		iProfiledGame->OnReadyToCalibrate = onCalib_ReadyToCalibrate;
		iProfiledGame->OnRecalibratePoint = onCalib_RecalibPoint;
		iProfiledGame->OnPointReady = onCalib_PointReady;
		iProfiledGame->OnPointAccepted = onCalib_PointAccepted;
		iProfiledGame->OnPointAborted = onCalib_PointAborted;
		iProfiledGame->OnFinished = onCalib_Finished;
		iProfiledGame->OnAborted = onCalib_Aborted;
		iProfiledGame->OnVerifStarted = onCalib_VerifStarted;
		iProfiledGame->OnVerifFinished = onCalib_VerifFinished;

		if (sMouseInput)
			iProfiledGame->OnMouseMove = onCalib_MouseMove;

		iCalibrationForm = iProfiledGame;
	}
	else
	{
		throw new Exception("No such game type");
	}

	if (FileExists(iSettingsFileName))
	{
		TiXML_INI* settings = new TiXML_INI(iSettingsFileName, "KidCalib", false);
		if (iFireflyAndPoints)
		{
			if (iCurrentUser)
				settings->openNode(iCurrentUser->Name, true);
			iFireflyAndPoints->loadSettings(settings);
			if (iCurrentUser)
				settings->closeNode();
		}
		else if (iProfiledGame)
		{
			iProfiledGame->loadSettings(settings);
		}
		delete settings;
	}
}

//---------------------------------------------------------------------------
void __fastcall TiController::DestroyCalibration(TObject* aSender)
{
	if (iCalibrationForm)
	{
		TiXML_INI* settings = new TiXML_INI(iSettingsFileName, "KidCalib", true);

		if (iFireflyAndPoints)
		{
			if (iCurrentUser)
				settings->openNode(iCurrentUser->Name, true);
			iFireflyAndPoints->saveSettings(settings);
			if (iCurrentUser)
				settings->closeNode();

			iFireflyAndPoints = NULL;
		}
		else if (iProfiledGame)
		{
			iProfiledGame->saveSettings(settings);
			iProfiledGame = NULL;
		}

		settings->save(false);
		delete settings;

		delete iCalibrationForm;
		iCalibrationForm = NULL;
	}

	if (iEvents)
	{
		delete iEvents;
		iEvents = NULL;
	}

	if (iSamples)
	{
		delete iSamples;
		iSamples = NULL;
	}

	if (iTimestamp)
	{
		delete iTimestamp;
		iTimestamp = NULL;
	}
}

//---------------------------------------------------------------------------
void __fastcall TiController::RunNextSession(TObject* aSender)
{
	DestroyCalibration();

	if (iCurrentSessionIndex >= SESSION_COUNT)
	{
		iCurrentUser = NULL;
		iCurrentDaySessions = NULL;
		iCurrentSessionIndex = -1;

		if (FOnFinished)
			FOnFinished(this);

		return;
	}

	run(iCurrentDaySessions[iCurrentSessionIndex]);
}

//---------------------------------------------------------------------------
void __fastcall TiController::ShowPreInstruction(TfrmPreInstruction::EiInstruction aInstruction)
{
	TfrmPreInstruction* kostyl = new TfrmPreInstruction(NULL);
	kostyl->Instruction = aInstruction;
	kostyl->ShowModal();
	delete kostyl;
}

//---------------------------------------------------------------------------
TStrings* __fastcall TiController::GetUsers()
{
	TStrings* result = new TStringList();
	for (int i = 0; i < iUsers->Count; i++)
		result->Add(iUsers->get(i)->Name);
	return result;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TiController::onCalib_Event(TObject* aSender, const String& aMsg)
{
	if (aMsg.Pos("CMD_"))
	{
		if (OnDebug)
			OnDebug(this, aMsg);
	}
	else
	{
		iEvents->line( String().sprintf("%d\t%s", iTimestamp->ms(), aMsg.c_str()) );
	}
}

//---------------------------------------------------------------------------
void __fastcall TiController::onCalib_Sample(TObject* aSender, double aX, double aY)
{
	iSamples->line( String().sprintf("%d\t%.2f\t%.2f", iTimestamp->ms(), aX, aY) );
}

//---------------------------------------------------------------------------
void __fastcall TiController::onCalib_Start(TObject* aSender) {
	if (OnDebug)
		OnDebug(this, "START");

	if (OnCalibrationStarted)
		OnCalibrationStarted(this);
}

//---------------------------------------------------------------------------
void __fastcall TiController::onCalib_ReadyToCalibrate(TObject* aSender)
{
	if (OnDebug)
		OnDebug(this, "DISPLAY_IS_READY");

	if (OnCalibrationDisplayReady)
		OnCalibrationDisplayReady(this);
}

//---------------------------------------------------------------------------
void __fastcall TiController::onCalib_PointReady(TObject* aSender, int aPointIndex, bool aIsSinglePointMode)
{
	if (OnDebug)
		OnDebug(this, String("PT_READY_") + aPointIndex + (aIsSinglePointMode?" [S]":""));
}

//---------------------------------------------------------------------------
void __fastcall TiController::onCalib_PointAborted(TObject* aSender, int aPointIndex, bool aIsSinglePointMode)
{
	if (OnDebug)
		OnDebug(this, String("PT_ABORT_")+aPointIndex+(aIsSinglePointMode?" [S]":""));

	if (OnPointAborted)
		OnPointAborted(this, aPointIndex, aIsSinglePointMode);
}

//---------------------------------------------------------------------------
void __fastcall TiController::onCalib_PointAccepted(TObject* aSender, int aPointIndex, bool aIsSinglePointMode)
{
	if (OnDebug)
		OnDebug(this, String("PT_ACCEPT_")+aPointIndex+(aIsSinglePointMode?" [S]":""));

	if (OnPointAccepted)
		OnPointAccepted(this, aPointIndex, aIsSinglePointMode);
}

//---------------------------------------------------------------------------
void __fastcall TiController::onCalib_RecalibPoint(TObject* aSender, int aPointNumber, bool aIsSinglePointMode)
{
	if (OnDebug)
		OnDebug(this, String("RECALIB_PT_#")+(aPointNumber-1));

	if (OnRecalibrateRequest)
		OnRecalibrateRequest(this, aPointNumber);
}

//---------------------------------------------------------------------------
void __fastcall TiController::onCalib_Finished(TObject* aSender)
{
	if (OnDebug)
		OnDebug(this, "VERIFYING");

	if (OnCalibrationFinished)
		OnCalibrationFinished(this);
}

//---------------------------------------------------------------------------
void __fastcall TiController::onCalib_Aborted(TObject* aSender)
{
	if (OnDebug)
		OnDebug(this, "ABORTED");

	if (OnAborted)
		OnAborted(this);
}

//---------------------------------------------------------------------------
void __fastcall TiController::onCalib_VerifStarted(TObject* aSender)
{
	if (OnDebug)
		OnDebug(this, "VERIF_STARTED");

	if (OnVerificationStarted)
		OnVerificationStarted(this);
}

//---------------------------------------------------------------------------
void __fastcall TiController::onCalib_VerifFinished(TObject* aSender)
{
	if (OnDebug)
		OnDebug(this, "VERIF_FINISHED");

	if (OnVerificationFinished)
		OnVerificationFinished(this);
}

//---------------------------------------------------------------------------
void __fastcall TiController::onCalib_MouseMove(TObject* aSender, TShiftState aShift, int aX, int aY)
{
	static double sLastEyeX = 0;
	static double sLastEyeY = 0;
	static double sLastDist = 530;

	if (!iCalibrationForm)
		return;

	if (!aShift.Contains(ssShift))
	{
		sLastEyeX = (double(aX) / iCalibrationForm->Width - 0.5) * 320;
		sLastEyeY = (0.5 - double(aY) / iCalibrationForm->Height) * 240;
	}
	else
	{
		double x = (double(aX) / iCalibrationForm->Width - 0.5) * 320;
		sLastDist = 500 + (x - sLastEyeX);
	}

	SampleStruct sample = {__int64(0),
			{double(aX), double(aY), 70.0, sLastEyeX - 27, sLastEyeY, sLastDist},
			{double(aX), double(aY), 70.0, sLastEyeX + 27, sLastEyeY, sLastDist}
	};

	if (iFireflyAndPoints)
		iFireflyAndPoints->setSample(sample);
	else if (iProfiledGame)
		iProfiledGame->setSample(sample);
}

//---------------------------------------------------------------------------
void __fastcall TiController::onCalibForm_Closed(TObject* aSender, TCloseAction& Action)
{
	if (!iCurrentDaySessions)
	{
		TiTimeout::run(200, DestroyCalibration);
		if (FOnFinished)
			FOnFinished(this);
	}
	else if (iDebug)
	{
		iCurrentSessionIndex++;
		TiTimeout::run(1000, RunNextSession);
	}
}

