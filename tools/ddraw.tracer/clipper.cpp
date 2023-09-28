#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ctype.h>
#include <stdio.h>
#include "dxdiag.h"
#include "ddraw.h"
#include "proxy.h"

// DirectDrawClipper
typedef HRESULT (WINAPI *QueryInterfaceC_Type)(LPDIRECTDRAWCLIPPER, REFIID, LPVOID *);
typedef HRESULT (WINAPI *AddRefC_Type)(LPDIRECTDRAWCLIPPER);
typedef HRESULT (WINAPI *ReleaseC_Type)(LPDIRECTDRAWCLIPPER);
typedef HRESULT (WINAPI *GetClipList_Type)(LPDIRECTDRAWCLIPPER, LPRECT, LPRGNDATA, LPDWORD);
typedef HRESULT (WINAPI *GetHWnd_Type)(LPDIRECTDRAWCLIPPER, HWND FAR *);
typedef HRESULT (WINAPI *InitializeC_Type)(LPDIRECTDRAWCLIPPER, LPDIRECTDRAW, DWORD);
typedef HRESULT (WINAPI *IsClipListChanged_Type)(LPDIRECTDRAWCLIPPER, BOOL FAR *);
typedef HRESULT (WINAPI *SetClipList_Type)(LPDIRECTDRAWCLIPPER, LPRGNDATA, DWORD);
typedef HRESULT (WINAPI *SetHWnd_Type)(LPDIRECTDRAWCLIPPER, DWORD, HWND);

/* DirectDrawClipper hook pointers */
QueryInterfaceC_Type pQueryInterfaceC = NULL;
AddRefC_Type pAddRefC = NULL;
ReleaseC_Type pReleaseC = NULL;
GetClipList_Type pGetClipList = NULL;
GetHWnd_Type pGetHWnd = NULL;
InitializeC_Type pInitializeC = NULL;
IsClipListChanged_Type pIsClipListChanged = NULL;
SetClipList_Type pSetClipList = NULL;
SetHWnd_Type pSetHWnd = NULL;

HRESULT WINAPI extQueryInterfaceC(LPDIRECTDRAWCLIPPER, REFIID, LPVOID *);
ULONG WINAPI extAddRefC(LPDIRECTDRAWCLIPPER);
ULONG WINAPI extReleaseC(LPDIRECTDRAWCLIPPER);
HRESULT WINAPI extGetClipList(LPDIRECTDRAWCLIPPER, LPRECT, LPRGNDATA, LPDWORD);
HRESULT WINAPI extGetHWnd(LPDIRECTDRAWCLIPPER, HWND FAR *);
HRESULT WINAPI extInitializeC(LPDIRECTDRAWCLIPPER, LPDIRECTDRAW, DWORD);
HRESULT WINAPI extIsClipListChanged(LPDIRECTDRAWCLIPPER, BOOL FAR *);
HRESULT WINAPI extSetClipList(LPDIRECTDRAWCLIPPER, LPRGNDATA, DWORD);
HRESULT WINAPI extSetHWnd(LPDIRECTDRAWCLIPPER, DWORD, HWND);

void HookDDClipper(LPDIRECTDRAWCLIPPER FAR *lplpddc) {
    // IDirectDrawClipper::QueryInterface
    SetHook((void *)(**(DWORD **)lplpddc + 0), extQueryInterfaceC, (void **)&pQueryInterfaceC, "QueryInterface(C)");
    // IDirectDrawClipper::AddRef
    SetHook((void *)(**(DWORD **)lplpddc + 4), extAddRefC, (void **)&pAddRefC, "AddRef(C)");
    // IDirectDrawClipper::Release
    SetHook((void *)(**(DWORD **)lplpddc + 8), extReleaseC, (void **)&pReleaseC, "Release(C)");
    // IDirectDrawClipper::GetClipList
    SetHook((void *)(**(DWORD **)lplpddc + 12), extGetClipList, (void **)&pGetClipList, "GetClipList(C)");
    // IDirectDrawClipper::GetHWnd
    SetHook((void *)(**(DWORD **)lplpddc + 16), extGetHWnd, (void **)&pGetHWnd, "GetHWnd(C)");
    // IDirectDrawClipper::Initialize
    SetHook((void *)(**(DWORD **)lplpddc + 20), extInitializeC, (void **)&pInitializeC, "Initialize(C)");
    // IDirectDrawClipper::IsClipListChanged
    SetHook((void *)(**(DWORD **)lplpddc + 24), extIsClipListChanged, (void **)&pIsClipListChanged, "IsClipListChanged(C)");
    // IDirectDrawClipper::SetClipList
    SetHook((void *)(**(DWORD **)lplpddc + 28), extSetClipList, (void **)&pSetClipList, "SetClipList(C)");
    // IDirectDrawClipper::SetHWnd
    SetHook((void *)(**(DWORD **)lplpddc + 32), extSetHWnd, (void **)&pSetHWnd, "SetHWnd(C)");
}

