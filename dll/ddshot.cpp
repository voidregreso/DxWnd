#define  _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include "dxwnd.h"
#include "dxwcore.hpp"
#include "dxhelper.h"

extern char *ExplainDDError(DWORD);

typedef HRESULT (WINAPI *Lock_Type)(LPDIRECTDRAWSURFACE, LPRECT, LPDDSURFACEDESC, DWORD, HANDLE);
typedef HRESULT (WINAPI *Unlock4_Type)(LPDIRECTDRAWSURFACE, LPRECT);
typedef HRESULT (WINAPI *Unlock1_Type)(LPDIRECTDRAWSURFACE, LPVOID);

extern Lock_Type pLockMethod(int);
extern Unlock4_Type pUnlockMethod(int);
extern int Set_dwSize_From_Surface();

void DDrawScreenShot(int dxversion, int hash) {
    LPDIRECTDRAWSURFACE s;
    DDSURFACEDESC2 ddsd;
    int w, h, iSurfaceSize, iScanLineSize;
    HRESULT res;
    static int MinTexX, MinTexY, MaxTexX, MaxTexY;
    char pszFile[MAX_PATH];
    if(dxw.Renderer->flags & DXWRF_USEBACKBUFFER) {
        // to do: SDL renderer has a better way ...
        s = dxwss.GetBackBufferSurface();
    } else
        s = dxwss.GetPrimarySurface();
    if(!s) return;
    memset(&ddsd, 0, sizeof(DDSURFACEDESC2));
    ddsd.dwSize = Set_dwSize_From_Surface();
    ddsd.dwFlags = DDSD_LPSURFACE | DDSD_PITCH;
    if(res = (*pLockMethod(dxversion))(s, 0, (LPDDSURFACEDESC)&ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WRITEONLY | DDLOCK_WAIT, 0)) {
        OutTraceE("ScreenShot: Lock ERROR res=%#x(%s) at %d\n", res, ExplainDDError(res), __LINE__);
        return;
    }
    while (TRUE) { // fake loop
        OutTraceDW("ScreenShot: lpdds=%#x BitCount=%d size=(%dx%d)\n",
                   s, ddsd.ddpfPixelFormat.dwRGBBitCount, ddsd.dwWidth, ddsd.dwHeight);
        w = ddsd.dwWidth;
        h = ddsd.dwHeight;
        if(ddsd.ddpfPixelFormat.dwRGBBitCount == 0) {
            OutTraceE("ScreenShot: SKIP 0BPP texture\n");
            break;
        }
        iSurfaceSize = ddsd.dwHeight * ddsd.lPitch;
        FILE *hf;
        BITMAPFILEHEADER hdr;       // bitmap file-header
        BITMAPV4HEADER pbi;			// bitmap info-header
        memset((void *)&pbi, 0, sizeof(BITMAPV4HEADER));
        pbi.bV4Size = sizeof(BITMAPV4HEADER);
        pbi.bV4Width = ddsd.dwWidth;
        pbi.bV4Height = ddsd.dwHeight;
        pbi.bV4BitCount = (WORD)ddsd.ddpfPixelFormat.dwRGBBitCount;
        pbi.bV4SizeImage = ((pbi.bV4Width * pbi.bV4BitCount + 0x1F) & ~0x1F) / 8 * pbi.bV4Height;
        pbi.bV4Height = - pbi.bV4Height;
        pbi.bV4Planes = 1;
        pbi.bV4V4Compression = BI_BITFIELDS;
        if(pbi.bV4BitCount == 8) pbi.bV4V4Compression = BI_RGB;
        pbi.bV4XPelsPerMeter = 1;
        pbi.bV4YPelsPerMeter = 1;
        pbi.bV4ClrUsed = 0;
        if(pbi.bV4BitCount == 8) pbi.bV4ClrUsed = 256;
        pbi.bV4ClrImportant = 0;
        pbi.bV4RedMask = ddsd.ddpfPixelFormat.dwRBitMask;
        pbi.bV4GreenMask = ddsd.ddpfPixelFormat.dwGBitMask;
        pbi.bV4BlueMask = ddsd.ddpfPixelFormat.dwBBitMask;
        pbi.bV4AlphaMask = ddsd.ddpfPixelFormat.dwRGBAlphaBitMask;
        pbi.bV4CSType = LCS_CALIBRATED_RGB;
        iScanLineSize = ((pbi.bV4Width * pbi.bV4BitCount + 0x1F) & ~0x1F) / 8;
        // Create the .BMP file.
        sprintf_s(pszFile, MAX_PATH, "%s\\screenshot.out\\shot.%08d.bmp", GetDxWndPath(), hash++);
        hf = fopen(pszFile, "wb");
        if(!hf) break;
        hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M"
        // Compute the size of the entire file.
        hdr.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) + pbi.bV4Size + pbi.bV4ClrUsed * sizeof(RGBQUAD) + pbi.bV4SizeImage);
        hdr.bfReserved1 = 0;
        hdr.bfReserved2 = 0;
        // Compute the offset to the array of color indices.
        hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + pbi.bV4Size + pbi.bV4ClrUsed * sizeof (RGBQUAD);
        // Copy the BITMAPFILEHEADER into the .BMP file.
        fwrite((LPVOID)&hdr, sizeof(BITMAPFILEHEADER), 1, hf);
        // Copy the BITMAPINFOHEADER array into the file.
        fwrite((LPVOID)&pbi, sizeof(BITMAPV4HEADER), 1, hf);
        // Copy the RGBQUAD array into the file.
        if(pbi.bV4ClrUsed) {
            extern DWORD PaletteEntries[256];
            fwrite((LPVOID)PaletteEntries, pbi.bV4ClrUsed * sizeof (RGBQUAD), 1, hf);
        }
        // Copy the array of color indices into the .BMP file.
        for(int y = 0; y < (int)ddsd.dwHeight; y++)
            fwrite((BYTE *)ddsd.lpSurface + (y * ddsd.lPitch), iScanLineSize, 1, hf);
        // Close the .BMP file.
        fclose(hf);
        break;
    }
    res = (*pUnlockMethod(dxversion))(s, NULL);
#ifndef DXW_NOTRACES
    if (res) OutTraceE("ScreenShot: Unlock ERROR lpdds=%#x res=%#x(%s) at %d\n", s, res, ExplainDDError(res), __LINE__);
#endif // DXW_NOTRACES
}
