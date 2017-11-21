//---------------------------------------------------------------------------
#include "ProfiledGame.h"
#include "assets_pg.h"

#ifndef RET_SUCCESS
#include "myGazeAPI.h"
#endif

//---------------------------------------------------------------------------
#include <math.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
using namespace ProfiledGame;

//---------------------------------------------------------------------------
static Gdiplus::GdiplusStartupInput gdiplusStartupInput;
static ULONG_PTR m_gdiplusToken = NULL;

//---------------------------------------------------------------------------
const int KViewportWidth = 1366;
const int KViewportHeight = 768;

const Gdiplus::Color KBackgroundColor(255, 0, 0, 0);

const int KMaxAllowedCalibQualityOffset = 40;
const double KMinAllowedCalibQualityValue = 0.5;

const int KVerificationPointCountX = 3;
const int KVerificationPointCountY = 2;

const int KVerificationCellMargin = 80;

//---------------------------------------------------------------------------
__fastcall TiProfiledGame::TiProfiledGame(TComponent* aOwner) :
		TForm(aOwner),
		iTimeout(NULL),
		iStaticBitmap(NULL),
		iProfileEditor(NULL),
		iIsVerifying(false),
		iIsRecalibrating(false),
		iLevelInstruction(NULL),
		iLevelLegend(NULL),
		iPositionTracker(NULL),
		FOnEvent(NULL),
		FOnSample(NULL),
		FOnStart(NULL),
		FOnReadyToCalibrate(NULL),
		FOnRecalibratePoint(NULL),
		FOnPointReady(NULL),
		FOnPointAccepted(NULL),
		FOnFinished(NULL),
		FOnAborted(NULL),
		FOnVerifStarted(NULL),
		FOnVerifFinished(NULL)
{
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	//Cursor = crNone;

	randomize();

	iCalibQualityEstimator = new TiCalibQualityEstimator();
	iCalibQualityEstimator->OnEvent = FOnEvent;

	iCalibQuality = new TiCalibQuality();

#ifndef _DEBUG
	FormStyle = fsStayOnTop;
#endif
}

//---------------------------------------------------------------------------
__fastcall TiProfiledGame::~TiProfiledGame()
{
	delete iCalibQuality;
	delete iCalibQualityEstimator;

	if (iStaticBitmap)
		delete iStaticBitmap;

	Gdiplus::GdiplusShutdown(m_gdiplusToken);
}

//---------------------------------------------------------------------------
void __fastcall TiProfiledGame::setSample(SampleStruct& aSample)
{
	iEyeBox->left(aSample.leftEye);
	iEyeBox->right(aSample.rightEye);

	TiCalibQualityEstimator::TiPointI cqePoint(aSample.leftEye.gazeX, aSample.leftEye.gazeY);
	iCalibQualityEstimator->addSample(cqePoint);

	if (iPositionTracker)
		if (iPositionTracker->isPositionWrong(aSample) && iCalibPoints->IsVisible)
			iCalibPoints->pause();

	if (FOnSample)
		FOnSample(this, aSample.leftEye.gazeX, aSample.leftEye.gazeY);
}

//---------------------------------------------------------------------------
int __fastcall TiProfiledGame::showModal(String& aName)
{
	showLogin(aName);

	return ShowModal();
}

//---------------------------------------------------------------------------
void __fastcall TiProfiledGame::showLogin(String& aName)
{
	if (aName.Length() > 0)
	{
		memName->Text = aName;
		onLoginDone();
		return;
	}

	iLogin->show();

	if (Visible && memName->CanFocus())
		memName->SetFocus();
}

//---------------------------------------------------------------------------
void __fastcall TiProfiledGame::clearPoints()
{
	iCalibPoints->clear(iProfile->Level);
}

//---------------------------------------------------------------------------
void __fastcall TiProfiledGame::addPoint(CalibrationPointStruct& aPoint)
{
	iCalibPoints->add(aPoint);
}

