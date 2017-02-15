// Pomodoros.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Pomodoros.h"
#include <windowsX.h>
#include <winuser.h>
#include <commctrl.h>
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "ComCtl32.lib")

#define MAX_LOADSTRING 100
#define IDC_BUTTON1 4
#define IDC_BUTTON2 5
#define IDT_TIMER1 6

#define TIME_POMODORO 25
#define COUNT_SHORTBREAK 4
#define SHORT_BREAK 3
#define LONG_BREAK 15
#define TIME_ELAPSE 900

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
void OnPaint(HWND hwnd);
void OnDestroy(HWND hwnd);
BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);

void setTimeText(int &hour, int &min, int &sec);
bool isLongBreak();
bool isShortBreak();
bool isInteger(WCHAR* buffer,int length);

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_POMODOROS, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_POMODOROS));

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



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_POMODOROS));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_POMODOROS);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      400, 50, 500,600, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//Khai báo biến
HWND txtBeginHour;
HWND txtBeginMinutes;
HWND txtRunningHour;
HWND txtRunningMinutes;
HWND txtRunningSec;

int setHour = 0;
int setMin = 0;

int iRunningHour = 0;
int iRunningMin= 0;
int iRunningSec=0;

int shortBreak = 0;
int countMin = 0;
bool isRunning = false;
bool havingShortBreak=false;
bool havingLongBreak = false;
HWND txtNotice;
//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{

		HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
		HANDLE_MSG(hWnd, WM_PAINT, OnPaint);
		HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
	case WM_TIMER:
		switch(wParam)
		{
		case IDT_TIMER1:
			if(isRunning == true)
			{
				iRunningSec++;

				setTimeText(iRunningHour,iRunningMin,iRunningSec);

				if(iRunningHour >= setHour && iRunningMin >= setMin)
				{
					isRunning = false;
					iRunningHour = 0;
					iRunningMin = 0;
					iRunningSec = 0;
					setTimeText(iRunningHour,iRunningMin,iRunningSec);
					EnableWindow(txtBeginHour,1);
					EnableWindow(txtBeginMinutes,1);
					MessageBox(hWnd,L"Finished! You are really hard-working",L"Alarm",MB_OK);
					break;
				}


				//Tính thời gian nghỉ giải lao
				if(havingLongBreak == true && countMin == LONG_BREAK)
				{
					countMin = 0;
					SetWindowText(txtNotice,L"Working!");
					MessageBox(hWnd,L"Working!",L"Alarm",MB_OK);
					havingLongBreak = false;
					havingShortBreak = false;
				}
				else if(havingShortBreak == true && countMin == SHORT_BREAK && havingLongBreak == false)
				{
					countMin = 0;
					SetWindowText(txtNotice,L"Working!");
					MessageBox(hWnd,L"Working!",L"Alarm",MB_OK);
					havingShortBreak =false;
				}

				//Kiểm tra xem đã đến giờ nghỉ chưa
				if(isShortBreak() && havingShortBreak == false && havingLongBreak == false && shortBreak != COUNT_SHORTBREAK)
				{
					havingShortBreak = true;
					countMin = 0;
					SetWindowText(txtNotice,L"Short break!");
					MessageBox(hWnd,L"Short break!",L"Alarm",MB_OK);
				}
				if(isLongBreak() && havingLongBreak == false)
				{
					havingLongBreak = true;
					countMin = 0;
					SetWindowText(txtNotice,L"Long break!");
					MessageBox(hWnd,L"Long break!",L"Alarm",MB_OK);

				}



			}
			break;
		}

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


