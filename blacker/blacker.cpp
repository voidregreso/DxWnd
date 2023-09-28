// blacker.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "blacker.h"
#include "dxwnd.h"

//#define OutTrace(...)
extern void OutTrace(const char *, ...);

#define MAX_LOADSTRING 100

void Hider(HWND);

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    // TODO: Place code here.
    MSG msg;
    HACCEL hAccelTable;
    // Initialize global strings
    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_BLACKER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
        return FALSE;
    hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_BLACKER));
    // Main message loop:
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
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
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance) {
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style			= CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc	= WndProc;
    wcex.cbClsExtra		= 0;
    wcex.cbWndExtra		= 0;
    wcex.hInstance		= hInstance;
    wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BLACKER));
    wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_BLACKER);
    wcex.lpszClassName	= szWindowClass;
    wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    return RegisterClassEx(&wcex);
}

void WINAPI GoBlack(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    static int j = 0;
    LPCTSTR p;
    j = (j + 1) % 4;
    switch(j) {
    case 0:
        p = L"blacker \\";
        break;
    case 1:
        p = L"blacker |";
        break;
    case 2:
        p = L"blacker /";
        break;
    case 3:
        p = L"blacker -";
        break;
    }
    SetWindowText(hwnd, p);
    DXWNDSTATUS DxWndStatus;
    int DxStatus;
    HWND TargetWin;
    DxStatus = GetHookStatus(&DxWndStatus);
    TargetWin = ((DxStatus == DXW_RUNNING) && (DxWndStatus.IsFullScreen)) ? DxWndStatus.hWnd : NULL;
    Hider(TargetWin);
    //if(j==0)SetWindowLong(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW|WS_BORDER);
    //else SetWindowLong(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
    //RECT rClient;
    //GetClientRect(hwnd, &rClient);
    //if(j==0) FillRect(GetDC(hwnd), &rClient, (HBRUSH)GetSysColor(COLOR_SCROLLBAR));
    //else FillRect(GetDC(hwnd), &rClient, (HBRUSH)GetSysColor(COLOR_BACKGROUND));
    //InvalidateRect(hwnd, NULL, FALSE);
    //InvalidateRect(hwnd, &rClient, TRUE);
    //SetWindowPos(hwnd, 0, 0, 0, 0, 0, SWP_FRAMECHANGED|SWP_NOSIZE|SWP_NOMOVE|SWP_NOZORDER);
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
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
    HWND hWnd;
    hInst = hInstance; // Store instance handle in our global variable
    hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
                        // CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
                        CW_USEDEFAULT, CW_USEDEFAULT, 200, 200, NULL, NULL, hInstance, NULL);
    if (!hWnd) return FALSE;
    //ShowWindow(hWnd, nCmdShow);
    ShowWindow(hWnd, SW_SHOWMINIMIZED);
    UpdateWindow(hWnd);
    SetTimer(hWnd, 1, 500, GoBlack);
    return TRUE;
}

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
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    int wmId, wmEvent;
    PAINTSTRUCT ps;
    HDC hdc;
    switch (message) {
    case WM_COMMAND:
        wmId    = LOWORD(wParam);
        wmEvent = HIWORD(wParam);
        // Parse the menu selections:
        switch (wmId) {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        // TODO: Add any drawing code here...
        EndPaint(hWnd, &ps);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    UNREFERENCED_PARAMETER(lParam);
    switch (message) {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

static LRESULT CALLBACK Hider_Message_Handler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam) {
    switch(umsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_CLOSE:
        //quit = true;
        break;
    }
    return DefWindowProc(hwnd, umsg, wparam, lparam);
}

void Hider(HWND hwnd) {
    static BOOL DoOnce = TRUE;
    static ATOM aClass;
    static HWND wleft = 0, wright = 0, wtop = 0, wbottom = 0;
    static HWND hParent;
    RECT wRect, wDesktop;
    static HINSTANCE hinst = NULL;
    LPCWSTR pClass = L"blacker:window";
    LPCWSTR pName = L"hider";
    if(DoOnce) {
        WNDCLASSEX WndClsEx;
        DoOnce = FALSE;
        hParent = GetDesktopWindow();
        if(!hParent) OutTrace("GetDesktopWindow hParent error %d\n", GetLastError());
        hinst = GetModuleHandle(NULL);
        if(!hinst) OutTrace("GetModuleHandle ERROR err=%d\n", GetLastError());
        WndClsEx.cbSize        = sizeof(WNDCLASSEX);
        WndClsEx.style         = 0;
        WndClsEx.lpfnWndProc   = Hider_Message_Handler; //DefWindowProc;
        WndClsEx.cbClsExtra    = 0;
        WndClsEx.cbWndExtra    = 0;
        WndClsEx.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
        //WndClsEx.hCursor       = LoadCursor(NULL, IDC_CROSS);
        WndClsEx.hCursor       = NULL;
        WndClsEx.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        //WndClsEx.hbrBackground = CreateSolidBrush(RGB(200,0,0));
        WndClsEx.lpszMenuName  = NULL;
        WndClsEx.lpszClassName = pClass;
        WndClsEx.hInstance     = hinst;
        WndClsEx.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);
        // Register the application
        aClass = RegisterClassEx(&WndClsEx);
        if(!aClass) OutTrace("RegisterClassEx: ERROR err=%d\n", GetLastError());
    }
    if(hwnd && !GetWindowRect(hwnd, &wRect)) {
        OutTrace("GetWindowRect ERROR hwnd=%x err=%d\n", hwnd, GetLastError());
        return;
    }
    if(!GetWindowRect(GetDesktopWindow(), &wDesktop)) {
        OutTrace("GetWindowRect ERROR hwnd=%x err=%d\n", NULL, GetLastError());
        return;
    }
    // this is tricky: if you create a window with zero style, the manager seems to apply a default
    // overlapped window style. The only way I got to be sure that the style is exactly nothing
    // at all is to repeat the command by a SetWindowLong(hwnd, GWL_STYLE, 0) that sets again zero
    // as the correct window style.
    if(!wleft) {
        wleft = CreateWindowEx(0, pClass, pName, 0, 0, 0, 0, 0, hParent, NULL, hinst, NULL);
        if(!wleft) OutTrace("CreateWindowExA wleft error %d\n", GetLastError());
        SetWindowLong(wleft, GWL_STYLE, 0);
    }
    if(!wright) {
        wright = CreateWindowEx(0, pClass, pName, 0, 0, 0, 0, 0, hParent, NULL, hinst, NULL);
        if(!wright) OutTrace("CreateWindowExA wright error %d\n", GetLastError());
        SetWindowLong(wright, GWL_STYLE, 0);
    }
    if(!wtop) {
        wtop = CreateWindowEx(0, pClass, pName, 0, 0, 0, 0, 0, hParent, NULL, hinst, NULL);
        if(!wtop) OutTrace("CreateWindowExA wtop error %d\n", GetLastError());
        SetWindowLong(wtop, GWL_STYLE, 0);
    }
    if(!wbottom) {
        wbottom = CreateWindowEx(0, pClass, pName, 0, 0, 0, 0, 0, hParent, NULL, hinst, NULL);
        if(!wbottom) OutTrace("CreateWindowExA wbottom error %d\n", GetLastError());
        SetWindowLong(wbottom, GWL_STYLE, 0);
    }
    if(!(wleft && wright && wtop && wbottom)) return;
    if(hwnd == 0) {
        ShowWindow(wleft, SW_HIDE);
        ShowWindow(wright, SW_HIDE);
        ShowWindow(wtop, SW_HIDE);
        ShowWindow(wbottom, SW_HIDE);
        return;
    }
    MoveWindow(wleft, wDesktop.left, wDesktop.top, wRect.left, wDesktop.bottom, TRUE);
    MoveWindow(wright, wRect.right, wDesktop.top, wDesktop.right - wRect.right, wDesktop.bottom, TRUE);
    MoveWindow(wtop, wDesktop.left, wDesktop.top, wDesktop.right, wRect.top - wDesktop.top, TRUE);
    MoveWindow(wbottom, wDesktop.left, wRect.bottom, wDesktop.right, wDesktop.bottom - wRect.bottom, TRUE);
    SetWindowLong(wleft, GWL_EXSTYLE, WS_EX_TOPMOST);
    SetWindowLong(wright, GWL_EXSTYLE, WS_EX_TOPMOST);
    SetWindowLong(wtop, GWL_EXSTYLE, WS_EX_TOPMOST);
    SetWindowLong(wbottom, GWL_EXSTYLE, WS_EX_TOPMOST);
    ShowWindow(wleft, SW_SHOW);
    ShowWindow(wright, SW_SHOW);
    ShowWindow(wtop, SW_SHOW);
    ShowWindow(wbottom, SW_SHOW);
}