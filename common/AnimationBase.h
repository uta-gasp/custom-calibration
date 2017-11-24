//---------------------------------------------------------------------------
#ifndef AnimationBaseH
#define AnimationBaseH

//---------------------------------------------------------------------------
#include <extctrls.hpp>
#include <dynarray.h>

//---------------------------------------------------------------------------
#pragma option push
#pragma warn -hid
#pragma warn -pch
#include "gdiplus.h"
#pragma option pop

//---------------------------------------------------------------------------
class TiAnimationManager;

//---------------------------------------------------------------------------
class TiAnimationBase : public TObject
{
	typedef TiDynArray<Gdiplus::Bitmap> TiBitmaps;

	private:
		TiAnimationManager* iParent;

		bool iIsStatic;

		int iWidth;
		int iHeight;
		int iX;
		int iY;
		TPoint iCenter;

		bool iLoopAnimation;
		bool iRewindAnimationAfterStop;
		bool iAllowAnimationReversion;

		TTimer* iAnimationTimer;
		TTimer* iUpdateTimer;

		TNotifyEvent FOnPaint;
		TNotifyEvent FOnAnimationFinished;

		void __fastcall SetParent(TiAnimationManager* aValue);
		void __fastcall SetOnPaint(TNotifyEvent aValue);
		bool __fastcall GetIsPaintRequested();
		bool __fastcall GetIsAnimating();
		double __fastcall GetAnimationSpeed();
		void __fastcall SetAnimationSpeed(double aValue);

		void __fastcall SetCenter(TPoint aCenter);

		void __fastcall onAnimationTimer(TObject* aSender);
		void __fastcall onUpdateTimer(TObject* aSender);

	protected:
		TiBitmaps iBitmaps;

		bool iIsPaintRequested;

		int iAnimationDirection;
		int iFrameIndex;
		int iFrameCount;

		void __fastcall SetSize(int aWidth, int aHeight);
		void __fastcall SetLocation(int aX, int aY);

		virtual bool __fastcall GetIsVisible() = 0;

	public:
		__fastcall TiAnimationBase(bool aIsStatic);
		virtual __fastcall ~TiAnimationBase();

		virtual void __fastcall paintTo(Gdiplus::Graphics* aGraphics) = 0;

		virtual void __fastcall show() = 0;
		virtual void __fastcall hide() = 0;

		virtual void __fastcall invalidate();
		virtual void __fastcall placeTo(int aX, int aY);

		virtual void __fastcall startAnimation();
		virtual void __fastcall stopAnimation();
		virtual void __fastcall toggleAnimation();
		virtual void __fastcall resetAnimation();
		virtual void __fastcall setFrame(int aIndex = -1);   // -1 means random frame

		__property TiAnimationManager* Parent = {read = iParent, write = SetParent};

		__property bool IsStatic = {read = iIsStatic};
		__property bool IsVisible = {read = GetIsVisible};
		__property bool IsPaintRequested = {read = GetIsPaintRequested};
		__property bool IsAnimating = {read = GetIsAnimating};

		__property int X = {read = iX};
		__property int Y = {read = iY};
		__property int Width = {read = iWidth};
		__property int Height = {read = iHeight};
		__property TPoint Center = {read = iCenter, write = SetCenter};

		__property int FrameIndex = {read = iFrameIndex};

		__property double AnimationSpeed = {read = GetAnimationSpeed, write = SetAnimationSpeed}; // frames/s
		__property bool LoopAnimation = {read = iLoopAnimation, write = iLoopAnimation};
		__property bool RewindAnimationAfterStop = {read = iRewindAnimationAfterStop, write = iRewindAnimationAfterStop};
		__property bool AllowAnimationReversion = {read = iAllowAnimationReversion, write = iAllowAnimationReversion};

		__property TNotifyEvent OnPaint = {read = FOnPaint, write = SetOnPaint};
		__property TNotifyEvent OnAnimationFinished = {read = FOnAnimationFinished, write = FOnAnimationFinished};
};

typedef TiDynArray<TiAnimationBase> TiAnimationBaseList;

//---------------------------------------------------------------------------
#endif
