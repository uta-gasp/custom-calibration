//---------------------------------------------------------------------------
#include "CustomCalibration.h"
#include "assets.h"

#ifndef RET_SUCCESS
#include "myGazeAPI.h"
#endif

//---------------------------------------------------------------------------
#include <math.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
static Gdiplus::GdiplusStartupInput gdiplusStartupInput;
static ULONG_PTR m_gdiplusToken = NULL;

//TfrmCalibration* frmCalibration;

const int KEyeBoxWidth = 160;
const int KEyeBoxHeight = 120;

const int KMaxAllowedCalibQualityOffset = 40;

//---------------------------------------------------------------------------
__fastcall TfrmCustomCalibration::TfrmCustomCalibration(TComponent* aOwner) :
		TForm(aOwner),
		iTimeout(NULL),
		iPointAcceptTimeout(NULL),
		iIsWaitingToAcceptPoint(false),
		iGame(NULL),
		iStaticBitmap(NULL),
		FOnDebug(NULL),
		FOnStart(NULL),
		FOnReadyToCalibrate(NULL),
		FOnRecalibrateSinglePoint(NULL),
		FOnPointReady(NULL),
		FOnPointAccepted(NULL),
		FOnPointAborted(NULL),
		FOnFinished(NULL),
		FOnAborted(NULL)
{
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	randomize();
}

//---------------------------------------------------------------------------
__fastcall TfrmCustomCalibration::~TfrmCustomCalibration()
{
	if (iStaticBitmap)
		delete iStaticBitmap;

	Gdiplus::GdiplusShutdown(m_gdiplusToken);
}

//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::setSample(SampleStruct& aSample)
{
	iEyeBox->left(aSample.leftEye);
	iEyeBox->right(aSample.rightEye);
}

//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::setTrackingStability(bool aStable)
{
	iEyeBox->setTrackingStability(aStable);
}

//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::clearPoints()
{
	iCalibPoints->clear();
}

//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::addPoint(CalibrationPointStruct& aPoint)
{
	iCalibPoints->add(aPoint);
}

//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::nextPoint(int aPointNumber, bool aPreviousDone)
{
	MoveToNextPoint(aPointNumber, aPreviousDone);
}

//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::reportCalibrationResult(int aNumber,
		CalibrationPointQualityStruct* aLeft,
		CalibrationPointQualityStruct* aRight)
{
	iCalibPlot->add(aNumber, aLeft, aRight);
}

//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::processCalibrationResult()
{
	UpdateCalibPlot();

	/*
	for (int i = 0; i < iCalibPoints->Count; i++)
	{
		TiCalibPoint* pt = (*iCalibPoints)[i];
		iCalibPlot->add(new CalibrationPointQualityStruct(i, pt->X, pt->Y,
				pt->X + randInRange(-20, 20), pt->Y + randInRange(-20, 20),
				randInRange(10, 15), randInRange(10, 15),
				0, 0));
	} */

}

//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::loadSettings(TiXML_INI* aStorage)
{
	iGame->BestTime = aStorage->getValue("Game", "BestTime", iGame->BestTime);
}

