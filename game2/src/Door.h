//---------------------------------------------------------------------------
#ifndef GAME2_DoorH
#define GAME2_DoorH

//---------------------------------------------------------------------------
#include "Scene.h"
#include "Animation.h"

//---------------------------------------------------------------------------
namespace ProfiledGame {

//---------------------------------------------------------------------------
class TiDoor : public TiScene
{
	public:
		enum EiMovement
		{
			dmUp,
			dmDown,
			dmOpen,
			dmClose
		};

	private:
		TiAnimation* iDoorsClose;
		TiAnimation* iDoorsOpen;
		TiAnimation* iDoorsUp;
		TiAnimation* iDoorsDown;

		TiAnimation* iDoorVisible;

		bool iIsClosed;

		void __fastcall onAnimationDone(TObject* aSender);

	public:
		__fastcall TiDoor(TiAnimationManager* aManager, TiSize aScreenSize, TiSize aViewport);

		void __fastcall show(EiMovement aMovement, int aDuration = 0);
		void __fastcall run();

		__property bool IsClosed = { read = iIsClosed };
};

//---------------------------------------------------------------------------
};	// namespace

#endif
