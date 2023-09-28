#define _WIN32_WINNT 0x0600
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS

#include <stdio.h>
#include <stdlib.h>
#include "dxwnd.h"
#include "dxwcore.hpp"
#include "syslibs.h"
#include "dxhook.h"
#include "dsound.h"
#include "dxhelper.h"

#define GLOBALFOCUSMASK (GLOBALFOCUSON | GLOBALFOCUSOFF)
//#define TRACEALL
#ifdef TRACEALL
#define TRACEALLSOUNDS
#endif

#ifndef DXW_NOTRACES
#define TraceDSError(e) OutTraceE("%s: ERROR res=%#x(%s)\n", ApiRef, e, ExplainDDError(e))
#else
#define TraceDSError(e)
#endif

//----------------------------------------------------------------------//
// Hookers typedefs, prototypes, pointers
//----------------------------------------------------------------------//

typedef HRESULT	(WINAPI *DirectSoundCreate_Type)(LPGUID, LPDIRECTSOUND *, LPUNKNOWN);
typedef HRESULT (WINAPI *DirectSoundCreate8_Type)(LPCGUID, LPDIRECTSOUND8 *, LPUNKNOWN);
typedef HRESULT	(WINAPI *SetCooperativeLevel_Type)  (void *, HWND, DWORD);
typedef HRESULT (WINAPI *DSGetCaps_Type)(void *, LPDSCAPS);
typedef HRESULT (WINAPI *CreateSoundBuffer_Type) (void *, LPCDSBUFFERDESC1, LPDIRECTSOUNDBUFFER *, LPUNKNOWN);
typedef HRESULT (WINAPI *DuplicateSoundBuffer_Type)(void *, LPDIRECTSOUNDBUFFER, LPDIRECTSOUNDBUFFER *);
typedef HRESULT (WINAPI *DirectSoundEnumerateA_Type)(LPDSENUMCALLBACKA, LPVOID);
typedef HRESULT (WINAPI *DirectSoundEnumerateW_Type)(LPDSENUMCALLBACKW, LPVOID);
typedef HRESULT (WINAPI *GetSpeakerConfig_Type)(void *, LPDWORD);
typedef HRESULT (WINAPI *SetSpeakerConfig_Type)(void *, DWORD);
typedef HRESULT (WINAPI *DSInitialize_Type)(void *, LPCGUID);
typedef HRESULT (WINAPI *Play_Type)(void *, DWORD, DWORD, DWORD);
typedef HRESULT (WINAPI *DSBSetCurrentPosition_Type)(void *, DWORD);
typedef HRESULT (WINAPI *DSBSetVolume_Type)(void *, LONG);
typedef HRESULT (WINAPI *Void_Type)(void *);
typedef HRESULT (WINAPI *DSBLock_Type)(void *, DWORD, DWORD, LPVOID *, LPDWORD, LPVOID *, LPDWORD, DWORD);
typedef HRESULT (WINAPI *DSBUnlock_Type)(void *, LPVOID, DWORD, LPVOID, DWORD);
typedef HRESULT (WINAPI *DSBInitialize_Type)(void *, LPDIRECTSOUND, LPCDSBUFFERDESC1);
typedef HRESULT (WINAPI *DSBGetCaps_Type)(void *, LPDSBCAPS);
typedef HRESULT (WINAPI *QueryInterface_Type)(void *, REFIID, LPVOID *);
typedef HRESULT (WINAPI *DSCompact_Type)(void *);
typedef HRESULT (WINAPI *DSBGetCurrentPosition_Type)(void *, LPDWORD, LPDWORD);
typedef HRESULT (WINAPI *DSBGetFormat_Type)(void *, LPWAVEFORMATEX, DWORD, LPDWORD);
typedef HRESULT (WINAPI *DSBGetVolume_Type)(void *, LPLONG);
typedef HRESULT (WINAPI *DSBGetPan_Type)(void *, LPLONG);
typedef HRESULT (WINAPI *DSBGetFrequency_Type)(void *, LPDWORD);
typedef HRESULT (WINAPI *DSBGetStatus_Type)(void *, LPDWORD);
typedef HRESULT (WINAPI *DSBSetFormat_Type)(void *, LPCWAVEFORMATEX);
typedef HRESULT (WINAPI *DSBSetPan_Type)(void *, LONG);
typedef HRESULT (WINAPI *DSBSetFrequency_Type)(void *, DWORD);

// DirectSound API method pointers

DirectSoundCreate_Type pDirectSoundCreate = NULL;
DirectSoundCreate8_Type pDirectSoundCreate8 = NULL;
#ifdef TRACEALLSOUNDS
DirectSoundEnumerateA_Type pDirectSoundEnumerateA ;
DirectSoundEnumerateW_Type pDirectSoundEnumerateW;
#endif // TRACEALLSOUNDS

// IDirectSound method pointers

SetCooperativeLevel_Type pDSSetCooperativeLevel = NULL;
CreateSoundBuffer_Type pDSCreateSoundBuffer = NULL;
DuplicateSoundBuffer_Type pDSDuplicateSoundBuffer = NULL;
#ifdef TRACEALLSOUNDS
DSGetCaps_Type pDSGetCaps = NULL;
QueryInterface_Type pDSQueryInterface;
GetSpeakerConfig_Type pDSGetSpeakerConfig = NULL;
SetSpeakerConfig_Type pDSSetSpeakerConfig = NULL;
DSInitialize_Type pDSInitialize = NULL;
DSCompact_Type pDSCompact;
#endif // TRACEALLSOUNDS

// IDirectSoundBuffer method pointers

Play_Type pDSSBPlay, pDSPBPlay;
DSBSetVolume_Type pDSSBSetVolume, pDSPBSetVolume;
DSBUnlock_Type pDSSBUnlock, pDSPBUnlock;
#ifdef TRACEALLSOUNDS
Void_Type pDSSBStop, pDSPBStop;
Void_Type pDSSBRestore, pDSPBRestore;
DSBSetCurrentPosition_Type pDSSBSetCurrentPosition, pDSPBSetCurrentPosition;
DSBInitialize_Type pDSSBInitialize, pDSPBInitialize;
DSBGetCaps_Type pDSSBGetCaps, pDSPBGetCaps;
QueryInterface_Type pDSSBQueryInterface, pDSPBQueryInterface;
DSBGetCurrentPosition_Type pDSSBGetCurrentPosition, pDSPBGetCurrentPosition;
DSBGetFormat_Type pDSSBGetFormat, pDSPBGetFormat;
DSBGetVolume_Type pDSSBGetVolume, pDSPBGetVolume;
DSBGetPan_Type pDSSBGetPan, pDSPBGetPan;
DSBGetFrequency_Type pDSSBGetFrequency, pDSPBGetFrequency;
DSBGetStatus_Type pDSSBGetStatus, pDSPBGetStatus;
DSBLock_Type pDSSBLock, pDSPBLock;
DSBSetFormat_Type pDSSBSetFormat, pDSPBSetFormat;
DSBSetPan_Type pDSSBSetPan, pDSPBSetPan;
DSBSetFrequency_Type pDSSBSetFrequency, pDSPBSetFrequency;
#endif // TRACEALLSOUNDS

// DirectSound API wrappers

HRESULT WINAPI extDirectSoundCreate(LPGUID, LPDIRECTSOUND *, LPUNKNOWN);
HRESULT WINAPI extDirectSoundCreate8(LPCGUID, LPDIRECTSOUND8 *, LPUNKNOWN);
#ifdef TRACEALLSOUNDS
HRESULT WINAPI extDirectSoundEnumerateA(LPDSENUMCALLBACKA, LPVOID);
HRESULT WINAPI extDirectSoundEnumerateW(LPDSENUMCALLBACKW, LPVOID);
#endif // TRACEALLSOUNDS

// IDirectSound method wrappers

HRESULT WINAPI extDSSetCooperativeLevel(void *, HWND, DWORD);
HRESULT WINAPI extDSCreateSoundBuffer(void *, LPCDSBUFFERDESC1, LPDIRECTSOUNDBUFFER *, LPUNKNOWN);
HRESULT WINAPI extDSDuplicateSoundBuffer(void *, LPDIRECTSOUNDBUFFER, LPDIRECTSOUNDBUFFER *);
#ifdef TRACEALLSOUNDS
HRESULT WINAPI extDSGetCaps(void *, LPDSCAPS);
HRESULT WINAPI extDSQueryInterface(void *, REFIID, LPVOID *);
HRESULT WINAPI extDSGetSpeakerConfig(void *, LPDWORD);
HRESULT WINAPI extDSSetSpeakerConfig(void *, DWORD);
HRESULT WINAPI extDSInitialize(void *, LPCGUID);
HRESULT WINAPI extDSCompact(void *);
#endif // TRACEALLSOUNDS

