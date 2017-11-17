//---------------------------------------------------------------------------
#include "MainForm.h"
#include "assets_ffap.h"
#include "utils.h"

#include "XML.h"

#include <Sysutils.hpp>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
using namespace FireflyAndPoints;

//---------------------------------------------------------------------------
TfrmMainForm *frmMainForm;
static Gdiplus::GdiplusStartupInput gdiplusStartupInput;
static ULONG_PTR m_gdiplusToken = NULL;

const String KIniFileName = "settings1.xml";

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
	iAnimationTimeout(NULL),
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

	iCreatures = new TiAnimationManager();
	iCreatures->OnPaint = onCreaturesPaint;

	for (int i = 0; i < 2; i++)
	{
		TiAnimation* animation = new TiAnimation();
		animation->addFrames(IDR_FIREFLY, 64);
		animation->LoopAnimation = false;
		animation->placeTo(32, 32 + 64*i);
		iCreatures->add(animation);
	}

	iFly = (*iCreatures)[0];
	iFly->OnAnimationFinished = onAnimationFinished;
	iFly->OnMoveFinished = onMoveFinished;
	iFly->OnRotationFinished = onRotationFinished;
	iFly->OnFadingFinished = onFadingFinished;

	Canvas->Brush->Color = this->Color;

	TrackBar1->Position = iFly->AnimationSpeed;
	TrackBar2->Position = iFly->MoveSpeed;
	TrackBar3->Position = iFly->RotationSpeed;
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

	iCustomCalibration = new TiFireflyAndPoints(this);
	iCustomCalibration->OnEvent = onCalibrationEvent;
	iCustomCalibration->OnSample = onCalibrationSample;
	iCustomCalibration->OnStart = onCalibrationStart;
	iCustomCalibration->OnReadyToCalibrate = onCalibrationReadyToCalibrate;
	iCustomCalibration->OnRecalibrateSinglePoint = onRecalibrateSinglePoint;
	iCustomCalibration->OnPointReady = onCalibrationPointReady;
	iCustomCalibration->OnPointAccepted = onCalibrationPointAccepted;
	iCustomCalibration->OnPointAborted = onCalibrationPointAborted;
	iCustomCalibration->OnFinished = onCalibrationFinished;
	iCustomCalibration->OnAborted = onCalibrationAborted;
	iCustomCalibration->OnGameStarted = onCalibrationGameStarted;
	iCustomCalibration->OnGameFinished = onCalibrationGameFinished;

	iCustomCalibration->AttractorType = TiFireflyAndPoints::atFirefly;
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
		if (iCustomCalibration->GameAfterCalibration)
		{
			Log("START_GAME");
		}
	}
}

//---------------------------------------------------------------------------
void __fastcall TfrmMainForm::onCreaturesPaint(TObject* aSender, EiUpdateType aUpdateType)
{
	Gdiplus::Rect destRect(0, 0, Panel1->Width, Panel1->Height);

	//Gdiplus::Bitmap* background = iBackground->Clone(destRect, iBackground->GetPixelFormat());
	Gdiplus::Bitmap* background;
	Utils::loadBitmapFromPNG(IDR_BACKGROUND, &background);

	Gdiplus::Graphics g(background);
	iCreatures->paintTo(&g);

	iGraphics->DrawImage(background, destRect,
			0, 0, Panel1->Width, Panel1->Height,
			Gdiplus::UnitPixel);

	delete background;
}

//---------------------------------------------------------------------------
void __fastcall TfrmMainForm::onAnimationFinished(TObject* aSender)
{
	Log("anim fisnihed");

	TiTimeout::run(1000, onPostAnimation, &iAnimationTimeout);
}

//---------------------------------------------------------------------------
void __fastcall TfrmMainForm::onMoveFinished(TObject* aSender)
{
	Log("move fisnihed");
}

//---------------------------------------------------------------------------
void __fastcall TfrmMainForm::onRotationFinished(TObject* aSender)
{
	Log("rotation fisnihed");
}

//---------------------------------------------------------------------------
void __fastcall TfrmMainForm::onFadingFinished(TObject* aSender)
{
	Log("fade fisnihed");
}

