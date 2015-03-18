// SendKey.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "filelog.h"

void printHelp(){
	filelog(L"\nmissing arguments.\nThe following args are supported:\n");
	filelog(L"\n");
	filelog(L"sendKey [0xXY|char]  where XY is the hex value of the key to send\n");
	filelog(L"					   the XY values is a VK_ values of winuser.h and winuserm.h\n");
	filelog(L"					   if a single char is specified, the keycode for this char is used\n");
	filelog(L"					   only 0-9 A-Z and a-z are usually converted correctly!\n");
	filelog(L"Examples:\n");
	filelog(L"sendKey 0x09  :simultes a TAB key press\n");
	filelog(L"sendKey 0x0D  :simultes a ENTER key press\n");
	filelog(L"sendKey 0x14  :simultes a CAPS_lock key press\n");
	filelog(L"sendKey 0x31  :simultes a 1 (one) key press\n");
}

BOOL startsWith0x(const TCHAR* szStr){
	TCHAR szTest[MAX_PATH];
	wsprintf(szTest, L"%s", szStr);
	TCHAR* pszTest=szTest;
	int iLen = wcslen(szTest);
	if(iLen<2)
		return FALSE;
	if(wcsncmp(pszTest, L"0", 1)==0){
		pszTest++;
		if(wcsnicmp(pszTest, L"X", 1)==0){
			return TRUE;
		}
	}
	return FALSE;
}

int _tmain(int argc, _TCHAR* argv[])
{
	filelog(L"SendKey v1.00 started\n");
	if(argc==1){
		printHelp();
	}
	for(int i=1; i<argc; i++){
		filelog(L"arg%i: '%s'\n", i, argv[i]);
	}

	if(argc==2){
		BYTE bVkey;
		if(startsWith0x(argv[1])){
			TCHAR* stopStr; //string that shows what stopped the conversion
			bVkey = wcstol(argv[1], &stopStr, 16); //_wtoi(argv[1]);
		}
		else if(wcslen(argv[1])==1){
			bVkey = (BYTE)argv[1][0];
		}

		DWORD dwFlags=0;
		DWORD dwExtras=0;
		BYTE bScan=0;

		BOOL bExtended=FALSE;
		if(/*bVkey==VK_CAPITAL ||*/ bVkey==VK_NUMLOCK)
			bExtended=TRUE;

		if(bVkey==VK_CAPITAL){ //make=0x3A break=0xBA, http://www.codeproject.com/KB/system/keyboard.aspx
			bScan = 0x58; //BA;
		}

		if(bExtended)
			dwFlags = KEYEVENTF_EXTENDEDKEY;

		//KEY_DOWN
		keybd_event(bVkey, bScan, dwFlags, dwExtras);
		
		Sleep(1);
		
		if(bExtended)
			dwFlags = KEYEVENTF_KEYUP | KEYEVENTF_EXTENDEDKEY;
		else
			dwFlags = KEYEVENTF_KEYUP;

		//KEY_UP
		keybd_event(bVkey, bScan, dwFlags, dwExtras);
		if(bExtended)
			filelog(L"sendKey done for 0x%02x as extended key\n", bVkey);
		else
			filelog(L"sendKey done for 0x%02x as normal key\n", bVkey);
	}
	filelog(L"----- SendKey END ------\n");
	return 0;
}

/*
	Scan Codes

	Key 	Scan Code
	~   ` 	29
	!  1 	2
	@  2 	3
	#  3 	4
	$  4 	5
	%  5 	6
	^  6 	7
	&  7 	8
	*  8 	9
	(  9 	0A
	)  0 	0B
	_   -	0C
	+   = 	0D
	Backspace 	0E
	Tab 	0F
	Q 	10
	W 	11
	E 	12
	R 	13
	T 	14
	Y 	15
	U 	16
	I 	17
	O 	18
	P 	19
	{  [ 	1A
	}  ] 	1B
	|  \ 	2B
	Caps Lock 	3A
	A 	1E
	S 	1F
	D 	20
	F 	21
	G 	22
	H 	23
	J 	24
	K 	25
	L 	26
	:   ; 	27
	“  ‘ 	28
	Enter 	1C
	L SHIFT 	2A
	Z 	2C
	X 	2D
	C 	2E
	V 	2F
	B 	30
	N 	31
	M 	32
	<  , 	33
	>  . 	34
	?  / 	35
	R SHIFT 	36
	L CTRL 	1D
	L ALT 	38
	Space Bar 	39
	R ALT 	E0 38
	R CTRL 	E0 1D
	Insert 	E0 52
	Delete 	E0 53
	L Arrow 	E0 4B
	Home 	E0 47
	End 	E0 4F
	Up Arrow 	E0 48
	Dn Arrow 	E0 50
	Page Up 	E0 49
	Page Down 	E0 51
	R Arrow 	E0 4D
	Num Lock 	45
	Numeric 7 	47
	Numeric 4 	4B
	Numeric 1 	4F
	Numeric / 	E0 35
	Numeric 8 	48
	Numeric 5 	4C
	Numeric 2 	50
	Numeric 0 	52
	Numeric * 	37
	Numeric 9 	49
	Numeric 6 	4D
	Numeric 3 	51
	Numeric . 	53
	Numeric - 	4A
	Numeric + 	4E
	Numeric Enter 	E0 1C
	Esc 	1
	F1 	3B
	F2 	3C
	F3 	3D
	F4 	3E
	F5 	3F
	F6 	40
	F7 	41
	F8 	42
	F9 	43
	F10	44
	F11 	57
	F12 	58
	Print Screen 	??
	Scroll Lock 	46
	Pause 		??
	Left Win 		E0 5B
	Right Win 		E0 5C
	Application 	E0 5D
	ACPI Power 	E0 5E
	ACPI Sleep 	E0 5F
	ACPI Wake 	E0 63
*/
