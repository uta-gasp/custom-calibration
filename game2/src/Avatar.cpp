//---------------------------------------------------------------------------
#include "Avatar.h"
#include "Profile.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
using namespace ProfiledGame;

//---------------------------------------------------------------------------
__fastcall TiAvatar::TiAvatar(TiAnimationManager* aManager, TiSize aScreenSize, TiSize aViewport) :
		iManager(aManager),
		iScreenSize(aScreenSize),
		iViewport(aViewport),
		iSelectedPrizes(0)
{
	iPrizes = new TiAnimations(false);

	for (int i = 0; i < IDR_PRIZE_COUNT; i++)
	{
		TiAnimation* prize = new TiAnimation(false);
		prize->addFrames(IDR_PRIZE_CENTER + i, aViewport.Width, aViewport.Height);
		prize->placeTo(aScreenSize.Width / 2, aScreenSize.Height / 2);

		aManager->add(prize);
		iPrizes->add(prize);
	}
}

//---------------------------------------------------------------------------
__fastcall TiAvatar::~TiAvatar()
{
	delete iPrizes;
}

//---------------------------------------------------------------------------
void __fastcall TiAvatar::add(TiAvatar::EiPart aPart)
{
	TiAnimation* part = new TiAnimation(false);
	part->addFrames((int)aPart, iViewport.Width, iViewport.Height);

	switch (aPart)
	{
		case apBody: iBody = part;   break;
		case apEyes: iEyes = part;   break;
		case apHair: iHair = part;   break;
		case apShirt: iShirt = part;	break;
	}

	int index = GetPartIndex(aPart);
	iParts[index] = part;

	iManager->add(part);
}

//---------------------------------------------------------------------------
void __fastcall TiAvatar::setPrizes(int aSelectedPrizes)
{
	iSelectedPrizes = aSelectedPrizes;
	ShowPrizes();
}

//---------------------------------------------------------------------------
void __fastcall TiAvatar::placeTo(int aX, int aY)
{
	for (int i = 0; i < apCount; i++)
		iParts[i]->placeTo(aX, aY);
	for (int i = 0; i < iPrizes->Count; i++)
		iPrizes->get(i)->placeTo(aX, aY);
}

//---------------------------------------------------------------------------
void __fastcall TiAvatar::setPartType(EiPart aPart, int aType)
{
	int index = GetPartIndex(aPart);

	iParts[index]->setFrame(aType);
}

//---------------------------------------------------------------------------
void __fastcall TiAvatar::paintTo(Gdiplus::Graphics* aGraphics)
{
	for (int i = 0; i < apCount; i++)
		iParts[i]->paintTo(aGraphics);
	for (int i = 0; i < iPrizes->Count; i++)
		iPrizes->get(i)->paintTo(aGraphics);
}

//---------------------------------------------------------------------------
void __fastcall TiAvatar::show()
{
	for (int i = 0; i < apCount; i++)
		iParts[i]->show();

	ShowPrizes();
}

//---------------------------------------------------------------------------
void __fastcall TiAvatar::hide()
{
	for (int i = 0; i < apCount; i++)
		iParts[i]->hide();
	for (int i = 0; i < iPrizes->Count; i++)
		iPrizes->get(i)->hide();
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int __fastcall TiAvatar::GetPartIndex(EiPart aPart)
{
	return (aPart & 0xFF) >> 4; // check assers.h to figure out why the index is computed this way
}

//---------------------------------------------------------------------------
void __fastcall TiAvatar::ShowPrizes()
{
	int prizes = iSelectedPrizes;
	int index = 0;
	while (index < iPrizes->Count)
	{
		TiAnimation* prize = iPrizes->get(index);

		if (prizes & 1)
			prize->show();
		else if (prize->IsVisible)
			prize->hide();
			
		index++;
		prizes >>= 1;
	}
}