BOOL OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
	INITCOMMONCONTROLSEX icc;
	icc.dwSize = sizeof(icc);
	icc.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&icc);
	
	// Lấy font hệ thống
	LOGFONT lf;
	GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
	HFONT hFont = CreateFont(20, lf.lfWidth,
		lf.lfEscapement, lf.lfOrientation, lf.lfWeight,
		lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet,
		lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality,
		lf.lfPitchAndFamily, lf.lfFaceName);
	HFONT hFontNumber = CreateFont(30, lf.lfWidth,
		lf.lfEscapement, lf.lfOrientation, lf.lfWeight,
		lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet,
		lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality,
		lf.lfPitchAndFamily, lf.lfFaceName);

	HWND hwnd = CreateWindowEx(0, L"STATIC", L"Your working time:", WS_CHILD | WS_VISIBLE | SS_LEFT, 50, 50, 200, 40, hWnd, NULL, hInst, NULL);
	SendMessage(hwnd, WM_SETFONT, WPARAM(hFont), TRUE);

	txtBeginHour = CreateWindowEx(0, L"EDIT", L"00", WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER, 50, 100, 40, 40, hWnd, NULL, hInst, NULL);
	SendMessage(txtBeginHour, WM_SETFONT, WPARAM(hFontNumber), TRUE);

	hwnd = CreateWindowEx(0, L"STATIC", L"hours", WS_CHILD | WS_VISIBLE | SS_LEFT, 100, 100, 100, 40, hWnd, NULL, hInst, NULL);
	SendMessage(hwnd, WM_SETFONT, WPARAM(hFontNumber), TRUE);

	txtBeginMinutes = CreateWindowEx(0, L"EDIT", L"00", WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER, 200, 100, 40, 40, hWnd, NULL, hInst, NULL);
	SendMessage(txtBeginMinutes, WM_SETFONT, WPARAM(hFontNumber), TRUE);

	hwnd = CreateWindowEx(0, L"STATIC", L"min", WS_CHILD | WS_VISIBLE | SS_LEFT, 250, 100, 50, 40, hWnd, NULL, hInst, NULL);
	SendMessage(hwnd, WM_SETFONT, WPARAM(hFontNumber), TRUE);

	hwnd = CreateWindowEx(0, L"STATIC", L"00", WS_CHILD | WS_VISIBLE , 350, 100, 40, 40, hWnd, NULL, hInst, NULL);
	SendMessage(hwnd, WM_SETFONT, WPARAM(hFontNumber), TRUE);

	hwnd = CreateWindowEx(0, L"STATIC", L"sec", WS_CHILD | WS_VISIBLE | SS_LEFT, 400, 100, 50, 40, hWnd, NULL, hInst, NULL);
	SendMessage(hwnd, WM_SETFONT, WPARAM(hFontNumber), TRUE);

	hwnd = CreateWindowEx(0,L"BUTTON",L"Start", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |WS_BORDER,120, 200, 100, 40,hWnd, (HMENU)IDC_BUTTON1,hInst,NULL);
	SendMessage(hwnd, WM_SETFONT, WPARAM(hFont), TRUE);

	hwnd = CreateWindowEx(0,L"BUTTON",L"Pause", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |WS_BORDER,270, 200, 100, 40,hWnd, (HMENU)IDC_BUTTON2,hInst,NULL);
	SendMessage(hwnd, WM_SETFONT, WPARAM(hFont), TRUE);

	txtRunningHour = CreateWindowEx(0, L"STATIC", L"00", WS_CHILD | WS_VISIBLE | SS_CENTER, 50, 300, 40, 40, hWnd, NULL, hInst, NULL);
	SendMessage(txtRunningHour, WM_SETFONT, WPARAM(hFontNumber), TRUE);

	hwnd = CreateWindowEx(0, L"STATIC", L"hours", WS_CHILD | WS_VISIBLE | SS_LEFT, 100, 300, 100, 40, hWnd, NULL, hInst, NULL);
	SendMessage(hwnd, WM_SETFONT, WPARAM(hFontNumber), TRUE);

	txtRunningMinutes = CreateWindowEx(0, L"STATIC", L"00", WS_CHILD | WS_VISIBLE | SS_CENTER, 200, 300, 40, 40, hWnd, NULL, hInst, NULL);
	SendMessage(txtRunningMinutes, WM_SETFONT, WPARAM(hFontNumber), TRUE);

	hwnd = CreateWindowEx(0, L"STATIC", L"min", WS_CHILD | WS_VISIBLE | SS_LEFT, 250, 300, 50, 40, hWnd, NULL, hInst, NULL);
	SendMessage(hwnd, WM_SETFONT, WPARAM(hFontNumber), TRUE);

	txtRunningSec = CreateWindowEx(0, L"STATIC", L"00", WS_CHILD | WS_VISIBLE , 350, 300, 40, 40, hWnd, NULL, hInst, NULL);
	SendMessage(txtRunningSec, WM_SETFONT, WPARAM(hFontNumber), TRUE);

	hwnd = CreateWindowEx(0, L"STATIC", L"sec", WS_CHILD | WS_VISIBLE | SS_LEFT, 400, 300, 50, 40, hWnd, NULL, hInst, NULL);
	SendMessage(hwnd, WM_SETFONT, WPARAM(hFontNumber), TRUE);

	txtNotice = CreateWindowEx(0, L"STATIC", L"", WS_CHILD | WS_VISIBLE | SS_LEFT, 100, 400, 200, 100, hWnd, NULL, hInst, NULL);
	SendMessage(txtNotice, WM_SETFONT, WPARAM(hFontNumber), TRUE);
	return true;
}

