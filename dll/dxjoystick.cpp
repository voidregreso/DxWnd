#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS

#include "dxwnd.h"
#include "dxwcore.hpp"
#include "syslibs.h"
#include "dxhook.h"
#include "dxhelper.h"
#include "dxwjoy.h"
#include "resource.h"

#include "MMSystem.h"
#include <stdio.h>

static void ShowJoystick(LONG x, LONG y, BOOL bIdle, DWORD dwButtons) {
    static BOOL JustOnce = FALSE;
    extern HMODULE hInst;
    BITMAP bm;
    HDC hClientDC;
    static HBITMAP g_hbmJoyCross;
    static HBITMAP g_hbmJoyFire1;
    static HBITMAP g_hbmJoyFire2;
    static HBITMAP g_hbmJoyFire3;
    static HBITMAP g_hbmJoyCenter;
    HBITMAP g_hbmJoy;
    RECT client;
    RECT win;
    POINT PrevViewPort;
    // don't show when system cursor is visible
    CURSORINFO ci;
    ci.cbSize = sizeof(CURSORINFO);
    GetCursorInfo(&ci);
    if(ci.flags == CURSOR_SHOWING) return;
    hClientDC = (*pGDIGetDC)(dxw.GethWnd());
    (*pGetClientRect)(dxw.GethWnd(), &client);
    if(!JustOnce) {
        g_hbmJoyCross = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_CROSS));
        g_hbmJoyFire1 = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_FIRE1));
        g_hbmJoyFire2 = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_FIRE2));
        g_hbmJoyFire3 = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_FIRE3));
        g_hbmJoyCenter = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_JOYCENTER));
        JustOnce = TRUE;
    }
    HDC hdcMem = CreateCompatibleDC(hClientDC);
    switch(dwButtons) {
    case 0:
        g_hbmJoy = bIdle ? g_hbmJoyCenter : g_hbmJoyCross;
        break;
    case JOY_BUTTON1:
        g_hbmJoy = g_hbmJoyFire1;
        break;
    case JOY_BUTTON2:
        g_hbmJoy = g_hbmJoyFire2;
        break;
    case JOY_BUTTON1|JOY_BUTTON2:
        g_hbmJoy = g_hbmJoyFire3;
        break;
    case JOY_BUTTON3:
        g_hbmJoy = g_hbmJoyCenter;
        break;
    default:
        g_hbmJoy = NULL;
        break;
    }
    if(g_hbmJoy == NULL) return; // show nothing ...
    HBITMAP hbmOld = (HBITMAP)(*pSelectObject)(hdcMem, g_hbmJoy);
    GetObject(g_hbmJoy, sizeof(bm), &bm);
    (*pGetWindowRect)(dxw.GethWnd(), &win);
    (*pSetViewportOrgEx)(hClientDC, 0, 0, &PrevViewPort);
    int w, h;
    w = bm.bmWidth;
    h = bm.bmHeight;
    (*pGDIBitBlt)(hClientDC, x - (w >> 1), y - (h >> 1), w, h, hdcMem, 0, 0, SRCPAINT);
    (*pSetViewportOrgEx)(hClientDC, PrevViewPort.x, PrevViewPort.y, NULL);
    (*pSelectObject)(hdcMem, hbmOld);
    (*pGDIReleaseDC)(dxw.GethWnd(), hClientDC); // v2.04.97: add to avoid DC leakage
    DeleteDC(hdcMem);
}

