//---------------------------------------------------------------------------
#include "LevelLegend.h"
#include "assets_pg.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
using namespace ProfiledGame;

//---------------------------------------------------------------------------
__fastcall TiLevelLegend::TiLevelLegend(TiAnimationManager* aManager,
		TiSize aScreenSize, TiSize aViewport) :
		TiScene(aManager, aScreenSize, aViewport)
{
}

//---------------------------------------------------------------------------
void __fastcall TiLevelLegend::show(bool aIsDirty)
{
	iIsDirty = aIsDirty;
	TiScene::show();
}