//---------------------------------------------------------------------------
void __fastcall TiProfiledGame::nextPoint(int aPointID)
{
	iNextPointID = aPointID;
	if (!iIsRecalibrating || iLastPointID == aPointID)
		TiTimeout::run(1000, AfterPointDone, &iTimeout);        // GT-driven
	else
		AfterPointDone();
}

//---------------------------------------------------------------------------
void __fastcall TiProfiledGame::reportCalibrationResult(int aNumber,
		CalibrationPointQualityStruct& aLeft,
		CalibrationPointQualityStruct& aRight)
{
	iCalibQuality->add(aNumber, aLeft, aRight);
}

//---------------------------------------------------------------------------
bool __fastcall TiProfiledGame::processCalibrationResult()
{
	bool finished = true;

	if (FOnEvent)
	{
		TStringList* calibQuality = new TStringList();
		iCalibQuality->log(calibQuality);
		for (int i = 0; i < calibQuality->Count; i++)
		{
			FOnEvent(this, String("quality\t") + calibQuality->Strings[i]);
		}

		delete calibQuality;
	}

	TiCalibQuality::Point worstCalibPoint = iCalibQuality->WorstPoint;
	if (FOnEvent)
		FOnEvent(this, String().sprintf("worst quality\t%d\t%.1f\t%.3f",
				worstCalibPoint.ID - 1, worstCalibPoint.Offset, worstCalibPoint.Quality));

	if (worstCalibPoint.Offset > KMaxAllowedCalibQualityOffset ||
			worstCalibPoint.Quality < KMinAllowedCalibQualityValue)
	{
		finished = false;
		iIsRecalibrating = true;
		RecalibratePoint(worstCalibPoint.ID);
	}
	else
	{
		TiTimeout::run(10, StartVerification, &iTimeout);
	}

	return finished;
}

//---------------------------------------------------------------------------
void __fastcall TiProfiledGame::loadSettings(TiXML_INI* aStorage)
{
	iProfile->load(aStorage);
}

//---------------------------------------------------------------------------
void __fastcall TiProfiledGame::saveSettings(TiXML_INI* aStorage)
{
	iProfile->save(aStorage);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TiProfiledGame::onObjectPaint(TObject* aSender, EiUpdateType aUpdateType)
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

		Gdiplus::SolidBrush backgroundBrush(KBackgroundColor);
		Gdiplus::Rect fillRect(0, 0, Width, Height);
		graphics->FillRectangle(&backgroundBrush, fillRect);

		//iBackground->paintTo(graphics);

		iCalibPoints->paintTo(graphics, updStatic);
		iLogin->paintTo(graphics, updStatic);
		if (iProfileEditor)
			iProfileEditor->paintTo(graphics, updStatic);
		iInstructionSitting->paintTo(graphics, updStatic);
		iEyeBox->paintTo(graphics, updStatic);
		iInstructionStart->paintTo(graphics, updStatic);
		if (iLevelLegend)
			iLevelLegend->paintTo(graphics, updStatic);
		if (iLevelInstruction)
			iLevelInstruction->paintTo(graphics, updStatic);
		iInstructionShape->paintTo(graphics, updStatic);
		iInstructionCalibrate->paintTo(graphics, updStatic);
		iRewards->paintTo(graphics, updStatic);
		iStatus->paintTo(graphics, updStatic);

		if (aUpdateType == updStatic)
			iGraphics->DrawImage(iStaticBitmap, destRect);
	}

	if (aUpdateType & updNonStatic)
	{
		Gdiplus::Bitmap buffer(Width, Height, iGraphics);
		Gdiplus::Graphics g(&buffer);
		Gdiplus::Graphics* graphics = &g;

		graphics->DrawImage(iStaticBitmap, destRect);

		if (iProfileEditor)
			iProfileEditor->paintTo(graphics, updNonStatic);
		iEyeBox->paintTo(graphics, updNonStatic);
		if (iLevelLegend)
			iLevelLegend->paintTo(graphics, updNonStatic);
		iDoor->paintTo(graphics, updNonStatic);
		iCalibPoints->paintTo(graphics, updNonStatic);
		if (iLevelInstruction)
			iLevelInstruction->paintTo(graphics, updNonStatic);
		iInstructionCalibrate->paintTo(graphics, updNonStatic);
		iGameResult->paintTo(graphics, updNonStatic);
		iRewards->paintTo(graphics, updNonStatic);
		iStatus->paintTo(graphics, updNonStatic);
		iPositionTracker->paintTo(graphics, updNonStatic);

		iGraphics->DrawImage(&buffer, 0, 0);
	}
}

