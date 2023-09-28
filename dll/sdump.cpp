#ifndef DXW_NOTRACES
#define  _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include "dxwnd.h"
#include "dxwcore.hpp"

extern char *ExplainDDError(DWORD);
extern unsigned int HashSurface(BYTE *, int, int, int);
extern char *SurfaceType(DDPIXELFORMAT);

extern void dx8SurfaceDump(int, LPDIRECTDRAWSURFACE);
extern void dx9SurfaceDump(int, LPDIRECTDRAWSURFACE);
extern void dx17SurfaceDump(int, LPDIRECTDRAWSURFACE, int);

void SurfaceDump(LPDIRECTDRAWSURFACE lpdds, int dxversion) {
    static BOOL DoOnce = TRUE;
    char pszFile[MAX_PATH];
    static int prog = 0;
    OutTrace("SurfaceDump: lpdds=%#x version=%d prog=%d\n", lpdds, dxversion, prog);
    if(DoOnce) {
        sprintf_s(pszFile, MAX_PATH, "%s\\surface.out", GetDxWndPath());
        CreateDirectory(pszFile, NULL);
        DoOnce = FALSE;
    }
    switch(dxversion) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 7:
        dx17SurfaceDump(++prog, lpdds, dxversion);
        break;
    case 8:
        dx8SurfaceDump(++prog, lpdds);
        break;
    case 9:
        dx9SurfaceDump(++prog, lpdds);
        break;
    }
    return;
}
#endif // DXW_NOTRACES