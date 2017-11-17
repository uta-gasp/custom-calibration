//---------------------------------------------------------------------------
#ifndef GAME2_LoginH
#define GAME2_LoginH

//---------------------------------------------------------------------------
#include "Scene.h"

//---------------------------------------------------------------------------
#include <Controls.hpp>

//---------------------------------------------------------------------------
namespace ProfiledGame {

//---------------------------------------------------------------------------
class TiLogin : public TiScene
{
	private:
		TiRect iButtonContinue;
		TControl* iInput;

	public:
		__fastcall TiLogin(TiAnimationManager* aManager, TiSize aScreenSize, TiSize aViewport, TControl* aInput);

		virtual void __fastcall show();
		virtual void __fastcall hide();

		virtual void __fastcall mouseClick(TMouseButton aButton, TShiftState aShift, int aX, int aY);
};

//---------------------------------------------------------------------------
};	// namespace

#endif
