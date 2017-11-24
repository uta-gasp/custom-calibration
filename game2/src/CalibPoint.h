//---------------------------------------------------------------------------
#ifndef GAME2_CalibPointH
#define GAME2_CalibPointH

//---------------------------------------------------------------------------
#include <dynarray.h>

//---------------------------------------------------------------------------
#include "Animation.h"
#include "AnimationManager.h"

//---------------------------------------------------------------------------
namespace ProfiledGame {

//---------------------------------------------------------------------------
class TiCalibPoint
{
	private:
		int iX;
		int iY;
		bool iEnabled;
		TiAnimation* iCalibTarget;

	public:
		__fastcall TiCalibPoint(TiAnimationManager* aManager, int aX, int aY, int aLevel);

		void __fastcall show();
		void __fastcall hide();
		void __fastcall start();
		void __fastcall stop();

		void __fastcall paintTo(Gdiplus::Graphics* aGraphics, EiUpdateType aUpdateType);

		__property int X = {read = iX};
		__property int Y = {read = iY};
		__property TiAnimation* CalibTarget = {read = iCalibTarget};
		__property bool Enabled = {read = iEnabled, write = iEnabled};
};

//---------------------------------------------------------------------------
};	// namespace

#endif
