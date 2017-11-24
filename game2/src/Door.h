//---------------------------------------------------------------------------
#ifndef GAME2_DoorH
#define GAME2_DoorH

//---------------------------------------------------------------------------
#include "Scene.h"
#include "AnimationSimple.h"

//---------------------------------------------------------------------------
namespace ProfiledGame {

//---------------------------------------------------------------------------
class TiDoor : public TiScene
{
	public:
		enum EiMovement
		{
			dmUp,
			dmDown
		};

	private:
		TiAnimationSimple* iDoorsUp;
		TiAnimationSimple* iDoorsDown;

		TiAnimationSimple* iDoorVisible;

		bool iIsClosed;

		void __fastcall onAnimationDone(TObject* aSender);

	public:
		__fastcall TiDoor(TiAnimationManager* aManager, TiSize aScreenSize, TiSize aViewport);

		void __fastcall show(EiMovement aMovement);
		void __fastcall run();

		__property bool IsClosed = { read = iIsClosed };
};

//---------------------------------------------------------------------------
};	// namespace

#endif
