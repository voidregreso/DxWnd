#define _CRT_SECURE_NO_WARNINGS
#define INITGUID
//#define FULLHEXDUMP
#define USETEXTURERECTANGLE

#include <windows.h>
#include <ddraw.h>
#include "dxwnd.h"
#include "ddrawi.h"
#include "dxwcore.hpp"
#include "stdio.h"
#include "hddraw.h"
#include "dxhelper.h"
#include "syslibs.h"
#include <wingdi.h>
#include <gl.h>

extern GetDC_Type pGetDCMethod();
extern ReleaseDC_Type pReleaseDCMethod();
extern Lock_Type pLockMethod(int);
extern Unlock4_Type pUnlockMethod(int);
extern SetClipper_Type pSetClipperMethod(int);
extern LPVOID dxwConvertFourCC(LPDDSURFACEDESC2);
extern DWORD PaletteEntries[];

typedef HGLRC(WINAPI *wglCreateContext_Type)(HDC);
typedef HGLRC(WINAPI *wglGetCurrentContext_Type)(void);
typedef BOOL (WINAPI *wglDeleteContext_Type)(HGLRC);
typedef BOOL (WINAPI *wglMakeCurrent_Type)(HDC, HGLRC);
typedef void (WINAPI *glViewport_Type)(GLint, GLint, GLsizei, GLsizei);
typedef void (WINAPI *glGenTextures_Type)(GLsizei, GLuint *);
typedef void (WINAPI *glBindTexture_Type)(GLenum, GLuint);
typedef void (WINAPI *glTexImage2D_type)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid *);
typedef void (WINAPI *glBegin_Type)(GLenum);
typedef void (WINAPI *glEnd_Type)(void);
typedef void (WINAPI *glTexCoord2i_Type)(GLint, GLint);
typedef void (WINAPI *glTexParameteri_Type)(GLenum, GLenum, GLint);
typedef void (WINAPI *glEnum_Type)(GLenum);
typedef void (WINAPI *glMatrixMode_Type)(GLenum);
typedef void (WINAPI *glLoadIdentity_Type)(void);
typedef void (WINAPI *glClearColor_Type)(GLclampf, GLclampf, GLclampf, GLclampf);
typedef void (WINAPI *glClear_Type)(GLbitfield);
typedef void (WINAPI *glColor3f_Type)(GLfloat, GLfloat, GLfloat);
typedef void (WINAPI *glColor4f_Type)(GLfloat, GLfloat, GLfloat, GLfloat);
typedef void (WINAPI *glTexCoord2f_Type)(GLfloat, GLfloat);
typedef void (WINAPI *glVertex2f_Type)(GLfloat, GLfloat);
typedef void (WINAPI *glFinish_Type)(void);
typedef void (WINAPI *glBlendFunc_Type)(GLenum, GLenum);

typedef enum {
    OGL_ROTATION_0 = 0,
    OGL_ROTATION_90,
    OGL_ROTATION_180,
    OGL_ROTATION_270
};

//#define OGLROTATION
#ifdef OGLROTATION
static int oglRotation = OGL_ROTATION_180;
#endif // OGLROTATION

#ifdef USETEXTURERECTANGLE
static int uppower2(int size) {
    return ((size >> 2) << 2);
}
#else
static int uppower2(int size) {
    int shift;
    for(shift = 0; size; shift++) size >>= 1;
    return 1 << shift;
}
#endif

static DWORD *oglPalette16BPP = NULL;

