//---------------------------------------------------------------------------
#ifndef UserPositionQualityEstimatorH
#define UserPositionQualityEstimatorH

//---------------------------------------------------------------------------
#include <dynarray.h>

#ifndef RET_SUCCESS
#include "myGazeAPI.h"
#endif

//---------------------------------------------------------------------------
namespace ProfiledGame {

//---------------------------------------------------------------------------
class TiUserPositionQualityEstimator
{
	private:
		struct SiProximity
		{
			double Z;
			double XY;
			SiProximity() : Z(0), XY(0) {}
		};

		typedef TiDynArray<SiProximity> TiProximities;

	public:
		enum PositionQuality
		{
			pqUndefined = -1,
			pqOK = 0,
			pqTooClose = 1,
			pqTooFar = 2,
			pqTooLeft = 4,
			pqTooRight = 8,
			pqTooUp = 16,
			pqTooDown = 32
		};

	private:
		TiProximities* iProximities;

		int iThresholdZ;
		int iThresholdXY;

	public:
		_fastcall TiUserPositionQualityEstimator(int aThresholdZ, int aThresholdXY);
		_fastcall ~TiUserPositionQualityEstimator();

		static int __fastcall getIdealUserDistance();

		PositionQuality __fastcall feed(EyeDataStruct& aEyeData);
};

//---------------------------------------------------------------------------
};	// namespace

#endif
 