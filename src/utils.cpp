//---------------------------------------------------------------------------
#include "utils.h"

#include <shlobj.h>
#include <FileCtrl.hpp>

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
	iRef(aRef),
	iKilled(false)
{ }

//---------------------------------------------------------------------------
void __fastcall TiTimeout::Timer(void)
{
	Enabled = false;
	TTimer::Timer();

	if (iRef)
		*iRef = NULL;

	iKilled = true;
	//delete this;
}

//---------------------------------------------------------------------------
void __fastcall TiTimeout::run(UINT aTimeout, TNotifyEvent aCallback, TiTimeout** aRef, Classes::TComponent* aOwner)
{
	TiTimeout* timer = new TiTimeout(aOwner, aRef);
	timer->Interval = aTimeout;
	timer->OnTimer = aCallback;
	timer->Enabled = true;

	if (aRef)
		*aRef = timer;
}

//---------------------------------------------------------------------------
void __fastcall TiTimeout::runSync(UINT aTimeout, TNotifyEvent aCallback, TiTimeout** aRef)
{
	TiSyncThread* thread = new TiSyncThread(aTimeout, aCallback, aRef);
}

//---------------------------------------------------------------------------
void __fastcall TiTimeout::kill()
{
	Enabled = false;

	if (iRef)
		*iRef = NULL;

	iKilled = true;
	//delete this;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall TiSyncThread::TiSyncThread(UINT aTimeout, TNotifyEvent aCallback, TiTimeout** aRef) :
	TThread(false)
{
	FreeOnTerminate = true;
	iTimeout = aTimeout;
	iCallback = aCallback;
	iRef = aRef;
}

//---------------------------------------------------------------------------
void __fastcall TiSyncThread::Execute()
{
	Synchronize((TThreadMethod)&CreateTimer);
}

//---------------------------------------------------------------------------
void __fastcall TiSyncThread::CreateTimer()
{
	MessageBox(NULL, "PRESS ENTER", "", MB_OK);
	TiTimeout::run(iTimeout, iCallback, iRef);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall TiTimestamp::TiTimestamp()
{
	LARGE_INTEGER fr;
	::QueryPerformanceFrequency(&fr);

	iFrequency = fr.LowPart;

	reset();
}

//---------------------------------------------------------------------------
void __fastcall TiTimestamp::reset()
{
	LARGE_INTEGER li;
	::QueryPerformanceCounter(&li);

	iStartTime = li.QuadPart;
}

//---------------------------------------------------------------------------
long __fastcall TiTimestamp::ms()
{
	long result;
	LARGE_INTEGER li;
	::QueryPerformanceCounter(&li);

	return double(li.QuadPart - iStartTime) * 1000 / iFrequency;
}

//---------------------------------------------------------------------------
double __fastcall TiTimestamp::sec()
{
	long result;
	LARGE_INTEGER li;
	::QueryPerformanceCounter(&li);

	return double(li.QuadPart - iStartTime) / iFrequency;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall TiLogger::TiLogger(String aName) :
		iFileHandle(-1)
{
	String filePath = folder("CustomCalib");
	String date = TDateTime::CurrentDateTime().FormatString("yy-mm-dd_hh-nn-ss");
	iFileName = filePath + "\\" + date + "_" + aName + ".log";
}

//---------------------------------------------------------------------------
__fastcall TiLogger::~TiLogger()
{
	if (iFileHandle < 0)
		return;

	FileClose(iFileHandle);
}

//---------------------------------------------------------------------------
String __fastcall TiLogger::folder(String& aName)
{
	char userFolder[MAX_PATH];
	::SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_CURRENT, userFolder);

	String filePath = String(userFolder) + "\\University of Tampere\\" + aName;
	if (!DirectoryExists(filePath))
		ForceDirectories(filePath);

	return filePath;
}

//---------------------------------------------------------------------------
void __fastcall TiLogger::line(String& aText)
{
	if (iFileHandle < 0)
		iFileHandle = FileCreate(iFileName);

	if (iFileHandle < 0)
		return;

	FileWrite(iFileHandle, aText.c_str(), (int)aText.Length());
	FileWrite(iFileHandle, "\r\n", 2);
}

