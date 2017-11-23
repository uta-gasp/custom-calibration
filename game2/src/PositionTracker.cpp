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

const int KEstimatesBufferSize = 6;
const double KWrongPositionShareThreshold = 0.7;

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

	iEstimatesBuffer = new TiUserPositionQualityEstimator::PositionQuality[KEstimatesBufferSize];
	iEstimatesBufferHead = 0;

	ResetBuffer();

	iUserPositionQualityEstimator = new TiUserPositionQualityEstimator(KThresholdZ, KThresholdXY);
}

//---------------------------------------------------------------------------
__fastcall TiPositionTracker::~TiPositionTracker()
{
	delete iUserPositionQualityEstimator;
	delete [] iEstimatesBuffer;
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

	TiUserPositionQualityEstimator::PositionQuality quality = iUserPositionQualityEstimator->feed(aSample.leftEye);

	bool result = false;
	if (quality != TiUserPositionQualityEstimator::pqUndefined)
	{
		iEstimatesBuffer[iEstimatesBufferHead] = quality;
		if (++iEstimatesBufferHead == KEstimatesBufferSize)
			iEstimatesBufferHead = 0;

		result = GetWrongPositionShare() > KWrongPositionShareThreshold;
	}

	if (result && !IsVisible)
	{
		ResetBuffer();
		show();
	}

	return result;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TiPositionTracker::ResetBuffer()
{
	for (int i = 0; i < KEstimatesBufferSize; i++)
		iEstimatesBuffer[i] = TiUserPositionQualityEstimator::pqOK;
}

//---------------------------------------------------------------------------
double __fastcall TiPositionTracker::GetWrongPositionShare()
{
	int wrongPosCount = 0;
	for (int i = 0; i < KEstimatesBufferSize; i++)
		wrongPosCount += iEstimatesBuffer[i] > TiUserPositionQualityEstimator::pqOK ? 1 : 0;

	return double(wrongPosCount) / KEstimatesBufferSize;
}