BOOL vjGetJoy(char *apiname, LPJOYINFO lpj) {
    LONG x, y, CenterX, CenterY;
    HWND hwnd;
    DWORD dwButtons;
    static BOOL bJoyLock = FALSE;
    BOOL bInDeadZone = FALSE;
    static DWORD dwLastClick = 0;
    extern DXWNDSTATUS *pStatus;
    DWORD dwVJoyStatus;
    dwVJoyStatus = GetHookInfo()->VJoyStatus;
    if(!(dwVJoyStatus & VJOYENABLED)) {
        lpj->wXpos = DXWJOY_XCENTER;
        lpj->wYpos = DXWJOY_YCENTER;
        lpj->wZpos = DXWJOY_ZCENTER;
        pStatus->joyposx = DXWJOY_XCENTER;
        pStatus->joyposy = DXWJOY_YCENTER;
        OutDebugIN("%s: disabled ret=0\n", apiname);
        return FALSE;
    }
    dwButtons = 0;
    if ((GetKeyState(VK_LBUTTON) < 0) || (dwVJoyStatus & B1AUTOFIRE)) dwButtons |= JOY_BUTTON1;
    if ((GetKeyState(VK_RBUTTON) < 0) || (dwVJoyStatus & B2AUTOFIRE)) dwButtons |= JOY_BUTTON2;
    if (GetKeyState(VK_MBUTTON) < 0) dwButtons |= JOY_BUTTON3;
    OutDebugIN("%s: Virtual Joystick buttons=%#x\n", apiname, dwButtons);
    if((dwButtons == JOY_BUTTON3) && (dwVJoyStatus & LOCKONMIDBTN)) { // V2.05.45: enable on LOCKONMIDBTN flag
        if(((*pGetTickCount)() - dwLastClick) > 200) {
            bJoyLock = !bJoyLock;
            dwButtons &= ~JOY_BUTTON3;
            dwLastClick = (*pGetTickCount)();
        }
    }
    // default: 0 offset from centered position
    x = 0;
    y = 0;
    // get cursor position and map it to virtual joystick x,y axis
    if(hwnd = dxw.GethWnd()) {
        POINT pt;
        RECT client;
        POINT upleft = {0, 0};
        (*pGetClientRect)(hwnd, &client);
        (*pClientToScreen)(hwnd, &upleft);
        (*pGetCursorPos)(&pt);
        CenterX = (client.right - client.left) >> 1;
        CenterY = (client.bottom - client.top) >> 1;
        pt.x -= upleft.x;
        pt.y -= upleft.y;
        if(bJoyLock || !dxw.bActive) {
            // when the joystick is "locked" (bJoyLock) or when the window lost focus
            // (dxw.bActive == FALSE) place the joystick in the central position
            OutDebugIN("%s: CENTERED lock=%#x active=%#x\n", apiname, bJoyLock, dxw.bActive);
            x = 0;
            y = 0;
            pt.x = client.right >> 1;
            pt.y = client.bottom >> 1;
            dwButtons = JOY_BUTTON3;
        } else {
            OutDebugIN("%s: ACTIVE mouse=(%d,%d)\n", apiname, pt.x, pt.y);
            if(dwVJoyStatus & VJMOUSEMAP) {
                if(pt.x < client.left) pt.x = client.left;
                if(pt.x > client.right) pt.x = client.right;
                if(pt.y < client.top) pt.y = client.top;
                if(pt.y > client.bottom) pt.y = client.bottom;
                x = ((pt.x - CenterX) * DXWJOY_XSPAN) / client.right;
                y = ((pt.y - CenterY) * DXWJOY_YSPAN) / client.bottom;
                if(dwVJoyStatus & VJAUTOCENTER) {
                    // autocenter: each time, moves 1/20 distance toward centered 0,0 position
                    // 1/20 = 0.05, changing value changes the autocenter speed (must be >0.0 and <1.0)
                    int x1, y1;
                    x1 = (int)(pt.x + upleft.x - ((pt.x - CenterX) * 0.05));
                    y1 = (int)(pt.y + upleft.y - ((pt.y - CenterY) * 0.05));
                    if((x1 != pt.x + upleft.x) || (y1 != pt.y + upleft.y)) (*pSetCursorPos)(x1, y1);
                }
            }
            if(dwVJoyStatus & VJKEYBOARDMAP) {
                if (GetKeyState(VK_LEFT) < 0)  x = -DXWJOY_XSPAN / 4;
                if (GetKeyState(VK_RIGHT) < 0) x = +DXWJOY_XSPAN / 4;
                if (GetKeyState(VK_UP) < 0)    y = -DXWJOY_YSPAN / 4;
                if (GetKeyState(VK_DOWN) < 0)  y = +DXWJOY_YSPAN / 4;
                if (GetKeyState(VK_SPACE) < 0)    dwButtons |= JOY_BUTTON1;
                if (GetKeyState(VK_LCONTROL) < 0) dwButtons |= JOY_BUTTON2;
                if (GetKeyState(VK_LSHIFT) < 0)   dwButtons |= JOY_BUTTON3;
            }
            if(dwVJoyStatus & INVERTXAXIS) x = -x;
            if(dwVJoyStatus & INVERTYAXIS) y = -y;
            //if(dwVJoyStatus & VJSENSIVITY){
            {
                DWORD dwSensivity = GetHookInfo()->VJoySensivity;
                if(dwSensivity) {
                    x = (x * (LONG)dwSensivity) / 100;
                    y = (y * (LONG)dwSensivity) / 100;
                }
            }
            // deadzone correction
            {
                DWORD dwDeadZone = GetHookInfo()->VJoyDeadZone;
                if(dwDeadZone) {
                    long dwDead = (DXWJOY_XCENTER * dwDeadZone) / 100;
                    long dwLive = DXWJOY_YCENTER - dwDead;
                    if(x > 0) x = (x < +dwDead) ? 0 : (x - dwDead) * DXWJOY_XCENTER / dwLive;
                    if(x < 0) x = (x > -dwDead) ? 0 : (x + dwDead) * DXWJOY_XCENTER / dwLive;
                    if(y > 0) y = (y < +dwDead) ? 0 : (y - dwDead) * DXWJOY_XCENTER / dwLive;
                    if(y < 0) y = (y > -dwDead) ? 0 : (y + dwDead) * DXWJOY_XCENTER / dwLive;
                    if((x == 0) && (y == 0)) bInDeadZone = TRUE;
                }
                // bounds checking
                {
                    if(x > DXWJOY_XCENTER) x = DXWJOY_XCENTER;
                    if(x < -DXWJOY_XCENTER) x = -DXWJOY_XCENTER;
                    if(y > DXWJOY_YCENTER) y = DXWJOY_YCENTER;
                    if(y < -DXWJOY_XCENTER) y = -DXWJOY_XCENTER;
                }
            }
        }
        if (dwVJoyStatus & CROSSENABLED) {
            int jx, jy;
            jx  = CenterX + (x * client.right) / DXWJOY_XSPAN;
            jy  = CenterY + (y * client.bottom) / DXWJOY_YSPAN;
            ShowJoystick(jx, jy, bJoyLock | bInDeadZone, dwButtons);
        }
    }
    lpj->wXpos = DXWJOY_XCENTER + x;
    lpj->wYpos = DXWJOY_YCENTER + y;
    lpj->wZpos = DXWJOY_ZCENTER;
    lpj->wButtons = dwButtons;
    OutTraceC("%s: pos=(%d,%d)\n", apiname, lpj->wXpos, lpj->wYpos);
    pStatus->joyposx = (short)((x * 64) / DXWJOY_XCENTER);
    pStatus->joyposy = (short)((y * 64) / DXWJOY_YCENTER);
    // return TRUE (valid data coming) only when active and not locked
    // this makes no difference for winmm.dll joyGetPos functions, but stops the data flow in dinput methods
    return (!bJoyLock && dxw.bActive);
}

