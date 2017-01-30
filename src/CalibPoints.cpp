//---------------------------------------------------------------------------
#include "CalibPoints.h"
#include "utils.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
__fastcall TiCalibPoints::TiCalibPoints(TiAnimationManager* aManager, int aWindowWidth, int aWindowHeight) :
		iCurrentPointIndex(-1),
		iAnimationManager(aManager)
{
	iPoints.DeleteContent = true;
}

//---------------------------------------------------------------------------
void __fastcall TiCalibPoints::lightOn()
{
	for (int i = 0; i < iPoints.Count; i++)
		iPoints[i]->lightOn();
}

//---------------------------------------------------------------------------
void __fastcall TiCalibPoints::lightOnCurrent()
{
	if (iCurrentPointIndex >= 0 && iCurrentPointIndex < iPoints.Count)
		iPoints[iCurrentPointIndex]->lightOn();
}

//---------------------------------------------------------------------------
void __fastcall TiCalibPoints::lightOff()
{
	for (int i = 0; i < iPoints.Count; i++)
		iPoints[i]->hide();
}

//---------------------------------------------------------------------------
void __fastcall TiCalibPoints::lightOffCurrent()
{
	if (iCurrentPointIndex >= 0 && iCurrentPointIndex < iPoints.Count)
		iPoints[iCurrentPointIndex]->lightOff();
}

//---------------------------------------------------------------------------
void __fastcall TiCalibPoints::fadeOut()
{
	for (int i = 0; i < iPoints.Count; i++)
		iPoints[i]->hide();
}

//---------------------------------------------------------------------------
void __fastcall TiCalibPoints::clear()
{
	iPoints.clear();
}

//---------------------------------------------------------------------------
void __fastcall TiCalibPoints::add(CalibrationPointStruct& aPoint)
{
	// we are not sure points are added in proper order, i.e. 1,2,3...
	int index = aPoint.number - 1;
	while (iPoints.Count < index) {
		iPoints.add(new TiCalibPoint(NULL, 0, 0));
	}

	if (index < iPoints.Count)
		iPoints.set(index, new TiCalibPoint(iAnimationManager, aPoint.positionX, aPoint.positionY));
	else
		iPoints.add(new TiCalibPoint(iAnimationManager, aPoint.positionX, aPoint.positionY));
}

//---------------------------------------------------------------------------
void __fastcall TiCalibPoints::prepare(int aCalibPointIndex)
{
	iCurrentPointIndex = -1;
	iIsSinglePointMode = aCalibPointIndex >= 0;

	if (iIsSinglePointMode)
	{
		TiCalibPoint* point = iPoints[aCalibPointIndex];
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
TiCalibPoint* __fastcall TiCalibPoints::next(int aPointNumber)
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
void __fastcall TiCalibPoints::paintTo(Gdiplus::Graphics* aGraphics, EiUpdateType aUpdateType)
{
	for (int i = 0; i < iPoints.Count; i++)
		iPoints[i]->paintTo(aGraphics, aUpdateType);
}

//---------------------------------------------------------------------------
TiCalibPoint* TiCalibPoints::operator[](int aIndex)
{
	return aIndex < 0 || aIndex >= iPoints.Count ? NULL : iPoints[aIndex];
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int __fastcall TiCalibPoints::GetCount()
{
	return iPoints.Count;
}

//---------------------------------------------------------------------------
TiCalibPoint* __fastcall TiCalibPoints::GetCurrent()
{
	return iCurrentPointIndex < 0 || iCurrentPointIndex >= iPoints.Count ? NULL : iPoints[iCurrentPointIndex];
}

//---------------------------------------------------------------------------
void __fastcall TiCalibPoints::Rearrange()
{
	for (int i = 1; i < iPoints.Count; i++)
	{
		int idx = 1 + random(iPoints.Count - 1);
		if (idx == i)
			continue;

		iPoints.exchange(idx, i);
	}
}

