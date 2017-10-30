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

//#define __DEBUG true

#include "utils.h"
#include "Animation.h"
#include "CustomCalibration.h"

//---------------------------------------------------------------------------
class TfrmMainForm : public TForm
{
__published:	// IDE-managed Components
	TButton *Button1;
	TCheckBox *CheckBox1;
	TTrackBar *TrackBar1;
	TTrackBar *TrackBar2;
	TMemo *log;
	TButton *Button2;
	TLabel *Label1;
	TLabel *Label2;
	TPanel *Panel1;
	TButton *Button3;
	TLabel *Label3;
	TTrackBar *TrackBar3;
	TButton *btnStartCalibAndGame;
	TButton *btnStartCalibOnly;
	TButton *btnStartGameOnly;
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall TrackBar1Change(TObject *Sender);
	void __fastcall TrackBar2Change(TObject *Sender);
	void __fastcall FormPaint(TObject *Sender);
	void __fastcall CheckBox1Click(TObject *Sender);
	void __fastcall FormKeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall Panel1MouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall Button3Click(TObject *Sender);
	void __fastcall TrackBar3Change(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall btnStartClick(TObject *Sender);

private:	// User declarations
	double iScaleX;
	double iScaleY;

	TfrmCustomCalibration* iCustomCalibration;
	int iCurrentCalibPointNumber;
	int* iCalibPointStatus;

	TiAnimation* iFly;
	TiAnimationManager* iCreatures;
	Gdiplus::Graphics* iGraphics;
	//Gdiplus::Bitmap* iBackground;
	TiTimeout* iAnimationTimeout;

	TiTimestamp* iTimestamp;
	TiLogger* iEvents;
	TiLogger* iSamples;

	void __fastcall CreateCalibration();
	void __fastcall DestroyCalibration();
	void __fastcall VerifyCalibration();

	void __fastcall onCreaturesPaint(TObject* aSender, EiUpdateType aUpdateType);
	void __fastcall onAnimationFinished(TObject* aSender);
	void __fastcall onMoveFinished(TObject* aSender);
	void __fastcall onRotationFinished(TObject* aSender);
	void __fastcall onFadingFinished(TObject* aSender);

	void __fastcall onPostAnimation(TObject* aSender);

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
