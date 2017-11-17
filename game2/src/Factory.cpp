//---------------------------------------------------------------------------
#include "Factory.h"
#include "assets_pg.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
using namespace ProfiledGame;

//---------------------------------------------------------------------------
const int KFlashCount = 6;
const int KFlashDuration = 300; // ms
const int KClearedDuration = 1500;

//---------------------------------------------------------------------------
__fastcall TiFactory::TiFactory(TiAnimationManager* aManager,
		TiSize aScreenSize, TiSize aViewport) :
		TiLevelLegend(aManager, aScreenSize, aViewport),
		iTimeout(NULL)
{
	TiAnimation* background = new TiAnimation(false);
	background->addFrames(IDR_STORY_LEVEL1_BACKGROUND, aViewport.Width, aViewport.Height);
	background->placeTo(aScreenSize.Width / 2, aScreenSize.Height / 2);
	aManager->add(background);
	iStaticAssets->add(background);

	iAlert = new TiAnimation(false);
	iAlert->addFrames(IDR_STORY_LEVEL1_ALERT, aViewport.Width, aViewport.Height);
	iAlert->placeTo(aScreenSize.Width / 2, aScreenSize.Height / 2);
	aManager->add(iAlert);
	iDynamicAssets->add(iAlert);
}

//---------------------------------------------------------------------------
void __fastcall TiFactory::show(bool aIsDirty)
{
	TiLevelLegend::show(aIsDirty);

	iAlert->hide();

	if (aIsDirty)
	{
		iFlashesLeft = KFlashCount;
		TiTimeout::run(KFlashDuration, Flash, &iTimeout);
	}
	else
	{
		TiTimeout::run(KClearedDuration, Cleared, &iTimeout);
	}
}

//---------------------------------------------------------------------------
void __fastcall TiFactory::hide()
{
	TiLevelLegend::hide();

	if (iTimeout)
	{
		iTimeout->kill();
		iTimeout = NULL;
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TiFactory::Flash(TObject* aSender)
{
	iTimeout = NULL;

	if (iAlert->IsVisible)
		iAlert->hide();
	else
		iAlert->show();

	if (--iFlashesLeft > 0)
		TiTimeout::run(KFlashDuration, Flash, &iTimeout);
	else if (FOnDone)
		FOnDone(this);
}

//---------------------------------------------------------------------------
void __fastcall TiFactory::Cleared(TObject* aSender)
{
	if (FOnDone)
		FOnDone(this);
}

