//---------------------------------------------------------------------------
#include "Game.h"
#include "assets.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
const int KWidth = 1377;
const int KResultHeight = 200;

const TiGame::SiHidingOlio KHidingOlios[] = { // picture ID is olio's ID
	TiGame::SiHidingOlio(1139.5, 71.5, 	51, 91, 	27, 27),
	TiGame::SiHidingOlio(1332, 667.5, 	54, 113, 	33, 30),
	TiGame::SiHidingOlio(592, 376, 			90, 82, 	45, 38),
	TiGame::SiHidingOlio(1060, 549, 		56, 148, 	24, 45),
	TiGame::SiHidingOlio(483.5, 366, 		55, 123, 	21, 42),
	TiGame::SiHidingOlio(1209, 263, 		64, 108, 	29, 30),
	TiGame::SiHidingOlio(796.5, 226.5, 	79, 117, 	40, 39),
	TiGame::SiHidingOlio(354.5, 103, 		81, 108, 	26, 40),
	TiGame::SiHidingOlio(63.5, 450.5, 	79, 133, 	39, 39),
	TiGame::SiHidingOlio(389.5, 718, 		105, 90, 	69, 16),
	TiGame::SiHidingOlio(1205, 399, 		72, 116, 	30, 39),
	TiGame::SiHidingOlio(574, 549, 			38, 84, 	20, 17),
	TiGame::SiHidingOlio(499, 96, 			44, 58, 	23, 15),
	TiGame::SiHidingOlio(867, 95.5, 		50, 61, 	21, 27),
	TiGame::SiHidingOlio(785, 435, 			52, 144, 	24, 41),
	TiGame::SiHidingOlio(815.5, 586, 		41, 74, 	20, 20),
	TiGame::SiHidingOlio(164.5, 268, 		83, 142, 	25, 40),
	TiGame::SiHidingOlio(303, 450.5, 		66, 95, 	37, 49),
	TiGame::SiHidingOlio(916, 635, 			58, 142, 	32, 39),
	TiGame::SiHidingOlio(1134.5, 592, 	43, 132, 	24, 38),
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall TiGameTimer::TiGameTimer(int aTimeout) :
	TiAnimation(false, false),
	FOnStop(NULL)
{
	Timeout = aTimeout;

	iTimer = new TTimer(NULL);
	iTimer->Enabled = false;
	iTimer->Interval = 200;
	iTimer->OnTimer = Tick;

	addFrames(IDR_GAME_TIMER, 100, 100);
}

//---------------------------------------------------------------------------
__fastcall TiGameTimer::~TiGameTimer()
{
	delete iTimer;
}

//---------------------------------------------------------------------------
void __fastcall TiGameTimer::paintTo(Gdiplus::Graphics* aGraphics)
{
	TiAnimation::paintTo(aGraphics);

	if (!IsVisible)
		return;

	String str = String(int(iMillisecondsLeft / 1000));
	WideString bstr(str);

	Gdiplus::Font font(L"Arial", 26);
	Gdiplus::RectF rect(10, 10, 100, 100);
	Gdiplus::StringFormat format;
	format.SetAlignment(Gdiplus::StringAlignmentCenter);
	format.SetLineAlignment(Gdiplus::StringAlignmentCenter);

	Gdiplus::SolidBrush brush(Gdiplus::Color(255, 255, 255, 255));

	aGraphics->DrawString(bstr.c_bstr(), -1, &font, rect, &format, &brush);
}

//---------------------------------------------------------------------------
void __fastcall TiGameTimer::start()
{
	iTimer->Enabled = true;
	fadeIn();
}

//---------------------------------------------------------------------------
void __fastcall TiGameTimer::stop()
{
	iTimer->Enabled = false;
	fadeOut();
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TiGameTimer::Tick(TObject* aSender)
{
	int secondsBefore = iMillisecondsLeft / 1000;
	iMillisecondsLeft -= iTimer->Interval;
	if (iMillisecondsLeft <= 0.0)
	{
		iTimer->Enabled = false;
		if (FOnStop)
			FOnStop(this);

		return;
	}

	int secondsAfter = iMillisecondsLeft / 1000;
	if (secondsAfter < secondsBefore)
		invalidate();

}

//---------------------------------------------------------------------------
int __fastcall TiGameTimer::GetTimeout()
{
	return iMillisecondsLeft / 1000;
}

//---------------------------------------------------------------------------
void __fastcall TiGameTimer::SetTimeout(int aValue)
{
	iMillisecondsLeft = aValue * 1000;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall TiGame::TiGame(TiAnimationManager* aManager, TiSize aScreenSize) :
		TObject(),
		iDuration(0.0),
		iOliosFound(0),
		iBestScore(0),
		iIsBestScore(false),
		iStartTime(0),
		iShowBestScoreLogo(true),
		iTimeout(30),
		iTimeoutRef(NULL),
		FOnEvent(NULL),
		FOnSelect(NULL),
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

	iInstruction = new TiAnimation(false, true);
	iInstruction->addFrames(IDR_GAME_INSTRUCTION, 1000, 450);
	iInstruction->placeTo(aScreenSize.Width / 2, aScreenSize.Height / 2);
	aManager->add(iInstruction);

	iCountdown = new TiGameTimer(iTimeout);
	iCountdown->placeTo(60, 60);
	iCountdown->OnStop = stop;
	aManager->add(iCountdown);

	iPointer = new TiAnimation(false, false);
	iPointer->addFrames(IDR_GAME_POINTER, 48, 48);
	aManager->add(iPointer);

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
bool __fastcall TiGame::GetIsInstructionVisible()
{
	return iInstruction->IsVisible;
}

//---------------------------------------------------------------------------
void __fastcall TiGame::SetTimeout(int aValue)
{
	iTimeout = aValue;
	iCountdown->Timeout = iTimeout;
}

//---------------------------------------------------------------------------
int __fastcall TiGame::ComputeScore(double aDuration, int aOliosFound)
{
	return int((iTimeout + 1) - aDuration) * 5 + aOliosFound * 20;
}

//---------------------------------------------------------------------------
void __fastcall TiGame::ComputeAndShowScore()
{
	if (iTimeoutRef)
		iTimeoutRef->kill();

	iCountdown->stop();
	iPointer->fadeOut();

	LARGE_INTEGER li;
	::QueryPerformanceCounter(&li);

	iDuration = double(li.QuadPart - iStartTime)/iSysTimerFreq;

	iScore = ComputeScore(iDuration, iOliosFound);

	if (iScore > iBestScore)
	{
		iBestScore = iScore;
		iBestScoreDate = TDateTime::CurrentDateTime().DateTimeString();
		iIsBestScore = true;
		TiTimeout::run(1000, ShowBestScoreLogos);
	}

	iResultBackground->fadeIn();
	iStartTime = 0;

	if (FOnEvent)
		FOnEvent(this, String().sprintf("game stop\t%d\t%d\t%s", iOliosFound, iScore,
				iIsBestScore ? "best" : "not best"));

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
void __fastcall TiGame::showInstruction()
{
	iInstruction->fadeIn();
	if (FOnEvent)
		FOnEvent(this, "show game instructions");
}

//---------------------------------------------------------------------------
void __fastcall TiGame::start(int aOliosToShow)
{
	iInstruction->fadeOut();

	int oliosToShow = min(aOliosToShow, (int)iHidingOlios.Count);
	int visibleCount = 0;
	TStringList* oliosCoords = new TStringList();

	while (visibleCount < oliosToShow)
	{
		int id = randInRange(0, ARRAYSIZE(KHidingOlios) - 1);
		TiAnimation* olio = iHidingOlios[id];
		olio->show();
		oliosCoords->Add(String().sprintf("%d %d", olio->X, olio->Y));

		visibleCount = 0;
		for (int i = 0; i < iHidingOlios.Count; i++)
			visibleCount += iHidingOlios[i]->IsVisible ? 1 : 0;
	}

	LARGE_INTEGER li;
	::QueryPerformanceCounter(&li);

	iStartTime = li.QuadPart;
	iOliosFound = 0;

	iOliosToFind = oliosToShow;

	iCountdown->start();
	iPointer->fadeIn();

	if (FOnEvent)
	{
		String olios = "";
		for (int i = 0; i < oliosCoords->Count; i++)
		{
			olios = String().sprintf("%s\t%s", olios.c_str(), oliosCoords->Strings[i].c_str());
		}
		FOnEvent(this, String().sprintf("game start\t%s", olios.c_str()));
	}
}

//---------------------------------------------------------------------------
void __fastcall TiGame::stop(TObject* aSender)
{
	for (int i = 0; i < iHidingOlios.Count; i++)
		iHidingOlios[i]->hide();

	ComputeAndShowScore();
}

//---------------------------------------------------------------------------
void __fastcall TiGame::click(int aX, int aY)
{
	int x = aX < 0 ? iPointer->X : aX;
	int y = aY < 0 ? iPointer->Y : aY;

	bool finished = false;
	double minDistance = MaxInt;

	TiAnimation* nearestOlio = NULL;
	int nearestOlioIndex = -1;

	for (int i = 0; i < iHidingOlios.Count; i++)
	{
		TiAnimation* olio = iHidingOlios[i];
		double distance = olio->distanceTo(x, y);
		if (distance == 0.0)
		{
			minDistance = 0;
			nearestOlio = olio;
			nearestOlioIndex = i;

			olio->hide();
			iOliosFound++;

			int visibleCount = 0;
			for (int i = 0; i < iHidingOlios.Count; i++)
				visibleCount += iHidingOlios[i]->IsVisible ? 1 : 0;

			finished = visibleCount == 0;
			break;
		}
		else if (distance < minDistance)
		{
			minDistance = distance;
			nearestOlio = olio;
			nearestOlioIndex = i;
		}
	}

	if (FOnEvent)
	{
		if (minDistance > 0)
			FOnEvent(this, String().sprintf("creature missed\t%d %d\t%d %d\t%.2f",
					nearestOlio->X, nearestOlio->Y, x, y, minDistance ));
		else
			FOnEvent(this, String().sprintf("creature captured\t%d %d\t%d %d",
					nearestOlio->X, nearestOlio->Y, x, y ));
	}

	SiHidingOlio olioData = KHidingOlios[nearestOlioIndex];
	TPoint eyes = nearestOlio->clientToScreen(olioData.GazeSpotX, olioData.GazeSpotY);
	if (FOnSelect)
		FOnSelect(this, eyes.x, eyes.y);
//		FOnSelect(this, nearestOlio->X, nearestOlio->Y);

	if (finished)
		ComputeAndShowScore();
}

//---------------------------------------------------------------------------
void __fastcall TiGame::placePointer(int aGazeX, int aGazeY, int aCorrectionX, int aCorrectionY)
{
	iPointer->placeTo(aGazeX + aCorrectionX, aGazeY + aCorrectionY);
}

//---------------------------------------------------------------------------
void __fastcall TiGame::paintTo(Gdiplus::Graphics* aGraphics, EiUpdateType aUpdateType)
{
	if (aUpdateType & updStatic)
	{
		for (int i = 0; i < iHidingOlios.Count; i++)
			iHidingOlios[i]->paintTo(aGraphics);

		if (!IsRunning && iDuration > 0.0)
		{
			String str = (iOliosFound == iOliosToFind) ? "Valmis!" : "Peli loppui.";
			if (iScore)
				str = str + "\nSaamasi pisteet: " + String(iScore);

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

	if (aUpdateType & updNonStatic)
	{
		iInstruction->paintTo(aGraphics);
		iCountdown->paintTo(aGraphics);
		iPointer->paintTo(aGraphics);
	}
}
