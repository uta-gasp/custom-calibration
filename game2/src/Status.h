//---------------------------------------------------------------------------
#ifndef GAME2_StatusH
#define GAME2_StatusH

//---------------------------------------------------------------------------
#include "Scene.h"
#include "Profile.h"

//---------------------------------------------------------------------------
namespace ProfiledGame {

//---------------------------------------------------------------------------
class TiStatus : public TiScene
{
	private:
		TiAnimation* iBackground;
		TiAnimation* iPanel;
		TiAnimation* btnButtonContinue;

		TiRect iButtonContinue;

		TiProfile* iProfile;

		void __fastcall DrawNumber(Gdiplus::Graphics* aGraphics, int aNumber, int aX, int aY,
			const Gdiplus::Color& aColor, Gdiplus::StringAlignment aAlignment = Gdiplus::StringAlignmentNear);
		void __fastcall DrawString(Gdiplus::Graphics* aGraphics, const WideString& aText, int aX, int aY,
			const Gdiplus::Color& aColor, Gdiplus::StringAlignment aAlignment = Gdiplus::StringAlignmentNear);

		void __fastcall DrawScoreBackground(Gdiplus::Graphics* aGraphics);

	public:
		__fastcall TiStatus(TiAnimationManager* aManager, TiSize aScreenSize, TiSize aViewport);

		virtual void __fastcall show(TiProfile* aProfile);
		virtual void __fastcall hide();

		virtual void __fastcall mouseClick(TMouseButton aButton, TShiftState aShift, int aX, int aY);
		virtual void __fastcall paintTo(Gdiplus::Graphics* aGraphics, EiUpdateType aUpdateType);
};

//---------------------------------------------------------------------------
};	// namespace

#endif
