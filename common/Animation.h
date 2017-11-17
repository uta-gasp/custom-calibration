//---------------------------------------------------------------------------
#ifndef AnimationH
#define AnimationH

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
class TiAnimation : public TObject
{
	typedef TiDynArray<Gdiplus::Bitmap> TiAnimations;

	public:
		typedef void __fastcall (__closure *TiOnFadingTransition)(System::TObject* aSender, double aAlpha);

	private:
		TiAnimationManager* iParent;
		TiAnimations iAnimations;
		Gdiplus::Bitmap* iAnimationFrames;
		
		TTimer* iAnimationTimer;
		TTimer* iMoveTimer;
		TTimer* iRotationTimer;
		TTimer* iFadingTimer;
		TTimer* iUpdateTimer;

		int iWidth;
		int iHeight;
		int iX;
		int iY;
		double iMoveSpeed;
		double iRotationSpeed;
		double iScale;
		double iAngle;
		int iAnimationIndex;
		int iFrameIndex;
		int iFrameCount;
		bool iIsStatic;
		bool iLoopAnimation;
		bool iRewindAnimationAfterStop;
		bool iAllowAnimationReversion;
		bool iIsPaintRequested;

		TPoint iDestinationLocation;
		double iMoveX, iMoveY;
		double iStartX, iStartY;
		double iDestinationAngle;
		double iRotationAngle;
		double iStartAngle;
		int iFadingDirection;
		int iFadingStepCount;
		double iAlpha;
		TPoint iCenter;
		int iAnimationDirection;

		TNotifyEvent FOnPaint;
		TNotifyEvent FOnAnimationFinished;
		TNotifyEvent FOnMoveFinished;
		TNotifyEvent FOnRotationFinished;
		TNotifyEvent FOnFadingFinished;
		TiOnFadingTransition FOnFadingTransition;

		void __fastcall AddFrames(Gdiplus::Bitmap* aFrames, int aWidth, int aHeight);

		void __fastcall SetParent(TiAnimationManager* aValue);
		bool __fastcall GetIsAnimating();
		bool __fastcall GetIsMoving();
		bool __fastcall GetIsRotating();
		bool __fastcall GetIsPaintRequested();
		bool __fastcall GetIsVisible();

		void __fastcall SetAnimationIndex(int aValue);
		double __fastcall GetAnimationSpeed();
		void __fastcall SetAnimationSpeed(double aValue);
		int  __fastcall GetFadingDuration();
		void __fastcall SetFadingDuration(int aValue);
		void __fastcall SetFadingStepCount(int aValue);
		void __fastcall SetScale(double aValue);
		void __fastcall SetAngle(double aValue);
		void __fastcall SetCenter(TPoint aCenter);
		void __fastcall SetOnPaint(TNotifyEvent aValue);

		void __fastcall onAnimationTimer(TObject* aSender);
		void __fastcall onMoveTimer(TObject* aSender);
		void __fastcall onRotationTimer(TObject* aSender);
		void __fastcall onFadingTimer(TObject* aSender);
		void __fastcall onUpdateTimer(TObject* aSender);

	public:
		__fastcall TiAnimation(bool aVisible = true, bool aIsStatic = true);
		__fastcall ~TiAnimation();

		void __fastcall addFrames(const wchar_t* aPath, const wchar_t* aFileName, int aWidth, int aHeight = 0);
		void __fastcall addFrames(const wchar_t* aName, int aWidth, int aHeight = 0);
		void __fastcall addFrames(UINT aID, int aWidth, int aHeight = 0);

		void __fastcall startAnimation();
		void __fastcall stopAnimation();
		void __fastcall toggleAnimation();
		void __fastcall resetAnimation();
		void __fastcall setFrame(int aIndex = -1);

		bool __fastcall fadeOut();
		bool __fastcall fadeIn();

		void __fastcall show();
		void __fastcall hide();

		void __fastcall placeTo(int aX, int aY);
		bool __fastcall moveTo(int aX, int aY);
		void __fastcall stopMove();

		void __fastcall rotateBy(double aAngle);
		void __fastcall stopRotation();

		void __fastcall setOrientation(double aDX, double aDY);

		bool __fastcall hitTest(int aX, int aY);
		double __fastcall distanceTo(int aX, int aY); 		// distance to center
		TPoint __fastcall clientToScreen(int aX, int aY);