//---------------------------------------------------------------------------
void __fastcall TfrmMainForm::onPostAnimation(TObject* aSender)
{
	iFly->AnimationIndex = 1 - iFly->AnimationIndex;
	iFly->toggleAnimation();
	Log("post anim");
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TfrmMainForm::onCalibrationEvent(TObject* aSender, const String& aMsg)
{
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

void __fastcall TfrmMainForm::onRecalibrateSinglePoint(TObject* aSender, int aPointNumber, bool aIsSinglePointMode) {
Log(String("RECALIB_PT_#")+(aPointNumber-1));

	iCurrentCalibPointNumber = aPointNumber;
	iCustomCalibration->nextPoint(iCurrentCalibPointNumber);
}

void __fastcall TfrmMainForm::onCalibrationPointReady(TObject* aSender, int aPointIndex, bool aIsSinglePointMode) {
Log(String("PT_READY_")+aPointIndex+(aIsSinglePointMode?" [S]":""));
}

void __fastcall TfrmMainForm::onCalibrationPointAborted(TObject* aSender, int aPointIndex, bool aIsSinglePointMode) {
Log(String("PT_ABORT_")+aPointIndex+(aIsSinglePointMode?" [S]":""));

	iCalibPointStatus[aPointIndex] = calibrationPointUnusedBecauseOfTimeout;

	if (!aIsSinglePointMode)
	{
		++iCurrentCalibPointNumber;
		if (iCurrentCalibPointNumber > ARRAYSIZE(KCalibPoints))
			iCurrentCalibPointNumber = -1;
		iCustomCalibration->nextPoint(iCurrentCalibPointNumber);
	}
	else
	{
		iCustomCalibration->nextPoint(-1);
	}
}

void __fastcall TfrmMainForm::onCalibrationPointAccepted(TObject* aSender, int aPointIndex, bool aIsSinglePointMode) {
Log(String("PT_ACCEPT_")+aPointIndex+(aIsSinglePointMode?" [S]":""));
	if (Mouse->CursorPos.x == 0)
		return;

	iCalibPointStatus[aPointIndex] = calibrationPointUsed;

	if (!aIsSinglePointMode)
	{
		++iCurrentCalibPointNumber;
		if (iCurrentCalibPointNumber > ARRAYSIZE(KCalibPoints))
			iCurrentCalibPointNumber = -1;
		iCustomCalibration->nextPoint(iCurrentCalibPointNumber);
	}
	else
	{
		iCustomCalibration->nextPoint(-1);
	}
}

void __fastcall TfrmMainForm::onCalibrationFinished(TObject* aSender) {
Log("VERIFYING");
VerifyCalibration();
}

void __fastcall TfrmMainForm::onCalibrationAborted(TObject* aSender) {
Log("ABORTED");
}

void __fastcall TfrmMainForm::onCalibrationGameStarted(TObject* aSender) {
Log("GAME_STARTED");
}

void __fastcall TfrmMainForm::onCalibrationGameFinished(TObject* aSender) {
Log("GAME_FINISHED");
}

void __fastcall TfrmMainForm::onCalibrationMouseMove(TObject* aSender, TShiftState Shift, int X, int Y) {
static double sLastEyeX = 0;
static double sLastEyeY = 0;
static double sLastDist = 500;

	if (!Shift.Contains(ssShift))
	{
		sLastEyeX = (double(X) / iCustomCalibration->Width - 0.5) * 320;
		sLastEyeY = (0.5 - double(Y) / iCustomCalibration->Height) * 240;
	}
	else
	{
		double x = (double(X) / iCustomCalibration->Width - 0.5) * 320;
		sLastDist = 500 + (x - sLastEyeX);

		//iCalibration->setTrackingStability(Y < Panel1->Height / 2);
	}

	SampleStruct sample = {__int64(0),
			{double(X), double(Y), 70.0, sLastEyeX-35, sLastEyeY, sLastDist},
			{double(X), double(Y), 70.0, sLastEyeX+35, sLastEyeY, sLastDist}
	};
	iCustomCalibration->setSample(sample);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TfrmMainForm::Button1Click(TObject *Sender)
{
	if (iAnimationTimeout)
	{
		iAnimationTimeout->kill();
		Log("timeout killed");
	}

	iCreatures->toggleAnimation();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMainForm::Button3Click(TObject *Sender)
{
	iFly->rotateBy(360);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMainForm::TrackBar1Change(TObject *Sender)
{
	iCreatures->setAnimationSpeed(TrackBar1->Position);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMainForm::TrackBar2Change(TObject *Sender)
{
	iCreatures->setMoveSpeed(TrackBar2->Position);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMainForm::TrackBar3Change(TObject *Sender)
{
	iCreatures->setRotationSpeed(TrackBar3->Position);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMainForm::CheckBox1Click(TObject *Sender)
{
	iCreatures->setLoopAnimation(CheckBox1->Checked);
}

//---------------------------------------------------------------------------
void __fastcall TfrmMainForm::FormPaint(TObject *Sender)
{
	onCreaturesPaint(Sender, updAll);
}

//---------------------------------------------------------------------------
void __fastcall TfrmMainForm::FormKeyUp(TObject *Sender, WORD &Key,
			TShiftState Shift)
{
	if (Key == VK_SPACE)
		iFly->moveTo(iFly->X + 100, iFly->Y);
}

//---------------------------------------------------------------------------
void __fastcall TfrmMainForm::Panel1MouseUp(TObject *Sender, TMouseButton Button,
			TShiftState Shift, int X, int Y)
{
	if (Button ==  mbLeft)
		iFly->moveTo(X, Y);
	else if (Button ==  mbRight)
		iFly->AnimationIndex = 1 - iFly->AnimationIndex;
}

//---------------------------------------------------------------------------
void __fastcall TfrmMainForm::FormCreate(TObject *Sender)
{
	iGraphics = new Gdiplus::Graphics(Panel1->Handle, false);
}

//---------------------------------------------------------------------------
void __fastcall TfrmMainForm::FormDestroy(TObject *Sender)
{
	DestroyCalibration();
}

//---------------------------------------------------------------------------
void __fastcall TfrmMainForm::Button2Click(TObject *Sender)
{
	if (!iFly->IsVisible || iFly->FadingDirection < 0)
		iFly->fadeIn();
	else if (iFly->FadingDirection >= 0)
		iFly->fadeOut();
}

//---------------------------------------------------------------------------
void __fastcall TfrmMainForm::btnStartClick(TObject *Sender)
{
	CreateCalibration();
	
	iCustomCalibration->GameAfterCalibration =
			Sender == btnStartCalibAndGame ||
			Sender == btnStartGameOnly;

	iCustomCalibration->Show();

	if (Sender == btnStartCalibAndGame || Sender == btnStartCalibOnly)
		iCustomCalibration->showEyeBox();
	else
		iCustomCalibration->playGame();
}

//---------------------------------------------------------------------------


