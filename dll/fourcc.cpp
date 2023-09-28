#define _CRT_SECURE_NO_WARNINGS
#define INITGUID

#include <windows.h>
#include <ddraw.h>
#include "dxwnd.h"
//#include "ddrawi.h"
//#include "dxwcore.hpp"
//#include "stdio.h"
//#include "hddraw.h"
//#include "dxhelper.h"
//#include "syslibs.h"
//#include "sdl2/SDL.h"

static float clip( float n, float lower, float upper) {
    n = ( n > lower ) * n + !( n > lower ) * lower;
    return ( n < upper ) * n + !( n < upper ) * upper;
}

LPVOID dxwConvertFourCC(LPDDSURFACEDESC2 lpddsd) {
    if((lpddsd->ddpfPixelFormat.dwFourCC == 0x32595559 /*YUY2*/) && (lpddsd->ddpfPixelFormat.dwRGBBitCount == 16)) {
        // see https://msdn.microsoft.com/en-us/library/windows/desktop/dd206750(v=vs.85).aspx
        LPBYTE yuy2buf;
        DWORD x, y, dwGap;
        int y0, y1, u, v;
        WORD r, g, b;
        yuy2buf = (LPBYTE)malloc(lpddsd->dwHeight * lpddsd->lPitch);
        LPBYTE psrc = (LPBYTE)lpddsd->lpSurface;
        LPWORD pdst = (LPWORD)yuy2buf;
        dwGap = (lpddsd->lPitch >> 1) - lpddsd->dwWidth;
        for(y = 0; y < lpddsd->dwHeight; y++) {
            for(x = 0; x < (lpddsd->dwWidth >> 1); x++) {
                y0 = (int) * psrc++ -16;
                u  = (int) * psrc++ -128;
                y1 = (int) * psrc++ -16;
                v  = (int) * psrc++ -128;
                // unoptimized, floating point calculation
                b  = (WORD)clip((float)(y0 +                 ( 1.140 * v )), 0, 255);
                g  = (WORD)clip((float)(y0 - ( 0.395 * u ) - ( 0.581 * v )), 0, 255);
                r  = (WORD)clip((float)(y0 + ( 2.032 * u )                ), 0, 255);
                // R5G6B5 conversion
                *pdst++ = ((r >> 3) & 0x1F) | (((g >> 2) & 0x3f) << 5) | (((b >> 3) & 0x1f) << 11);
                // unoptimized, floating point calculation
                b  = (WORD)clip((float)(y1 +                 ( 1.140 * v )), 0, 255);
                g  = (WORD)clip((float)(y1 - ( 0.395 * u ) - ( 0.581 * v )), 0, 255);
                r  = (WORD)clip((float)(y1 + ( 2.032 * u )                ), 0, 255);
                // R5G6B5 conversion
                *pdst++ = ((r >> 3) & 0x1F) | (((g >> 2) & 0x3f) << 5) | (((b >> 3) & 0x1f) << 11);
            }
            psrc += (dwGap << 1);
            pdst += dwGap;
        }
        return yuy2buf;
    }
    return NULL;
}