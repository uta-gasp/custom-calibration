//---------------------------------------------------------------------------
#ifndef GAME2_CalibPointsH
#define GAME2_CalibPointsH

//---------------------------------------------------------------------------
#include <dynarray.h>

//---------------------------------------------------------------------------
#include "Scene.h"
#include "CalibPoint.h"

#ifndef RET_SUCCESS
#include "myGazeAPI.h"
#endif

//---------------------------------------------------------------------------
namespace ProfiledGame {

//---------------------------------------------------------------------------
class TiCalibPoints : public TiScene
{
	public:
		enum EiAnimationDuration	// ms
		{
			adSlow = 500,
			adModerate = 490,
			adFast = 480
		};

		enum EiItemResultType
		{
			irtFailure = 0,
			irtSuccess = 1
		};

	private:
		typedef TiDynArray<TiCalibPoint> TiPoints;

		TiAnimation* iItemResult;
		TiTimestamp* iTimestamp;

		int iLevel;
		TiPoints iPoints;
		int iCurrentPointIndex;
		bool iIsSinglePointMode;
		bool iIsWaitingToAcceptPoint;
		EiAnimationDuration iAnimationDuration;
		int iPointsCompleted;

		int __fastcall GetCount();
		TiCalibPoint* __fastcall GetCurrent();

		void __fastcall Rearrange();
		void __fastcall SetAnimationSpeed();

	public:
		__fastcall TiCalibPoints(TiAnimationManager* aManager, TiSize aScreenSize, TiSize aViewport);
		__fastcall ~TiCalibPoints();

		void __fastcall reset();

		void __fastcall clear(int aLevel = -1);
		void __fastcall add(CalibrationPointStruct& aPoint);
		void __fastcall prepare(int aCalibPointNumber = -1);

		TiCalibPoint* __fastcall next(int aPointNumber = 0);
		void __fastcall waitAcceptance();
		int __fastcall accept();

		EiItemResultType __fastcall showItemResult(int aTargetID);

		virtual void __fastcall paintTo(Gdiplus::Graphics* aGraphics, EiUpdateType aUpdateType);

		TiCalibPoint* operator[](int aIndex);

		__property int Count = {read = GetCount};
		__property TiCalibPoint* Current = {read = GetCurrent};
		__property int CurrentPointIndex = {read = iCurrentPointIndex};
		__property bool IsSinglePointMode = {read = iIsSinglePointMode};
		__property bool IsWaitingToAcceptPoint = {read = iIsWaitingToAcceptPoint};
		__property EiAnimationDuration AnimationDuration = {read = iAnimationDuration};
};

//---------------------------------------------------------------------------
};	// namespace

#endif
