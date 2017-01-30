//---------------------------------------------------------------------------
#include "MainForm.h"
#include "assets.h"
#include "utils.h"

#include "XML.h"

#include <Sysutils.hpp>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
TfrmMainForm *frmMainForm;

const String KIniFileName = "settings.ini";

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
	iAnimationTimeout(NULL)
{
	frmMainForm = this;

	//loadBitmapFromPNG(IDR_BACKGROUND, &iBackground);
	iCustomCalibration = new TfrmCustomCalibration(this);
	iCustomCalibration->OnDebug = onCalibrationDebug;
	iCustomCalibration->OnStart = onCalibrationStart;
	iCustomCalibration->OnReadyToCalibrate = onCalibrationReadyToCalibrate;
	iCustomCalibration->OnRecalibrateSinglePoint = onRecalibrateSinglePoint;
	iCustomCalibration->OnPointReady = onCalibrationPointReady;
	iCustomCalibration->OnPointAccepted = onCalibrationPointAccepted;
	iCustomCalibration->OnFinished = onCalibrationFinished;
	iCustomCalibration->OnAborted = onCalibrationAborted;

	iCustomCalibration->OnMouseMove = onCalibrationMouseMove;

	if (FileExists(KIniFileName))
	{
		String fileName = ExtractFilePath(Application->ExeName);
		TiXML_INI* ini = new TiXML_INI(fileName + "\\" + KIniFileName, "KidCalib", false);
		iCustomCalibration->loadSettings(ini);
		delete ini;
	}

	iCustomCalibration->Show();

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
void __fastcall TfrmMainForm::onCreaturesPaint(TObject* aSender, EiUpdateType aUpdateType)
{
	Gdiplus::Rect destRect(0, 0, Panel1->Width, Panel1->Height);

	//Gdiplus::Bitmap* background = iBackground->Clone(destRect, iBackground->GetPixelFormat());
	Gdiplus::Bitmap* background;
	loadBitmapFromPNG(IDR_BACKGROUND, &background);

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
void __fastcall TfrmMainForm::onCalibrationDebug(TObject* aSender, const char* aMsg)
{
	Log(aMsg);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TfrmMainForm::onCalibrationStart(TObject* aSender) {
Log("START");
	iCustomCalibration->clearPoints();

	for (int i = 1; i <= ARRAYSIZE(KCalibPoints); i++)
	{
		iCustomCalibration->addPoint(KCalibPoints[i - 1]);
	}
}

void __fastcall TfrmMainForm::onCalibrationReadyToCalibrate(TObject* aSender) {
Log("READY_TO_CALIB");
	iCurrentCalibPointNumber = 1;
	iCustomCalibration->nextPoint(iCurrentCalibPointNumber);
}

void __fastcall TfrmMainForm::onRecalibrateSinglePoint(TObject* aSender, int aPointIndex, bool aIsSinglePointMode) {
Log(String("RECALIB_PT_")+aPointIndex);
}

void __fastcall TfrmMainForm::onCalibrationPointReady(TObject* aSender, int aPointIndex, bool aIsSinglePointMode) {
Log(String("PT_READY_")+aPointIndex+(aIsSinglePointMode?" [S]":""));
}

void __fastcall TfrmMainForm::onCalibrationPointAborted(TObject* aSender, int aPointIndex, bool aIsSinglePointMode) {
Log(String("PT_ABORT_")+aPointIndex+(aIsSinglePointMode?" [S]":""));
}

void __fastcall TfrmMainForm::onCalibrationPointAccepted(TObject* aSender, int aPointIndex, bool aIsSinglePointMode) {
Log(String("PT_ACCEPT_")+aPointIndex+(aIsSinglePointMode?" [S]":""));
	++iCurrentCalibPointNumber;
	if (iCurrentCalibPointNumber > ARRAYSIZE(KCalibPoints))
		iCurrentCalibPointNumber = -1;
	iCustomCalibration->nextPoint(iCurrentCalibPointNumber);
}

void __fastcall TfrmMainForm::onCalibrationFinished(TObject* aSender) {
Log("FINISHED");
	for (int i = 1; i <= ARRAYSIZE(KCalibPoints); i++)
	{
		CalibrationPointStruct& calibPoint = KCalibPoints[i - 1];
		CalibrationPointQualityStruct* left = new CalibrationPointQualityStruct();
		CalibrationPointQualityStruct* right = new CalibrationPointQualityStruct();
		left->number = i;
		left->positionX = calibPoint.positionX;
		left->positionY = calibPoint.positionY;
		left->correctedPorX = calibPoint.positionX + randInRange(-20, 20);
		left->correctedPorY = calibPoint.positionY + randInRange(-20, 20);
		left->standardDeviationX = randInRange(-40, 40);
		left->standardDeviationY = randInRange(-40, 40);
		left->usageStatus = 0;
		left->qualityIndex = double(randInRange(80, 100)) / 100.0;
		*right = *left;

		iCustomCalibration->reportCalibrationResult(i, left, right);
	}

	iCustomCalibration->processCalibrationResult();
}

void __fastcall TfrmMainForm::onCalibrationAborted(TObject* aSender) {
Log("ABORTED");
	Close();
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
			{0.0, 0.0, 0.0, sLastEyeX-35, sLastEyeY, sLastDist},
			{0.0, 0.0, 0.0, sLastEyeX+35, sLastEyeY, sLastDist}
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
	String fileName = ExtractFilePath(Application->ExeName);
	TiXML_INI* ini = new TiXML_INI(fileName + "\\" + KIniFileName, "KidCalib", true);
	iCustomCalibration->saveSettings(ini);
	ini->save(false);
	delete ini;
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

