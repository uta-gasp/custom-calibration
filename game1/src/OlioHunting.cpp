//---------------------------------------------------------------------------
#include "OlioHunting.h"
#include "assets_ffap.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
using namespace FireflyAndPoints;

//---------------------------------------------------------------------------
const int KWidth = 1377;
const int KHeight = 768;
const int KResultHeight = 200;

const TiOlioHunting::SiHidingOlio KHidingOlios[] = { // picture ID is olio's ID
	TiOlioHunting::SiHidingOlio(1139.5, 71.5, 	51, 91, 	27, 27),
	TiOlioHunting::SiHidingOlio(1332, 667.5, 	54, 113, 	33, 30),
	TiOlioHunting::SiHidingOlio(592, 376, 			90, 82, 	45, 38),
	TiOlioHunting::SiHidingOlio(1060, 549, 		56, 148, 	24, 45),
	TiOlioHunting::SiHidingOlio(483.5, 366, 		55, 123, 	21, 42),
	TiOlioHunting::SiHidingOlio(1209, 263, 		64, 108, 	29, 30),
	TiOlioHunting::SiHidingOlio(796.5, 226.5, 	79, 117, 	40, 39),
	TiOlioHunting::SiHidingOlio(354.5, 103, 		81, 108, 	26, 40),
	TiOlioHunting::SiHidingOlio(63.5, 450.5, 	79, 133, 	39, 39),
	TiOlioHunting::SiHidingOlio(389.5, 718, 		105, 90, 	69, 16),
	TiOlioHunting::SiHidingOlio(1205, 399, 		72, 116, 	30, 39),
	TiOlioHunting::SiHidingOlio(574, 549, 			38, 84, 	20, 17),
	TiOlioHunting::SiHidingOlio(499, 96, 			44, 58, 	23, 15),
	TiOlioHunting::SiHidingOlio(867, 95.5, 		50, 61, 	21, 27),
	TiOlioHunting::SiHidingOlio(785, 435, 			52, 144, 	24, 41),
	TiOlioHunting::SiHidingOlio(815.5, 586, 		41, 74, 	20, 20),
	TiOlioHunting::SiHidingOlio(164.5, 268, 		83, 142, 	25, 40),
	TiOlioHunting::SiHidingOlio(303, 450.5, 		66, 95, 	37, 49),
	TiOlioHunting::SiHidingOlio(916, 635, 			58, 142, 	32, 39),
	TiOlioHunting::SiHidingOlio(1134.5, 592, 	43, 132, 	24, 38),
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall TiOlioHuntingTimer::TiOlioHuntingTimer(int aTimeout) :
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
__fastcall TiOlioHuntingTimer::~TiOlioHuntingTimer()
{
	delete iTimer;
}

//---------------------------------------------------------------------------
void __fastcall TiOlioHuntingTimer::paintTo(Gdiplus::Graphics* aGraphics)
{
	TiAnimation::paintTo(aGraphics);

	if (!IsVisible)
		return;

	String str = String(int(iMillisecondsLeft / 1000));
	WideString bstr(str);

	Gdiplus::Font font(L"Arial", 26);
	Gdiplus::RectF rect( X - 50, Y - 50, 100, 100);
	Gdiplus::StringFormat format;
	format.SetAlignment(Gdiplus::StringAlignmentCenter);
	format.SetLineAlignment(Gdiplus::StringAlignmentCenter);

	Gdiplus::SolidBrush brush(Gdiplus::Color(255, 255, 255, 255));

	aGraphics->DrawString(bstr.c_bstr(), -1, &font, rect, &format, &brush);
}

//---------------------------------------------------------------------------
void __fastcall TiOlioHuntingTimer::start()
{
	iTimer->Enabled = true;
	fadeIn();
}

//---------------------------------------------------------------------------
void __fastcall TiOlioHuntingTimer::stop()
{
	iTimer->Enabled = false;
	fadeOut();
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TiOlioHuntingTimer::Tick(TObject* aSender)
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
int __fastcall TiOlioHuntingTimer::GetTimeout()
{
	return iMillisecondsLeft / 1000;
}

//---------------------------------------------------------------------------
void __fastcall TiOlioHuntingTimer::SetTimeout(int aValue)
{
	iMillisecondsLeft = aValue * 1000;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall TiOlioHunting::TiOlioHunting(TiAnimationManager* aManager, TiSize aScreenSize) :
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

	iScreenSize = aScreenSize;
	int offsetX = (aScreenSize.Width - KWidth) / 2;
	int offsetY = (aScreenSize.Height - KHeight) / 2;

	for (int i = 0; i < ARRAYSIZE(KHidingOlios); i++)
	{
		TiOlioHunting::SiHidingOlio hidingOlio = KHidingOlios[i];

		TiAnimation* olio = new TiAnimation();
		//olio->Center = TPoint(0, 0);
		olio->addFrames(IDR_HIDDEN_OLIOS + 1 + i, hidingOlio.Width, hidingOlio.Height);
		olio->placeTo(offsetX + hidingOlio.X, offsetY + hidingOlio.Y);
		olio->hide();

		iHidingOlios.add(olio);
		aManager->add(olio);
	}

	int screenCenterX = aScreenSize.Width / 2;
	int screenCenterY = aScreenSize.Height / 2;

	iResultBackground = new TiAnimation();
	iResultBackground->FadingDuration = 400;
	iResultBackground->addFrames(IDR_GAME_BACK, 700, 200);
	iResultBackground->placeTo(screenCenterX, offsetY + KResultHeight / 2);
	iResultBackground->hide();
	aManager->add(iResultBackground);

	iBestScoreLogo1 = new TiAnimation();
	iBestScoreLogo1->addFrames(IDR_GAME_BEST, 128, 128);
	iBestScoreLogo1->placeTo(screenCenterX + 450, offsetY + KResultHeight / 2);
	iBestScoreLogo1->hide();
	aManager->add(iBestScoreLogo1);

	iBestScoreLogo2 = new TiAnimation();
	iBestScoreLogo2->addFrames(IDR_GAME_BEST, 128, 128);
	iBestScoreLogo2->placeTo(screenCenterX - 450, offsetY + KResultHeight / 2);
	iBestScoreLogo2->hide();
	aManager->add(iBestScoreLogo2);

	iInstruction = new TiAnimation(false, true);
	iInstruction->addFrames(IDR_GAME_INSTRUCTION, 1000, 450);
	iInstruction->placeTo(screenCenterX, screenCenterY);
	aManager->add(iInstruction);

	iCountdown = new TiOlioHuntingTimer(iTimeout);
	iCountdown->placeTo(offsetX + 60, offsetY + 60);
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
bool __fastcall TiOlioHunting::GetIsRunning()
{
	return iStartTime != 0;
}

//---------------------------------------------------------------------------
bool __fastcall TiOlioHunting::GetIsInstructionVisible()
{
	return iInstruction->IsVisible;
}

//---------------------------------------------------------------------------
void __fastcall TiOlioHunting::SetTimeout(int aValue)
{
	iTimeout = aValue;
	iCountdown->Timeout = iTimeout;
}

//---------------------------------------------------------------------------
int __fastcall TiOlioHunting::ComputeScore(double aDuration, int aOliosFound)
{
	return int((iTimeout + 1) - aDuration) * 5 + aOliosFound * 20;
}

//---------------------------------------------------------------------------
void __fastcall TiOlioHunting::ComputeAndShowScore()
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
		FOnEvent(this, String().sprintf("verification finished\t%d\t%d\t%s", iOliosFound, iScore,
				iIsBestScore ? "best" : "not best"));

	if (FOnFinished)
		FOnFinished(this);
}

//---------------------------------------------------------------------------
void __fastcall TiOlioHunting::ShowBestScoreLogos(TObject* aSender)
{
	iBestScoreLogo1->fadeIn();
	iBestScoreLogo2->fadeIn();
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TiOlioHunting::showInstruction()
{
	iInstruction->fadeIn();
//	if (FOnEvent)
//		FOnEvent(this, "show game instructions");
}

//---------------------------------------------------------------------------
void __fastcall TiOlioHunting::start(int aOliosToShow)
{
	iInstruction->fadeOut();

	int oliosToShow = min(aOliosToShow, (int)iHidingOlios.Count);
	int visibleCount = 0;
	TStringList* oliosCoords = new TStringList();

	while (visibleCount < oliosToShow)
	{
		int id = Utils::randInRange(0, ARRAYSIZE(KHidingOlios) - 1);
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
		FOnEvent(this, String().sprintf("verification start%s", olios.c_str()));
	}
}

//---------------------------------------------------------------------------
void __fastcall TiOlioHunting::stop(TObject* aSender)
{
	for (int i = 0; i < iHidingOlios.Count; i++)
		iHidingOlios[i]->hide();

	ComputeAndShowScore();
}

//---------------------------------------------------------------------------
void __fastcall TiOlioHunting::click(int aX, int aY)
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
		FOnEvent(this, String().sprintf("verification point\t%d %d\t%d %d\t%.2f",
				nearestOlio->X, nearestOlio->Y, x, y, minDistance ));
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
void __fastcall TiOlioHunting::placePointer(int aGazeX, int aGazeY, int aCorrectionX, int aCorrectionY)
{
	iPointer->placeTo(aGazeX + aCorrectionX, aGazeY + aCorrectionY);
}

//---------------------------------------------------------------------------
void __fastcall TiOlioHunting::paintTo(Gdiplus::Graphics* aGraphics, EiUpdateType aUpdateType)
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
			Gdiplus::RectF rect(
				(iScreenSize.Width - KWidth) / 2,
				(iScreenSize.Height - KHeight) / 2,
				KWidth, KResultHeight
			);
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
