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

		int GetPartIndex(EiPart aPart);

	public:
		void add(TiAnimationManager* aManager, EiPart aPart);
		void placeTo(int aX, int aY);
		void setPartType(EiPart aPart, int aType);
		void paintTo(Gdiplus::Graphics* aGraphics);

		void show();
		void hide();
};

//---------------------------------------------------------------------------
};	// namespace

#endif
