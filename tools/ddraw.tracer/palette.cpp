// to do:
// QueryInterface - useful? there are no alternate versions for palette object
// palette detailed dump, as debug option

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ctype.h>
#include <stdio.h>
#include "dxdiag.h"
#include "ddraw.h"
#include "proxy.h"

// DirectDrawPalette
typedef HRESULT (WINAPI *QueryInterfaceP_Type)(LPDIRECTDRAWPALETTE, REFIID, LPVOID *);
typedef ULONG	(WINAPI *AddRefP_Type)(LPDIRECTDRAWPALETTE);
typedef ULONG	(WINAPI *ReleaseP_Type)(LPDIRECTDRAWPALETTE);
typedef HRESULT (WINAPI *GetCapsP_Type)(LPDIRECTDRAWPALETTE, LPDWORD);
typedef HRESULT (WINAPI *GetEntries_Type)(LPDIRECTDRAWPALETTE, DWORD, DWORD, DWORD, LPPALETTEENTRY);
typedef HRESULT (WINAPI *InitializeP_Type)(LPDIRECTDRAWPALETTE, LPDIRECTDRAW, DWORD, LPPALETTEENTRY);
typedef HRESULT (WINAPI *SetEntries_Type)(LPDIRECTDRAWPALETTE, DWORD, DWORD, DWORD, LPPALETTEENTRY);

/* DirectDrawPalette hook pointers */
QueryInterfaceP_Type pQueryInterfaceP = NULL;
AddRefP_Type pAddRefP = NULL;
ReleaseP_Type pReleaseP = NULL;
GetCapsP_Type pGetCapsP = NULL;
GetEntries_Type pGetEntries = NULL;
InitializeP_Type pInitializeP = NULL;
SetEntries_Type pSetEntries = NULL;

// DirectDraw Palette redirectors
HRESULT WINAPI extQueryInterfaceP(LPDIRECTDRAWPALETTE, REFIID, LPVOID *);
ULONG	WINAPI extAddRefP(LPDIRECTDRAWPALETTE);
ULONG	WINAPI extReleaseP(LPDIRECTDRAWPALETTE);
HRESULT WINAPI extGetCapsP(LPDIRECTDRAWPALETTE, LPDWORD);
HRESULT WINAPI extGetEntries(LPDIRECTDRAWPALETTE, DWORD, DWORD, DWORD, LPPALETTEENTRY);
HRESULT WINAPI extInitializeP(LPDIRECTDRAWPALETTE, LPDIRECTDRAW, DWORD, LPPALETTEENTRY);
HRESULT WINAPI extSetEntries(LPDIRECTDRAWPALETTE, DWORD, DWORD, DWORD, LPPALETTEENTRY);

void HookDDPalette(LPDIRECTDRAWPALETTE *lplpddp) {
    // IDirectDrawPalette::QueryInterface
    SetHook((void *)(**(DWORD **)lplpddp + 0), extQueryInterfaceP, (void **)&pQueryInterfaceP, "QueryInterface(P)");
    // IDirectDrawPalette::AddRef
    SetHook((void *)(**(DWORD **)lplpddp + 4), extAddRefP, (void **)&pAddRefP, "AddRef(P)");
    // IDirectDrawPalette::Release
    SetHook((void *)(**(DWORD **)lplpddp + 8), extReleaseP, (void **)&pReleaseP, "Release(P)");
    // IDirectDrawPalette::GetCaps
    SetHook((void *)(**(DWORD **)lplpddp + 12), extGetCapsP, (void **)&pGetCapsP, "GetCaps(P)");
    // IDirectDrawPalette::GetEntries
    SetHook((void *)(**(DWORD **)lplpddp + 16), extGetEntries, (void **)&pGetEntries, "GetEntries(P)");
    // IDirectDrawPalette::Initialize
    SetHook((void *)(**(DWORD **)lplpddp + 20), extInitializeP, (void **)&pInitializeP, "Initialize(P)");
    // IDirectDrawPalette::SetEntries
    SetHook((void *)(**(DWORD **)lplpddp + 24), extSetEntries, (void **)&pSetEntries, "SetEntries(P)");
}

HRESULT WINAPI extQueryInterfaceP(LPDIRECTDRAWPALETTE lpddp, REFIID riid, LPVOID *obp) {
    HRESULT res;
    //int version;
    trace(">> Palette::QueryInterface(lpdd=%x refiid=%s)\n", lpddp, sGUID((GUID *)&riid));
    res = (*pQueryInterfaceP)(lpddp, riid, obp);
    trace("<< Palette::QueryInterface(obp=%x) res=%x(%s)\n", *obp, res, ExplainDDError(res));
    //version = GUIDClipperVersion((GUID *)&riid, NULL);
    //HookClipSession((LPDIRECTDRAW *)obp, dwLocalDDVersion);
    return res;
}

ULONG WINAPI extAddRefP(LPDIRECTDRAWPALETTE lpddp) {
    ULONG res;
    trace(">> Palette::AddRef(lpddp=%x)\n", lpddp);
    res = (*pAddRefP)(lpddp);
    trace("<< Palette::AddRef res(refcount)=%x\n", res);
    return res;
}

ULONG WINAPI extReleaseP(LPDIRECTDRAWPALETTE lpddp) {
    ULONG res;
    trace(">> Palette::Release(lpddp=%x)\n", lpddp);
    res = (*pReleaseP)(lpddp);
    trace("<< Palette::Release res(refcount)=%x\n", res);
    return res;
}

HRESULT WINAPI extGetCapsP(LPDIRECTDRAWPALETTE lpddp, LPDWORD lpdwflags) {
    HRESULT res;
    trace(">> Palette::GetCaps(lpddp=%x)\n", lpddp);
    res = (*pGetCapsP)(lpddp, lpdwflags);
    trace("<< Palette::GetCaps(flags=%x) res=%x(%s)\n", *lpdwflags, res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extGetEntries(LPDIRECTDRAWPALETTE lpddp, DWORD dwflags, DWORD dwstart, DWORD dwcount, LPPALETTEENTRY lpentries) {
    HRESULT res;
    trace(">> Palette::GetEntries(lpddp=%x flags=%x start=%d count=%d)\n", lpddp, dwflags, dwstart, dwcount);
    res = (*pGetEntries)(lpddp, dwflags, dwstart, dwcount, lpentries);
    trace("<< Palette::GetEntries res=%x(%s)\n", res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extInitializeP(LPDIRECTDRAWPALETTE lpddp, LPDIRECTDRAW lpdd, DWORD dwflags, LPPALETTEENTRY lpentries) {
    HRESULT res;
    trace(">> Palette::Initialize(lpddp=%x lpdd=%x flags=%x)\n", lpddp, lpdd, dwflags);
    res = (*pInitializeP)(lpddp, lpdd, dwflags, lpentries);
    trace("<< Palette::Initialize res=%x(%s)\n", res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extSetEntries(LPDIRECTDRAWPALETTE lpddp, DWORD dwflags, DWORD dwstart, DWORD dwcount, LPPALETTEENTRY lpentries) {
    HRESULT res;
    trace(">> Palette::SetEntries(lpddp=%x flags=%x start=%d count=%d)\n", lpddp, dwflags, dwstart, dwcount);
    res = (*pSetEntries)(lpddp, dwflags, dwstart, dwcount, lpentries);
    trace("<< Palette::SetEntries res=%x(%s)\n", res, ExplainDDError(res));
    return res;
}
