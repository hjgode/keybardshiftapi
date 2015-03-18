// setKeybdPlane.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "itc_shiftApi.h"
#include "filelog.h"

void printHelp(){
	filelog(L"\nmissing arguments.\nThe following args are supported:\n");
	filelog(L"[setlock|setunlock|set|unset|get] [normal|orange|aqua]\n");
	filelog(L"setlock _plane_   \tset and lock plane\n");
	filelog(L"setunlock _plane_ \tunset and unlock plane\n");
	filelog(L"set _plane_       \tset a plane (no lock)\n");
	filelog(L"unset _plane_     \tunset a plane\n");
	filelog(L"get               \tget current keyboard plane only\nreturn value is shift+lock\n\t\tfor example 0x0101 is shift=0x01 and lock=0x01\n");
}

WORD printCurrent(){
	DWORD dwSh=0, dwLck=0;
	if(ITCGetShiftState(&dwSh, &dwLck))
		filelog(L"ITCGetShiftState OK\n\tshift = %s\n\tlock = %s\n", planeStrings[dwSh], planeStrings[dwLck]);
	else
		filelog(L"ITCGetShiftState FAILED\n");
	WORD wRet = MAKEWORD((byte)dwSh, (byte)dwLck);
	return wRet;
}

int _tmain(int argc, _TCHAR* argv[])
{
	filelog(L"%s v1.00 started\n", argv[0]);

	if(argc!=3 && argc!=2){
		printHelp();
		filelog(L"---------- END ----------\n");
		return -1;
	}

	for(int i=1; i<argc; i++){
		filelog(L"arg%i: '%s'\n", i, argv[i]);
	}

	BOOL bLock=FALSE;
	planes ePlane;
	BOOL bSet=FALSE;

	if(argc==2){
		if(wcsicmp(argv[1], L"get")==0){
			filelog(L"---------- END ----------\n");
			return printCurrent();
		}
	}

	if(argc==3){
		//get action
		if(wcsicmp(argv[1], L"setlock")==0){ //set a plane and lock it
			bLock=TRUE;
			bSet=TRUE;
		}
		else if(wcsicmp(argv[1], L"setunlock")==0){ //unset a plane
			bLock=TRUE;
			bSet=TRUE;
		}
		else if(wcsicmp(argv[1], L"set")==0){
			bSet=TRUE;
			bLock=FALSE;
		}
		else if(wcsicmp(argv[1], L"unset")==0){
			bSet=FALSE;
			bLock=FALSE;
		}
		else{
			filelog(L"\targ '%s' is not supported\n\n", argv[1]);
			printHelp();
			filelog(L"---------- END ----------\n");
			return -7;
		}
		
		//get plane
		if(wcsicmp(argv[2], L"normal")==0){
			ePlane=normal;
			bSet=TRUE;
			bLock=FALSE;//you cannot lock the normal plane
		}
		else if(wcsicmp(argv[2], L"orange")==0)
			ePlane=orange;
		else if(wcsicmp(argv[2], L"aqua")==0)
			ePlane=aqua;
		else{
			filelog(L"\targ '%s' is not supported\n\n", argv[2]);
			printHelp();
			filelog(L"---------- END ----------\n");
			return -7;
		}

		//doWork
		DWORD dwVal=ePlane; 
		DWORD dwPlaneCnt;
		if(!ITCGetShiftPlanes(&dwPlaneCnt)){
			filelog(L"ITCGetShiftPlanes FAILED\n----------- END ------------\n");
			return -3;
		}

		printCurrent();

		if( dwVal >= dwPlaneCnt){
			filelog(L"Plane '%s' unavailable. Max plane=%i\n----------- END ------------\n", planeStrings[ePlane], dwPlaneCnt-1);
			return -2; //bad arg
		}

		filelog(L"Starting with set=%i, lock=%i, plane=%i\n", bSet, bLock, ePlane);

		DWORD dwLock=0;
		//unset and unlock all
		if(bSet==FALSE){
			dwVal=0;
			dwLock=0;
			if(ITCSetShiftState(dwVal, dwLock)){
				filelog(L"Plane 'normal' activated\n----------- END ------------\n");
				return 0;
			}
			else{
				filelog(L"ITCSetShiftState FAILED\n----------- END ------------\n");
				return -4;
			}
		}
		else
			dwVal=ePlane;
		
		if(bLock){
			dwLock=ePlane;
		}
		else
			dwLock=normal;

		if(ITCSetShiftState(dwVal, dwLock)){
			filelog(L"ITCSetShiftState OK for shift=%s and lock=%s\n", planeStrings[dwVal], planeStrings[dwLock]);
		}
		else{
			filelog(L"ITCSetShiftState FAILED\n----------- END ------------\n");
			return -5;
		}

	}
	filelog(L"----------- END ------------\n");
	return 0;
}

