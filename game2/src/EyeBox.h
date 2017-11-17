//---------------------------------------------------------------------------
#ifndef GAME2_EyeBoxH
#define GAME2_EyeBoxH

//---------------------------------------------------------------------------
#include "Scene.h"
#include "Animation.h"
#include "Profile.h"
#include "utils.h"

#ifndef RET_SUCCESS
#include "myGazeAPI.h"
#endif

//---------------------------------------------------------------------------
#include <dynarray.h>

//---------------------------------------------------------------------------
namespace ProfiledGame {

//---------------------------------------------------------------------------
class TiEyeBox : public TiScene
{
	struct SiProximity
	{
		double Z;
		double XY;
		SiProximity() : Z(0), XY(0) {}
	};

	typedef TiDynArray<SiProximity> TiProximities;

	private:
		TiRect iEyeArea;

		TiAnimation* iBackground;
		TiAnimation* iLeft;
		TiAnimation* iRight;

		TiProfile* iProfile;

		TiProximities* iProximities;
		SiProximity iAvgProximity;

		void __fastcall SetEyeLocation(TiAnimation* aEye, double aX, double aY);
		void __fastcall SetEyeScale(TiAnimation* aEye, double aDist);
		void __fastcall EstimatePositionQuality(EyeDataStruct& aEyeData);
		double __fastcall GetScale(double aDist);

	public:
		__fastcall TiEyeBox(TiAnimationManager* aManager, TiSize aScreenSize, TiSize aViewport);
		__fastcall ~TiEyeBox();

		void __fastcall show(TiProfile* aProfile);
		virtual void __fastcall hide();

		void __fastcall left(EyeDataStruct& aEyeData);
		void __fastcall right(EyeDataStruct& aEyeData);

		void __fastcall paintTo(Gdiplus::Graphics* aGraphics, EiUpdateType aUpdateType);
};

//---------------------------------------------------------------------------
};	// namespace

#endif
