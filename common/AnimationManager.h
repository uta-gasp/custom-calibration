//---------------------------------------------------------------------------
#ifndef AnimationManagerH
#define AnimationManagerH

//---------------------------------------------------------------------------
#include <extctrls.hpp>

//---------------------------------------------------------------------------
#include "AnimationBase.h"

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
	public:
		typedef void __fastcall (__closure *FiOnPaint)(System::TObject* Sender, EiUpdateType aUpdateType);

	private:
		TiAnimationBaseList iAnimations;
		TTimer* iUpdateTimer;

		FiOnPaint FOnPaint;

		int  _fastcall GetAnimationCount();

		void __fastcall onAnimationPaint(TObject* aSender);
		void __fastcall onUpdateTimer(TObject* aSender);

	public:
		__fastcall TiAnimationManager();
		__fastcall ~TiAnimationManager();

		void __fastcall add(TiAnimationBase* aAnimation);
		void __fastcall paintTo(Gdiplus::Graphics* aGraphics);
		/*
		void __fastcall startAnimation();
		void __fastcall stopAnimation();
		void __fastcall toggleAnimation();
		void __fastcall stopMove();

		void __fastcall setLoopAnimation(bool aValue);
		void __fastcall setAnimationSpeed(int aValue); 	  // frames/s
		void __fastcall setMoveSpeed(double aValue);      // px/s
		void __fastcall setRotationSpeed(double aValue);  // deg/s

		TiAnimation* operator[](int aIndex);
		*/

		__property int AnimationCount = {read = GetAnimationCount};

		__property FiOnPaint OnPaint = {read = FOnPaint, write = FOnPaint};
};

//---------------------------------------------------------------------------
#endif
 