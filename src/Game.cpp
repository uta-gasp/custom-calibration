//---------------------------------------------------------------------------
#include "Game.h"
#include "utils.h"
#include "assets.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
const TiGame::SiOlioRect KOlioRects[] = {
	TiGame::SiOlioRect(50, 50, 100, 100),
	TiGame::SiOlioRect(50, 150, 100, 100),
	TiGame::SiOlioRect(50, 250, 100, 100),
	TiGame::SiOlioRect(50, 350, 100, 100),
	TiGame::SiOlioRect(50, 450, 100, 100),
	TiGame::SiOlioRect(50, 550, 100, 100),
	TiGame::SiOlioRect(50, 650, 100, 100),
	TiGame::SiOlioRect(250, 50, 100, 100),
	TiGame::SiOlioRect(250, 150, 100, 100),
	TiGame::SiOlioRect(250, 250, 100, 100),
	TiGame::SiOlioRect(250, 350, 100, 100),
	TiGame::SiOlioRect(250, 450, 100, 100),
	TiGame::SiOlioRect(250, 550, 100, 100),
	TiGame::SiOlioRect(250, 650, 100, 100),
	TiGame::SiOlioRect(450, 50, 100, 100),
	TiGame::SiOlioRect(450, 150, 100, 100),
	TiGame::SiOlioRect(450, 250, 100, 100),
	TiGame::SiOlioRect(450, 350, 100, 100),
	TiGame::SiOlioRect(450, 450, 100, 100),
	TiGame::SiOlioRect(450, 550, 100, 100),
};

//---------------------------------------------------------------------------
__fastcall TiGame::TiGame(TiAnimationManager* aManager) :
	iDuration(0.0)
{
	iHidingOlios.DeleteContent = false;
	
	for (int i = 0; i < ARRAYSIZE(KOlioRects); i++)
	{
		TiGame::SiOlioRect olioRect = KOlioRects[i];

		TiAnimation* olio = new TiAnimation();
		olio->Center = TPoint(0, 0);
		olio->addFrames(IDR_GAME_OLIOS + i, olioRect.Width, olioRect.Height);
		olio->placeTo(olioRect.X, olioRect.Y);
		olio->hide();

		iHidingOlios.add(olio);
		aManager->add(olio);
	}

	LARGE_INTEGER fr;
	::QueryPerformanceFrequency(&fr);

	iSysTimerFreq = fr.LowPart;
}

//---------------------------------------------------------------------------
void __fastcall TiGame::start(int aOliosToShow)
{
	int visibleCount = 0;

	while (visibleCount < aOliosToShow)
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
	}

	return finished;
}

//---------------------------------------------------------------------------
void __fastcall TiGame::paintTo(Gdiplus::Graphics* aGraphics)
{
	for (int i = 0; i < iHidingOlios.Count; i++)
		iHidingOlios[i]->paintTo(aGraphics);

	if (iDuration > 0.0)
	{
		WideString str(String("Time: ") + String(iDuration) + " seconds");

		Gdiplus::Font font(L"Arial", 26);
		Gdiplus::RectF rect(0, 0, 1377, 768);
		Gdiplus::StringFormat format;
		format.SetAlignment(Gdiplus::StringAlignmentCenter);
		format.SetLineAlignment(Gdiplus::StringAlignmentCenter);

		Gdiplus::SolidBrush brush(Gdiplus::Color(255, 255, 255, 255));

		aGraphics->DrawString(str.c_bstr(), -1, &font, rect, &format, &brush);
		//aGraphics->DrawString(L"", -1, &font, rect, &format, &brush);
	}
}
