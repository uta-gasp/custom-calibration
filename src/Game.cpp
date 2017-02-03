//---------------------------------------------------------------------------
#include "Game.h"
#include "utils.h"
#include "assets.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
const int KWidth = 1377;
const int KResultHeight = 200;

const TiGame::SiHidingOlio KHidingOlios[] = { // picture ID is olio's ID
	TiGame::SiHidingOlio(50, 50, 100, 100),
	TiGame::SiHidingOlio(50, 150, 100, 100),
	TiGame::SiHidingOlio(50, 250, 100, 100),
	TiGame::SiHidingOlio(50, 350, 100, 100),
	TiGame::SiHidingOlio(50, 450, 100, 100),
	TiGame::SiHidingOlio(50, 550, 100, 100),
	TiGame::SiHidingOlio(50, 650, 100, 100),
	TiGame::SiHidingOlio(250, 50, 100, 100),
	TiGame::SiHidingOlio(250, 150, 100, 100),
	TiGame::SiHidingOlio(250, 250, 100, 100),
	TiGame::SiHidingOlio(250, 350, 100, 100),
	TiGame::SiHidingOlio(250, 450, 100, 100),
	TiGame::SiHidingOlio(250, 550, 100, 100),
	TiGame::SiHidingOlio(250, 650, 100, 100),
	TiGame::SiHidingOlio(450, 50, 100, 100),
	TiGame::SiHidingOlio(450, 150, 100, 100),
	TiGame::SiHidingOlio(450, 250, 100, 100),
	TiGame::SiHidingOlio(450, 350, 100, 100),
	TiGame::SiHidingOlio(450, 450, 100, 100),
	TiGame::SiHidingOlio(450, 550, 100, 100),
};

//---------------------------------------------------------------------------
__fastcall TiGame::TiGame(TiAnimationManager* aManager) :
	iDuration(0.0),
	iBestTime(10000000),
	iIsBestTime(false),
	iStartTime(0),
	iShowBestTimeLogo(false)
{
	iHidingOlios.DeleteContent = false;
	
	for (int i = 0; i < ARRAYSIZE(KHidingOlios); i++)
	{
		TiGame::SiHidingOlio hidingOlio = KHidingOlios[i];

		TiAnimation* olio = new TiAnimation();
		olio->Center = TPoint(0, 0);
		olio->addFrames(IDR_GAME_OLIOS + i, hidingOlio.Width, hidingOlio.Height);
		olio->placeTo(hidingOlio.X, hidingOlio.Y);
		olio->hide();

		iHidingOlios.add(olio);
		aManager->add(olio);
	}

	iResultBackground = new TiAnimation();
	iResultBackground->FadingDuration = 400;
	iResultBackground->addFrames(IDR_GAME_BACK, 700, 200);
	iResultBackground->placeTo(KWidth / 2, KResultHeight / 2);
	iResultBackground->hide();
	aManager->add(iResultBackground);

	iBestTimeLogo1 = new TiAnimation();
	iBestTimeLogo1->addFrames(IDR_GAME_BEST, 128, 128);
	iBestTimeLogo1->placeTo(KWidth - 200, KResultHeight / 2);
	iBestTimeLogo1->hide();
	aManager->add(iBestTimeLogo1);

	iBestTimeLogo2 = new TiAnimation();
	iBestTimeLogo2->addFrames(IDR_GAME_BEST, 128, 128);
	iBestTimeLogo2->placeTo(200, KResultHeight / 2);
	iBestTimeLogo2->hide();
	aManager->add(iBestTimeLogo2);

	LARGE_INTEGER fr;
	::QueryPerformanceFrequency(&fr);

	iSysTimerFreq = fr.LowPart;
}

//---------------------------------------------------------------------------
bool __fastcall TiGame::GetIsRunning()
{
	return iStartTime != 0;
}

//---------------------------------------------------------------------------
void __fastcall TiGame::onBestTimeLogoShow(TObject* aSender)
{
	iBestTimeLogo1->fadeIn();
	iBestTimeLogo2->fadeIn();
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TiGame::start(int aOliosToShow)
{
	int oliosToShow = min(aOliosToShow, (int)iHidingOlios.Count);
	int visibleCount = 0;

	while (visibleCount < oliosToShow)
	{
		int id = randInRange(0, 19);
		iHidingOlios[id]->show();

		visibleCount = 0;
		for (int i = 0; i < iHidingOlios.Count; i++)
			visibleCount += iHidingOlios[i]->IsVisible ? 1 : 0;
	}

	LARGE_INTEGER li;
	::QueryPerformanceCounter(&li);
	iStartTime = li.QuadPart;
}

//---------------------------------------------------------------------------
bool __fastcall TiGame::click(int aX, int aY)
{
	bool finished = false;
	for (int i = 0; i < iHidingOlios.Count; i++)
	{
		TiAnimation* olio = iHidingOlios[i];
		if (olio->hitTest(aX, aY))
		{
			olio->hide();

			int visibleCount = 0;
			for (int i = 0; i < iHidingOlios.Count; i++)
				visibleCount += iHidingOlios[i]->IsVisible ? 1 : 0;

			finished = visibleCount == 0;
		}
	}

	if (finished)
	{
		LARGE_INTEGER li;
		::QueryPerformanceCounter(&li);

		iDuration = double(li.QuadPart - iStartTime)/iSysTimerFreq;

		if (iDuration < iBestTime)
		{
			iBestTime = iDuration;
			iBestTimeDate = TDateTime::CurrentDateTime().DateTimeString();
			iIsBestTime = true;
			TiTimeout::run(1000, onBestTimeLogoShow);
		}

		iResultBackground->fadeIn();
		iStartTime = 0;
	}

	return finished;
}

//---------------------------------------------------------------------------
void __fastcall TiGame::paintTo(Gdiplus::Graphics* aGraphics)
{
	for (int i = 0; i < iHidingOlios.Count; i++)
		iHidingOlios[i]->paintTo(aGraphics);

	if (!IsRunning && iDuration > 0.0)
	{
		String str = String("Valmis!\nTuloksesi on ") + String((int)iDuration) + " sekuntia";
		WideString bstr(str);

		Gdiplus::Font font(L"Arial", 26);
		Gdiplus::RectF rect(0, 0, KWidth, KResultHeight);
		Gdiplus::StringFormat format;
		format.SetAlignment(Gdiplus::StringAlignmentCenter);
		format.SetLineAlignment(Gdiplus::StringAlignmentCenter);

		Gdiplus::SolidBrush brush(Gdiplus::Color(255, 255, 255, 255));

		iResultBackground->paintTo(aGraphics);
		aGraphics->DrawString(bstr.c_bstr(), -1, &font, rect, &format, &brush);

		if (iShowBestTimeLogo)
		{
			iBestTimeLogo1->paintTo(aGraphics);
			iBestTimeLogo2->paintTo(aGraphics);
		}
	}
}
