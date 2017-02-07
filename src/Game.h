//---------------------------------------------------------------------------
#ifndef GameH
#define GameH

//---------------------------------------------------------------------------
#include <dynarray.h>

//---------------------------------------------------------------------------
#include "Animation.h"
#include "utils.h"

//---------------------------------------------------------------------------
class TiGame : public TObject
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
		TiAnimation* iBestScoreLogo1;
		TiAnimation* iBestScoreLogo2;

		double iBestScore;
		String iBestScoreDate;
		bool iIsBestScore;
		bool iShowBestScoreLogo;

		__int64 iStartTime;
		DWORD iSysTimerFreq;
		double iDuration;     // seconds
		int iMonstersFound;
		int iScore;

		int iTimeout;         // seconds
		TiTimeout* iTimeoutRef;

		TNotifyEvent FOnFinished;

		bool __fastcall GetIsRunning();
		int __fastcall ComputeScore(double aDuration, int aMonstersFound);
		void __fastcall ComputeAndShowScore();

		void __fastcall StopOnTimeout(TObject* aSender);
		void __fastcall ShowBestScoreLogos(TObject* aSender);

	public:
		__fastcall TiGame(TiAnimationManager* aManager);

		void __fastcall start(int aOliosToShow);
		void __fastcall click(int aX, int aY); // return true if all done

		void __fastcall paintTo(Gdiplus::Graphics* aGraphics);

		__property bool IsRunning = {read = GetIsRunning};
		__property double Duration = {read = iDuration};
		__property int MonstersFound = {read = iMonstersFound};
		__property int Score = {read = iScore};
		__property double BestScore = {read = iBestScore, write = iBestScore};
		__property String BestScoreDate = {read = iBestScoreDate, write = iBestScoreDate};
		__property bool ShowBestScoreLogo = {read = iShowBestScoreLogo, write = iShowBestScoreLogo};
		__property int Timeout = {read = iTimeout, write = iTimeout};

		__property TNotifyEvent OnFinished = {read = FOnFinished, write = FOnFinished};
};

//---------------------------------------------------------------------------
#endif
