//---------------------------------------------------------------------------
#include "CalibPoints.h"
#include "utils.h"
#include "assets_pg.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
using namespace ProfiledGame;

//---------------------------------------------------------------------------
const int KExpectedPointCount = 11;

const int KMaxPointCalibrationCount = 3;

//---------------------------------------------------------------------------
__fastcall TiCalibPoints::TiCalibPoints(TiAnimationManager* aManager,
		TiSize aScreenSize, TiSize aViewport) :
		TiScene(aManager, aScreenSize, aViewport),
		iCurrentPointIndex(-1),
		iIsWaitingToAcceptPoint(false),
		iLevel(0),
		iPointsCompleted(0),
		iItemResult(NULL)
{
	iPoints.DeleteContent = true;

	TiAnimation* background = new TiAnimation(false);
	background->addFrames(IDR_DOORS, aViewport.Width, aViewport.Height);
	background->placeTo(iScreenSize.Width / 2, iScreenSize.Height / 2);
	iManager->add(background);
	iStaticAssets->add(background);   

	iTimestamp = new TiTimestamp();
}

//---------------------------------------------------------------------------
__fastcall TiCalibPoints::~TiCalibPoints()
{
	delete iTimestamp;
}

//---------------------------------------------------------------------------
void __fastcall TiCalibPoints::show(TiAnimation* aBackground)
{
	if (aBackground)
		iStaticAssets->add(aBackground);

	TiScene::show();
	iItemResult->hide();
}

//---------------------------------------------------------------------------
void __fastcall TiCalibPoints::hide()
{
	TiScene::hide();

	iStaticAssets->clear();
}

//---------------------------------------------------------------------------
void __fastcall TiCalibPoints::reset()
{
	iIsWaitingToAcceptPoint = false;
	iAnimationDuration = adSlow;

	for (int i = 0; i < iPoints.Count; i++)
		iPoints[i]->hide();
}

//---------------------------------------------------------------------------
void __fastcall TiCalibPoints::clear(int aLevel)
{
	iPoints.clear();

	if (aLevel >= 0)
		iLevel = aLevel;

	if (iItemResult)
	{
		iDynamicAssets->remove(iItemResult);
	}

	iItemResult = new TiAnimation(false, false);
	switch (iLevel)
	{
		case 0: iItemResult->addFrames(IDR_CALIB_LEVEL1_ITEM_RESULT, 59, 78); break;
		case 1: iItemResult->addFrames(IDR_CALIB_LEVEL2_ITEM_RESULT, 84, 77); break;
	}

	iManager->add(iItemResult);
	iDynamicAssets->add(iItemResult);
}

//---------------------------------------------------------------------------
void __fastcall TiCalibPoints::add(CalibrationPointStruct& aPoint)
{
	// we are not sure points are added in proper order, i.e. 1,2,3...
	int index = aPoint.number - 1;
	while (iPoints.Count < index) {
		iPoints.add(new TiCalibPoint(NULL, 0, 0, iLevel));
	}

	if (index < iPoints.Count)
		iPoints.set(index, new TiCalibPoint(iManager, aPoint.positionX, aPoint.positionY, iLevel));
	else
		iPoints.add(new TiCalibPoint(iManager, aPoint.positionX, aPoint.positionY, iLevel));
}

//---------------------------------------------------------------------------
void __fastcall TiCalibPoints::prepare(int aCalibPointNumber)
{
	iCurrentPointIndex = -1;
	iIsSinglePointMode = aCalibPointNumber > 0;

	if (iIsSinglePointMode)
	{
		TiCalibPoint* point = iPoints[aCalibPointNumber - 1];
		if (point)
			point->stop();

		for (int i = 0; i < iPoints.Count; i++)
			iPoints[i]->Enabled = iPoints[i] == point;
	}
	else
	{
		reset();
		//Rearrange();

		for (int i = 0; i < iPoints.Count; i++)
		{
			iPoints[i]->Enabled = true;
			iPoints[i]->CalibTarget->AnimationSpeed = 1000.0 / adSlow;
		}
	}
}

