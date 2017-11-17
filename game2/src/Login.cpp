//---------------------------------------------------------------------------
#include "Login.h"
#include "assets_pg.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
using namespace ProfiledGame;

//---------------------------------------------------------------------------
__fastcall TiLogin::TiLogin(TiAnimationManager* aManager,
		TiSize aScreenSize, TiSize aViewport, TControl* aInput) :
		TiScene(aManager, aScreenSize, aViewport),
		iInput(aInput)
{
	TiAnimation* background = new TiAnimation(false);
	background->addFrames(IDR_INSTRUCTIONS_LOGIN, aViewport.Width, aViewport.Height);
	background->placeTo(aScreenSize.Width / 2, aScreenSize.Height / 2);
	aManager->add(background);

	iStaticAssets->add(background);

	iButtonContinue = TiRect(Offset.x + 583, Offset.y + 450, 200, 60);

	iInput->Left = (iScreenSize.Width - iInput->Width) / 2;
	iInput->Top = iButtonContinue.Y - 36 - iInput->Height;
}

//---------------------------------------------------------------------------
void __fastcall TiLogin::show()
{
	iInput->Show();
	TiScene::show();
}

//---------------------------------------------------------------------------
void __fastcall TiLogin::hide()
{
	iInput->Hide();
	TiScene::hide();
}

//---------------------------------------------------------------------------
void __fastcall TiLogin::mouseClick(TMouseButton aButton, TShiftState aShift, int aX, int aY)
{
	if (iButtonContinue.hits(aX, aY))
	{
		if (FOnDone)
			FOnDone(this);
	}
}