//---------------------------------------------------------------------------
void __fastcall TiProfiledGame::onCalibPointReady(TObject* aSender)
{
	if (!iCalibPoints->Current)
		return;

	if (FOnEvent && !iIsVerifying)
		FOnEvent(this, String().sprintf("ready to %s\t%d\t%d %d",
				iIsVerifying ? "verify" : "accept",
				iCalibPoints->CurrentPointIndex,
				iCalibPoints->Current->X, iCalibPoints->Current->Y));

	if (!iIsVerifying && FOnPointReady)
		FOnPointReady(this, iCalibPoints->CurrentPointIndex, iCalibPoints->IsSinglePointMode);

	iCalibPoints->waitAcceptance();
}

//---------------------------------------------------------------------------
void __fastcall TiProfiledGame::onLoginDone(TObject* aSender)
{
	iLogin->hide();

	WideString name = memName->Text;

	// convert RU chars to FI chars.
	wchar_t* nameRef = name.c_bstr();
	for (int i = 0; i < name.Length(); i++)
		if (nameRef[i] == 0x0426)
			nameRef[i] = 0xD6;
		else if (nameRef[i] == 0x0414)
			nameRef[i] = 0xE6;

	iProfile->Name = name;

	if (iProfile->Status == TiProfile::psNew)
	{
		if (!iProfileEditor)
		{
			iProfileEditor = new TiProfileEditor(iObjects, TiSize( Width, Height ), TiSize(KViewportWidth, KViewportHeight));
			iProfileEditor->OnDone = onProfileEditorDone;
		}

		iProfileEditor->show(iProfile);
	}
	else
	{
		iStatus->show(iProfile);
	}

	switch (iProfile->Level)
	{
		case 0: iTargetID = random(TiInstructionColor::cidCount); break;
		case 1: iTargetID = random(TiInstructionShape::sidCount); break;
	}
}

//---------------------------------------------------------------------------
void __fastcall TiProfiledGame::onProfileEditorDone(TObject* aSender)
{
	iProfileEditor->hide();
	iInstructionSitting->show();
}

//---------------------------------------------------------------------------
void __fastcall TiProfiledGame::onInstructionSittingDone(TObject* aSender)
{
	iInstructionSitting->hide();
	iEyeBox->show(iProfile);
}

//---------------------------------------------------------------------------
void __fastcall TiProfiledGame::onEyeBoxDone(TObject* aSender)
{
	TiTimeout::run(1500, AfterEyeBox, &iTimeout);
}

//---------------------------------------------------------------------------
void __fastcall TiProfiledGame::AfterEyeBox(TObject* aSender)
{
	iEyeBox->hide();
	iInstructionStart->show();
}

//---------------------------------------------------------------------------
void __fastcall TiProfiledGame::onInstructionStartDone(TObject* aSender)
{
	iInstructionStart->hide();

	switch (iProfile->Level)
	{
		case 0: iLevelLegend = iFactory;	break;
		case 1: iLevelLegend = iGhostBox;	break;
		default: return; // no more levels
	}
	iLevelLegend->show(true);
}