//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::saveSettings(TiXML_INI* aStorage)
{
	aStorage->putValue("Game", "BestTime", iGame->BestTime);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::onObjectPaint(TObject* aSender, EiUpdateType aUpdateType)
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

		iGame->paintTo(graphics);
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
void __fastcall TfrmCustomCalibration::onEyeBoxHidden(TObject* aSender)
{
	if (FOnDebug)
		FOnDebug(this, "eyebox hidden");
}

//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::onFireFlyFadingFisnihed(TObject* aSender)
{
	if (iFireFly->IsVisible)
	{
		if (FOnDebug)
			FOnDebug(this, "firefly on");

		iFireFly->startAnimation();

		if (FOnReadyToCalibrate)
			FOnReadyToCalibrate(this);
		//MoveToNextPoint();
	}
	else
	{
		iFireFly->stopAnimation();
	}
}

//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::onFireFlyMoveFisnihed(TObject* aSender)
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
void __fastcall TfrmCustomCalibration::onCalibPointTimeout(TObject* aSender)
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
void __fastcall TfrmCustomCalibration::onBackgroundFadingFisnihed(TObject* aSender)
{
	if (iBackground->IsVisible)
	{
		iGame->start(10);
	}
	else
	{
		Done();
	}
}

//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::onGameFisnihed(TObject* aSender)
{
	iBackground->FadingDuration = 400;
	iBackground->fadeOut();
	iCalibPoints->fadeOut();
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::StartCalibration()
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
void __fastcall TfrmCustomCalibration::RestartCalibration(int aRecalibrationPointNumber)
{
	if (FOnDebug)
		FOnDebug(this, "restart");

	iCalibPlot->IsVisible = false;

	iCalibPoints->prepare(aRecalibrationPointNumber);

	bool isSinglePoint = aRecalibrationPointNumber > 0;
	if (isSinglePoint)
	{
		iFireFly->fadeIn();
		//if (!iFireFly->fadeIn())
		//	MoveToNextPoint();
		if (FOnRecalibrateSinglePoint)
			FOnRecalibrateSinglePoint(this, aRecalibrationPointNumber, true);
	}
	else
	{
		iEyeBox->IsVisible = true;
	}
}

//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::PointDone(TObject* aSender)
{
	iIsWaitingToAcceptPoint = false;

	if (!iPointAcceptTimeout)
	{
		TiTimeout::run(1000, PointDone, &iPointAcceptTimeout);
	}
	else
	{
		if (FOnDebug)
			FOnDebug(this, "...no acceptance");
		TiTimeout::run(1000, PointAbort, &iPointAcceptTimeout);
	}

	if (FOnPointAccepted && iCalibPoints->CurrentPointIndex >= 0)
		FOnPointAccepted(this, iCalibPoints->CurrentPointIndex, iCalibPoints->IsSinglePointMode);

	//MoveToNextPoint();
}

//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::PointAbort(TObject* aSender)
{
	if (FOnDebug)
		FOnDebug(this, "...abort point");
	if (FOnPointAborted)
		FOnPointAborted(this, iCalibPoints->CurrentPointIndex, iCalibPoints->IsSinglePointMode);
}

//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::MoveToNextPoint(int aPointNumber, bool aPreviousDone)
{
	if (iPointAcceptTimeout)
		iPointAcceptTimeout->kill();

	if (aPreviousDone)
		iCalibPoints->lightOnCurrent();

	TiCalibPoint* calibPoint = iCalibPoints->next(aPointNumber);
	if (calibPoint)
	{
		bool isAtPlaceAlready = !iFireFly->moveTo(calibPoint->X, calibPoint->Y);
		if (!isAtPlaceAlready)
		{
			iFireFly->setOrientation(calibPoint->X - iFireFly->X, calibPoint->Y - iFireFly->Y);
			iFireFly->startAnimation();
		}
		else
		{
			iCalibPoints->lightOffCurrent();
			onFireFlyMoveFisnihed(NULL);
		}

		iTarget->placeTo(calibPoint->X, calibPoint->Y);
		iTarget->show();

		calibPoint->show(isAtPlaceAlready);
	}
	else
	{
		Finish();
		iCalibPlot->reset();

		if (FOnDebug)
			FOnDebug(this, "finished");
		if (FOnFinished)
			FOnFinished(this);

		//UpdateCalibPlot();
	}
}

//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::Finish()
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
void __fastcall TfrmCustomCalibration::Abort()
{
	if (FOnDebug)
		FOnDebug(this, "aborted");
	if (FOnAborted)
		FOnAborted(this);

	if (iTimeout)
		iTimeout->kill();
	if (iPointAcceptTimeout)
		iPointAcceptTimeout->kill();

	Finish();

	iCalibPoints->lightOff();

	TiTimeout::run(1000, Done);

	//iEyeBox->IsVisible = true;
}

//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::Done(TObject* aSender)
{
	if (FOnDebug)
		FOnDebug(this, "close");

	if (FormState.Contains(fsModal))
		ModalResult = mrOk;
	else
		Close();
}

//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::UpdateCalibPlot()
{
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
void __fastcall TfrmCustomCalibration::FormCreate(TObject *Sender)
{
	SetBounds(0, 0, Screen->Width, Screen->Height);

	iGraphics = new Gdiplus::Graphics(Handle, false);

	//loadBitmapFromPNG(IDR_BACKGROUND, &iBackground);

	iObjects = new TiAnimationManager();
	iObjects->OnPaint = onObjectPaint;

	iBackground = new TiAnimation(false);
	iBackground->addFrames(IDR_BACKGROUND, 1366, 768);
	iBackground->placeTo(Width / 2, Height / 2);
	iBackground->OnFadingFinished = onBackgroundFadingFisnihed;
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

	iGame = new TiGame(iObjects);
}

//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::FormDestroy(TObject *Sender)
{
	delete iCalibPoints;
	delete iEyeBox;
	delete iCalibPlot;
	delete iGame;
	//delete iBackground;
	delete iObjects;
	delete iGraphics;

	if (iTimeout)
		delete iTimeout;
}

//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::FormMouseUp(TObject *Sender,
			TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if (Button != mbLeft)
		return;

	if (iEyeBox->IsVisible)
	{
		if (iEyeBox->Start->hitTest(X, Y))
			StartCalibration();
		//else if (iEyeBox->Close->hitTest(X, Y))
		//	Done();
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
	else if (iGame->IsRunning && !iTimeout)
	{
		bool isFinished = iGame->click(X, Y);
		if (isFinished)
			TiTimeout::run(5000, onGameFisnihed, &iTimeout);
	}
}

//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::FormKeyUp(TObject *Sender, WORD &Key,
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
void __fastcall TfrmCustomCalibration::FormMouseMove(TObject *Sender,
			TShiftState Shift, int X, int Y)
{
/*
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
	*/
}
//---------------------------------------------------------------------------
