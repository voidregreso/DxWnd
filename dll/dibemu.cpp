#define _CRT_SECURE_NO_WARNINGS

#include "dxwnd.h"
#include "dxwcore.hpp"
#include "syslibs.h"
#include "hddraw.h"
#include "dxhook.h"
#include "dxhelper.h"

#include "stdio.h"

UINT dxwCore::SetDIBColors(HDC hdc, UINT uStartIndex, UINT cEntries, const RGBQUAD *pColors) {
    if(DIBRGBQuadEntries == NULL) DIBRGBQuadEntries = (RGBQUAD *)malloc(256 * sizeof(RGBQUAD));
    if(DIBRGBQuadEntries == NULL) return 0; // in case of malloc failure ....
    if((uStartIndex + cEntries) > 256) cEntries = 256 - uStartIndex; // trim color number if exceeding size
    memcpy(&DIBRGBQuadEntries[uStartIndex], pColors, cEntries * sizeof(RGBQUAD));
    return cEntries;
}

UINT dxwCore::GetDIBColors(HDC hdc, UINT uStartIndex, UINT cEntries, const RGBQUAD *pColors) {
    if(DIBRGBQuadEntries == NULL) return 0; // in case of malloc failure ....
    if((uStartIndex + cEntries) > 256) cEntries = 256 - uStartIndex; // trim color number if exceeding size
    memcpy((LPVOID)pColors, &DIBRGBQuadEntries[uStartIndex], cEntries * sizeof(RGBQUAD));
    return cEntries;
}

LPVOID dxwCore::EmulateDIB(LPVOID lpvBits, BITMAPINFO *lpbmi, UINT fuColorUse) {
    LPVOID lpvNewBits;
    RGBQUAD *quad;
    LPDWORD pixel32;
    LPBYTE pixel8;
    RGBQUAD DDrawPalette[256];
    extern DWORD PaletteEntries[256];
    int w, h;
    w = lpbmi->bmiHeader.biWidth;
    h = lpbmi->bmiHeader.biHeight;
    if(h < 0) h = -h;
    lpvNewBits = malloc(w * h * sizeof(DWORD));
    // v2.04.94: copy ddraw virtual palette to DIBRGBQuadEntries
    // the operation must be repeated for each frame because ddraw palette could have
    // been changed in meanwhile. Fixes "iM1A2 Abrams".
    quad = DDrawPalette;
    for(int i = 0; i < 256; i++) {
        quad->rgbRed = (BYTE)((PaletteEntries[i] >> 16) & 0xFF);
        quad->rgbGreen = (BYTE)((PaletteEntries[i] >> 8) & 0xFF);
        quad->rgbBlue = (BYTE)((PaletteEntries[i] >> 0) & 0xFF);
        quad->rgbReserved = 0;
        quad++;
    }
    this->SetDIBColors(NULL, 0, 256, DDrawPalette);
    quad = DIBRGBQuadEntries;
    lpbmi->bmiHeader.biClrImportant = 0;
    lpbmi->bmiHeader.biClrUsed = 0;
    lpbmi->bmiHeader.biCompression = 0;
    lpbmi->bmiHeader.biSizeImage = 0;
    lpbmi->bmiHeader.biBitCount = 32;
    pixel32 = (DWORD *)lpvNewBits;
    pixel8 = (LPBYTE)lpvBits;
    for (int y = 0; y < h; y++) {
        // to do: DIB width not multiple of 4 bytes
        for (int x = 0; x < w; x++) {
            RGBQUAD q = quad[*pixel8++];
            *pixel32++ = (q.rgbRed << 16) | (q.rgbGreen << 8) | (q.rgbBlue << 0);
        }
    }
    return lpvNewBits;
}
