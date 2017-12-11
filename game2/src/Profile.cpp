//---------------------------------------------------------------------------
#include "Profile.h"

//---------------------------------------------------------------------------
#include <stdlib.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
using namespace ProfiledGame;

//---------------------------------------------------------------------------
const double KSuccessThreshold = 0.8;

const int KScores[] = {10, 7, 5, 2};
const int KCoins[] 	= {11, 7, 4, 1};

const int KSpeedExcellent = 3000;
const int KSpeedGood 			= 5000;
const int KSpeedOK 				= 8000;

const int KLevelScoreMax	= 350;

// Getting a bonus: thresholds
const double KBonusScoreThreshold = 0.80;	// % of max possible
const double KBonusFailedThreshold = 1;		// max number of failed targets

//---------------------------------------------------------------------------
TiProfile::TiProfile(TiAnimationManager* aManager, TiSize aScreenSize, TiSize aViewport) :
		iManager(aManager),
		iScreenSize(aScreenSize),
		iViewport(aViewport),
		iStatus(psNoName),
		iAvatar(NULL),
		iSkinColorID(0),
		iHairColorID(0),
		iEyesColorID(0),
		iShirtColorID(0),
		iLevel(0),
		iScore(0),
		iSaldo(0),
		iBonus(0),
		iSelectedPrizes(0),
		iIsGainedNewLevel(false),
		iGameScore(0),
		iGameCoins(0),
		iGameBonus(0)
{
	iSavedData = new TiXMLDataList(true);
	iTargetPoints = new TiTargetPoints(true);
}

//---------------------------------------------------------------------------
TiProfile::~TiProfile()
{
	delete iSavedData;
	delete iTargetPoints;

	if (iAvatar)
		delete iAvatar;
}

//---------------------------------------------------------------------------
void __fastcall TiProfile::load(TiXML_INI* aStorage)
{
	TiXML_Strings* profileNames = new TiXML_Strings(true);
	aStorage->getAllNodes(L"Profiles", profileNames);

	if (aStorage->openNode(WideString(L"Profiles"), false))
	{
		for (int i = 0; i < profileNames->Count; i++)
		{
			WideString name = profileNames->get(i)->Value;

			SiXMLData* data = new SiXMLData();
			data->Name = name;
			data->Avatar = aStorage->getValue(name, L"Avatar", 0l);
			data->Level = aStorage->getValue(name, L"Level", 0l);
			data->Score = aStorage->getValue(name, L"Score", 0l);
			data->Saldo = aStorage->getValue(name, L"Saldo", 0l);
			data->Bonus = aStorage->getValue(name, L"Bonus", 0l);
			data->SelectedPrizes = aStorage->getValue(name, L"SelectedPrizes", 0l);
			data->Succeeded = aStorage->getValue(name, L"Succeeded", 0l);
			data->Failured = aStorage->getValue(name, L"Failured", 0l);
			data->Duration = aStorage->getValue(name, L"Duration", 0l);

			iSavedData->add(data);
		}

		aStorage->closeNode();
	}

	delete profileNames;
}

//---------------------------------------------------------------------------
void __fastcall TiProfile::save(TiXML_INI* aStorage)
{
	// create/edit the node
	SiXMLData* data = NULL;
	for (int i = 0; i < iSavedData->Count; i++)
	{
		SiXMLData* d = iSavedData->get(i);
		if (d->Name == iName)
		{
			data = d;
			break;
		}
	}

	if (!data)
	{
		data = new SiXMLData();
		iSavedData->add(data);
	}

	long avatar = iSkinColorID +
			(iHairColorID << 4) +
			(iEyesColorID << 8) +
			(iShirtColorID << 12);

	if (aStorage->openNode(WideString("Profiles"), true))
	{
		aStorage->putValue(iName, L"Avatar", avatar);
		aStorage->putValue(iName, L"Level", (long)iLevel);
		aStorage->putValue(iName, L"Score", (long)iScore);
		aStorage->putValue(iName, L"Saldo", (long)iSaldo);
		aStorage->putValue(iName, L"Bonus", (long)iBonus);
		aStorage->putValue(iName, L"SelectedPrizes", (long)iSelectedPrizes);
		aStorage->putValue(iName, L"Succeeded", long(data->Succeeded + iGameResults.Success));
		aStorage->putValue(iName, L"Failured", long(data->Failured + iGameResults.Failure));
		aStorage->putValue(iName, L"Duration", long(data->Duration + iGameResults.Duration));

		aStorage->closeNode();
	}
}

//---------------------------------------------------------------------------
void __fastcall TiProfile::addPoint(SiTargetPoint* aPoint)
{
	iTargetPoints->add(aPoint);

	iGameResults.Duration += aPoint->Duration;
	if (aPoint->Success)
		iGameResults.Success++;
	else
		iGameResults.Failure++;
};

