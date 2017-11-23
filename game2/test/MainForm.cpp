//---------------------------------------------------------------------------
#include "MainForm.h"
#include "assets_pg.h"
#include "utils.h"

#include "XML.h"

#include <Sysutils.hpp>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
using namespace ProfiledGame;

//---------------------------------------------------------------------------
TfrmMainForm *frmMainForm;
static Gdiplus::GdiplusStartupInput gdiplusStartupInput;
static ULONG_PTR m_gdiplusToken = NULL;

const String KIniFileName = "settings2.xml";

const CalibrationPointStruct KCalibPoints[] = {
	{1, 700, 380},
	{2, 1200, 180},
	{3, 1000, 690},
	{4, 150, 600},
	{5, 200, 80},
};

//---------------------------------------------------------------------------
void Log(String aMsg) {
	frmMainForm->log->Lines->Add(aMsg);
}

//---------------------------------------------------------------------------
__fastcall TfrmMainForm::TfrmMainForm(TComponent* Owner)
	: TForm(Owner),
	iCustomCalibration(NULL),
	iTimestamp(NULL),
	iEvents(NULL),
	iSamples(NULL)
{
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	frmMainForm = this;
	iCalibPointStatus = new CalibrationPointUsageStatusEnum[ARRAYSIZE(KCalibPoints)];

	iScaleX = double(Screen->Width) / 1377;
	iScaleY = double(Screen->Height) / 768;

	//loadBitmapFromPNG(IDR_BACKGROUND, &iBackground);

	Canvas->Brush->Color = this->Color;
}

//---------------------------------------------------------------------------
__fastcall TfrmMainForm::~TfrmMainForm()
{
	delete [] iCalibPointStatus;
	Gdiplus::GdiplusShutdown(m_gdiplusToken);
}

//---------------------------------------------------------------------------
void __fastcall TfrmMainForm::CreateCalibration()
{
	DestroyCalibration();

	iTimestamp = new TiTimestamp();
	iEvents = new TiLogger("events");
	iSamples = new TiLogger("samples");

	iCustomCalibration = new TiProfiledGame(this);
	iCustomCalibration->OnEvent = onCalibrationEvent;
	iCustomCalibration->OnSample = onCalibrationSample;
	iCustomCalibration->OnStart = onCalibrationStart;
	iCustomCalibration->OnReadyToCalibrate = onCalibrationReadyToCalibrate;
	iCustomCalibration->OnRecalibratePoint = onRecalibratePoint;
	iCustomCalibration->OnPointReady = onCalibrationPointReady;
	iCustomCalibration->OnPointAccepted = onCalibrationPointAccepted;
	iCustomCalibration->OnFinished = onCalibrationFinished;
	iCustomCalibration->OnVerifStarted = onCalibrationVerificationStarted;
	iCustomCalibration->OnVerifFinished = onCalibrationVerificationFinished;
	iCustomCalibration->OnAborted = onCalibrationAborted;

	iCustomCalibration->OnMouseMove = onCalibrationMouseMove;

	if (FileExists(KIniFileName))
	{
		String fileName = ExtractFilePath(Application->ExeName);
		TiXML_INI* ini = new TiXML_INI(fileName + "\\" + KIniFileName, "KidCalib", false);
		iCustomCalibration->loadSettings(ini);
		delete ini;
	}
}

//---------------------------------------------------------------------------
void __fastcall TfrmMainForm::DestroyCalibration()
{
	if (iCustomCalibration)
	{
		String fileName = ExtractFilePath(Application->ExeName);
		TiXML_INI* ini = new TiXML_INI(fileName + "\\" + KIniFileName, "KidCalib", true);
		iCustomCalibration->saveSettings(ini);
		ini->save(false);
		delete ini;

		delete iCustomCalibration;
		iCustomCalibration = NULL;

		delete iEvents;
		iEvents = NULL;

		delete iSamples;
		iSamples = NULL;

		delete iTimestamp;
		iTimestamp = NULL;
	}
}

