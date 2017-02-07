//---------------------------------------------------------------------------
#include "Game.h"
#include "assets.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
const int KWidth = 1377;
const int KResultHeight = 200;

const TiGame::SiHidingOlio KHidingOlios[] = { // picture ID is olio's ID
	TiGame::SiHidingOlio(1139.5, 71.5, 51, 91),
	TiGame::SiHidingOlio(1332, 667.5, 54, 113),
	TiGame::SiHidingOlio(592, 376, 90, 82),
	TiGame::SiHidingOlio(1060, 549, 56, 148),
	TiGame::SiHidingOlio(483.5, 366, 55, 123),
	TiGame::SiHidingOlio(1209, 263, 64, 108),
	TiGame::SiHidingOlio(796.5, 226.5, 79, 117),
	TiGame::SiHidingOlio(354.5, 103, 81, 108),
	TiGame::SiHidingOlio(63.5, 450.5, 79, 133),
	TiGame::SiHidingOlio(389.5, 718, 105, 90),
	TiGame::SiHidingOlio(1205, 399, 72, 116),
	TiGame::SiHidingOlio(574, 549, 38, 84),
	TiGame::SiHidingOlio(499, 96, 44, 58),
	TiGame::SiHidingOlio(867, 95.5, 50, 61),
	TiGame::SiHidingOlio(785, 435, 52, 144),
	TiGame::SiHidingOlio(815.5, 586, 41, 74),
	TiGame::SiHidingOlio(164.5, 268, 83, 142),
	TiGame::SiHidingOlio(303, 450.5, 66, 95),
	TiGame::SiHidingOlio(916, 635, 58, 142),
	TiGame::SiHidingOlio(1134.5, 592, 43, 132),
};

//---------------------------------------------------------------------------
__fastcall TiGame::TiGame(TiAnimationManager* aManager) :
	TObject(),
	iDuration(0.0),
	iMonstersFound(0),
	iBestScore(0),
	iIsBestScore(false),
	iStartTime(0),
	iShowBestScoreLogo(false),
	iTimeout(30),
	iTimeoutRef(NULL),
	FOnFinished(NULL)
{
	iHidingOlios.DeleteContent = false;

	for (int i = 0; i < ARRAYSIZE(KHidingOlios); i++)
	{
		TiGame::SiHidingOlio hidingOlio = KHidingOlios[i];

		TiAnimation* olio = new TiAnimation();
		//olio->Center = TPoint(0, 0);
		olio->addFrames(IDR_HIDDEN_OLIOS + 1 + i, hidingOlio.Width, hidingOlio.Height);
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

	iBestScoreLogo1 = new TiAnimation();
	iBestScoreLogo1->addFrames(IDR_GAME_BEST, 128, 128);
	iBestScoreLogo1->placeTo(KWidth - 200, KResultHeight / 2);
	iBestScoreLogo1->hide();
	aManager->add(iBestScoreLogo1);

	iBestScoreLogo2 = new TiAnimation();
	iBestScoreLogo2->addFrames(IDR_GAME_BEST, 128, 128);
	iBestScoreLogo2->placeTo(200, KResultHeight / 2);
	iBestScoreLogo2->hide();
	aManager->add(iBestScoreLogo2);

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
int __fastcall TiGame::ComputeScore(double aDuration, int aMonstersFound)
{
	return int((iTimeout + 1) - aDuration) * 5 + aMonstersFound * 20;
}

//---------------------------------------------------------------------------
void __fastcall TiGame::ComputeAndShowScore()
{
	if (iTimeoutRef)
		iTimeoutRef->kill();
		
	LARGE_INTEGER li;
	::QueryPerformanceCounter(&li);

	iDuration = double(li.QuadPart - iStartTime)/iSysTimerFreq;

	iScore = ComputeScore(iDuration, iMonstersFound);

	if (iScore > iBestScore)
	{
		iBestScore = iScore;
		iBestScoreDate = TDateTime::CurrentDateTime().DateTimeString();
		iIsBestScore = true;
		TiTimeout::run(1000, ShowBestScoreLogos);
	}

	iResultBackground->fadeIn();
	iStartTime = 0;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TiGame::StopOnTimeout(TObject* aSender)
{
	for (int i = 0; i < iHidingOlios.Count; i++)
		iHidingOlios[i]->hide();

	ComputeAndShowScore();

	if (FOnFinished)
		FOnFinished(this);
}

//---------------------------------------------------------------------------
void __fastcall TiGame::ShowBestScoreLogos(TObject* aSender)
{
	iBestScoreLogo1->fadeIn();
	iBestScoreLogo2->fadeIn();
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TiGame::start(int aOliosToShow)
{
	int oliosToShow = min(aOliosToShow, (int)iHidingOlios.Count);
	int visibleCount = 0;

	while (visibleCount < oliosToShow)
	{
		int id = randInRange(0, ARRAYSIZE(KHidingOlios) - 1);
		iHidingOlios[id]->show();

		visibleCount = 0;
		for (int i = 0; i < iHidingOlios.Count; i++)
			visibleCount += iHidingOlios[i]->IsVisible ? 1 : 0;
	}

	LARGE_INTEGER li;
	::QueryPerformanceCounter(&li);

	iStartTime = li.QuadPart;
	iMonstersFound = 0;

	TiTimeout::run(1000 * iTimeout, StopOnTimeout, &iTimeoutRef);
}

//---------------------------------------------------------------------------
void __fastcall TiGame::click(int aX, int aY)
{
	bool finished = false;
	for (int i = 0; i < iHidingOlios.Count; i++)
	{
		TiAnimation* olio = iHidingOlios[i];
		if (olio->hitTest(aX, aY))
		{
			olio->hide();
			iMonstersFound++;

			int visibleCount = 0;
			for (int i = 0; i < iHidingOlios.Count; i++)
				visibleCount += iHidingOlios[i]->IsVisible ? 1 : 0;

			finished = visibleCount == 0;
			break;
		}
	}

	if (finished)
	{
		ComputeAndShowScore();

		if (FOnFinished)
			FOnFinished(this);
	}
}

//---------------------------------------------------------------------------
void __fastcall TiGame::paintTo(Gdiplus::Graphics* aGraphics)
{
	for (int i = 0; i < iHidingOlios.Count; i++)
		iHidingOlios[i]->paintTo(aGraphics);

	if (!IsRunning && iScore > 0)
	{
		String str = String("Valmis!\nTuloksesi on ") + String(iScore) + " pistetta";
		WideString bstr(str);

		Gdiplus::Font font(L"Arial", 26);
		Gdiplus::RectF rect(0, 0, KWidth, KResultHeight);
		Gdiplus::StringFormat format;
		format.SetAlignment(Gdiplus::StringAlignmentCenter);
		format.SetLineAlignment(Gdiplus::StringAlignmentCenter);

		Gdiplus::SolidBrush brush(Gdiplus::Color(255, 255, 255, 255));

		iResultBackground->paintTo(aGraphics);
		aGraphics->DrawString(bstr.c_bstr(), -1, &font, rect, &format, &brush);

		if (iShowBestScoreLogo)
		{
			iBestScoreLogo1->paintTo(aGraphics);
			iBestScoreLogo2->paintTo(aGraphics);
		}
	}
}
