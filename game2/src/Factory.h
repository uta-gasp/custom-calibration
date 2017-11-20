//---------------------------------------------------------------------------
#ifndef GAME2_FactoryH
#define GAME2_FactoryH

//---------------------------------------------------------------------------
#include "LevelLegend.h"

//---------------------------------------------------------------------------
namespace ProfiledGame {

//---------------------------------------------------------------------------
class TiFactory : public TiLevelLegend
{
	private:
		int iFlashesLeft;

		TiAnimation* iBackground;
		TiAnimation* iAlert;
		TiTimeout* iTimeout;

		void __fastcall Flash(TObject* aSender);
		void __fastcall Cleared(TObject* aSender);

	protected:
		virtual TiAnimation* __fastcall GetBackground();

	public:
		__fastcall TiFactory(TiAnimationManager* aManager, TiSize aScreenSize, TiSize aViewport);

		virtual void __fastcall show(bool aIsDirty);
		virtual void __fastcall hide();
};

//---------------------------------------------------------------------------
};	// namespace

#endif
