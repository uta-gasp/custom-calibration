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
		struct SiHidingOlio {
			int X;
			int Y;
			int Width;
			int Height;
			SiHidingOlio() { }
			SiHidingOlio(int aX, int aY, int aW, int aH) : X(aX), Y(aY), Width(aW), Height(aH) { }
		};

	private:
		TiAnimations iHidingOlios;
		TiAnimation* iResultBackground;
		TiAnimation* iBestTimeLogo1;
		TiAnimation* iBestTimeLogo2;

		double iBestTime;
		String iBestTimeDate;
		bool iIsBestTime;
		bool iShowBestTimeLogo;

		__int64 iStartTime;
		DWORD iSysTimerFreq;
		double iDuration;

		bool __fastcall GetIsRunning();

		void __fastcall onBestTimeLogoShow(TObject* aSender);

	public:
		__fastcall TiGame(TiAnimationManager* aManager);

		void __fastcall start(int aOliosToShow);
		bool __fastcall click(int aX, int aY); // return true if all done

		void __fastcall paintTo(Gdiplus::Graphics* aGraphics);

		__property bool IsRunning = {read = GetIsRunning};
		__property double Duration = {read = iDuration};
		__property double BestTime = {read = iBestTime, write = iBestTime};
		__property String BestTimeDate = {read = iBestTimeDate, write = iBestTimeDate};
		__property bool ShowBestTimeLogo = {read = iShowBestTimeLogo, write = iShowBestTimeLogo};
};

//---------------------------------------------------------------------------
#endif
 