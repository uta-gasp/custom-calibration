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
			pqWrongZ = 1,
			pqWrongXY = 2
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
 