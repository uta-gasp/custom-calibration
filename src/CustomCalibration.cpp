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
const double KMinAllowedCalibQualityValue = 0.5;

const double KMouseGazeCorrectionFactor = 0; //0.05;

const String KIniGame = "Game";
const String KIniBestScore = "BestScore";
const String KIniBestScoreDate = "BestScoreDate";
const String KIniResults = "Results";
const String KIniResult = "Result";
const String KIniResultDate = "Date";
const String KIniResultScore = "Score";
const String KIniResultIsBest = "IsBest";

//---------------------------------------------------------------------------
__fastcall TfrmCustomCalibration::TfrmCustomCalibration(TComponent* aOwner) :
		TForm(aOwner),
		iTimeout(NULL),
		iPointAcceptTimeout(NULL),
		iIsWaitingToAcceptPoint(false),
		iLastPointAborted(false),
		iGame(NULL),
		iStaticBitmap(NULL),
		iGameAfterCalibration(true),
		iGazeControlInGame(true),
		iMouseInitialPosition(-1, -1),
		iAttractor(NULL),
		iAttractorType(ATTRACTOR_CIRCLE),
		FOnEvent(NULL),
		FOnSample(NULL),
		FOnStart(NULL),
		FOnReadyToCalibrate(NULL),
		FOnRecalibrateSinglePoint(NULL),
		FOnPointReady(NULL),
		FOnPointAccepted(NULL),
		FOnPointAborted(NULL),
		FOnFinished(NULL),
		FOnAborted(NULL),
		FOnGameStarted(NULL),
		FOnGameFinished(NULL)
{
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	iSetupBackgroundColor = Gdiplus::Color(255, 0, 0, 0);
	iCalibrationBackgroundColor = Gdiplus::Color(255, 0, 0, 0);
	iBackgroundColor = iSetupBackgroundColor;

	randomize();

	iCalibQualityEstimator = new TiCalibQualityEstimator();
	iCalibQualityEstimator->OnEvent = FOnEvent;

#ifdef __DEBUG
	FormStyle = __DEBUG ? fsNormal : fsStayOnTop;
#else
	FormStyle = fsStayOnTop;
#endif
}

//---------------------------------------------------------------------------
__fastcall TfrmCustomCalibration::~TfrmCustomCalibration()
{
	delete iCalibQualityEstimator;

	if (iStaticBitmap)
		delete iStaticBitmap;

	Gdiplus::GdiplusShutdown(m_gdiplusToken);
}

//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::setSample(SampleStruct& aSample)
{
	iEyeBox->left(aSample.leftEye);
	iEyeBox->right(aSample.rightEye);

	if (iGame->IsRunning)
	{
		int dx = KMouseGazeCorrectionFactor * (Mouse->CursorPos.x - iMouseInitialPosition.x);
		int dy = KMouseGazeCorrectionFactor * (Mouse->CursorPos.y - iMouseInitialPosition.y);
		iGame->placePointer(aSample.leftEye.gazeX, aSample.leftEye.gazeY, dx, dy);

		TiCalibQualityEstimator::TiPointI cqePoint(aSample.leftEye.gazeX, aSample.leftEye.gazeY);
		iCalibQualityEstimator->addSample(cqePoint);

		if (FOnSample)
			FOnSample(this, aSample.leftEye.gazeX + dx, aSample.leftEye.gazeY + dy);
	}
}

//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::setTrackingStability(bool aStable)
{
	iEyeBox->setTrackingStability(aStable);
}

//---------------------------------------------------------------------------
int __fastcall TfrmCustomCalibration::showModal(bool aSkipCalibration)
{
	if (aSkipCalibration)
		playGame();
	else
		showEyeBox();

	return ShowModal();
}

