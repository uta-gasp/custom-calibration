//---------------------------------------------------------------------------
#include "Calibration.h"
#include "assets.h"
#include "fakeMyGaze.h"

//---------------------------------------------------------------------------
#include <math.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
static Gdiplus::GdiplusStartupInput gdiplusStartupInput;
static ULONG_PTR m_gdiplusToken = NULL;

TfrmCalibration* frmCalibration;

const int KEyeBoxWidth = 160;
const int KEyeBoxHeight = 120;

const int KMaxAllowedCalibQualityOffset = 22;

//---------------------------------------------------------------------------
__fastcall TfrmCalibration::TfrmCalibration(TComponent* aOwner) :
		TForm(aOwner),
		iTimeout(NULL),
		iIsWaitingToAcceptPoint(false),
		iStaticBitmap(NULL),
		FOnDebug(NULL),
		FOnStart(NULL),
		FOnRecalibrateSinglePoint(NULL),
		FOnPointReady(NULL),
		FOnPointAccept(NULL),
		FOnFinished(NULL),
		FOnAborted(NULL)
{
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	randomize();
}

//---------------------------------------------------------------------------
__fastcall TfrmCalibration::~TfrmCalibration()
{
	if (iStaticBitmap)
		delete iStaticBitmap;

	Gdiplus::GdiplusShutdown(m_gdiplusToken);
}

//---------------------------------------------------------------------------
void __fastcall TfrmCalibration::setSample(SampleStruct& aSample)
{
	iEyeBox->left(aSample.leftEye);
	iEyeBox->right(aSample.rightEye);
}

