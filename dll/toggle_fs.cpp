#define _WIN32_WINNT 0x0600
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_DEPRECATE 1

#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include "dxwnd.h"
#include "dxwcore.hpp"
#include "dxhelper.h"

extern void RecoverScreenMode();
extern void RestoreDDrawSurfaces();
extern void RestoreD3DSurfaces(BOOL);

void dx_FullScreenToggle(HWND hwnd) {
    static BOOL bFullScreen = FALSE;
    static RECT WinRect = {0, 0, 0, 0};
    static DWORD OldStyle, OldExtStyle;
    static DEVMODE oldDisplayMode;
    static DWORD OrigFlags;
    static char szDevice[32];
    static int pPosX, pPosY, pSizX, pSizY;
    static short PrevCoordinates;
    static int PrevMonitorId;
    if(!pGetMonitorInfoA) pGetMonitorInfoA =::GetMonitorInfoA;
    // toggle ....
    if (bFullScreen) {
        OutTraceDW("DxWnd: exiting fullscreen mode: style=%#x extstyle=%#x pos=(%d,%d)-(%d,%d)\n",
                   OldStyle, OldExtStyle, WinRect.left, WinRect.top, WinRect.right, WinRect.bottom);
        // recover monitor mode
        int ChangeDisplayResult = (*pChangeDisplaySettingsExA)(szDevice, &oldDisplayMode, NULL, CDS_FULLSCREEN, NULL);
        if(ChangeDisplayResult != DISP_CHANGE_SUCCESSFUL) {
            OutTraceE("ChangeDisplaySettingsEx ERROR: res=%d at %d\n", ChangeDisplayResult, __LINE__);
            MessageBox(NULL, "Error: Failed to recover display mode.", "Error", 0);
        }
        // recover status
        dxw.Windowize = TRUE;
        dxw.Coordinates = PrevCoordinates;
        dxw.MonitorId = PrevMonitorId;
        dxw.iPosX = pPosX;
        dxw.iPosY = pPosY;
        dxw.iSizX = pSizX;
        dxw.iSizY = pSizY;
        dxw.FixWindowFrame(hwnd);
        dxw.iPosX = pPosX;
        dxw.iPosY = pPosY;
        dxw.iSizX = pSizX;
        dxw.iSizY = pSizY;
        dxw.AdjustWindowPos(hwnd, dxw.iSizX, dxw.iSizY);
    } else {
        OutTraceDW("DxWnd: entering fullscreen mode\n");
        // save status
        PrevCoordinates = dxw.Coordinates;
        PrevMonitorId = dxw.MonitorId;
        pPosX = dxw.iPosX;
        pPosY = dxw.iPosY;
        pSizX = dxw.iSizX;
        pSizY = dxw.iSizY;
        // set new mode
        dxw.Coordinates = DXW_DESKTOP_FULL;
        dxw.Windowize = FALSE;
        dxw.MonitorId = dxw.PickMonitorId();
        dxw.iPosX = dxw.iPosY = 0;
        dxw.iSizX = dxw.GetScreenWidth();
        dxw.iSizY = dxw.GetScreenHeight();
        // switch vide mode
        int BestIndex, iCost, iBestCost;
        DEVMODE DisplayMode;
        HMONITOR hBestMonitor;
        hBestMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
        MONITORINFOEX mi;
        memset(&mi, 0, sizeof(mi));
        mi.cbSize = sizeof(mi);
        (*pGetMonitorInfoA)(hBestMonitor, &mi);
        OutTraceDW("Using monitor=\"%s\", rect=(%d,%d)-(%d,%d) type=%s\n",
                   mi.szDevice,
                   mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right, mi.rcMonitor.bottom,
                   (mi.dwFlags & MONITORINFOF_PRIMARY) ? "PRIMARY" : "SECONDARY");
        memset(&oldDisplayMode, 0, sizeof(DEVMODE));
        if(!(*pEnumDisplaySettings)(mi.szDevice, ENUM_CURRENT_SETTINGS, &oldDisplayMode))
            MessageBox(NULL, "EnumDisplaySettings Failed ???", "Error!", 0);
        iBestCost = 1000000; // huge
        for (int i = 0; ; i++) {
            iCost = 0;
            memset(&DisplayMode, 0, sizeof(DEVMODE));
            DisplayMode.dmSize = sizeof(DEVMODE);
            if(!(*pEnumDisplaySettings)(mi.szDevice, i, &DisplayMode))break; // no more modes
            if(DisplayMode.dmPelsWidth < dxw.GetScreenWidth()) continue; // bad: too narrow
            if(DisplayMode.dmPelsHeight < dxw.GetScreenHeight()) continue; // bad: too low
            if (DisplayMode.dmBitsPerPel != oldDisplayMode.dmBitsPerPel) continue; // bad: different color depth
            iCost =
                (DisplayMode.dmPelsWidth - dxw.GetScreenWidth()) +
                (DisplayMode.dmPelsHeight - dxw.GetScreenHeight()) +
                (DisplayMode.dmDisplayFrequency == oldDisplayMode.dmDisplayFrequency) ? 1 : 0;
            if(iCost < iBestCost) {
                iBestCost = iCost;
                BestIndex = i;
            }
            if(iBestCost == 0) break; // got the perfect one!
        }
        memset(&DisplayMode, 0, sizeof(DEVMODE));
        strncpy(szDevice, mi.szDevice, 32);
        (*pEnumDisplaySettings)(mi.szDevice, BestIndex, &DisplayMode);
        OutTraceDW("DxWnd: selected mode bpp=%d size=(%dx%d) freq=%d\n",
                   DisplayMode.dmBitsPerPel, DisplayMode.dmPelsWidth, DisplayMode.dmPelsHeight, DisplayMode.dmDisplayFrequency);
        DisplayMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFLAGS | DM_DISPLAYFREQUENCY | DM_POSITION;
        int ChangeDisplayResult = (*pChangeDisplaySettingsExA)(mi.szDevice, &DisplayMode, NULL, CDS_FULLSCREEN, NULL);
        if(ChangeDisplayResult != DISP_CHANGE_SUCCESSFUL) {
            OutTraceE("ChangeDisplaySettingsEx ERROR: res=%d at %d\n", ChangeDisplayResult, __LINE__);
            MessageBox(NULL, "Error: Failed to change display mode.", "Error", 0);
        }
        // query again the NEW screen coordinates ....
        memset(&mi, 0, sizeof(mi));
        mi.cbSize = sizeof(mi);
        (*pGetMonitorInfoA)(hBestMonitor, &mi);
        OutTraceDW("Updated monitor=\"%s\", rect=(%d,%d)-(%d,%d) type=%s\n",
                   mi.szDevice,
                   mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right, mi.rcMonitor.bottom,
                   (mi.dwFlags & MONITORINFOF_PRIMARY) ? "PRIMARY" : "SECONDARY");
        // now reposition the window coordinates
        dxw.iPosX = mi.rcMonitor.left;
        dxw.iPosY = mi.rcMonitor.top;
        //// enforce the change ... ????
        //(*pMoveWindow)(hwnd,
        //	mi.rcMonitor.left, mi.rcMonitor.top,	// x, y
        //	DisplayMode.dmPelsWidth,				// width
        //	DisplayMode.dmPelsHeight,				// height
        //	TRUE);
        // activate new mode
        dxw.FixWindowFrame(hwnd);
        dxw.AdjustWindowPos(hwnd, dxw.iSizX, dxw.iSizY);
    }
    (*pUpdateWindow)(hwnd);
    RestoreDDrawSurfaces();
    //RestoreD3DSurfaces(bFullScreen);
    bFullScreen = !bFullScreen; // switch toggle
}