//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::showEyeBox()
{
	iEyeBox->IsVisible = true;
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
void __fastcall TfrmCustomCalibration::nextPoint(int aPointNumber)
{
	MoveToNextPoint(aPointNumber);
}

//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::reportCalibrationResult(int aNumber,
		CalibrationPointQualityStruct& aLeft,
		CalibrationPointQualityStruct& aRight)
{
	iCalibPlot->add(aNumber, aLeft, aRight);
}

//---------------------------------------------------------------------------
bool __fastcall TfrmCustomCalibration::processCalibrationResult()
{
	bool finished = true;

	if (FOnEvent)
	{
		TStringList* calibQuality = new TStringList();
		iCalibPlot->log(calibQuality);
		for (int i = 0; i < calibQuality->Count; i++)
		{
			FOnEvent(this, String("quality\t") + calibQuality->Strings[i]);
		}

		delete calibQuality;
	}

	TiCalibPlot::Point worstCalibPoint = iCalibPlot->WorstPoint;
	if (FOnEvent)
		FOnEvent(this, String().sprintf("worst quality\t%d\t%.1f\t%.3f",
				worstCalibPoint.ID - 1, worstCalibPoint.Offset, worstCalibPoint.Quality));

	if (worstCalibPoint.Offset > KMaxAllowedCalibQualityOffset ||
			worstCalibPoint.Quality < KMinAllowedCalibQualityValue)
	{
		finished = false;
		RestartCalibration(worstCalibPoint.ID);
	}
	else
	{
		iCalibPoints->fadeOut();
		if (GameAfterCalibration)
			TiTimeout::run(500, playGame, &iTimeout);
		else
			TiTimeout::run(1000, Done, &iTimeout);
	}

	return finished;
}

//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::playGame(TObject* aSender)
{
	Cursor = crNone;
	iBackground->fadeIn();

	TiTimeout::run(700, ShowGameInstructions);
}

//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::loadSettings(TiXML_INI* aStorage)
{
	iGame->BestScore = aStorage->getValue(KIniGame, KIniBestScore, (long)iGame->BestScore);
	iGame->BestScoreDate = aStorage->getValue(KIniGame, KIniBestScoreDate, iGame->BestScoreDate);
}

//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::saveSettings(TiXML_INI* aStorage)
{
	if (!iGame->Duration)
		return;

	aStorage->putValue(KIniGame, KIniBestScore, (long)iGame->BestScore);
	aStorage->putValue(KIniGame, KIniBestScoreDate, iGame->BestScoreDate);

	if (aStorage->openNode(KIniGame, false))
	{
		TStringList* results = new TStringList();
		aStorage->getAllNodes(KIniResults, results);
		int id = results->Count;
		delete results;

		if (aStorage->openNode(KIniResults, true))
		{
			aStorage->putValue(KIniResult, KIniResultDate, TDateTime::CurrentDateTime().DateTimeString(), id);
			aStorage->putValue(KIniResult, KIniResultScore, (long)iGame->Score, id);
			aStorage->putValue(KIniResult, KIniResultIsBest, iGame->IsBestScore, id);
			aStorage->closeNode();
		}

		aStorage->closeNode();
	}
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
		Gdiplus::SolidBrush backgroundBrush(iBackgroundColor);
		Gdiplus::Rect fillRect(0, 0, Width, Height);
		graphics->FillRectangle(&backgroundBrush, fillRect);

		iBackground->paintTo(graphics);
		iCalibPoints->paintTo(graphics, updStatic);
		iEyeBox->paintTo(graphics, updStatic);

		iCalibPlot->paintTo(graphics);
		iTarget->paintTo(graphics);

		if (aUpdateType == updStatic)
			iGraphics->DrawImage(iStaticBitmap, destRect);

		iGame->paintTo(graphics, updStatic);
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
		iCircle->paintTo(graphics);
		iGame->paintTo(graphics, updNonStatic);

		iGraphics->DrawImage(&buffer, 0, 0);
	}
}

