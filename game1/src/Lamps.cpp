//---------------------------------------------------------------------------
#include "Lamps.h"
#include "utils.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
using namespace FireflyAndPoints;

//---------------------------------------------------------------------------
const int KMaxPointCalibrationCount = 3;

//---------------------------------------------------------------------------
__fastcall TiLamps::TiLamps(TiAnimationManager* aManager, int aWindowWidth, int aWindowHeight) :
		iCurrentPointIndex(-1),
		iAnimationManager(aManager)
{
	iPoints.DeleteContent = true;
}

//---------------------------------------------------------------------------
void __fastcall TiLamps::lightOn()
{
	for (int i = 0; i < iPoints.Count; i++)
		iPoints[i]->lightOn();
}

//---------------------------------------------------------------------------
void __fastcall TiLamps::lightOnCurrent()
{
	if (iCurrentPointIndex >= 0 && iCurrentPointIndex < iPoints.Count)
		iPoints[iCurrentPointIndex]->lightOn();
}

//---------------------------------------------------------------------------
void __fastcall TiLamps::lightOff()
{
	for (int i = 0; i < iPoints.Count; i++)
		iPoints[i]->hide();
}

//---------------------------------------------------------------------------
void __fastcall TiLamps::lightOffCurrent()
{
	if (iCurrentPointIndex >= 0 && iCurrentPointIndex < iPoints.Count)
		iPoints[iCurrentPointIndex]->lightOff();
}

//---------------------------------------------------------------------------
void __fastcall TiLamps::fadeOut()
{
	for (int i = 0; i < iPoints.Count; i++)
		iPoints[i]->hide();
}

//---------------------------------------------------------------------------
void __fastcall TiLamps::clear()
{
	iPoints.clear();
}

//---------------------------------------------------------------------------
void __fastcall TiLamps::add(CalibrationPointStruct& aPoint)
{
	// we are not sure points are added in proper order, i.e. 1,2,3...
	int index = aPoint.number - 1;
	while (iPoints.Count < index) {
		iPoints.add(new TiLamp(NULL, 0, 0));
	}

	if (index < iPoints.Count)
		iPoints.set(index, new TiLamp(iAnimationManager, aPoint.positionX, aPoint.positionY));
	else
		iPoints.add(new TiLamp(iAnimationManager, aPoint.positionX, aPoint.positionY));
}

//---------------------------------------------------------------------------
void __fastcall TiLamps::prepare(int aCalibPointNumber)
{
	iCurrentPointIndex = -1;
	iIsSinglePointMode = aCalibPointNumber > 0;

	if (iIsSinglePointMode)
	{
		TiLamp* point = iPoints[aCalibPointNumber - 1];
		if (point)
			point->lightOff();

		for (int i = 0; i < iPoints.Count; i++)
			iPoints[i]->Enabled = iPoints[i] == point;
	}
	else
	{
		lightOff();
		//Rearrange();
		for (int i = 0; i < iPoints.Count; i++)
			iPoints[i]->Enabled = true;
	}
}

//---------------------------------------------------------------------------
void __fastcall TiLamps::accept()
{
	if (!Current)
		return;

	Current->accept();
}

//---------------------------------------------------------------------------
TiLamp* __fastcall TiLamps::next(int aPointNumber)
{
	if (!aPointNumber)
	{
		while (true) {
			iCurrentPointIndex++;
			if (iCurrentPointIndex == iPoints.Count)
				break;
			if (iPoints[iCurrentPointIndex]->Enabled)
				break;
		}
	}
	else {
		iCurrentPointIndex = aPointNumber - 1;
	}

	bool canContinue = iCurrentPointIndex >= 0 && iCurrentPointIndex < iPoints.Count;
	if (!canContinue)
		iCurrentPointIndex = -1;

	return canContinue ? iPoints[iCurrentPointIndex] : NULL;
}

//---------------------------------------------------------------------------
void __fastcall TiLamps::paintTo(Gdiplus::Graphics* aGraphics, EiUpdateType aUpdateType)
{
	for (int i = 0; i < iPoints.Count; i++)
		iPoints[i]->paintTo(aGraphics, aUpdateType);
}

//---------------------------------------------------------------------------
bool __fastcall TiLamps::canRecalibratePoint(int aIndex)
{
	return aIndex < 0 || aIndex >= iPoints.Count ? false : iPoints[aIndex]->AcceptanceCount < KMaxPointCalibrationCount;
}

//---------------------------------------------------------------------------
int __fastcall TiLamps::pointRecalibrationCount(int aIndex)
{
	return aIndex < 0 || aIndex >= iPoints.Count ? 0 : iPoints[aIndex]->AcceptanceCount - 1;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int __fastcall TiLamps::GetCount()
{
	return iPoints.Count;
}

//---------------------------------------------------------------------------
TiLamp* __fastcall TiLamps::GetCurrent()
{
	return iCurrentPointIndex < 0 || iCurrentPointIndex >= iPoints.Count ? NULL : iPoints[iCurrentPointIndex];
}

//---------------------------------------------------------------------------
void __fastcall TiLamps::Rearrange()
{
	for (int i = 1; i < iPoints.Count; i++)
	{
		int idx = 1 + random(iPoints.Count - 1);
		if (idx == i)
			continue;

		iPoints.exchange(idx, i);
	}
}

