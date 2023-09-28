#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include "dxwnd.h"
#include "dxwcore.hpp"
#include "syslibs.h"
#include "dxhelper.h"

//#define NOROUNDING

// IsValidMainWindow: returns TRUE if the main window can be queried for coordinates,
// FALSE otherwise (e.g. when minimized)

BOOL dxwCore::IsValidMainWindow() {
    RECT Client;
    POINT UpLeft = {0, 0};
    if(!(*pGetClientRect)(hWnd, &Client)) return FALSE;
    if((Client.right == 0) || (Client.bottom == 0)) return FALSE;
    if(!(*pClientToScreen)(hWnd, &UpLeft)) return FALSE;
    return TRUE;
}

// if the main window coordinates are still valid updates the window placement values

void dxwCore::UpdateDesktopCoordinates() {
    RECT Client;
    POINT UpLeft = {0, 0};
    //OutDebugDW("dxwCore::UpdateDesktopCoordinates: OLD pos=(%d,%d) size=(%dx%d)\n", iPosX, iPosY, iSizX, iSizY);
    if(!(*pGetClientRect)(hWnd, &Client)) return;
    if((Client.right == 0) || (Client.bottom == 0)) return;
    if(!(*pClientToScreen)(hWnd, &UpLeft)) return;
    // update coordinates ....
    iPosX = UpLeft.x;
    iPosY = UpLeft.y;
    iSizX = Client.right - Client.left;
    iSizY = Client.bottom - Client.top;
    //OutDebugDW("dxwCore::UpdateDesktopCoordinates: NEW pos=(%d,%d) size=(%dx%d)\n", iPosX, iPosY, iSizX, iSizY);
}

// GetScreenRect: returns a RECT sized as the virtual desktop

RECT dxwCore::GetScreenRect() {
    static RECT Screen;
    Screen.left = 0;
    Screen.top = 0;
    Screen.right = dwScreenWidth;
    Screen.bottom = dwScreenHeight;
    return Screen;
}

// GetUnmappedScreenRect: returns the RECT of the real coordinates of the virtual desktop,
// that is the client area of the main window

RECT dxwCore::GetUnmappedScreenRect() {
    RECT Screen;
    Screen.top = iPosY;
    Screen.bottom = iPosY + iSizY; // v2.04.48 fixed
    Screen.left = iPosX;
    Screen.right = iPosX + iSizX;
    return Screen;
}

// GetFrameOffset: return the POINT coordinates of the upper-left corner of the main window frame
// taking in account of the window frame and title bar, then it needs to query hWnd

POINT dxwCore::GetFrameOffset() {
    RECT wrect;
    POINT FrameOffset = {0, 0};
    (*pGetWindowRect)(hWnd, &wrect);
    (*pClientToScreen)(hWnd, &FrameOffset);
    FrameOffset.x -= wrect.left;
    FrameOffset.y -= wrect.top;
    OutDebugDW("GetFrameOffset: offset=(%d,%d)\n", FrameOffset.x, FrameOffset.y);
    return FrameOffset;
}

// ClientOffset: returns the virtual offset of a window within the virtual desktop

POINT dxwCore::ClientOffset(HWND hwnd) {
    POINT upleft, ret = {0, 0};
    if(iSizX && iSizY) {
        upleft.x = upleft.y = 0;
        (*pClientToScreen)(hwnd, &upleft);
#ifdef NOROUNDING
        ret.x = ((upleft.x - iPosX) * dwScreenWidth) / iSizX;
        ret.y = ((upleft.y - iPosY) * dwScreenHeight) / iSizY;
#else
        ret.x = (((upleft.x - iPosX) * dwScreenWidth) + (iSizX >> 1)) / iSizX;
        ret.y = (((upleft.y - iPosY) * dwScreenHeight) + (iSizY >> 1)) / iSizY;
#endif
        if(ret.x < 0) ret.x = 0;
        if(ret.y < 0) ret.y = 0;
        OutDebugDW("ClientOffset: hwnd=%#x offset=(%d,%d)\n", hwnd, ret.x, ret.y);
    }
    return ret;
}

// GetWindowRect: returns the virtual coordinates of a window within the virtual desktop

RECT dxwCore::GetWindowRect(RECT win) {
    if(iSizX && iSizY) {
#ifdef NOROUNDING
        win.left = (((win.left - iPosX) * (LONG)dwScreenWidth)) / iSizX;
        win.top = (((win.top - iPosY) * (LONG)dwScreenHeight)) / iSizY;
        win.right = (((win.right - iPosX) * (LONG)dwScreenWidth)) / iSizX;
        win.bottom = (((win.bottom - iPosY) * (LONG)dwScreenHeight)) / iSizY;
#else
        win.left = (((win.left - iPosX) * (LONG)dwScreenWidth) + (iSizX >> 1)) / iSizX;
        win.top = (((win.top - iPosY) * (LONG)dwScreenHeight) + (iSizY >> 1)) / iSizY;
        win.right = (((win.right - iPosX) * (LONG)dwScreenWidth) + (iSizX >> 1)) / iSizX;
        win.bottom = (((win.bottom - iPosY) * (LONG)dwScreenHeight) + (iSizY >> 1)) / iSizY;
#endif
    }
    return win;
}

// GetClientRect: returns the virtual client coordinates of a window

RECT dxwCore::GetClientRect(RECT win) {
    if(iSizX && iSizY) {
#ifdef NOROUNDING
        win.left = ((win.left * dwScreenWidth)) / iSizX;
        win.top = ((win.top * dwScreenHeight)) / iSizY;
        win.right = ((win.right * dwScreenWidth)) / iSizX;
        win.bottom = ((win.bottom * dwScreenHeight)) / iSizY;
#else
        win.left = ((win.left * dwScreenWidth) + (iSizX >> 1)) / iSizX;
        win.top = ((win.top * dwScreenHeight) + (iSizY >> 1)) / iSizY;
        win.right = ((win.right * dwScreenWidth) + (iSizX >> 1)) / iSizX;
        win.bottom = ((win.bottom * dwScreenHeight) + (iSizY >> 1)) / iSizY;
#endif
    }
    return win;
}

// AddCoordinates, SubCoordinates: adds or subtracts a POINT displacement

POINT dxwCore::AddCoordinates(POINT p1, POINT p2) {
    POINT ps;
    ps.x = p1.x + p2.x;
    ps.y = p1.y + p2.y;
    return ps;
}

