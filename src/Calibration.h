//---------------------------------------------------------------------------
#ifndef CalibrationH
#define CalibrationH

//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>

#include "Animation.h"
#include "EyeBox.h"
#include "CalibPlot.h"
#include "CalibPoints.h"
#include "utils.h"

//---------------------------------------------------------------------------
class TfrmCalibration : public TForm
{
	__published:
		void __fastcall FormCreate(TObject *Sender);
		void __fastcall FormDestroy(TObject *Sender);
		void __fastcall FormMouseUp(TObject *Sender, TMouseButton Button,
					TShiftState Shift, int X, int Y);
		void __fastcall FormKeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall FormMouseMove(TObject *Sender, TShiftState Shift, int X,
					int Y);

	public:
		typedef void __fastcall (__closure *FiOnDebug)(System::TObject* Sender, const char* aMsg);
		typedef void __fastcall (__closure *FiOnCalibrationPoint)(System::TObject* Sender,
				int aPointIndex, bool aIsSinglePointMode = false);

	private:
		Gdiplus::Graphics* iGraphics;
		Gdiplus::Bitmap* iBackground;
		Gdiplus::Bitmap* iStaticBitmap;

		TiEyeBox* iEyeBox;
		TiCalibPlot* iCalibPlot;
		TiAnimationManager* iObjects;
		TiAnimation* iTarget;
		TiAnimation* iFireFly;
		TiTimeout* iTimeout;

		TiCalibPoints* iCalibPoints;

		FiOnDebug FOnDebug;
		TNotifyEvent FOnStart;
		TNotifyEvent FOnRecalibrateSinglePoint;
		FiOnCalibrationPoint FOnPointReady;
		FiOnCalibrationPoint FOnPointAccept;
		TNotifyEvent FOnFinished;
		TNotifyEvent FOnAborted;

		void __fastcall onObjectPaint(TObject* aSender, EiUpdateType aUpdateType);
		void __fastcall onEyeBoxHidden(TObject* aSender);
		void __fastcall onFireFlyFadingFisnihed(TObject* aSender);
		void __fastcall onFireFlyMoveFisnihed(TObject* aSender);
		void __fastcall onCalibPointTimeout(TObject* aSender);

		void __fastcall StartCalibration();
		void __fastcall RestartCalibration(CalibrationPointQualityStruct* aCalibPoint = NULL);
		void __fastcall MoveToNextPoint();
		void __fastcall Abort();
		void __fastcall Finish();
		void __fastcall Done();

		void __fastcall UpdateCalibPlot();

	public:
		__fastcall TfrmCalibration(TComponent* aOwner);
		__fastcall ~TfrmCalibration();

		void __fastcall setSample(SampleStruct& aSample);
		void __fastcall setTrackingStability(bool aStable);

		__property FiOnDebug OnDebug = {read = FOnDebug, write = FOnDebug};
		__property TNotifyEvent OnStart = {read = FOnStart, write = FOnStart};
		__property TNotifyEvent OnRecalibrateSinglePoint = {read = FOnRecalibrateSinglePoint, write = FOnRecalibrateSinglePoint};
		__property FiOnCalibrationPoint OnPointReady = {read = FOnPointReady, write = FOnPointReady};
		__property FiOnCalibrationPoint OnPointAccept = {read = FOnPointAccept, write = FOnPointAccept};
		__property TNotifyEvent OnFinished = {read = FOnFinished, write = FOnFinished};
		__property TNotifyEvent OnAborted = {read = FOnAborted, write = FOnAborted};
};

//---------------------------------------------------------------------------
#endif
