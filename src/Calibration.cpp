//---------------------------------------------------------------------------
#include "Calibration.h"
#include "assets.h"
#include "fakeMyGaze.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
static Gdiplus::GdiplusStartupInput gdiplusStartupInput;
static ULONG_PTR m_gdiplusToken = NULL;

TfrmCalibration* frmCalibration;

const int KEyeBoxWidth = 160;
const int KEyeBoxHeight = 120;

//---------------------------------------------------------------------------
__fastcall TfrmCalibration::TfrmCalibration(TComponent* aOwner) :
		TForm(aOwner),
		iTimeout(NULL),
		FOnDebug(NULL),
		FOnStart(NULL),
		FOnRecalibrateSinglePoint(NULL),
		FOnPointReady(NULL),
		FOnPointAccept(NULL),
		FOnFinished(NULL),
		FOnAborted(NULL)
{
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
}

//---------------------------------------------------------------------------
__fastcall TfrmCalibration::~TfrmCalibration()
{
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
void __fastcall TfrmCalibration::onObjectPaint(TObject* aSender)
{
	Gdiplus::Rect destRect(0, 0, Width, Height);

//	Gdiplus::Bitmap* background;
//	loadBitmapFromPNG(IDR_BACKGROUND, &background);
//	Gdiplus::Graphics g(background);

	Gdiplus::Bitmap buffer(Width, Height, iGraphics);
	Gdiplus::Graphics g(&buffer);

	g.DrawImage(iBackground, destRect);

	iCalibPoints->paintTo(&g);

	iEyeBox->paintTo(&g);
	iCalibPlot->paintTo(&g);
	iFireFly->paintTo(&g);

//	iGraphics->DrawImage(background, destRect,
//			0, 0, Width, Height,
//			Gdiplus::UnitPixel);
	iGraphics->DrawImage(&buffer, destRect);
//	delete background;
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

		iFireFly->AnimationIndex = 1;
		TiTimeout::run(2000, onCalibPointTimeout, &iTimeout);
	}
}

//---------------------------------------------------------------------------
void __fastcall TfrmCalibration::onCalibPointTimeout(TObject* aSender)
{
	if (iFireFly->IsVisible)
	{
		if (FOnDebug)
			FOnDebug(this, "point accepted");
		if (FOnPointAccept && iCalibPoints->CurrentPointIndex >= 0)
			FOnPointAccept(this, iCalibPoints->CurrentPointIndex, iCalibPoints->IsSinglePointMode);

		MoveToNextPoint();
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
void __fastcall TfrmCalibration::RestartCalibration(CalibrationPointQualityStruct* aCalibPoint)
{
	if (FOnDebug)
		FOnDebug(this, "restart");

	iCalibPlot->IsVisible = false;

	if (!aCalibPoint)
		iEyeBox->IsVisible = true;
	else
		iFireFly->fadeIn();

	iCalibPoints->prepare(aCalibPoint ? aCalibPoint->number : -1);

	if (iCalibPoints->IsSinglePointMode && FOnRecalibrateSinglePoint)
		FOnRecalibrateSinglePoint(this);
}

//---------------------------------------------------------------------------
void __fastcall TfrmCalibration::MoveToNextPoint()
{
	iCalibPoints->lightOnCurrent();

	TiCalibPoint* calibPoint = iCalibPoints->next();
	if (calibPoint)
	{
		iFireFly->moveTo(calibPoint->X, calibPoint->Y);
		iFireFly->AnimationIndex = 0;
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
	iFireFly->moveTo(Width / 2, 32);
	iFireFly->fadeOut();
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
	iCalibPlot->IsVisible = true;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TfrmCalibration::FormCreate(TObject *Sender)
{
	iGraphics = new Gdiplus::Graphics(Handle, false);

	loadBitmapFromPNG(IDR_BACKGROUND, &iBackground);

	iObjects = new TiAnimationManager();
	iObjects->OnPaint = onObjectPaint;

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

	iFireFly = new TiAnimation(false);
	iFireFly->addFrames(IDR_FIREFLY_RED, 64);
	iFireFly->addFrames(IDR_FIREFLY_BLUE, 64);
	iFireFly->LoopAnimation = true;
	iFireFly->MoveSpeed = 1000;
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
	delete iBackground;
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
			RestartCalibration(focusedCalibPoint);
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
		if (Key == VK_ESCAPE)
			Abort();
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

