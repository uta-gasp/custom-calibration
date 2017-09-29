//---------------------------------------------------------------------------
#ifndef GameH
#define GameH

//---------------------------------------------------------------------------
#include <dynarray.h>

//---------------------------------------------------------------------------
#include "Animation.h"
#include "utils.h"

//---------------------------------------------------------------------------
class TiGameTimer : public TiAnimation
{
	private:
		TTimer* iTimer;
		double iMillisecondsLeft;

		TNotifyEvent FOnStop;

		void __fastcall Tick(TObject* aSender);

		int __fastcall GetTimeout();
		void __fastcall SetTimeout(int aValue);

	public:
		__fastcall TiGameTimer(int aTimeout);
		__fastcall ~TiGameTimer();

		virtual void __fastcall paintTo(Gdiplus::Graphics* aGraphics);

		void __fastcall start();
		void __fastcall stop();

		__property int Timeout = {read = GetTimeout, write = SetTimeout};

		__property TNotifyEvent OnStop = {read = FOnStop, write = FOnStop};
};

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
			int GazeSpotX;
			int GazeSpotY;

			SiHidingOlio() { }
			SiHidingOlio(int aX, int aY, int aW, int aH, int aGSX, int aGSY) :
					X(aX), Y(aY), Width(aW), Height(aH), GazeSpotX(aGSX), GazeSpotY(aGSY) { }
		};

	public:
		typedef void __fastcall (__closure *FiOnEvent)(System::TObject* aSender, const String& aMsg);
		typedef void __fastcall (__closure *FiOnSelect)(System::TObject* aSender, int aTargetX, int aTargetY);

	private:
		TiAnimations iHidingOlios;
		TiAnimation* iResultBackground;
		TiAnimation* iBestScoreLogo1;
		TiAnimation* iBestScoreLogo2;
		TiAnimation* iInstruction;
		TiGameTimer* iCountdown;
		TiAnimation* iPointer;

		int iBestScore;
		String iBestScoreDate;
		bool iIsBestScore;
		bool iShowBestScoreLogo;

		__int64 iStartTime;
		DWORD iSysTimerFreq;
		double iDuration;     // seconds
		int iOliosFound;
		int iScore;

		int iOliosToFind;
		int iTimeout;         // seconds
		TiTimeout* iTimeoutRef;

		FiOnEvent FOnEvent;
		FiOnSelect FOnSelect;
		TNotifyEvent FOnFinished;

		bool __fastcall GetIsRunning();
		bool __fastcall GetIsInstructionVisible();
		void __fastcall SetTimeout(int aValue);
		int __fastcall ComputeScore(double aDuration, int aOliosFound);
		void __fastcall ComputeAndShowScore();

		void __fastcall ShowBestScoreLogos(TObject* aSender);

	public:
		__fastcall TiGame(TiAnimationManager* aManager, TiSize aScreenSize);

		void __fastcall showInstruction();
		void __fastcall start(int aOliosToShow);
		void __fastcall stop(TObject* aSender = NULL);

		void __fastcall click(int aX = -1, int aY = -1);
		void __fastcall placePointer(int aGazeX, int aGazeY, int aCorrectionX, int aCorrectionY);

		void __fastcall paintTo(Gdiplus::Graphics* aGraphics, EiUpdateType aUpdateType);

		__property bool IsRunning = {read = GetIsRunning};
		__property bool IsInstructionVisible = {read = GetIsInstructionVisible};
		__property double Duration = {read = iDuration};
		__property int OliosFound = {read = iOliosFound};
		__property int Score = {read = iScore};
		__property bool IsBestScore = {read = iIsBestScore};
		__property int BestScore = {read = iBestScore, write = iBestScore};
		__property String BestScoreDate = {read = iBestScoreDate, write = iBestScoreDate};
		__property bool ShowBestScoreLogo = {read = iShowBestScoreLogo, write = iShowBestScoreLogo};
		__property int Timeout = {read = iTimeout, write = SetTimeout};

		__property FiOnEvent OnEvent = {read = FOnEvent, write = FOnEvent};
		__property FiOnSelect OnSelect = {read = FOnSelect, write = FOnSelect};
		__property TNotifyEvent OnFinished = {read = FOnFinished, write = FOnFinished};
};

//---------------------------------------------------------------------------
#endif