//---------------------------------------------------------------------------
void __fastcall TiProfiledGame::onLevelLegendDone(TObject* aSender)
{
	if (!iLevelLegend)
		return;

	if (iLevelLegend->IsDirty)
	{
		iDoor->show(TiDoor::dmDown);
		TiTimeout::run(200, AfterLevelLegend, &iTimeout);
	}
	else
	{
		iGameResult->show(iProfile, iCalibrationLevel);
	}
}

//---------------------------------------------------------------------------
void __fastcall TiProfiledGame::AfterLevelLegend(TObject* aSender)
{
	iDoor->run();
}

//---------------------------------------------------------------------------
void __fastcall TiProfiledGame::onDoorDone(TObject* aSender)
{
	if (iDoor->IsClosed)
	{
		if (iLevelLegend)
			iLevelLegend->hide();

		TiTimeout::run(500, AfterDoor, &iTimeout);
	}
}

//---------------------------------------------------------------------------
void __fastcall TiProfiledGame::AfterDoor(TObject *Sender)
{
	if (iLevelInstruction)
		return;

	switch (iProfile->Level)
	{
		case 0:
			iLevelInstruction = iInstructionColor;
			iInstructionColor->show(iTargetID);
			break;
		case 1:
			iLevelInstruction = iInstructionShape;
			iInstructionShape->show(iTargetID);
			break;
	}
}

//---------------------------------------------------------------------------
void __fastcall TiProfiledGame::onLevelInstructionDone(TObject* aSender)
{
	iLevelInstruction->hide();
	iLevelInstruction = NULL;

	iInstructionCalibrate->show();
}

//---------------------------------------------------------------------------
void __fastcall TiProfiledGame::onInstructionCalibrateDone(TObject* aSender)
{
	iDoor->hide();
	iInstructionCalibrate->hide();

	iCalibrationLevel = iProfile->Level;

	StartCalibration();
	iLevelLegend = NULL;

	if (FOnEvent)
		FOnEvent(this, "scene appeared");

	if (FOnReadyToCalibrate)
		FOnReadyToCalibrate(this);

	//NextPoint(1); // GT-driven
}

//---------------------------------------------------------------------------
void __fastcall TiProfiledGame::onCalibPointsDone(TObject* aSender)
{
	iPositionTracker->Enabled = false;

	iCalibPoints->hide();
	iDoor->show(TiDoor::dmUp);

	TiTimeout::run(1000, AfterCalibPoints, &iTimeout);
}

//---------------------------------------------------------------------------
void __fastcall TiProfiledGame::AfterCalibPoints(TObject *Sender)
{
	iProfile->updateScore();

	if (iProfile->IsSucceeded)
	{
		switch (iCalibrationLevel)
		{
			case 0: iLevelLegend = iFactory;	break;
			case 1: iLevelLegend = iGhostBox;	break;
		}
		iLevelLegend->show(false);
		iDoor->run();
	}
	else
	{
		iGameResult->show(iProfile, iCalibrationLevel);
	}
}

//---------------------------------------------------------------------------
void __fastcall TiProfiledGame::onGameResultDone(TObject* aSender)
{
	if (iLevelLegend)
	{
		iLevelLegend->hide();
		iLevelLegend = NULL;
	}
	
	iDoor->hide();
	iGameResult->hide();

	iRewards->show(iProfile);
	TiTimeout::run(1500, DelayedRewards, &iTimeout);
}

//---------------------------------------------------------------------------
void __fastcall TiProfiledGame::DelayedRewards(TObject* aSender)
{
	if (iProfile->GameBonus)
		iRewards->showBonus();
}

//---------------------------------------------------------------------------
void __fastcall TiProfiledGame::onRewardsDone(TObject* aSender)
{
	iRewards->hide();
	iStatus->show(iProfile);
}

//---------------------------------------------------------------------------
void __fastcall TiProfiledGame::onStatusDone(TObject* aSender)
{
	iStatus->hide();

	if (!iProfile->TargetPoints->Count)
		iInstructionSitting->show();
	else
		TiTimeout::run(100, Done, &iTimeout);
}

