#define  _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include <winver.h>
#include "dxwnd.h"
#include "dxwcore.hpp"
#include "syslibs.h"
#include "dxhook.h"

#include "bink.h"

#ifndef BINK_SOUND_FUNC
typedef s32 (*BINK_SOUND_FUNC)(s32);
#endif

#define BINKVERSIONUNKNOWN 0
#define BINKVERSION0 1
#define BINKVERSION1 2
static void InitializeBinkInterface(void);

int _BinkVersion = BINKVERSIONUNKNOWN;

//#define TRACEALL

typedef HBINK (WINAPI *BinkOpen_Type)(const char *, U32); //
typedef void  (WINAPI *BinkClose_Type)(HBINK); //
typedef void (WINAPI *BinkGoto_Type)(HBINK, U32, S32);
#ifdef TRACEALL
typedef HBINK (WINAPI *Binker_Type)(HBINK);
typedef HBINK (WINAPI *Binker2_Type)(HBINK, U32);
typedef S32   (WINAPI *BinkSurfaceType_Type)(void *);
typedef HBINKBUFFER (WINAPI *BinkBufferOpen_Type)( void *, U32, U32, U32);
typedef void  (WINAPI *BinkBufferCheckWinPos_Type)(HBINKBUFFER, S32 *, S32 *);
typedef void (WINAPI *BinkBufferBlit_Type)(HBINKBUFFER, BINKRECT *, U32);
typedef S32 (WINAPI *BinkBufferSetOffset_Type)(HBINKBUFFER, S32, S32);
typedef S32  (WINAPI *BinkDoFrame_Type)(HBINK);
typedef void (WINAPI *BinkNextFrame_Type)(HBINK);
typedef S32 (WINAPI *BinkWait_Type)(HBINK);
typedef S32 (WINAPI *BinkBufferLock_Type)(HBINKBUFFER);
typedef S32 (WINAPI *BinkBufferUnlock_Type)(HBINKBUFFER);
typedef S32 (WINAPI *BinkCopyToBuffer_Type)(HBINK, void *, S32, U32, U32, U32, U32);
typedef S32 (WINAPI *BinkGetRects_Type)(HBINK, U32);
typedef S32 (WINAPI *BinkPause_Type)(HBINK, S32);
typedef void (WINAPI *BinkBufferClose_Type)(HBINKBUFFER);
typedef s32 (WINAPI *BinkSetSoundSystem_Type)(BINK_SOUND_FUNC, VOID *); // arg2 should be IDirectSound
typedef void (WINAPI *BinkSetIOSize_Type)(s32);
typedef S32 (WINAPI *BinkShouldSkip_Type)(HBINK);
typedef S32 (WINAPI *BinkControlBackgroundIO_Type)(HBINK, U32);
typedef void (WINAPI *BinkGetFrameBuffersInfo_Type)(HBINK, BINKFRAMEBUFFERS *);
typedef void (WINAPI *BinkRegisterFrameBuffers_Type)(HBINK, BINKFRAMEBUFFERS *);
typedef BINKSNDOPEN (WINAPI *BinkOpenWaveOut_Type)(UINTa);
typedef BINKSNDOPEN (WINAPI *BinkOpenMiles_Type)(UINTa);
typedef BINKSNDOPEN (WINAPI *BinkOpenDirectSound_Type)(UINTa);
typedef void (WINAPI *BinkSetSoundTrack_Type)(U32, U32 *);
#endif // TRACEALL

