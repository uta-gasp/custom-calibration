//---------------------------------------------------------------------------
#include "MainForm.h"
#include "utils.h"

#include "XML.h"

//---------------------------------------------------------------------------
#include <Sysutils.hpp>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
TfrmMainForm *frmMainForm;

//---------------------------------------------------------------------------
const CalibrationPointStruct KCalibPoints[] = {
	{1, 700, 380},
	{2, 1200, 180},
	{3, 1000, 690},
	{4, 150, 600},
	{5, 200, 80},
};

const String KIniFileName = "settings.xml";
const String KSessionsFileName = "sessions.txt";

const bool KDebug = false;

//---------------------------------------------------------------------------
void Log(String aMsg) {
	frmMainForm->log->Lines->Add(aMsg);
}

//---------------------------------------------------------------------------
__fastcall TfrmMainForm::TfrmMainForm(TComponent* Owner) :
		TForm(Owner),
		iController(NULL)
{
	frmMainForm = this;

	iScaleX = double(Screen->Width) / 1377;
	iScaleY = double(Screen->Height) / 768;

	Canvas->Brush->Color = this->Color;

	String fileName = ExtractFilePath(Application->ExeName);
	iSettingsFileName = fileName + "\\" + KIniFileName;

	for (int i = 0; i < DAY_COUNT; i++)
		cmbDays->Items->Add(String().sprintf("Day %d", i + 1));

	CreateController();
}

//---------------------------------------------------------------------------
__fastcall TfrmMainForm::~TfrmMainForm()
{
	DeleteController();
}

//---------------------------------------------------------------------------
void __fastcall TfrmMainForm::CreateController()
{
	iController = new TiController(KDebug, iSettingsFileName);
	iController->OnDebug = onCtrl_Debug;
	iController->OnCalibrationStarted = onCtrl_CalibrationStarted;
	iController->OnCalibrationDisplayReady = onCtrl_CalibrationDisplayReady;
	iController->OnPointAborted = onCtrl_PointAborted;
	iController->OnPointAccepted = onCtrl_PointAccepted;
	iController->OnRecalibrateRequest = onCtrl_RecalibPoint;
	iController->OnCalibrationFinished = onCtrl_CalibrationFinished;
	//iController->OnVerificationStarted = onCtrl_VerificationStarted;
	//iController->OnVerificationFinished = onCtrl_VerificationFinished;
	//iController->OnAborted = onCtrl_Aborted;

	String fileName = ExtractFilePath(Application->ExeName);
	fileName = fileName + "\\" + KSessionsFileName;
	iController->loadInstructions(fileName);

	TStrings* users = iController->Users;
	cmbUsers->Clear();
	for (int i = 0; i < users->Count; i++)
		cmbUsers->Items->Add(users->Strings[i]);
}

//---------------------------------------------------------------------------
void __fastcall TfrmMainForm::DeleteController()
{
	if (iController)
	{
		delete iController;
		iController = NULL;
	}
}