//---------------------------------------------------------------------------
void __fastcall TfrmMainForm::VerifyCalibration()
{
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
		left.usageStatus = iCalibPointStatus[i - 1];
		left.qualityIndex = double(Utils::randInRange(90, 100)) / 100.0;
		right = left;

		iCustomCalibration->reportCalibrationResult(i, left, right);
	}

	if (iCustomCalibration->processCalibrationResult())
	{
		Log("CALIB_FINISHED");
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TfrmMainForm::onCalibrationEvent(TObject* aSender, const String& aMsg)
{
	Log(aMsg);
	iEvents->line( String().sprintf("%d\t%s", iTimestamp->ms(), aMsg.c_str()) );
}

//---------------------------------------------------------------------------
void __fastcall TfrmMainForm::onCalibrationSample(TObject* aSender, double aX, double aY)
{
	iSamples->line( String().sprintf("%d\t%.2f\t%.2f", iTimestamp->ms(), aX, aY) );
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TfrmMainForm::onCalibrationStart(TObject* aSender) {
Log("START");
	iCustomCalibration->clearPoints();

	for (int i = 0; i < ARRAYSIZE(KCalibPoints); i++)
	{
		CalibrationPointStruct& calibPoint = KCalibPoints[i];
		CalibrationPointStruct cp;
		cp.number = calibPoint.number;
		cp.positionX = calibPoint.positionX * iScaleX;
		cp.positionY = calibPoint.positionY * iScaleY;

		iCustomCalibration->addPoint(cp);
	}
}

void __fastcall TfrmMainForm::onCalibrationReadyToCalibrate(TObject* aSender) {
Log("READY_TO_CALIB");
	iCurrentCalibPointNumber = 1;
	iCustomCalibration->nextPoint(iCurrentCalibPointNumber);
}

void __fastcall TfrmMainForm::onRecalibratePoint(TObject* aSender, int aPointID, bool aIsSinglePointMode) {
Log(String("RECALIB_PT_#")+(aPointID-1));
	iCurrentCalibPointNumber = aPointID;
	iCustomCalibration->nextPoint(iCurrentCalibPointNumber);
}

void __fastcall TfrmMainForm::onCalibrationPointReady(TObject* aSender, int aPointIndex, bool aIsSinglePointMode) {
Log(String("PT_READY_")+aPointIndex+(aIsSinglePointMode?" [S]":""));
}

void __fastcall TfrmMainForm::onCalibrationPointAccepted(TObject* aSender, int aPointIndex, bool aIsSinglePointMode) {
Log(String("PT_ACCEPT_")+aPointIndex+(aIsSinglePointMode?" [S]":""));
	iCalibPointStatus[aPointIndex] = calibrationPointUsed;

	int nextPointID = -1;
	if (!aIsSinglePointMode)
	{
		++iCurrentCalibPointNumber;
		if (iCurrentCalibPointNumber > ARRAYSIZE(KCalibPoints))
			iCurrentCalibPointNumber = -1;
		nextPointID = iCurrentCalibPointNumber;
	}

	iCustomCalibration->nextPoint(nextPointID);
}

void __fastcall TfrmMainForm::onCalibrationFinished(TObject* aSender) {
Log("CALIB FINISHED");
VerifyCalibration();
}

void __fastcall TfrmMainForm::onCalibrationAborted(TObject* aSender) {
Log("ABORTED");
}

void __fastcall TfrmMainForm::onCalibrationVerificationStarted(TObject* aSender) {
Log("VERIF START");
}

void __fastcall TfrmMainForm::onCalibrationVerificationFinished(TObject* aSender) {
Log("VERIF FINISHED");
}

void __fastcall TfrmMainForm::onCalibrationMouseMove(TObject* aSender, TShiftState Shift, int X, int Y) {
	static double sLastEyeX = 0;
	static double sLastEyeY = 0;
	static double sLastDist = 530;

	if (!Shift.Contains(ssShift))
	{
		sLastEyeX = (double(X) / iCustomCalibration->Width - 0.5) * 320;
		sLastEyeY = (0.5 - double(Y) / iCustomCalibration->Height) * 240;
	}
	else
	{
		double x = (double(X) / iCustomCalibration->Width - 0.5) * 320;
		sLastDist = 500 + (x - sLastEyeX);
	}

	SampleStruct sample = {__int64(0),
			{double(X), double(Y), 70.0, sLastEyeX-30, sLastEyeY, sLastDist},
			{double(X), double(Y), 70.0, sLastEyeX+30, sLastEyeY, sLastDist}
	};
	iCustomCalibration->setSample(sample);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TfrmMainForm::FormDestroy(TObject *Sender)
{
	DestroyCalibration();
}

//---------------------------------------------------------------------------
void __fastcall TfrmMainForm::btnStartClick(TObject *Sender)
{
	CreateCalibration();

	iCustomCalibration->Show();

	iCustomCalibration->showLogin(log->Text);
}

//---------------------------------------------------------------------------