BinkOpen_Type pBinkOpen;
BinkClose_Type pBinkClose;
BinkGoto_Type pBinkGoto = 0;
#ifdef TRACEALL
BinkSurfaceType_Type pBinkDX8SurfaceType;
BinkSurfaceType_Type pBinkDDSurfaceType;
BinkBufferOpen_Type pBinkBufferOpen;
BinkBufferCheckWinPos_Type pBinkBufferCheckWinPos;
BinkBufferBlit_Type pBinkBufferBlit;
BinkBufferSetOffset_Type pBinkBufferSetOffset;
BinkDoFrame_Type pBinkDoFrame;
BinkNextFrame_Type pBinkNextFrame;
BinkWait_Type pBinkWait;
BinkBufferLock_Type pBinkBufferLock;
BinkBufferUnlock_Type pBinkBufferUnlock;
BinkCopyToBuffer_Type pBinkCopyToBuffer;
BinkGetRects_Type pBinkGetRects;
BinkPause_Type pBinkPause;
BinkBufferClose_Type pBinkBufferClose;
BinkSetSoundSystem_Type pBinkSetSoundSystem;
BinkSetIOSize_Type pBinkSetIOSize;
BinkShouldSkip_Type pBinkShouldSkip;
BinkControlBackgroundIO_Type pBinkControlBackgroundIO;
BinkGetFrameBuffersInfo_Type pBinkGetFrameBuffersInfo;
BinkRegisterFrameBuffers_Type pBinkRegisterFrameBuffers;
BinkOpenWaveOut_Type pBinkOpenWaveOut;
BinkOpenMiles_Type pBinkOpenMiles;
BinkOpenDirectSound_Type pBinkOpenDirectSound;
BinkSetSoundTrack_Type pBinkSetSoundTrack;
#endif // TRACEALL

HBINK RADEXPLINK extBinkOpen(const char *name, U32 flags);
void  RADEXPLINK extBinkClose(HBINK);
#ifdef TRACEALL
S32   RADEXPLINK extBinkDX8SurfaceType(void *);
S32   RADEXPLINK extBinkDDSurfaceType(void *);
HBINKBUFFER RADEXPLINK extBinkBufferOpen( void *, U32, U32, U32);
void RADEXPLINK extBinkBufferCheckWinPos(HBINKBUFFER buf, S32 *NewWindowX, S32 *NewWindowY);
void RADEXPLINK extBinkBufferBlit(HBINKBUFFER, BINKRECT *, U32);
S32 RADEXPLINK extBinkBufferSetOffset(HBINKBUFFER, S32, S32);
S32  RADEXPLINK extBinkDoFrame(HBINK);
void RADEXPLINK extBinkNextFrame(HBINK);
S32 RADEXPLINK extBinkWait(HBINK);
S32 RADEXPLINK extBinkBufferLock(HBINKBUFFER);
S32 RADEXPLINK extBinkBufferUnlock(HBINKBUFFER);
S32 RADEXPLINK extBinkCopyToBuffer(HBINK, void *, S32, U32, U32, U32, U32);
S32 RADEXPLINK extBinkGetRects(HBINK, U32);
S32 RADEXPLINK extBinkPause(HBINK, S32);
void RADEXPLINK extBinkBufferClose(HBINKBUFFER);
s32 RADEXPLINK extBinkSetSoundSystem(BINK_SOUND_FUNC, VOID *); // arg2 should be IDirectSound
void RADEXPLINK extBinkSetIOSize(s32);
void RADEXPLINK extBinkGoto(HBINK, U32, S32);
S32 RADEXPLINK extBinkShouldSkip(HBINK);
S32 RADEXPLINK extBinkControlBackgroundIO(HBINK, U32);
void RADEXPLINK extBinkGetFrameBuffersInfo(HBINK, BINKFRAMEBUFFERS *);
void RADEXPLINK extBinkRegisterFrameBuffers(HBINK, BINKFRAMEBUFFERS *);
BINKSNDOPEN RADEXPLINK extBinkOpenWaveOut(UINTa);
BINKSNDOPEN RADEXPLINK extBinkOpenMiles(UINTa);
BINKSNDOPEN RADEXPLINK extBinkOpenDirectSound(UINTa);
void RADEXPLINK extBinkSetSoundTrack(U32, U32 *);
#endif // TRACEALL

