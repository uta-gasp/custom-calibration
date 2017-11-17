//---------------------------------------------------------------------------
#ifndef GAME2_InstructionSittingH
#define GAME2_InstructionSittingH

//---------------------------------------------------------------------------
#include "Scene.h"

//---------------------------------------------------------------------------
namespace ProfiledGame {

//---------------------------------------------------------------------------
class TiInstructionSitting : public TiScene
{
	private:
		TiRect iButtonContinue;

	public:
		__fastcall TiInstructionSitting(TiAnimationManager* aManager, TiSize aScreenSize, TiSize aViewport);

		virtual void __fastcall mouseClick(TMouseButton aButton, TShiftState aShift, int aX, int aY);
};

//---------------------------------------------------------------------------
};	// namespace

#endif
