//---------------------------------------------------------------------------
#ifndef GAME2_ProfileEditorH
#define GAME2_ProfileEditorH

//---------------------------------------------------------------------------
#include "Scene.h"
#include "Animation.h"
#include "Profile.h"
#include "utils.h"
#include "assets_pg.h"

//---------------------------------------------------------------------------
namespace ProfiledGame {

//---------------------------------------------------------------------------
class TiProfileEditor : public TiScene
{
	enum EiPart
	{
		pepSkin = 0,
		pepHair,
		pepEyes,
		pepShirt,
		pepCount,
	};

	enum EiSelection
	{
		pesCount = 3
	};

	struct SiCheckbox
	{
		TiAnimation* Selection;
		TiRect Area;
		SiCheckbox() {}
		SiCheckbox(TiAnimation* aSelection, TiRect& aArea) : Selection(aSelection), Area(aArea) {}
	};

	private:
		TiAnimation* iBackground;
		TiAnimation* iTitle;
		TiAnimation* iButtons;
		TiAnimation* iSelection[pepCount];
		SiCheckbox* iCheckbox[pepCount * pesCount];

		TiRect iReadyButton;

		TiProfile* iProfile;

		void __fastcall DrawName(Gdiplus::Graphics* aGraphics);
		
	public:
		TiProfileEditor(TiAnimationManager* aManager, TiSize aScreenSize, TiSize aViewport);

		void __fastcall show(TiProfile* iProfile);
		virtual void __fastcall hide();

		void __fastcall paintTo(Gdiplus::Graphics* aGraphics, EiUpdateType aUpdateType);
		void __fastcall mouseClick(TMouseButton aButton, TShiftState aShift, int aX, int aY);
};

//---------------------------------------------------------------------------
};	// namespace

#endif
