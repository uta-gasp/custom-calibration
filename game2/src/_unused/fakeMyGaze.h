//---------------------------------------------------------------------------
#ifndef FakeMyGazeH
#define FakeMyGazeH

//---------------------------------------------------------------------------
struct CalibrationPointQualityStruct
{
	int 	number;
	int 	positionX;
	int 	positionY;
	double 	correctedPorX;
	double 	correctedPorY;
	double 	standardDeviationX;
	double 	standardDeviationY;
	int 	usageStatus;
	double 	qualityIndex;

	CalibrationPointQualityStruct(int aNumber, int aPositionX, int aPositionY,
			double aCorrectedPorX, double aCorrectedPorY,
			double aStandardDeviationX, double aStandardDeviationY,
			int aUsageStatus, double aQualityIndex) :
			number(aNumber),
			positionX(aPositionX),
			positionY(aPositionY),
			correctedPorX(aCorrectedPorX),
			correctedPorY(aCorrectedPorY),
			standardDeviationX(aStandardDeviationX),
			standardDeviationY(aStandardDeviationY),
			usageStatus(aUsageStatus),
			qualityIndex(aQualityIndex)
	{ }
};

struct EyeDataStruct
{
	double 	gazeX;
	double 	gazeY;
	double 	diam;
	double 	eyePositionX;
	double 	eyePositionY;
	double 	eyePositionZ;
	EyeDataStruct(double aGazeX, double aGazeY, double aDiam,
			double aEyePositionX, double aEyePositionY, double aEyePositionZ) :
			gazeX(aGazeX),
			gazeY(aGazeY),
			diam(aDiam),
			eyePositionX(aEyePositionX),
			eyePositionY(aEyePositionY),
			eyePositionZ(aEyePositionZ)
	{ }
};

struct SampleStruct
{
	__int64	timestamp;
	EyeDataStruct 	leftEye;
	EyeDataStruct 	rightEye;
	SampleStruct(__int64 aTtimestamp, EyeDataStruct& aLeftEye, EyeDataStruct& aRightEye) :
			timestamp(aTtimestamp), leftEye(aLeftEye), rightEye(aRightEye)
	{ }
};

//---------------------------------------------------------------------------
#endif