RECT dxwCore::AddCoordinates(RECT r1, POINT p2) {
    RECT rs;
    rs.left = r1.left + p2.x;
    rs.right = r1.right + p2.x;
    rs.top = r1.top + p2.y;
    rs.bottom = r1.bottom + p2.y;
    return rs;
}

POINT dxwCore::SubCoordinates(POINT p1, POINT p2) {
    POINT ps;
    ps.x = p1.x - p2.x;
    ps.y = p1.y - p2.y;
    return ps;
}

// MapWindow, MapWindowRect: transforms the virtual window coordinates of the virtual desktop into real ones

RECT dxwCore::MapWindowRect(LPRECT lpRect) {
    RECT RetRect;
    if(!Windowize) {
        if(lpRect)
            RetRect = *lpRect;
        else {
            RetRect.left = RetRect.top = 0;
            RetRect.right = dwScreenWidth;
            RetRect.bottom = dwScreenHeight;
        }
        return RetRect;
    }
    if(lpRect) {
        RetRect.left = iPosX + (lpRect->left * iSizX / dwScreenWidth);
        RetRect.right = iPosX + (lpRect->right * iSizX / dwScreenWidth);
        RetRect.top = iPosY + (lpRect->top * iSizY / dwScreenHeight);
        RetRect.bottom = iPosY + (lpRect->bottom * iSizY / dwScreenHeight);
    } else {
        RetRect.left = iPosX;
        RetRect.right = iPosX + iSizX;
        RetRect.top = iPosY;
        RetRect.bottom = iPosY + iSizY;
    }
    return RetRect;
}

void dxwCore::MapWindow(LPRECT rect) {
#ifdef NOROUNDING
    rect->left = iPosX + (((rect->left * iSizX)) / (int)dwScreenWidth);
    rect->top = iPosY + (((rect->top * iSizY)) / (int)dwScreenHeight);
    rect->right = iPosX + (((rect->right * iSizX)) / (int)dwScreenWidth);
    rect->bottom = iPosY + (((rect->bottom * iSizY)) / (int)dwScreenHeight);
#else
    rect->left = iPosX + (((rect->left * iSizX) + (dwScreenWidth >> 1)) / (int)dwScreenWidth);
    rect->top = iPosY + (((rect->top * iSizY) + (dwScreenHeight >> 1)) / (int)dwScreenHeight);
    rect->right = iPosX + (((rect->right * iSizX) + (dwScreenWidth >> 1)) / (int)dwScreenWidth);
    rect->bottom = iPosY + (((rect->bottom * iSizY) + (dwScreenHeight >> 1)) / (int)dwScreenHeight);
#endif
}

void dxwCore::MapWindow(int *nXDest, int *nYDest, int *nWDest, int *nHDest) {
#ifdef NOROUNDING
    *nXDest = iPosX + (((*nXDest * iSizX)) / (int)dwScreenWidth);
    *nYDest = iPosY + (((*nYDest * iSizY)) / (int)dwScreenHeight);
    *nWDest = ((*nWDest * iSizX)) / (int)dwScreenWidth;
    *nHDest = ((*nHDest * iSizY)) / (int)dwScreenHeight;
#else
    *nXDest = iPosX + (((*nXDest * iSizX) + (dwScreenWidth >> 1)) / (int)dwScreenWidth);
    *nYDest = iPosY + (((*nYDest * iSizY) + (dwScreenHeight >> 1)) / (int)dwScreenHeight);
    *nWDest = ((*nWDest * iSizX) + (dwScreenWidth >> 1)) / (int)dwScreenWidth;
    *nHDest = ((*nHDest * iSizY) + (dwScreenHeight >> 1)) / (int)dwScreenHeight;
#endif
}

void dxwCore::MapWindow(int *nXDest, int *nYDest) {
#ifdef NOROUNDING
    *nXDest = iPosX + (((*nXDest * iSizX)) / (int)dwScreenWidth);
    *nYDest = iPosY + (((*nYDest * iSizY)) / (int)dwScreenHeight);
#else
    *nXDest = iPosX + (((*nXDest * iSizX) + (dwScreenWidth >> 1)) / (int)dwScreenWidth);
    *nYDest = iPosY + (((*nYDest * iSizY) + (dwScreenHeight >> 1)) / (int)dwScreenHeight);
#endif
}

void dxwCore::MapWindow(LPPOINT lppoint) {
#ifdef NOROUNDING
    lppoint->x = iPosX + (((lppoint->x * iSizX)) / dwScreenWidth);
    lppoint->y = iPosY + (((lppoint->y * iSizY)) / dwScreenHeight);
#else
    lppoint->x = iPosX + (((lppoint->x * iSizX) + (dwScreenWidth >> 1)) / dwScreenWidth);
    lppoint->y = iPosY + (((lppoint->y * iSizY) + (dwScreenHeight >> 1)) / dwScreenHeight);
#endif
}

// MapClient, MapClientRect: transforms the client virtual coordinates of the virtual desktop into real ones

RECT dxwCore::MapClientRect(LPRECT lpRect) {
    RECT RetRect;
    if(lpRect) {
        RetRect.left = (lpRect->left * iSizX / dwScreenWidth);
        RetRect.right = (lpRect->right * iSizX / dwScreenWidth);
        RetRect.top = (lpRect->top * iSizY / dwScreenHeight);
        RetRect.bottom = (lpRect->bottom * iSizY / dwScreenHeight);
    } else {
        RetRect.left = 0;
        RetRect.right = iSizX;
        RetRect.top = 0;
        RetRect.bottom = iSizY;
    }
    return RetRect;
}

void dxwCore::MapClient(LPRECT rect) {
#ifdef NOROUNDING
    rect->left = ((rect->left * iSizX)) / (int)dwScreenWidth;
    rect->top = ((rect->top * iSizY)) / (int)dwScreenHeight;
    rect->right = ((rect->right * iSizX)) / (int)dwScreenWidth;
    rect->bottom = ((rect->bottom * iSizY)) / (int)dwScreenHeight;
#else
    rect->left = ((rect->left * iSizX) + (dwScreenWidth >> 1)) / (int)dwScreenWidth;
    rect->top = ((rect->top * iSizY) + (dwScreenHeight >> 1)) / (int)dwScreenHeight;
    rect->right = ((rect->right * iSizX) + (dwScreenWidth >> 1)) / (int)dwScreenWidth;
    rect->bottom = ((rect->bottom * iSizY) + (dwScreenHeight >> 1)) / (int)dwScreenHeight;
#endif // NOROUNDING
}

