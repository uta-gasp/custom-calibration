//---------------------------------------------------------------------------
#include "UserPositionQualityEstimator.h"

//---------------------------------------------------------------------------
#include <math.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
using namespace ProfiledGame;

//---------------------------------------------------------------------------
const int KProximityBufferSize = 20;

const double KIdealDistance = 480.0;    // mm
const double KIdealEyeX 		= -28.0;    // px, left eye
const double KIdealEyeY 		=   2.0;    // px, left eye

//---------------------------------------------------------------------------
_fastcall TiUserPositionQualityEstimator::TiUserPositionQualityEstimator(int aThresholdZ, int aThresholdXY) :
		iThresholdZ(aThresholdZ),
		iThresholdXY(aThresholdXY)
{
	iProximities = new TiProximities(true);
}

//---------------------------------------------------------------------------
__fastcall TiUserPositionQualityEstimator::~TiUserPositionQualityEstimator()
{
	delete iProximities;
}

//---------------------------------------------------------------------------
int _fastcall TiUserPositionQualityEstimator::getIdealUserDistance()
{
	return KIdealDistance;
}

//---------------------------------------------------------------------------
TiUserPositionQualityEstimator::PositionQuality _fastcall TiUserPositionQualityEstimator::feed(EyeDataStruct& aEyeData)
{
	SiProximity* proximity = new SiProximity();

	double dx = aEyeData.eyePositionX - KIdealEyeX;
	double dy = aEyeData.eyePositionY - KIdealEyeY;
	proximity->XY = sqrt(dx*dx + dy*dy);
	proximity->Z = aEyeData.eyePositionZ - KIdealDistance;

	if (iProximities->Count == KProximityBufferSize)
		iProximities->remove(0l);

	iProximities->add(proximity);

	PositionQuality result = pqUndefined;

	if (iProximities->Count == KProximityBufferSize)
	{
		result = pqOK;

		double avgZ  = 0;
		double avgXY = 0;
		double weights = 0;
		for (int i = 0; i < iProximities->Count; i++)
		{
			proximity = iProximities->get(i);
			avgZ += proximity->Z;
			avgXY += proximity->XY;
			weights += 1;
		}

		SiProximity avgProximity;
		avgProximity.Z = avgZ / weights;
		avgProximity.XY = avgXY / weights;

		if (avgProximity.Z > iThresholdZ)
			result |= pqTooFar;
		else if (avgProximity.Z < -iThresholdZ)
			result |= pqTooClose;

		if (avgProximity.XY > iThresholdXY)
		{
			result |= dx < 0 ? pqTooLeft : pqTooRight;
			result |= dy < 0 ? pqTooUp : pqTooDown;
		}

		if (result != pqOK)
			iProximities->clear();
	}

	return result;
}