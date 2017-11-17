//---------------------------------------------------------------------------
#ifndef GAME2_InstructionColorH
#define GAME2_InstructionColorH

//---------------------------------------------------------------------------
#include "LevelInstruction.h"

//---------------------------------------------------------------------------
namespace ProfiledGame {

//---------------------------------------------------------------------------
class TiInstructionColor : public TiLevelInstruction
{
	public:
		enum EiColorID
		{
			cidRed = 0,
			cidGreed,
			cidBlue,
			cidYellow,
			cidCount
		};

	public:
		__fastcall TiInstructionColor(TiAnimationManager* aManager, TiSize aScreenSize, TiSize aViewport);
};

//---------------------------------------------------------------------------
};	// namespace

#endif
