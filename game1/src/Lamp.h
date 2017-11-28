//---------------------------------------------------------------------------
#ifndef GAME1_CalibPointH
#define GAME1_CalibPointH

//---------------------------------------------------------------------------
#include <dynarray.h>

//---------------------------------------------------------------------------
#include "Animation.h"
#include "AnimationManager.h"

//---------------------------------------------------------------------------
namespace FireflyAndPoints {

//---------------------------------------------------------------------------
class TiLamp
{
	class TiOlio;
	typedef TiDynArray<TiOlio> TiOlios;

	class TiOlio : public TiAnimation
	{
		private:
			int iLampX;
			int iLampY;

			double iDistanceToLamp;
			double iDirection;

			void __fastcall StartIdle(bool aNewSpeed = true);

			void __fastcall onAnimationFinished(TObject* aSender);

		public:
			__fastcall TiOlio(int aLampX, int aLampY, double aDirectionFromLamp = -1);

			static double __fastcall getDirectionFromLamp(TiOlios* aOthers, int aMaxCount); // in degrees

			void __fastcall idle();
			void __fastcall appear();
			void __fastcall disappear();
			//void __fastcall moveAway();
			//void __fastcall moveClose();
			__property double Direction = {read = iDirection};
	};

	private:
		int iX;
		int iY;
		bool iEnabled;
		TiAnimation* iLamp;
		TiOlios iOlios;

		int iAcceptanceCount;

		//void __fastcall onOlioHideRequest(TObject* aSender);
		//void __fastcall onOlioFadingFinished(TObject* aSender);

	public:
		__fastcall TiLamp(TiAnimationManager* aManager, int aX, int aY);

		void __fastcall show(bool aSetOliosOnIdle = false);
		void __fastcall hide();
		void __fastcall lightOn();
		void __fastcall lightOff();

		void __fastcall accept();

		void __fastcall paintTo(Gdiplus::Graphics* aGraphics, EiUpdateType aUpdateType);

		__property int X = {read = iX};
		__property int Y = {read = iY};
		__property TiAnimation* Lamp = {read = iLamp};
		__property bool Enabled = {read = iEnabled, write = iEnabled};
		__property int AcceptanceCount = {read = iAcceptanceCount};
};

//---------------------------------------------------------------------------
};	// namespace

#endif