static void oglSetPalette16BPP() {
    unsigned int pi;
    OutTraceOGL("oglSetPalette16BPP\n");
    oglPalette16BPP = (DWORD *)malloc(0x10000 * sizeof(DWORD));
    if (dxw.dwFlags3 & BLACKWHITE) {
        // actually, it should be like this: R/G/B = (red * 0.30) + (green * 0.59) + (blue * 0.11)
        // (http://www.codeproject.com/Articles/66253/Converting-Colors-to-Gray-Shades)
        DWORD grey;
        if (dxw.dwFlags1 & USERGB565) {
            for (pi = 0; pi < 0x10000; pi++) {
                grey = (((((pi & 0x1F) << 3) * 30) + (((pi & 0x7E0) >> 3) * 59) + (((pi & 0xF800) >> 8) * 11)) / 100) & 0xFF;
                oglPalette16BPP[pi] = (grey << 16) + (grey << 8) + (grey);
            }
        } else {
            for (pi = 0; pi < 0x10000; pi++) {
                grey = (((((pi & 0x1F) << 3) * 30) + (((pi & 0x3E0) >> 2) * 59) + (((pi & 0x7C00) >> 7) * 11)) / 100) & 0xFF;
                oglPalette16BPP[pi] = (grey << 16) + (grey << 8) + (grey);
            }
        }
    } else {
        if (dxw.dwFlags1 & USERGB565) {
            for (pi = 0; pi < 0x10000; pi++) {
                oglPalette16BPP[pi] = (pi & 0x1F) << 19 | (pi & 0x7E0) << 5 | (pi & 0xF800) >> 8; // RGB565
            }
        } else {
            for (pi = 0; pi < 0x10000; pi++) {
                oglPalette16BPP[pi] = (pi & 0x1F) << 19 | (pi & 0x3E0) << 6 | (pi & 0x7C00) >> 7; // RGB555
            }
        }
    }
}

static void oglTransform8Pal(LPVOID pixels, LPVOID texbuf, UINT w, UINT h, UINT tpitch, UINT lPitch) {
    BYTE *p8 = (LPBYTE)pixels;
    DWORD *t32 = (LPDWORD)texbuf;
    // build a reversed palette array
    DWORD quads[256];
    for(UINT c = 0; c < 256; c++) {
        DWORD pixel = PaletteEntries[c];
        quads[c] = ((pixel & 0xFF) << 16) | (pixel & 0xFF00) | ((pixel & 0xFF0000) >> 16);
    }
    // convert 8bpp pixels to 32bpp using palette
    for(UINT y = 0; y < h; y++) {
        for(UINT x = 0; x < w; x++)
            t32[y * tpitch + x] = quads[*(p8 ++)];
        // v2.05.18: added pitch compensation
        p8 += (lPitch - w);
    }
}

static void oglTransform16(LPVOID pixels, LPVOID texbuf, UINT w, UINT h, UINT tpitch, UINT lPitch) {
    WORD *p16 = (LPWORD)pixels;
    DWORD *t32 = (LPDWORD)texbuf;
    // build a reversed fale-16bit palette array
    if (!oglPalette16BPP) oglSetPalette16BPP();
    // convert 8bpp pixels to 32bpp using palette
    for(UINT y = 0; y < h; y++) {
        for(UINT x = 0; x < w; x++)
            t32[y * tpitch + x] = oglPalette16BPP[*(p16 ++)];
        // v2.05.18: added pitch compensation
        p16 += ((lPitch >> 1) - w);
    }
}

static void oglTransform24(LPVOID pixels, LPVOID texbuf, UINT w, UINT h, UINT tpitch, UINT lPitch) {
    LPBYTE p8 = (LPBYTE)pixels;
    LPBYTE t8 = (LPBYTE)texbuf;
    // v2.05.25: new color depth
    for(UINT y = 0; y < h; y++) {
        t8 = (LPBYTE)texbuf + (y * tpitch * sizeof(DWORD)); // tpitch is in DWORDs !!
        p8 = (LPBYTE)pixels + (y * lPitch);
        for(UINT x = 0; x < w; x++) {
            *(t8)     = *(p8 + 2);
            *(t8 + 1) = *(p8 + 1);
            *(t8 + 2) = *(p8);
            *(t8 + 3) = 0;
            t8 += 4;
            p8 += 3;
        }
    }
}

static void oglTransform32(LPVOID pixels, LPVOID texbuf, UINT w, UINT h, UINT tpitch, UINT lPitch) {
    LPBYTE p8 = (LPBYTE)pixels;
    LPBYTE t8 = (LPBYTE)texbuf;
    // just move pixels
    // v2.05.24: fix - RGBA must be byte swapped to BGRA !!
    for(UINT y = 0; y < h; y++) {
        t8 = (LPBYTE)texbuf + (y * tpitch * sizeof(DWORD)); // tpitch is in DWORDs !!
        p8 = (LPBYTE)pixels + (y * lPitch);
        for(UINT x = 0; x < w; x++) {
            *(t8)     = *(p8 + 2);
            *(t8 + 1) = *(p8 + 1);
            *(t8 + 2) = *(p8);
            *(t8 + 3) = 0;
            t8 += 4;
            p8 += 4;
        }
    }
}

