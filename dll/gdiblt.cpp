#define _CRT_SECURE_NO_WARNINGS
#define INITGUID
//#define FULLHEXDUMP

#include <windows.h>
#include <ddraw.h>
#include "dxwnd.h"
#include "dxhook.h"
#include "ddrawi.h"
#include "dxwcore.hpp"
#include "stdio.h"
#include "hddraw.h"
#include "dxhelper.h"
#include "syslibs.h"

extern GetDC_Type pGetDCMethod();
extern ReleaseDC_Type pReleaseDCMethod();

HRESULT BlitToWindow(int dxversion, Blt_Type pBlt, char *api, LPDIRECTDRAWSURFACE lpdds, LPRECT lpdestrect,
                     LPDIRECTDRAWSURFACE s, LPRECT lpsrcrect, DWORD dwflags, LPDDBLTFX lpddbltfx, BOOL isFlipping) {
    HDC shdc, thdc;
    RECT client;
    HRESULT res;
    BOOL ret;
    HWND w;
    RECT dstrect;
    res = (*pBlt)(lpdds, lpdestrect, s, lpsrcrect, dwflags, lpddbltfx);
    if(res != DD_OK) {
        OutTraceE("gdiblt: Blt ERROR err=%#x(%s)\n", res, ExplainDDError(res));
        if(res == DDERR_UNSUPPORTED) {
            // do not desist, try blitting directly from source
            lpdds = s;
        } else
            return res;
    }
    w = dxw.GethWnd();
    res = (*pGetDCMethod())(lpdds, &shdc);
    if(res) {
        OutTraceE("ddraw GetDC error lpdds=%#x res=%#x(%s)\n", lpdds, res, ExplainDDError(res));
        return DD_FALSE;
    }
    thdc = (*pGDIGetDC)(w);
    if(!thdc) {
        OutTraceE("GDI GetDC error=%d\n", GetLastError());
        return DD_FALSE;
    }
    if(dxw.dwFlags5 & BILINEARFILTER) {
        ret = SetStretchBltMode(thdc, HALFTONE);
#ifndef DXW_NOTRACES
        if((!ret) || (ret == ERROR_INVALID_PARAMETER)) OutTraceE("GDI SetStretchBltMode error=%d\n", GetLastError());
#endif // DXW_NOTRACES
    }
    if(dxw.dwDFlags & CENTERTOWIN) {
        int x, y;
        client = dxw.MapClientRect(NULL);
        x = (client.left + client.right - dxw.GetScreenWidth()) >> 1; // right-shift 1 bit means divide by 2!
        y = (client.top + client.bottom - dxw.GetScreenHeight()) >> 1;
        ret = (*pGDIBitBlt)(thdc, x, y, dxw.GetScreenWidth(), dxw.GetScreenHeight(), shdc, 0, 0, SRCCOPY);
#ifndef DXW_NOTRACES
        if(!ret) OutTraceE("BitBlt error=%d\n", GetLastError());
#endif // DXW_NOTRACES
    } else {
        dstrect = dxw.MapClientRect(NULL);
        ret = (*pGDIStretchBlt)(thdc,
                                dstrect.left, dstrect.top, dstrect.right - dstrect.left, dstrect.bottom - dstrect.top,
                                shdc, 0, 0, dxw.GetScreenWidth(), dxw.GetScreenHeight(), SRCCOPY);
#ifndef DXW_NOTRACES
        if(!ret) OutTraceE("GDI StretchBlt error=%d\n", GetLastError());
#endif // DXW_NOTRACES
    }
    dxw.ShowOverlay(thdc);
    res = (*pReleaseDCMethod())(lpdds, shdc);
#ifndef DXW_NOTRACES
    if(res) OutTraceE("ddraw ReleaseDC error lpdds=%#x res=%#x(%s)\n", lpdds, res, ExplainDDError(res));
#endif // DXW_NOTRACES
    ret = (*pGDIReleaseDC)(w, thdc);
#ifndef DXW_NOTRACES
    if(!ret) OutTraceE("GDI ReleaseDC error=%d\n", GetLastError());
#endif // DXW_NOTRACES
    return DD_OK;
}