//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::onEyeBoxFadingTransition(TObject* aSender, double iAlpha)
{
	iBackgroundColor = Gdiplus::Color(
		iSetupBackgroundColor.GetR() * iAlpha + iCalibrationBackgroundColor.GetR() * (1 - iAlpha),
		iSetupBackgroundColor.GetG() * iAlpha + iCalibrationBackgroundColor.GetG() * (1 - iAlpha),
		iSetupBackgroundColor.GetB() * iAlpha + iCalibrationBackgroundColor.GetB() * (1 - iAlpha)
	);
}


//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::onEyeBoxHidden(TObject* aSender)
{
	//if (FOnEvent)
	//	FOnEvent(this, "eyebox hidden");
}

//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::onAttractorFadingFisnihed(TObject* aSender)
{
	if (iAttractor->IsVisible)
	{
		if (FOnEvent)
			FOnEvent(this, "attractor appeared");

		if (iAttractor == iFireFly)
			iFireFly->startAnimation();

		if (!iCalibPoints->Current && FOnReadyToCalibrate)
			FOnReadyToCalibrate(this);
	}
	else
	{
		iAttractor->stopAnimation();
	}
}

//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::onAttractorMoveFisnihed(TObject* aSender)
{
	if (iAttractor->IsVisible)
	{
		if (FOnEvent)
			FOnEvent(this, String().sprintf("arrived to point\t%d\t%d %d", iCalibPoints->CurrentPointIndex,
					iCalibPoints->Current->X, iCalibPoints->Current->Y));
		if (FOnPointReady && iCalibPoints->CurrentPointIndex >= 0)
			FOnPointReady(this, iCalibPoints->CurrentPointIndex, iCalibPoints->IsSinglePointMode);

		if (iAttractor == iFireFly)
			iFireFly->stopAnimation();
		else if (iAttractor == iCircle)
			iCircle->startAnimation();

		TiTimeout::run(500, onCalibPointTimeout, &iTimeout);
	}
}

//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::onCalibPointTimeout(TObject* aSender)
{
	if (iTarget->IsVisible)
		iTarget->hide();

	if (iAttractor->IsVisible)
	{
		if (FOnEvent)
			FOnEvent(this, String().sprintf("ready to accept\t%d\t%d %d", iCalibPoints->CurrentPointIndex,
					iCalibPoints->Current->X, iCalibPoints->Current->Y));

		iIsWaitingToAcceptPoint = true;
	}
}

//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::onBackgroundFadingFisnihed(TObject* aSender)
{
	if (!iBackground->IsVisible)
		Done();
}

//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::onGameSelect(TObject* aSender, int aTargetX, int aTargetY)
{
	iCalibQualityEstimator->addSelection(aTargetX, aTargetY);
}