// IDirectSoundBuffer method wrappers

HRESULT WINAPI extDSPBPlay(void *, DWORD, DWORD, DWORD);
HRESULT WINAPI extDSSBPlay(void *, DWORD, DWORD, DWORD);
HRESULT WINAPI extDSSBSetVolume(void *, LONG);
HRESULT WINAPI extDSPBSetVolume(void *, LONG);
HRESULT WINAPI extDSSBUnlock(void *, LPVOID, DWORD, LPVOID, DWORD);
HRESULT WINAPI extDSPBUnlock(void *, LPVOID, DWORD, LPVOID, DWORD);
#ifdef TRACEALLSOUNDS
HRESULT WINAPI extDSSBStop(void *);
HRESULT WINAPI extDSPBStop(void *);
HRESULT WINAPI extDSSBRestore(void *);
HRESULT WINAPI extDSPBRestore(void *);
HRESULT WINAPI extDSSBSetCurrentPosition(void *, DWORD);
HRESULT WINAPI extDSPBSetCurrentPosition(void *, DWORD);
HRESULT WINAPI extDSSBInitialize(void *, LPDIRECTSOUND, LPCDSBUFFERDESC1);
HRESULT WINAPI extDSPBInitialize(void *, LPDIRECTSOUND, LPCDSBUFFERDESC1);
HRESULT WINAPI extDSSBGetCaps(void *, LPDSBCAPS);
HRESULT WINAPI extDSPBGetCaps(void *, LPDSBCAPS);
HRESULT WINAPI extDSSBQueryInterface(void *, REFIID, LPVOID *);
HRESULT WINAPI extDSPBQueryInterface(void *, REFIID, LPVOID *);
HRESULT WINAPI extDSSBLock(void *, DWORD, DWORD, LPVOID *, LPDWORD, LPVOID *, LPDWORD, DWORD);
HRESULT WINAPI extDSPBLock(void *, DWORD, DWORD, LPVOID *, LPDWORD, LPVOID *, LPDWORD, DWORD);
HRESULT WINAPI extDSSBGetCurrentPosition(void *, LPDWORD, LPDWORD);
HRESULT WINAPI extDSPBGetCurrentPosition(void *, LPDWORD, LPDWORD);
HRESULT WINAPI extDSSBGetFormat(void *, LPWAVEFORMATEX, DWORD, LPDWORD);
HRESULT WINAPI extDSPBGetFormat(void *, LPWAVEFORMATEX, DWORD, LPDWORD);
HRESULT WINAPI extDSSBGetVolume(void *, LPLONG);
HRESULT WINAPI extDSPBGetVolume(void *, LPLONG);
HRESULT WINAPI extDSSBGetPan(void *, LPLONG);
HRESULT WINAPI extDSPBGetPan(void *, LPLONG);
HRESULT WINAPI extDSSBGetFrequency(void *, LPDWORD);
HRESULT WINAPI extDSPBGetFrequency(void *, LPDWORD);
HRESULT WINAPI extDSSBGetStatus(void *, LPDWORD);
HRESULT WINAPI extDSPBGetStatus(void *, LPDWORD);
HRESULT WINAPI extDSSBSetFormat(void *, LPCWAVEFORMATEX);
HRESULT WINAPI extDSPBSetFormat(void *, LPCWAVEFORMATEX);
HRESULT WINAPI extDSSBSetPan(void *, LONG);
HRESULT WINAPI extDSPBSetPan(void *, LONG);
HRESULT WINAPI extDSSBSetFrequency(void *, DWORD);
HRESULT WINAPI extDSPBSetFrequency(void *, DWORD);
#endif // TRACEALLSOUNDS

#ifndef DSSCL_NORMAL
#define DSSCL_NORMAL                0x00000001
#define DSSCL_PRIORITY              0x00000002
#define DSSCL_EXCLUSIVE             0x00000003
#define DSSCL_WRITEPRIMARY          0x00000004
#endif

//----------------------------------------------------------------------//
// Hooking procedures
//----------------------------------------------------------------------//

