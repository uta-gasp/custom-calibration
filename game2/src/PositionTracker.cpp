//---------------------------------------------------------------------------
#include "PositionTracker.h"
#include "assets_pg.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
using namespace ProfiledGame;

//---------------------------------------------------------------------------
const int KThresholdZ = 80;
const int KThresholdXY = 60;

//---------------------------------------------------------------------------
__fastcall TiPositionTracker::TiPositionTracker(TiAnimationManager* aManager,
		TiSize aScreenSize, TiSize aViewport) :
		TiScene(aManager, aScreenSize, aViewport),
		iEnabled(false)
{
	TiAnimation* background = new TiAnimation(false, false);
	background->addFrames(IDR_INSTRUCTIONS_POSITION_ALERT, aViewport.Width, aViewport.Height);
	background->placeTo(aScreenSize.Width / 2, aScreenSize.Height / 2);
	aManager->add(background);
	iDynamicAssets->add(background);

	iButtonContinue = TiRect(Offset.x + 583, Offset.y + 475, 200, 60);

	iUserPositionQualityEstimator = new TiUserPositionQualityEstimator(KThresholdZ, KThresholdXY);
}

//---------------------------------------------------------------------------
__fastcall TiPositionTracker::~TiPositionTracker()
{
	delete iUserPositionQualityEstimator;
}

//---------------------------------------------------------------------------
void __fastcall TiPositionTracker::mouseClick(TMouseButton aButton, TShiftState aShift, int aX, int aY)
{
	if (iButtonContinue.hits(aX, aY))
	{
		if (FOnDone)
			FOnDone(this);
	}
}

//---------------------------------------------------------------------------
bool __fastcall TiPositionTracker::isPositionWrong(SampleStruct& aSample)
{
	if (!iEnabled || IsVisible)
		return false;

	//bool isWrong = aSample.leftEye.eyePositionX > 120;

	TiUserPositionQualityEstimator::PositionQuality quality = iUserPositionQualityEstimator->feed(aSample.leftEye);
	bool isWrong = quality > TiUserPositionQualityEstimator::pqOK;

	if (isWrong && !IsVisible)
		show();

	return isWrong;
}

