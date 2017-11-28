//---------------------------------------------------------------------------
#ifndef CalibQualityEstimatorH
#define CalibQualityEstimatorH

//---------------------------------------------------------------------------
#include <dynarray.h>
#include <deque>

#include <system.hpp>

//---------------------------------------------------------------------------
template <typename T>
struct TiPoint
{
	T X;
	T Y;

	TiPoint(T aX, T aY) : X(aX), Y(aY) { }

	TiPoint<T> operator -(const TiPoint<T>& aRef) const {
		return TiPoint<T>(this->X - aRef.X, this->Y - aRef.Y);
	}

	double length() const {
		return sqrt(X*X + Y*Y);
	}
};

//---------------------------------------------------------------------------
class TiQualityMetric
{
	private:
		double iValue; // pixels

		static int DPI;
		static double CM_PER_INCH;

	public:
		TiQualityMetric() { }
		TiQualityMetric(double aValue) :   // aValue in pixels
				iValue(aValue) { }

		double pixels() const
			{ return iValue; }

		double cm() const
			{ return iValue * CM_PER_INCH / DPI; }

		double deg(double aDistCm) const
			{ return 2 * atan(cm() / 2 / aDistCm) * 180 / M_PI; }
};

//---------------------------------------------------------------------------
struct SiQuality
{
	int X, Y;
	TiQualityMetric Precision;
	TiQualityMetric Accuracy;

	SiQuality() : X(0), Y(0) { }
	SiQuality(int aX, int aY, TiQualityMetric aPrecision, TiQualityMetric aAccuracy) :
			X(aX), Y(aY), Precision(aPrecision), Accuracy(aAccuracy) { }

	String toString(const char* aHeader = "");
};

//---------------------------------------------------------------------------
class TiCalibQualityEstimator : public TObject
{
	public:
		typedef TiPoint<int> TiPointI;
		typedef TiPoint<double> TiPointD;

		typedef void __fastcall (__closure *FiOnEvent)(System::TObject* aSender, const String& aMsg);

	private:
		typedef std::deque<TiPointI> TiPoints;
		typedef TiDynArray<SiQuality> TiQualities;

		TiPoints iGazePoints;
		TiQualities* iQualities;

		size_t iBufferLength;

		FiOnEvent FOnEvent;

		bool __fastcall GetAverage(TiPointD& aResult, const TiPoints& aPoints);
		bool __fastcall GetStandardDeviation(TiPointD& aResult, const TiPoints& aPoints, TiPointD* aMean = NULL);
		TiPointD __fastcall GetPoint();

	public:
		__fastcall TiCalibQualityEstimator();
		__fastcall ~TiCalibQualityEstimator();

		void __fastcall addSample(const TiPointI& aPoint);
		bool __fastcall addSelection(int aTargetX, int aTargetY);
		void __fastcall reset();

		int __fastcall estimate(SiQuality& aQuality);

		__property size_t BufferLength = {read = iBufferLength, write = iBufferLength};
		__property TiPointD Point = {read = GetPoint};

		__property FiOnEvent OnEvent = {read = FOnEvent, write = FOnEvent};
};

//---------------------------------------------------------------------------
#endif
