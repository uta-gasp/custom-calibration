//---------------------------------------------------------------------------
#ifndef PositionTrackerH
#define PositionTrackerH

//---------------------------------------------------------------------------
#include "Scene.h"

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

	public:
		__fastcall TiPositionTracker(TiAnimationManager* aManager, TiSize aScreenSize, TiSize aViewport);

		virtual void __fastcall mouseClick(TMouseButton aButton, TShiftState aShift, int aX, int aY);

		bool __fastcall isPositionWrong(SampleStruct& aSample);

		__property bool Enabled = {read = iEnabled, write = iEnabled};
};

//---------------------------------------------------------------------------
};	// namespace

//---------------------------------------------------------------------------
#endif
 