//---------------------------------------------------------------------------
void __fastcall TiProfile::updateScore()
{
	int score = 0;
	int coins = 0;

	for (int i = 0; i < iTargetPoints->Count; i++)
	{
		SiTargetPoint* pt = iTargetPoints->get(i);
		if (!pt->Success)
			continue;

		int pointArchivementIndex;
		if (pt->Duration < KSpeedExcellent)
			pointArchivementIndex = 0;
		else if (pt->Duration < KSpeedGood)
			pointArchivementIndex = 1;
		else if (pt->Duration < KSpeedOK)
			pointArchivementIndex = 2;
		else
			pointArchivementIndex = 3;

		int bonus = 0;
		if (pt->Speed == TiCalibPoints::adModerate)
			bonus = 1;
		else if (pt->Speed == TiCalibPoints::adFast)
			bonus = 2;

		score += KScores[pointArchivementIndex] + bonus;
		coins += KCoins[pointArchivementIndex] + bonus;
	}

	if (!IsSucceeded)
	{
		coins = coins / 2;
	}
	if (GetBonusCount(iBonus) < IDR_PRIZE_COUNT)
	{
		int maxPossibleScore = (KScores[0] + 2) * iTargetPoints->Count;

		if (score >= (KBonusScoreThreshold * maxPossibleScore) &&
				iGameResults.Failure <= KBonusFailedThreshold)
		{
			iGameBonus = GetRandomBonus();
			if (iGameBonus)
				iBonus = iBonus | (1 << (iGameBonus - 1));
		}
	}

	iGameScore = score;
	iGameCoins = coins;

	iScore = min(9999, iScore + score);
	iSaldo = min(9999, iSaldo + coins);

	int nextLevelThreshold = (iLevel + 1) * LevelScoreMax;
	if (iScore > nextLevelThreshold)
	{
		iIsGainedNewLevel = true;
		iLevel++;
	}

	if (iIsGainedNewLevel && !iBonus)
	{
		iGameBonus = GetRandomBonus();
		if (iGameBonus)
			iBonus = iBonus | (1 << (iGameBonus - 1));
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TiProfile::SetName(WideString aValue)
{
	iName = aValue;

	CreateAvatar();

	iStatus = psNew;
	for (int i = 0; i < iSavedData->Count; i++)
	{
		SiXMLData* saved = iSavedData->get(i);
		if (saved->Name == iName)
		{
			iStatus = psLogged;
			SetSkinColorID(saved->Avatar & 0x0000000F);
			SetHairColorID((saved->Avatar & 0x000000F0) >> 4);
			SetEyesColorID((saved->Avatar & 0x00000F00) >> 8);
			SetShirtColorID((saved->Avatar & 0x0000F000) >> 12);
			iLevel = saved->Level;
			iScore = saved->Score;
			iSaldo = saved->Saldo;
			iBonus = saved->Bonus;
			iSelectedPrizes = saved->SelectedPrizes;

			iAvatar->setPrizes(iSelectedPrizes);

			break;
		}
	}

	// KOSTYL
	if (!iScore && rand() < RAND_MAX/2) {
		iLevel = 1;
		iScore = LevelScoreMax;
	}
}

//---------------------------------------------------------------------------
void __fastcall TiProfile::SetSkinColorID(int aValue)
{
	iSkinColorID = aValue;

	if (iAvatar)
		iAvatar->setPartType(TiAvatar::apBody, iSkinColorID);
}

//---------------------------------------------------------------------------
void __fastcall TiProfile::SetHairColorID(int aValue)
{
	iHairColorID = aValue;
	if (iAvatar)
		iAvatar->setPartType(TiAvatar::apHair, iHairColorID);
}

//---------------------------------------------------------------------------
void __fastcall TiProfile::SetEyesColorID(int aValue)
{
	iEyesColorID = aValue;
	if (iAvatar)
		iAvatar->setPartType(TiAvatar::apEyes, iEyesColorID);
}

//---------------------------------------------------------------------------
void __fastcall TiProfile::SetShirtColorID(int aValue)
{
	iShirtColorID = aValue;
	if (iAvatar)
		iAvatar->setPartType(TiAvatar::apShirt, iShirtColorID);
}

//---------------------------------------------------------------------------
int __fastcall TiProfile::GetLevelScore()
{
	return (iScore - iLevel * LevelScoreMax);
}

//---------------------------------------------------------------------------
int __fastcall TiProfile::GetLevelScoreMax()
{
	return KLevelScoreMax;
}

//---------------------------------------------------------------------------
bool __fastcall TiProfile::GetIsSucceeded()
{
	return (iGameResults.Success + iGameResults.Failure) ? (double(iGameResults.Success) / (iGameResults.Success + iGameResults.Failure)) > KSuccessThreshold : false;
}

//---------------------------------------------------------------------------
void __fastcall TiProfile::SetSelectedPrizes(int aValue)
{
	iSelectedPrizes = aValue;
	if (iAvatar)
		iAvatar->setPrizes(iSelectedPrizes);
}

//---------------------------------------------------------------------------
int __fastcall TiProfile::GetBonusCount(int aValue)
{
	int result = 0;
	int value = aValue;
	for (int i = 0; i < IDR_PRIZE_COUNT; i++)
	{
		result += (value & 1);
		value = value >> 1;
	}

	return result;
}

//---------------------------------------------------------------------------
int __fastcall TiProfile::GetRandomBonus()
{
	int result = -1;
	while (result < 0)
	{
		int bonus = random(IDR_PRIZE_COUNT);
		if (((1 << bonus) & iBonus) == 0)
		{
			result = bonus;
			break;
		}
	}

	return result + 1;
}

//---------------------------------------------------------------------------
void __fastcall TiProfile::CreateAvatar()
{
	iAvatar = new TiAvatar(iManager, iScreenSize, iViewport);

	iAvatar->add(TiAvatar::apBody);
	iAvatar->add(TiAvatar::apEyes);
	iAvatar->add(TiAvatar::apHair);
	iAvatar->add(TiAvatar::apShirt);
}