HRESULT OGL_BlitToWindow(int dxversion, Blt_Type pBlt, char *api, LPDIRECTDRAWSURFACE lpdds, LPRECT lpdestrect,
                         LPDIRECTDRAWSURFACE s, LPRECT lpsrcrect, DWORD dwflags, LPDDBLTFX lpddbltfx, BOOL isFlipping) {
    static BOOL bInitialized = FALSE;
    static LONG CurW = 0, CurH = 0;
    ULONG PicW, PicH;
    RECT Rect;
    DDSURFACEDESC2 ddsd;
    HRESULT res;
    HWND hwnd;
    HGLRC oglContext;
    GLuint Tex;
    int tex_w, tex_h;
    LPVOID texbuf;
    static BOOL bRebuildContext = TRUE;
    HDC gWindowDC;
    static wglCreateContext_Type pwglCreateContext;
    static wglGetCurrentContext_Type pwglGetCurrentContext;
    static wglDeleteContext_Type pwglDeleteContext;
    static wglMakeCurrent_Type pwglMakeCurrent;
    static glViewport_Type pglViewport;
    static glGenTextures_Type pglGenTextures;
    static glBindTexture_Type pglBindTexture;
    static glTexImage2D_type pglTexImage2D;
    static glBegin_Type pglBegin;
    static glEnd_Type pglEnd;
    static glTexCoord2i_Type pglTexCoord2i;
    static glTexParameteri_Type pglTexParameteri;
    static glEnum_Type pglEnable, pglDisable;
    static glEnum_Type pglShadeModel;
    static glMatrixMode_Type pglMatrixMode;
    static glLoadIdentity_Type pglLoadIdentity;
    static glClearColor_Type pglClearColor;
    static glClear_Type pglClear;
    static glColor3f_Type pglColor3f;
    static glTexCoord2f_Type pglTexCoord2f;
    static glVertex2f_Type pglVertex2f;
    static glFinish_Type pglFinish;
    static glColor4f_Type pglColor4f;
    static glBlendFunc_Type pglBlendFunc;
    // initialization, just once and for all
    if(!bInitialized) {
        HMODULE hOGL;
        // load SDL function pointers
        hOGL = (*pLoadLibraryA)("opengl32.dll");
        if(!hOGL) {
            OutTraceE("%s: load opengl32.dll FAILED\n", api);
            return DD_FALSE;
        }
        pwglCreateContext = (wglCreateContext_Type)(*pGetProcAddress)(hOGL, "wglCreateContext");
        pwglGetCurrentContext = (wglGetCurrentContext_Type)(*pGetProcAddress)(hOGL, "wglGetCurrentContext");
        pwglDeleteContext = (wglDeleteContext_Type)(*pGetProcAddress)(hOGL, "wglDeleteContext");
        pwglMakeCurrent = (wglMakeCurrent_Type)(*pGetProcAddress)(hOGL, "wglMakeCurrent");
        pglViewport = (glViewport_Type)(*pGetProcAddress)(hOGL, "glViewport");
        pglGenTextures = (glGenTextures_Type)(*pGetProcAddress)(hOGL, "glGenTextures");
        pglBindTexture = (glBindTexture_Type)(*pGetProcAddress)(hOGL, "glBindTexture");
        pglTexImage2D = (glTexImage2D_type)(*pGetProcAddress)(hOGL, "glTexImage2D");
        pglBegin = (glBegin_Type)(*pGetProcAddress)(hOGL, "glBegin");
        pglEnd = (glEnd_Type)(*pGetProcAddress)(hOGL, "glEnd");
        pglTexCoord2i = (glTexCoord2i_Type)(*pGetProcAddress)(hOGL, "glTexCoord2i");
        pglTexParameteri = (glTexParameteri_Type)(*pGetProcAddress)(hOGL, "glTexParameteri");
        pglEnable = (glEnum_Type)(*pGetProcAddress)(hOGL, "glEnable");
        pglDisable = (glEnum_Type)(*pGetProcAddress)(hOGL, "glDisable");
        pglShadeModel = (glEnum_Type)(*pGetProcAddress)(hOGL, "glShadeModel");
        pglMatrixMode = (glMatrixMode_Type)(*pGetProcAddress)(hOGL, "glMatrixMode");
        pglLoadIdentity = (glLoadIdentity_Type)(*pGetProcAddress)(hOGL, "glLoadIdentity");
        pglClearColor = (glClearColor_Type)(*pGetProcAddress)(hOGL, "glClearColor");
        pglClear = (glClear_Type)(*pGetProcAddress)(hOGL, "glClear");
        pglColor3f = (glColor3f_Type)(*pGetProcAddress)(hOGL, "glColor3f");
        pglTexCoord2f = (glTexCoord2f_Type)(*pGetProcAddress)(hOGL, "glTexCoord2f");
        pglVertex2f = (glVertex2f_Type)(*pGetProcAddress)(hOGL, "glVertex2f");
        pglFinish = (glFinish_Type)(*pGetProcAddress)(hOGL, "glFinish");
        pglColor4f = (glColor4f_Type)(*pGetProcAddress)(hOGL, "glColor4f");
        pglBlendFunc = (glBlendFunc_Type)(*pGetProcAddress)(hOGL, "glBlendFunc");
        if(!(pwglCreateContext && pwglDeleteContext && pwglMakeCurrent && pglViewport &&
                pwglGetCurrentContext && pglGenTextures && pglBindTexture &&
                pglTexImage2D && pglBegin && pglEnd && pglTexCoord2i && pglTexParameteri &&
                pglEnable && pglDisable && pglShadeModel && pglMatrixMode && pglLoadIdentity &&
                pglClearColor && pglClear && pglColor3f && pglTexCoord2f && pglVertex2f)) {
            OutTraceE("%s: ERROR initialize opengl32 failed\n", api);
            return DD_FALSE;
        }
        (*pglGenTextures)(1, &Tex);
        (*pglBindTexture)(GL_TEXTURE_2D, Tex);
        OutTraceOGL("%s: opengl renderer initialized\n", api);
        bInitialized = TRUE;
    }
    hwnd = dxw.GethWnd();
    gWindowDC = (*pGDIGetDC)(hwnd);
    // v2.05.43: what is the correct criteria to renew the opengl context?
    // ddhack doesn't pose the question: the WC games only require a single context created at startup
    // this is equivalent here to line "method 1:", oglContext set only once first time through
    // but this doesn't work in general if you change something, like window size or color depth
    // line "method 2:" was the previous DxWnd implementation, context is rebuilt every time it has a different value
    // but this leads to repeated and unnecessary context rebuild and a crash in "A.M.E.R.I.C.A No Peace Beyond the Line"
    // current solution is to rebuild the context whenever a valid context no longer exists.
    // method 1: if(oglContext == (HGLRC)-1){
    // method 2: if((*pwglGetCurrentContext)() != oglContext){
    //if((*pwglGetCurrentContext)() == NULL){
    // v2.05.46: it still happens that the context gets invalidated. Setting gWindowDC to NULL is a criteria
    // to notify the blitter loop to rebuild a new context. This makes "Re-Volt" working also after switching
    // between 2D and 3D modes.
    // v2.05.48: revised all behavior, taking in proper account the fact that a glGetCurrentContext returning NULL
    // with 0 as errorcode means a valid opengl rendering context. Should reduce the hdc leakage in DK2.
    SetLastError(0);
    oglContext = (*pwglGetCurrentContext)();
    if((oglContext == NULL) && GetLastError()) {
        OutTrace("wglGetCurrentContext returns NULL err=%d\n", GetLastError());
        bRebuildContext = TRUE;
    }
    if(bRebuildContext) {
        PIXELFORMATDESCRIPTOR pfd = {
            sizeof(PIXELFORMATDESCRIPTOR),
            1,
            PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
            PFD_TYPE_RGBA,
            24,
            0, 0, 0, 0, 0, 0, 0, 0, // RGBA sizes & shifts
            0, 0, 0, 0, // accum bits
            (BYTE)dxw.ActualPixelFormat.dwRGBBitCount, //32,
            0, 0,
            PFD_MAIN_PLANE,
            0, 0, 0, 0
        };
        int kount = 0;
        do {
            int iPixelFormat;
            kount++;
            iPixelFormat = ChoosePixelFormat(gWindowDC, &pfd);
            OutTraceOGL("%s: pixel format = %d\n", api, iPixelFormat);
            if(!SetPixelFormat(gWindowDC, iPixelFormat, &pfd))
                OutTraceOGL("%s: SetPixelFormat failed err=%d at=%d\n", api, GetLastError(), __LINE__);
            oglContext = (*pwglCreateContext)(gWindowDC);
            if(!oglContext && (kount > 5)) {
                OutTraceOGL("%s: ERROR glCreateContext failed err=%d at=%d\n", api, GetLastError(), __LINE__);
                (*pGDIReleaseDC)(hwnd, gWindowDC);
                return DD_FALSE;
            }
            if(oglContext != NULL) break;
            Sleep(20);
        } while(TRUE);
        (*pwglMakeCurrent)(gWindowDC, oglContext);
        OutTraceOGL("%s: opengl context initialized hdc=%#x context=%#x\n", api, gWindowDC, oglContext);
        bRebuildContext = FALSE;
    }
    // clear the clipper to avoid conflicts with the ogl Viewport
    // fixes "Age of Empires" partial blitting
    // warning: this DOESN'T fix "Abuse", you still have to disable video clipper.
    extern SetClipper_Type pSetClipper1, pSetClipper2, pSetClipper3, pSetClipper4, pSetClipper7;
    switch(dxversion) {
    case 1:
        if(pSetClipper1) (*pSetClipper1)(lpdds, NULL);
        break;
    case 2:
        if(pSetClipper2) (*pSetClipper2)(lpdds, NULL);
        break;
    case 3:
        if(pSetClipper3) (*pSetClipper3)(lpdds, NULL);
        break;
    case 4:
        if(pSetClipper4) (*pSetClipper4)(lpdds, NULL);
        break;
    case 7:
        if(pSetClipper7) (*pSetClipper7)(lpdds, NULL);
        break;
    }
#if 0 // debug
    LPRECT p = lpdestrect;
    if (p)
        OutTrace(">>> dstrect=(%d,%d)-(%d,%d)\n", p->left, p->top, p->right, p->bottom);
    else
        OutTrace(">>> dstrect=NULL\n");
    p = lpsrcrect;
    if (p)
        OutTrace(">>> srcrect=(%d,%d)-(%d,%d)\n", p->left, p->top, p->right, p->bottom);
    else
        OutTrace(">>> srcrect=NULL\n");
    OutTrace(">>> flags=%#x fx=%#x\n", dwflags, lpddbltfx);
#endif // end debug
    res = (*pBlt)(lpdds, lpdestrect, s, lpsrcrect, dwflags, lpddbltfx);
    if(res != DD_OK) {
        OutTraceE("%s: opengl Blt ERROR err=%#x(%s)\n", api, res, ExplainDDError(res));
        if(res == DDERR_UNSUPPORTED) {
            // do not desist, try blitting directly from source
            lpdds = s;
        } else {
            (*pGDIReleaseDC)(hwnd, gWindowDC);
            return res;
        }
    } else {
        // don't show overlay on s surface!!
        dxw.ShowOverlay(lpdds);
    }
    // check for size changes (including initial call)
    (*pGetClientRect)(hwnd, &Rect);
    if((Rect.right != CurW) || (Rect.bottom != CurH)) {
        CurW = Rect.right;
        CurH = Rect.bottom;
        // change Viewport ???
        (*pglViewport)(0, 0, CurW, CurH);
        OutTraceOGL("%s: set video hwnd=%#x size=(%dx%d)\n", api, hwnd, CurW, CurH);
    }
    // get surface infos & lock
    memset(&ddsd, 0, sizeof(DDSURFACEDESC2));
    ddsd.dwSize = Set_dwSize_From_Surface();
    ddsd.dwFlags = DDSD_LPSURFACE | DDSD_PITCH;
    if(res = (*pLockMethod(dxversion))(lpdds, 0, (LPDDSURFACEDESC)&ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_READONLY, 0)) {
        OutTraceE("%s: Lock ERROR res=%#x(%s) at %d\n", api, res, ExplainDDError(res), __LINE__);
        (*pUnlockMethod(dxversion))(lpdds, NULL);
        (*pGDIReleaseDC)(hwnd, gWindowDC);
        return DD_FALSE;
    }
    OutTraceOGL("%s: surface size=(%dx%d) data=%#x pitch=%#x bpp=%d fourcc=%#x rgba=%#x.%#x.%#x.%#x\n",
                api, ddsd.dwWidth, ddsd.dwHeight, ddsd.lpSurface, ddsd.lPitch,
                ddsd.ddpfPixelFormat.dwRGBBitCount, ddsd.ddpfPixelFormat.dwFourCC,
                ddsd.ddpfPixelFormat.dwRBitMask, ddsd.ddpfPixelFormat.dwGBitMask, ddsd.ddpfPixelFormat.dwBBitMask,
                ddsd.ddpfPixelFormat.dwRGBAlphaBitMask);
    // save the picture size
    PicW = ddsd.dwWidth;
    PicH = ddsd.dwHeight;
    // convert FourCC format (if possible)
    LPVOID pixels = ddsd.lpSurface;
    if(ddsd.ddpfPixelFormat.dwFourCC) pixels = dxwConvertFourCC(&ddsd);
    if(!pixels) {
        OutTraceE("%s: ConvertFourCC ERROR\n", api);
        (*pGDIReleaseDC)(hwnd, gWindowDC);
        return DD_FALSE;
    }
    tex_w = uppower2(ddsd.dwWidth);
    tex_h = uppower2(ddsd.dwHeight);
    OutDebugOGL("%s: surface size=(%dx%d) texture size=(%dx%d)\n",
                api, ddsd.dwWidth, ddsd.dwHeight, tex_w, tex_h);
    texbuf = malloc(tex_w * tex_h * sizeof(DWORD));
    if(!texbuf) {
        OutTraceE("%s: ERROR texture buffer alloc failed at=%d\n", api, __LINE__);
        (*pGDIReleaseDC)(hwnd, gWindowDC);
        return DD_FALSE;
    }
    switch(ddsd.ddpfPixelFormat.dwRGBBitCount) {
    case 8:
        oglTransform8Pal(pixels, texbuf, ddsd.dwWidth, ddsd.dwHeight, tex_w, ddsd.lPitch);
        break;
    case 16:
        oglTransform16(pixels, texbuf, ddsd.dwWidth, ddsd.dwHeight, tex_w, ddsd.lPitch);
        break;
    case 24:
        oglTransform24(pixels, texbuf, ddsd.dwWidth, ddsd.dwHeight, tex_w, ddsd.lPitch);
        break;
    case 32:
        oglTransform32(pixels, texbuf, ddsd.dwWidth, ddsd.dwHeight, tex_w, ddsd.lPitch);
        break;
    default:
        MessageBox(NULL, "bad pixel color depth", "DxWnd", 0);
        break;
    }
    // V2.05.24: despite all declarations, the Windows implementation of OpenGL glTexImage2D, though
    // with valid arguments, COULD throw an exception! In that case the best policy is to ignore the
    // problem and keep going. It happens in "Dungeon Keeper 2" DKII-DX.EXE and this workaround fixes it.
    __try {
        (*pglTexImage2D)(
            GL_TEXTURE_2D,
            0,
            GL_RGB,
            tex_w, tex_h,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            texbuf);
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        OutTraceE("%s: ERROR glTexImage2D exception at=%d\n", api, __LINE__);
        free(texbuf);
        (*pUnlockMethod(dxversion))(lpdds, NULL);
        (*pGDIReleaseDC)(hwnd, gWindowDC);
        return DD_FALSE;
    }
    if(dxw.dwFlags5 & BILINEARFILTER) {
        (*pglTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        (*pglTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    } else {
        (*pglTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        (*pglTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
    (*pglEnable)(GL_TEXTURE_2D);
    (*pglShadeModel)(GL_SMOOTH);
    (*pglClearColor)(0.0f, 0.0f, 0.0f, 0.0f);
    (*pglViewport)(0, 0, CurW, CurH); // necessary here?
    (*pglMatrixMode)(GL_PROJECTION);
    (*pglLoadIdentity)();
    (*pglMatrixMode)(GL_MODELVIEW);
    (*pglLoadIdentity)();
    if(dxw.dwFlags12 & BLUREFFECT) {
        (*pglEnable)(GL_BLEND);
        (*pglColor4f)(1.0f, 1.0f, 1.0f, 1.5f / (4 + 1.0f));
        (*pglBlendFunc)(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    } else {
        (*pglClear)(GL_COLOR_BUFFER_BIT);
        (*pglDisable)(GL_BLEND);
        (*pglColor3f)(1.0f, 1.0f, 1.0f);
    }
    // Do the actual rendering.
    float w = 1.0f, h = 1.0f;
    // Handle the fact that while our texture is a power of two,
    // the area we're using isn't.
    float wRatio = (float)PicW / (float)tex_w;
    float hRatio = (float)PicH / (float)tex_h;
    // beware of float rounding errors, anything greater than 1.0 means overflow
    if(wRatio > 1.0f) wRatio = 1.0f;
    if(hRatio > 1.0f) hRatio = 1.0f;
    //if(pglFinish) (*pglFinish)();
    (*pglBegin)(GL_TRIANGLE_FAN);
#ifdef OGLROTATION
    switch(oglRotation) {
    case OGL_ROTATION_0:
        (*pglTexCoord2f)(0, 0);
        (*pglVertex2f)( -w,  h);
        (*pglTexCoord2f)(wRatio, 0);
        (*pglVertex2f)(  w,  h);
        (*pglTexCoord2f)(wRatio, hRatio);
        (*pglVertex2f)(  w, -h);
        (*pglTexCoord2f)(0, hRatio);
        (*pglVertex2f)( -w, -h);
        break;
    case OGL_ROTATION_90:
        (*pglTexCoord2f)(wRatio, 0);
        (*pglVertex2f)(  w, -h);
        (*pglTexCoord2f)(wRatio, hRatio);
        (*pglVertex2f)( -w, -h);
        (*pglTexCoord2f)(0, hRatio);
        (*pglVertex2f)( -w,  h);
        (*pglTexCoord2f)(0, 0);
        (*pglVertex2f)(  w,  h);
        break;
    case OGL_ROTATION_180:
        (*pglTexCoord2f)(0, 0);
        (*pglVertex2f)(  w, -h);
        (*pglTexCoord2f)(wRatio, 0);
        (*pglVertex2f)( -w, -h);
        (*pglTexCoord2f)(wRatio, hRatio);
        (*pglVertex2f)( -w,  h);
        (*pglTexCoord2f)(0, hRatio);
        (*pglVertex2f)(  w,  h);
        break;
    case OGL_ROTATION_270:
        (*pglTexCoord2f)(wRatio, 0);
        (*pglVertex2f)( -w,  h);
        (*pglTexCoord2f)(wRatio, hRatio);
        (*pglVertex2f)(  w,  h);
        (*pglTexCoord2f)(0, hRatio);
        (*pglVertex2f)(  w, -h);
        (*pglTexCoord2f)(0, 0);
        (*pglVertex2f)( -w, -h);
        break;
    }
#else
    (*pglTexCoord2f)(0.0f, 0.0f);
    (*pglVertex2f)( -w,  h);
    (*pglTexCoord2f)(wRatio, 0.0f);
    (*pglVertex2f)(  w,  h);
    (*pglTexCoord2f)(wRatio, hRatio);
    (*pglVertex2f)(  w, -h);
    (*pglTexCoord2f)(0.0f, hRatio);
    (*pglVertex2f)( -w, -h);
#endif // OGLROTATION
    (*pglEnd)();
    free(texbuf);
    if(!SwapBuffers(gWindowDC))
        OutTraceE("%s: SwapBuffers(%#x) ERROR err=%d\n", api, gWindowDC, GetLastError());
    // unlock
    res = (*pUnlockMethod(dxversion))(lpdds, NULL);
    (*pGDIReleaseDC)(hwnd, gWindowDC);
#ifndef DXW_NOTRACES
    if (res) OutTraceE("%s: Unlock ERROR res=%#x(%s) at %d\n", api, res, ExplainDDError(res), __LINE__);
#endif // DXW_NOTRACES
    if((dxw.dwDFlags & CAPTURESCREENS) && dxw.bCustomKeyToggle) dxw.ScreenShot();
    return DD_OK;
}

