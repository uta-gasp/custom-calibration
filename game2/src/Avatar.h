//---------------------------------------------------------------------------
#ifndef GAME2_AvatarH
#define GAME2_AvatarH

//---------------------------------------------------------------------------
#include "Animation.h"
#include "utils.h"
#include "assets_pg.h"

//---------------------------------------------------------------------------
namespace ProfiledGame {

//---------------------------------------------------------------------------
class TiAvatar {
	public:
		enum EiPart {
			apBody = IDR_AVATAR_BODY,
			apEyes = IDR_AVATAR_EYES,
			apHair = IDR_AVATAR_HAIR,
			apShirt = IDR_AVATAR_SHIRT,
			apCount = 4
		};

	private:
		TiAnimation* iBody;
		TiAnimation* iEyes;
		TiAnimation* iHair;
		TiAnimation* iShirt;

		TiAnimation* iParts[apCount];
		TiAnimations* iPrizes;
		int iSelectedPrizes;

		TiAnimationManager* iManager;
		TiSize iScreenSize;
		TiSize iViewport;

		int __fastcall GetPartIndex(EiPart aPart);

		void __fastcall ShowPrizes();

	public:
		__fastcall TiAvatar(TiAnimationManager* aManager, TiSize aScreenSize, TiSize aViewport);
		__fastcall TiAvatar::~TiAvatar();

		void __fastcall add(EiPart aPart);
		void __fastcall setPrizes(int aSelectedPrizes);
		void __fastcall placeTo(int aX, int aY);
		void __fastcall setPartType(EiPart aPart, int aType);
		void __fastcall paintTo(Gdiplus::Graphics* aGraphics);

		void __fastcall show();
		void __fastcall hide();
};

//---------------------------------------------------------------------------
};	// namespace

#endif
