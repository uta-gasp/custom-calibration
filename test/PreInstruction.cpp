//---------------------------------------------------------------------------
#include "PreInstruction.h"
#include "assets.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

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
	iInstruction->AnimationIndex = (int)aValue;
	iHasInstruction = true;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TfrmPreInstruction::FormCreate(TObject *Sender)
{
	SetBounds(0, 0, Screen->Width, Screen->Height);

	iGraphics = new Gdiplus::Graphics(Handle, false);

	iInstruction = new TiAnimation(true);
	iInstruction->addFrames(IDR_PREINSTRUCTION_START, 950, 170);
	iInstruction->addFrames(IDR_PREINSTRUCTION_PAUSE, 950, 170);
	iInstruction->addFrames(IDR_PREINSTRUCTION_FINISHED, 950, 170);
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

