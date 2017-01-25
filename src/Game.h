//---------------------------------------------------------------------------
#ifndef GameH
#define GameH

//---------------------------------------------------------------------------
#include <dynarray.h>

//---------------------------------------------------------------------------
#include "Animation.h"

//---------------------------------------------------------------------------
class TiGame
{
	typedef TiDynArray<TiAnimation> TiAnimations;

	public:
		struct SiOlioRect {
			int X;
			int Y;
			int Width;
			int Height;
			SiOlioRect() { }
			SiOlioRect(int aX, int aY, int aW, int aH) : X(aX), Y(aY), Width(aW), Height(aH) { }
		};

	private:
		TiAnimations iHidingOlios;

		__int64 iStartTime;
		double iDuration;
		DWORD iSysTimerFreq;

	public:
		__fastcall TiGame(TiAnimationManager* aManager);

		void __fastcall start(int aOliosToShow);
		bool __fastcall click(int aX, int aY); // return true if all done

		void __fastcall paintTo(Gdiplus::Graphics* aGraphics);
};

//---------------------------------------------------------------------------
#endif
 