//---------------------------------------------------------------------------
void __fastcall TfrmCalibration::setTrackingStability(bool aStable)
{
	iEyeBox->setTrackingStability(aStable);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TfrmCalibration::onObjectPaint(TObject* aSender, EiUpdateType aUpdateType)
{
	Gdiplus::Rect destRect(0, 0, Width, Height);

	if (!iStaticBitmap)
	{
		aUpdateType = updAll;
		iStaticBitmap = new Gdiplus::Bitmap(Width, Height, iGraphics);
	}

	if (aUpdateType & updStatic)
	{
		Gdiplus::Graphics g(iStaticBitmap);
		Gdiplus::Graphics* graphics = &g;

		//graphics->DrawImage(iBackground, 0, 0);
		Gdiplus::SolidBrush blackBrush(Gdiplus::Color(255, 0, 0, 0));
		Gdiplus::Rect fillRect(0, 0, Width, Height);
		graphics->FillRectangle(&blackBrush, fillRect);

		iBackground->paintTo(graphics);
		iCalibPoints->paintTo(graphics, updStatic);
		iEyeBox->paintTo(graphics, updStatic);

		iCalibPlot->paintTo(graphics);
		iTarget->paintTo(graphics);

		if (aUpdateType == updStatic)
			iGraphics->DrawImage(iStaticBitmap, destRect);
	}

	if (aUpdateType & updNonStatic)
	{
		Gdiplus::Bitmap buffer(Width, Height, iGraphics);
		Gdiplus::Graphics g(&buffer);
		Gdiplus::Graphics* graphics = &g;

		graphics->DrawImage(iStaticBitmap, destRect);

		iCalibPoints->paintTo(graphics, updNonStatic);
		iEyeBox->paintTo(graphics, updNonStatic);

		iFireFly->paintTo(graphics);

		iGraphics->DrawImage(&buffer, 0, 0);
	}
}

//---------------------------------------------------------------------------
void __fastcall TfrmCalibration::onEyeBoxHidden(TObject* aSender)
{
	if (FOnDebug)
		FOnDebug(this, "eyebox hidden");
}

//---------------------------------------------------------------------------
void __fastcall TfrmCalibration::onFireFlyFadingFisnihed(TObject* aSender)
{
	if (iFireFly->IsVisible)
	{
		if (FOnDebug)
			FOnDebug(this, "firefly on");

		iFireFly->startAnimation();
		MoveToNextPoint();
	}
	else
	{
		iFireFly->stopAnimation();
	}
}

//---------------------------------------------------------------------------
void __fastcall TfrmCalibration::onFireFlyMoveFisnihed(TObject* aSender)
{
	if (iFireFly->IsVisible)
	{
		if (FOnDebug)
			FOnDebug(this, "point ready");
		if (FOnPointReady && iCalibPoints->CurrentPointIndex >= 0)
			FOnPointReady(this, iCalibPoints->CurrentPointIndex, iCalibPoints->IsSinglePointMode);

		iFireFly->stopAnimation();
		TiTimeout::run(500, onCalibPointTimeout, &iTimeout);
	}
}

//---------------------------------------------------------------------------
void __fastcall TfrmCalibration::onCalibPointTimeout(TObject* aSender)
{
	if (iTarget->IsVisible)
		iTarget->hide();

	if (iFireFly->IsVisible)
	{
		if (FOnDebug)
			FOnDebug(this, "point ready - 2");

		iIsWaitingToAcceptPoint = true;
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TfrmCalibration::StartCalibration()
{
	if (FOnDebug)
		FOnDebug(this, "start");
	if (FOnStart)
		FOnStart(this);

	iEyeBox->IsVisible = false;

	iFireFly->placeTo(Width / 2, 32);
	iFireFly->fadeIn();

	iCalibPoints->prepare();
}

//---------------------------------------------------------------------------
void __fastcall TfrmCalibration::RestartCalibration(int aRecalibrationPointIndex)
{
	if (FOnDebug)
		FOnDebug(this, "restart");

	iCalibPlot->IsVisible = false;

	bool isSinglePoint = aRecalibrationPointIndex >= 0;
	if (!isSinglePoint)
		iEyeBox->IsVisible = true;
	else
		iFireFly->fadeIn();

	iCalibPoints->prepare(aRecalibrationPointIndex);

	if (isSinglePoint && FOnRecalibrateSinglePoint)
		FOnRecalibrateSinglePoint(this);
}

//---------------------------------------------------------------------------
void __fastcall TfrmCalibration::PointDone()
{
	iIsWaitingToAcceptPoint = false;

	if (FOnPointAccept && iCalibPoints->CurrentPointIndex >= 0)
		FOnPointAccept(this, iCalibPoints->CurrentPointIndex, iCalibPoints->IsSinglePointMode);

	MoveToNextPoint();
}

//---------------------------------------------------------------------------
void __fastcall TfrmCalibration::MoveToNextPoint()
{
	iCalibPoints->lightOnCurrent();

	TiCalibPoint* calibPoint = iCalibPoints->next();
	if (calibPoint)
	{
		iFireFly->setOrientation(calibPoint->X - iFireFly->X, calibPoint->Y - iFireFly->Y);
		iFireFly->moveTo(calibPoint->X, calibPoint->Y);
		iFireFly->startAnimation();

		iTarget->placeTo(calibPoint->X, calibPoint->Y);
		iTarget->show();

		calibPoint->show();
	}
	else
	{
		Finish();

		if (FOnDebug)
			FOnDebug(this, "finished");
		if (FOnFinished)
			FOnFinished(this);

		UpdateCalibPlot();
	}
}

//---------------------------------------------------------------------------
void __fastcall TfrmCalibration::Finish()
{
	iIsWaitingToAcceptPoint = false;

	iFireFly->setOrientation(Width / 2 - iFireFly->X, 32 - iFireFly->Y);
	iFireFly->moveTo(Width / 2, 32);
	iFireFly->fadeOut();
	iFireFly->startAnimation();

	if (iTarget->IsVisible)
		iTarget->hide();
}

//---------------------------------------------------------------------------
void __fastcall TfrmCalibration::Abort()
{
	if (FOnDebug)
		FOnDebug(this, "aborted");
	if (FOnAborted)
		FOnAborted(this);

	if (iTimeout)
		iTimeout->kill();

	Finish();

	iCalibPoints->lightOff();

	iEyeBox->IsVisible = true;
}

//---------------------------------------------------------------------------
void __fastcall TfrmCalibration::Done()
{
	if (FOnDebug)
		FOnDebug(this, "close");
	Close();
}

//---------------------------------------------------------------------------
void __fastcall TfrmCalibration::UpdateCalibPlot()
{
	iCalibPlot->reset();
	for (int i = 0; i < iCalibPoints->Count; i++)
	{
		TiCalibPoint* pt = (*iCalibPoints)[i];
		iCalibPlot->add(new CalibrationPointQualityStruct(i, pt->X, pt->Y,
				pt->X + randInRange(-20, 20), pt->Y + randInRange(-20, 20),
				randInRange(10, 15), randInRange(10, 15),
				0, 0));
	}

	TiCalibPlot::Point worstCalibPoint = iCalibPlot->WorstPoint;
	if (worstCalibPoint.Offset > KMaxAllowedCalibQualityOffset)
	{
		RestartCalibration(worstCalibPoint.ID);
	}
	else
	{
		iBackground->fadeIn();
		//iCalibPlot->IsVisible = true;
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TfrmCalibration::FormCreate(TObject *Sender)
{
	SetBounds(0, 0, Screen->Width, Screen->Height);

	iGraphics = new Gdiplus::Graphics(Handle, false);

	//loadBitmapFromPNG(IDR_BACKGROUND, &iBackground);

	iObjects = new TiAnimationManager();
	iObjects->OnPaint = onObjectPaint;

	iBackground = new TiAnimation(false);
	iBackground->addFrames(IDR_BACKGROUND, 1366, 768);
	iBackground->placeTo(Width / 2, Height / 2);
	iObjects->add(iBackground);

	iCalibPoints = new TiCalibPoints(iObjects, Width, Height);

	iEyeBox = new TiEyeBox(iObjects, TRect(
		(Width - KEyeBoxWidth) / 2, (Height - KEyeBoxHeight) / 2,
		(Width + KEyeBoxWidth) / 2, (Height + KEyeBoxHeight) / 2
	));
	iEyeBox->OnHidden = onEyeBoxHidden;

	iCalibPlot = new TiCalibPlot(iObjects, TRect(
		(Width - KEyeBoxWidth) / 2, (Height - KEyeBoxHeight) / 2,
		(Width + KEyeBoxWidth) / 2, (Height + KEyeBoxHeight) / 2
	), Width, Height);

	iTarget = new TiAnimation(false);
	iTarget->addFrames(IDR_TARGET, 45);
	iObjects->add(iTarget);

	iFireFly = new TiAnimation(false, false);
	iFireFly->addFrames(IDR_FIREFLY, 45);
	iFireFly->LoopAnimation = true;
	iFireFly->RewindAnimationAfterStop = false;
	iFireFly->MoveSpeed = 650;
	iFireFly->placeTo(Width / 2, 32);
	iFireFly->OnFadingFinished = onFireFlyFadingFisnihed;
	iFireFly->OnMoveFinished = onFireFlyMoveFisnihed;
	iObjects->add(iFireFly);
}

//---------------------------------------------------------------------------
void __fastcall TfrmCalibration::FormDestroy(TObject *Sender)
{
	delete iCalibPoints;
	delete iEyeBox;
	delete iCalibPlot;
	//delete iBackground;
	delete iObjects;
	delete iGraphics;

	if (iTimeout)
		delete iTimeout;
}

//---------------------------------------------------------------------------
void __fastcall TfrmCalibration::FormMouseUp(TObject *Sender,
			TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if (Button != mbLeft)
		return;

	if (iEyeBox->IsVisible)
	{
		if (iEyeBox->Start->hitTest(X, Y))
			StartCalibration();
		else if (iEyeBox->Close->hitTest(X, Y))
			Done();
	}
	else if (iCalibPlot->IsVisible)
	{
		if (iCalibPlot->Restart->hitTest(X, Y))
			RestartCalibration();
		else if (iCalibPlot->Close->hitTest(X, Y))
			Done();
		else if (CalibrationPointQualityStruct* focusedCalibPoint = iCalibPlot->calibPointHitTest(X, Y))
			RestartCalibration(focusedCalibPoint->number);
	}
}

//---------------------------------------------------------------------------
void __fastcall TfrmCalibration::FormKeyUp(TObject *Sender, WORD &Key,
			TShiftState Shift)
{
	if (iEyeBox->IsVisible)
	{
		if (Key == VK_SPACE)
			StartCalibration();
		if (Key == VK_ESCAPE)
			Done();
	}
	else if (iCalibPlot->IsVisible)
	{
		if (Key == VK_SPACE)
			RestartCalibration();
		else if (Key == VK_ESCAPE)
			Done();
	}
	else
	{
		if (Key == VK_SPACE)
		{
			if (iIsWaitingToAcceptPoint)
				PointDone();
		}
		else if (Key == VK_ESCAPE)
		{
			if (iCalibPoints->Current)
				Abort();
			else
				Done();
		}
	}
}

//---------------------------------------------------------------------------
void __fastcall TfrmCalibration::FormMouseMove(TObject *Sender,
			TShiftState Shift, int X, int Y)
{
	double x = (double(X) / Width - 0.5) / 2 + 0.5;
	double y = (double(Y) / Height - 0.5) / 2 + 0.5;

	EyeDataStruct left(0.0,0.0,0.0,X < 5 ? -1.0 : x - 0.1, X < 5 ? -1.0 : y, X);
	EyeDataStruct right(0.0,0.0,0.0,Y < 5 ? -1.0 : x + 0.1, Y < 5 ? -1.0 : y, Y);
	setSample(	SampleStruct(__int64(0), left, right) );

	iCalibPlot->calibPointHitTest(X, Y);

	if (iEyeBox->IsVisible)
	{
		iEyeBox->Start->AnimationIndex = iEyeBox->Start->hitTest(X, Y) ? 1 : 0;
		iEyeBox->Close->AnimationIndex = iEyeBox->Close->hitTest(X, Y) ? 1 : 0;
	}
	else if (iCalibPlot->IsVisible)
	{
		iCalibPlot->Restart->AnimationIndex = iCalibPlot->Restart->hitTest(X, Y) ? 1 : 0;
		iCalibPlot->Close->AnimationIndex = iCalibPlot->Close->hitTest(X, Y) ? 1 : 0;
	}
}
//---------------------------------------------------------------------------

