//---------------------------------------------------------------------------
#ifndef AnimationH
#define AnimationH

//---------------------------------------------------------------------------
#include "AnimationBase.h"

//---------------------------------------------------------------------------
class TiAnimation : public TiAnimationBase
{
	public:
		typedef void __fastcall (__closure *TiOnFadingTransition)(System::TObject* aSender, double aAlpha);

	private:
		Gdiplus::Bitmap* iAnimationFrames;

		TTimer* iMoveTimer;
		TTimer* iRotationTimer;
		TTimer* iFadingTimer;

		double iMoveSpeed;
		double iRotationSpeed;
		double iScale;
		double iAngle;
		int iAnimationIndex;

		TPoint iDestinationLocation;
		double iMoveX, iMoveY;
		double iStartX, iStartY;
		double iDestinationAngle;
		double iRotationAngle;
		double iStartAngle;
		int iFadingDirection;
		int iFadingStepCount;
		double iAlpha;

		TNotifyEvent FOnMoveFinished;
		TNotifyEvent FOnRotationFinished;
		TNotifyEvent FOnFadingFinished;
		TiOnFadingTransition FOnFadingTransition;

		void __fastcall AddFrames(Gdiplus::Bitmap* aFrames, int aWidth, int aHeight);

		bool __fastcall GetIsMoving();
		bool __fastcall GetIsRotating();

		void __fastcall SetAnimationIndex(int aValue);
		int  __fastcall GetFadingDuration();
		void __fastcall SetFadingDuration(int aValue);
		void __fastcall SetFadingStepCount(int aValue);
		void __fastcall SetScale(double aValue);
		void __fastcall SetAngle(double aValue);

		void __fastcall onMoveTimer(TObject* aSender);
		void __fastcall onRotationTimer(TObject* aSender);
		void __fastcall onFadingTimer(TObject* aSender);

	protected:
		virtual bool __fastcall GetIsVisible();

	public:
		__fastcall TiAnimation(bool aVisible = true, bool aIsStatic = true);
		virtual __fastcall ~TiAnimation();

		void __fastcall addFrames(const wchar_t* aPath, const wchar_t* aFileName, int aWidth, int aHeight = 0);
		void __fastcall addFrames(const wchar_t* aName, int aWidth, int aHeight = 0);
		void __fastcall addFrames(UINT aID, int aWidth, int aHeight = 0);

		bool __fastcall fadeOut();
		bool __fastcall fadeIn();

		virtual void __fastcall show();
		virtual void __fastcall hide();

		bool __fastcall moveTo(int aX, int aY);
		void __fastcall stopMove();

		void __fastcall rotateBy(double aAngle);
		void __fastcall stopRotation();

		void __fastcall setOrientation(double aDX, double aDY);

		bool __fastcall hitTest(int aX, int aY);
		double __fastcall distanceTo(int aX, int aY); 		// distance to center
		TPoint __fastcall clientToScreen(int aX, int aY);

		virtual void __fastcall paintTo(Gdiplus::Graphics* aGraphics);

		__property bool IsMoving = {read = GetIsMoving};
		__property bool IsRotating = {read = GetIsRotating};
		__property int FadingDirection = {read = iFadingDirection};
		__property double Opacity = {read = iAlpha};

		__property int AnimationIndex = {read = iAnimationIndex, write = SetAnimationIndex};
		__property double MoveSpeed = {read = iMoveSpeed, write = iMoveSpeed};                 // px/s
		__property double RotationSpeed = {read = iRotationSpeed, write = iRotationSpeed};     // deg/s
		__property int FadingDuration = {read = GetFadingDuration, write = SetFadingDuration}; // ms
		__property int FadingStepCount = {read = iFadingStepCount, write = SetFadingStepCount};
		__property double Scale = {read = iScale, write = SetScale};
		__property double Angle = {read = iAngle, write = SetAngle};

		__property TNotifyEvent OnMoveFinished = {read = FOnMoveFinished, write = FOnMoveFinished};
		__property TNotifyEvent OnRotationFinished = {read = FOnRotationFinished, write = FOnRotationFinished};
		__property TNotifyEvent OnFadingFinished = {read = FOnFadingFinished, write = FOnFadingFinished};
		__property TiOnFadingTransition OnFadingTransition = {read = FOnFadingTransition, write = FOnFadingTransition};
};

typedef TiDynArray<TiAnimation> TiAnimations;

//---------------------------------------------------------------------------
#endif
