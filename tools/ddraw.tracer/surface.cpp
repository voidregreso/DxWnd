#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ctype.h>
#include <stdio.h>
#include "dxdiag.h"
#include "ddraw.h"
#include "proxy.h"

// COM generic types
typedef HRESULT (WINAPI *QueryInterface_Type)(void *, REFIID, LPVOID *);
typedef ULONG	(WINAPI *AddRef_Type)(LPVOID);
typedef ULONG	(WINAPI *Release_Type)(LPVOID);

// DirectDrawSurface COM methods
typedef ULONG	(WINAPI *AddRefS_Type)(LPDIRECTDRAWSURFACE);
typedef ULONG	(WINAPI *ReleaseS_Type)(LPDIRECTDRAWSURFACE);
typedef HRESULT (WINAPI *AddAttachedSurface_Type)(LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE);
typedef HRESULT (WINAPI *Blt_Type)(LPDIRECTDRAWSURFACE, LPRECT, LPDIRECTDRAWSURFACE, LPRECT, DWORD, LPDDBLTFX);
typedef HRESULT (WINAPI *BltFast_Type)(LPDIRECTDRAWSURFACE, DWORD, DWORD, LPDIRECTDRAWSURFACE, LPRECT, DWORD);
typedef HRESULT (WINAPI *Flip_Type)(LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE, DWORD);

AddRefS_Type pAddRefS1, pAddRefS2, pAddRefS3, pAddRefS4, pAddRefS7;
ReleaseS_Type pReleaseS1, pReleaseS2, pReleaseS3, pReleaseS4, pReleaseS7;
AddAttachedSurface_Type pAddAttachedSurface = NULL;
QueryInterface_Type pQueryInterfaceS1, pQueryInterfaceS2, pQueryInterfaceS3, pQueryInterfaceS4, pQueryInterfaceS7;
Blt_Type pBlt1, pBlt2, pBlt3, pBlt4, pBlt7;
BltFast_Type pBltFast1, pBltFast2, pBltFast3, pBltFast4, pBltFast7;
Flip_Type pFlip1 = NULL;
Flip_Type pFlip2 = NULL;
Flip_Type pFlip3 = NULL;
Flip_Type pFlip4 = NULL;
Flip_Type pFlip7 = NULL;

// DirectDrawSurface redirectors
ULONG WINAPI extAddRefS1(LPDIRECTDRAWSURFACE);
ULONG WINAPI extAddRefS2(LPDIRECTDRAWSURFACE);
ULONG WINAPI extAddRefS3(LPDIRECTDRAWSURFACE);
ULONG WINAPI extAddRefS4(LPDIRECTDRAWSURFACE);
ULONG WINAPI extAddRefS7(LPDIRECTDRAWSURFACE);
ULONG WINAPI extReleaseS1(LPDIRECTDRAWSURFACE);
ULONG WINAPI extReleaseS2(LPDIRECTDRAWSURFACE);
ULONG WINAPI extReleaseS3(LPDIRECTDRAWSURFACE);
ULONG WINAPI extReleaseS4(LPDIRECTDRAWSURFACE);
ULONG WINAPI extReleaseS7(LPDIRECTDRAWSURFACE);
HRESULT WINAPI extQueryInterfaceS1(void *, REFIID, LPVOID *);
HRESULT WINAPI extQueryInterfaceS2(void *, REFIID, LPVOID *);
HRESULT WINAPI extQueryInterfaceS3(void *, REFIID, LPVOID *);
HRESULT WINAPI extQueryInterfaceS4(void *, REFIID, LPVOID *);
HRESULT WINAPI extQueryInterfaceS7(void *, REFIID, LPVOID *);
HRESULT WINAPI extBlt1(LPDIRECTDRAWSURFACE, LPRECT, LPDIRECTDRAWSURFACE, LPRECT, DWORD, LPDDBLTFX);
HRESULT WINAPI extBlt2(LPDIRECTDRAWSURFACE, LPRECT, LPDIRECTDRAWSURFACE, LPRECT, DWORD, LPDDBLTFX);
HRESULT WINAPI extBlt3(LPDIRECTDRAWSURFACE, LPRECT, LPDIRECTDRAWSURFACE, LPRECT, DWORD, LPDDBLTFX);
HRESULT WINAPI extBlt4(LPDIRECTDRAWSURFACE, LPRECT, LPDIRECTDRAWSURFACE, LPRECT, DWORD, LPDDBLTFX);
HRESULT WINAPI extBlt7(LPDIRECTDRAWSURFACE, LPRECT, LPDIRECTDRAWSURFACE, LPRECT, DWORD, LPDDBLTFX);
HRESULT WINAPI extBltFast1(LPDIRECTDRAWSURFACE, DWORD, DWORD, LPDIRECTDRAWSURFACE, LPRECT, DWORD);
HRESULT WINAPI extBltFast2(LPDIRECTDRAWSURFACE, DWORD, DWORD, LPDIRECTDRAWSURFACE, LPRECT, DWORD);
HRESULT WINAPI extBltFast3(LPDIRECTDRAWSURFACE, DWORD, DWORD, LPDIRECTDRAWSURFACE, LPRECT, DWORD);
HRESULT WINAPI extBltFast4(LPDIRECTDRAWSURFACE, DWORD, DWORD, LPDIRECTDRAWSURFACE, LPRECT, DWORD);
HRESULT WINAPI extBltFast7(LPDIRECTDRAWSURFACE, DWORD, DWORD, LPDIRECTDRAWSURFACE, LPRECT, DWORD);
HRESULT WINAPI extFlip1(LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE, DWORD);
HRESULT WINAPI extFlip2(LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE, DWORD);
HRESULT WINAPI extFlip3(LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE, DWORD);
HRESULT WINAPI extFlip4(LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE, DWORD);
HRESULT WINAPI extFlip7(LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE, DWORD);

typedef HRESULT (WINAPI *AddAttachedSurface_Type)(LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE);
typedef HRESULT (WINAPI *GetAttachedSurface_Type)(LPDIRECTDRAWSURFACE, DDSCAPS2 *, LPDIRECTDRAWSURFACE *);
typedef HRESULT (WINAPI *DeleteAttachedSurface_Type)(LPDIRECTDRAWSURFACE,  DWORD, LPDIRECTDRAWSURFACE);
typedef HRESULT (WINAPI *EnumAttachedSurfaces_Type)(LPDIRECTDRAWSURFACE, LPVOID, LPDDENUMSURFACESCALLBACK);