void dxwCore::MapClient(int *nXDest, int *nYDest, int *nWDest, int *nHDest) {
#ifdef NOROUNDING
    *nXDest = ((*nXDest * iSizX)) / (int)dwScreenWidth;
    *nYDest = ((*nYDest * iSizY)) / (int)dwScreenHeight;
    *nWDest = ((*nWDest * iSizX)) / (int)dwScreenWidth;
    *nHDest = ((*nHDest * iSizY)) / (int)dwScreenHeight;
#else
    *nXDest = ((*nXDest * iSizX) + (dwScreenWidth >> 1)) / (int)dwScreenWidth;
    *nYDest = ((*nYDest * iSizY) + (dwScreenHeight >> 1)) / (int)dwScreenHeight;
    *nWDest = ((*nWDest * iSizX) + (dwScreenWidth >> 1)) / (int)dwScreenWidth;
    *nHDest = ((*nHDest * iSizY) + (dwScreenHeight >> 1)) / (int)dwScreenHeight;
#endif // NOROUNDING
}

void dxwCore::MapClient(short *nXDest, short *nYDest, short *nWDest, short *nHDest) {
    int x, y, w, h;
#ifdef NOROUNDING
    x = ((*nXDest * iSizX)) / (int)dwScreenWidth;
    y = ((*nYDest * iSizY)) / (int)dwScreenHeight;
    w = ((*nWDest * iSizX)) / (int)dwScreenWidth;
    h = ((*nHDest * iSizY)) / (int)dwScreenHeight;
#else
    x = ((*nXDest * iSizX) + (dwScreenWidth >> 1)) / (int)dwScreenWidth;
    y = ((*nYDest * iSizY) + (dwScreenHeight >> 1)) / (int)dwScreenHeight;
    w = ((*nWDest * iSizX) + (dwScreenWidth >> 1)) / (int)dwScreenWidth;
    h = ((*nHDest * iSizY) + (dwScreenHeight >> 1)) / (int)dwScreenHeight;
#endif // NOROUNDING
    *nXDest = (short)x;
    *nYDest = (short)y;
    *nWDest = (short)w;
    *nHDest = (short)h;
    //OutTrace("dxwCore::MapClient %d %d %d %d\n", *nXDest, *nYDest, *nWDest, *nHDest);
}

void dxwCore::MapClient(LPPOINT lppoint) {
#ifdef NOROUNDING
    lppoint->x = ((lppoint->x * iSizX)) / (int)dwScreenWidth;
    lppoint->y = ((lppoint->y * iSizY)) / (int)dwScreenHeight;
#else
    lppoint->x = ((lppoint->x * iSizX) + (dwScreenWidth >> 1)) / (int)dwScreenWidth;
    lppoint->y = ((lppoint->y * iSizY) + (dwScreenHeight >> 1)) / (int)dwScreenHeight;
#endif // NOROUNDING
}

void dxwCore::MapClient(int *nXDest, int *nYDest) {
#ifdef NOROUNDING
    *nXDest = ((*nXDest * iSizX)) / (int)dwScreenWidth;
    *nYDest = ((*nYDest * iSizY)) / (int)dwScreenHeight;
#else
    *nXDest = ((*nXDest * iSizX) + (dwScreenWidth >> 1)) / (int)dwScreenWidth;
    *nYDest = ((*nYDest * iSizY) + (dwScreenHeight >> 1)) / (int)dwScreenHeight;
#endif // NOROUNDING
}

void dxwCore::MapClient(USHORT *nXDest, USHORT *nYDest) {
#ifdef NOROUNDING
    *nXDest = (USHORT)((*nXDest * iSizX)) / (int)dwScreenWidth;
    *nYDest = (USHORT)((*nYDest * iSizY)) / (int)dwScreenHeight;
#else
    *nXDest = (USHORT)((*nXDest * iSizX) + (dwScreenWidth >> 1)) / (int)dwScreenWidth;
    *nYDest = (USHORT)((*nYDest * iSizY) + (dwScreenHeight >> 1)) / (int)dwScreenHeight;
#endif // NOROUNDING
}

void dxwCore::MapClient(unsigned long *nXDest, unsigned long *nYDest) {
#ifdef NOROUNDING
    *nXDest = ((*nXDest * iSizX)) / (int)dwScreenWidth;
    *nYDest = ((*nYDest * iSizY)) / (int)dwScreenHeight;
#else
    *nXDest = ((*nXDest * iSizX) + (dwScreenWidth >> 1)) / (int)dwScreenWidth;
    *nYDest = ((*nYDest * iSizY) + (dwScreenHeight >> 1)) / (int)dwScreenHeight;
#endif // NOROUNDING
}

void dxwCore::MapClient(float *nXDest, float *nYDest) {
    *nXDest = ((*nXDest * (float)iSizX)) / (float)dwScreenWidth;
    *nYDest = ((*nYDest * (float)iSizY)) / (float)dwScreenHeight;
}

// UnmapWindow: transforms the window real coordinates of the real desktop into virtual ones

void dxwCore::UnmapWindow(LPRECT rect) {
    if(iSizX && iSizY) {
        rect->left = ((rect->left  - iPosX) * (int)dwScreenWidth) / iSizX;
        rect->top = ((rect->top  - iPosY) * (int)dwScreenHeight) / iSizY;
        rect->right = ((rect->right  - iPosX) * (int)dwScreenWidth) / iSizX;
        rect->bottom = ((rect->bottom  - iPosY) * (int)dwScreenHeight) / iSizY;
    }
}

void dxwCore::UnmapWindow(LPPOINT point) {
    if(iSizX && iSizY) {
        point->x = ((point->x  - iPosX) * (int)dwScreenWidth) / iSizX;
        point->y = ((point->y  - iPosY) * (int)dwScreenHeight) / iSizY;
    }
}

void dxwCore::UnmapWindow(int *x, int *y, int *cx, int *cy) {
    if(iSizX && iSizY) {
        *x = ((*x  - iPosX) * (int)dwScreenWidth) / iSizX;
        *y = ((*y  - iPosY) * (int)dwScreenHeight) / iSizY;
        *cx = (*cx * (int)dwScreenWidth) / iSizX;
        *cy = (*cy * (int)dwScreenHeight) / iSizY;
    }
}