void dx_DesktopToggle(HWND hwnd, BOOL bWorkArea) {
    static BOOL bDesktopToggle = FALSE;
    static int pPosX, pPosY, pSizX, pSizY;
    static short PrevCoordinates;
    static int PrevMonitorId;
    if (bDesktopToggle) { 	// toggle ....
        OutTraceDW("DxWnd: exiting desktop mode: mode=%#x pos=(%d,%d) size=(%dx%d)\n",
                   PrevCoordinates, pPosX, pPosY, pSizX, pSizY);
        // recover status
        dxw.Coordinates = PrevCoordinates;
        dxw.MonitorId = PrevMonitorId;
        dxw.iPosX = pPosX;
        dxw.iPosY = pPosY;
        dxw.iSizX = pSizX;
        dxw.iSizY = pSizY;
        dxw.FixWindowFrame(hwnd);
        dxw.iPosX = pPosX;
        dxw.iPosY = pPosY;
        dxw.iSizX = pSizX;
        dxw.iSizY = pSizY;
        dxw.AdjustWindowPos(hwnd, dxw.iSizX, dxw.iSizY);
    } else {
        OutTraceDW("DxWnd: entering desktop mode: mode=%#x pos=(%d,%d) size=(%dx%d)\n",
                   dxw.Coordinates, dxw.iPosX, dxw.iPosY, dxw.iSizX, dxw.iSizY);
        // save status
        PrevCoordinates = dxw.Coordinates;
        PrevMonitorId = dxw.MonitorId;
        pPosX = dxw.iPosX;
        pPosY = dxw.iPosY;
        pSizX = dxw.iSizX;
        pSizY = dxw.iSizY;
        // set new toggled status
        dxw.Coordinates = bWorkArea ? DXW_DESKTOP_WORKAREA : DXW_DESKTOP_FULL;
        // pick the target monitor (in multi-monitor configurations)
        dxw.MonitorId = dxw.PickMonitorId();
        // activate new mode
        dxw.FixWindowFrame(hwnd);
        dxw.AdjustWindowPos(hwnd, dxw.iSizX, dxw.iSizY);
    }
    bDesktopToggle = !bDesktopToggle; // switch toggle
    (*pUpdateWindow)(hwnd);
    dxw.ScreenRefresh();
    (*pInvalidateRect)(hwnd, NULL, FALSE); // force window update
}
