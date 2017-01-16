//---------------------------------------------------------------------------
#ifndef CalibPointH
#define CalibPointH

//---------------------------------------------------------------------------
#include <dynarray.h>

//---------------------------------------------------------------------------
#include "Animation.h"

//---------------------------------------------------------------------------
class TiCalibPoint
{
	class TiWorm : public TiAnimation
	{
		private:
			int iLampX;
			int iLampY;
			
			double iDistanceToLamp;
			double iOriginalAngle;

		public:
			__fastcall TiWorm(int aLampX, int aLampY);

			void __fastcall moveAway();
			void __fastcall moveClose();
	};

	typedef TiDynArray<TiWorm> TiWorms;

	private:
		int iX;
		int iY;
		bool iEnabled;
		TiAnimation* iLamp;
		TiWorms iWorms;

		void __fastcall onWormsHideRequest(TObject* aSender);
		void __fastcall onWormFadingFinished(TObject* aSender);

	public:
		__fastcall TiCalibPoint(TiAnimationManager* aManager, int aX, int aY);

		void __fastcall lightOn();
		void __fastcall lightOff();

		void __fastcall paintTo(Gdiplus::Graphics* aGraphics);

		__property int X = {read = iX};
		__property int Y = {read = iY};
		__property TiAnimation* Lamp = {read = iLamp};
		__property bool Enabled = {read = iEnabled, write = iEnabled};
};

//---------------------------------------------------------------------------
#endif
 