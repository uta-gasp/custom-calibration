//---------------------------------------------------------------------------
#ifndef GAME2_RewardsH
#define GAME2_RewardsH

//---------------------------------------------------------------------------
#include "Scene.h"
#include "Profile.h"
#include "AnimationManager.h"

//---------------------------------------------------------------------------
namespace ProfiledGame {

//---------------------------------------------------------------------------
class TiRewards : public TiScene
{
	private:
		TiAnimation* iBackground;
		TiAnimation* iBonus;

		TiRect iButtonContinue;

		TiAnimations* iPrizes;

		int iLevel;
		int iScore;
		int iLevelScoreMax;

		bool iIsNewLevel;
		int iGainedScore;
		int iGainedCoins;
		int iGainedPrizeIndex;

		void __fastcall DrawScoreBackground(Gdiplus::Graphics* aGraphics, double aValue, const Gdiplus::Color& aColor);
		void __fastcall DrawNumber(Gdiplus::Graphics* aGraphics, int aNumber, int aX, int aY,
			const Gdiplus::Color& aColor, Gdiplus::StringAlignment aAlignment = Gdiplus::StringAlignmentNear);
		void __fastcall DrawString(Gdiplus::Graphics* aGraphics, String& aText, int aX, int aY,
			const Gdiplus::Color& aColor, Gdiplus::StringAlignment aAlignment = Gdiplus::StringAlignmentNear);

	public:
		__fastcall TiRewards(TiAnimationManager* aManager, TiSize aScreenSize, TiSize aViewport);
		__fastcall ~TiRewards();

		virtual void __fastcall show(TiProfile* aProfile);
		virtual void __fastcall hide();
		virtual void __fastcall showBonus();

		virtual void __fastcall mouseClick(TMouseButton aButton, TShiftState aShift, int aX, int aY);
		virtual void __fastcall paintTo(Gdiplus::Graphics* aGraphics, EiUpdateType aUpdateType);
};

//---------------------------------------------------------------------------
};	// namespace

#endif
