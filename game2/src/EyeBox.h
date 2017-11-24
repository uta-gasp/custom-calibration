//---------------------------------------------------------------------------
#ifndef GAME2_EyeBoxH
#define GAME2_EyeBoxH

//---------------------------------------------------------------------------
#include "Scene.h"
#include "Animation.h"
#include "AnimationManager.h"
#include "Profile.h"
#include "UserPositionQualityEstimator.h"
#include "utils.h"

#ifndef RET_SUCCESS
#include "myGazeAPI.h"
#endif

//---------------------------------------------------------------------------
namespace ProfiledGame {

//---------------------------------------------------------------------------
class TiEyeBox : public TiScene
{
	private:
		TiRect iEyeArea;

		TiAnimation* iBackground;
		TiAnimation* iLeft;
		TiAnimation* iRight;

		TiProfile* iProfile;

		TiUserPositionQualityEstimator* iUserPositionQualityEstimatorLeft;
		TiUserPositionQualityEstimator* iUserPositionQualityEstimatorRight;
		bool iEstimateUserPositionQuality;

		void __fastcall SetEyeLocation(TiAnimation* aEye, double aX, double aY);
		void __fastcall SetEyeScale(TiAnimation* aEye, double aDist);
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
