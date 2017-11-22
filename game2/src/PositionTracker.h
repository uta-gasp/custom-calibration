//---------------------------------------------------------------------------
#ifndef PositionTrackerH
#define PositionTrackerH

//---------------------------------------------------------------------------
#include "Scene.h"
#include "UserPositionQualityEstimator.h"

#ifndef RET_SUCCESS
#include "myGazeAPI.h"
#endif

//---------------------------------------------------------------------------
namespace ProfiledGame {

//---------------------------------------------------------------------------
class TiPositionTracker : public TiScene
{
	private:
		TiRect iButtonContinue;
		bool iEnabled;

		TiUserPositionQualityEstimator::PositionQuality* iEstimatesBuffer;
		int iEstimatesBufferHead;

		TiUserPositionQualityEstimator* iUserPositionQualityEstimator;

		void __fastcall ResetBuffer();
		double __fastcall GetWrongPositionShare();

	public:
		__fastcall TiPositionTracker(TiAnimationManager* aManager, TiSize aScreenSize, TiSize aViewport);
		__fastcall ~TiPositionTracker();

		virtual void __fastcall mouseClick(TMouseButton aButton, TShiftState aShift, int aX, int aY);

		bool __fastcall isPositionWrong(SampleStruct& aSample);

		__property bool Enabled = {read = iEnabled, write = iEnabled};
};

//---------------------------------------------------------------------------
};	// namespace

//---------------------------------------------------------------------------
#endif
 