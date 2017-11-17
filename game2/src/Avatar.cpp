//---------------------------------------------------------------------------
#include "Avatar.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
using namespace ProfiledGame;

//---------------------------------------------------------------------------
const int KCharPartWidth  = 1366;
const int KCharPartHeight = 768;

//---------------------------------------------------------------------------
void TiAvatar::add(TiAnimationManager* aManager, TiAvatar::EiPart aPart)
{
	TiAnimation* part = new TiAnimation(false);
	part->addFrames((int)aPart, KCharPartWidth, KCharPartHeight);

	switch (aPart)
	{
		case apBody: iBody = part;   break;
		case apEyes: iEyes = part;   break;
		case apHair: iHair = part;   break;
		case apShirt: iShirt = part;	break;
	}

	int index = GetPartIndex(aPart);
	iParts[index] = part;

	aManager->add(part);
}

//---------------------------------------------------------------------------
void TiAvatar::placeTo(int aX, int aY)
{
	for (int i = 0; i < apCount; i++)
		iParts[i]->placeTo(aX, aY);
}

//---------------------------------------------------------------------------
void TiAvatar::setPartType(EiPart aPart, int aType)
{
	int index = GetPartIndex(aPart);

	iParts[index]->setFrame(aType);
}

//---------------------------------------------------------------------------
void TiAvatar::paintTo(Gdiplus::Graphics* aGraphics)
{
	for (int i = 0; i < apCount; i++)
		iParts[i]->paintTo(aGraphics);
}

//---------------------------------------------------------------------------
void TiAvatar::show()
{
	for (int i = 0; i < apCount; i++)
		iParts[i]->show();
}

//---------------------------------------------------------------------------
void TiAvatar::hide()
{
	for (int i = 0; i < apCount; i++)
		iParts[i]->hide();
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int TiAvatar::GetPartIndex(EiPart aPart)
{
	return (aPart & 0xFF) >> 4; // check assers.h to figure out why the index is computed this way
}