HRESULT WINAPI extAddAttachedSurface1(LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE);
HRESULT WINAPI extAddAttachedSurface2(LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE);
HRESULT WINAPI extAddAttachedSurface3(LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE);
HRESULT WINAPI extAddAttachedSurface4(LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE);
HRESULT WINAPI extAddAttachedSurface7(LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE);
HRESULT WINAPI extGetAttachedSurface1(LPDIRECTDRAWSURFACE, LPDDSCAPS, LPDIRECTDRAWSURFACE *);
HRESULT WINAPI extGetAttachedSurface2(LPDIRECTDRAWSURFACE, LPDDSCAPS, LPDIRECTDRAWSURFACE *);
HRESULT WINAPI extGetAttachedSurface3(LPDIRECTDRAWSURFACE, LPDDSCAPS, LPDIRECTDRAWSURFACE *);
HRESULT WINAPI extGetAttachedSurface4(LPDIRECTDRAWSURFACE, LPDDSCAPS2, LPDIRECTDRAWSURFACE *);
HRESULT WINAPI extGetAttachedSurface7(LPDIRECTDRAWSURFACE, LPDDSCAPS2, LPDIRECTDRAWSURFACE *);
HRESULT WINAPI extDeleteAttachedSurface1(LPDIRECTDRAWSURFACE,  DWORD, LPDIRECTDRAWSURFACE);
HRESULT WINAPI extDeleteAttachedSurface2(LPDIRECTDRAWSURFACE,  DWORD, LPDIRECTDRAWSURFACE);
HRESULT WINAPI extDeleteAttachedSurface3(LPDIRECTDRAWSURFACE,  DWORD, LPDIRECTDRAWSURFACE);
HRESULT WINAPI extDeleteAttachedSurface4(LPDIRECTDRAWSURFACE,  DWORD, LPDIRECTDRAWSURFACE);
HRESULT WINAPI extDeleteAttachedSurface7(LPDIRECTDRAWSURFACE,  DWORD, LPDIRECTDRAWSURFACE);
HRESULT WINAPI extEnumAttachedSurfaces1(LPDIRECTDRAWSURFACE, LPVOID, LPDDENUMSURFACESCALLBACK);
HRESULT WINAPI extEnumAttachedSurfaces2(LPDIRECTDRAWSURFACE, LPVOID, LPDDENUMSURFACESCALLBACK);
HRESULT WINAPI extEnumAttachedSurfaces3(LPDIRECTDRAWSURFACE, LPVOID, LPDDENUMSURFACESCALLBACK);
HRESULT WINAPI extEnumAttachedSurfaces4(LPDIRECTDRAWSURFACE, LPVOID, LPDDENUMSURFACESCALLBACK);
HRESULT WINAPI extEnumAttachedSurfaces7(LPDIRECTDRAWSURFACE, LPVOID, LPDDENUMSURFACESCALLBACK);

AddAttachedSurface_Type pAddAttachedSurface1, pAddAttachedSurface2, pAddAttachedSurface3, pAddAttachedSurface4, pAddAttachedSurface7;
DeleteAttachedSurface_Type pDeleteAttachedSurface1, pDeleteAttachedSurface2, pDeleteAttachedSurface3, pDeleteAttachedSurface4, pDeleteAttachedSurface7;
EnumAttachedSurfaces_Type pEnumAttachedSurfaces1, pEnumAttachedSurfaces2, pEnumAttachedSurfaces3, pEnumAttachedSurfaces4, pEnumAttachedSurfaces7;
GetAttachedSurface_Type pGetAttachedSurface1, pGetAttachedSurface2, pGetAttachedSurface3, pGetAttachedSurface4, pGetAttachedSurface7;