// UnmapClient: transforms the client real coordinates of the real desktop into virtual ones

void dxwCore::UnmapClient(LPPOINT lppoint) {
    if(iSizX && iSizY) {
#ifdef NOROUNDING
        lppoint->x = ((lppoint->x * (int)dwScreenWidth)) / iSizX;
        lppoint->y = ((lppoint->y * (int)dwScreenHeight)) / iSizY;
#else
        lppoint->x = ((lppoint->x * (int)dwScreenWidth) + (iSizX >> 1)) / iSizX;
        lppoint->y = ((lppoint->y * (int)dwScreenHeight) + (iSizY >> 1)) / iSizY;
#endif // NOROUNDING
    }
}

void dxwCore::UnmapClient(int *nXDest, int *nYDest) {
    if(iSizX && iSizY) {
#ifdef NOROUNDING
        *nXDest = ((*nXDest * (int)dwScreenWidth)) / iSizX;
        *nYDest = ((*nYDest * (int)dwScreenHeight)) / iSizY;
#else
        *nXDest = ((*nXDest * (int)dwScreenWidth) + (iSizX >> 1)) / iSizX;
        *nYDest = ((*nYDest * (int)dwScreenHeight) + (iSizY >> 1)) / iSizY;
#endif // NOROUNDING
    }
}

void dxwCore::UnmapClient(int *nXDest, int *nYDest, int *nWidth, int *nHeight) {
    if(iSizX && iSizY) {
#ifdef NOROUNDING
        *nXDest = ((*nXDest * (int)dwScreenWidth)) / iSizX;
        *nYDest = ((*nYDest * (int)dwScreenHeight)) / iSizY;
        *nWidth = ((*nWidth * (int)dwScreenWidth)) / iSizX;
        *nHeight = ((*nHeight * (int)dwScreenHeight)) / iSizY;
#else
        *nXDest = ((*nXDest * (int)dwScreenWidth) + (iSizX >> 1)) / iSizX;
        *nYDest = ((*nYDest * (int)dwScreenHeight) + (iSizY >> 1)) / iSizY;
        *nWidth = ((*nWidth * (int)dwScreenWidth) + (iSizX >> 1)) / iSizX;
        *nHeight = ((*nHeight * (int)dwScreenHeight) + (iSizY >> 1)) / iSizY;
#endif // NOROUNDING
    }
}

void dxwCore::UnmapClient(LPRECT lpRect) {
    if(iSizX && iSizY) {
#ifdef NOROUNDING
        lpRect->left = ((lpRect->left * (int)dwScreenWidth)) / iSizX;
        lpRect->right = ((lpRect->right * (int)dwScreenWidth)) / iSizX;
        lpRect->top = ((lpRect->top * (int)dwScreenHeight)) / iSizY;
        lpRect->bottom = ((lpRect->bottom * (int)dwScreenHeight)) / iSizY;
#else
        lpRect->left = ((lpRect->left * (int)dwScreenWidth) + (iSizX >> 1)) / iSizX;
        lpRect->right = ((lpRect->right * (int)dwScreenWidth) + (iSizX >> 1)) / iSizX;
        lpRect->top = ((lpRect->top * (int)dwScreenHeight) + (iSizY >> 1)) / iSizY;
        lpRect->bottom = ((lpRect->bottom * (int)dwScreenHeight) + (iSizY >> 1)) / iSizY;
#endif // NOROUNDING
    }
}

void dxwCore::UnmapClient(float *nXDest, float *nYDest) {
    *nXDest = ((*nXDest * (float)dwScreenWidth)) / (float)iSizX;
    *nYDest = ((*nYDest * (float)dwScreenHeight)) / (float)iSizY;
}

void dxwCore::UnmapClient(double *nXDest, double *nYDest) {
    *nXDest = ((*nXDest * (double)dwScreenWidth)) / (double)iSizX;
    *nYDest = ((*nYDest * (double)dwScreenHeight)) / (double)iSizY;
}

// GetMonitorWorkarea: retrieves the desktop coordinates of the whole desktop (id == -1) or of a given monitor (id >= 0)
// returning either the whole desktop area (WorkArea == FALSE) or the workarea (WorkArea == TRUE)

// callback function called by EnumDisplayMonitors for each enabled monitor

struct ENUM_DISP_ARG {
    int TargetId;
    int MonId;
    RECT rect;
    HMONITOR hMon;
};

BOOL CALLBACK EnumDispProc(HMONITOR hMon, HDC dcMon, RECT *pRcMon, LPARAM lParam) {
    ENUM_DISP_ARG *pArg = reinterpret_cast<ENUM_DISP_ARG *>(lParam);
    if (pArg->MonId >= pArg->TargetId) {
        pArg->rect = *pRcMon;
        pArg->hMon = hMon;
        return FALSE;
    } else {
        pArg->MonId++;
        return TRUE;
    }
}

