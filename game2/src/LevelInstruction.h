//---------------------------------------------------------------------------
#ifndef GAME2_LevelInstructionH
#define GAME2_LevelInstructionH

//---------------------------------------------------------------------------
#include "Scene.h"

//---------------------------------------------------------------------------
namespace ProfiledGame {

//---------------------------------------------------------------------------
class TiLevelInstruction : public TiScene
{
	protected:
		TiRect iButtonContinue;
		int iLevelRcID;

	public:
		__fastcall TiLevelInstruction(TiAnimationManager* aManager, TiSize aScreenSize, TiSize aViewport);

		virtual void __fastcall show(int aTargetID);

		virtual void __fastcall mouseClick(TMouseButton aButton, TShiftState aShift, int aX, int aY);
};

//---------------------------------------------------------------------------
};	// namespace

#endif