//---------------------------------------------------------------------------
void __fastcall TfrmMainForm::VerifyCalibration()
{
	TiController::TiCalibrationQualityPoints* calibQualityPoints = new TiController::TiCalibrationQualityPoints(true);

	for (int i = 1; i <= ARRAYSIZE(KCalibPoints); i++)
	{
		CalibrationPointStruct& calibPoint = KCalibPoints[i - 1];

		CalibrationPointQualityStruct left, right;
		int x = calibPoint.positionX * iScaleX;
		int y = calibPoint.positionY * iScaleY;

		left.number = i;
		left.positionX = x;
		left.positionY = y;
		left.correctedPorX = x + Utils::randInRange(-35, 35);
		left.correctedPorY = y + Utils::randInRange(-35, 35);
		left.standardDeviationX = Utils::randInRange(-40, 40);
		left.standardDeviationY = Utils::randInRange(-40, 40);
		left.usageStatus = calibrationPointUsed;
		left.qualityIndex = double(Utils::randInRange(90, 100)) / 100.0;
		right = left;

		calibQualityPoints->add(new TiController::SiCalibrationQualityPoint(i, left, right));
	}

	iController->verify(calibQualityPoints);

	delete calibQualityPoints;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TfrmMainForm::onCtrl_Debug(TObject* aSender, const String& aMsg)
{
	Log(aMsg);
}

//---------------------------------------------------------------------------
void __fastcall TfrmMainForm::onCtrl_CalibrationStarted(TObject* aSender)
{
	TiController::TiCalibrationPoints* calibPoints = new TiController::TiCalibrationPoints(true);

	for (int i = 0; i < ARRAYSIZE(KCalibPoints); i++)
	{
		CalibrationPointStruct& calibPoint = KCalibPoints[i];
		CalibrationPointStruct* cp = new CalibrationPointStruct();
		cp->number = calibPoint.number;
		cp->positionX = calibPoint.positionX * iScaleX;
		cp->positionY = calibPoint.positionY * iScaleY;

		calibPoints->add(cp);
	}

	iController->setPoints(calibPoints);
	delete calibPoints;
}

//---------------------------------------------------------------------------
void __fastcall TfrmMainForm::onCtrl_CalibrationDisplayReady(TObject* aSender)
{
	iCurrentCalibPointNumber = 1;
	iController->nextPoint(iCurrentCalibPointNumber);
}

//---------------------------------------------------------------------------
void __fastcall TfrmMainForm::onCtrl_RecalibPoint(TObject* aSender, int aPointID)
{
	iCurrentCalibPointNumber = aPointID;
	iController->nextPoint(iCurrentCalibPointNumber);
}

//---------------------------------------------------------------------------
void __fastcall TfrmMainForm::onCtrl_PointAborted(TObject* aSender, int aPointID, bool aIsRecalibrating)
{
	int nextPointID = -1;
	if (!aIsRecalibrating)
	{
		++iCurrentCalibPointNumber;
		if (iCurrentCalibPointNumber > ARRAYSIZE(KCalibPoints))
			iCurrentCalibPointNumber = -1;
		nextPointID = iCurrentCalibPointNumber;
	}

	iController->nextPoint(nextPointID);
}

//---------------------------------------------------------------------------
void __fastcall TfrmMainForm::onCtrl_PointAccepted(TObject* aSender, int aPointID, bool aIsRecalibrating)
{
	int nextPointID = -1;
	if (!aIsRecalibrating)
	{
		++iCurrentCalibPointNumber;
		if (iCurrentCalibPointNumber > ARRAYSIZE(KCalibPoints))
			iCurrentCalibPointNumber = -1;
		nextPointID = iCurrentCalibPointNumber;
	}

	iController->nextPoint(nextPointID);
}

//---------------------------------------------------------------------------
void __fastcall TfrmMainForm::onCtrl_CalibrationFinished(TObject* aSender)
{
	VerifyCalibration();
}

//---------------------------------------------------------------------------
//void __fastcall TfrmMainForm::onCtrl_Aborted(TObject* aSender) { }
//void __fastcall TfrmMainForm::onCtrl_VerifStarted(TObject* aSender) {}
//void __fastcall TfrmMainForm::onCtrl_VerifFinished(TObject* aSender) {}

//---------------------------------------------------------------------------
void __fastcall TfrmMainForm::onCtrl_Finished(TObject* aSender)
{
	MessageBox(Handle, "Finished", "KidCalib", MB_OK);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TfrmMainForm::btnStartCalib1Click(TObject *Sender)
{
	iController->OnFinished = NULL;
	iController->run(TiController::ctStandard);
}

//---------------------------------------------------------------------------
void __fastcall TfrmMainForm::btnStartCalib2Click(TObject *Sender)
{
	iController->OnFinished = NULL;
	iController->run(TiController::ctFirefly);
}

//---------------------------------------------------------------------------
void __fastcall TfrmMainForm::btnStartCalib3Click(TObject *Sender)
{
	iController->OnFinished = NULL;
	iController->run(TiController::ctProfiledGame);
}

//---------------------------------------------------------------------------
void __fastcall TfrmMainForm::cmbUsersOrDaysChange(TObject *Sender)
{
	btnRunUserDay->Enabled = cmbUsers->ItemIndex >= 0 && cmbDays->ItemIndex >= 0;
}

//---------------------------------------------------------------------------
void __fastcall TfrmMainForm::btnRunUserDayClick(TObject *Sender)
{
	iController->OnFinished = onCtrl_Finished;
	iController->run(cmbUsers->Text, cmbDays->ItemIndex);
}

//---------------------------------------------------------------------------

