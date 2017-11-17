//---------------------------------------------------------------------------
#include "InstructionColor.h"
#include "assets_pg.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
using namespace ProfiledGame;

//---------------------------------------------------------------------------
__fastcall TiInstructionColor::TiInstructionColor(TiAnimationManager* aManager,
		TiSize aScreenSize, TiSize aViewport) :
		TiLevelInstruction(aManager, aScreenSize, aViewport)
{
	iLevelRcID = IDR_INSTRUCTIONS_LEVEL1;
}

