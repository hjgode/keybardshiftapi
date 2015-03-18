// KeyboardShiftAPI.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "nclog.h"

#define USEWINIOCTL

#ifndef USEWINIOCTL
//alternative defines 1
	#include "winbase.h"
	#pragma comment (lib, "coredll.lib")
	#include "windev.h"
#else
//alternative defines 2
	#include "winioctl.h"
	extern "C" __declspec(dllimport) BOOL KernelIoControl(DWORD dwIoControlCode, LPVOID lpInBuf, DWORD nInBufSize, LPVOID lpOutBuf, DWORD nOutBufSize, LPDWORD lpBytesReturned);
#endif

//see PlatformFilesITC\cookja_Andromeda2_WINCE_Rel2.5\WinCE500\platform\andromedaii\src\drivers\usb\class\hid\clients\itchidkbd
#define FILE_DEVICE_KBD	0x8000		// First value available for use outside Microsoft

#define IOCTL_KBD_GET_SHIFT_STATE		CTL_CODE(FILE_DEVICE_KBD,  3, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_KBD_SET_SHIFT_STATE		CTL_CODE(FILE_DEVICE_KBD, 12, METHOD_BUFFERED, FILE_ANY_ACCESS) 
#define IOCTL_KBD_GET_SHIFT_PLANES		CTL_CODE(FILE_DEVICE_KBD, 16, METHOD_BUFFERED, FILE_ANY_ACCESS)

DWORD gLockedState = 0;

/*
IOCTL_KBD_GET_SHIFT_STATE

Gets the current keyboard shift state where 0 is the unshifted state. 1 is the shifted state.

Parameters:
	lpInBuffer - NULL
	nInBufferSize - 0
	lpOutBuffer - pointer to DWORD that the current shift plane will be returned in
	nOutBufferSize - 4
	lpBytesReturned - pointer to DWORD that will tell the number of bytes written to lpOutBuffer.
	lpOverlapped - NULL
*/
BOOL ITCGetShiftState(DWORD *value, DWORD *locked)
{
   BOOL fRet = FALSE;
    DWORD actualSize;
    HANDLE hFile;
    DWORD params[2];

    // open keyboard interface
    if ((hFile = CreateFile(_T("KBD1:"), GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, 0)) != INVALID_HANDLE_VALUE)
    {
        if ( DeviceIoControl(hFile, IOCTL_KBD_GET_SHIFT_STATE, NULL, 0, (void *)params, sizeof(DWORD)*2, &actualSize, NULL) )
        *value = params[0];
        *locked = params[1];
        fRet = TRUE;
        CloseHandle(hFile);
    }

    return fRet;

}

/*
IOCTL_KBD_ SET_SHIFT_STATE

Sets the current keyboard shift state where 0 is the unshifted state. 1 is the shifted state.

Parameters:
	lpInBuffer - pointer to DWORD containing the shift state to switch to.
	nInBufferSize - 4
	lpOutBuffer - NULL
	nOutBufferSize - 0
	lpBytesReturned - pointer to DWORD that will tell the number of bytes written to lpOutBuffer.
	lpOverlapped - NULL
*/
BOOL ITCSetShiftState(DWORD value, DWORD locked)
{
    BOOL fRet = FALSE;
    DWORD actualSize;
    HANDLE hFile;
	DWORD params[2];
	
    // open keyboard interface
    if ((hFile = CreateFile(_T("KBD1:"), GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, 0)) != INVALID_HANDLE_VALUE)
    {
		params[0] = value;
        params[1] = locked;
        if (DeviceIoControl(hFile, IOCTL_KBD_SET_SHIFT_STATE, (void *)&params, sizeof(DWORD)*2, NULL, 0, &actualSize, NULL))
        fRet = TRUE;

        CloseHandle(hFile);
    }

    return fRet;

}

/*
IOCTL_KBD_GET_SHIFT_PLANES

Gets the number of keyboard shift planes available in the current hardware including the unshifted shift plane.

Parameters:
	lpInBuffer - NULL
	nInBufferSize - 0
	lpOutBuffer - pointer to DWORD that the number of shift planes will be returned in
	nOutBufferSize - 4
	lpBytesReturned - pointer to DWORD that will tell the number of bytes written to lpOutBuffer.
	lpOverlapped - NULL
*/
BOOL ITCGetShiftPlanes(DWORD *value)
{
    BOOL fRet = FALSE;
    DWORD actualSize;
    HANDLE hFile;

    // open keyboard interface
    if ((hFile = CreateFile(_T("KBD1:"), GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, 0)) != INVALID_HANDLE_VALUE)
    {
        if (DeviceIoControl(hFile, IOCTL_KBD_GET_SHIFT_PLANES, NULL, 0, (void *)value, sizeof(DWORD), &actualSize, NULL))
            fRet = TRUE;
        CloseHandle(hFile);
    }

    return fRet;
}

const DWORD Plane_Normal	= 0x00;
const DWORD Plane_Orange	= 0x01;
const DWORD Plane_Aqua		= 0x02;

enum planes{
	normal=0x00,
	orange=0x01,
	aqua=0x02
};

TCHAR* planeStrings[] = { L"Normal Plane", L"Orange Plane", L"Aqua Plane" };

int _tmain(int argc, _TCHAR* argv[])
{
	//get number of shift planes
	DWORD dwPlaneCount=0;
	if(ITCGetShiftPlanes(&dwPlaneCount))
		nclog(L"Number of Planes: %i\n", dwPlaneCount);
	else
		nclog(L"Call to ITCGetShiftPlanes FAILED\n");

	DWORD dwVal=0, dwLock=0;
	if(ITCGetShiftState(&dwVal, &dwLock)){ // orange locked -> val=0x1, lock=0x1
		nclog(L"ITCGetShiftState: val=0x%x, lock=0x%x\n\tcurrent plane: %s", dwVal, dwLock, planeStrings[dwVal]);
	}
	else
		nclog(L"Call to ITCGetShiftState FAILED\n");

	dwVal=Plane_Orange;
	dwLock=Plane_Orange;
	if(ITCSetShiftState(dwVal, dwLock)) // orange locked -> val=0x1, lock=0x1
		nclog(L"ITCSetShiftState OK: val=0x%x, lock=0x%x\n", dwVal, dwLock);
	else
		nclog(L"Call to ITCSetShiftState FAILED\n");

	return 0;
}

