//---------------------------------------------------------------------------
#ifndef MainFormH
#define MainFormH

//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>

#include "utils.h"
#include "Animation.h"
#include "FireflyAndPoints.h"

//---------------------------------------------------------------------------
class TfrmMainForm : public TForm
{
__published:	// IDE-managed Components
	TMemo *log;
	TButton *btnStartCalibAndGame;
	TButton *btnStartCalibOnly;
	TButton *btnStartGameOnly;
	TComboBox *cmbTarget;
	TLabel *Label4;
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall btnStartClick(TObject *Sender);

private:	// User declarations
	double iScaleX;
	double iScaleY;

	TiFireflyAndPoints* iCustomCalibration;
	int iCurrentCalibPointNumber;
	CalibrationPointUsageStatusEnum* iCalibPointStatus;

	TiTimestamp* iTimestamp;
	TiLogger* iEvents;
	TiLogger* iSamples;

	void __fastcall CreateCalibration();
	void __fastcall DestroyCalibration();
	void __fastcall VerifyCalibration();

	void __fastcall onCalibrationEvent(TObject* aSender, const String& aMsg);
	void __fastcall onCalibrationSample(TObject* aSender, double aX, double aY);
	void __fastcall onCalibrationStart(TObject* aSender);
	void __fastcall onCalibrationReadyToCalibrate(TObject* aSender);
	void __fastcall onRecalibrateSinglePoint(TObject* aSender, int aPointNumber, bool aIsSinglePointMode);
	void __fastcall onCalibrationPointReady(TObject* aSender, int aPointIndex, bool aIsSinglePointMode);
	void __fastcall onCalibrationPointAccepted(TObject* aSender, int aPointIndex, bool aIsSinglePointMode);
	void __fastcall onCalibrationPointAborted(TObject* aSender, int aPointIndex, bool aIsSinglePointMode);
	void __fastcall onCalibrationFinished(TObject* aSender);
	void __fastcall onCalibrationAborted(TObject* aSender);
	void __fastcall onCalibrationGameStarted(TObject* aSender);
	void __fastcall onCalibrationGameFinished(TObject* aSender);

	void __fastcall onCalibrationMouseMove(TObject* aSender, TShiftState Shift, int X, int Y);

public:		// User declarations
	__fastcall TfrmMainForm(TComponent* Owner);
	virtual __fastcall ~TfrmMainForm();
};

//---------------------------------------------------------------------------
#endif
