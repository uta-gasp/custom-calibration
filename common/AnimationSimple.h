//---------------------------------------------------------------------------
#ifndef AnimationSimpleH
#define AnimationSimpleH

//---------------------------------------------------------------------------
#include "AnimationBase.h"

//---------------------------------------------------------------------------
class TiAnimationSimple : public TiAnimationBase
{
	private:
		bool iIsVisible;

	protected:
		virtual bool __fastcall GetIsVisible();

	public:
		__fastcall TiAnimationSimple(bool aIsStatic = true);

		virtual void __fastcall paintTo(Gdiplus::Graphics* aGraphics);
		virtual void __fastcall show();
		virtual void __fastcall hide();

		void __fastcall addFrames(UINT aID);
};

//---------------------------------------------------------------------------
#endif
