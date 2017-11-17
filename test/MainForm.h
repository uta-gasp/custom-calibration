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

//---------------------------------------------------------------------------
#include "Controller.h"

//---------------------------------------------------------------------------
class TfrmMainForm : public TForm
{
__published:	// IDE-managed Components
	TMemo *log;
	TButton *btnStartCalib3;
	TButton *btnStartCalib1;
	TButton *btnStartCalib2;
	TButton *btnRunUserDay;
	TComboBox *cmbUsers;
	TComboBox *cmbDays;
	void __fastcall btnStartCalib3Click(TObject *Sender);
	void __fastcall btnStartCalib1Click(TObject *Sender);
	void __fastcall btnStartCalib2Click(TObject *Sender);
	void __fastcall cmbUsersOrDaysChange(TObject *Sender);
	void __fastcall btnRunUserDayClick(TObject *Sender);

private:	// User declarations
	TiController* iController;

	String iSettingsFileName;

	double iScaleX;
	double iScaleY;

	int iCurrentCalibPointNumber;

	void __fastcall CreateController();
	void __fastcall DeleteController();

	void __fastcall VerifyCalibration();

	void __fastcall onCtrl_Debug(TObject* aSender, const String& aMsg);
	void __fastcall onCtrl_CalibrationStarted(TObject* aSender);
	void __fastcall onCtrl_CalibrationDisplayReady(TObject* aSender);
	void __fastcall onCtrl_PointAborted(TObject* aSender, int aPointID, bool aIsRecalibrating);
	void __fastcall onCtrl_PointAccepted(TObject* aSender, int aPointID, bool aIsRecalibrating);
	void __fastcall onCtrl_RecalibPoint(TObject* aSender, int aPointID);
	void __fastcall onCtrl_CalibrationFinished(TObject* aSender);
	//void __fastcall onCtrl_VerificationStarted(TObject* aSender);
	//void __fastcall onCtrl_VerificationFinished(TObject* aSender);
	//void __fastcall onCtrl_Aborted(TObject* aSender);

public:		// User declarations
	__fastcall TfrmMainForm(TComponent* Owner);
	virtual __fastcall ~TfrmMainForm();
};

//---------------------------------------------------------------------------
#endif