void HookSurface(LPDIRECTDRAWSURFACE *lplpdds, int dxversion) {
    // IDirectDrawSurface::Query
    if(bLogAll || bLogQueryInterface) {
        switch(dxversion) {
        case 1:
            SetHook((void *)(**(DWORD **)lplpdds), extQueryInterfaceS1, (void **)&pQueryInterfaceS1, "QueryInterface(S1)");
            break;
        case 2:
            SetHook((void *)(**(DWORD **)lplpdds), extQueryInterfaceS2, (void **)&pQueryInterfaceS2, "QueryInterface(S2)");
            break;
        case 3:
            SetHook((void *)(**(DWORD **)lplpdds), extQueryInterfaceS3, (void **)&pQueryInterfaceS3, "QueryInterface(S3)");
            break;
        case 4:
            SetHook((void *)(**(DWORD **)lplpdds), extQueryInterfaceS4, (void **)&pQueryInterfaceS4, "QueryInterface(S4)");
            break;
        case 7:
            SetHook((void *)(**(DWORD **)lplpdds), extQueryInterfaceS7, (void **)&pQueryInterfaceS7, "QueryInterface(S7)");
            break;
        default:
            trace("*** unknown interface ***\n");
            break;
        }
    }
    if(bLogRelease) {
        switch(dxversion) {
        case 1:
            SetHook((void *)(**(DWORD **)lplpdds + 4), extAddRefS1, (void **)&pAddRefS1, "AddRef(S)");
            SetHook((void *)(**(DWORD **)lplpdds + 8), extReleaseS1, (void **)&pReleaseS1, "Release(S)");
            break;
        case 2:
            SetHook((void *)(**(DWORD **)lplpdds + 4), extAddRefS2, (void **)&pAddRefS2, "AddRef(S)");
            SetHook((void *)(**(DWORD **)lplpdds + 8), extReleaseS2, (void **)&pReleaseS2, "Release(S)");
            break;
        case 3:
            SetHook((void *)(**(DWORD **)lplpdds + 4), extAddRefS3, (void **)&pAddRefS3, "AddRef(S)");
            SetHook((void *)(**(DWORD **)lplpdds + 8), extReleaseS3, (void **)&pReleaseS3, "Release(S)");
            break;
        case 4:
            SetHook((void *)(**(DWORD **)lplpdds + 4), extAddRefS4, (void **)&pAddRefS4, "AddRef(S)");
            SetHook((void *)(**(DWORD **)lplpdds + 8), extReleaseS4, (void **)&pReleaseS4, "Release(S)");
            break;
        case 7:
            SetHook((void *)(**(DWORD **)lplpdds + 4), extAddRefS7, (void **)&pAddRefS7, "AddRef(S)");
            SetHook((void *)(**(DWORD **)lplpdds + 8), extReleaseS7, (void **)&pReleaseS7, "Release(S)");
            break;
        default:
            trace("*** unknown interface ***\n");
            break;
        }
    }
    if(bLogAll || bLogBlits) {
        switch(dxversion) {
        case 1:
            SetHook((void *)(**(DWORD **)lplpdds + 20), extBlt1, (void **)&pBlt1, "Blt(S1)");
            SetHook((void *)(**(DWORD **)lplpdds + 28), extBltFast1, (void **)&pBltFast1, "BltFast(S1)");
            SetHook((void *)(**(DWORD **)lplpdds + 44), extFlip1, (void **)&pFlip1, "Flip(S1)");
            break;
        case 2:
            SetHook((void *)(**(DWORD **)lplpdds + 20), extBlt2, (void **)&pBlt2, "Blt(S2)");
            SetHook((void *)(**(DWORD **)lplpdds + 28), extBltFast2, (void **)&pBltFast2, "BltFast(S2)");
            SetHook((void *)(**(DWORD **)lplpdds + 44), extFlip2, (void **)&pFlip2, "Flip(S2)");
            break;
        case 3:
            SetHook((void *)(**(DWORD **)lplpdds + 20), extBlt3, (void **)&pBlt3, "Blt(S3)");
            SetHook((void *)(**(DWORD **)lplpdds + 28), extBltFast3, (void **)&pBltFast3, "BltFast(S3)");
            SetHook((void *)(**(DWORD **)lplpdds + 44), extFlip3, (void **)&pFlip3, "Flip(S3)");
            break;
        case 4:
            SetHook((void *)(**(DWORD **)lplpdds + 20), extBlt4, (void **)&pBlt4, "Blt(S4)");
            SetHook((void *)(**(DWORD **)lplpdds + 28), extBltFast4, (void **)&pBltFast4, "BltFast(S4)");
            SetHook((void *)(**(DWORD **)lplpdds + 44), extFlip4, (void **)&pFlip4, "Flip(S4)");
            break;
        case 7:
            SetHook((void *)(**(DWORD **)lplpdds + 20), extBlt7, (void **)&pBlt7, "Blt(S7)");
            SetHook((void *)(**(DWORD **)lplpdds + 28), extBltFast7, (void **)&pBltFast7, "BltFast(S7)");
            SetHook((void *)(**(DWORD **)lplpdds + 44), extFlip7, (void **)&pFlip7, "Flip(S7)");
            break;
        default:
            trace("*** unknown interface ***\n");
            break;
        }
    }
    if(bLogAttach) {
        switch(dxversion) {
        case 1:
            SetHook((void *)(**(DWORD **)lplpdds + 12), extAddAttachedSurface1, (void **)&pAddAttachedSurface1, "AddAttachedSurface(S1)");
            SetHook((void *)(**(DWORD **)lplpdds + 32), extDeleteAttachedSurface1, (void **)&pDeleteAttachedSurface1, "DeleteAttachedSurface(S1)");
            SetHook((void *)(**(DWORD **)lplpdds + 36), extEnumAttachedSurfaces1, (void **)&pEnumAttachedSurfaces1, "EnumAttachedSurfaces(S1)");
            SetHook((void *)(**(DWORD **)lplpdds + 48), extGetAttachedSurface1, (void **)&pGetAttachedSurface1, "GetAttachedSurface(S1)");
            break;
        case 2:
            SetHook((void *)(**(DWORD **)lplpdds + 12), extAddAttachedSurface2, (void **)&pAddAttachedSurface2, "AddAttachedSurface(S2)");
            SetHook((void *)(**(DWORD **)lplpdds + 32), extDeleteAttachedSurface2, (void **)&pDeleteAttachedSurface2, "DeleteAttachedSurface(S2)");
            SetHook((void *)(**(DWORD **)lplpdds + 36), extEnumAttachedSurfaces2, (void **)&pEnumAttachedSurfaces2, "EnumAttachedSurfaces(S2)");
            SetHook((void *)(**(DWORD **)lplpdds + 48), extGetAttachedSurface2, (void **)&pGetAttachedSurface2, "GetAttachedSurface(S2)");
            break;
        case 3:
            SetHook((void *)(**(DWORD **)lplpdds + 12), extAddAttachedSurface3, (void **)&pAddAttachedSurface3, "AddAttachedSurface(S3)");
            SetHook((void *)(**(DWORD **)lplpdds + 32), extDeleteAttachedSurface3, (void **)&pDeleteAttachedSurface3, "DeleteAttachedSurface(S3)");
            SetHook((void *)(**(DWORD **)lplpdds + 36), extEnumAttachedSurfaces3, (void **)&pEnumAttachedSurfaces3, "EnumAttachedSurfaces(S3)");
            SetHook((void *)(**(DWORD **)lplpdds + 48), extGetAttachedSurface3, (void **)&pGetAttachedSurface3, "GetAttachedSurface(S3)");
            break;
        case 4:
            SetHook((void *)(**(DWORD **)lplpdds + 12), extAddAttachedSurface4, (void **)&pAddAttachedSurface4, "AddAttachedSurface(S4)");
            SetHook((void *)(**(DWORD **)lplpdds + 32), extDeleteAttachedSurface4, (void **)&pDeleteAttachedSurface4, "DeleteAttachedSurface(S4)");
            SetHook((void *)(**(DWORD **)lplpdds + 36), extEnumAttachedSurfaces4, (void **)&pEnumAttachedSurfaces4, "EnumAttachedSurfaces(S4)");
            SetHook((void *)(**(DWORD **)lplpdds + 48), extGetAttachedSurface4, (void **)&pGetAttachedSurface4, "GetAttachedSurface(S4)");
            break;
        case 7:
            SetHook((void *)(**(DWORD **)lplpdds + 12), extAddAttachedSurface7, (void **)&pAddAttachedSurface7, "AddAttachedSurface(S7)");
            SetHook((void *)(**(DWORD **)lplpdds + 32), extDeleteAttachedSurface7, (void **)&pDeleteAttachedSurface7, "DeleteAttachedSurface(S7)");
            SetHook((void *)(**(DWORD **)lplpdds + 36), extEnumAttachedSurfaces7, (void **)&pEnumAttachedSurfaces7, "EnumAttachedSurfaces(S7)");
            SetHook((void *)(**(DWORD **)lplpdds + 48), extGetAttachedSurface7, (void **)&pGetAttachedSurface7, "GetAttachedSurface(S7)");
            break;
        default:
            trace("*** unknown interface ***\n");
            break;
        }
    }
}