void vjGetCaps(LPJOYCAPSA pjc) {
    // mimick vJoy working settings
    memset(pjc, 0, sizeof(JOYCAPSA));
    //strncpy(pjc->szPname, "DxWnd Joystick Emulator", MAXPNAMELEN);
    strncpy(pjc->szPname, "Microsoft PC-joystick driver", MAXPNAMELEN);
    pjc->wPid = 0xdead;
    pjc->wMid = 0xbeef;
    pjc->wXmin = 0;
    pjc->wXmax = DXWJOY_XSPAN;
    pjc->wYmin = 0;
    pjc->wYmax = DXWJOY_YSPAN;
    pjc->wNumButtons = 2;
    pjc->wMaxButtons = 2;
    pjc->wPeriodMin = 10;
    pjc->wPeriodMax = 1000;
    pjc->wCaps = 0;
    pjc->wMaxAxes = 2;
    pjc->wNumAxes = 2;
#if (WINVER >= 0x0400)
    pjc->wCaps = 0;
    pjc->wMaxAxes = 6;
    pjc->wNumAxes = 2;
    pjc->wMaxButtons = 32;
    pjc->wRmin = pjc->wUmin = pjc->wVmin = 0;
    pjc->wRmax = pjc->wUmax = pjc->wVmax = DXWJOY_ZSPAN;
    strcpy(pjc->szRegKey, "DINPUT.DLL");
#endif
}