HRESULT WINAPI extQueryInterfaceC(LPDIRECTDRAWCLIPPER lpddc, REFIID riid, LPVOID *obp) {
    HRESULT res;
    //int version;
    trace(">> Clipper::QueryInterface(lpdd=%x refiid=%s)\n", lpddc, sGUID((GUID *)&riid));
    res = (*pQueryInterfaceC)(lpddc, riid, obp);
    trace("<< Clipper::QueryInterface(obp=%x) res=%x(%s)\n", *obp, res, ExplainDDError(res));
    //version = GUIDClipperVersion((GUID *)&riid, NULL);
    //HookClipSession((LPDIRECTDRAW *)obp, dwLocalDDVersion);
    return res;
}

ULONG WINAPI extAddRefC(LPDIRECTDRAWCLIPPER lpddc) {
    ULONG res;
    trace(">> Clipper::AddRef(lpddc=%x)\n", lpddc);
    res = (*pAddRefC)(lpddc);
    trace("<< Clipper::AddRef res(refcount)=%x\n", res);
    return res;
}

ULONG WINAPI extReleaseC(LPDIRECTDRAWCLIPPER lpddc) {
    ULONG res;
    trace(">> Clipper::Release(lpddc=%x)\n", lpddc);
    res = (*pReleaseC)(lpddc);
    trace("<< Clipper::Release res(refcount)=%x\n", res);
    return res;
}

HRESULT WINAPI extGetClipList(LPDIRECTDRAWCLIPPER lpddc, LPRECT lpRect, LPRGNDATA lpRgnData, LPDWORD lpw) {
    HRESULT res;
    trace(">> Clipper::GetClipList(lpddc=%x)\n", lpddc);
    res = (*pGetClipList)(lpddc, lpRect, lpRgnData, lpw);
    trace("<< Clipper::GetClipList res=%x(%s)\n", res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extGetHWnd(LPDIRECTDRAWCLIPPER lpddc, HWND FAR *lphwnd) {
    HRESULT res;
    trace(">> Clipper::GetHWnd(lpddc=%x)\n", lpddc);
    res = (*pGetHWnd)(lpddc, lphwnd);
    trace("<< Clipper::GetHWnd(hwnd=%x) res=%x(%s)\n", *lphwnd, res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extInitializeC(LPDIRECTDRAWCLIPPER lpddc, LPDIRECTDRAW lpdd, DWORD w) {
    HRESULT res;
    trace(">> Clipper::Initialize(lpddc=%x lpdd=%x w=%x)\n", lpddc, lpdd, w);
    res = (*pInitializeC)(lpddc, lpdd, w);
    trace("<< Clipper::Initialize res=%x(%s)\n", res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extIsClipListChanged(LPDIRECTDRAWCLIPPER lpddc, BOOL FAR *b) {
    HRESULT res;
    trace(">> Clipper::IsClipListChanged(lpddc=%x)\n", lpddc);
    res = (*pIsClipListChanged)(lpddc, b);
    trace("<< Clipper::IsClipListChanged(b=%x) res=%x(%s)\n", *b, res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extSetClipList(LPDIRECTDRAWCLIPPER lpddc, LPRGNDATA lpRegnData, DWORD dwflags) {
    HRESULT res;
    trace(">> Clipper::SetClipList(lpddc=%x flags=%x)\n", lpddc, dwflags);
    res = (*pSetClipList)(lpddc, lpRegnData, dwflags);
    trace("<< Clipper::SetClipList res=%x(%s)\n", res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extSetHWnd(LPDIRECTDRAWCLIPPER lpddc, DWORD w, HWND hwnd) {
    HRESULT res;
    trace(">> Clipper::SetHWnd(lpddc=%x w=%x hwnd=%x)\n", lpddc, w, hwnd);
    res = (*pSetHWnd)(lpddc, w, hwnd);
    trace("<< Clipper::SetHWnd res=%x(%s)\n", res, ExplainDDError(res));
    return res;
}