BOOL CALLBACK IdentifyDisplay(HMONITOR hMon, HDC dcMon, RECT *pRcMon, LPARAM lParam) {
    ENUM_DISP_ARG *pArg = reinterpret_cast<ENUM_DISP_ARG *>(lParam);
    //OutTrace("DEBUG: hMon=%#x id=%d rect=(%d,%d)-(%d,%d) search=(%d,%d)-(%d,%d)\n",
    //	hMon, pArg->MonId, pRcMon->left, pRcMon->top, pRcMon->right, pRcMon->bottom,
    //	pArg->rect.left, pArg->rect.top, pArg->rect.right, pArg->rect.bottom);
#ifdef COMPAREAREA
    // compare 2 points, it's enough ...
    if ((pArg->rect.top == pRcMon->top) && (pArg->rect.left == pRcMon->left)) {
#else
    if (pArg->hMon == hMon) {
#endif
        return FALSE;
    } else {
        pArg->MonId++;
        return TRUE;
    }
}

void dxwCore::GetMonitorWorkarea(LPRECT lpRect, BOOL WorkArea) {
    // initialization for usage before hooking!!!
    if(!pSystemParametersInfoA) pSystemParametersInfoA =::SystemParametersInfoA;
    if(!pGetClientRect) pGetClientRect =::GetClientRect;
    if(!pGetMonitorInfoA) pGetMonitorInfoA =::GetMonitorInfoA;
    if(MonitorId == -1) { // whole desktop from all monitors
        if(WorkArea)
            (*pSystemParametersInfoA)(SPI_GETWORKAREA, NULL, lpRect, 0);
        else
            (*pGetClientRect)((*pGetDesktopWindow)(), lpRect);
    } else {
        MONITORINFO mi;
        memset((void *)&mi, 0, sizeof(MONITORINFO));
        mi.cbSize = sizeof(MONITORINFO);
        ENUM_DISP_ARG arg = { 0 };
        arg.TargetId = MonitorId; // monitor to look for
        EnumDisplayMonitors(0, 0, EnumDispProc, reinterpret_cast<LPARAM>(&arg));
        if((*pGetMonitorInfoA)(arg.hMon, &mi))
            *lpRect = WorkArea ? mi.rcWork : mi.rcMonitor;
        else {
            // recurse with undefined id
            MonitorId = -1;
            GetMonitorWorkarea(lpRect, WorkArea);
        }
    }
    OutDebugDW("dxwCore::GetMonitorWorkarea: id=%d workarea=%#x rect=(%d,%d)-(%d,%d)\n",
               MonitorId, WorkArea, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom);
}

int dxwCore::PickMonitorId() {
    RECT Target;
    HMONITOR hBestMonitor;
    Target.left = iPosX;
    Target.right = iPosX + iSizX;
    Target.top = iPosY;
    Target.bottom = iPosY + iSizY;
    if(!pGetMonitorInfoA) pGetMonitorInfoA =::GetMonitorInfoA;
    hBestMonitor = MonitorFromRect(&Target, MONITOR_DEFAULTTONEAREST);
#ifdef COMPAREAREA
    MONITORINFOEX mi;
    memset(&mi, 0, sizeof(mi));
    mi.cbSize = sizeof(mi);
    (*pGetMonitorInfo)(hBestMonitor, &mi);
    OutTrace("PickMonitorId: monitor=\"%s\", hmon=%#x rect=(%d,%d)-(%d,%d) type=%s\n",
             mi.szDevice,
             hBestMonitor,
             mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right, mi.rcMonitor.bottom,
             (mi.dwFlags & MONITORINFOF_PRIMARY) ? "PRIMARY" : "SECONDARY");
    ENUM_DISP_ARG arg = { 0 };
    arg.rect = mi.rcMonitor; // search for monitor with same area
    arg.hMon = hBestMonitor;
#else
    ENUM_DISP_ARG arg = { 0 };
    arg.hMon = hBestMonitor;
#endif
    EnumDisplayMonitors(0, 0, IdentifyDisplay, reinterpret_cast<LPARAM>(&arg));
    OutTrace("PickMonitorId: monitorid=%d\n", arg.MonId);
    return arg.MonId;
}

static BOOL IsEmpty(RECT rect) {
    BOOL ret = TRUE;
    if(rect.bottom) ret = FALSE;
    if(rect.top) ret = FALSE;
    if(rect.left) ret = FALSE;
    if(rect.right) ret = FALSE;
    return ret;
}

typedef enum {
    POS_NOWHERE = 0,
    POS_TOP,
    POS_BOTTOM,
    POS_LEFT,
    POS_RIGHT
};

static int GetTaskbarRect(LPRECT lprect) {
    int pos;
    HMONITOR hPrimaryMonitor;
    const POINT ptZero = {0, 0};
    MONITORINFO mi;
    RECT rTaskBar = {0, 0, 0, 0};
    char *sLocation;
    hPrimaryMonitor = MonitorFromPoint(ptZero, MONITOR_DEFAULTTOPRIMARY);
    memset(&mi, 0, sizeof(mi));
    mi.cbSize = sizeof(mi);
    if(!pGetMonitorInfoA) pGetMonitorInfoA =::GetMonitorInfoA;
#ifndef DXW_NOTRACES
    if(!(*pGetMonitorInfoA)(hPrimaryMonitor, &mi))
        OutTraceE("dxw::GetTaskbarRect: GetMonitorInfo ERROR err=%d\n", GetLastError());
#else
    (*pGetMonitorInfoA)(hPrimaryMonitor, &mi);
#endif // DXW_NOTRACES
    //OutDebugDW("Desktop  pos=(%d,%d)-(%d,%d)\n", mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right, mi.rcMonitor.bottom);
    //OutDebugDW("WorkArea pos=(%d,%d)-(%d,%d)\n", mi.rcWork.left, mi.rcWork.top, mi.rcWork.right, mi.rcWork.bottom);
    if(mi.rcWork.top != mi.rcMonitor.top) {
        // taskbar on top
        sLocation = "top";
        pos = POS_TOP;
        rTaskBar.top = 0;
        rTaskBar.bottom = mi.rcWork.top;
        rTaskBar.left = 0;
        rTaskBar.right = mi.rcWork.right;
    } else if (mi.rcWork.bottom != mi.rcMonitor.bottom) {
        // taskbar on bottom
        sLocation = "bottom";
        pos = POS_BOTTOM;
        rTaskBar.top = mi.rcWork.bottom;
        rTaskBar.bottom = mi.rcMonitor.bottom;
        rTaskBar.left = 0;
        rTaskBar.right = mi.rcWork.right;
    } else if (mi.rcWork.left != mi.rcMonitor.left) {
        // taskbar on left
        sLocation = "left";
        pos = POS_LEFT;
        rTaskBar.top = 0;
        rTaskBar.bottom = mi.rcMonitor.bottom;
        rTaskBar.left = 0;
        rTaskBar.right = mi.rcWork.left;
    } else if (mi.rcWork.right != mi.rcMonitor.right) {
        // taskbar on right
        sLocation = "right";
        pos = POS_RIGHT;
        rTaskBar.top = 0;
        rTaskBar.bottom = mi.rcMonitor.bottom;
        rTaskBar.left = mi.rcWork.right;
        rTaskBar.right = mi.rcMonitor.right;
    } else {
        // no taskbar ?
        sLocation = "nowhere";
        pos = POS_NOWHERE;
        rTaskBar.top = 0;
        rTaskBar.left = 0;
        rTaskBar.right = 0;
        rTaskBar.bottom = 0;
    }
    OutDebugDW("Taskbar location=%s pos=(%d,%d)-(%d,%d)\n",
               sLocation, rTaskBar.left, rTaskBar.top, rTaskBar.right, rTaskBar.bottom);
    *lprect = rTaskBar;
    return pos;
}

// v2.04.91: initialization for general positions in case of centered/fake fullscreen modes

void dxwCore::InitPos(TARGETMAP *target) {
    RECT rect, desktop;
    int MaxX, MaxY;
    char *label = "???";
    // initialize from target
    iSiz0X = target->sizx;
    iSiz0Y = target->sizy;
    iPos0X = target->posx;
    iPos0Y = target->posy;
    // Aspect Ratio from window size, or traditional 4:3 by default
    iRatioX = iSiz0X ? iSiz0X : 800;
    iRatioY = iSiz0Y ? iSiz0Y : 600;
    if(dwFlags4 & SUPPORTHDTV) {
        // change default ratio for 16:9 display
        iRatioX = iSiz0X ? iSiz0X : 1600;
        iRatioY = iSiz0Y ? iSiz0Y : 900;
    }
    // AutoScale: when iSiz0X == iSiz0Y == 0, size is set to current screen resolution
    bAutoScale = !(iSiz0X && iSiz0Y);
    switch(Coordinates) {
    case DXW_DESKTOP_CENTER:
        label = "center";
        MaxX = iSiz0X ? iSiz0X : 800;
        MaxY = iSiz0Y ? iSiz0Y : 600;
        GetMonitorWorkarea(&desktop, TRUE);
        iPos0X = (desktop.right + desktop.left - MaxX) / 2;
        iPos0Y = (desktop.bottom + desktop.top - MaxY) / 2;
        break;
    case DXW_DESKTOP_WORKAREA:
        label = "workarea";
        GetMonitorWorkarea(&rect, TRUE);
        if(dxw.dwFlags2 & KEEPASPECTRATIO) dxw.FixWorkarea(&rect);
        iPos0X = rect.left;
        iPos0Y = rect.top;
        iSiz0X = rect.right - rect.left;
        iSiz0Y = rect.bottom - rect.top;
        break;
    case DXW_DESKTOP_FULL:
        label = "desktop";
        GetMonitorWorkarea(&rect, FALSE);
        if(dxw.dwFlags2 & KEEPASPECTRATIO) dxw.FixWorkarea(&rect);
        iPos0X = rect.left;
        iPos0Y = rect.top;
        iSiz0X = rect.right - rect.left;
        iSiz0Y = rect.bottom - rect.top;
        break;
    case DXW_SET_COORDINATES:
    default:
        label = "coord";
        // just use defined values
        break;
    }
    // initialize working set
    iPosX = iPos0X;
    iPosY = iPos0Y;
    iSizX = iSiz0X;
    iSizY = iSiz0Y;
    target->posx = (short)iPosX;
    target->posy = (short)iPosY;
    target->sizx = (short)iSizX;
    target->sizy = (short)iSizY;
    OutDebugDW("dxwCore::InitPos: coord=%d(%s) pos0=(%d,%d) siz0=(%dx%d)\n",
               Coordinates, label, iPos0X, iPos0Y, iSiz0X, iSiz0Y);
}

// v.2.1.80: unified positioning logic into CalculateWindowPos routine
// now taking in account for window menus (see "Alien Cabal")
// sets the correct values in wp LPWINDOWPOS structure, but doesn't do any window update

void dxwCore::CalculateWindowPos(HWND hwnd, DWORD width, DWORD height, LPWINDOWPOS wp) {
    ApiName("dxwCore::CalculateWindowPos");
    RECT rect, desktop;
    DWORD dwStyle, dwExStyle;
    int MaxX, MaxY;
    HMENU hMenu;
    char *label = "???";
    switch(Coordinates) {
    case DXW_DESKTOP_CENTER:
        label = "center";
        if(bAutoScale) {
            MaxX = GetScreenWidth();
            MaxY = GetScreenHeight();
            if(dxw.FilterXScalingFactor) MaxX *= FilterXScalingFactor;
            if(dxw.FilterYScalingFactor) MaxY *= FilterYScalingFactor;
        } else {
            MaxX = iSizX;
            MaxY = iSizY;
        }
        if (!MaxX) {
            MaxX = width;
            if(dxw.FilterXScalingFactor) MaxX *= FilterXScalingFactor;
            if(dxw.FilterYScalingFactor) MaxY *= FilterYScalingFactor;
        }
        if (!MaxY) {
            MaxY = height;
            if(dxw.FilterXScalingFactor) MaxX *= FilterXScalingFactor;
            if(dxw.FilterYScalingFactor) MaxY *= FilterYScalingFactor;
        }
        GetMonitorWorkarea(&desktop, TRUE);
        rect.left =  (desktop.right + desktop.left - MaxX) / 2; // v2.03.89 - fixed
        rect.top = (desktop.bottom + desktop.top - MaxY) / 2;	// v2.03.89 - fixed
        rect.right = rect.left + MaxX;
        rect.bottom = rect.top + MaxY; //v2.02.09
        // fixed ....
        if(rect.left < desktop.left)		rect.left = desktop.left;
        if(rect.top < desktop.top)			rect.top = desktop.top;
        if(rect.bottom > desktop.bottom)	rect.bottom = desktop.bottom;
        if(rect.right > desktop.right)		rect.right = desktop.right;
        break;
    case DXW_DESKTOP_WORKAREA:
        label = "workarea";
        GetMonitorWorkarea(&rect, TRUE);
        if(dxw.dwFlags2 & KEEPASPECTRATIO) dxw.FixWorkarea(&rect);
        break;
    case DXW_DESKTOP_FULL:
        label = "desktop";
        GetMonitorWorkarea(&rect, FALSE);
        if(dxw.dwFlags2 & KEEPASPECTRATIO) dxw.FixWorkarea(&rect);
        break;
    case DXW_SET_COORDINATES:
    default:
        label = "coord";
        // set size
        if(bAutoScale) {
            MaxX = GetScreenWidth();
            MaxY = GetScreenHeight();
            if(dxw.FilterXScalingFactor) MaxX *= FilterXScalingFactor;
            if(dxw.FilterYScalingFactor) MaxY *= FilterYScalingFactor;
        } else {
            if((dxw.dwFlags2 & LOCKEDSIZE) || (dxw.dwFlags7 & ANCHORED)) {
                iSizX = iSiz0X;
                iSizY = iSiz0Y;
            }
            MaxX = iSizX;
            MaxY = iSizY;
        }
        if (!MaxX) {
            MaxX = width;
            if(dxw.FilterXScalingFactor) MaxX *= FilterXScalingFactor;
        }
        if (!MaxY) {
            MaxY = height;
            if(dxw.FilterYScalingFactor) MaxY *= FilterYScalingFactor;
        }
        // set pos
        if(dxw.dwFlags7 & ANCHORED) {
            iPosX = iPos0X;
            iPosY = iPos0Y;
        }
        rect.left = iPosX;
        rect.top = iPosY; //v2.02.09
        rect.right = iPosX + MaxX;
        rect.bottom = iPosY + MaxY; //v2.02.09
        break;
    }
    OutDebugDW("%s: coord=%d(%s) client rect=(%d,%d)-(%d,%d)\n",
               ApiRef, Coordinates, label, rect.left, rect.top, rect.right, rect.bottom);
    //v2.04.28: update window coordinates to make cursor exact calculations in fake-fullscreen modes
    iPosX = rect.left;
    iPosY = rect.top;
    iSizX = rect.right - rect.left;
    iSizY = rect.bottom - rect.top;
    if(hwnd) {
        RECT UnmappedRect;
        RECT Borders;
        UnmappedRect = rect;
        dwStyle = (*pGetWindowLong)(hwnd, GWL_STYLE);
        dwExStyle = (*pGetWindowLong)(hwnd, GWL_EXSTYLE);
        // BEWARE: from MSDN -  If the window is a child window, the return value is undefined.
        hMenu = (dwStyle & WS_CHILD) ? NULL : GetMenu(hwnd);
        (*pAdjustWindowRectEx)(&rect, dwStyle, (hMenu != NULL), dwExStyle);
        // get window borders
        Borders.left = UnmappedRect.left - rect.left;
        Borders.right = rect.right - UnmappedRect.right;
        Borders.top = UnmappedRect.top - rect.top;
        Borders.bottom = rect.bottom - UnmappedRect.bottom;
        OutDebugDW("%s: borders=(%d,%d)-(%d,%d)\n", ApiRef, Borders.left, Borders.top, Borders.right, Borders.bottom);
        // if (hMenu) __try {CloseHandle(hMenu);} __except(EXCEPTION_EXECUTE_HANDLER){};
        switch(dxw.Coordinates) {
        case DXW_DESKTOP_WORKAREA:
        case DXW_DESKTOP_FULL:
            // if there's a menu, reduce height to fit area
            if(rect.top != UnmappedRect.top)
                rect.bottom = rect.bottom - UnmappedRect.top + rect.top;
            break;
        default:
            break;
        }
        // shift down-right so that the border is visible
        if(rect.left < dxw.VirtualDesktop.left) {
            rect.right = dxw.VirtualDesktop.left - rect.left + rect.right;
            rect.left = dxw.VirtualDesktop.left;
        }
        if(rect.top < dxw.VirtualDesktop.top) {
            rect.bottom = dxw.VirtualDesktop.top - rect.top + rect.bottom;
            rect.top = dxw.VirtualDesktop.top;
        }
        // shift up-left (or shrink) so that the window doesn't exceed on the other side
        // v2.04.34: fixed logic: if shift brings the other coordinate to negative range, then reduce the size.
        if(rect.right > dxw.VirtualDesktop.right) {
            rect.left = dxw.VirtualDesktop.right - rect.right + rect.left;
            if(rect.left < dxw.VirtualDesktop.left) rect.left = dxw.VirtualDesktop.left;
            rect.right = dxw.VirtualDesktop.right;
        }
        if(rect.bottom > dxw.VirtualDesktop.bottom) {
            rect.top = dxw.VirtualDesktop.bottom - rect.bottom + rect.top;
            if(rect.top < dxw.VirtualDesktop.top) rect.top = dxw.VirtualDesktop.top;
            rect.bottom = dxw.VirtualDesktop.bottom;
        }
        // v2.04.35: added NOTASKBAROVERLAP mode
        if(dxw.dwFlags9 & NOTASKBAROVERLAP) {
            switch(dxw.Coordinates) {
            case DXW_DESKTOP_CENTER:
            case DXW_SET_COORDINATES:
                RECT rTaskBar;
                int pos;
                pos = GetTaskbarRect(&rTaskBar);
                if(pos != POS_NOWHERE) {
                    RECT rIntersect;
                    IntersectRect(&rIntersect, &rTaskBar, &rect);
                    OutDebugDW("%s: Intersection=(%d,%d)-(%d,%d)\n",
                               ApiRef, rIntersect.left, rIntersect.top, rIntersect.right, rIntersect.bottom);
                    if(!IsEmpty(rIntersect)) {
                        switch(pos) {
                        case POS_BOTTOM:
                            if(rect.bottom > rIntersect.top) rect.bottom = rIntersect.top;
                            break;
                        case POS_TOP:
                            if(rect.top < rIntersect.bottom) rect.top = rIntersect.bottom;
                            break;
                        case POS_LEFT:
                            if(rect.left < rIntersect.right) rect.left = rIntersect.right;
                            break;
                        case POS_RIGHT:
                            if(rect.right > rIntersect.left) rect.right = rIntersect.left;
                            break;
                        }
                    }
                }
                break;
            default:
                break;
            }
        }
        //v2.04.28: update window coordinates to make cursor exact calculations
        iPosX = rect.left + Borders.left;
        iPosY = rect.top + Borders.top;
        iSizX = rect.right - rect.left - (Borders.left + Borders.right);
        iSizY = rect.bottom - rect.top - (Borders.top + Borders.bottom);
    }
    OutDebugDW("%s: coord=%d(%s) window rect=(%d,%d)-(%d,%d) client pos=(%d,%d) siz=(%dx%d)\n",
               ApiRef, Coordinates,
               label, rect.left, rect.top, rect.right, rect.bottom,
               iPosX, iPosY, iSizX, iSizY);
    // update the arguments for the window creation
    wp->x = rect.left;
    wp->y = rect.top;
    wp->cx = rect.right - rect.left;
    wp->cy = rect.bottom - rect.top;
}

void dxwCore::AutoScale() {
    ApiName("dxwCore::AutoScale");
    WINDOWPOS wp;
    if(!dxw.Windowize) return;
    CalculateWindowPos(hWnd, dwScreenWidth, dwScreenHeight, &wp);
    OutTrace("%s: new pos=(%d,%d) size=(%dx%d)\n", ApiRef, wp.x, wp.y, wp.cx, wp.cy);
    if(!(*pSetWindowPos)(hWnd, 0, wp.x, wp.y, wp.cx, wp.cy, 0))
        OutTraceE("%s: SetWindowPos ERROR err=%d at %d\n", ApiRef, GetLastError(), __LINE__);
    // need to ShowWindow(SW_SHOW) to make new coordinates active
    (*pShowWindow)(hWnd, SW_SHOW);
    UpdateDesktopCoordinates();
}

HWND dxwCore::CreateVirtualDesktop() {
    ApiName("dxwCore::CreateVirtualDesktop");
    HWND hDesktopWindow;
    HINSTANCE hinst = NULL;
    HWND hParent;
    hParent = (*pGetDesktopWindow)();
    hDesktopWindow = (*pCreateWindowExA)(0, "Static", "DxWnd Desktop", WS_OVERLAPPEDWINDOW, 0, 0, 0, 0, hParent, NULL, hinst, NULL);
    if(hDesktopWindow) {
        dxw.AdjustWindowFrame(hDesktopWindow, dxw.GetScreenWidth(), dxw.GetScreenHeight());
        OutTraceDW("%s: created desktop emulation: hwnd=%#x\n", ApiRef, hDesktopWindow);
        return hDesktopWindow;
    } else {
        OutTraceE("%s: CreateWindowEx ERROR: err=%d at %d\n", ApiRef, GetLastError(), __LINE__);
        return NULL;
    }
}

void dxwCore::ScaleRelMouse(char *api, LPPOINT point) {
    static POINT lastup = {0, 0}; // last calculated mouse real (unscaled) position
    static POINT lastmp = {0, 0}; // last mapped (scaled) mouse virtual position
    POINT newup, newmp, prev;
    memcpy(&prev, point, sizeof(POINT)); // prev = *point;
    newup.y = lastup.y + prev.y;
    newup.x = lastup.x + prev.x;
    memcpy(&newmp, &newup, sizeof(POINT)); // newmp = newup;
    dxw.UnmapClient(&newmp);
    point->x = newmp.x - lastmp.x;
    point->y = newmp.y - lastmp.y;
    OutDebugIN("%s: REL mousedata fixed rel=(%d,%d)->(%d,%d) abs=(%d,%d)\n",
               api, prev.x, prev.y, point->x, point->y, newmp.x, newmp.y);
    memcpy(&lastmp, &newmp, sizeof(POINT)); // lastmp = newmp;
    memcpy(&lastup, &newup, sizeof(POINT)); // lastup = newup;
}

HRGN dxwCore::MapRegion(char *api, HRGN hrgn) {
    HRGN hrgnScaled;
    LPRGNDATA lpRgnData;
    size_t size;
    DWORD nCount;
    LPRECT lpRect;
    if(hrgn == 0) return 0; // don't scale NULL region
    size = GetRegionData(hrgn, 0, NULL);
    lpRgnData = (LPRGNDATA)malloc(size);
    GetRegionData(hrgn, size, lpRgnData);
    OutTraceSYS("%s: region\n", ApiRef);
    OutTraceSYS("> dwSize=%d\n", lpRgnData->rdh.dwSize);
    OutTraceSYS("> iType=%d(%s)\n", lpRgnData->rdh.iType, lpRgnData->rdh.iType == RDH_RECTANGLES ? "RECTANGLES" : "unknown");
    OutTraceSYS("> nCount=%d\n", lpRgnData->rdh.nCount);
    OutTraceSYS("> nRgnSize=%d\n", lpRgnData->rdh.nRgnSize);
    OutTraceSYS("> rcBound=(%d,%d)-(%d,%d)\n",
                lpRgnData->rdh.rcBound.left,
                lpRgnData->rdh.rcBound.top,
                lpRgnData->rdh.rcBound.right,
                lpRgnData->rdh.rcBound.bottom
               );
    lpRect = (LPRECT)(&lpRgnData->Buffer);
    nCount = lpRgnData->rdh.nCount;
    dxw.MapClient(&(lpRgnData->rdh.rcBound));
    for(DWORD i = 0; i < nCount; i++) {
        OutDebugSYS("> rect[%d]=(%d,%d)-(%d,%d)\n", i, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom);
        dxw.MapClient(lpRect);
        lpRect++;
    }
    hrgnScaled = ExtCreateRegion(NULL, size, lpRgnData);
    _if(!hrgnScaled) OutTraceE("%s: ExtCreateRegion ERROR err=%d\n", ApiRef, GetLastError());
    free(lpRgnData);
    return hrgnScaled;
}

HRGN dxwCore::UnmapRegion(char *api, HRGN hrgn) {
    HRGN hrgnScaled;
    LPRGNDATA lpRgnData;
    size_t size;
    DWORD nCount;
    LPRECT lpRect;
    size = GetRegionData(hrgn, 0, NULL);
    lpRgnData = (LPRGNDATA)malloc(size);
    GetRegionData(hrgn, size, lpRgnData);
    OutTraceSYS("%s: region\n", ApiRef);
    OutTraceSYS("> dwSize=%d\n", lpRgnData->rdh.dwSize);
    OutTraceSYS("> iType=%d(%s)\n", lpRgnData->rdh.iType, lpRgnData->rdh.iType == RDH_RECTANGLES ? "RECTANGLES" : "unknown");
    OutTraceSYS("> nCount=%d\n", lpRgnData->rdh.nCount);
    OutTraceSYS("> nRgnSize=%d\n", lpRgnData->rdh.nRgnSize);
    OutTraceSYS("> rcBound=(%d,%d)-(%d,%d)\n",
                lpRgnData->rdh.rcBound.left,
                lpRgnData->rdh.rcBound.top,
                lpRgnData->rdh.rcBound.right,
                lpRgnData->rdh.rcBound.bottom
               );
    lpRect = (LPRECT)(&lpRgnData->Buffer);
    nCount = lpRgnData->rdh.nCount;
    dxw.UnmapClient(&(lpRgnData->rdh.rcBound));
    for(DWORD i = 0; i < nCount; i++) {
        OutDebugSYS("> rect[%d]=(%d,%d)-(%d,%d)\n", i, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom);
        dxw.UnmapClient(lpRect);
        lpRect++;
    }
    hrgnScaled = ExtCreateRegion(NULL, size, lpRgnData);
    _if(!hrgnScaled) OutTraceE("%s: ExtCreateRegion ERROR err=%d\n", ApiRef, GetLastError());
    free(lpRgnData);
    return hrgnScaled;
}