void OnCommand(HWND hWnd, int id, HWND hwndCtl, UINT codeNotify)
{
	int stHour = 0;
	int stMin = 0;

	WCHAR* bufferHour = NULL;
	WCHAR* bufferMin = NULL;
	switch(id)
	{
		case IDM_ABOUT:
			MessageBox(hWnd,L"1412430 Nguyễn Vũ Quang\nvuquangkhtn@gmail.com",L"My information",MB_OK|MB_ICONINFORMATION|MB_SETFOREGROUND);
			break;

		case IDM_EXIT:
			DestroyWindow(hWnd);
			return;
			
		case IDC_BUTTON1:
			stHour = GetWindowTextLength(txtBeginHour);
			bufferHour = new WCHAR[stHour+1];
			GetWindowText(txtBeginHour, bufferHour, stHour+1);
			setHour = _wtoi(bufferHour);
			
			stMin = GetWindowTextLength(txtBeginMinutes);
			bufferMin = new WCHAR[stMin+1];
			GetWindowText(txtBeginMinutes, bufferMin, stMin+1);
			setMin = _wtoi(bufferMin);

			if(!isInteger(bufferMin,stMin) || !isInteger(bufferHour,stHour))
			{
				MessageBox(hWnd,L"Value in textboxes must be unsigned interger",L"Error",MB_OK|MB_ICONERROR);
				return;
			}
			if(setMin >= 60)
			{
				MessageBox(hWnd,L"Minutes must be smaller than 60",L"Error",MB_OK|MB_ICONERROR);
				return;
			}
			if(setHour > 72)
			{
				MessageBox(hWnd,L"It's too long. Choose hours again",L"Warning",MB_OK|MB_ICONWARNING);
				return;
			}



			isRunning = true;
			EnableWindow(txtBeginHour,0);
			EnableWindow(txtBeginMinutes,0);
			SetTimer(hWnd,IDT_TIMER1,TIME_ELAPSE,(TIMERPROC)NULL);
			return;
		case IDC_BUTTON2:
			isRunning = false;
			return;
		default:
			break;
	}
	if(!bufferHour)
		delete[] bufferHour;
	if(!bufferMin)
		delete[] bufferMin;
}

void OnPaint(HWND hWnd)
{
	HDC hdc;
	PAINTSTRUCT ps;
	hdc = BeginPaint(hWnd, &ps);
	// TODO: Add any drawing code here...
	EndPaint(hWnd, &ps);
	return;
}

void OnDestroy(HWND hwnd)
{
	PostQuitMessage(0);
	return;
}

void setTimeText(int &hour, int &min, int &sec)
{
	WCHAR* bufferRunningHour = new WCHAR[2];
	WCHAR* bufferRunningMin = new WCHAR[2];
	WCHAR* bufferRunningSec = new WCHAR[2];

	if(sec >= 60)
	{
		sec = 0;
		countMin++;
		min++;
	}
	if(min >= 60)
	{
		min=0;
		hour++;
	}
	wsprintf(bufferRunningSec,L"%d",sec);
	wsprintf(bufferRunningMin,L"%d",min);
	wsprintf(bufferRunningHour,L"%d",hour);
	SetWindowText(txtRunningSec,bufferRunningSec);
	SetWindowText(txtRunningMinutes,bufferRunningMin);
	SetWindowText(txtRunningHour,bufferRunningHour);
	return;
}

bool isShortBreak()
{
	if(countMin == TIME_POMODORO)
	{
		shortBreak++;
		return true;
	}
	return false;
}

bool isLongBreak()
{
	if(shortBreak == COUNT_SHORTBREAK)
	{
		shortBreak=0;
		return true;
	}
	return false;

}

bool isInteger(WCHAR* buffer,int length)
{
	if(buffer == NULL && length == 0)
		return true;
	for(int i=0;i<length;i++)
	{
		if(buffer[i] == '\0')
			break;
		if(buffer[i] < '0' || buffer[i] > '9' )
		{
			return false;
		}

	}
	return true;

}
