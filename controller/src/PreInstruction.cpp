//---------------------------------------------------------------------------
#include "PreInstruction.h"
#include "assets_ctrl.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
const int KInstructionWidth = 600;
const int KInstructionHeight = 170;

//---------------------------------------------------------------------------
__fastcall TfrmPreInstruction::TfrmPreInstruction(TComponent* aOwner) :
		TForm(aOwner),
		iHasInstruction(false)
{
	iButtonContinue = TiRect(Width / 2, 2 * Height / 2, 200, 60);
}

//---------------------------------------------------------------------------
TfrmPreInstruction::EiInstruction __fastcall TfrmPreInstruction::GetInstruction()
{
	return (EiInstruction)iInstruction->AnimationIndex;
}

//---------------------------------------------------------------------------
void __fastcall TfrmPreInstruction::SetInstruction(EiInstruction aValue)
{
	switch (aValue)
	{
		case instFinished: iInstruction->AnimationIndex = 0; break;
		case instStandard: iInstruction->AnimationIndex = 1; break;
		case instFirefly:	 iInstruction->AnimationIndex = 2; break;
		case instProfiled: iInstruction->AnimationIndex = 3; break;
	}

	iHasInstruction = true;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TfrmPreInstruction::FormCreate(TObject *Sender)
{
	SetBounds(0, 0, Screen->Width, Screen->Height);

	iGraphics = new Gdiplus::Graphics(Handle, false);

	iInstruction = new TiAnimation(true);
	iInstruction->addFrames(IDR_PREINSTRUCTION_FINISHED, KInstructionWidth, KInstructionHeight);
	iInstruction->addFrames(IDR_PREINSTRUCTION_CIRCLE, KInstructionWidth, KInstructionHeight);
	iInstruction->addFrames(IDR_PREINSTRUCTION_FIREFLY, KInstructionWidth, KInstructionHeight);
	iInstruction->addFrames(IDR_PREINSTRUCTION_PROFILED, KInstructionWidth, KInstructionHeight);
	iInstruction->placeTo(Width / 2, Height / 2);
}

//---------------------------------------------------------------------------
void __fastcall TfrmPreInstruction::FormPaint(TObject *Sender)
{
	if (!iHasInstruction)
		return;

	Gdiplus::Bitmap buffer(Width, Height, iGraphics);
	Gdiplus::Graphics g(&buffer);
	Gdiplus::Graphics* graphics = &g;

	iInstruction->paintTo(graphics);

	iGraphics->DrawImage(&buffer, 0, 0);
}

//---------------------------------------------------------------------------
void __fastcall TfrmPreInstruction::FormDestroy(TObject *Sender)
{
	delete iInstruction;
	delete iGraphics;
}

//---------------------------------------------------------------------------
void __fastcall TfrmPreInstruction::FormClick(TObject *Sender)
{
//	if (iButtonContinue.hits(aX, aY))
//	{
	if (FormState.Contains(fsModal))
		ModalResult = mrOk;
	else
		Close();
//	}
}

//---------------------------------------------------------------------------

