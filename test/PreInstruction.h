//---------------------------------------------------------------------------
#ifndef PreInstructionH
#define PreInstructionH

//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>

//---------------------------------------------------------------------------
#include "Animation.h"
#include "utils.h"
#include <ExtCtrls.hpp>

//---------------------------------------------------------------------------
class TfrmPreInstruction : public TForm
{
	__published:	// IDE-managed Components
	TTimer *trmInvalidator;
		void __fastcall FormCreate(TObject *Sender);
		void __fastcall FormPaint(TObject *Sender);
		void __fastcall FormDestroy(TObject *Sender);
	void __fastcall FormClick(TObject *Sender);
	void __fastcall trmInvalidatorTimer(TObject *Sender);

	public:
		enum EiInstruction
		{
			instStart = 0,
			instPause,
			instFinished
		};

	private:	// User declarations
		Gdiplus::Graphics* iGraphics;

		TiAnimation* iInstruction;
		TiRect iButtonContinue;

		EiInstruction __fastcall GetInstruction();
		void __fastcall SetInstruction(EiInstruction aValue);

	public:		// User declarations
		__fastcall TfrmPreInstruction(TComponent* aOwner);

		__property EiInstruction Instruction = {read = GetInstruction, write = SetInstruction};
};

//---------------------------------------------------------------------------
#endif
