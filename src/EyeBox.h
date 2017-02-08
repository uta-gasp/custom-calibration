//---------------------------------------------------------------------------
#ifndef EyeBoxH
#define EyeBoxH

//---------------------------------------------------------------------------
#include "Animation.h"

#ifndef RET_SUCCESS
#include "myGazeAPI.h"
#endif

//---------------------------------------------------------------------------
class TiEyeBox : public TObject
{
	public:
		struct TiSize
		{
			int Width;
			int Height;
			__fastcall TiSize() : Width(0), Height(0) { }
			__fastcall TiSize(int aWidth, int aHeight) : Width(aWidth), Height(aHeight) { }
		};

	private:
		TRect iBox;
		TiAnimation* iBackground;
		TiAnimation* iLeft;
		TiAnimation* iRight;
		TiAnimation* iWarning;
		TiAnimation* iInstructionTop;
		TiAnimation* iInstructionBottom;
		TiAnimation* iStart;
		TiAnimation* iClose;

		bool iVisible;
		int iInstabilityCounter;

		TNotifyEvent FOnHidden;

		void __fastcall onBackgroundFadingFinished(TObject* aSender);

		void __fastcall SetEyeLocation(TiAnimation* aEye, double aX, double aY);
		void __fastcall SetEyeScale(TiAnimation* aEye, double aDist);
		double __fastcall GetScale(double aDist);

		void __fastcall SetVisible(bool aValue);

	public:
		__fastcall TiEyeBox(TiAnimationManager* aManager, TRect aBox, TiSize aScreenSize);
		__fastcall ~TiEyeBox();

		void __fastcall left(EyeDataStruct& aEyeData);
		void __fastcall right(EyeDataStruct& aEyeData);
		void __fastcall setTrackingStability(bool aStable);

		void __fastcall paintTo(Gdiplus::Graphics* aGraphics, EiUpdateType aUpdateType);

		__property TRect Rect = {read = iBox};
		__property TiAnimation* Background = {read = iBackground};
		__property TiAnimation* Left = {read = iLeft};
		__property TiAnimation* Right = {read = iRight};
		__property TiAnimation* Warning = {read = iWarning};
		__property TiAnimation* Start = {read = iStart};
		//__property TiAnimation* Close = {read = iClose};

		__property bool IsVisible = {read = iVisible, write = SetVisible};

		__property TNotifyEvent OnHidden = {read = FOnHidden, write = FOnHidden};
};

//---------------------------------------------------------------------------
#endif
