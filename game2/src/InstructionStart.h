//---------------------------------------------------------------------------
#ifndef GAME2_InstructionStartH
#define GAME2_InstructionStartH

//---------------------------------------------------------------------------
#include "Scene.h"

//---------------------------------------------------------------------------
namespace ProfiledGame {

//---------------------------------------------------------------------------
class TiInstructionStart : public TiScene
{
	private:
		TiRect iButtonContinue;

	public:
		__fastcall TiInstructionStart(TiAnimationManager* aManager, TiSize aScreenSize, TiSize aViewport);

		virtual void __fastcall mouseClick(TMouseButton aButton, TShiftState aShift, int aX, int aY);
};

//---------------------------------------------------------------------------
};	// namespace

#endif
