//---------------------------------------------------------------------------
#ifndef UtilsH
#define UtilsH

//---------------------------------------------------------------------------
#include <extctrls.hpp>

//---------------------------------------------------------------------------
#pragma option push
#pragma warn -hid
#pragma warn -pch
#include "gdiplus.h"
#pragma option pop

//---------------------------------------------------------------------------
#ifdef __DEBUG
#define debug(x) \
	if(__DEBUG)    \
		::MessageBox(NULL, String((x)).c_str(), "myGazeDebug", MB_OK);
#else
#define debug(x)
#endif

//---------------------------------------------------------------------------
bool sameSign(double a, double b);
inline int sign(double a);
inline int sign(float a);
inline int sign(int a);
inline int sign(short a);

int randInRange(int _min, int _max);

bool loadBitmapFromPNG(UINT resourceID, Gdiplus::Bitmap** bitmapOut);

//---------------------------------------------------------------------------
class TiTimeout : public TTimer
{
	private:
		TiTimeout** iRef;

	protected:
		__fastcall TiTimeout(Classes::TComponent* aOwner, TiTimeout** aRef = NULL);

		DYNAMIC void __fastcall Timer(void);

	public:
		static void __fastcall run(UINT timeout, TNotifyEvent aCallback, TiTimeout** aRef = NULL, Classes::TComponent* aOwner = NULL);
		static void __fastcall runSync(UINT timeout, TNotifyEvent aCallback, TiTimeout** aRef = NULL);
		void __fastcall kill();
};

//---------------------------------------------------------------------------
class TiSyncThread : public TThread
{
	private:
		UINT iTimeout;
		TNotifyEvent iCallback;
		TiTimeout** iRef;

		void __fastcall CreateTimer();

	protected:
		virtual void __fastcall Execute();

	public:
		__fastcall TiSyncThread(UINT aTimeout, TNotifyEvent aCallback, TiTimeout** aRef = NULL);
};

//---------------------------------------------------------------------------
#endif
