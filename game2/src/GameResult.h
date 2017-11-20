//---------------------------------------------------------------------------
#ifndef GAME2_GameResultH
#define GAME2_GameResultH

//---------------------------------------------------------------------------
#include "Scene.h"
#include "Profile.h"

//---------------------------------------------------------------------------
namespace ProfiledGame {

//---------------------------------------------------------------------------
class TiGameResult : public TiScene
{
	private:
		TiAnimation* iBackground;

		TiRect iButtonContinue;
		TiProfile::SiGameResults iGameResults;

		void __fastcall DrawNumber(Gdiplus::Graphics* aGraphics, int aNumber, int aX);

	public:
		__fastcall TiGameResult(TiAnimationManager* aManager, TiSize aScreenSize, TiSize aViewport);

		virtual void __fastcall show(TiProfile* aProfile, int aLevel);

		virtual void __fastcall mouseClick(TMouseButton aButton, TShiftState aShift, int aX, int aY);
		virtual void __fastcall paintTo(Gdiplus::Graphics* aGraphics, EiUpdateType aUpdateType);
};

//---------------------------------------------------------------------------
};	// namespace

#endif
