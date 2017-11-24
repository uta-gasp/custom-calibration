//---------------------------------------------------------------------------
#ifndef GAME2_SceneH
#define GAME2_SceneH

//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <dynarray.h>

//---------------------------------------------------------------------------
#include "Animation.h"
#include "AnimationManager.h"
#include "utils.h"

//---------------------------------------------------------------------------
namespace ProfiledGame {

//---------------------------------------------------------------------------
class TiScene : public TObject
{
	protected:
		typedef TiDynArray<TiAnimationBase> TiAssets;

		TiAnimationManager* iManager;
		TiSize iScreenSize;
		TiSize iViewport;

		bool iIsVisible;

		TiAssets* iStaticAssets;
		TiAssets* iDynamicAssets;

		TNotifyEvent FOnDone;
		TNotifyEvent FOnAbort;

		TPoint __fastcall GetOffset();

		__property TPoint Offset = {read = GetOffset};

	public:
		__fastcall TiScene(TiAnimationManager* aManager, TiSize aScreenSize, TiSize aViewport);
		__fastcall ~TiScene();

		virtual void __fastcall show();
		virtual void __fastcall hide();

		virtual void __fastcall paintTo(Gdiplus::Graphics* aGraphics, EiUpdateType aUpdateType);
		virtual void __fastcall mouseClick(TMouseButton aButton, TShiftState aShift, int aX, int aY);
		virtual void __fastcall keyUp(WORD& aKey, TShiftState aShift);

		__property bool IsVisible = {read = iIsVisible};
		__property TNotifyEvent OnDone = {read = FOnDone, write = FOnDone};
		__property TNotifyEvent OnAbort = {read = FOnAbort, write = FOnAbort};
};

//---------------------------------------------------------------------------
};	// namespace

#endif