//---------------------------------------------------------------------------
void __fastcall TiProfiledGame::onPositionTrackerDone(TObject* aSender)
{
	iPositionTracker->hide();
	iCalibPoints->resume();
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TiProfiledGame::StartCalibration(TObject* aSender)
{
	iIsVerifying = false;
	iIsRecalibrating = false;
	iLastPointID = -1;

	if (FOnEvent)
		FOnEvent(this, "calibration start");
	if (FOnStart)
		FOnStart(this);

	iCalibPoints->show();
	iPositionTracker->Enabled = true;

	iCalibPoints->prepare();
}

//---------------------------------------------------------------------------
void __fastcall TiProfiledGame::StartVerification(TObject* aSender)
{
	if (FOnEvent)
		FOnEvent(this, "verification start");

	iIsVerifying = true;

	if (FOnVerifStarted)
		FOnVerifStarted(this);

	AddVerificationPoints();

	NextPoint(1);
}

//---------------------------------------------------------------------------
void __fastcall TiProfiledGame::NextPoint(int aPointNumber)
{
	if (iCalibPoints->Current)
		iCalibPoints->Current->hide();

	TiCalibPoint* calibPoint = iCalibPoints->next(aPointNumber);
	if (calibPoint)
		iLastPointID = iCalibPoints->CurrentPointIndex + 1;

	if (calibPoint)
	{
		if (FOnEvent && !iIsVerifying)
			FOnEvent(this, "move to next point");

		calibPoint->show();
		TiTimeout::run(800, onCalibPointReady, &iTimeout);
	}
	else
	{
		if (FOnEvent)
		{
			if (iIsVerifying)
				FOnEvent(this, "verification finished");
			else
				FOnEvent(this, "calibration finished");
		}

		if (!iIsVerifying)
		{
			iCalibQuality->reset();

			if (FOnFinished)
				FOnFinished(this);
		}
		else
		{
			EstimateVerificationQuality();

			if (FOnVerifFinished)
				FOnVerifFinished(this);

			onCalibPointsDone();
		}
	}
}

//---------------------------------------------------------------------------
void __fastcall TiProfiledGame::RecalibratePoint(int aPointNumber)
{
	if (FOnEvent)
		FOnEvent(this, String().sprintf("restart\t%d", aPointNumber - 1));

	iCalibPoints->prepare(aPointNumber);

	if (FOnRecalibratePoint)
		FOnRecalibratePoint(this, aPointNumber, true);

	//NextPoint(aPointNumber); // GT-driven
}

//---------------------------------------------------------------------------
void __fastcall TiProfiledGame::PointDone(TObject* aSender)
{
	if (!iCalibPoints->IsWaitingToAcceptPoint)
		return;

	iIsRecalibrating = false;
	ReportPointAcceptance();

	int duration = iCalibPoints->accept();

	TiCalibPoints::EiItemResultType itemResultType = iCalibPoints->showItemResult(iTargetID);

	TiProfile::SiTargetPoint* point = new TiProfile::SiTargetPoint(
				iCalibPoints->AnimationDuration, iTargetID, duration,
				itemResultType == TiCalibPoints::irtSuccess);
	iProfile->addPoint(point);

	if (iIsVerifying)
		iCalibQualityEstimator->addSelection(iCalibPoints->Current->X, iCalibPoints->Current->Y);

	if (!iIsVerifying && FOnPointAccepted)
		FOnPointAccepted(this, iCalibPoints->CurrentPointIndex, iCalibPoints->IsSinglePointMode);

	if (iIsVerifying)
	{
		iNextPointID = -1;
		TiTimeout::run(1000, AfterPointDone, &iTimeout);
	}
}

//---------------------------------------------------------------------------
void __fastcall TiProfiledGame::AfterPointDone(TObject* aSender)
{
	NextPoint(iNextPointID);
}

//---------------------------------------------------------------------------
void __fastcall TiProfiledGame::Abort(String aMessage)
{
	if (FOnEvent)
		FOnEvent(this, String("aborted ") + aMessage);

	if (FOnAborted)
		FOnAborted(this);

	if (iTimeout)
		iTimeout->kill();

	iCalibPoints->reset();

	TiTimeout::run(1000, Done);
}

//---------------------------------------------------------------------------
void __fastcall TiProfiledGame::Done(TObject* aSender)
{
	if (FOnEvent)
		FOnEvent(this, "done");

	if (FormState.Contains(fsModal))
		ModalResult = mrOk;
	else
		Close();
}

//- Other -------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TiProfiledGame::ReportPointAcceptance()
{
	if (!FOnEvent)
		return;

	if (iIsVerifying)
	{
		TiCalibQualityEstimator::TiPointD pt = iCalibQualityEstimator->Point;
		double dx = pt.X - iCalibPoints->Current->X;
		double dy = pt.Y - iCalibPoints->Current->Y;
		double distance = sqrt(dx*dx + dy*dy);
		FOnEvent(this, String().sprintf("verification point\t%d %d\t%d %d\t%.2f",
			iCalibPoints->Current->X, iCalibPoints->Current->Y, int(pt.X), int(pt.Y), distance ));
	}
	else
	{
		FOnEvent(this, String().sprintf("point accepted\t%d\t%d %d", iCalibPoints->CurrentPointIndex,
				iCalibPoints->Current->X, iCalibPoints->Current->Y));
	}
}

//---------------------------------------------------------------------------
void __fastcall TiProfiledGame::AddVerificationPoints()
{
	clearPoints();

	int cellWidth = Width / KVerificationPointCountX;
	int cellHeight = Height / KVerificationPointCountY;

	for (int i = 0; i < KVerificationPointCountX; i++)
		for (int j = 0; j < KVerificationPointCountY; j++)
		{
			CalibrationPointStruct cp;
			cp.number = i * KVerificationPointCountY + j + 1;
			cp.positionX = i * cellWidth + KVerificationCellMargin + random(cellWidth - 2 * KVerificationCellMargin);
			cp.positionY = j * cellHeight + KVerificationCellMargin + random(cellHeight - 2 * KVerificationCellMargin);

			addPoint(cp);
		}

	iCalibPoints->prepare();
}

//---------------------------------------------------------------------------
void __fastcall TiProfiledGame::EstimateVerificationQuality()
{
	if (!FOnEvent)
		return;

	SiQuality quality;
	int points = iCalibQualityEstimator->estimate(quality);

	if (points)
		FOnEvent(this, quality.toString("verification result"));
	else
		FOnEvent(this, "verification result\t0");
}

//- Setters -----------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TiProfiledGame::SetOnEvent(FiOnEvent aFOnEvent)
{
	FOnEvent = aFOnEvent;
	iCalibQualityEstimator->OnEvent = FOnEvent;
}

//- GUI event handlers ------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TiProfiledGame::FormCreate(TObject *Sender)
{
	SetBounds(0, 0, Screen->Width, Screen->Height);

	TiSize screenSize(Width, Height);
	TiSize viewport(KViewportWidth, KViewportHeight);

	iGraphics = new Gdiplus::Graphics(Handle, false);

	iObjects = new TiAnimationManager();
	iObjects->OnPaint = onObjectPaint;

	iProfile = new TiProfile(iObjects, screenSize, viewport);

	//iBackground = new TiAnimation(false);
	//iBackground->addFrames(IDR_BACKGROUND, KViewportWidth, KViewportHeight);
	//iBackground->placeTo(Width / 2, Height / 2);
	//iObjects->add(iBackground);

	iLogin = new TiLogin(iObjects, screenSize, viewport, pnlNameContainer);
	iLogin->OnDone = onLoginDone;

	iInstructionSitting = new TiInstructionSitting(iObjects, screenSize, viewport);
	iInstructionSitting->OnDone = onInstructionSittingDone;

	iInstructionStart = new TiInstructionStart(iObjects, screenSize, viewport);
	iInstructionStart->OnDone = onInstructionStartDone;

	iEyeBox = new TiEyeBox(iObjects, screenSize, viewport);
	iEyeBox->OnDone = onEyeBoxDone;

	iFactory = new TiFactory(iObjects, screenSize, viewport);
	iFactory->OnDone = onLevelLegendDone;

	iGhostBox = new TiGhostBox(iObjects, screenSize, viewport);
	iGhostBox->OnDone = onLevelLegendDone;

	iDoor = new TiDoor(iObjects, screenSize, viewport);
	iDoor->OnDone = onDoorDone;

	iInstructionColor = new TiInstructionColor(iObjects, screenSize, viewport);
	iInstructionColor->OnDone = onLevelInstructionDone;

	iInstructionShape = new TiInstructionShape(iObjects, screenSize, viewport);
	iInstructionShape->OnDone = onLevelInstructionDone;

	iInstructionCalibrate = new TiInstructionCalibrate(iObjects, screenSize, viewport);
	iInstructionCalibrate->OnDone = onInstructionCalibrateDone;

	iCalibPoints = new TiCalibPoints(iObjects, screenSize, viewport);
//	iCalibPoints->OnDone = onCalibPointsDone;

	iGameResult = new TiGameResult(iObjects, screenSize, viewport);
	iGameResult->OnDone = onGameResultDone;

	iRewards = new TiRewards(iObjects, screenSize, viewport);
	iRewards->OnDone = onRewardsDone;

	iStatus = new TiStatus(iObjects, screenSize, viewport);
	iStatus->OnDone = onStatusDone;

	iPositionTracker = new TiPositionTracker(iObjects, screenSize, viewport);
	iPositionTracker->OnDone = onPositionTrackerDone;
}

//---------------------------------------------------------------------------
void __fastcall TiProfiledGame::FormDestroy(TObject *Sender)
{
	if (iProfileEditor)
	{
		delete iProfileEditor;
		iProfileEditor = NULL;
	}

	delete iLogin;
	delete iInstructionSitting;
	delete iEyeBox;
	delete iInstructionStart;
	delete iFactory;
	delete iGhostBox;
	delete iDoor;
	delete iInstructionColor;
	delete iInstructionShape;
	delete iInstructionCalibrate;
	delete iCalibPoints;
	delete iGameResult;
	delete iRewards;
	delete iStatus;
	delete iPositionTracker;

	delete iProfile;

	delete iObjects;
	delete iGraphics;

	if (iTimeout)
		delete iTimeout;
}

//---------------------------------------------------------------------------
void __fastcall TiProfiledGame::FormMouseUp(TObject *Sender,
			TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if (iLogin->IsVisible)
		iLogin->mouseClick(Button, Shift, X, Y);
	else if (iProfileEditor && iProfileEditor->IsVisible)
		iProfileEditor->mouseClick(Button, Shift, X, Y);
	else if (iInstructionSitting->IsVisible)
		iInstructionSitting->mouseClick(Button, Shift, X, Y);
	else if (iInstructionStart->IsVisible)
		iInstructionStart->mouseClick(Button, Shift, X, Y);
	else if (iLevelInstruction && iLevelInstruction->IsVisible)
		iLevelInstruction->mouseClick(Button, Shift, X, Y);
	else if (iCalibPoints->IsWaitingToAcceptPoint && !iPositionTracker->IsVisible)
		PointDone();
	else if (iGameResult->IsVisible)
		iGameResult->mouseClick(Button, Shift, X, Y);
	else if (iRewards->IsVisible)
		iRewards->mouseClick(Button, Shift, X, Y);
	else if (iStatus->IsVisible)
		iStatus->mouseClick(Button, Shift, X, Y);
	else if (iPositionTracker->IsVisible)
		iPositionTracker->mouseClick(Button, Shift, X, Y);
}

//---------------------------------------------------------------------------
void __fastcall TiProfiledGame::FormKeyUp(TObject *Sender, WORD &Key,
			TShiftState Shift)
{
	if (iLogin->IsVisible)
	{
	}
	else if (iProfileEditor && iProfileEditor->IsVisible)
	{
		if (Key == VK_ESCAPE)
			Abort("profiling");
		else if (Key == VK_SPACE)
			onProfileEditorDone();
	}
	else if (iInstructionSitting->IsVisible)
	{
		if (Key == VK_ESCAPE)
			Abort("instruction sitting");
		else if (Key == VK_SPACE)
			onInstructionSittingDone();
	}
	else if (iEyeBox->IsVisible)
	{
		if (Key == VK_ESCAPE)
			Abort("eyebox");
		else if (Key == VK_SPACE)
			AfterEyeBox();
	}
	else if (iInstructionStart->IsVisible)
	{
		if (Key == VK_ESCAPE)
			Abort("instruction start");
		else if (Key == VK_SPACE)
			onInstructionStartDone();
	}
	else if (iLevelLegend && iLevelLegend->IsVisible && !iGameResult->IsVisible)
	{
		if (Key == VK_ESCAPE)
			Abort("instruction start");
		else if (Key == VK_SPACE)
			onLevelLegendDone();
	}
	else if (iLevelInstruction && iLevelInstruction->IsVisible)
	{
		if (Key == VK_ESCAPE)
			Abort("instruction start");
		else if (Key == VK_SPACE)
			onLevelInstructionDone();
	}
	else if (iInstructionCalibrate->IsVisible)
	{
		if (Key == VK_ESCAPE)
			Abort("instruction start");
		else if (Key == VK_SPACE)
			onInstructionCalibrateDone();
	}
	else if (iCalibPoints->Current)
	{
		if (Key == VK_ESCAPE)
			Abort();
	}
	else if (iGameResult->IsVisible)
	{
		if (Key == VK_ESCAPE)
			Abort("display result");
		else if (Key == VK_SPACE)
			onGameResultDone();
	}
	else if (iRewards->IsVisible)
	{
		if (Key == VK_ESCAPE)
			Abort("display rewards");
		else if (Key == VK_SPACE)
			onRewardsDone();
	}
	else if (iStatus->IsVisible)
	{
		if (Key == VK_ESCAPE)
			Abort("display status");
		else if (Key == VK_SPACE)
			onStatusDone();
	}
	else if (iPositionTracker->IsVisible)
	{
		if (Key == VK_ESCAPE)
			Abort("display status");
		else if (Key == VK_SPACE)
			onPositionTrackerDone();
	}
	else
	{
		if (Key == VK_ESCAPE)
			Done();
	}
}

//---------------------------------------------------------------------------
void __fastcall TiProfiledGame::FormKeyDown(TObject *Sender,
			WORD &Key, TShiftState Shift)
{
	if (Key == VK_SPACE)
	{
		if (iCalibPoints->IsWaitingToAcceptPoint && !iPositionTracker->IsVisible)
			PointDone();
	}
}

//---------------------------------------------------------------------------
void __fastcall TiProfiledGame::memNameKeyDown(TObject *Sender,
			WORD &Key, TShiftState Shift)
{
	if (Key == VK_RETURN && memName->Text.Length() > 0)
		onLoginDone();
	else if (Key == VK_ESCAPE)
		Abort("instruction login");
}

//---------------------------------------------------------------------------
void __fastcall TiProfiledGame::memNameKeyPress(TObject *Sender,
			char &Key)
{
	WORD key = Key & 0xFF;
	if ((key >= 'a' && key <= 'z') || key == 0xF6 || key == 0xE4)
	{
		Key = Key & 0xDF;
	}
}

//---------------------------------------------------------------------------

