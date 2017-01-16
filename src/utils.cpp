//---------------------------------------------------------------------------
#include "utils.h"

//---------------------------------------------------------------------------
//#include <SysInit.hpp>

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool sameSign(double a, double b)
{
	return sign(a) == sign(b);
//	if (a == 0.0 || b == 0.0)
//		return true;

//	return fabs(a/fabs(a) - b/fabs(b)) < 0.5;
}

//---------------------------------------------------------------------------
int sign(double a) {	return a < 0.0 ? -1 : (a > 0.0 ? 1 : 0); }
int sign(float a) {	return a < 0.0f ? -1 : (a > 0.0f ? 1 : 0); }
int sign(int a) {	return a < 0 ? -1 : (a > 0 ? 1 : 0); }
int sign(short a) {	return a < 0 ? -1 : (a > 0 ? 1 : 0); }

//---------------------------------------------------------------------------
int randInRange(int _min, int _max)
{
	return random(_max - _min + 1) + _min;
}

//---------------------------------------------------------------------------
bool loadBitmapFromPNG(UINT resourceID, Gdiplus::Bitmap** bitmapOut)
{
	bool bRet = false;

	HRSRC hResourceHandle = ::FindResource(HInstance, MAKEINTRESOURCE(resourceID), "IMAGE");

	if (0 == hResourceHandle)
		return bRet;

	DWORD nImageSize = ::SizeofResource(HInstance, hResourceHandle);
	if (0 == nImageSize)
		return bRet;

	HGLOBAL hResourceInstance = ::LoadResource(HInstance, hResourceHandle);
	if (0 == hResourceInstance)
		return bRet;

	const void* pResourceData = ::LockResource(hResourceInstance);
	if (0 == pResourceData)
	{
		FreeResource(hResourceInstance);
		return bRet;
	}

	HGLOBAL hBuffer = ::GlobalAlloc(GMEM_MOVEABLE, nImageSize);
	if (0 == hBuffer)
	{
		FreeResource(hResourceInstance);
		return bRet;
	}

	void* pBuffer = ::GlobalLock(hBuffer);
	if (0 != pBuffer)
	{
		CopyMemory(pBuffer, pResourceData, nImageSize);
		IStream* pStream = 0;
		if (S_OK == ::CreateStreamOnHGlobal(hBuffer, FALSE, &pStream))
		{
			*bitmapOut = new Gdiplus::Bitmap(pStream);
			pStream->Release();
			bRet = true;
		}
		::GlobalUnlock(hBuffer);
	}
	::GlobalFree(hBuffer);

	UnlockResource(hResourceInstance);
	FreeResource(hResourceInstance);

	return bRet;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall TiTimeout::TiTimeout(Classes::TComponent* aOwner, TiTimeout** aRef) :
	TTimer(aOwner),
	iRef(aRef)
{ }

//---------------------------------------------------------------------------
void __fastcall TiTimeout::Timer(void)
{
	Enabled = false;
	TTimer::Timer();

	if (iRef)
		*iRef = NULL;

	delete this;
}

//---------------------------------------------------------------------------
void __fastcall TiTimeout::run(UINT timeout, TNotifyEvent aCallback, TiTimeout** ref)
{
	TiTimeout* timer = new TiTimeout(NULL, ref);
	timer->Interval = timeout;
	timer->OnTimer = aCallback;
	timer->Enabled = true;

	if (ref)
		*ref = timer;
}

//---------------------------------------------------------------------------
void __fastcall TiTimeout::kill()
{
	Enabled = false;

	if (iRef)
		*iRef = NULL;

	delete this;
}
