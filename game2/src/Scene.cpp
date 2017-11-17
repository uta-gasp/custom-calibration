//---------------------------------------------------------------------------
#include "Scene.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
using namespace ProfiledGame;

//---------------------------------------------------------------------------
__fastcall TiScene::TiScene(TiAnimationManager* aManager, TiSize aScreenSize, TiSize aViewport) :
		TObject(),
		iManager(aManager),
		iScreenSize(aScreenSize),
		iViewport(aViewport),
		iIsVisible(false),
		FOnDone(NULL),
		FOnAbort(NULL)
{
	iStaticAssets = new TiAssets(false);
	iDynamicAssets = new TiAssets(false);
}

//---------------------------------------------------------------------------
__fastcall TiScene::~TiScene()
{
	delete iStaticAssets;
	delete iDynamicAssets;
}

//---------------------------------------------------------------------------
void __fastcall TiScene::show()
{
	iIsVisible = true;

	for (int i = 0; i < iStaticAssets->Count; i++)
		iStaticAssets->get(i)->show();
	for (int i = 0; i < iDynamicAssets->Count; i++)
		iDynamicAssets->get(i)->show();
}

//---------------------------------------------------------------------------
void __fastcall TiScene::hide()
{
	iIsVisible = false;

	for (int i = 0; i < iStaticAssets->Count; i++)
		iStaticAssets->get(i)->hide();
	for (int i = 0; i < iDynamicAssets->Count; i++)
		iDynamicAssets->get(i)->hide();
}

//---------------------------------------------------------------------------
void __fastcall TiScene::paintTo(Gdiplus::Graphics* aGraphics, EiUpdateType aUpdateType)
{
	if (aUpdateType & updStatic)
	{
		for (int i = 0; i < iStaticAssets->Count; i++)
			iStaticAssets->get(i)->paintTo(aGraphics);
	}
	if (aUpdateType & updNonStatic)
	{
		for (int i = 0; i < iDynamicAssets->Count; i++)
			iDynamicAssets->get(i)->paintTo(aGraphics);
	}
}

//---------------------------------------------------------------------------
void __fastcall TiScene::mouseClick(TMouseButton aButton, TShiftState aShift, int aX, int aY)
{
}

//---------------------------------------------------------------------------
void __fastcall TiScene::keyUp(WORD& aKey, TShiftState aShift)
{
	if (aKey == VK_ESCAPE && FOnAbort)
		FOnAbort(this);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TPoint __fastcall TiScene::GetOffset()
{
	return TPoint(
		(iScreenSize.Width - iViewport.Width)/2,
		(iScreenSize.Height - iViewport.Height)/2
	);
}

