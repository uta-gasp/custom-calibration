//---------------------------------------------------------------------------
#include "AnimationManager.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
__fastcall TiAnimationManager::TiAnimationManager() :
	TObject(),
	FOnPaint(NULL)
{
	iAnimations.DeleteContent = true;

	iUpdateTimer = new TTimer(NULL);
	iUpdateTimer->Enabled = true;
	iUpdateTimer->Interval = 35;
	iUpdateTimer->OnTimer = onUpdateTimer;
}

//---------------------------------------------------------------------------
__fastcall TiAnimationManager::~TiAnimationManager()
{
	delete iUpdateTimer;
}

//---------------------------------------------------------------------------
void __fastcall TiAnimationManager::add(TiAnimationBase* aAnimationBase)
{
	aAnimationBase->Parent = this;
	iAnimations.add(aAnimationBase);
}

//---------------------------------------------------------------------------
void __fastcall TiAnimationManager::paintTo(Gdiplus::Graphics* aGraphics)
{
	for (int i = 0; i < iAnimations.Count; i++)
		iAnimations[i]->paintTo(aGraphics);
}
/*
//---------------------------------------------------------------------------
void __fastcall TiAnimationManager::startAnimation()
{
	for (int i = 0; i < iAnimations.Count; i++)
		iAnimations[i]->startAnimation();
}

//---------------------------------------------------------------------------
void __fastcall TiAnimationManager::stopAnimation()
{
	for (int i = 0; i < iAnimations.Count; i++)
		iAnimations[i]->stopAnimation();
}

//---------------------------------------------------------------------------
void __fastcall TiAnimationManager::toggleAnimation()
{
	for (int i = 0; i < iAnimations.Count; i++)
		iAnimations[i]->toggleAnimation();
}

//---------------------------------------------------------------------------
void __fastcall TiAnimationManager::stopMove()
{
	for (int i = 0; i < iAnimations.Count; i++)
		iAnimations[i]->stopMove();
}

//---------------------------------------------------------------------------
void __fastcall TiAnimationManager::setLoopAnimation(bool aValue)
{
	for (int i = 0; i < iAnimations.Count; i++)
		iAnimations[i]->LoopAnimation = aValue;
}

//---------------------------------------------------------------------------
void __fastcall TiAnimationManager::setAnimationSpeed(int aValue)
{
	for (int i = 0; i < iAnimations.Count; i++)
		iAnimations[i]->AnimationSpeed = aValue;
}

//---------------------------------------------------------------------------
void __fastcall TiAnimationManager::setMoveSpeed(double aValue)
{
	for (int i = 0; i < iAnimations.Count; i++)
		iAnimations[i]->MoveSpeed = aValue;
}

//---------------------------------------------------------------------------
void __fastcall TiAnimationManager::setRotationSpeed(double aValue)
{
	for (int i = 0; i < iAnimations.Count; i++)
		iAnimations[i]->RotationSpeed = aValue;
}

//---------------------------------------------------------------------------
TiAnimationBase* TiAnimationManager::operator[](int aIndex)
{
	return aIndex < 0 || aIndex >= iAnimations.Count ? NULL : iAnimations[aIndex];
}
*/

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int __fastcall TiAnimationManager::GetAnimationCount()
{
	return iAnimations.Count;
}

//---------------------------------------------------------------------------
void __fastcall TiAnimationManager::onUpdateTimer(TObject* aSender)
{
	if (!FOnPaint)
		return;

	bool updateRequested = false;
	bool onlyNonStatic = true;
	for (int i = 0; i < iAnimations.Count; i++)
	{
		TiAnimationBase* animation = iAnimations[i];
		bool animationToUpdate = animation->IsPaintRequested;
		updateRequested = animationToUpdate || updateRequested;

		if (animationToUpdate && onlyNonStatic)
			onlyNonStatic = !animation->IsStatic;
	}

	if (updateRequested)
		FOnPaint(this, onlyNonStatic ? updNonStatic : updAll);
}