//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::onGameFisnihed(TObject* aSender)
{
	if (FOnEvent)
	{
		SiQuality quality;
		int points = iCalibQualityEstimator->estimate(quality);

		if (points)
		{
			FOnEvent(this, quality.toString("verification result"));
		}
		else
		{
			FOnEvent(this, "verification result\t0");
		}
	}

	if (FOnGameFinished)
		FOnGameFinished(this);

	TiTimeout::run(4000, FadeOut, &iTimeout);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::StartCalibration()
{
	if (FOnEvent)
		FOnEvent(this, String().sprintf("calibration start\t%s\t%s",
				iGameAfterCalibration ? "game" : "no game",
				iGazeControlInGame ? "gaze" : "mouse" ));
	if (FOnStart)
		FOnStart(this);

	Cursor = crNone;

	iEyeBox->IsVisible = false;

	switch (iAttractorType)
	{
		case ATTRACTOR_FIREFLY:
			iAttractor = iFireFly;
			iCalibrationBackgroundColor = Gdiplus::Color(255, 0, 0, 0);
			break;
		case ATTRACTOR_CIRCLE:
			iAttractor = iCircle;
			iCalibrationBackgroundColor = Gdiplus::Color(255, 0xDD, 0xDD, 0xDD);
			break;
	}

	iAttractor->placeTo(Width / 2, 32);
	iAttractor->fadeIn();

	iCalibPoints->prepare();
	iLastPointAborted = false;
}

//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::RestartCalibration(int aRecalibrationPointNumber)
{
	if (FOnEvent)
		FOnEvent(this, String().sprintf("restart\t%d", aRecalibrationPointNumber - 1));

	iCalibPlot->IsVisible = false;

	iCalibPoints->prepare(aRecalibrationPointNumber);
	iLastPointAborted = false;

	bool isSinglePoint = aRecalibrationPointNumber > 0;
	if (isSinglePoint)
	{
		iAttractor->fadeIn();
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
		if (FOnEvent)
			FOnEvent(this, "no acceptance");
		TiTimeout::run(1000, PointAbort, &iPointAcceptTimeout);
	}

	if (iCalibPoints->CurrentPointIndex >= 0)
	{
		if (FOnEvent)
			FOnEvent(this, String().sprintf("point accepted\t%d\t%d %d", iCalibPoints->CurrentPointIndex,
					iCalibPoints->Current->X, iCalibPoints->Current->Y));
		if (FOnPointAccepted)
			FOnPointAccepted(this, iCalibPoints->CurrentPointIndex, iCalibPoints->IsSinglePointMode);
	}
}

//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::PointAbort(TObject* aSender)
{
	iLastPointAborted = true;
	if (FOnEvent)
		FOnEvent(this, String().sprintf("point aborted\t%d", iCalibPoints->CurrentPointIndex));
	if (FOnPointAborted)
		FOnPointAborted(this, iCalibPoints->CurrentPointIndex, iCalibPoints->IsSinglePointMode);
}

//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::MoveToNextPoint(int aPointNumber)
{
	if (iPointAcceptTimeout)
		iPointAcceptTimeout->kill();

	if (!iLastPointAborted)
		iCalibPoints->lightOnCurrent();

	iLastPointAborted = false;

	TiCalibPoint* calibPoint = iCalibPoints->next(aPointNumber);
	if (calibPoint)
	{
		if (FOnEvent)
			FOnEvent(this, "move to next point");

		bool isAtPlaceAlready = !iAttractor->moveTo(calibPoint->X, calibPoint->Y);
		if (!isAtPlaceAlready)
		{
			if (iAttractor == iFireFly)
			{
				iFireFly->setOrientation(calibPoint->X - iFireFly->X, calibPoint->Y - iFireFly->Y);
				iFireFly->startAnimation();
			}
			else if (iAttractor == iCircle)
			{
				iCircle->stopAnimation();
			}
		}
		else
		{
			onAttractorMoveFisnihed(NULL);
		}

		iCalibPoints->lightOffCurrent();

		iTarget->placeTo(calibPoint->X, calibPoint->Y);

		if (iAttractor == iFireFly)
		{
			iTarget->show();
			calibPoint->show(isAtPlaceAlready);
		}
	}
	else
	{
		Finish();
		iCalibPlot->reset();

		if (FOnEvent)
			FOnEvent(this, "calibration finished");

		tmrKostyl->Enabled = true;
	}
}

//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::Finish()
{
	iIsWaitingToAcceptPoint = false;

	if (iAttractor == iFireFly)
	{
		iFireFly->setOrientation(Width / 2 - iFireFly->X, 32 - iFireFly->Y);
		iFireFly->startAnimation();
	}
	else if (iAttractor == iCircle)
	{
		iCircle->stopAnimation();
	}

	iAttractor->moveTo(Width / 2, 32);
	iAttractor->fadeOut();

	if (iTarget->IsVisible)
		iTarget->hide();
}

//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::Abort()
{
	if (FOnEvent)
		FOnEvent(this, "calibration aborted");
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
void __fastcall TfrmCustomCalibration::ShowGameInstructions(TObject* aSender)
{
	iGame->showInstruction();
}

//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::StartGame(TObject* aSender)
{
	if (!iGazeControlInGame)
		Cursor = crDefault;

	iGame->start(10);

	if (FOnGameStarted)
		FOnGameStarted(this);
}

//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::FadeOut(TObject* aSender)
{
	iBackground->FadingDuration = 400;
	iBackground->fadeOut();
}

//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::Done(TObject* aSender)
{
	if (FOnEvent)
		FOnEvent(this, "done");

	if (FormState.Contains(fsModal))
		ModalResult = mrOk;
	else
		Close();
}

//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::SetOnEvent(FiOnEvent aFOnEvent)
{
	FOnEvent = aFOnEvent;
	if (iGame)
		iGame->OnEvent = FOnEvent;
	iCalibQualityEstimator->OnEvent = FOnEvent;
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
	), TiSize( Width, Height ));
	iEyeBox->OnHidden = onEyeBoxHidden;
	iEyeBox->Background->OnFadingTransition = onEyeBoxFadingTransition;

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
	iFireFly->MoveSpeed = 1000;
	iFireFly->placeTo(Width / 2, 32);
	iFireFly->OnFadingFinished = onAttractorFadingFisnihed;
	iFireFly->OnMoveFinished = onAttractorMoveFisnihed;
	iObjects->add(iFireFly);

	iCircle = new TiAnimation(false, false);
	iCircle->addFrames(IDR_CIRCLE, 45);
	iCircle->LoopAnimation = true;
	iCircle->RewindAnimationAfterStop = true;
	iCircle->AllowAnimationReversion = true;
	iCircle->MoveSpeed = 1000;
	iCircle->placeTo(Width / 2, 32);
	iCircle->OnFadingFinished = onAttractorFadingFisnihed;
	iCircle->OnMoveFinished = onAttractorMoveFisnihed;
	iObjects->add(iCircle);

	iGame = new TiGame(iObjects, TiSize( Width, Height ));
	iGame->OnEvent = FOnEvent;
	iGame->OnSelect = onGameSelect;
	iGame->OnFinished = onGameFisnihed;
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
	else if (iGame->IsInstructionVisible)
	{
		StartGame(NULL);
	}
	else if (iGame->IsRunning && !iTimeout)
	{
		if (iGazeControlInGame)
			iGame->click();
		else
			iGame->click(X, Y);
	}
	else if (iIsWaitingToAcceptPoint)
	{
		PointDone();
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
		else if (Key == VK_ESCAPE)
		{
			if (FOnEvent)
				FOnEvent(this, "calibration skipped, exiting");
			Done();
		}
		else if (Key == VK_RETURN)
		{
			if (FOnEvent)
				FOnEvent(this, "calibration skipped");
			iEyeBox->IsVisible = false;
			playGame();
		}
	}
	else if (iCalibPlot->IsVisible)
	{
		if (Key == VK_SPACE)
			RestartCalibration();
		if (Key == VK_ESCAPE)
			Done();
	}
	else if (iGame->IsInstructionVisible)
	{
		if (Key == VK_SPACE)
			StartGame(NULL);
		if (Key == VK_ESCAPE)
		{
			if (FOnEvent)
				FOnEvent(this, "game skipped");
			Done();
		}
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
			if (iGame->IsRunning)
				if (FOnEvent)
					FOnEvent(this, "game aborted");

			if (iCalibPoints->Current)
				Abort();
			else
				Done();
		}
	}
}

//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::tmrKostylTimer(TObject *Sender)
{
	tmrKostyl->Enabled = false;

	if (FOnFinished)
		FOnFinished(this);
}

//---------------------------------------------------------------------------
void __fastcall TfrmCustomCalibration::FormMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
	if (iGame->IsRunning && iMouseInitialPosition.x < 0)
	{
		iMouseInitialPosition.x = Mouse->CursorPos.x;
		iMouseInitialPosition.y = Mouse->CursorPos.y;
	}
}

//---------------------------------------------------------------------------