static HookEntryEx_Type Hooks[] = {
    {HOOK_HOT_CANDIDATE, 0x0001, "DirectSoundCreate", (FARPROC)NULL, (FARPROC *) &pDirectSoundCreate, (FARPROC)extDirectSoundCreate},
    {HOOK_HOT_CANDIDATE, 0x000B, "DirectSoundCreate8", (FARPROC)NULL, (FARPROC *) &pDirectSoundCreate8, (FARPROC)extDirectSoundCreate8},
#ifdef TRACEALLSOUNDS
    {HOOK_HOT_CANDIDATE, 0x0002, "DirectSoundEnumerateA", (FARPROC)NULL, (FARPROC *) &pDirectSoundEnumerateA, (FARPROC)extDirectSoundEnumerateA},
    {HOOK_HOT_CANDIDATE, 0x0003, "DirectSoundEnumerateW", (FARPROC)NULL, (FARPROC *) &pDirectSoundEnumerateW, (FARPROC)extDirectSoundEnumerateW},
#endif // TRACEALLSOUNDS
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

FARPROC Remap_DSound_ProcAddress(LPCSTR proc, HMODULE hModule) {
    FARPROC addr;
    if (addr = RemapLibraryEx(proc, hModule, Hooks)) return addr;
    return NULL;
}

static char *libname = "dsound.dll";

void HookDirectSound(HMODULE hModule) {
    HookLibraryEx(hModule, Hooks, "dsound.dll");
}

void HookDirectSoundInit() {
    HookLibInitEx(Hooks);
}

void HookDirectSoundObj(LPDIRECTSOUND *lpds) {
    // v2.03.99: check or DirectSound could be hooked through CoCreateInstance !!
    if(!(dxw.dwFlags7 & HOOKDIRECTSOUND)) return;
    // IDIrectSound::SetCooperativeLevel
#ifdef TRACEALLSOUNDS
    SetHook((void *)(**(DWORD **)lpds +  0), extDSQueryInterface, (void **)&pDSQueryInterface, "QueryInterface");
#endif // TRACEALLSOUNDS
    SetHook((void *)(**(DWORD **)lpds + 12), extDSCreateSoundBuffer, (void **)&pDSCreateSoundBuffer, "CreateSoundBuffer");
#ifdef TRACEALLSOUNDS
    SetHook((void *)(**(DWORD **)lpds + 16), extDSGetCaps, (void **)&pDSGetCaps, "GetCaps");
#endif // TRACEALLSOUNDS
    SetHook((void *)(**(DWORD **)lpds + 20), extDSDuplicateSoundBuffer, (void **)&pDSDuplicateSoundBuffer, "DuplicateSoundBuffer");
    SetHook((void *)(**(DWORD **)lpds + 24), extDSSetCooperativeLevel, (void **)&pDSSetCooperativeLevel, "SetCooperativeLevel(DSound)");
#ifdef TRACEALLSOUNDS
    SetHook((void *)(**(DWORD **)lpds + 28), extDSCompact, (void **)&pDSCompact, "Compact");
    SetHook((void *)(**(DWORD **)lpds + 32), extDSGetSpeakerConfig, (void **)&pDSGetSpeakerConfig, "GetSpeakerConfig(DSound)");
    SetHook((void *)(**(DWORD **)lpds + 36), extDSSetSpeakerConfig, (void **)&pDSSetSpeakerConfig, "SetSpeakerConfig(DSound)");
    SetHook((void *)(**(DWORD **)lpds + 40), extDSInitialize, (void **)&pDSInitialize, "Initialize(DSound)");
#endif // TRACEALLSOUNDS
}

// naming: DSB: DirectSound Buffer, DSPB: DirectSound Primary Buffer, DSSB: DirectSound Secondary Buffer

void HookSoundBuffer(LPDIRECTSOUNDBUFFER *ppDSBuffer) {
#ifdef TRACEALLSOUNDS
    SetHook((void *)(**(DWORD **)ppDSBuffer +  0), extDSSBQueryInterface, (void **)&pDSSBQueryInterface, "QueryInterface");
    SetHook((void *)(**(DWORD **)ppDSBuffer + 12), extDSSBGetCaps, (void **)&pDSSBGetCaps, "GetCaps");
    SetHook((void *)(**(DWORD **)ppDSBuffer + 16), extDSSBGetCurrentPosition, (void **)&pDSSBGetCurrentPosition, "GetCurrentPosition");
    SetHook((void *)(**(DWORD **)ppDSBuffer + 20), extDSSBGetFormat, (void **)&pDSSBGetFormat, "GetFormat");
    SetHook((void *)(**(DWORD **)ppDSBuffer + 24), extDSSBGetVolume, (void **)&pDSSBGetVolume, "GetVolume");
    SetHook((void *)(**(DWORD **)ppDSBuffer + 28), extDSSBGetPan, (void **)&pDSSBGetPan, "GetPan");
    SetHook((void *)(**(DWORD **)ppDSBuffer + 32), extDSSBGetFrequency, (void **)&pDSSBGetFrequency, "GetFrequency");
    SetHook((void *)(**(DWORD **)ppDSBuffer + 36), extDSSBGetStatus, (void **)&pDSSBGetStatus, "GetStatus");
    SetHook((void *)(**(DWORD **)ppDSBuffer + 40), extDSSBInitialize, (void **)&pDSSBInitialize, "Initialize");
    SetHook((void *)(**(DWORD **)ppDSBuffer + 44), extDSSBLock, (void **)&pDSSBLock, "Lock");
#endif // TRACEALLSOUNDS
    SetHook((void *)(**(DWORD **)ppDSBuffer + 48), extDSSBPlay, (void **)&pDSSBPlay, "Play");
#ifdef TRACEALLSOUNDS
    SetHook((void *)(**(DWORD **)ppDSBuffer + 52), extDSSBSetCurrentPosition, (void **)&pDSSBSetCurrentPosition, "SetCurrentPosition");
    SetHook((void *)(**(DWORD **)ppDSBuffer + 56), extDSSBSetFormat, (void **)&pDSSBSetFormat, "SetFormat");
#endif // TRACEALLSOUNDS
    SetHook((void *)(**(DWORD **)ppDSBuffer + 60), extDSSBSetVolume, (void **)&pDSSBSetVolume, "SetVolume");
#ifdef TRACEALLSOUNDS
    SetHook((void *)(**(DWORD **)ppDSBuffer + 64), extDSSBSetPan, (void **)&pDSSBSetPan, "SetPan");
    SetHook((void *)(**(DWORD **)ppDSBuffer + 68), extDSSBSetFrequency, (void **)&pDSSBSetFrequency, "SetFrequency");
    SetHook((void *)(**(DWORD **)ppDSBuffer + 72), extDSSBStop, (void **)&pDSSBStop, "Stop");
#endif // TRACEALLSOUNDS
    SetHook((void *)(**(DWORD **)ppDSBuffer + 76), extDSSBUnlock, (void **)&pDSSBUnlock, "Unlock");
#ifdef TRACEALLSOUNDS
    SetHook((void *)(**(DWORD **)ppDSBuffer + 80), extDSSBRestore, (void **)&pDSSBRestore, "Restore");
#endif // TRACEALLSOUNDS
}
void HookPrimaryBuffer(LPDIRECTSOUNDBUFFER *ppDSBuffer) {
#ifdef TRACEALLSOUNDS
    SetHook((void *)(**(DWORD **)ppDSBuffer +  0), extDSPBQueryInterface, (void **)&pDSPBQueryInterface, "QueryInterface");
    SetHook((void *)(**(DWORD **)ppDSBuffer + 12), extDSPBGetCaps, (void **)&pDSPBGetCaps, "GetCaps");
    SetHook((void *)(**(DWORD **)ppDSBuffer + 16), extDSPBGetCurrentPosition, (void **)&pDSPBGetCurrentPosition, "GetCurrentPosition");
    SetHook((void *)(**(DWORD **)ppDSBuffer + 20), extDSPBGetFormat, (void **)&pDSPBGetFormat, "GetFormat");
    SetHook((void *)(**(DWORD **)ppDSBuffer + 24), extDSPBGetVolume, (void **)&pDSPBGetVolume, "GetVolume");
    SetHook((void *)(**(DWORD **)ppDSBuffer + 28), extDSPBGetPan, (void **)&pDSPBGetPan, "GetPan");
    SetHook((void *)(**(DWORD **)ppDSBuffer + 32), extDSPBGetFrequency, (void **)&pDSPBGetFrequency, "GetFrequency");
    SetHook((void *)(**(DWORD **)ppDSBuffer + 36), extDSPBGetStatus, (void **)&pDSPBGetStatus, "GetStatus");
    SetHook((void *)(**(DWORD **)ppDSBuffer + 40), extDSPBInitialize, (void **)&pDSPBInitialize, "Initialize");
    SetHook((void *)(**(DWORD **)ppDSBuffer + 44), extDSPBLock, (void **)&pDSPBLock, "Lock");
#endif // TRACEALLSOUNDS
    SetHook((void *)(**(DWORD **)ppDSBuffer + 48), extDSPBPlay, (void **)&pDSPBPlay, "Play");
#ifdef TRACEALLSOUNDS
    SetHook((void *)(**(DWORD **)ppDSBuffer + 52), extDSPBSetCurrentPosition, (void **)&pDSPBSetCurrentPosition, "SetCurrentPosition");
    SetHook((void *)(**(DWORD **)ppDSBuffer + 56), extDSPBSetFormat, (void **)&pDSPBSetFormat, "SetFormat");
#endif
    SetHook((void *)(**(DWORD **)ppDSBuffer + 60), extDSPBSetVolume, (void **)&pDSPBSetVolume, "SetVolume");
#ifdef TRACEALLSOUNDS
    SetHook((void *)(**(DWORD **)ppDSBuffer + 64), extDSPBSetPan, (void **)&pDSPBSetPan, "SetPan");
    SetHook((void *)(**(DWORD **)ppDSBuffer + 68), extDSPBSetFrequency, (void **)&pDSPBSetFrequency, "SetFrequency");
    SetHook((void *)(**(DWORD **)ppDSBuffer + 72), extDSPBStop, (void **)&pDSPBStop, "Stop");
#endif // TRACEALLSOUNDS
    SetHook((void *)(**(DWORD **)ppDSBuffer + 76), extDSPBUnlock, (void **)&pDSPBUnlock, "Unlock");
#ifdef TRACEALLSOUNDS
    SetHook((void *)(**(DWORD **)ppDSBuffer + 80), extDSPBRestore, (void **)&pDSPBRestore, "Restore");
#endif // TRACEALLSOUNDS
}

//----------------------------------------------------------------------//
// Auxiliary functions
//----------------------------------------------------------------------//

//----------------------------------------------------------------------//
// Tracing
//----------------------------------------------------------------------//

#ifndef DXW_NOTRACES
static char *ExplainLevel(DWORD lev) {
    char *s;
    switch(lev) {
    case DSSCL_NORMAL:
        s = "NORMAL";
        break;
    case DSSCL_PRIORITY:
        s = "PRIORITY";
        break;
    case DSSCL_EXCLUSIVE:
        s = "EXCLUSIVE";
        break;
    case DSSCL_WRITEPRIMARY:
        s = "WRITEPRIMARY";
        break;
    default:
        s = "invalid";
        break;
    }
    return s;
}

static char *ExplainDSFlags(DWORD c) {
    static char eb[256];
    unsigned int l;
    strcpy(eb, "DSCAPS_");
    if (c & DSCAPS_PRIMARYMONO) strcat(eb, "PRIMARYMONO+");
    if (c & DSCAPS_PRIMARYSTEREO) strcat(eb, "PRIMARYSTEREO+");
    if (c & DSCAPS_PRIMARY8BIT) strcat(eb, "PRIMARY8BIT+");
    if (c & DSCAPS_PRIMARY16BIT) strcat(eb, "PRIMARY16BIT+");
    if (c & DSCAPS_CONTINUOUSRATE) strcat(eb, "CONTINUOUSRATE+");
    if (c & DSCAPS_EMULDRIVER) strcat(eb, "EMULDRIVER+");
    if (c & DSCAPS_CERTIFIED) strcat(eb, "CERTIFIED+");
    if (c & DSCAPS_SECONDARYMONO) strcat(eb, "SECONDARYMONO+");
    if (c & DSCAPS_SECONDARYSTEREO) strcat(eb, "SECONDARYSTEREO+");
    if (c & DSCAPS_SECONDARY8BIT) strcat(eb, "SECONDARY8BIT+");
    if (c & DSCAPS_SECONDARY16BIT) strcat(eb, "SECONDARY16BIT+");
    l = strlen(eb);
    if (l > strlen("DSCAPS_")) eb[l - 1] = 0; // delete last '+' if any
    else eb[0] = 0;
    return(eb);
}

static char *ExplainDSBFlags(DWORD c) {
    static char eb[256];
    unsigned int l;
    strcpy(eb, "DSBCAPS_");
    if (c & DSBCAPS_PRIMARYBUFFER) strcat(eb, "PRIMARYBUFFER+");
    if (c & DSBCAPS_STATIC) strcat(eb, "STATIC+");
    if (c & DSBCAPS_LOCHARDWARE) strcat(eb, "LOCHARDWARE+");
    if (c & DSBCAPS_LOCSOFTWARE) strcat(eb, "LOCSOFTWARE+");
    if (c & DSBCAPS_CTRL3D) strcat(eb, "CTRL3D+");
    if (c & DSBCAPS_CTRLFREQUENCY) strcat(eb, "CTRLFREQUENCY+");
    if (c & DSBCAPS_CTRLPAN) strcat(eb, "CTRLPAN+");
    if (c & DSBCAPS_CTRLVOLUME) strcat(eb, "CTRLVOLUME+");
    if (c & DSBCAPS_CTRLPOSITIONNOTIFY) strcat(eb, "CTRLPOSITIONNOTIFY+");
    if (c & DSBCAPS_CTRLFX) strcat(eb, "CTRLFX+");
    if (c & DSBCAPS_STICKYFOCUS) strcat(eb, "STICKYFOCUS+");
    if (c & DSBCAPS_GLOBALFOCUS) strcat(eb, "GLOBALFOCUS+");
    if (c & DSBCAPS_GETCURRENTPOSITION2) strcat(eb, "GETCURRENTPOSITION2+");
    if (c & DSBCAPS_MUTE3DATMAXDISTANCE) strcat(eb, "MUTE3DATMAXDISTANCE+");
    if (c & DSBCAPS_LOCDEFER) strcat(eb, "LOCDEFER+");
    l = strlen(eb);
    if (l > strlen("DSBCAPS_")) eb[l - 1] = 0; // delete last '+' if any
    else eb[0] = 0;
    return(eb);
}

static char *ExplainPlayFlags(DWORD c) {
    static char eb[512];
    unsigned int l;
    strcpy(eb, "DSBPLAY_");
    if (c & DSBPLAY_LOOPING) strcat(eb, "LOOPING+");
    if (c & DSBPLAY_LOCHARDWARE) strcat(eb, "LOCHARDWARE+");
    if (c & DSBPLAY_LOCSOFTWARE) strcat(eb, "LOCSOFTWARE+");
    if (c & DSBPLAY_TERMINATEBY_TIME) strcat(eb, "TERMINATEBY_TIME+");
    if (c & DSBPLAY_TERMINATEBY_DISTANCE) strcat(eb, "TERMINATEBY_DISTANCE+");
    if (c & DSBPLAY_TERMINATEBY_PRIORITY) strcat(eb, "TERMINATEBY_PRIORITY+");
    l = strlen(eb);
    if (l > strlen("DSBPLAY_")) eb[l - 1] = 0; // delete last '+' if any
    else eb[0] = 0;
    return(eb);
}

static char *ExplainDSLockFlags(DWORD c) {
    static char eb[512];
    unsigned int l;
    strcpy(eb, "DSBLOCK_");
    if (c & DSBLOCK_FROMWRITECURSOR) strcat(eb, "FROMWRITECURSOR+");
    if (c & DSBLOCK_ENTIREBUFFER) strcat(eb, "ENTIREBUFFER+");
    l = strlen(eb);
    if (l > strlen("DSBLOCK_")) eb[l - 1] = 0; // delete last '+' if any
    else eb[0] = 0;
    return(eb);
}

#endif // DXW_NOTRACES

//----------------------------------------------------------------------//
// Service
//----------------------------------------------------------------------//

void dxwInitVolume() {
    typedef MMRESULT (WINAPI * waveOutSetVolume_Type)(HWAVEOUT, DWORD);
    waveOutSetVolume_Type pwaveOutSetVolume;
    DWORD volume;
    HMODULE hWinMM = (*pLoadLibraryA)("winmm.dll");
    pwaveOutSetVolume = (waveOutSetVolume_Type)(*pGetProcAddress)(hWinMM, "waveOutSetVolume");
    volume = (dxw.FixedVolume * 0xFFFF) / 100; // scale in range 0 - 0xFFFF
    volume |= (volume << 16); // set left+right channels
    //(*pwaveOutSetVolume)(NULL, 0x8FFF8FFF); // initialize at 50% volume
    (*pwaveOutSetVolume)(NULL, volume);
}

//----------------------------------------------------------------------//
// Proxies
//----------------------------------------------------------------------//

HRESULT WINAPI extDirectSoundCreate(LPGUID guid, LPDIRECTSOUND *lpds, LPUNKNOWN unk) {
    HRESULT res;
    ApiName("DirectSoundCreate");
    OutTraceSND("%s: guid=%#x\n", ApiRef, guid);
    res = (*pDirectSoundCreate)(guid, lpds, unk);
    if(res)
        TraceDSError(res);
    else {
        OutTraceSND("%s: lpDS=%#x\n", ApiRef, *lpds);
        HookDirectSoundObj(lpds);
    }
    return res;
}

HRESULT WINAPI extDirectSoundCreate8(LPCGUID lpcGuidDevice, LPDIRECTSOUND8 *ppDS8, LPUNKNOWN pUnkOuter) {
    HRESULT res;
    ApiName("DirectSoundCreate8");
    OutTraceSND("%s: guid=%#x\n", ApiRef, lpcGuidDevice);
    res = (*pDirectSoundCreate8)(lpcGuidDevice, ppDS8, pUnkOuter);
    res = DD_OK;
    if(res)
        TraceDSError(res);
    else {
        OutTraceSND("%s: lpDS8=%#x\n", ApiRef, *ppDS8);
        HookDirectSoundObj((LPDIRECTSOUND *)ppDS8);
    }
    return res;
}

#ifdef TRACEALLSOUNDS
HRESULT WINAPI extDSQueryInterface(void *lpds, REFIID riid, LPVOID *ppvObj) {
    HRESULT res;
    ApiName("IDirectSound::QueryInterface");
    OutTraceSND("%s: lpdds=%#x REFIID=%#x(%s)\n", ApiRef, lpds, riid.Data1, ExplainGUID((GUID *)&riid));
    res = (*pDSQueryInterface)(lpds, riid, ppvObj);
    _if(res) TraceDSError(res);
    return res;
}
#endif // TRACEALLSOUNDS

HRESULT WINAPI extDSSetCooperativeLevel(void *lpds, HWND hwnd, DWORD dwLevel) {
    HRESULT res;
    ApiName("IDirectSound::SetCooperativeLevel");
    OutTraceSND("%s: lpds=%#x hwnd=%#x level=%#x(%s)\n", ApiRef, lpds, hwnd, dwLevel, ExplainLevel(dwLevel));
    if(dwLevel == DSSCL_EXCLUSIVE) {
        dwLevel = DSSCL_PRIORITY; // Arxel Tribe patch
        OutTraceSND("%s: PATCH level=DSSCL_PRIORITY hwnd=%#x\n", ApiRef, hwnd);
    }
    if((dwLevel == DSSCL_PRIORITY) && (hwnd == 0)) {
        if(dxw.GethWnd()) hwnd = dxw.GethWnd(); // v2.04.69: "Ashgan" D3D version
        else dwLevel = DSSCL_NORMAL; // v2.05.15: "Wheel of Time" ...
        OutTraceSND("%s: PATCH level=%s hwnd=%#x\n", ApiRef, ExplainLevel(dwLevel), hwnd);
    }
    // v2.05.38 fix: "Championship Manager 2006"
    if((dwLevel == DSSCL_NORMAL) && (hwnd == 0)) {
        if(dxw.GethWnd()) hwnd = dxw.GethWnd(); //
        // else dwLevel = DSSCL_EXCLUSIVE; // else ???
        OutTraceSND("%s: PATCH level=%s hwnd=%#x\n", ApiRef, ExplainLevel(dwLevel), hwnd);
    }
    res = (*pDSSetCooperativeLevel)(lpds, hwnd, dwLevel);
    _if(res) TraceDSError(res);
    if(dxw.dwFlags12 & DSINITVOLUME) dxwInitVolume();
    return res;
}

#ifdef TRACEALLSOUNDS
HRESULT WINAPI extDSGetCaps(void *lpds, LPDSCAPS pDSCaps) {
    HRESULT res;
    ApiName("IDirectSound::GetCaps");
    OutTraceSND("%s: lpds=%#x\n", ApiRef, lpds);
    res = (*pDSGetCaps)(lpds, pDSCaps);
    if(res) {
        TraceDSError(res);
        return res;
    }
    OutDebugSND("%s: caps={size=%d flags=%#x(%s) SecSampleRate(min:max)=(%d:%d) primbuffers=%d "
                "MaxHwMixingBuffers(All:Static:Streaming)=(%d:%d:%d) "
                "FreeHwMixingBuffers(All:Static:Streaming)=(%d:%d:%d) "
                "MaxHw3DBuffers(All:Static:Streaming)=(%d:%d:%d) "
                "FreeHw3DBuffers(All:Static:Streaming)=(%d:%d:%d) "
                "TotalHwMemBytes=%d FreeHwMemBytes=%d MaxContigFreeHwMemBytes=%d UnlockTransferRateHwBuffers=%d PlayCpuOverheadSwBuffers=%d}\n",
                ApiRef,
                pDSCaps->dwSize, pDSCaps->dwFlags, ExplainDSFlags(pDSCaps->dwFlags),
                pDSCaps->dwMinSecondarySampleRate, pDSCaps->dwMaxSecondarySampleRate,
                pDSCaps->dwPrimaryBuffers,
                pDSCaps->dwMaxHwMixingAllBuffers, pDSCaps->dwMaxHwMixingStaticBuffers, pDSCaps->dwMaxHwMixingStreamingBuffers,
                pDSCaps->dwFreeHwMixingAllBuffers, pDSCaps->dwFreeHwMixingStaticBuffers, pDSCaps->dwFreeHwMixingStreamingBuffers,
                pDSCaps->dwMaxHw3DAllBuffers, pDSCaps->dwMaxHw3DStaticBuffers, pDSCaps->dwMaxHw3DStreamingBuffers,
                pDSCaps->dwFreeHw3DAllBuffers, pDSCaps->dwFreeHw3DStaticBuffers, pDSCaps->dwFreeHw3DStreamingBuffers,
                pDSCaps->dwTotalHwMemBytes, pDSCaps->dwFreeHwMemBytes, pDSCaps->dwMaxContigFreeHwMemBytes,
                pDSCaps->dwUnlockTransferRateHwBuffers, pDSCaps->dwPlayCpuOverheadSwBuffers);
    return res;
}
#endif // TRACEALLSOUNDS

HRESULT WINAPI extDSCreateSoundBuffer(void *lpds, LPCDSBUFFERDESC1 pcDSBufferDesc, LPDIRECTSOUNDBUFFER *ppDSBuffer, LPUNKNOWN pUnkOuter) {
    HRESULT res;
    ApiName("IDirectSound::CreateSoundBuffer");
    OutTraceSND("%s: desc={flags=%#x(%s) BufferBytes=%d}\n", ApiRef, pcDSBufferDesc->dwFlags, ExplainDSBFlags(pcDSBufferDesc->dwFlags), pcDSBufferDesc->dwBufferBytes);
    switch(dxw.dwFlags5 & GLOBALFOCUSMASK) {
    case 0:
        break;
    case GLOBALFOCUSON:
        if(!(pcDSBufferDesc->dwFlags & DSBCAPS_PRIMARYBUFFER))
            ((DSBUFFERDESC1 *)pcDSBufferDesc)->dwFlags |= DSBCAPS_GLOBALFOCUS;
        break;
    case GLOBALFOCUSOFF:
        if(!(pcDSBufferDesc->dwFlags & DSBCAPS_PRIMARYBUFFER))
            ((DSBUFFERDESC1 *)pcDSBufferDesc)->dwFlags &= ~DSBCAPS_GLOBALFOCUS;
        break;
    }
    res = (*pDSCreateSoundBuffer)(lpds, pcDSBufferDesc, ppDSBuffer, pUnkOuter);
    // v2.05.53: capability check: DSBCAPS_LOCHARDWARE or DSBCAPS_LOCSOFTWARE could be requested without
    // that GetCaps lists these capabilities. In this case, you can try trimming them off.
    // Fixes "KA52 Team Alligator" missing sound.
    if((res == DSERR_UNSUPPORTED) && (pcDSBufferDesc->dwFlags & (DSBCAPS_LOCHARDWARE | DSBCAPS_LOCSOFTWARE))) {
        OutTraceSND("%s: DSERR_UNSUPPORTED try clearing LOC caps\n", ApiRef);
        (DWORD)(pcDSBufferDesc->dwFlags) = pcDSBufferDesc->dwFlags & ~(DSBCAPS_LOCHARDWARE | DSBCAPS_LOCSOFTWARE);
        res = (*pDSCreateSoundBuffer)(lpds, pcDSBufferDesc, ppDSBuffer, pUnkOuter);
    }
    if(res) {
        OutTraceE("%s: ERROR res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    // Play method is different for primary buffer?
    if(pcDSBufferDesc->dwFlags & DSBCAPS_PRIMARYBUFFER)
        HookPrimaryBuffer(ppDSBuffer);
    else
        HookSoundBuffer(ppDSBuffer);
    OutTraceSND("%s: lpbuf=%#x\n", ApiRef, *ppDSBuffer);
    return res;
}

HRESULT WINAPI extDSDuplicateSoundBuffer (void *lpds, LPDIRECTSOUNDBUFFER pDSBuffer, LPDIRECTSOUNDBUFFER *ppDSBuffer) {
    HRESULT res;
    ApiName("IDirectSound::DuplicateSoundBuffer");
    OutTraceSND("%s: lpds=%#x soundbuffer=%#x\n", ApiRef, lpds, pDSBuffer);
    res = (*pDSDuplicateSoundBuffer)(lpds, pDSBuffer, ppDSBuffer);
    if(res) {
        OutTraceE("%s: ERROR res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
        return res;
    }
    HookSoundBuffer(ppDSBuffer);
    OutTraceSND("%s: lpbuf=%#x\n", ApiRef, *ppDSBuffer);
    return res;
}

#ifdef TRACEALLSOUNDS
HRESULT WINAPI extDSCompact(void *lpds) {
    HRESULT res;
    ApiName("IDirectSound::Compact");
    OutTraceSND("%s: lpds=%#x\n", ApiRef, lpds);
    res = (*pDSCompact)(lpds);
    _if(res) TraceDSError(res);
    return res;
}

HRESULT WINAPI extDirectSoundEnumerateA(LPDSENUMCALLBACKA pDSEnumCallback, LPVOID pContext) {
    HRESULT res;
    ApiName("DirectSoundEnumerateA");
    OutTraceSND("%s\n", ApiRef);
    res = (*pDirectSoundEnumerateA)(pDSEnumCallback, pContext);
    _if(res) TraceDSError(res);
    return res;
}

HRESULT WINAPI extDirectSoundEnumerateW(LPDSENUMCALLBACKW pDSEnumCallback, LPVOID pContext) {
    HRESULT res;
    ApiName("DirectSoundEnumerateW");
    res = (*pDirectSoundEnumerateW)(pDSEnumCallback, pContext);
    _if(res) TraceDSError(res);
    return res;
}

HRESULT WINAPI extDSGetSpeakerConfig(void *lpds, LPDWORD pdwSpeakerConfig) {
    HRESULT res;
    ApiName("IDirectSound::GetSpeakerConfig");
    OutTraceSND("%s\n", ApiRef);
    res = (*pDSGetSpeakerConfig)(lpds, pdwSpeakerConfig);
    _if(res) TraceDSError(res);
    return res;
}

HRESULT WINAPI extDSSetSpeakerConfig(void *lpds, DWORD pdwSpeakerConfig) {
    HRESULT res;
    ApiName("IDirectSound::SetSpeakerConfig");
    OutTraceSND("%s\n", ApiRef);
    res = (*pDSSetSpeakerConfig)(lpds, pdwSpeakerConfig);
    _if(res) TraceDSError(res);
    return res;
}

HRESULT WINAPI extDSInitialize(void *lpds, LPCGUID pcGuidDevice) {
    HRESULT res;
    ApiName("IDirectSound::Initialize");
    OutTraceSND("%s\n", ApiRef);
    res = (*pDSInitialize)(lpds, pcGuidDevice);
    _if(res) TraceDSError(res);
    return res;
}
#endif // TRACEALLSOUNDS

HRESULT WINAPI extPlay(Play_Type pPlay, void *lpb, DWORD dwReserved1, DWORD dwPriority, DWORD dwFlags) {
    HRESULT res;
    ApiName("DirectSoundBuffer::Play");
    OutTraceSND("%s: lpbuf=%#x reserved=%#x priority=%#x flags=%#x(%s)\n", ApiRef, lpb, dwReserved1, dwPriority, dwFlags, ExplainPlayFlags(dwFlags));
    if(dxw.dwFlags9 & SOUNDMUTE) {
        OutTraceSND("%s: MUTE\n", ApiRef);
        return S_OK;
    }
    res = (*pPlay)(lpb, dwReserved1, dwPriority, dwFlags);
    _if(res) TraceDSError(res);
    return res;
}

HRESULT WINAPI extDSSBPlay(void *lpb, DWORD dwReserved1, DWORD dwPriority, DWORD dwFlags) {
    return extPlay(pDSSBPlay, lpb, dwReserved1, dwPriority, dwFlags);
}
HRESULT WINAPI extDSPBPlay(void *lpb, DWORD dwReserved1, DWORD dwPriority, DWORD dwFlags) {
    return extPlay(pDSPBPlay, lpb, dwReserved1, dwPriority, dwFlags);
}

HRESULT WINAPI extDSBSetVolume(DSBSetVolume_Type pSetVolume, void *lpb, LONG lVolume) {
    HRESULT res;
    ApiName("IDirectSoundBuffer::SetVolume");
    OutTraceSND("%s: lpbuf=%#x volume=%ld\n", ApiRef, lpb, lVolume);
    if(dxw.dwFlags9 & LOCKVOLUME) {
        OutTraceSND("%s: LOCK\n", ApiRef);
        return S_OK;
    }
    res = (*pSetVolume)(lpb, lVolume);
    _if(res) TraceDSError(res);
    return res;
}

HRESULT WINAPI extDSSBSetVolume(void *lpb, LONG lVolume) {
    return extDSBSetVolume(pDSSBSetVolume, lpb, lVolume);
}
HRESULT WINAPI extDSPBSetVolume(void *lpb, LONG lVolume) {
    return extDSBSetVolume(pDSPBSetVolume, lpb, lVolume);
}

#ifdef TRACEALLSOUNDS
HRESULT WINAPI extDSBSetPan(DSBSetPan_Type pSetPan, void *lpb, LONG lPan) {
    HRESULT res;
    ApiName("IDirectSoundBuffer::SetPan");
    OutTraceSND("%s: lpbuf=%#x pan=%ld\n", ApiRef, lpb, lPan);
    res = (*pSetPan)(lpb, lPan);
    _if(res) TraceDSError(res);
    return res;
}
HRESULT WINAPI extDSSBSetPan(void *lpb, LONG lPan) {
    return extDSBSetPan(pDSSBSetPan, lpb, lPan);
}
HRESULT WINAPI extDSPBSetPan(void *lpb, LONG lPan) {
    return extDSBSetPan(pDSPBSetPan, lpb, lPan);
}
#endif // TRCEALLSOUNDS

#ifdef TRACEALLSOUNDS
HRESULT WINAPI extDSBSetFrequency(DSBSetFrequency_Type pSetFrequency, void *lpb, DWORD dwFrequency) {
    HRESULT res;
    ApiName("IDirectSoundBuffer::SetFrequency");
    OutTraceSND("%s: lpbuf=%#x frequency=%ld\n", ApiRef, lpb, dwFrequency);
    res = (*pSetFrequency)(lpb, dwFrequency);
    _if(res) TraceDSError(res);
    return res;
}
HRESULT WINAPI extDSSBSetFrequency(void *lpb, DWORD dwFrequency) {
    return extDSBSetFrequency(pDSSBSetFrequency, lpb, dwFrequency);
}
HRESULT WINAPI extDSPBSetFrequency(void *lpb, DWORD dwFrequency) {
    return extDSBSetFrequency(pDSPBSetFrequency, lpb, dwFrequency);
}
#endif // TRCEALLSOUNDS

#ifdef TRACEALLSOUNDS
HRESULT WINAPI extDSBStop(Void_Type pStop, void *lpb) {
    HRESULT res;
    ApiName("IDirectSoundBuffer::Stop");
    OutTraceSND("%s: lpbuf=%#x\n", ApiRef, lpb);
    res = (*pStop)(lpb);
    _if(res) TraceDSError(res);
    return res;
}
HRESULT WINAPI extDSSBStop(void *lpb) {
    return extDSBStop(pDSSBStop, lpb);
}
HRESULT WINAPI extDSPBStop(void *lpb) {
    return extDSBStop(pDSPBStop, lpb);
}
#endif // TRCEALLSOUNDS

#ifdef TRACEALLSOUNDS
HRESULT WINAPI extDSBRestore(Void_Type pRestore, void *lpb) {
    HRESULT res;
    ApiName("IDirectSoundBuffer::Restore");
    OutTraceSND("%s: lpbuf=%#x\n", ApiRef, lpb);
    res = (*pRestore)(lpb);
    _if(res) TraceDSError(res);
    return res;
}

HRESULT WINAPI extDSSBRestore(void *lpb) {
    return extDSBRestore(pDSSBRestore, lpb);
}
HRESULT WINAPI extDSPBRestore(void *lpb) {
    return extDSBRestore(pDSPBRestore, lpb);
}
#endif // TRCEALLSOUNDS

#ifdef TRACEALLSOUNDS
HRESULT WINAPI extDSBSetCurrentPosition(DSBSetCurrentPosition_Type pSetCurrentPosition, void *lpb, DWORD dwNewPosition) {
    HRESULT res;
    ApiName("IDirectSoundBuffer::SetCurrentPosition");
    OutTraceSND("%s: lpbuf=%#x position=%#x\n", ApiRef, lpb, dwNewPosition);
    res = (*pSetCurrentPosition)(lpb, dwNewPosition);
    _if(res) TraceDSError(res);
    return res;
}

HRESULT WINAPI extDSSBSetCurrentPosition(void *lpb, DWORD dwNewPosition) {
    return extDSBSetCurrentPosition(pDSSBSetCurrentPosition, lpb, dwNewPosition);
}
HRESULT WINAPI extDSPBSetCurrentPosition(void *lpb, DWORD dwNewPosition) {
    return extDSBSetCurrentPosition(pDSPBSetCurrentPosition, lpb, dwNewPosition);
}
#endif // TRCEALLSOUNDS

#ifdef TRACEALLSOUNDS
HRESULT WINAPI extDSBSetFormat(DSBSetFormat_Type pSetFormat, void *lpb, LPCWAVEFORMATEX pcfxFormat) {
    HRESULT res;
    ApiName("IDirectSoundBuffer::SetFormat");
    OutTraceSND("%s: lpbuf=%#x\n", ApiRef, lpb);
    OutTraceSND("> wFormatTag=%#x\n", pcfxFormat->wFormatTag);
    OutTraceSND("> nChannels=%d\n", pcfxFormat->nChannels);
    OutTraceSND("> nSamplesPerSec=%d\n", pcfxFormat->nSamplesPerSec);
    OutTraceSND("> nAvgBytesPerSec=%d\n", pcfxFormat->nAvgBytesPerSec);
    OutTraceSND("> nBlockAlign=%d\n", pcfxFormat->nBlockAlign);
    OutTraceSND("> wBitsPerSample=%d\n", pcfxFormat->wBitsPerSample);
    OutTraceSND("> cbSize=%d\n", pcfxFormat->cbSize);
    res = (*pSetFormat)(lpb, pcfxFormat);
    _if(res) TraceDSError(res);
    return res;
}

HRESULT WINAPI extDSSBSetFormat(void *lpb, LPCWAVEFORMATEX pcfxFormat) {
    return extDSBSetFormat(pDSSBSetFormat, lpb, pcfxFormat);
}
HRESULT WINAPI extDSPBSetFormat(void *lpb, LPCWAVEFORMATEX pcfxFormat) {
    return extDSBSetFormat(pDSPBSetFormat, lpb, pcfxFormat);
}
#endif // TRCEALLSOUNDS

#ifdef TRACEALLSOUNDS
HRESULT WINAPI extDSBLock(DSBLock_Type pLock, void *lpb, DWORD dwOffset, DWORD dwBytes, LPVOID *ppvAudioPtr1, LPDWORD pdwAudioBytes1, LPVOID *ppvAudioPtr2, LPDWORD pdwAudioBytes2, DWORD dwFlags) {
    HRESULT res;
    ApiName("IDirectSoundBuffer::Lock");
    OutTraceSND("%s: lpbuf=%#x offset=%d bytes=%d flags=%#x(%s)\n",
                ApiRef, lpb, dwOffset, dwBytes, dwFlags, ExplainDSLockFlags(dwFlags));
    res = (*pLock)(lpb, dwOffset, dwBytes, ppvAudioPtr1, pdwAudioBytes1, ppvAudioPtr2, pdwAudioBytes2, dwFlags);
    _if(res) TraceDSError(res);
    return res;
}

HRESULT WINAPI extDSSBLock(void *lpb, DWORD dwOffset, DWORD dwBytes, LPVOID *ppvAudioPtr1, LPDWORD pdwAudioBytes1, LPVOID *ppvAudioPtr2, LPDWORD pdwAudioBytes2, DWORD dwFlags) {
    return extDSBLock(pDSSBLock, lpb, dwOffset, dwBytes, ppvAudioPtr1, pdwAudioBytes1, ppvAudioPtr2, pdwAudioBytes2, dwFlags);
}
HRESULT WINAPI extDSPBLock(void *lpb, DWORD dwOffset, DWORD dwBytes, LPVOID *ppvAudioPtr1, LPDWORD pdwAudioBytes1, LPVOID *ppvAudioPtr2, LPDWORD pdwAudioBytes2, DWORD dwFlags) {
    return extDSBLock(pDSPBLock, lpb, dwOffset, dwBytes, ppvAudioPtr1, pdwAudioBytes1, ppvAudioPtr2, pdwAudioBytes2, dwFlags);
}
#endif // TRCEALLSOUNDS

HRESULT WINAPI extDSBUnlock(DSBUnlock_Type pUnlock, void *lpb, LPVOID pvAudioPtr1, DWORD dwAudioBytes1, LPVOID pvAudioPtr2, DWORD dwAudioBytes2) {
    HRESULT res;
    ApiName("IDirectSoundBuffer::Unlock");
    OutTraceSND("%s: lpbuf=%#x\n", ApiRef, lpb);
    // this trick seems the only way to stop a background music loaded to primary/setvol buffers and
    // playing apparently without issuing the Play method.
    if(dxw.dwFlags9 & SOUNDMUTE) {
        OutTraceSND("%s: MUTE\n", ApiRef);
        dwAudioBytes1 = 0;
        dwAudioBytes2 = 0;
    }
    res = (*pUnlock)(lpb, pvAudioPtr1, dwAudioBytes1, pvAudioPtr2, dwAudioBytes2);
    _if(res) TraceDSError(res);
    return res;
}

HRESULT WINAPI extDSSBUnlock(void *lpb, LPVOID pvAudioPtr1, DWORD dwAudioBytes1, LPVOID pvAudioPtr2, DWORD dwAudioBytes2) {
    return extDSBUnlock(pDSSBUnlock, lpb, pvAudioPtr1, dwAudioBytes1, pvAudioPtr2, dwAudioBytes2);
}
HRESULT WINAPI extDSPBUnlock(void *lpb, LPVOID pvAudioPtr1, DWORD dwAudioBytes1, LPVOID pvAudioPtr2, DWORD dwAudioBytes2) {
    return extDSBUnlock(pDSPBUnlock, lpb, pvAudioPtr1, dwAudioBytes1, pvAudioPtr2, dwAudioBytes2);
}

#ifdef TRACEALLSOUNDS
HRESULT WINAPI extDSBInitialize(DSBInitialize_Type pInitialize, void *lpb, LPDIRECTSOUND pDirectSound, LPCDSBUFFERDESC1 pcDSBufferDesc) {
    HRESULT res;
    ApiName("IDirectSoundBuffer::Initialize");
    OutTraceSND("%s: lpbuf=%#x lpds=%#x desc={flags=%#x(%s) BufferBytes=%d}\n",
                ApiRef, lpb, pDirectSound, pcDSBufferDesc->dwFlags, ExplainDSBFlags(pcDSBufferDesc->dwFlags), pcDSBufferDesc->dwBufferBytes);
    res = (*pDSSBInitialize)(lpb, pDirectSound, pcDSBufferDesc);
    _if(res) TraceDSError(res);
    return res;
}

HRESULT WINAPI extDSSBInitialize(void *lpb, LPDIRECTSOUND pDirectSound, LPCDSBUFFERDESC1 pcDSBufferDesc) {
    return extDSBInitialize(pDSSBInitialize, lpb, pDirectSound, pcDSBufferDesc);
}
HRESULT WINAPI extDSPBInitialize(void *lpb, LPDIRECTSOUND pDirectSound, LPCDSBUFFERDESC1 pcDSBufferDesc) {
    return extDSBInitialize(pDSPBInitialize, lpb, pDirectSound, pcDSBufferDesc);
}
#endif // TRACEALLSOUNDS

#ifdef TRACEALLSOUNDS
HRESULT WINAPI extDSBQueryInterface(QueryInterface_Type pQueryInterface, LPVOID lpds, REFIID riid, LPVOID *ppvObj) {
    HRESULT res;
    ApiName("IDirectSoundBuffer::QueryInterface");
    OutTraceSND("%s: lpdds=%#x REFIID=%#x(%s)\n", ApiRef, lpds, riid.Data1, ExplainGUID((GUID *)&riid));
    res = (*pQueryInterface)(lpds, riid, ppvObj);
    _if(res) TraceDSError(res);
    return res;
}

HRESULT WINAPI extDSSBQueryInterface(LPVOID lpds, REFIID riid, LPVOID *ppvObj) {
    return extDSBQueryInterface(pDSSBQueryInterface, lpds, riid, ppvObj);
}
HRESULT WINAPI extDSPBQueryInterface(LPVOID lpds, REFIID riid, LPVOID *ppvObj) {
    return extDSBQueryInterface(pDSPBQueryInterface, lpds, riid, ppvObj);
}
#endif // TRACEALLSOUNDS

#ifdef TRACEALLSOUNDS
HRESULT WINAPI extDSBGetCaps(DSBGetCaps_Type pGetCaps, void *lpb, LPDSBCAPS pDSBufferCaps) {
    HRESULT res;
    ApiName("IDirectSoundBuffer::GetCaps");
    OutTraceSND("%s: lpds=%#x\n", ApiRef, lpb);
    res = (*pGetCaps)(lpb, pDSBufferCaps);
    if(res) {
        TraceDSError(res);
        return res;
    }
    OutTraceSND("%s: caps={Size=%d Flags=%#x(%s) BufferBytes=%d UnlockTransferRate=%#x PlayCpuOverhead=%#x}\n",
                ApiRef, pDSBufferCaps->dwSize, pDSBufferCaps->dwFlags, ExplainDSBFlags(pDSBufferCaps->dwFlags),
                pDSBufferCaps->dwBufferBytes, pDSBufferCaps->dwUnlockTransferRate, pDSBufferCaps->dwPlayCpuOverhead);
    return res;
}

HRESULT WINAPI extDSSBGetCaps(void *lpb, LPDSBCAPS pDSBufferCaps) {
    return extDSBGetCaps(pDSSBGetCaps, lpb, pDSBufferCaps);
}
HRESULT WINAPI extDSPBGetCaps(void *lpb, LPDSBCAPS pDSBufferCaps) {
    return extDSBGetCaps(pDSPBGetCaps, lpb, pDSBufferCaps);
}
#endif // TRACEALLSOUNDS

#ifdef TRACEALLSOUNDS
HRESULT WINAPI extDSBGetCurrentPosition(DSBGetCurrentPosition_Type pGetCurrentPosition, void *lpdsb, LPDWORD pdwCurrentPlayCursor, LPDWORD pdwCurrentWriteCursor) {
    HRESULT res;
    ApiName("IDirectSoundBuffer::GetCurrentPosition");
    OutTraceSND("%s: lpdsb=%#x\n", ApiRef, lpdsb);
    res = (*pGetCurrentPosition)(lpdsb, pdwCurrentPlayCursor, pdwCurrentWriteCursor);
    if(res) {
        TraceDSError(res);
        return res;
    }
    OutTraceSND("%s: play=%d write=%d\n", ApiRef,
                pdwCurrentPlayCursor ? *pdwCurrentPlayCursor : 0,
                pdwCurrentWriteCursor ? *pdwCurrentWriteCursor : 0);
    return res;
}

HRESULT WINAPI extDSSBGetCurrentPosition(void *lpdsb, LPDWORD pdwCurrentPlayCursor, LPDWORD pdwCurrentWriteCursor) {
    return extDSBGetCurrentPosition(pDSSBGetCurrentPosition, lpdsb, pdwCurrentPlayCursor, pdwCurrentWriteCursor);
}
HRESULT WINAPI extDSPBGetCurrentPosition(void *lpdsb, LPDWORD pdwCurrentPlayCursor, LPDWORD pdwCurrentWriteCursor) {
    return extDSBGetCurrentPosition(pDSPBGetCurrentPosition, lpdsb, pdwCurrentPlayCursor, pdwCurrentWriteCursor);
}
#endif // TRACEALLSOUNDS

#ifdef TRACEALLSOUNDS
HRESULT WINAPI extDSBGetFormat(DSBGetFormat_Type pGetFormat, void *lpdsb, LPWAVEFORMATEX pwfxFormat, DWORD dwSizeAllocated, LPDWORD pdwSizeWritten) {
    HRESULT res;
    ApiName("IDirectSoundBuffer::GetFormat");
    OutTraceSND("%s: lpdsb=%#x allocated=%d\n", ApiRef, lpdsb, dwSizeAllocated);
    res = (*pGetFormat)(lpdsb, pwfxFormat, dwSizeAllocated, pdwSizeWritten);
    if(res) {
        TraceDSError(res);
        return res;
    }
    OutTraceSND("> written=%d\n", pdwSizeWritten ? *pdwSizeWritten : 0);
    OutTraceSND("> wFormatTag=%#x\n", pwfxFormat->wFormatTag);
    OutTraceSND("> nChannels=%d\n", pwfxFormat->nChannels);
    OutTraceSND("> nSamplesPerSec=%d\n", pwfxFormat->nSamplesPerSec);
    OutTraceSND("> nAvgBytesPerSec=%d\n", pwfxFormat->nAvgBytesPerSec);
    OutTraceSND("> nBlockAlign=%d\n", pwfxFormat->nBlockAlign);
    OutTraceSND("> wBitsPerSample=%d\n", pwfxFormat->wBitsPerSample);
    OutTraceSND("> cbSize=%d\n", pwfxFormat->cbSize);
    return res;
}

HRESULT WINAPI extDSSBGetFormat(void *lpdsb, LPWAVEFORMATEX pwfxFormat, DWORD dwSizeAllocated, LPDWORD pdwSizeWritten) {
    return extDSBGetFormat(pDSSBGetFormat, lpdsb, pwfxFormat, dwSizeAllocated, pdwSizeWritten);
}
HRESULT WINAPI extDSPBGetFormat(void *lpdsb, LPWAVEFORMATEX pwfxFormat, DWORD dwSizeAllocated, LPDWORD pdwSizeWritten) {
    return extDSBGetFormat(pDSPBGetFormat, lpdsb, pwfxFormat, dwSizeAllocated, pdwSizeWritten);
}
#endif // TRACEALLSOUNDS

#ifdef TRACEALLSOUNDS
HRESULT WINAPI extDSBGetVolume(DSBGetVolume_Type pGetVolume, void *lpdsb, LPLONG plVolume) {
    HRESULT res;
    ApiName("IDirectSoundBuffer::GetVolume");
    OutTraceSND("%s: lpdsb=%#x\n", ApiRef, lpdsb);
    res = (*pGetVolume)(lpdsb, plVolume);
    if(res) {
        TraceDSError(res);
        return res;
    }
    OutTraceSND("%s: volume=%d\n", ApiRef, *plVolume);
    return res;
}

HRESULT WINAPI extDSSBGetVolume(void *lpdsb, LPLONG plVolume) {
    return extDSBGetVolume(pDSSBGetVolume, lpdsb, plVolume);
}
HRESULT WINAPI extDSPBGetVolume(void *lpdsb, LPLONG plVolume) {
    return extDSBGetVolume(pDSPBGetVolume, lpdsb, plVolume);
}
#endif // TRACEALLSOUNDS

#ifdef TRACEALLSOUNDS
HRESULT WINAPI extDSBGetPan(DSBGetPan_Type pGetPan, void *lpdsb, LPLONG plPan) {
    HRESULT res;
    ApiName("IDirectSoundBuffer::GetPan");
    OutTraceSND("%s: lpdsb=%#x\n", ApiRef, lpdsb);
    res = (*pGetPan)(lpdsb, plPan);
    if(res) {
        TraceDSError(res);
        return res;
    }
    OutTraceSND("%s: pan=%d\n", ApiRef, *plPan);
    return res;
}

HRESULT WINAPI extDSSBGetPan(void *lpdsb, LPLONG plPan) {
    return extDSBGetPan(pDSSBGetPan, lpdsb, plPan);
}
HRESULT WINAPI extDSPBGetPan(void *lpdsb, LPLONG plPan) {
    return extDSBGetPan(pDSPBGetPan, lpdsb, plPan);
}
#endif // TRACEALLSOUNDS

#ifdef TRACEALLSOUNDS
HRESULT WINAPI extDSBGetFrequency(DSBGetFrequency_Type pGetFrequency, void *lpdsb, LPDWORD pdwFrequency) {
    HRESULT res;
    ApiName("IDirectSoundBuffer::GetFrequency");
    OutTraceSND("%s: lpdsb=%#x\n", ApiRef, lpdsb);
    res = (*pGetFrequency)(lpdsb, pdwFrequency);
    if(res) {
        TraceDSError(res);
        return res;
    }
    OutTraceSND("%s: frequency=%#x\n", ApiRef, *pdwFrequency);
    return res;
}

HRESULT WINAPI extDSSBGetFrequency(void *lpdsb, LPDWORD pdwFrequency) {
    return extDSBGetFrequency(pDSSBGetFrequency, lpdsb, pdwFrequency);
}
HRESULT WINAPI extDSPBGetFrequency(void *lpdsb, LPDWORD pdwFrequency) {
    return extDSBGetFrequency(pDSPBGetFrequency, lpdsb, pdwFrequency);
}
#endif // TRACEALLSOUNDS

#ifdef TRACEALLSOUNDS
HRESULT WINAPI extDSBGetStatus(DSBGetStatus_Type pGetStatus, void *lpdsb, LPDWORD pdwStatus) {
    HRESULT res;
    ApiName("IDirectSoundBuffer::GetStatus");
    OutTraceSND("%s: lpdsb=%#x pstatus=%#x\n", ApiRef, lpdsb, pdwStatus);
    res = (*pGetStatus)(lpdsb, pdwStatus);
    if(res) {
        TraceDSError(res);
        return res;
    }
    OutTraceSND("%s: status=%#x\n", ApiRef, *pdwStatus);
    return res;
}
HRESULT WINAPI extDSSBGetStatus(void *lpdsb, LPDWORD pdwStatus) {
    return extDSBGetStatus(pDSSBGetStatus, lpdsb, pdwStatus);
}
HRESULT WINAPI extDSPBGetStatus(void *lpdsb, LPDWORD pdwStatus) {
    return extDSBGetStatus(pDSPBGetStatus, lpdsb, pdwStatus);
}
#endif // TRACEALLSOUNDS