static HookEntryEx_Type Hooks[] = {
    {HOOK_IAT_CANDIDATE, 0x0000, "_BinkOpen@8", (FARPROC)NULL, (FARPROC *) &pBinkOpen, (FARPROC)extBinkOpen},
    {HOOK_IAT_CANDIDATE, 0x0000, "_BinkClose@4", (FARPROC)NULL, (FARPROC *) &pBinkClose, (FARPROC)extBinkClose},
#ifdef TRACEALL
    {HOOK_IAT_CANDIDATE, 0x0000, "_BinkDX8SurfaceType@4", (FARPROC)NULL, (FARPROC *) &pBinkDX8SurfaceType, (FARPROC)extBinkDX8SurfaceType},
    {HOOK_IAT_CANDIDATE, 0x0000, "_BinkDDSurfaceType@4", (FARPROC)NULL, (FARPROC *) &pBinkDDSurfaceType, (FARPROC)extBinkDDSurfaceType}, // Ground Control
    {HOOK_IAT_CANDIDATE, 0x0000, "_BinkBufferOpen@16", (FARPROC)NULL, (FARPROC *) &pBinkBufferOpen, (FARPROC)extBinkBufferOpen}, // Neverwinter Nights
    {HOOK_IAT_CANDIDATE, 0x0000, "_BinkBufferCheckWinPos@12", (FARPROC)NULL, (FARPROC *) &pBinkBufferCheckWinPos, (FARPROC)extBinkBufferCheckWinPos}, // Neverwinter Nights
    {HOOK_IAT_CANDIDATE, 0x0000, "_BinkBufferSetOffset@12", (FARPROC)NULL, (FARPROC *) &pBinkBufferSetOffset, (FARPROC)extBinkBufferSetOffset}, // Neverwinter Nights
    {HOOK_IAT_CANDIDATE, 0x0000, "_BinkBufferBlit@12", (FARPROC)NULL, (FARPROC *) &pBinkBufferBlit, (FARPROC)extBinkBufferBlit}, // Neverwinter Nights
    {HOOK_IAT_CANDIDATE, 0x0000, "_BinkDoFrame@4", (FARPROC)NULL, (FARPROC *) &pBinkDoFrame, (FARPROC)extBinkDoFrame},
    {HOOK_IAT_CANDIDATE, 0x0000, "_BinkNextFrame@4", (FARPROC)NULL, (FARPROC *) &pBinkNextFrame, (FARPROC)extBinkNextFrame},
    //{HOOK_IAT_CANDIDATE, 0x0000, "_BinkWait@4", (FARPROC)NULL, (FARPROC *)&pBinkWait, (FARPROC)extBinkWait},
    {HOOK_IAT_CANDIDATE, 0x0000, "_BinkBufferLock@4", (FARPROC)NULL, (FARPROC *) &pBinkBufferLock, (FARPROC)extBinkBufferLock},
    {HOOK_IAT_CANDIDATE, 0x0000, "_BinkBufferUnlock@4", (FARPROC)NULL, (FARPROC *) &pBinkBufferUnlock, (FARPROC)extBinkBufferUnlock},
    {HOOK_IAT_CANDIDATE, 0x0000, "_BinkCopyToBuffer@28", (FARPROC)NULL, (FARPROC *) &pBinkCopyToBuffer, (FARPROC)extBinkCopyToBuffer},
    {HOOK_IAT_CANDIDATE, 0x0000, "_BinkGetRects@8", (FARPROC)NULL, (FARPROC *) &pBinkGetRects, (FARPROC)extBinkGetRects},
    {HOOK_IAT_CANDIDATE, 0x0000, "_BinkPause@8", (FARPROC)NULL, (FARPROC *) &pBinkPause, (FARPROC)extBinkPause},
    {HOOK_IAT_CANDIDATE, 0x0000, "_BinkBufferClose@4", (FARPROC)NULL, (FARPROC *) &pBinkBufferClose, (FARPROC)extBinkBufferClose},
    {HOOK_IAT_CANDIDATE, 0x0000, "_BinkSetSoundSystem@8", (FARPROC)NULL, (FARPROC *) &pBinkSetSoundSystem, (FARPROC)extBinkSetSoundSystem},
    {HOOK_IAT_CANDIDATE, 0x0000, "_BinkSetIOSize@4", (FARPROC)NULL, (FARPROC *) &pBinkSetIOSize, (FARPROC)extBinkSetIOSize},
    {HOOK_IAT_CANDIDATE, 0x0000, "_BinkGoto@12", (FARPROC)NULL, (FARPROC *) &pBinkGoto, (FARPROC)extBinkGoto},
    {HOOK_IAT_CANDIDATE, 0x0000, "_BinkShouldSkip@4", (FARPROC)NULL, (FARPROC *) &pBinkShouldSkip, (FARPROC)extBinkShouldSkip},
    {HOOK_IAT_CANDIDATE, 0x0000, "_BinkControlBackgroundIO@8", (FARPROC)NULL, (FARPROC *) &pBinkControlBackgroundIO, (FARPROC)extBinkControlBackgroundIO},
    {HOOK_IAT_CANDIDATE, 0x0000, "_BinkGetFrameBuffersInfo@8", (FARPROC)NULL, (FARPROC *) &pBinkGetFrameBuffersInfo, (FARPROC)extBinkGetFrameBuffersInfo},
    {HOOK_IAT_CANDIDATE, 0x0000, "_BinkRegisterFrameBuffers@8", (FARPROC)NULL, (FARPROC *) &pBinkRegisterFrameBuffers, (FARPROC)extBinkRegisterFrameBuffers},
    {HOOK_IAT_CANDIDATE, 0x0000, "_BinkOpenWaveOut@4", (FARPROC)NULL, (FARPROC *) &pBinkOpenWaveOut, (FARPROC)extBinkOpenWaveOut},
    {HOOK_IAT_CANDIDATE, 0x0000, "_BinkOpenMiles@4", (FARPROC)NULL, (FARPROC *) &pBinkOpenMiles, (FARPROC)extBinkOpenMiles},
    {HOOK_IAT_CANDIDATE, 0x0000, "_BinkOpenDirectSound@4", (FARPROC)NULL, (FARPROC *) &pBinkOpenDirectSound, (FARPROC)extBinkOpenDirectSound},
    {HOOK_IAT_CANDIDATE, 0x0000, "_BinkSetSoundTrack@8", (FARPROC)NULL, (FARPROC *) &pBinkSetSoundTrack, (FARPROC)extBinkSetSoundTrack},
#endif // TRACEALL
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

FARPROC Remap_bink_ProcAddress(LPCSTR proc, HMODULE hModule) {
    FARPROC addr;
    if(!(dxw.dwFlags5 & HOOKBINKW32)) return NULL;
    InitializeBinkInterface();
    if (addr = RemapLibraryEx(proc, hModule, Hooks)) return addr;
    // NULL -> keep the original call address
    return NULL;
}

static char *libname = "binkw32.dll";

void HookBinkW32(HMODULE hModule) {
    if(!(dxw.dwFlags5 & HOOKBINKW32)) return;
    InitializeBinkInterface();
    HookLibraryEx(hModule, Hooks, libname);
    return;
}

static char *SafeString(const char *s) {
    static char SafeBuf[MAX_PATH + 1];
    __try {
        char probe;
        probe = *s;
        strncpy(SafeBuf, s, MAX_PATH);
        SafeBuf[MAX_PATH] = 0;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        sprintf(SafeBuf, "0x%X", s);
    };
    return SafeBuf;
}

static void InitializeBinkInterface() {
    // select the hook table by looking at the existence of some given key function that
    // does not exists in previous interface versions
    HMODULE h;
    FARPROC fun;
    // do the operation just once
    if(_BinkVersion != BINKVERSIONUNKNOWN) return;
    // can't use the module handle because it doesn't work for main process handle, which
    // is the most useful thing. Need opening (and closing) a new module handle, hoping
    // to load the very same dll ...
    h = (*pLoadLibraryA)("binkw32.dll");
    if(!h) return;
    while(TRUE) { // fake loop
        fun = (*pGetProcAddress)(h, "_BinkBufferSetDirectDraw@8");
        if(fun) {
            OutTraceDW("Bink interface greater than 1.0\n");
            _BinkVersion = BINKVERSION1;
            break;
        }
        fun = (*pGetProcAddress)(h, "_BinkOpen@8");
        if(fun) {
            OutTraceDW("Bink interface 0.x\n");
            _BinkVersion = BINKVERSION0;
            break;
        }
        OutTraceE("Smack interface unknown\n");
        _BinkVersion = BINKVERSIONUNKNOWN;
        break; // exit fake loop
    }
    FreeLibrary(h);
}

#if 0
static void GetBinkVersion(HBINK h) {
    if(_BinkVersion != BINKVERSIONUNKNOWN) return;
    HMODULE hbinkw32 = (*pLoadLibraryA)("BinkW32.dll");
    if(!hbinkw32) {
        OutTraceE("GetBinkVersion: LoadLibrary failed, err=%d\n", GetLastError());
        return;
    }
    char path[MAX_PATH + 1];
    if (GetModuleFileName(hbinkw32, path, sizeof(path)) == 0) {
        OutTraceE("GetBinkVersion: GetModuleFileName failed, err=%d\n", GetLastError());
        return;
    }
    CloseHandle(hbinkw32);
    DWORD  verHandle = 0;
    UINT   size      = 0;
    LPBYTE lpBuffer  = NULL;
    DWORD  verSize   = GetFileVersionInfoSize(path, &verHandle);
    if (verSize != NULL) {
        LPSTR verData = (LPSTR)malloc(verSize);
        if (GetFileVersionInfo(path, verHandle, verSize, verData)) {
            if (VerQueryValue(verData, "\\", (VOID FAR * FAR *)&lpBuffer, &size)) {
                if (size) {
                    VS_FIXEDFILEINFO *verInfo = (VS_FIXEDFILEINFO *)lpBuffer;
                    if (verInfo->dwSignature == 0xfeef04bd) {
                        // Doesn't matter if you are on 32 bit or 64 bit,
                        // DWORD is always 32 bits, so first two revision numbers
                        // come from dwFileVersionMS, last two come from dwFileVersionLS
                        OutTraceDW("BinkW32: File Version %d.%d.%d.%d\n",
                                   ( verInfo->dwFileVersionMS >> 16 ) & 0xffff,
                                   ( verInfo->dwFileVersionMS >>  0 ) & 0xffff,
                                   ( verInfo->dwFileVersionLS >> 16 ) & 0xffff,
                                   ( verInfo->dwFileVersionLS >>  0 ) & 0xffff
                                  );
                        switch(verInfo->dwFileVersionMS) {
                        case 0:
                            _BinkVersion = BINKVERSION0;
                            break;
                        default:
                            _BinkVersion = BINKVERSION1;
                            break;
                        }
                    }
                }
            }
        }
        free(verData);
    }
}
#endif

#ifndef DXW_NOTRACES
static void DumpBink(HBINK bink, ApiArg) {
    BINK0 *bink0;
    BINK1 *bink1;
    if((!IsDebugDW) || (bink == NULL)) return;
    switch(_BinkVersion) {
    case BINKVERSION0:
        bink0 = (BINK0 *)bink;
        OutTrace("%s: BINK(%#x) {size=(%dx%d) frames=%d fnum=%d lastn=%d}\n",
                 ApiRef, bink,
                 bink0->Width,
                 bink0->Height,
                 bink0->Frames,
                 bink0->FrameNum,
                 bink0->LastFrameNum);
        break;
    case BINKVERSION1:
        bink1 = (BINK1 *)bink;
        OutTrace("%s: BINK(%#x) {size=(%dx%d) frames=%d fnum=%d lastn=%d}\n",
                 ApiRef, bink,
                 bink1->Width,
                 bink1->Height,
                 bink1->Frames,
                 bink1->FrameNum,
                 bink1->LastFrameNum);
        break;
    }
    OutHexDW((unsigned char *)bink, 40);
}

static void DumpFBSet(BINKFRAMEBUFFERS *fbset, ApiArg) {
    OutTrace("> TotalFrames: %d\n", fbset->TotalFrames);
    OutTrace("> YABuffer(w,h): (%d,%d)\n", fbset->YABufferWidth, fbset->YABufferHeight);
    OutTrace("> cRcBuffer(w,h): (%d,%d)\n", fbset->cRcBBufferWidth, fbset->cRcBBufferHeight);
    OutTrace("> FrameNum: %d\n", fbset->FrameNum);
    OutHexDW((unsigned char *)fbset, sizeof(BINKFRAMEBUFFERS));
}
#endif // DXW_NOTRACES

static void BinkSkipMovie(ApiArg, HBINK h) {
    DWORD Frames;
    OutTraceDW("%s: (debug) SKIPMOVIE\n", ApiRef);
    if(!pBinkGoto) {
        HINSTANCE hinst;
        hinst = (*pLoadLibraryA)("binkw32.dll");
        if(!hinst) {
            OutTraceDW("%s: LoadLibrary ERROR err=%d\n", ApiRef, GetLastError());
            return;
        }
        pBinkGoto = (BinkGoto_Type)(*pGetProcAddress)(hinst, "_BinkGoto@12");
        if(!pBinkGoto) {
            OutTraceDW("%s: GetProcAddress ERROR err=%d\n", ApiRef, GetLastError());
            return;
        }
    }
    switch(_BinkVersion) {
    case BINKVERSION0:
        Frames = ((BINK0 *)h)->Frames;
        break;
    case BINKVERSION1:
        Frames = ((BINK1 *)h)->Frames;
        break;
    default:
        Frames = 0;
        break;
    }
    OutTraceDW("%s: SKIPMOVIE frames=%d\n", ApiRef, Frames);
    if(Frames > 10) (*pBinkGoto)(h, Frames - 1, 0);
    //if(Frames > 10) (*pBinkGoto)(h, Frames-10, 0);
    //DumpBink(h, ApiRef);
}

HBINK RADEXPLINK extBinkOpen(const char PTR4 *name, U32 flags) {
    HBINK h;
    ApiName("BinkOpen");
    OutTraceDW("%s: name=%s flags=%#x\n", ApiRef, SafeString(name), flags);
    h = (*pBinkOpen)(name, flags);
    //GetBinkVersion(h);
    OutTraceDW("%s: ret=%#x\n", ApiRef, h);
    if(dxw.dwFlags10 & SUSPENDTIMESTRETCH) dxw.SuspendTimeStretch(TIMESHIFT_OFF);
    if (!h) return NULL;
    if(dxw.dwFlags6 & NOMOVIES) BinkSkipMovie(ApiRef, h);
#ifndef DXW_NOTRACES
    DumpBink(h, ApiRef);
#endif // DXW_NOTRACES
    return h;
}

void RADEXPLINK extBinkClose(HBINK h) {
    ApiName("BinkClose");
    OutTraceDW("%s: h=%#x\n", ApiRef, h);
#ifndef DXW_NOTRACES
    DumpBink(h, ApiRef);
#endif // DXW_NOTRACES
    if(dxw.dwFlags10 & SUSPENDTIMESTRETCH) dxw.SuspendTimeStretch(TIMESHIFT_ON);
    (*pBinkClose)(h);
}

#ifdef TRACEALL

S32 RADEXPLINK extBinkDX8SurfaceType(void *lpD3Dd) {
    S32 ret;
    ApiName("BinkDX8SurfaceType");
    OutTraceDW("%s: lpD3Dd=%#x\n", ApiRef, lpD3Dd);
    ret = (*pBinkDX8SurfaceType)(lpD3Dd);
    OutTraceDW("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

S32 RADEXPLINK extBinkDDSurfaceType(void *lpDD) {
    S32 ret;
    ApiName("BinkDDSurfaceType");
    OutTraceDW("%s: lpDD=%#x\n", ApiRef, lpDD);
    ret = (*pBinkDDSurfaceType)(lpDD);
    OutTraceDW("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

HBINKBUFFER RADEXPLINK extBinkBufferOpen( void *wnd, U32 width, U32 height, U32 bufferflags) {
    HBINKBUFFER ret;
    ApiName("BinkBufferOpen");
    OutTraceDW("%s: hwnd=%#x size=(%dx%d) flags=%#x\n", ApiRef, wnd, width, height, bufferflags);
    ret = (*pBinkBufferOpen)(wnd, width, height, bufferflags);
    OutTraceDW("%s: ret=%#x\n", ApiRef, ret);
    return ret;
}

void RADEXPLINK extBinkBufferCheckWinPos(HBINKBUFFER buf, S32 *NewWindowX, S32 *NewWindowY) {
    ApiName("BinkBufferCheckWinPos");
    (*pBinkBufferCheckWinPos)(buf, NewWindowX, NewWindowY);
    OutTraceDW("%s: buf=%#x newpos=(%d,%d)\n", ApiRef, buf, *NewWindowX, *NewWindowY);
}

void RADEXPLINK extBinkBufferBlit(HBINKBUFFER buf, BINKRECT *rects, U32 numrects) {
    ApiName("BinkBufferBlit");
    OutTraceDW("%s: buf=%#x rects=%d\n", ApiRef, buf, numrects);
    for(unsigned int i = 0; i < numrects; i++)
        OutTraceDW("> rect[%d] pos=(%d,%d) size=(%dx%d)\n", i, rects[i].Left, rects[i].Top, rects[i].Width, rects[i].Height);
    (*pBinkBufferBlit)(buf, rects, numrects);
}

S32 RADEXPLINK extBinkBufferSetOffset(HBINKBUFFER buf, S32 destx, S32 desty) {
    S32 ret;
    ApiName("BinkBufferSetOffset");
    OutTraceDW("%s: buf=%#x dest=(%d,%d)\n", ApiRef, buf, destx, desty);
    ret = (*pBinkBufferSetOffset)(buf, destx, desty);
    OutTraceDW("%s: ret=%d\n", ApiRef, ret);
    return ret;
}

S32  RADEXPLINK extBinkDoFrame(HBINK bnk) {
    S32 ret;
    ApiName("BinkDoFrame");
    OutTraceDW("%s: hbnk=%#x\n", ApiRef, bnk);
    ret = (*pBinkDoFrame)(bnk);
#ifndef DXW_NOTRACES
    DumpBink(bnk, ApiRef);
#endif // DXW_NOTRACES
    OutTraceDW("%s: ret=%d\n", ApiRef, ret);
    return ret;
}

void RADEXPLINK extBinkNextFrame(HBINK bnk) {
    ApiName("BinkNextFrame");
    OutTraceDW("%s: hbnk=%#x\n", ApiRef, bnk);
    (*pBinkNextFrame)(bnk);
#ifndef DXW_NOTRACES
    DumpBink(bnk, ApiRef);
#endif // DXW_NOTRACES
}

S32 RADEXPLINK extBinkWait(HBINK bnk) {
    S32 ret;
    ApiName("BinkWait");
    OutTraceDW("%s: hbnk=%#x\n", ApiRef, bnk);
    ret = (*pBinkWait)(bnk);
#ifndef DXW_NOTRACES
    DumpBink(bnk, ApiRef);
#endif // DXW_NOTRACES
    OutTraceDW("%s: ret=%d\n", ApiRef, ret);
    return ret;
}

S32 RADEXPLINK extBinkBufferLock(HBINKBUFFER buf) {
    S32 ret;
    ApiName("BinkBufferLock");
    OutTraceDW("%s: buf=%#x\n", ApiRef, buf);
    ret = (*pBinkBufferLock)(buf);
    OutTraceDW("%s: ret=%d\n", ApiRef, ret);
    return ret;
}

S32 RADEXPLINK extBinkBufferUnlock(HBINKBUFFER buf) {
    S32 ret;
    ApiName("BinkBufferUnlock");
    OutTraceDW("%s: buf=%#x\n", ApiRef, buf);
    ret = (*pBinkBufferUnlock)(buf);
    OutTraceDW("%s: ret=%d\n", ApiRef, ret);
    return ret;
}

S32 RADEXPLINK extBinkCopyToBuffer(HBINK bnk, void *dest, S32 destpitch, U32 destheight, U32 destx, U32 desty, U32 flags) {
    S32 ret;
    ApiName("BinkCopyToBuffer");
    OutTraceDW("%s: hbnk=%#x dest=%#x destpitch=%d destheight=%d pos=(%d,%d) flags=%#x\n",
               ApiRef, bnk, dest, destpitch, destheight, destx, desty, flags);
    ret = (*pBinkCopyToBuffer)(bnk, dest, destpitch, destheight, destx, desty, flags);
    OutTraceDW("%s: ret=%d\n", ApiRef, ret);
    return ret;
}

S32 RADEXPLINK extBinkGetRects(HBINK bnk, U32 flags) {
    S32 ret;
    ApiName("BinkGetRects");
    OutTraceDW("%s: hbnk=%#x flags=%#x\n", ApiRef, bnk, flags);
    ret = (*pBinkGetRects)(bnk, flags);
    OutTraceDW("%s: ret=%d\n", ApiRef, ret);
    return ret;
}

S32 RADEXPLINK extBinkPause(HBINK bnk, S32 pause) {
    S32 ret;
    ApiName("BinkPause");
    OutTraceDW("%s: hbnk=%#x pause=%#x\n", ApiRef, bnk, pause);
    ret = (*pBinkPause)(bnk, pause);
#ifndef DXW_NOTRACES
    DumpBink(bnk, ApiRef);
#endif // DXW_NOTRACES
    OutTraceDW("%s: ret=%d\n", ApiRef, ret);
    return ret;
}

void RADEXPLINK extBinkBufferClose(HBINKBUFFER buf) {
    ApiName("BinkBufferClose");
    OutTraceDW("%s: hbuf=%#x\n", ApiRef, buf);
    (*pBinkBufferClose)(buf);
}

s32 RADEXPLINK extBinkSetSoundSystem(BINK_SOUND_FUNC SoundFunction, VOID *pDS) {
    S32 ret;
    ApiName("BinkSetSoundSystem");
    OutTraceDW("%s: sfunct=%#x pDS=%#x\n", ApiRef, SoundFunction, pDS);
    ret = (*pBinkSetSoundSystem)(SoundFunction, pDS);
    OutTraceDW("%s: ret=%d\n", ApiRef, ret);
    return ret;
}

void RADEXPLINK extBinkSetIOSize(s32 iosize) {
    ApiName("BinkSetIOSize");
    OutTraceDW("%s: iosize=%d\n", ApiRef, iosize);
    (*pBinkSetIOSize)(iosize);
}

void RADEXPLINK extBinkGoto(HBINK bnk, U32 frame, S32 flags) {
    ApiName("BinkGoTo");
    OutTraceDW("%s: hbnk=%#x frame=%d flags=%#x\n", ApiRef, bnk, frame, flags);
    (*pBinkGoto)(bnk, frame, flags);
#ifndef DXW_NOTRACES
    DumpBink(bnk, ApiRef);
#endif // DXW_NOTRACES
}

S32 RADEXPLINK extBinkShouldSkip(HBINK bnk) {
    S32 ret;
    ApiName("BinkShouldSkip");
    OutTraceDW("%s: hbnk=%#x\n", ApiRef, bnk);
    ret = (*pBinkShouldSkip)(bnk);
#ifndef DXW_NOTRACES
    DumpBink(bnk, ApiRef);
#endif // DXW_NOTRACES
    OutTraceDW("%s: ret=%d\n", ApiRef, ret);
    return ret;
}

S32 RADEXPLINK extBinkControlBackgroundIO(HBINK bnk, U32 control) {
    S32 ret;
    ApiName("BinkControlBackgroundIO");
    OutTraceDW("%s: hbnk=%#x control=%#x\n", ApiRef, bnk, control);
    ret = (*pBinkControlBackgroundIO)(bnk, control);
#ifndef DXW_NOTRACES
    DumpBink(bnk, ApiRef);
#endif // DXW_NOTRACES
    OutTraceDW("%s: ret=%d\n", ApiRef, ret);
    return ret;
}

void RADEXPLINK extBinkGetFrameBuffersInfo(HBINK bnk, BINKFRAMEBUFFERS *fbset) {
    ApiName("BinkGetFrameBuffersInfo");
    OutTraceDW("%s: hbnk=%#x fbset=%#x\n", ApiRef, bnk, fbset);
    (*pBinkGetFrameBuffersInfo)(bnk, fbset);
#ifndef DXW_NOTRACES
    DumpFBSet(fbset, ApiRef);
#endif // DXW_NOTRACES
}

void RADEXPLINK extBinkRegisterFrameBuffers(HBINK bnk, BINKFRAMEBUFFERS *fbset) {
    ApiName("BinkRegisterFrameBuffers");
    OutTraceDW("%s: hbnk=%#x fbset=%#x\n", ApiRef, bnk, fbset);
#ifndef DXW_NOTRACES
    DumpFBSet(fbset, ApiRef);
#endif // DXW_NOTRACES
    (*pBinkRegisterFrameBuffers)(bnk, fbset);
}

BINKSNDOPEN RADEXPLINK extBinkOpenDirectSound(UINTa param) {
    ApiName("BinkOpenDirectSound");
    OutTraceDW("%s: param=%#x\n", ApiRef, param);
    return (*pBinkOpenDirectSound)(param);
}

BINKSNDOPEN RADEXPLINK extBinkOpenMiles(UINTa param) {
    ApiName("BinkOpenMiles");
    OutTraceDW("%s: param=%#x\n", ApiRef, param);
    return (*pBinkOpenMiles)(param);
}

BINKSNDOPEN RADEXPLINK extBinkOpenWaveOut(UINTa param) {
    ApiName("BinkOpenWaveOut");
    OutTraceDW("%s: param=%#x\n", ApiRef, param);
    return (*pBinkOpenWaveOut)(param);
}

void RADEXPLINK extBinkSetSoundTrack(U32 total_tracks, U32 *tracks) {
    ApiName("BinkSetSoundTrack");
    OutTraceDW("%s: total=%d\n", ApiRef, total_tracks);
    (*pBinkSetSoundTrack)(total_tracks, tracks);
}
#endif // TRACEALL
