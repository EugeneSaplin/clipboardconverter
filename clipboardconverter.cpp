// clipboardconverter.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <shellapi.h>
#include <cassert>
#include "clipboardconverter.h"
#define MAX_LOADSTRING 100

#define HOTKEYID1 0xB001
#define HOTKEYID2 0xB002
#define MYWM_NOTIFYICON		(WM_USER+3)
#define ID_EXIT_APPLICATION 0xFFE2

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_CLIPBOARDCONVERTER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_CLIPBOARDCONVERTER);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDC_CLIPBOARDCONVERTER);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)IDC_CLIPBOARDCONVERTER;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	bool AlreadyRunning;

    HANDLE hMutexOneInstance = ::CreateMutex( NULL, TRUE,
		_T("CBCONVERTER-1234214AA-EF65-4ec8-A54B-0A15581D103B"));

    AlreadyRunning = (GetLastError() == ERROR_ALREADY_EXISTS);

    if (hMutexOneInstance != NULL) 
    {
        ::ReleaseMutex(hMutexOneInstance);
    }
	if ( AlreadyRunning ) 
		return FALSE;


	HWND hWnd;

	hInst = hInstance; // Store instance handle in our global variable

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	//   ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	TrayMessage(hWnd,NIM_ADD);
	RegisterHotkey(hWnd);
	return TRUE;
}


BOOL TrayMessage(HWND hWnd,DWORD dwMessage)
{
//	CString sTip(_T("Sdictionary"));	
	const WCHAR sTip[] = L"ClipboardConverter";
	NOTIFYICONDATA tnd;

	tnd.cbSize		= sizeof(NOTIFYICONDATA);
	tnd.hWnd		= hWnd;
	tnd.uID			= IDC_CLIPBOARDCONVERTER;
	tnd.uFlags		= NIF_MESSAGE|NIF_ICON;
	tnd.uCallbackMessage	= MYWM_NOTIFYICON;
	HICON hIcon = LoadIcon(hInst, MAKEINTRESOURCE (IDC_CLIPBOARDCONVERTER));
	tnd.hIcon = hIcon;
	tnd.uFlags		= NIF_MESSAGE|NIF_ICON|NIF_TIP;

	lstrcpyn(tnd.szTip, (LPCTSTR)sTip, sizeof(tnd.szTip)/sizeof(tnd.szTip[0]) );
	
	return Shell_NotifyIcon(dwMessage, &tnd);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	if( message == MYWM_NOTIFYICON )
	{
		switch (lParam)
		{
	    case WM_LBUTTONDBLCLK:
			{
				return TRUE;
			}
		    break;
		case WM_RBUTTONDOWN:
			{
				SetForegroundWindow(hWnd);
				POINT pt;
				GetCursorPos(&pt);
				HMENU hMenu = CreatePopupMenu();
				AppendMenu(hMenu,MF_STRING,IDM_ABOUT,L"About...");
				AppendMenu(hMenu,MF_SEPARATOR,0,0);
				AppendMenu(hMenu,MF_STRING,IDM_EXIT,L"Exit");
				TrackPopupMenu(hMenu,TPM_RIGHTALIGN,pt.x,pt.y,0,hWnd,NULL);
				DestroyMenu(hMenu);
			}
			break;
		}
	}


	switch (message) 
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDM_ABOUT:
			DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_DESTROY:
		TrayMessage(hWnd,NIM_DELETE );	
		UnregisterHotKey(hWnd,HOTKEYID1 );
		UnregisterHotKey(hWnd,HOTKEYID2 );	
		PostQuitMessage(0);
		break;
	case WM_HOTKEY:
		ClipboardConverter(hWnd);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void RegisterHotkey(HWND hWnd)
{
	UnregisterHotKey(hWnd,HOTKEYID1);
	UnregisterHotKey(hWnd,HOTKEYID2);	
	
	UINT mod = MOD_CONTROL | MOD_SHIFT;
	UINT key1 = 0x43; // 'C' from ms-help://MS.MSDNQTR.2003OCT.1033/winui/winui/windowsuserinterface/userinput/VirtualKeyCodes.htm
	UINT key2 = VK_INSERT; 

	RegisterHotKey(hWnd,HOTKEYID1,mod,key1);
	RegisterHotKey(hWnd,HOTKEYID2,mod,key2);
}

void ClipboardConverter(HWND hWnd)
{
	TCHAR* textData = NULL;
	HGLOBAL global;
	// Get data from Clipboard
	if (!OpenClipboard(hWnd)) 
        return; 
	HGLOBAL handle = GetClipboardData(CF_UNICODETEXT);
	LPCTSTR text = (LPCTSTR)GlobalLock(handle);
	int siz = lstrlen(text);
	if (siz)
	{
		global = GlobalAlloc(GMEM_MOVEABLE,(siz+1)*sizeof(TCHAR));
		textData = (TCHAR*)GlobalLock(global);
		memcpy(textData,text,(siz+1)*sizeof(TCHAR));
        textData[siz] = '\0';	
	}
	GlobalUnlock(handle);
	CloseClipboard();
	if (siz)
	{
		if (!OpenClipboard(hWnd)) 
			return; 
		EmptyClipboard();
		SetClipboardData(CF_UNICODETEXT,textData);
		GlobalUnlock(global);
		//GlobalFree(global);
		CloseClipboard();
	}
}

LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}
