//---------------------------------------------------------------------------
#ifndef PreInstructionH
#define PreInstructionH

//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>

//---------------------------------------------------------------------------
#include "Animation.h"
#include "utils.h"

//---------------------------------------------------------------------------
class TfrmPreInstruction : public TForm
{
	__published:	// IDE-managed Components
		void __fastcall FormCreate(TObject *Sender);
		void __fastcall FormPaint(TObject *Sender);
		void __fastcall FormDestroy(TObject *Sender);
	void __fastcall FormClick(TObject *Sender);
	void __fastcall FormKeyUp(TObject *Sender, WORD &Key, TShiftState Shift);

	public:
		enum EiInstruction
		{
			instFinished = 0,
			instStandard = 's',
			instFirefly = 'f',
			instProfiled = 'p'
		};

	private:	// User declarations
		Gdiplus::Graphics* iGraphics;

		TiAnimation* iInstruction;
		TiRect iButtonContinue;
		bool iHasInstruction;
		bool iWasAborted;

		EiInstruction __fastcall GetInstruction();
		void __fastcall SetInstruction(EiInstruction aValue);

	public:		// User declarations
		__fastcall TfrmPreInstruction(TComponent* aOwner);

		__property EiInstruction Instruction = {read = GetInstruction, write = SetInstruction};
		__property bool WasAborted = {read = iWasAborted};
};

//---------------------------------------------------------------------------
#endif
