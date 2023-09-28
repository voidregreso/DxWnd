#define _CRT_SECURE_NO_WARNINGS 1
//#define TRACEALL

#include "dxwnd.h"
#include "dxwcore.hpp"
#include "syslibs.h"
#include "dxhook.h"
#include "Vfw.h"

extern char *sFourCC(DWORD);

#define DXWDECLARATIONS TRUE
#include "msvfwhook.h"
#undef DXWDECLARATIONS

#ifdef TRACEALL
typedef LRESULT (WINAPI *ICClose_Type)(HIC);
ICClose_Type pICClose;
LRESULT WINAPI extICClose(HIC);
typedef DWORD (VFWAPIV *ICDecompress_Type)(HIC, DWORD, LPBITMAPINFOHEADER, LPVOID, LPBITMAPINFOHEADER, LPVOID);
ICDecompress_Type pICDecompress;
DWORD VFWAPIV extICDecompress(HIC, DWORD, LPBITMAPINFOHEADER, LPVOID, LPBITMAPINFOHEADER, LPVOID);
#endif // TRACEALL

static HookEntryEx_Type Hooks[] = {
    {HOOK_HOT_CANDIDATE, 0, "MCIWndCreateA", (FARPROC)NULL, (FARPROC *) &pMCIWndCreateA, (FARPROC)extMCIWndCreateA}, // "Man in Black" - beware: this is NOT STDCALL!!!
    {HOOK_HOT_CANDIDATE, 0, "ICGetDisplayFormat", (FARPROC)NULL, (FARPROC *) &pICGetDisplayFormat, (FARPROC)extICGetDisplayFormat}, // "Man in Black" - beware: this is NOT STDCALL!!!
    {HOOK_HOT_CANDIDATE, 0, "ICDrawBegin", (FARPROC)NULL, (FARPROC *) &pICDrawBegin, (FARPROC)extICDrawBegin}, // AoE demo: not a STDCALL !!
    {HOOK_HOT_CANDIDATE, 0, "DrawDibDraw", (FARPROC)NULL, (FARPROC *) &pDrawDibDraw, (FARPROC)extDrawDibDraw},
    {HOOK_HOT_CANDIDATE, 0, "DrawDibBegin", (FARPROC)NULL, (FARPROC *) &pDrawDibBegin, (FARPROC)extDrawDibBegin},
#ifdef TRACEALL
    {HOOK_HOT_CANDIDATE, 0, "DrawDibOpen", (FARPROC)NULL, (FARPROC *) &pDrawDibOpen, (FARPROC)extDrawDibOpen},
    {HOOK_HOT_CANDIDATE, 0, "DrawDibClose", (FARPROC)NULL, (FARPROC *) &pDrawDibClose, (FARPROC)extDrawDibClose},
    {HOOK_HOT_CANDIDATE, 0, "ICSendMessage", (FARPROC)NULL, (FARPROC *) &pICSendMessage, (FARPROC)extICSendMessage},
    {HOOK_HOT_CANDIDATE, 0, "ICOpen", (FARPROC)NULL, (FARPROC *) &pICOpen, (FARPROC)extICOpen},
    {HOOK_HOT_CANDIDATE, 0, "ICClose", (FARPROC)NULL, (FARPROC *) &pICClose, (FARPROC)extICClose},
    {HOOK_HOT_CANDIDATE, 0, "ICLocate", (FARPROC)NULL, (FARPROC *) &pICLocate, (FARPROC)extICLocate},
    {HOOK_HOT_CANDIDATE, 0, "ICDecompress", (FARPROC)NULL, (FARPROC *) &pICDecompress, (FARPROC)extICDecompress},
    {HOOK_HOT_CANDIDATE, 0, "DrawDibStart", (FARPROC)NULL, (FARPROC *) &pDrawDibStart, (FARPROC)extDrawDibStart},
    {HOOK_HOT_CANDIDATE, 0, "DrawDibStop", (FARPROC)NULL, (FARPROC *) &pDrawDibStop, (FARPROC)extDrawDibStop},
    {HOOK_HOT_CANDIDATE, 0, "DrawDibEnd", (FARPROC)NULL, (FARPROC *) &pDrawDibEnd, (FARPROC)extDrawDibEnd},
#endif // TRACEALL
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

FARPROC Remap_vfw_ProcAddress(LPCSTR proc, HMODULE hModule) {
    FARPROC addr;
    if (addr = RemapLibraryEx(proc, hModule, Hooks)) return addr;
    // NULL -> keep the original call address
    return NULL;
}

void HookMSV4WLibs(HMODULE module) {
    HookLibraryEx(module, Hooks, "MSVFW32.dll");
}

#ifdef TRACEALL
LRESULT WINAPI extICSendMessage(HIC hic, UINT wMsg, DWORD_PTR dw1, DWORD_PTR dw2) {
    LRESULT res;
    ApiName("ICSendMessage");
    OutTraceSYS("%s: hic=%#x wMsg=%#x dw1=%#x dw2=%#x\n", ApiRef, hic, wMsg, dw1, dw2);
    res = (*pICSendMessage)(hic, wMsg, dw1, dw2);
    OutTraceSYS("%s: ret=%#x\n", ApiRef, res);
    return res;
}

#ifndef DXW_NOTRACES
static char *ExplainICModeFlags(WORD f) {
    char *t[] = {"", "COMPRESS", "DECOMPRESS", "FASTDECOMPRESS", "QUERY", "FASTCOMPRESS", "", "", "DRAW"};
    if(f <= ICMODE_DRAW) return t[f];
    return "";
}
#endif // DXW_NOTRACES

HIC WINAPI extICOpen(DWORD fccType, DWORD fccHandler, UINT wMode) {
    HIC res;
    ApiName("ICOpen");
    OutTraceSYS("%s: fccType=0x%X(\"%s\") fccHandler=%#x wMode=%#x(%s)\n",
                ApiRef, fccType, sFourCC(fccType), fccHandler, wMode, ExplainICModeFlags(wMode));
    res = (*pICOpen)(fccType, fccHandler, wMode);
    OutTraceSYS("%s: ret=%#x\n", ApiRef, res);
    return res;
}

LRESULT WINAPI extICClose(HIC hic) {
    LRESULT res;
    ApiName("ICClose");
    OutTraceSYS("%s: hic=%#x\n", ApiRef, hic);
    res = (*pICClose)(hic);
    OutTraceSYS("%s: ret=%#x\n", ApiRef, res);
    return res;
}

HIC VFWAPI extICLocate(DWORD fccType, DWORD fccHandler, LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut, WORD wflags) {
    HIC res;
    ApiName("ICLocate");
    OutTraceSYS("%s: fccType=%#x(%s) fccHandler=%#x(%s) flags=%#x(%s)\n",
                ApiRef,
                fccType, sFourCC(fccType),
                fccHandler, sFourCC(fccHandler),
                wflags, ExplainICModeFlags(wflags));
    if(IsTraceSYS) {
        OutTrace("> size=(%dx%d)\n", lpbiIn->biWidth, lpbiIn->biHeight);
        OutTrace("> planes=%d\n", lpbiIn->biPlanes);
        OutTrace("> bitcount=%d\n", lpbiIn->biBitCount);
        OutTrace("> compression=%#x(%s)\n", lpbiIn->biCompression, sFourCC(lpbiIn->biCompression));
        OutTrace("> sizeimage=%d\n", lpbiIn->biSizeImage);
        OutTrace("> pixelxmeter=(%dx%d)\n", lpbiIn->biXPelsPerMeter, lpbiIn->biYPelsPerMeter);
        OutTrace("> colors(used/imp)=%d/%d\n", lpbiIn->biClrUsed, lpbiIn->biClrImportant);
    }
    if(lpbiOut && IsTraceSYS) {
        OutTrace("< size=(%dx%d)\n", lpbiOut->biWidth, lpbiOut->biHeight);
        OutTrace("< planes=%d\n", lpbiOut->biPlanes);
        OutTrace("< bitcount=%d\n", lpbiOut->biBitCount);
        OutTrace("< compression=%#x(%s)\n", lpbiOut->biCompression, sFourCC(lpbiOut->biCompression));
        OutTrace("< sizeimage=%d\n", lpbiOut->biSizeImage);
        OutTrace("< pixelxmeter=(%dx%d)\n", lpbiOut->biXPelsPerMeter, lpbiOut->biYPelsPerMeter);
        OutTrace("< colors(used/imp)=%d/%d\n", lpbiOut->biClrUsed, lpbiOut->biClrImportant);
    }
    res = (*pICLocate)(fccType, fccHandler, lpbiIn, lpbiOut, wflags);
    OutTraceSYS("%s: ret=%#x\n", ApiRef, res);
    return res;
}
#endif // TRACEALL

/*
    ICDrawBegin function

    The ICDrawBegin function initializes the renderer and prepares the drawing destination for drawing.
    Syntax
    DWORD ICDrawBegin(
    HIC hic,
    DWORD dwFlags,
    HPALETTE hpal,
    HWND hwnd,
    HDC hdc,
    int xDst,
    int yDst,
    int dxDst,
    int dyDst,
    LPBITMAPINFOHEADER lpbi,
    int xSrc,
    int ySrc,
    int dxSrc,
    int dySrc,
    DWORD dwRate,
    DWORD dwScale
    );


    Parameters

    hic						Handle to the decompressor to use.

    dwFlags
    Decompression flags. The following values are defined.
    Value 				Meaning
    ICDRAW_ANIMATE		Application can animate the palette.
    ICDRAW_CONTINUE		Drawing is a continuation of the previous frame.
    ICDRAW_FULLSCREEN	Draws the decompressed data on the full screen.
    ICDRAW_HDC			Draws the decompressed data to a window or a DC.
    ICDRAW_MEMORYDC		DC is off-screen.
    ICDRAW_QUERY		Determines if the decompressor can decompress the data. The driver does not decompress the data.
    ICDRAW_UPDATING		Current frame is being updated rather than played.

    hpal					Handle to the palette used for drawing.

    hwnd					Handle to the window used for drawing.

    hdc						DC used for drawing.

    xDst					The x-coordinate of the upper right corner of the destination rectangle.
    yDst					The y-coordinate of the upper right corner of the destination rectangle.
    dxDst					Width of the destination rectangle.
    dyDst					Height of the destination rectangle.

    lpbi					Pointer to a BITMAPINFOHEADER structure containing the format of the input data to be decompressed.

    xSrc					The x-coordinate of the upper right corner of the source rectangle.
    ySrc					The y-coordinate of the upper right corner of the source rectangle.
    dxSrc					Width of the source rectangle.
    dySrc					Height of the source rectangle.

    dwRate					Frame rate numerator. The frame rate, in frames per second, is obtained by dividing dwRate by dwScale.
    dwScale					Frame rate denominator. The frame rate, in frames per second, is obtained by dividing dwRate by dwScale.

    Return value

    Returns ICERR_OK if the renderer can decompress the data or ICERR_UNSUPPORTED otherwise.
    Remarks

    The ICDRAW_HDC and ICDRAW_FULLSCREEN flags are mutually exclusive. If an application sets the ICDRAW_HDC flag in dwFlags, the decompressor uses hwnd, hdc, and the parameters defining the destination rectangle (xDst, yDst, dxDst, and dyDst). Your application should set these parameters to the size of the destination rectangle. Specify destination rectangle values relative to the current window or DC.
    If an application sets the ICDRAW_FULLSCREEN flag in dwFlags, the hwnd and hdc parameters are not used and should be set to NULL. Also, the destination rectangle is not used and its parameters can be set to zero.
    The source rectangle is relative to the full video frame. The portion of the video frame specified by the source rectangle is stretched or shrunk to fit the destination rectangle.
    The dwRate and dwScale parameters specify the decompression rate. The integer value specified for dwRate divided by the integer value specified for dwScale defines the frame rate in frames per second. This value is used by the renderer when it is responsible for timing frames during playback.

    Requirements
    Minimum supported client
	Windows 2000 Professional [desktop apps only]
    Minimum supported server
	Windows 2000 Server [desktop apps only]

    Header					Vfw.h
    Library					Vfw32.lib
    DLL						Msvfw32.dll

*/
#ifndef DXW_NOTRACES
static char *ExplainMCIWndCreateFlags(DWORD c) {
    static char eb[256];
    unsigned int l;
    strcpy(eb, "MCIWNDF_");
    if (c & MCIWNDF_NOAUTOSIZEWINDOW) strcat(eb, "NOAUTOSIZEWINDOW+");
    if (c & MCIWNDF_NOPLAYBAR) strcat(eb, "NOPLAYBAR+");
    if (c & MCIWNDF_NOAUTOSIZEMOVIE) strcat(eb, "NOAUTOSIZEMOVIE+");
    if (c & MCIWNDF_NOMENU) strcat(eb, "NOMENU+");
    if (c & MCIWNDF_SHOWNAME) strcat(eb, "SHOWNAME+");
    if (c & MCIWNDF_SHOWPOS) strcat(eb, "SHOWPOS+");
    if (c & MCIWNDF_SHOWMODE) strcat(eb, "SHOWMODE+");
    if (c & MCIWNDF_NOTIFYMODE) strcat(eb, "NOTIFYMODE+");
    if (c & MCIWNDF_NOTIFYPOS) strcat(eb, "NOTIFYPOS+");
    if (c & MCIWNDF_NOTIFYSIZE) strcat(eb, "NOTIFYSIZE+");
    if (c & MCIWNDF_NOTIFYERROR) strcat(eb, "NOTIFYERROR+");
    if (c & MCIWNDF_NOTIFYMEDIAW) strcat(eb, "NOTIFYMEDIAW+");
    if (c & MCIWNDF_NOTIFYANSI) strcat(eb, "NOTIFYANSI+");
    if (c & MCIWNDF_RECORD) strcat(eb, "RECORD+");
    if (c & MCIWNDF_NOERRORDLG) strcat(eb, "NOERRORDLG+");
    if (c & MCIWNDF_NOOPEN) strcat(eb, "NOOPEN+");
    l = strlen(eb);
    if (l > strlen("MCIWNDF_")) eb[l - 1] = 0; // delete last '+' if any
    else eb[0] = 0;
    return(eb);
}
#endif // DXW_NOTRACES

HWND extMCIWndCreateA(HWND hwndParent, HINSTANCE hInstance, DWORD dwStyle, LPCTSTR szFile) {
    HWND g_hwndMCIWnd;
    ApiName("MCIWndCreateA");
    OutTraceSYS("%s: hwnd=%#x hInst=%#x style=%#x(%s) file=%s\n",
                ApiRef, hwndParent, hInstance, dwStyle, ExplainMCIWndCreateFlags(dwStyle), szFile);
    if(dxw.dwFlags6 & NOMOVIES) {
        OutTraceDW("%s: SUPPRESSED\n", ApiRef);
        return NULL;
    }
    // v2.04.29 if REMAPMCI suppress the flags that prevent the video stretching
    if(dxw.dwFlags5 & REMAPMCI) {
        if(dwStyle & (MCIWNDF_NOAUTOSIZEWINDOW | MCIWNDF_NOAUTOSIZEMOVIE)) {
            OutTraceDW("%s: SUPPRESSED fixed size flags\n", ApiRef);
            dwStyle &= ~ (MCIWNDF_NOAUTOSIZEWINDOW | MCIWNDF_NOAUTOSIZEMOVIE);
        }
    }
    g_hwndMCIWnd = (*pMCIWndCreateA)(hwndParent, hInstance, dwStyle, szFile);
    // look at https://msdn.microsoft.com/en-us/library/windows/desktop/dd757178%28v=vs.85%29.aspx
    if(dxw.dwFlags5 & REMAPMCI) {
        // since there seem to be no way to stretch the movie, we do an attempt to center
        // the movie on the screen by shifting the video window.
        // We assume (but we don't really know for sure ...) that the movie was to be rendered
        // fulscreen using the size of the virtual desktop
        RECT client;
        OutTraceDW("%s: CENTERED\n", ApiRef);
        (*pGetClientRect)(dxw.GethWnd(), &client);
        (*pSetWindowPos)(g_hwndMCIWnd,					// window to resize
                         NULL,											// z-order: don't care
                         (client.right - dxw.GetScreenWidth()) / 2,		// X pos
                         (client.bottom - dxw.GetScreenHeight()) / 2,	// Y pos
                         dxw.GetScreenWidth(),							// width
                         dxw.GetScreenHeight(),							// height
                         SWP_NOZORDER | SWP_NOACTIVATE);
        //(*pUpdateWindow)(g_hwndMCIWnd);
    }
    return g_hwndMCIWnd;
}

HIC WINAPI extICGetDisplayFormat(HIC hic, LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut, int BitDepth, int dx, int dy) {
    HIC ret;
    ApiName("ICGetDisplayFormat");
    OutTraceSYS("%s: hic=%#x bitdepth=%d dx=%d dy=%d indepth=%d\n", ApiRef, hic, BitDepth, dx, dy, lpbiIn->biBitCount);
    ret = (*pICGetDisplayFormat)(hic, lpbiIn, lpbiOut, BitDepth, dx, dy);
    if(ret) {
        OutTraceSYS("%s: ret=%#x outdepth=%d\n", ApiRef, hic, lpbiOut->biBitCount);
        if(dxw.dwFlags6 & FIXMOVIESCOLOR) {
            lpbiOut->biBitCount = (WORD)dxw.VirtualPixelFormat.dwRGBBitCount;
            OutTraceDW("%s: FIXED outdepth=%d\n", ApiRef, lpbiOut->biBitCount);
        }
    } else
        OutTraceSYS("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

DWORD WINAPIV extICDrawBegin(HIC hic, DWORD dwFlags, HPALETTE hpal, HWND hwnd,  HDC hdc, int xDst, int yDst, int dxDst, int dyDst, LPBITMAPINFOHEADER lpbi, int xSrc, int ySrc, int dxSrc, int dySrc, DWORD dwRate, DWORD dwScale) {
    DWORD ret;
    ApiName("ICDrawBegin");
    OutTraceSYS("%s: flags=%#x hpal=%#x hwnd=%#x hdc=%#x\n"
                "> dest:{pos=(%d,%d) siz=(%dx%d)}\n"
                "> src :{pos=(%d,%d) siz=(%dx%d)}\n"
                "> rate=%d scale=%d\n",
                ApiRef, dwFlags, hpal, hwnd, hdc,
                xDst, yDst, dxDst, dyDst,
                xSrc, ySrc, dxSrc, dySrc,
                dwRate, dwScale);
    if(dxw.Windowize) {
        // no fullscreen ... to be tested
        if(dwFlags & ICDRAW_FULLSCREEN) {
            OutTraceDW("%s: ICDRAW_FULLSCREEN case\n", ApiRef);
            dwFlags &= ~ICDRAW_FULLSCREEN;
            dwFlags |= ICDRAW_HDC;
            hwnd = dxw.GethWnd();
            hdc = NULL;
        }
        if(dxw.GDIEmulationMode == GDIMODE_STRETCHED)
            dxw.MapClient(&xDst, &yDst, &dxDst, &dyDst);
    }
    ret = (*pICDrawBegin)(hic, dwFlags, hpal, hwnd, hdc, xDst, yDst, dxDst, dyDst, lpbi, xSrc, ySrc, dxSrc, dySrc, dwRate, dwScale);
    OutTraceSYS("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

#ifdef TRACEALL
DWORD VFWAPIV extICDecompress(HIC hic, DWORD dwFlags, LPBITMAPINFOHEADER lpbiFormat, LPVOID lpData, LPBITMAPINFOHEADER lpbi, LPVOID lpBits) {
    DWORD ret;
    ApiName("ICDecompress");
    OutTraceSYS("%s: hic=%#x flags=%#x data=%#x bits=%#x\n",
                ApiRef, hic, dwFlags, lpData, lpBits);
    OutTraceSYS("> IN : size=(%dx%d) bpp=%d comp=%#x(%s) size=%d\n",
                lpbiFormat->biWidth, lpbiFormat->biHeight,
                lpbiFormat->biBitCount,
                lpbiFormat->biCompression, sFourCC(lpbiFormat->biCompression),
                lpbiFormat->biSizeImage);
    ret = (*pICDecompress)(hic, dwFlags, lpbiFormat, lpData, lpbi, lpBits);
    if(ret == ICERR_OK) {
        OutTraceSYS("> OUT: size=(%dx%d) bpp=%d comp=%#x(%s) size=%d\n",
                    lpbi->biWidth, lpbi->biHeight,
                    lpbi->biBitCount,
                    lpbi->biCompression, sFourCC(lpbi->biCompression),
                    lpbi->biSizeImage);
    }
    OutTraceSYS("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}
#endif

BOOL WINAPI extDrawDibDraw(HDRAWDIB hdd, HDC hdc, int xDst, int yDst, int dxDst, int dyDst, LPBITMAPINFOHEADER lpbi, LPVOID lpBits, int xSrc, int ySrc, int dxSrc, int dySrc, UINT wFlags) {
    // Reah intro movie
    // eXtreme Bullrider intro movie ??
    BOOL ret;
    ApiName("DrawDibDraw");
    OutTraceSYS("%s: hdd=%#x hdc=%#x DEST pos=(%d,%d) size=(%d,%d) SRC pos=(%d,%d) size=(%d,%d) flags=%#x\n",
                ApiRef, hdd, hdc, xDst, yDst, dxDst, dyDst, xSrc, ySrc, dxSrc, dySrc, wFlags);
    if(dxw.GDIEmulationMode == GDIMODE_STRETCHED)
        dxw.MapClient(&xDst, &yDst, &dxDst, &dyDst);
    ret = (*pDrawDibDraw)(hdd, hdc, xDst, yDst, dxDst, dyDst, lpbi, lpBits, xSrc, ySrc, dxSrc, dySrc, wFlags);
#ifndef DXW_NOTRACES
    if(!ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
#endif // DXW_NOTRACES
    return ret;
}

BOOL WINAPI extDrawDibBegin(HDRAWDIB hdd, HDC hdc, int dxDest, int dyDest, LPBITMAPINFOHEADER lpbi, int dxSrc, int dySrc, UINT wFlags) {
    // Reah game transitions
    BOOL ret;
    ApiName("DrawDibBegin");
    OutTraceSYS("%s: hdd=%#x hdc=%#x DEST size=(%d,%d) SRC size=(%d,%d) flags=%#x\n",
                ApiRef, hdd, hdc, dxDest, dyDest, dxSrc, dySrc, wFlags);
    if(dxw.GDIEmulationMode == GDIMODE_STRETCHED)
        dxw.MapClient(&dxDest, &dyDest);
    ret = (*pDrawDibBegin)(hdd, hdc, dxDest, dyDest, lpbi, dxSrc, dySrc, wFlags);
#ifndef DXW_NOTRACES
    if(!ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
#endif // DXW_NOTRACES
    return ret;
}

#ifdef TRACEALL
BOOL WINAPI extDrawDibStart(HDRAWDIB hdd, LONG rate) {
    // Reah game transitions
    BOOL ret;
    ApiName("DrawDibStart");
    OutTraceSYS("%s: hdd=%#x rate=%#x\n", ApiRef, hdd, rate);
    ret = (*pDrawDibStart)(hdd, rate);
    if(!ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

BOOL WINAPI extDrawDibStop(HDRAWDIB hdd) {
    // Reah game transitions
    BOOL ret;
    ApiName("DrawDibStop");
    OutTraceSYS("%s: hdd=%#x\n", ApiRef, hdd);
    ret = (*pDrawDibStop)(hdd);
    if(!ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

BOOL WINAPI extDrawDibEnd(HDRAWDIB hdd) {
    // Reah game transitions
    BOOL ret;
    ApiName("DrawDibEnd");
    OutTraceSYS("%s: hdd=%#x\n", ApiRef, hdd);
    ret = (*pDrawDibEnd)(hdd);
    if(!ret) OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
    return ret;
}

HDRAWDIB WINAPI extDrawDibOpen(void) {
    HDRAWDIB ret;
    ApiName("DrawDibOpen");
    ret = (*pDrawDibOpen)();
    OutTraceSYS("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

BOOL WINAPI extDrawDibClose(HDRAWDIB hdd) {
    BOOL ret;
    ApiName("DrawDibClose");
    ret = (*pDrawDibClose)(hdd);
    OutTraceSYS("%s: hdd=%#x ret=%#x\n", ApiRef, hdd, ret);
    return ret;
}
#endif // TRACEALL


