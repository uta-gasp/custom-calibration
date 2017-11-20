//---------------------------------------------------------------------------
#include "GhostBox.h"
#include "assets_pg.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
using namespace ProfiledGame;

//---------------------------------------------------------------------------
const int KDurationClosed = 1000;
const int KDurationOpened = 1000;
const int KDurationBeforeEscape = 500;
const int KDurationBeforeTrapped = 1000;

const int KFlashDuration = 100;
const int KTrappedDuration = 1000;
const int KGreenButtonFlashCount = 9;
const int KRedButtonFlashCount = 7;

//---------------------------------------------------------------------------
__fastcall TiGhostBox::TiGhostBox(TiAnimationManager* aManager,
		TiSize aScreenSize, TiSize aViewport) :
		TiLevelLegend(aManager, aScreenSize, aViewport),
		iTimeout(NULL)
{
	iBoxClosed = new TiAnimation(false);
	iBoxClosed->addFrames(IDR_STORY_LEVEL2_BOX_CLOSED, aViewport.Width, aViewport.Height);
	iBoxClosed->placeTo(aScreenSize.Width / 2, aScreenSize.Height / 2);
	aManager->add(iBoxClosed);
	iStaticAssets->add(iBoxClosed);

	iBoxOpened = new TiAnimation(false);
	iBoxOpened->addFrames(IDR_STORY_LEVEL2_BOX_OPENED, aViewport.Width, aViewport.Height);
	iBoxOpened->placeTo(aScreenSize.Width / 2, aScreenSize.Height / 2);
	aManager->add(iBoxOpened);
	iStaticAssets->add(iBoxOpened);

	iButtonGreen = new TiAnimation(false);
	iButtonGreen->addFrames(IDR_STORY_LEVEL2_BUTTON_GREEN, aViewport.Width, aViewport.Height);
	iButtonGreen->placeTo(aScreenSize.Width / 2, aScreenSize.Height / 2);
	aManager->add(iButtonGreen);
	iStaticAssets->add(iButtonGreen);

	iButtonRed = new TiAnimation(false);
	iButtonRed->addFrames(IDR_STORY_LEVEL2_BUTTON_RED, aViewport.Width, aViewport.Height);
	iButtonRed->placeTo(aScreenSize.Width / 2, aScreenSize.Height / 2);
	aManager->add(iButtonRed);
	iStaticAssets->add(iButtonRed);

	iGhostsEscaping = new TiAnimation(false, false);
	iGhostsEscaping->addFrames(IDR_STORY_LEVEL2_GHOSTS_ESCAPE, aViewport.Width, aViewport.Height);
	iGhostsEscaping->LoopAnimation = false;
	iGhostsEscaping->RewindAnimationAfterStop = false;
	iGhostsEscaping->AllowAnimationReversion = false;
	iGhostsEscaping->AnimationSpeed = 10;
	iGhostsEscaping->placeTo(aScreenSize.Width / 2, aScreenSize.Height / 2);
	iGhostsEscaping->OnAnimationFinished = onGhostsEscapingAnimationFinished;
	aManager->add(iGhostsEscaping);
	iDynamicAssets->add(iGhostsEscaping);

	iGhostsTrapping = new TiAnimation(false, false);
	iGhostsTrapping->addFrames(IDR_STORY_LEVEL2_GHOSTS_TRAPPED, aViewport.Width, aViewport.Height);
	iGhostsTrapping->LoopAnimation = false;
	iGhostsTrapping->RewindAnimationAfterStop = false;
	iGhostsTrapping->AllowAnimationReversion = false;
	iGhostsTrapping->AnimationSpeed = 10;
	iGhostsTrapping->placeTo(aScreenSize.Width / 2, aScreenSize.Height / 2);
	iGhostsTrapping->OnAnimationFinished = onGhostsTrappingAnimationFinished;
	aManager->add(iGhostsTrapping);
	iDynamicAssets->add(iGhostsTrapping);
}

//---------------------------------------------------------------------------
void __fastcall TiGhostBox::show(bool aIsDirty)
{
	TiLevelLegend::show(aIsDirty);

	iGhostsEscaping->hide();
	iGhostsTrapping->hide();

	if (aIsDirty)
	{
		iBoxOpened->hide();
		iButtonRed->hide();
		iFlashesLeft = KGreenButtonFlashCount;
		TiTimeout::run(KDurationClosed, FlashButtonGreen, &iTimeout);
	}
	else
	{
		iBoxClosed->hide();
		iButtonGreen->hide();
		iFlashesLeft = KRedButtonFlashCount;
		TiTimeout::run(KDurationOpened, FlashButtonRed, &iTimeout);
	}
}

//---------------------------------------------------------------------------
void __fastcall TiGhostBox::hide()
{
	iGhostsEscaping->OnAnimationFinished = NULL;
	iGhostsEscaping->stopAnimation();
	iGhostsTrapping->stopAnimation();

	TiLevelLegend::hide();

	if (iTimeout)
	{
		iTimeout->kill();
		iTimeout = NULL;
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TiGhostBox::onGhostsEscapingAnimationFinished(TObject* aSender)
{
	if (FOnDone)
		FOnDone(this);
}

//---------------------------------------------------------------------------
void __fastcall TiGhostBox::onGhostsTrappingAnimationFinished(TObject* aSender)
{
	iBoxOpened->hide();
	iBoxClosed->show();

	TiTimeout::run(KTrappedDuration, AfterTrapped, &iTimeout);
}

//---------------------------------------------------------------------------
void __fastcall TiGhostBox::FlashButtonGreen(TObject* aSender)
{
	iTimeout = NULL;

	if (iButtonGreen->IsVisible)
		iButtonGreen->hide();
	else
		iButtonGreen->show();

	if (--iFlashesLeft > 0)
	{
		TiTimeout::run(KFlashDuration, FlashButtonGreen, &iTimeout);
	}
	else
	{
		iBoxClosed->hide();
		iBoxOpened->show();
		iButtonRed->show();
		iGhostsEscaping->show();
		TiTimeout::run(KDurationBeforeEscape, AnimateGhostsEscaping, &iTimeout);
	}
}

//---------------------------------------------------------------------------
void __fastcall TiGhostBox::FlashButtonRed(TObject* aSender)
{
	iTimeout = NULL;

	if (iButtonRed->IsVisible)
		iButtonRed->hide();
	else
		iButtonRed->show();

	if (--iFlashesLeft > 0)
	{
		TiTimeout::run(KFlashDuration, FlashButtonRed, &iTimeout);
	}
	else
	{
		iButtonGreen->show();
		iGhostsTrapping->show();
		TiTimeout::run(KDurationBeforeTrapped, AnimateGhostsTrapping, &iTimeout);
	}
}

//---------------------------------------------------------------------------
void __fastcall TiGhostBox::AnimateGhostsEscaping(TObject* aSender)
{
	iTimeout = NULL;
	iGhostsEscaping->startAnimation();
}


//---------------------------------------------------------------------------
void __fastcall TiGhostBox::AnimateGhostsTrapping(TObject* aSender)
{
	iTimeout = NULL;
	iGhostsTrapping->startAnimation();
}

//---------------------------------------------------------------------------
void __fastcall TiGhostBox::AfterTrapped(TObject* aSender)
{
	if (FOnDone)
		FOnDone(this);
}

//---------------------------------------------------------------------------
TiAnimation* __fastcall TiGhostBox::GetBackground()
{
	return iBoxOpened;
}

