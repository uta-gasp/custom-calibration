//---------------------------------------------------------------------------
#ifndef UtilsH
#define UtilsH

//---------------------------------------------------------------------------
#include <extctrls.hpp>
#include <stdarg.h>

//---------------------------------------------------------------------------
#pragma option push
#pragma warn -hid
#pragma warn -pch
#include "gdiplus.h"
#pragma option pop

//---------------------------------------------------------------------------
#ifdef __DEBUG
#ifndef debug
#define debug(x) \
	if(__DEBUG)    \
		::MessageBox(NULL, String((x)).c_str(), "KidCalib Debug", MB_OK);
#endif
#else
#define debug(x)
#endif

//---------------------------------------------------------------------------
namespace Utils {
	int sign(double a);
	int sign(float a);
	int sign(int a);
	int sign(short a);
	bool sameSign(double a, double b);

	int randInRange(int _min, int _max);

	bool loadBitmapFromPNG(UINT resourceID, Gdiplus::Bitmap** bitmapOut);
};

//---------------------------------------------------------------------------
class TiTimeout : public TTimer
{
	private:
		TiTimeout** iRef;
		bool iKilled;

	protected:
		__fastcall TiTimeout(Classes::TComponent* aOwner, TiTimeout** aRef = NULL);

		DYNAMIC void __fastcall Timer(void);

	public:
		static void __fastcall run(UINT timeout, TNotifyEvent aCallback, TiTimeout** aRef = NULL, Classes::TComponent* aOwner = NULL);
		static void __fastcall runSync(UINT timeout, TNotifyEvent aCallback, TiTimeout** aRef = NULL);

		void __fastcall kill();

		__property bool Killed = {read = iKilled};
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
struct TiSize
{
	int Width;
	int Height;

	__fastcall TiSize() : Width(0), Height(0) { }
	__fastcall TiSize(int aWidth, int aHeight) : Width(aWidth), Height(aHeight) { }
};

//---------------------------------------------------------------------------
struct TiRect
{
	int X;
	int Y;
	int Width;
	int Height;

	__fastcall TiRect() : X(0), Y(0), Width(0), Height(0) { }
	__fastcall TiRect(int aX, int aY, int aWidth, int aHeight) : X(aX), Y(aY), Width(aWidth), Height(aHeight) { }

	bool __fastcall hits(int aX, int aY) {
		return X < aX && aX < (X + Width) && Y < aY && aY < (Y + Height);  
	}
};

//---------------------------------------------------------------------------
class TiTimestamp
{
	private:
		DWORD iFrequency;
		__int64 iStartTime;

	public:
		__fastcall TiTimestamp();

		void __fastcall reset();
		long __fastcall ms();
		double __fastcall sec();
};

//---------------------------------------------------------------------------
class TiLogger
{
	private:
		String iFileName;
		int iFileHandle;

	public:
		__fastcall TiLogger(String aName);
		__fastcall ~TiLogger();

		static String __fastcall folder(String& aName);

		void __fastcall line(String& aText);
};

//---------------------------------------------------------------------------
class TiStrings
{
	private:
		TiStrings() { }

	public:
		static TStrings* split(String& aText, String& aSeparator);
};

//---------------------------------------------------------------------------
#endif