//---------------------------------------------------------------------------
TiCalibPoint* __fastcall TiCalibPoints::next(int aPointNumber)
{
	iItemResult->hide();

	if (aPointNumber < 1)
	{
		while (true) {
			iCurrentPointIndex++;
			if (iCurrentPointIndex == iPoints.Count)
				break;
			if (iPoints[iCurrentPointIndex]->Enabled)
				break;
		}
	}
	else {
		iCurrentPointIndex = aPointNumber - 1;
	}

	bool canContinue = iCurrentPointIndex >= 0 && iCurrentPointIndex < iPoints.Count;
	if (!canContinue)
		iCurrentPointIndex = -1;
	else
		SetAnimationSpeed();

	return canContinue ? iPoints[iCurrentPointIndex] : NULL;
}

//---------------------------------------------------------------------------
void __fastcall TiCalibPoints::waitAcceptance()
{
	iTimestamp->reset();
	iIsWaitingToAcceptPoint = true;

	if (!iPauseState.Enabled)
		Current->start();
}

//---------------------------------------------------------------------------
int __fastcall TiCalibPoints::accept()
{
	iPointsCompleted++;
	iIsWaitingToAcceptPoint = false;
	int duration = iTimestamp->ms();

	Current->stop(true);

	return duration;
}

//---------------------------------------------------------------------------
void __fastcall TiCalibPoints::pause()
{
	if (Current)
		Current->stop();

	iPauseState.Enabled = true;
	iPauseState.PointIndex = iCurrentPointIndex;
	iPauseState.Frame = Current ? Current->CalibTarget->FrameIndex : -1;
}

//---------------------------------------------------------------------------
void __fastcall TiCalibPoints::resume()
{
	iTimestamp->reset();

	iPauseState.Enabled = false;

	if (Current && IsVisible)
	{
		if (iCurrentPointIndex == iPauseState.PointIndex)
			Current->CalibTarget->setFrame(iPauseState.Frame);

		Current->start();
	}
}

//---------------------------------------------------------------------------
TiCalibPoints::EiItemResultType __fastcall TiCalibPoints::showItemResult(int aTargetID)
{
	bool success = Current->CalibTarget->FrameIndex == aTargetID;
	iItemResult->setFrame(success ? 1 : 0);
	iItemResult->placeTo(Current->CalibTarget->X, Current->CalibTarget->Y);
	iItemResult->show();

	return success ? irtSuccess : irtFailure;
}

//---------------------------------------------------------------------------
void __fastcall TiCalibPoints::paintTo(Gdiplus::Graphics* aGraphics, EiUpdateType aUpdateType)
{
	for (int i = 0; i < iPoints.Count; i++)
		iPoints[i]->paintTo(aGraphics, aUpdateType);

	TiScene::paintTo(aGraphics, aUpdateType);
}

//---------------------------------------------------------------------------
bool __fastcall TiCalibPoints::canRecalibratePoint(int aIndex)
{
	return aIndex < 0 || aIndex >= iPoints.Count ? false : iPoints[aIndex]->AcceptanceCount < KMaxPointCalibrationCount;
}

//---------------------------------------------------------------------------
int __fastcall TiCalibPoints::pointRecalibrationCount(int aIndex)
{
	return aIndex < 0 || aIndex >= iPoints.Count ? 0 : iPoints[aIndex]->AcceptanceCount - 1;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int __fastcall TiCalibPoints::GetCount()
{
	return iPoints.Count;
}

//---------------------------------------------------------------------------
TiCalibPoint* __fastcall TiCalibPoints::GetCurrent()
{
	return iCurrentPointIndex < 0 || iCurrentPointIndex >= iPoints.Count ? NULL : iPoints[iCurrentPointIndex];
}

//---------------------------------------------------------------------------
void __fastcall TiCalibPoints::Rearrange()
{
	for (int i = 1; i < iPoints.Count; i++)
	{
		int idx = 1 + random(iPoints.Count - 1);
		if (idx == i)
			continue;

		iPoints.exchange(idx, i);
	}
}

//---------------------------------------------------------------------------
void __fastcall TiCalibPoints::SetAnimationSpeed()
{
	if (iPointsCompleted <= (KExpectedPointCount / 3))
		iAnimationDuration = adSlow;
	else if (iPointsCompleted <= (2 * KExpectedPointCount / 3))
		iAnimationDuration = adModerate;
	else
		iAnimationDuration = adFast;

	for (int i = 0; i < iPoints.Count; i++)
		iPoints[i]->CalibTarget->AnimationSpeed = 1000.0 / iAnimationDuration;
}

