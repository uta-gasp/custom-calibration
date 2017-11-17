//---------------------------------------------------------------------------
#ifndef GAME2_InstructionShapeH
#define GAME2_InstructionShapeH

//---------------------------------------------------------------------------
#include "LevelInstruction.h"

//---------------------------------------------------------------------------
namespace ProfiledGame {

//---------------------------------------------------------------------------
class TiInstructionShape : public TiLevelInstruction
{
	public:
		enum EiShapeID
		{
			sidRed = 0,
			sidGreed,
			sidBlue,
			sidYellow,
			sidCount
		};

	public:
		__fastcall TiInstructionShape(TiAnimationManager* aManager, TiSize aScreenSize, TiSize aViewport);
};

//---------------------------------------------------------------------------
};	// namespace

#endif
