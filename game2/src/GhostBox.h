//---------------------------------------------------------------------------
#ifndef GAME2_GhostBoxH
#define GAME2_GhostBoxH

//---------------------------------------------------------------------------
#include "LevelLegend.h"
#include "AnimationSimple.h"

//---------------------------------------------------------------------------
namespace ProfiledGame {

//---------------------------------------------------------------------------
class TiGhostBox : public TiLevelLegend
{
	private:
		TiAnimation* iBoxClosed;
		TiAnimation* iBoxOpened;
		TiAnimation* iButtonGreen;
		TiAnimation* iButtonRed;
		TiAnimationSimple* iGhostsEscaping;
		TiAnimationSimple* iGhostsTrapping;

		TiTimeout* iTimeout;
		int iFlashesLeft;

		void __fastcall onGhostsEscapingAnimationFinished(TObject* aSender = NULL);
		void __fastcall onGhostsTrappingAnimationFinished(TObject* aSender = NULL);

		void __fastcall FlashButtonGreen(TObject* aSender = NULL);
		void __fastcall FlashButtonRed(TObject* aSender = NULL);
		void __fastcall AnimateGhostsEscaping(TObject* aSender = NULL);
		void __fastcall AnimateGhostsTrapping(TObject* aSender = NULL);
		void __fastcall AfterTrapped(TObject* aSender = NULL);

	protected:
		virtual TiAnimation* __fastcall GetBackground();

	public:
		__fastcall TiGhostBox(TiAnimationManager* aManager, TiSize aScreenSize, TiSize aViewport);

		virtual void __fastcall show(bool aIsDirty);
		virtual void __fastcall hide();
};

//---------------------------------------------------------------------------
};	// namespace

#endif
