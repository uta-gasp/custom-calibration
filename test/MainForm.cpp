//---------------------------------------------------------------------------
#include "MainForm.h"
#include "assets.h"
#include "utils.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
TfrmMainForm *frmMainForm;

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
	iCalibration = new TfrmCalibration(this);
	iCalibration->OnDebug = onCalibrationDebug;
	iCalibration->OnStart = onCalibrationStart;
	iCalibration->OnRecalibrateSinglePoint = onRecalibrateSinglePoint;
	iCalibration->OnPointReady = onCalibrationPointReady;
	iCalibration->OnPointAccept = onCalibrationPointAccept;
	iCalibration->OnFinished = onCalibrationFinished;
	iCalibration->OnAborted = onCalibrationAborted;
	iCalibration->Show();

	iCreatures = new TiAnimationManager();
	iCreatures->OnPaint = onCreaturesPaint;

	for (int i = 0; i < 2; i++)
	{
		TiAnimation* animation = new TiAnimation();
		animation->addFrames(IDR_FIREFLY_RED, 64);
		animation->addFrames(IDR_FIREFLY_BLUE, 64);
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
void __fastcall TfrmMainForm::onCreaturesPaint(TObject* aSender)
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
void __fastcall TfrmMainForm::onCalibrationStart(TObject* aSender) { Log("START"); }
void __fastcall TfrmMainForm::onRecalibrateSinglePoint(TObject* aSender) { Log("RECALIB_PT"); }
void __fastcall TfrmMainForm::onCalibrationPointReady(TObject* aSender, int aPointIndex, bool aIsSinglePointMode) { Log(String("PT_READY_")+aPointIndex+(aIsSinglePointMode?" [S]":"")); }
void __fastcall TfrmMainForm::onCalibrationPointAccept(TObject* aSender, int aPointIndex, bool aIsSinglePointMode) { Log(String("PT_ACCEPT_")+aPointIndex+(aIsSinglePointMode?" [S]":"")); }
void __fastcall TfrmMainForm::onCalibrationFinished(TObject* aSender) { Log("FINISHED"); }
void __fastcall TfrmMainForm::onCalibrationAborted(TObject* aSender) { Log("ABORTED"); }

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
	onCreaturesPaint(Sender);
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
void __fastcall TfrmMainForm::Button2Click(TObject *Sender)
{
	if (!iFly->IsVisible || iFly->FadingDirection < 0)
		iFly->fadeIn();
	else if (iFly->FadingDirection >= 0)
		iFly->fadeOut();
}

//---------------------------------------------------------------------------
void __fastcall TfrmMainForm::Panel1MouseMove(TObject *Sender, TShiftState Shift,
			int X, int Y)
{
	iCalibration->setTrackingStability(Y < Panel1->Height / 2);
}
//---------------------------------------------------------------------------