		void __fastcall invalidate();
		virtual void __fastcall paintTo(Gdiplus::Graphics* aGraphics);

		__property TiAnimationManager* Parent = {read = iParent, write = SetParent};

		__property int X = {read = iX};
		__property int Y = {read = iY};
		__property int Width = {read = iWidth};
		__property int Height = {read = iHeight};
		__property bool IsStatic = {read = iIsStatic};
		__property bool IsAnimating = {read = GetIsAnimating};
		__property bool IsMoving = {read = GetIsMoving};
		__property bool IsRotating = {read = GetIsRotating};
		__property bool IsPaintRequested = {read = GetIsPaintRequested};
		__property bool IsVisible = {read = GetIsVisible};
		__property int FadingDirection = {read = iFadingDirection};
		__property double Opacity = {read = iAlpha};
		__property int FrameIndex = {read = iFrameIndex};

		__property int AnimationIndex = {read = iAnimationIndex, write = SetAnimationIndex};
		__property bool LoopAnimation = {read = iLoopAnimation, write = iLoopAnimation};
		__property bool RewindAnimationAfterStop = {read = iRewindAnimationAfterStop, write = iRewindAnimationAfterStop};
		__property bool AllowAnimationReversion = {read = iAllowAnimationReversion, write = iAllowAnimationReversion};
		__property double AnimationSpeed = {read = GetAnimationSpeed, write = SetAnimationSpeed}; // frames/s
		__property double MoveSpeed = {read = iMoveSpeed, write = iMoveSpeed};                 // px/s
		__property double RotationSpeed = {read = iRotationSpeed, write = iRotationSpeed};     // deg/s
		__property int FadingDuration = {read = GetFadingDuration, write = SetFadingDuration}; // ms
		__property int FadingStepCount = {read = iFadingStepCount, write = SetFadingStepCount};
		__property double Scale = {read = iScale, write = SetScale};
		__property double Angle = {read = iAngle, write = SetAngle};
		__property TPoint Center = {read = iCenter, write = SetCenter};

		__property TNotifyEvent OnPaint = {read = FOnPaint, write = SetOnPaint};
		__property TNotifyEvent OnAnimationFinished = {read = FOnAnimationFinished, write = FOnAnimationFinished};
		__property TNotifyEvent OnMoveFinished = {read = FOnMoveFinished, write = FOnMoveFinished};
		__property TNotifyEvent OnRotationFinished = {read = FOnRotationFinished, write = FOnRotationFinished};
		__property TNotifyEvent OnFadingFinished = {read = FOnFadingFinished, write = FOnFadingFinished};
		__property TiOnFadingTransition OnFadingTransition = {read = FOnFadingTransition, write = FOnFadingTransition};
};

//---------------------------------------------------------------------------
enum EiUpdateType
{
	updNone = 0,
	updStatic = 1,
	updNonStatic = 2,
	updAll = 3
};

//---------------------------------------------------------------------------
class TiAnimationManager : public TObject
{
	typedef TiDynArray<TiAnimation> TiAnimations;

	public:
		typedef void __fastcall (__closure *FiOnPaint)(System::TObject* Sender, EiUpdateType aUpdateType);

	private:
		TiAnimations iAnimations;
		TTimer* iUpdateTimer;

		FiOnPaint FOnPaint;

		int  _fastcall GetAnimationCount();

		void __fastcall onAnimationPaint(TObject* aSender);
		void __fastcall onUpdateTimer(TObject* aSender);

	public:
		__fastcall TiAnimationManager();
		__fastcall ~TiAnimationManager();

		void __fastcall add(TiAnimation* aAnimation);
		void __fastcall paintTo(Gdiplus::Graphics* aGraphics);

		void __fastcall startAnimation();
		void __fastcall stopAnimation();
		void __fastcall toggleAnimation();
		void __fastcall stopMove();

		void __fastcall setLoopAnimation(bool aValue);
		void __fastcall setAnimationSpeed(int aValue); 	  // frames/s
		void __fastcall setMoveSpeed(double aValue);      // px/s
		void __fastcall setRotationSpeed(double aValue);  // deg/s

		TiAnimation* operator[](int aIndex);

		__property int AnimationCount = {read = GetAnimationCount};

		__property FiOnPaint OnPaint = {read = FOnPaint, write = FOnPaint};
};

//---------------------------------------------------------------------------
#endif
