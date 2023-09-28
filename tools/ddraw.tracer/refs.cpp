#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ctype.h>
#include <stdio.h>
#include "dxdiag.h"
#include "ddraw.h"
#include "proxy.h"

#if 0
typedef ULONG (WINAPI *AddRefS_Type)(LPDIRECTDRAWSURFACE);
typedef ULONG (WINAPI *ReleaseS_Type)(LPDIRECTDRAWSURFACE);
extern ReleaseS_Type pReleaseS1, pReleaseS2, pReleaseS3, pReleaseS4, pReleaseS7;
extern AddRefS_Type pAddRefS1, pAddRefS2, pAddRefS3, pAddRefS4, pAddRefS7;

void DumpRefS(BOOL bInitial, int dxversion, LPDIRECTDRAWSURFACE lpdds) {
    AddRefS_Type pAddRef;
    ReleaseS_Type pRelease;
    if(!bLogRefCount) return;
    switch(dxversion) {
    case 1:
        pAddRef = pAddRefS1;
        pRelease = pReleaseS1;
        break;
    case 2:
        pAddRef = pAddRefS2;
        pRelease = pReleaseS2;
        break;
    case 3:
        pAddRef = pAddRefS3;
        pRelease = pReleaseS3;
        break;
    case 4:
        pAddRef = pAddRefS4;
        pRelease = pReleaseS4;
        break;
    case 7:
        pAddRef = pAddRefS7;
        pRelease = pReleaseS7;
        break;
    default:
        trace("*** bad version ***\n");
        return;
        break;
    }
    if(lpdds) {
        ULONG kount;
        if(!pAddRef) {
            trace("*** no AddRef method ***\n");
            return;
        }
        if(!pRelease) {
            trace("*** no Release method ***\n");
            return;
        }
        (*pAddRef)(lpdds);
        kount = (*pRelease)(lpdds);
        trace("%s lpdds=%x ref=%d\n", bInitial ? ">" : "<", lpdds, kount);
    } else
        trace("%s lpdds=%x(NULL)\n", bInitial ? ">" : "<", lpdds);
}
#endif
#if 1
void DumpRefS(BOOL bInitial, int dxversion, LPDIRECTDRAWSURFACE lpdds) {
    if(!bLogRefCount) return;
    if(lpdds) {
        ULONG kount;
        lpdds->AddRef();
        kount = lpdds->Release();
        trace("%s lpdds=%x ref=%d\n", bInitial ? ">" : "<", lpdds, kount);
    } else
        trace("%s lpdds=%x(NULL)\n", bInitial ? ">" : "<", lpdds);
}
#endif

