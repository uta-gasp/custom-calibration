//---------------------------------------------------------------------------
#ifndef GAME2_RewardsH
#define GAME2_RewardsH

//---------------------------------------------------------------------------
#include "Scene.h"
#include "Profile.h"

//---------------------------------------------------------------------------
namespace ProfiledGame {

//---------------------------------------------------------------------------
class TiRewards : public TiScene
{
	private:
		TiAnimation* iBackground;
		TiAnimation* iBonus;

		TiRect iButtonContinue;

		int iLevel;
		int iScore;
		int iLevelScoreMax;
		int iCoins;

		void __fastcall DrawNumber(Gdiplus::Graphics* aGraphics, int aNumber, int aX, int aY,
			const Gdiplus::Color& aColor, Gdiplus::StringAlignment aAlignment = Gdiplus::StringAlignmentNear);
		void __fastcall DrawString(Gdiplus::Graphics* aGraphics, String& aText, int aX, int aY,
			const Gdiplus::Color& aColor, Gdiplus::StringAlignment aAlignment = Gdiplus::StringAlignmentNear);

	public:
		__fastcall TiRewards(TiAnimationManager* aManager, TiSize aScreenSize, TiSize aViewport);

		virtual void __fastcall show(TiProfile* aProfile);
		virtual void __fastcall showBonus();

		virtual void __fastcall mouseClick(TMouseButton aButton, TShiftState aShift, int aX, int aY);
		virtual void __fastcall paintTo(Gdiplus::Graphics* aGraphics, EiUpdateType aUpdateType);
};

//---------------------------------------------------------------------------
};	// namespace

#endif