static ULONG WINAPI extAddRefS(int dxversion, AddRefS_Type pAddRefS, LPDIRECTDRAWSURFACE lpdds) {
    ULONG res;
    trace(">> Surface(%d)::AddRef(lpdds=%x)\n", dxversion, lpdds);
    res = (*pAddRefS)(lpdds);
    trace("<< Surface(%d)::AddRef res(refcount)=%x\n", dxversion, res);
    return res;
}

ULONG WINAPI extAddRefS1(LPDIRECTDRAWSURFACE lpdds) {
    return extAddRefS(1, pAddRefS1, lpdds);
}
ULONG WINAPI extAddRefS2(LPDIRECTDRAWSURFACE lpdds) {
    return extAddRefS(2, pAddRefS2, lpdds);
}
ULONG WINAPI extAddRefS3(LPDIRECTDRAWSURFACE lpdds) {
    return extAddRefS(3, pAddRefS3, lpdds);
}
ULONG WINAPI extAddRefS4(LPDIRECTDRAWSURFACE lpdds) {
    return extAddRefS(4, pAddRefS4, lpdds);
}
ULONG WINAPI extAddRefS7(LPDIRECTDRAWSURFACE lpdds) {
    return extAddRefS(7, pAddRefS7, lpdds);
}

static ULONG WINAPI extReleaseS(int dxversion, ReleaseS_Type pReleaseS, LPDIRECTDRAWSURFACE lpdds) {
    HRESULT res;
    trace(">> Surface(%d)::Release(lpdds=%x)\n", dxversion, lpdds);
    res = (*pReleaseS)(lpdds);
    trace("<< Surface(%d)::Release res(refcount)=%x\n", dxversion, res);
    return res;
}

ULONG WINAPI extReleaseS1(LPDIRECTDRAWSURFACE lpdds) {
    return extReleaseS(1, pReleaseS1, lpdds);
}
ULONG WINAPI extReleaseS2(LPDIRECTDRAWSURFACE lpdds) {
    return extReleaseS(2, pReleaseS2, lpdds);
}
ULONG WINAPI extReleaseS3(LPDIRECTDRAWSURFACE lpdds) {
    return extReleaseS(3, pReleaseS3, lpdds);
}
ULONG WINAPI extReleaseS4(LPDIRECTDRAWSURFACE lpdds) {
    return extReleaseS(4, pReleaseS4, lpdds);
}
ULONG WINAPI extReleaseS7(LPDIRECTDRAWSURFACE lpdds) {
    return extReleaseS(7, pReleaseS7, lpdds);
}

