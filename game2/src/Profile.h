//---------------------------------------------------------------------------
#ifndef GAME2_ProfileH
#define GAME2_ProfileH

//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <dynarray.h>

//---------------------------------------------------------------------------
#include "Avatar.h"
#include "CalibPoints.h"

#include "XML.h"

//---------------------------------------------------------------------------
namespace ProfiledGame {

//---------------------------------------------------------------------------
class TiProfile
{
	struct SiXMLData
	{
		WideString Name;
		int Avatar;
		int Level;
		int Score;
		int Saldo;
		int Bonus;
		int Succeeded;
		int Failured;
		int Duration;
		SiXMLData() : Avatar(0), Level(0), Score(0), Saldo(0), Bonus(0), Succeeded(0), Failured(0), Duration(0) { }
	};

	typedef TiDynArray<SiXMLData> TiXMLDataList;

	public:
		enum EiStatus
		{
			psNoName,
			psNew,
			psLogged
		};

		struct SiGameScore
		{
			int Success;
			int Failure;
			int Duration;

			SiGameScore() : Success(0), Failure(0), Duration(0) { }
		};

		struct SiTargetPoint
		{
			TiCalibPoints::EiAnimationDuration Speed;
			int TargetID;
			int Duration;
			bool Success;

			SiTargetPoint() {}
			SiTargetPoint(TiCalibPoints::EiAnimationDuration aSpeed, int aTargetID, int aDuration, bool aSuccess) :
					Speed(aSpeed), TargetID(aTargetID), Duration(aDuration), Success(aSuccess) {}
		};

	private:
		typedef TiDynArray<SiTargetPoint> TiTargetPoints;

		TiAnimationManager* iManager;
		TiSize iScreenSize;
		TiSize iViewport;

		TiXMLDataList* iSavedData;

		EiStatus iStatus;
		WideString iName;
		int iSkinColorID;
		int iHairColorID;
		int iEyesColorID;
		int iShirtColorID;

		TiAvatar* iAvatar;

		SiGameScore iGameScore;

		TiTargetPoints* iTargetPoints;

		int iLevel;
		int iScore;
		int iSaldo;
		int iBonus;

		int iGameCoins;
		int iGameBonus;

		void __fastcall SetName(WideString aValue);
		void __fastcall SetSkinColorID(int aValue);
		void __fastcall SetHairColorID(int aValue);
		void __fastcall SetEyesColorID(int aValue);
		void __fastcall SetShirtColorID(int aValue);

		void __fastcall SetGameScore(SiGameScore& aValue);
		int __fastcall GetLevelScore();
		int __fastcall GetLevelScoreMax();
		bool __fastcall GetIsSucceeded();

		int __fastcall GetBonusCount(int aValue);
		int __fastcall GetRandomBonus();
		void __fastcall CreateAvatar();

	public:
		TiProfile(TiAnimationManager* aManager, TiSize aScreenSize, TiSize aViewport);
		//TiProfile(TiAnimationManager* aManager, WideString aName);

		~TiProfile();

		void __fastcall load(TiXML_INI* aStorage);
		void __fastcall save(TiXML_INI* aStorage);

		void __fastcall addPoint(SiTargetPoint* aPoint);
		void __fastcall updateScore();

		static int __fastcall getBonusCountMax();

		__property EiStatus Status = { read = iStatus };
		__property WideString Name = { read = iName, write = SetName };
		__property int SkinColorID = { read = iSkinColorID, write = SetSkinColorID };
		__property int HairColorID = { read = iHairColorID, write = SetHairColorID };
		__property int EyesColorID = { read = iEyesColorID, write = SetEyesColorID };
		__property int ShirtColorID = { read = iShirtColorID, write = SetShirtColorID };

		__property TiAvatar* Avatar = { read = iAvatar };

		__property SiGameScore GameScore = { read = iGameScore, write = SetGameScore };

		__property int Level = { read = iLevel };
		__property int LevelScore = { read = GetLevelScore };
		__property int LevelScoreMax = { read = GetLevelScoreMax };
		__property int Saldo = { read = iSaldo };
		__property int GameCoins = { read = iGameCoins };
		__property int GameBonus = { read = iGameBonus };

		__property bool IsSucceeded = { read = GetIsSucceeded };

		__property TiTargetPoints* TargetPoints = { read = iTargetPoints };
};

//---------------------------------------------------------------------------
};	// namespace

#endif
