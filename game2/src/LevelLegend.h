//---------------------------------------------------------------------------
#ifndef GAME2_LevelLegendH
#define GAME2_LevelLegendH

//---------------------------------------------------------------------------
#include "Scene.h"

//---------------------------------------------------------------------------
namespace ProfiledGame {

//---------------------------------------------------------------------------
class TiLevelLegend : public TiScene
{
	private:
		bool iIsDirty;

	protected:
		virtual TiAnimation* __fastcall GetBackground() = 0;

	public:
		__fastcall TiLevelLegend(TiAnimationManager* aManager, TiSize aScreenSize, TiSize aViewport);

		virtual void __fastcall show(bool aIsDirty); // aIsDirty to be used in descendats

		__property bool IsDirty = { read = iIsDirty };
		__property TiAnimation* Background = { read = GetBackground };
};

//---------------------------------------------------------------------------
};	// namespace

#endif