static HRESULT WINAPI extQueryInterfaceS(int dxversion, QueryInterface_Type pQueryInterfaceS,
        void *lpdds, REFIID riid, LPVOID *obp) {
    HRESULT res;
    int dwLocalDDVersion;
    trace(">> Surface(%d)::QueryInterface(lpdds=%x refiid=%s)\n", dxversion, lpdds, sGUID((GUID *)&riid));
    dumprefi(lpdds);
    res = (*pQueryInterfaceS)(lpdds, riid, obp);
    trace("<< Surface(%d)::QueryInterface(obp=%x) res=%x(%s)\n", dxversion, *obp, res, ExplainDDError(res));
    dwLocalDDVersion = GUIDSurfaceVersion((GUID *)&riid, NULL);
    if(!res) {
        dumpreff(lpdds);
        if(dwLocalDDVersion) dumpreff(*obp);
        HookSurface((LPDIRECTDRAWSURFACE *)obp, dwLocalDDVersion);
    } else
        trace("<< Surface(%d)::QueryInterface res=%x(%s)\n", dxversion, res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extQueryInterfaceS1(void *lpdds, REFIID riid, LPVOID *obp) {
    return extQueryInterfaceS(1, pQueryInterfaceS1, lpdds, riid, obp);
}
HRESULT WINAPI extQueryInterfaceS2(void *lpdds, REFIID riid, LPVOID *obp) {
    return extQueryInterfaceS(2, pQueryInterfaceS2, lpdds, riid, obp);
}
HRESULT WINAPI extQueryInterfaceS3(void *lpdds, REFIID riid, LPVOID *obp) {
    return extQueryInterfaceS(3, pQueryInterfaceS3, lpdds, riid, obp);
}
HRESULT WINAPI extQueryInterfaceS4(void *lpdds, REFIID riid, LPVOID *obp) {
    return extQueryInterfaceS(4, pQueryInterfaceS4, lpdds, riid, obp);
}
HRESULT WINAPI extQueryInterfaceS7(void *lpdds, REFIID riid, LPVOID *obp) {
    return extQueryInterfaceS(7, pQueryInterfaceS7, lpdds, riid, obp);
}

char *ExplainBltFlags(DWORD c) {
    static char eb[512];
    unsigned int l;
    strcpy(eb, "DDBLT_");
    if (c & DDBLT_ALPHADEST) strcat(eb, "ALPHADEST+");
    if (c & DDBLT_ALPHADESTCONSTOVERRIDE) strcat(eb, "ALPHADESTCONSTOVERRIDE+");
    if (c & DDBLT_ALPHADESTNEG) strcat(eb, "ALPHADESTNEG+");
    if (c & DDBLT_ALPHADESTSURFACEOVERRIDE) strcat(eb, "ALPHADESTSURFACEOVERRIDE+");
    if (c & DDBLT_ALPHAEDGEBLEND) strcat(eb, "ALPHAEDGEBLEND+");
    if (c & DDBLT_ALPHASRC) strcat(eb, "ALPHASRC+");
    if (c & DDBLT_ALPHASRCCONSTOVERRIDE) strcat(eb, "ALPHASRCCONSTOVERRIDE+");
    if (c & DDBLT_ALPHASRCNEG) strcat(eb, "ALPHASRCNEG+");
    if (c & DDBLT_ALPHASRCSURFACEOVERRIDE) strcat(eb, "ALPHASRCSURFACEOVERRIDE+");
    if (c & DDBLT_ASYNC) strcat(eb, "ASYNC+");
    if (c & DDBLT_COLORFILL) strcat(eb, "COLORFILL+");
    if (c & DDBLT_DDFX) strcat(eb, "DDFX+");
    if (c & DDBLT_DDROPS) strcat(eb, "DDROPS+");
    if (c & DDBLT_KEYDEST) strcat(eb, "KEYDEST+");
    if (c & DDBLT_KEYDESTOVERRIDE) strcat(eb, "KEYDESTOVERRIDE+");
    if (c & DDBLT_KEYSRC) strcat(eb, "KEYSRC+");
    if (c & DDBLT_KEYSRCOVERRIDE) strcat(eb, "KEYSRCOVERRIDE+");
    if (c & DDBLT_ROP) strcat(eb, "ROP+");
    if (c & DDBLT_ROTATIONANGLE) strcat(eb, "ROTATIONANGLE+");
    if (c & DDBLT_ZBUFFER) strcat(eb, "ZBUFFER+");
    if (c & DDBLT_ZBUFFERDESTCONSTOVERRIDE) strcat(eb, "ZBUFFERDESTCONSTOVERRIDE+");
    if (c & DDBLT_ZBUFFERDESTOVERRIDE) strcat(eb, "ZBUFFERDESTOVERRIDE+");
    if (c & DDBLT_ZBUFFERSRCCONSTOVERRIDE) strcat(eb, "ZBUFFERSRCCONSTOVERRIDE+");
    if (c & DDBLT_ZBUFFERSRCOVERRIDE) strcat(eb, "ZBUFFERSRCOVERRIDE+");
    if (c & DDBLT_WAIT) strcat(eb, "WAIT+");
    if (c & DDBLT_DEPTHFILL) strcat(eb, "DEPTHFILL+");
    if (c & DDBLT_DONOTWAIT) strcat(eb, "DONOTWAIT+");
    if (c & DDBLT_ROTATIONANGLE) strcat(eb, "ROTATIONANGLE+");
    l = strlen(eb);
    if (l > strlen("DDBLT_")) eb[l - 1] = 0; // delete last '+' if any
    else eb[0] = 0;
    return(eb);
}

static HRESULT WINAPI extBlt(int dxversion, Blt_Type pBlt,
                             LPDIRECTDRAWSURFACE lpdds, LPRECT lpdestrect, LPDIRECTDRAWSURFACE lpddssrc, LPRECT lpsrcrect, DWORD dwflags, LPDDBLTFX lpddbltfx) {
    HRESULT res;
    char rectsrc[41], rectdst[41];
    if (lpdestrect) sprintf(rectdst, "(%d,%d)-(%d,%d)", lpdestrect->left, lpdestrect->top, lpdestrect->right, lpdestrect->bottom);
    else sprintf(rectdst, "(NULL)");
    if (lpsrcrect) sprintf(rectsrc, "(%d,%d)-(%d,%d)", lpsrcrect->left, lpsrcrect->top, lpsrcrect->right, lpsrcrect->bottom);
    else sprintf(rectsrc, "(NULL)");
    trace(">> Surface(%d)::Blt(lpdds=%x lpdestrect=%s lpddssrc=%x lpsrcrect=%s flags=%x(%s)",
          dxversion, lpdds, rectdst, lpddssrc, rectsrc, dwflags, ExplainBltFlags(dwflags));
    if(lpddbltfx) {
        if (dwflags & DDBLT_COLORFILL) trace(" ddbltfx.FillColor=%x", lpddbltfx->dwFillColor);
        if (dwflags & DDBLT_KEYDESTOVERRIDE) trace(" ddbltfx.DestColorkey=%x", lpddbltfx->ddckDestColorkey);
        if (dwflags & DDBLT_KEYSRCOVERRIDE) trace(" ddbltfx.SrcColorkey=%x", lpddbltfx->ddckSrcColorkey);
        if (dwflags & DDBLT_ROP) trace(" ddbltfx.ROP=%x", lpddbltfx->dwROP);
        if (dwflags & DDBLT_DEPTHFILL) trace(" ddbltfx.FillDepth=%x", lpddbltfx->dwFillDepth);
    }
    trace(")\n");
    res = (*pBlt)(lpdds, lpdestrect, lpddssrc, lpsrcrect, dwflags, lpddbltfx);
    trace("<< Surface(%d)::Blt res=%x(%s)\n", dxversion, res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extBlt1(LPDIRECTDRAWSURFACE lpdds, LPRECT lpdestrect, LPDIRECTDRAWSURFACE lpddssrc, LPRECT lpsrcrect, DWORD dwflags, LPDDBLTFX lpddbltfx) {
    return extBlt(1, pBlt1, lpdds, lpdestrect, lpddssrc, lpsrcrect, dwflags, lpddbltfx);
}
HRESULT WINAPI extBlt2(LPDIRECTDRAWSURFACE lpdds, LPRECT lpdestrect, LPDIRECTDRAWSURFACE lpddssrc, LPRECT lpsrcrect, DWORD dwflags, LPDDBLTFX lpddbltfx) {
    return extBlt(2, pBlt2, lpdds, lpdestrect, lpddssrc, lpsrcrect, dwflags, lpddbltfx);
}
HRESULT WINAPI extBlt3(LPDIRECTDRAWSURFACE lpdds, LPRECT lpdestrect, LPDIRECTDRAWSURFACE lpddssrc, LPRECT lpsrcrect, DWORD dwflags, LPDDBLTFX lpddbltfx) {
    return extBlt(3, pBlt3, lpdds, lpdestrect, lpddssrc, lpsrcrect, dwflags, lpddbltfx);
}
HRESULT WINAPI extBlt4(LPDIRECTDRAWSURFACE lpdds, LPRECT lpdestrect, LPDIRECTDRAWSURFACE lpddssrc, LPRECT lpsrcrect, DWORD dwflags, LPDDBLTFX lpddbltfx) {
    return extBlt(4, pBlt4, lpdds, lpdestrect, lpddssrc, lpsrcrect, dwflags, lpddbltfx);
}
HRESULT WINAPI extBlt7(LPDIRECTDRAWSURFACE lpdds, LPRECT lpdestrect, LPDIRECTDRAWSURFACE lpddssrc, LPRECT lpsrcrect, DWORD dwflags, LPDDBLTFX lpddbltfx) {
    return extBlt(7, pBlt7, lpdds, lpdestrect, lpddssrc, lpsrcrect, dwflags, lpddbltfx);
}

char *ExplainBltFastFlags(DWORD c) {
    static char eb[256];
    unsigned int l;
    strcpy(eb, "DDBLTFAST_");
    if (!(c & (DDBLTFAST_SRCCOLORKEY | DDBLTFAST_DESTCOLORKEY))) strcat(eb, "NOCOLORKEY+");
    if (c & DDBLTFAST_SRCCOLORKEY) strcat(eb, "SRCCOLORKEY+");
    if (c & DDBLTFAST_DESTCOLORKEY) strcat(eb, "DESTCOLORKEY+");
    if (c & DDBLTFAST_WAIT) strcat(eb, "WAIT+");
    if (c & DDBLTFAST_DONOTWAIT) strcat(eb, "DONOTWAIT+");
    l = strlen(eb);
    if (l > strlen("DDBLTFAST_")) eb[l - 1] = 0; // delete last '+' if any
    else eb[0] = 0;
    return(eb);
}

static HRESULT WINAPI extBltFast(int dxversion, BltFast_Type pBltFast,
                                 LPDIRECTDRAWSURFACE lpdds, DWORD dwx, DWORD dwy, LPDIRECTDRAWSURFACE lpddssrc, LPRECT lpsrcrect, DWORD dwtrans) {
    HRESULT res;
    char rectsrc[41];
    if (lpsrcrect) sprintf(rectsrc, "(%d,%d)-(%d,%d)", lpsrcrect->left, lpsrcrect->top, lpsrcrect->right, lpsrcrect->bottom);
    else sprintf(rectsrc, "(NULL)");
    trace(">> Surface(%d)::BltFast(lpdds=%x pos=(%d,%d) lpddssrc=%x lpsrcrect=%s trans=%x(%s))\n",
          dxversion, lpdds, dwx, dwy, lpddssrc, rectsrc, dwtrans, ExplainBltFastFlags(dwtrans));
    res = (*pBltFast)(lpdds, dwx, dwy, lpddssrc, lpsrcrect, dwtrans);
    trace("<< Surface(%d)::BltFast res=%x(%s)\n", dxversion, res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extBltFast1(LPDIRECTDRAWSURFACE lpdds, DWORD dwx, DWORD dwy, LPDIRECTDRAWSURFACE lpddssrc, LPRECT lpsrcrect, DWORD dwtrans) {
    return extBltFast(1, pBltFast1, lpdds, dwx, dwy, lpddssrc, lpsrcrect, dwtrans);
}
HRESULT WINAPI extBltFast2(LPDIRECTDRAWSURFACE lpdds, DWORD dwx, DWORD dwy, LPDIRECTDRAWSURFACE lpddssrc, LPRECT lpsrcrect, DWORD dwtrans) {
    return extBltFast(2, pBltFast2, lpdds, dwx, dwy, lpddssrc, lpsrcrect, dwtrans);
}
HRESULT WINAPI extBltFast3(LPDIRECTDRAWSURFACE lpdds, DWORD dwx, DWORD dwy, LPDIRECTDRAWSURFACE lpddssrc, LPRECT lpsrcrect, DWORD dwtrans) {
    return extBltFast(3, pBltFast3, lpdds, dwx, dwy, lpddssrc, lpsrcrect, dwtrans);
}
HRESULT WINAPI extBltFast4(LPDIRECTDRAWSURFACE lpdds, DWORD dwx, DWORD dwy, LPDIRECTDRAWSURFACE lpddssrc, LPRECT lpsrcrect, DWORD dwtrans) {
    return extBltFast(4, pBltFast4, lpdds, dwx, dwy, lpddssrc, lpsrcrect, dwtrans);
}
HRESULT WINAPI extBltFast7(LPDIRECTDRAWSURFACE lpdds, DWORD dwx, DWORD dwy, LPDIRECTDRAWSURFACE lpddssrc, LPRECT lpsrcrect, DWORD dwtrans) {
    return extBltFast(7, pBltFast1, lpdds, dwx, dwy, lpddssrc, lpsrcrect, dwtrans);
}

static HRESULT WINAPI extGetAttachedSurface(int dxversion, GetAttachedSurface_Type pGetAttachedSurface,
        LPDIRECTDRAWSURFACE lpdds, LPDDSCAPS2 lpddsc, LPDIRECTDRAWSURFACE *lplpddas) {
    HRESULT res;
    char *ExplainDDSCaps(DWORD);
    trace(">> Surface(%d)::GetAttachedSurface(lpdds=%x caps=%x(%s))\n", dxversion, lpdds, lpddsc->dwCaps, ExplainDDSCaps(lpddsc->dwCaps));
    dumprefi(lpdds);
    res = (*pGetAttachedSurface)(lpdds, lpddsc, lplpddas);
    if(!res) {
        dumpreff(*lplpddas);
        trace("<< Surface(%d)::GetAttachedSurface(lpddas=%x)\n", dxversion, *lplpddas);
    } else
        trace("<< Surface(%d)::GetAttachedSurface res=%x(%s)\n", dxversion, res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extGetAttachedSurface1(LPDIRECTDRAWSURFACE lpdds, LPDDSCAPS lpddsc, LPDIRECTDRAWSURFACE *lplpddas) {
    return extGetAttachedSurface(1, pGetAttachedSurface1, lpdds, (LPDDSCAPS2)lpddsc, lplpddas);
}
HRESULT WINAPI extGetAttachedSurface2(LPDIRECTDRAWSURFACE lpdds, LPDDSCAPS lpddsc, LPDIRECTDRAWSURFACE *lplpddas) {
    return extGetAttachedSurface(2, pGetAttachedSurface2, lpdds, (LPDDSCAPS2)lpddsc, lplpddas);
}
HRESULT WINAPI extGetAttachedSurface3(LPDIRECTDRAWSURFACE lpdds, LPDDSCAPS lpddsc, LPDIRECTDRAWSURFACE *lplpddas) {
    return extGetAttachedSurface(3, pGetAttachedSurface3, lpdds, (LPDDSCAPS2)lpddsc, lplpddas);
}
HRESULT WINAPI extGetAttachedSurface4(LPDIRECTDRAWSURFACE lpdds, LPDDSCAPS2 lpddsc, LPDIRECTDRAWSURFACE *lplpddas) {
    return extGetAttachedSurface(4, pGetAttachedSurface4, lpdds, lpddsc, lplpddas);
}
HRESULT WINAPI extGetAttachedSurface7(LPDIRECTDRAWSURFACE lpdds, LPDDSCAPS2 lpddsc, LPDIRECTDRAWSURFACE *lplpddas) {
    return extGetAttachedSurface(7, pGetAttachedSurface7, lpdds, lpddsc, lplpddas);
}

static HRESULT WINAPI extAddAttachedSurface(int dxversion, AddAttachedSurface_Type pAddAttachedSurface, LPDIRECTDRAWSURFACE lpdds, LPDIRECTDRAWSURFACE lpddsadd) {
    HRESULT res;
    trace(">> Surface(%d)::AddAttachedSurface(lpdds=%x lpddsadd=%x)\n", dxversion, lpdds, lpddsadd);
    dumprefi(lpdds);
    dumprefi(lpddsadd);
    res = (*pAddAttachedSurface)(lpdds, lpddsadd);
    dumpreff(lpdds);
    dumpreff(lpddsadd);
    trace("<< Surface(%d)::AddAttachedSurface() res=%x(%s)\n", dxversion, res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extAddAttachedSurface1(LPDIRECTDRAWSURFACE lpdds, LPDIRECTDRAWSURFACE lpddsadd) {
    return extAddAttachedSurface(1, pAddAttachedSurface1, lpdds, lpddsadd);
}
HRESULT WINAPI extAddAttachedSurface2(LPDIRECTDRAWSURFACE lpdds, LPDIRECTDRAWSURFACE lpddsadd) {
    return extAddAttachedSurface(2, pAddAttachedSurface2, lpdds, lpddsadd);
}
HRESULT WINAPI extAddAttachedSurface3(LPDIRECTDRAWSURFACE lpdds, LPDIRECTDRAWSURFACE lpddsadd) {
    return extAddAttachedSurface(3, pAddAttachedSurface3, lpdds, lpddsadd);
}
HRESULT WINAPI extAddAttachedSurface4(LPDIRECTDRAWSURFACE lpdds, LPDIRECTDRAWSURFACE lpddsadd) {
    return extAddAttachedSurface(4, pAddAttachedSurface4, lpdds, lpddsadd);
}
HRESULT WINAPI extAddAttachedSurface7(LPDIRECTDRAWSURFACE lpdds, LPDIRECTDRAWSURFACE lpddsadd) {
    return extAddAttachedSurface(7, pAddAttachedSurface7, lpdds, lpddsadd);
}

static HRESULT WINAPI extEnumAttachedSurfaces(int dxversion, EnumAttachedSurfaces_Type pEnumAttachedSurfaces,
        LPDIRECTDRAWSURFACE lpdds, LPVOID lpContext, LPDDENUMSURFACESCALLBACK lpEnumSurfacesCallback) {
    HRESULT res;
    trace(">> Surface(%d)::EnumAttachedSurfaces(lpdds=%x context=%x cback=%x)\n", dxversion, lpdds, lpContext, lpEnumSurfacesCallback);
    res = (*pEnumAttachedSurfaces)(lpdds, lpContext, lpEnumSurfacesCallback);
    trace("<< Surface(%d)::EnumAttachedSurfaces() res=%x(%s)\n", dxversion, res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extEnumAttachedSurfaces1(LPDIRECTDRAWSURFACE lpdds, LPVOID lpContext, LPDDENUMSURFACESCALLBACK lpEnumSurfacesCallback) {
    return extEnumAttachedSurfaces(1, pEnumAttachedSurfaces1, lpdds, lpContext, lpEnumSurfacesCallback);
}
HRESULT WINAPI extEnumAttachedSurfaces2(LPDIRECTDRAWSURFACE lpdds, LPVOID lpContext, LPDDENUMSURFACESCALLBACK lpEnumSurfacesCallback) {
    return extEnumAttachedSurfaces(2, pEnumAttachedSurfaces2, lpdds, lpContext, lpEnumSurfacesCallback);
}
HRESULT WINAPI extEnumAttachedSurfaces3(LPDIRECTDRAWSURFACE lpdds, LPVOID lpContext, LPDDENUMSURFACESCALLBACK lpEnumSurfacesCallback) {
    return extEnumAttachedSurfaces(3, pEnumAttachedSurfaces3, lpdds, lpContext, lpEnumSurfacesCallback);
}
HRESULT WINAPI extEnumAttachedSurfaces4(LPDIRECTDRAWSURFACE lpdds, LPVOID lpContext, LPDDENUMSURFACESCALLBACK lpEnumSurfacesCallback) {
    return extEnumAttachedSurfaces(4, pEnumAttachedSurfaces4, lpdds, lpContext, lpEnumSurfacesCallback);
}
HRESULT WINAPI extEnumAttachedSurfaces7(LPDIRECTDRAWSURFACE lpdds, LPVOID lpContext, LPDDENUMSURFACESCALLBACK lpEnumSurfacesCallback) {
    return extEnumAttachedSurfaces(7, pEnumAttachedSurfaces7, lpdds, lpContext, lpEnumSurfacesCallback);
}

static HRESULT WINAPI extDeleteAttachedSurface(int dxversion, DeleteAttachedSurface_Type pDeleteAttachedSurface,
        LPDIRECTDRAWSURFACE lpdds,  DWORD dwflags, LPDIRECTDRAWSURFACE lpddsdel) {
    HRESULT res;
    trace(">> Surface(%d)::DeleteAttachedSurface(lpdds=%x flags=%x lpddsdel=%x)\n", dxversion, lpdds, dwflags, lpddsdel);
    dumprefi(lpdds);
    dumprefi(lpddsdel);
    res = (*pDeleteAttachedSurface)(lpdds, dwflags, lpddsdel);
    dumpreff(lpdds);
    dumpreff(lpddsdel);
    trace("<< Surface(%d)::DeleteAttachedSurface() res=%x(%s)\n", dxversion, res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extDeleteAttachedSurface1(LPDIRECTDRAWSURFACE lpdds,  DWORD dwflags, LPDIRECTDRAWSURFACE lpddsdel) {
    return extDeleteAttachedSurface(1, pDeleteAttachedSurface1, lpdds,  dwflags, lpddsdel);
}
HRESULT WINAPI extDeleteAttachedSurface2(LPDIRECTDRAWSURFACE lpdds,  DWORD dwflags, LPDIRECTDRAWSURFACE lpddsdel) {
    return extDeleteAttachedSurface(2, pDeleteAttachedSurface2, lpdds,  dwflags, lpddsdel);
}
HRESULT WINAPI extDeleteAttachedSurface3(LPDIRECTDRAWSURFACE lpdds,  DWORD dwflags, LPDIRECTDRAWSURFACE lpddsdel) {
    return extDeleteAttachedSurface(3, pDeleteAttachedSurface3, lpdds,  dwflags, lpddsdel);
}
HRESULT WINAPI extDeleteAttachedSurface4(LPDIRECTDRAWSURFACE lpdds,  DWORD dwflags, LPDIRECTDRAWSURFACE lpddsdel) {
    return extDeleteAttachedSurface(4, pDeleteAttachedSurface4, lpdds,  dwflags, lpddsdel);
}
HRESULT WINAPI extDeleteAttachedSurface7(LPDIRECTDRAWSURFACE lpdds,  DWORD dwflags, LPDIRECTDRAWSURFACE lpddsdel) {
    return extDeleteAttachedSurface(7, pDeleteAttachedSurface7, lpdds,  dwflags, lpddsdel);
}

char *ExplainFlipFlags(DWORD c) {
    static char eb[512];
    unsigned int l;
    strcpy(eb, "DDFLIP_");
    if (c & DDFLIP_WAIT) strcat(eb, "WAIT+");
    if (c & DDFLIP_EVEN) strcat(eb, "EVEN+");
    if (c & DDFLIP_ODD) strcat(eb, "ODD+");
    if (c & DDFLIP_NOVSYNC) strcat(eb, "NOVSYNC+");
    if (c & DDFLIP_INTERVAL3) strcat(eb, "INTERVAL3+");
    else if (c & DDFLIP_INTERVAL2) strcat(eb, "INTERVAL2+");
    if (c & DDFLIP_INTERVAL4) strcat(eb, "INTERVAL4+");
    if (c & DDFLIP_STEREO) strcat(eb, "STEREO+");
    if (c & DDFLIP_DONOTWAIT) strcat(eb, "DONOTWAIT+");
    l = strlen(eb);
    if (l > strlen("DDFLIP_")) eb[l - 1] = 0; // delete last '+' if any
    else eb[0] = 0;
    return(eb);
}

HRESULT WINAPI extFlip1(LPDIRECTDRAWSURFACE lpdds, LPDIRECTDRAWSURFACE lpddssrc, DWORD dwflags) {
    HRESULT res;
    extern char *ExplainFlipFlags(DWORD);
    trace(">> Surface[1]::Flip(lpdds=%x lpddssrc=%x flags=%x(%s))\n", lpdds, lpddssrc, dwflags, ExplainFlipFlags(dwflags));
    res = (*pFlip1)(lpdds, lpddssrc, dwflags);
    trace("<< Surface[1]::Flip res=%x(%s)\n", res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extFlip2(LPDIRECTDRAWSURFACE lpdds, LPDIRECTDRAWSURFACE lpddssrc, DWORD dwflags) {
    HRESULT res;
    extern char *ExplainFlipFlags(DWORD);
    trace(">> Surface[2]::Flip(lpdds=%x lpddssrc=%x flags=%x(%s))\n", lpdds, lpddssrc, dwflags, ExplainFlipFlags(dwflags));
    res = (*pFlip2)(lpdds, lpddssrc, dwflags);
    trace("<< Surface[2]::Flip res=%x(%s)\n", res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extFlip3(LPDIRECTDRAWSURFACE lpdds, LPDIRECTDRAWSURFACE lpddssrc, DWORD dwflags) {
    HRESULT res;
    extern char *ExplainFlipFlags(DWORD);
    trace(">> Surface[3]::Flip(lpdds=%x lpddssrc=%x flags=%x(%s))\n", lpdds, lpddssrc, dwflags, ExplainFlipFlags(dwflags));
    res = (*pFlip3)(lpdds, lpddssrc, dwflags);
    trace("<< Surface[3]::Flip res=%x(%s)\n", res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extFlip4(LPDIRECTDRAWSURFACE lpdds, LPDIRECTDRAWSURFACE lpddssrc, DWORD dwflags) {
    HRESULT res;
    extern char *ExplainFlipFlags(DWORD);
    trace(">> Surface[4]::Flip(lpdds=%x lpddssrc=%x flags=%x(%s))\n", lpdds, lpddssrc, dwflags, ExplainFlipFlags(dwflags));
    res = (*pFlip4)(lpdds, lpddssrc, dwflags);
    trace("<< Surface[4]::Flip res=%x(%s)\n", res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extFlip7(LPDIRECTDRAWSURFACE lpdds, LPDIRECTDRAWSURFACE lpddssrc, DWORD dwflags) {
    HRESULT res;
    extern char *ExplainFlipFlags(DWORD);
    trace(">> Surface[7]::Flip(lpdds=%x lpddssrc=%x flags=%x(%s))\n", lpdds, lpddssrc, dwflags, ExplainFlipFlags(dwflags));
    res = (*pFlip7)(lpdds, lpddssrc, dwflags);
    trace("<< Surface[7]::Flip res=%x(%s)\n", res, ExplainDDError(res));
    return res;
}