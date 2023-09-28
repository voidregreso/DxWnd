/*
    DXWnd/dxwnd.cpp
    DirectX Hook Module
    Copyright(C) 2004-2017 SFB7/GHO

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#ifdef DWX_ACL_ANYBODY_FULL_ACCESS
#include <aclapi.h>
#include <accctrl.h>
#endif // DWX_ACL_ANYBODY_FULL_ACCESS
#include "dxwnd.h"
#include "dxwcore.hpp"

#include "TlHelp32.h"

#ifdef _DEBUG
#define DEBUGSUFFIX ".debug"
#else
#define DEBUGSUFFIX
#endif

#ifdef DXW_NOTRACES
#define STREAMSUFFIX ".stream"
#else
#define STREAMSUFFIX
#endif
#ifdef TRACEALL
#define TRACESUFFIX ".trace"
#else
#define TRACESUFFIX
#endif
#define VERSION "v2.05.54" DEBUGSUFFIX STREAMSUFFIX TRACESUFFIX

#define DDTHREADLOCK 1

LRESULT CALLBACK HookProc(int ncode, WPARAM wparam, LPARAM lparam);

HINSTANCE hInst;
HHOOK hHook = NULL;
HHOOK hMouseHook = NULL;
HHOOK hKeyboardHook = NULL;
HANDLE hMapping;
TARGETMAP *pMapping;
DXWNDSTATUS *pStatus;
HANDLE hMutex;
HANDLE hTraceMutex;
HANDLE hLockMutex;
HANDLE hDDLockMutex;
HANDLE hKillMutex;
int HookStatus = DXW_IDLE;
static int TaskIndex = -1;
DXWNDSTATUS DxWndStatus;

void InjectHook();

/* ------------------------------------------------------------------ */
// filters table (to be sync with GUI!!)
/* ------------------------------------------------------------------ */

dxw_Filter_Type dxwFilters[] = {
    {"none",				DXW_FILTER_NONE,		0, 0},
    {"fast bilinear 2x",	DXW_FILTER_BILX2,		2, 2},
    {"HQ 2x",				DXW_FILTER_HQX2,		2, 2},
    {"HQ 3x",				DXW_FILTER_HQX3,		3, 3},
    {"HQ 4x",				DXW_FILTER_HQX4,		4, 4},
    {"deinterlace",			DXW_FILTER_DEINTERLACE,	0, 0},
    {"interlace",			DXW_FILTER_INTERLACE,	0, 0},
    {"PIX 2x",				DXW_FILTER_PIX2,		2, 2},
    {"PIX 3x",				DXW_FILTER_PIX3,		3, 3},
    {"PIX 4x",				DXW_FILTER_PIX4,		4, 4},
    {"Scale2X 2x",			DXW_FILTER_SCALE2X,		2, 2},
    {"Scale2X 3x",			DXW_FILTER_SCALE3X,		3, 3},
    {"Scale2X 4x",			DXW_FILTER_SCALE4X,		4, 4},
    {"Scale2K 2x",			DXW_FILTER_SCALE2K,		2, 2},
    {"Scale2K 3x",			DXW_FILTER_SCALE3K,		3, 3},
    {"Scale2K 4x",			DXW_FILTER_SCALE4K,		4, 4},
    {NULL, 0, 0, 0}					// terminator
};

dxw_Renderer_Type dxwRenderers[] = {
    {"none",				DXW_RENDERER_NONE,			0,															},
    {"primary buffer",		DXW_RENDERER_EMULATEBUFFER,	DXWRF_BUILDREALSURFACES | DXWRF_BUILDLOCKBUFFER				},
    {"locked surface",		DXW_RENDERER_LOCKEDSURFACE, DXWRF_BUILDLOCKSURFACE										},
    {"primary surface",		DXW_RENDERER_PRIMSURFACE,	DXWRF_EMULATED | DXWRF_BUILDREALSURFACES | DXWRF_SUPPORTFILTERS	},
    {"hybrid",				DXW_RENDERER_HYBRID,		DXWRF_EMULATED | DXWRF_BUILDREALSURFACES						},
    {"gdi",					DXW_RENDERER_GDI,			DXWRF_EMULATED | DXWRF_PALETTEREFRESH | DXWRF_USEBACKBUFFER		},
    {"SDL",					DXW_RENDERER_SDL,			DXWRF_EMULATED | DXWRF_PALETTEREFRESH | DXWRF_USEBACKBUFFER		},
    {"SDL2",				DXW_RENDERER_SDL2,			DXWRF_EMULATED | DXWRF_PALETTEREFRESH | DXWRF_USEBACKBUFFER		},
    {"OpenGL",				DXW_RENDERER_OPENGL,		DXWRF_EMULATED | DXWRF_PALETTEREFRESH | DXWRF_USEBACKBUFFER		},
    {NULL, 0, 0}			// terminator
};

#ifdef DWX_ACL_ANYBODY_FULL_ACCESS
void SetSecurityForEverybody(SECURITY_ATTRIBUTES *lpsa) {
    SID_IDENTIFIER_AUTHORITY SIDAuthWorld = SECURITY_WORLD_SID_AUTHORITY;
    PSID everyone_sid = NULL;
    AllocateAndInitializeSid(&SIDAuthWorld, 1, SECURITY_WORLD_RID,
                             0, 0, 0, 0, 0, 0, 0, &everyone_sid);
    EXPLICIT_ACCESS ea;
    ZeroMemory(&ea, sizeof(EXPLICIT_ACCESS));
    ea.grfAccessPermissions = SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;
    ea.grfAccessMode = SET_ACCESS;
    ea.grfInheritance = NO_INHERITANCE;
    ea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
    ea.Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
    ea.Trustee.ptstrName  = (LPTSTR)everyone_sid;
    PACL acl = NULL;
    SetEntriesInAcl(1, &ea, NULL, &acl);
    PSECURITY_DESCRIPTOR sd = (PSECURITY_DESCRIPTOR)LocalAlloc(LPTR,
                              SECURITY_DESCRIPTOR_MIN_LENGTH);
    InitializeSecurityDescriptor(sd, SECURITY_DESCRIPTOR_REVISION);
    SetSecurityDescriptorDacl(sd, TRUE, acl, FALSE);
    lpsa->nLength = sizeof(SECURITY_ATTRIBUTES);
    lpsa->lpSecurityDescriptor = sd;
    lpsa->bInheritHandle = FALSE;
}
#endif // DWX_ACL_ANYBODY_FULL_ACCESS

BOOL APIENTRY DllMain( HANDLE hmodule,
                       DWORD  dwreason,
                       LPVOID preserved
                     ) {
    HANDLE hCurrentThread;
    if(dwreason == DLL_PROCESS_DETACH) {
        if(pInvalidateRect) (*pInvalidateRect)(0, NULL, FALSE); // invalidate full desktop, no erase.
        // UnmapViewOfFile(pMapping); // v2.04.83
        UnmapViewOfFile(pStatus); // v2.04.81
        CloseHandle(hMapping);
    }
    if(dwreason != DLL_PROCESS_ATTACH) return TRUE;
    hCurrentThread = GetCurrentThread();
    SetThreadPriority(hCurrentThread, THREAD_PRIORITY_HIGHEST); // trick to reduce concurrency problems at program startup
    hInst = (HINSTANCE)hmodule;
    // optimization: disables DLL_THREAD_ATTACH and DLL_THREAD_DETACH notifications for the specified DLL
    DisableThreadLibraryCalls((HMODULE)hmodule);
#ifdef DWX_ACL_ANYBODY_FULL_ACCESS
    SECURITY_ATTRIBUTES sa;
    SetSecurityForEverybody(&sa);
    hMapping = CreateFileMapping(INVALID_HANDLE_VALUE, &sa, PAGE_READWRITE,
                                 0, sizeof(DxWndStatus) + sizeof(TARGETMAP) * MAXTARGETS, "UniWind_TargetList");
#else
    hMapping = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE,
                                 0, sizeof(DxWndStatus) + sizeof(TARGETMAP) * MAXTARGETS, "UniWind_TargetList");
#endif // DWX_ACL_ANYBODY_FULL_ACCESS
    if(!hMapping) {
        SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
        return false;
    }
    // v2.0.2.75: beware: some tasks (namely, Flash player) get dxwnd.dll loaded, but can't create the file mapping
    // this situation has to be intercepted, or it can cause the dll to cause faults that may crash the program.
    pStatus = (DXWNDSTATUS *)MapViewOfFile(hMapping, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(DXWNDSTATUS) + sizeof(TARGETMAP) * MAXTARGETS);
    if(!pStatus) return false;
    pMapping = (TARGETMAP *)((char *)pStatus + sizeof(DXWNDSTATUS));
    hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, "UniWind_Mutex");
    if(!hMutex) hMutex = CreateMutex(0, FALSE, "UniWind_Mutex");
    hTraceMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, "Trace_Mutex");
    if(!hTraceMutex) hTraceMutex = CreateMutex(0, FALSE, "Trace_Mutex");
    hLockMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, "Lock_Mutex");
    if(!hLockMutex) hLockMutex = CreateMutex(0, FALSE, "Lock_Mutex");
    if(DDTHREADLOCK) {
        hDDLockMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, "DDLock_Mutex");
        if(!hDDLockMutex) hDDLockMutex = CreateMutex(0, FALSE, "DDLock_Mutex");
    }
    InjectHook();
    SetThreadPriority(hCurrentThread, THREAD_PRIORITY_NORMAL);
    CloseHandle(hCurrentThread);
    return true;
}

dxw_Filter_Type *GetFilterList() {
    return dxwFilters;
}

dxw_Renderer_Type *GetRendererList() {
    return dxwRenderers;
}

DWORD GetDxWndCaps() {
    // default: all ON
    DWORD dxwCaps = 0xFFFFFFFF;
#ifdef DXW_NOTRACES
    dxwCaps &= ~DXWCAPS_CANLOG;
#endif
    return dxwCaps;
}

static BOOL GetMultiTaskEnabling() {
    char inipath[MAX_PATH];
    GetModuleFileName(GetModuleHandle("dxwnd"), inipath, MAX_PATH);
    inipath[strlen(inipath) - strlen("dxwnd.dll")] = 0; // terminate the string just before "dxwnd.dll"
    strcat(inipath, "dxwnd.ini");
    return GetPrivateProfileInt("window", "multiprocesshook", 0, inipath);
}

int SetTarget(DXWNDSTATUS *statusmap, TARGETMAP *targets) {
    int i;
    char path[MAX_PATH + 1];
    WaitForSingleObject(hMutex, INFINITE);
    pStatus->Status = DXW_IDLE;
    pStatus->IsFullScreen = FALSE;
    pStatus->TaskIdx = 0;
    pStatus->hWnd = NULL;
    pStatus->ColorDepth = 0;
    memset((void *) & (pStatus->pfd), 0, sizeof(DDPIXELFORMAT));
    pStatus->Height = pStatus->Width = 0;
    pStatus->DXVersion = 0;
    pStatus->AllowMultiTask = GetMultiTaskEnabling();
    //pStatus->VJoyStatus=VJOYENABLED|CROSSENABLED|INVERTYAXIS;
    if(statusmap) {
        pStatus->VJoyStatus = statusmap->VJoyStatus;
        pStatus->VJoySensivity = statusmap->VJoySensivity;
        pStatus->VJoyDeadZone = statusmap->VJoyDeadZone;
    }
    for(i = 0; targets[i].path[0]; i ++) {
        char *c;
        pMapping[i] = targets[i];
        c = targets[i].path;
        if(*c == '*')
            strcpy(path, targets[i].path);
        else
            GetFullPathName(targets[i].path, MAX_PATH, path, NULL);
        for(c = path; *c; c++) *c = tolower(*c);
        strcpy(pMapping[i].path, path);
    }
    pMapping[i].path[0] = 0;
    ReleaseMutex(hMutex);
    return i;
}

int StartHook(void) {
    hHook = SetWindowsHookEx(WH_CALLWNDPROC, HookProc, hInst, 0);
    HookStatus = DXW_ACTIVE;
    return 0;
}

int EndHook(void) {
    if (hHook) UnhookWindowsHookEx(hHook);
    if (hMouseHook) UnhookWindowsHookEx(hMouseHook);
    if (hKeyboardHook) UnhookWindowsHookEx(hKeyboardHook);
    HookStatus = DXW_IDLE;
    return 0;
}

void GetDllVersion(char *dest) {
    strcpy_s(dest, strlen(VERSION) + 1, VERSION);
}

int GetHookStatus(DXWNDSTATUS *s) {
    DWORD ret;
    ret = WaitForSingleObject(hLockMutex, 0);
    ReleaseMutex(hLockMutex);
    if(ret == WAIT_TIMEOUT) {
        if (s) *s = *pStatus;
        return DXW_RUNNING;
    }
    return HookStatus;
}

DXWNDSTATUS *GetHookInfo() {
    return pStatus;
}

void SetFPS(int fps) {
    pStatus->FPSCount = fps;
}

static DWORD WINAPI StatusLockThread(LPVOID hLockMutex) {
    if(WaitForSingleObject((HANDLE)hLockMutex, 0) == WAIT_TIMEOUT)
        exit(0);
    while(1) Sleep(INFINITE);
    return 0;
}

static DWORD WINAPI StatusMultiLockThread(LPVOID hLockMutex) {
    WaitForSingleObject((HANDLE)hLockMutex, 0);
    while(1) Sleep(INFINITE);
    return 0;
}

LRESULT CALLBACK HookProc(int ncode, WPARAM wparam, LPARAM lparam) {
    char name[MAX_PATH + 1];
    HWND hwnd;
    int i;
    static int DoOnce = FALSE;
    // don't do more than once per process
    if(DoOnce) return CallNextHookEx(hHook, ncode, wparam, lparam);
    // take care here: if anything stops or delays the execution logic, the whole
    // operating system hangs, since it can't activate new windows!
    // could use WM_NCCREATE instead of WM_CREATE. Are there differences?
    hwnd = ((CWPSTRUCT *)lparam)->hwnd;
    if(((CWPSTRUCT *)lparam)->message == WM_CREATE) {
        int iNameLength;
        name[MAX_PATH] = 0; // string terminator
        GetModuleFileName(0, name, MAX_PATH);
        for(i = 0; name[i]; i ++) name[i] = tolower(name[i]);
        iNameLength = strlen(name);
        WaitForSingleObject(hMutex, INFINITE);
        for(i = 0; pMapping[i].path[0] && (i < MAXTARGETS); i++) {
            register BOOL bMatched;
            if (!(pMapping[i].flags3 & HOOKENABLED)) continue;
            if(pMapping[i].path[0] == '*')
                bMatched = !strncmp(&name[iNameLength - strlen(pMapping[i].path) + 1], &pMapping[i].path[1], iNameLength);
            else
                bMatched = !strncmp(name, pMapping[i].path, iNameLength);
            if(bMatched) {
                // V.68 late fix:
                // check for locking thread (and hook) just once per process.
                // This callback is invoked per each process' thread.
                DoOnce = TRUE;
                extern HHOOK hHook;
                if(hHook) UnhookWindowsHookEx(hHook);
                // ??
                //if (hMouseHook) UnhookWindowsHookEx(hMouseHook);
                //if (hKeyboardHook) UnhookWindowsHookEx(hKeyboardHook);
                // V.68: concurrency check. One game at a time, or exiting.
                // no good trying to insert fancy dialog boxes: the window
                // isn't ready yet, and the operation fails.
                // V2.03.07: allow multiple process hooking depending on config
                // v2.05.40: the mutex lock is moved in a separate, independent thread.
                // This because some games can be hooked in a temporary thread that will
                // die after a while, then releasing the mutex signaled status.
                CreateThread(NULL, 0,
                             pStatus->AllowMultiTask ? StatusMultiLockThread : StatusLockThread,
                             (LPVOID)hLockMutex, 0, 0);
                pStatus->Status = DXW_RUNNING;
                pStatus->TaskIdx = i;
                pStatus->OrigIdx = pMapping[i].index;
                pStatus->IsFullScreen = FALSE;
                pStatus->hWnd = hwnd;
                pStatus->dwPid = GetProcessId(GetCurrentProcess());
                pStatus->TimeShift = pMapping[i].InitTS;
                pStatus->CursorX = pStatus->CursorY = (short) - 1;
                pStatus->MessageX = pStatus->MessageY = (short) - 1;
                pStatus->WinProcX = pStatus->WinProcY = (short) - 1;
                pStatus->MsgHookX = pStatus->MsgHookY = (short) - 1;
                pStatus->PlayerStatus = DXW_PLAYER_STOPPED;
                pStatus->TrackNo = 0;
                memset((void *) & (pStatus->pfd), 0, sizeof(DDPIXELFORMAT));
                DxWndStatus = *pStatus;
                HookInit(&pMapping[i], hwnd);
            }
        }
        ReleaseMutex(hMutex);
    }
    return CallNextHookEx(hHook, ncode, wparam, lparam);
}

void UnhookProc() {
    // used to unhook DxWnd from the current process and allow another one (a son) to be managed
    //ReleaseMutex(hMutex);
    ReleaseMutex(hLockMutex);
    // UnmapViewOfFile(pMapping); v2.04.83
    UnmapViewOfFile(pStatus); // v2.04.81
    CloseHandle(hMapping);
}

void InjectHook() {
    char name[MAX_PATH + 1];
    int i;
    GetModuleFileName(0, name, MAX_PATH);
    name[MAX_PATH] = 0; // terminator
    for(char *c = name; *c; c++) *c = tolower(*c);
    for(i = 0; pMapping[i].path[0]; i ++) {
        if(pMapping[i].flags3 & HOOKENABLED) {
            if(!strncmp(name, pMapping[i].path, strlen(name))) {
                if ((pMapping[i].flags2 & STARTDEBUG) ||
                        (pMapping[i].flags7 & INJECTSUSPENDED) ||
                        (pMapping[i].flags12 & INJECTPROXY)
                   ) {
                    HookInit(&pMapping[i], NULL);
                    // beware: logging is possible only AFTER HookInit execution
                    OutTrace("InjectHook: task[%d]=\"%s\" hooked\n", i, pMapping[i].path);
                }
                break;
            }
        }
    }
}

#ifndef DXW_NOTRACES
static char *FlagNames[19][32] = {{
        // Flags1
        //					+					+					+
        "UNNOTIFY",			"SETDPIAWARE",		"CLIPCURSOR",		"NEEDADMINCAPS",
        "HOOKDI",			"MODIFYMOUSE",		"HANDLEEXCEPTIONS", "SAVELOAD",
        "HOOKXINPUT",		"HOOKDI8",			"BLITFROMBACKBUFFER", "SUPPRESSCLIPPING",
        "AUTOREFRESH",		"FIXWINFRAME",		"HIDEHWCURSOR",		"SLOWDOWN",
        "DISABLECLIPPING",	"LOCKWINSTYLE",		"MAPGDITOPRIMARY",	"FIXTEXTOUT",
        "KEEPCURSORWITHIN", "USERGB565",		"SUPPRESSDXERRORS", "PREVENTMAXIMIZE",
        "LOCK24BITDEPTH",	"FIXPARENTWIN",		"SWITCHVIDEOMEMORY", "CLIENTREMAPPING",
        "HANDLEALTF4",		"LOCKWINPOS",		"HOOKCHILDWIN",		"MESSAGEPROC"
    }, {
        // Flags2
        "RECOVERSCREENMODE", "REFRESHONRESIZE",	"BACKBUFATTACH",	"MODALSTYLE",
        "KEEPASPECTRATIO",  "INIT8BPP",			"FORCEWINRESIZE",	"INIT16BPP",
        "KEEPCURSORFIXED",  "DISABLEGAMMARAMP",	"INDEPENDENTREFRESH", "FIXNCHITTEST",
        "LIMITFPS",			"SKIPFPS",			"SHOWFPS",			"HIDEMULTIMONITOR",
        "TIMESTRETCH",		"HOOKOPENGL",		"LOCKEDSIZE",		"SHOWHWCURSOR",
        "GDISTRETCHED",		"SHOWFPSOVERLAY",	"FAKEVERSION",		"PALDIBEMULATION",
        "NOPALETTEUPDATE",	"SUPPRESSIME",		"NOBANNER",			"WINDOWIZE",
        "LIMITRESOURCES",	"STARTDEBUG",		"SETCOMPATIBILITY", "WIREFRAME",
    }, {
        // Flags3
        "FORCEHOOKOPENGL",	"REFRESHONREALIZE",	"HOOKDLLS",			"SUPPRESSD3DEXT",
        "HOOKENABLED",		"FIXD3DFRAME",		"FORCE16BPP",		"BLACKWHITE",
        "SKIPIATHINT",		"SINGLEPROCAFFINITY", "EMULATEREGISTRY", "CDROMDRIVETYPE",
        "NOWINDOWMOVE",		"FORCECLIPPER",		"LOCKSYSCOLORS",	"GDIEMULATEDC",
        "FULLSCREENONLY",	"FONTBYPASS",		"MINIMALCAPS",		"DEFAULTMESSAGES",
        "BUFFEREDIOFIX",	"FILTERMESSAGES",	"PEEKALLMESSAGES",	"SETZBUFFER16BIT",
        "SETZBUFFER24BIT",	"FORCESHEL",		"SKIPFREELIBRARY",	"COLORFIX",
        "FULLPAINTRECT",	"FIXALTEREDPATH",	"LOCKFPSCORNER",	"NOPIXELFORMAT",
    }, {
        // Flags4
        "NOALPHACHANNEL",	"SUPPRESSCHILD",	"FIXREFCOUNTER",	"SHOWTIMESTRETCH",
        "ZBUFFERCLEAN",		"ZBUFFER0CLEAN",	"FORCECLIPCHILDREN", "DISABLEFOGGING",
        "NOPOWER2FIX",		"NOPERFCOUNTER",	"PREVENTMINIMIZE",	"INTERCEPTRDTSC",
        "LIMITSCREENRES",	"NOFILLRECT",		"HOOKGLIDE",		"HIDEDESKTOP",
        "STRETCHTIMERS",	"NOFLIPEMULATION",	"NOTEXTURES",		"RETURNNULLREF",
        "FINETIMING",		"NATIVERES",		"SUPPORTSVGA",		"SUPPORTHDTV",
        "RELEASEMOUSE",		"ENABLETIMEFREEZE", "HOTPATCH",			"ENABLEHOTKEYS",
        "IGNOREDEBOUTPUT",	"NOD3DRESET",		"OVERRIDEREGISTRY", "HIDECDROMEMPTY",
    }, {
        // Flags5
        "DIABLOTWEAK",		"CLEARTARGET",		"NOWINPOSCHANGES",	"MAXCLIPPER",
        "LIMITBEGINSCENE",	"USELASTCORE",		"SWALLOWMOUSEMOVE",	"AEROBOOST",
        "QUARTERBLT",		"NOIMAGEHLP",		"BILINEARFILTER",	"REPLACEPRIVOPS",
        "REMAPMCI",			"TEXTUREHIGHLIGHT",	"TEXTUREDUMP",		"TEXTUREHACK",
        "TEXTURETRANSP",	"NORMALIZEPERFCOUNT", "DISABLEMMX",		"NOACCESSIBILITY",
        "INJECTSON",		"DEBUGSON",			"DISABLEALTTAB",	"HOOKBINKW32",
        "GLOBALFOCUSON",	"GLOBALFOCUSOFF",	"MESSAGEPUMP",		"TEXTUREFORMAT",
        "PUSHACTIVEMOVIE",	"LOCKRESERVEDPALETTE", "UNLOCKZORDER",	"EASPORTSHACK",
    }, {
        // Flags6
        "FORCESWAPEFFECT",	"LEGACYALLOC",		"NODESTROYWINDOW",	"NOMOVIES",
        "SUPPRESSRELEASE",	"FIXMOVIESCOLOR",	"WOW64REGISTRY",	"DISABLEMAXWINMODE",
        "FIXPITCH",			"POWER2WIDTH",		"HIDETASKBAR",		"ACTIVATEAPP",
        "NOSYSMEMPRIMARY",	"NOSYSMEMBACKBUF",	"CONFIRMONCLOSE",	"TERMINATEONCLOSE",
        "FLIPEMULATION",	"SETZBUFFERBITDEPTHS", "SHAREDDC",		"WOW32REGISTRY",
        "STRETCHMOVIES",	"BYPASSMCI",		"FIXPIXELZOOM",		"SCALERELMOUSE",
        "CREATEDESKTOP",	"FRONTEND",			"SYNCPALETTE",		"VIRTUALJOYSTICK",
        "UNACQUIRE",		"HOOKGOGLIBS",		"BYPASSGOGLIBS",	"EMULATERELMOUSE",
    }, {
        // Flags7
        "LIMITDDRAW",		"NODISABLEALTTAB",	"FIXCLIPPERAREA",	"HOOKDIRECTSOUND",
        "HOOKSMACKW32",		"BLOCKPRIORITYCLASS", "CPUSLOWDOWN",		"CPUMAXUSAGE",
        "NOWINERRORS",		"SUPPRESSOVERLAY",	"INIT24BPP",		"INIT32BPP",
        "FIXGLOBALUNLOCK",	"SHOWHINTS",		"SKIPDEVTYPEHID",	"INJECTSUSPENDED",
        "SUPPRESSDIERRORS", "HOOKNORUN",		"FIXBINDTEXTURE",	"ENUM16BITMODES",
        "SHAREDKEYBOARD",	"HOOKDOUPDATE",		"HOOKGLUT32",		"INITIALRES",
        "MAXIMUMRES",		"LOCKCOLORDEPTH",	"FIXSMACKLOOP",		"FIXFREELIBRARY",
        "ANCHORED",			"CLEARTEXTUREFOURCC", "NODDEXCLUSIVEMODE", "COPYNOSHIMS",
    }, {
        // Flags8
        "FORCEWAIT",		"FORCENOWAIT",		"FORCEVSYNC",		"FORCENOVSYNC",
        "VSYNCSCANLINES",	"TRIMTEXTUREFORMATS", "NOHALDEVICE",		"CLIPLOCK",
        "PRETENDVISIBLE",	"RAWFORMAT",		"WININSULATION",	"FIXMOUSEHOOK",
        "DDSFORMAT",		"HOOKWING32",		"FIXAUDIOPCM",		"D3D8BACK16",
        "VIRTUALCDAUDIO",	"DYNAMICZCLEAN",	"FORCETRACKREPEAT",	"IGNOREMCIDEVID",
        "LOADGAMMARAMP",	"QUALITYFONTS",		"ALLOWSYSMEMON3DDEV", "CLIPMENU",
        "BACKGROUNDPRIORITY", "OFFSCREENZBUFFER", "VIRTUALHEAP",		"ZBUFFERHARDCLEAN",
        "LOADLIBRARYERR",	"SHAREDDCHYBRID",	"FIXADJUSTWINRECT", "HOOKDLGWIN",
    }, {
        // Flags9
        "FIXTHINFRAME",		"NOMOUSEEVENTS",	"IATWORDALIGNED",	"IATBYTEALIGNED",
        "WRITEON000B0000",	"NODIALOGS",		"SAFEPALETTEUSAGE", "LOCKFULLSCREENCOOP",
        "NOBAADFOOD",		"HOTREGISTRY",		"NOIATSCAN",		"HOOKSDLLIB",
        "SDLEMULATION",		"HIDEJOYSTICKS",	"HOOKSDL2LIB",		"SDLFORCESTRETCH",
        "MOUSEMOVEBYEVENT", "D3DRESOLUTIONHACK", "FIXAILSOUNDLOCKS", "LOCKTOPZORDER",
        "EMULATEMAXIMIZE",	"MAKEWINVISIBLE",	"FIXEMPIREOFS",		"SCALEGLBITMAPS",
        "HOOKWGLCONTEXT",	"NOTASKBAROVERLAP", "CACHED3DSESSION",	"SLOWSCROLLING",
        "KILLBLACKWIN",		"ZERODISPLAYCOUNTER", "SOUNDMUTE",		"LOCKVOLUME",
    }, {
        // Flags10
        "FORCEHWVERTEXPROC", "FORCESWVERTEXPROC", "FORCEMXVERTEXPROC", "PRECISETIMING",
        "REPLACEDIALOGS",	"FAKEHDDRIVE",		"FAKECDDRIVE",		"LIGHTGAMMARAMP",
        "FORCED3DGAMMARAMP", "HANDLEFOURCC",		"SUSPENDTIMESTRETCH", "SLOWWINPOLLING",
        "NOOLEINITIALIZE",	"HWFOURCC",			"SWFOURCC",			"LIMITDIBOPERATIONS",
        "FIXMOUSERAWINPUT", "SETCDVOLUME",		"CUSTOMRES",		"CHAOSOVERLORDSFIX",
        "FIXFOLDERPATHS",	"NOCOMPLEXMIPMAPS", "CDROMPRESENT",		"SUPPRESSFOURCCBLT",
        "INVALIDATECLIENT", "MOUSESHIELD",		"CREATEDCHOOK",		"NOZBUFATTACH",
        "SAFEPRIMLOCK",		"PEFILEHOOK",		"HIDEWINDOWCHANGES", "STRETCHDIALOGS",
    }, {
        // Flags11
        "EXTENDSDLHOOK",	"D3D8MAXWINMODE",	"VIRTUALPROCHEAP",	"MUTEX4CRITSECTION",
        "DELAYCRITSECTION",	"HACKMCIFRAMES",	"UNNOTIFYINACTIVE",	"REMAPNUMKEYPAD",
        "SETUSKEYDESCR",	"HOOKEARSOUND",		"CDPAUSECAPABILITY", "ADAPTIVERATIO",
        "REMAPSYSFOLDERS",	"SCALEMAINVIEWPORT", "FORCESHAL",		"FORCESNULL",
        "USESHORTPATH",		"INVERTMOUSEXAXIS",	"INVERTMOUSEYAXIS",	"SMACKBUFFERNODEPTH",
        "LOCKSYSSETTINGS",	"INVALIDATEFULLRECT", "FIXMESSAGEHOOK",	"NODISABLEPRINT",
        "SHRINKFONTWIDTH",	"SAFEMIDIOUT",		"MERGEMULTIPLECD",	"SAFEALLOCS",
        "FIXASYNCKEYSTATE",	"TRANSFORMANDLIGHT", "CUSTOMLOCALE",		"FIXDEFAULTMCIID",
    }, {
        // Flags12
        "LOCKCDTRAY",		"LOCKGLVIEWPORT",	"NOTNLDEVICE",		"CLASSLOCALE",
        "PROJECTBUFFER",	"COMMITPAGE",		"SETCMDLINE",		"FORCERELAXIS",
        "FORCEABSAXIS",		"LIMITFLIPONLY",	"DIRECTXREPLACE",	"W98OPAQUEFONT",
        "SUPPRESSGLIDE",	"NOSETPIXELFORMAT",	"GLEXTENSIONSLIE",	"FIXMOUSELPARAM",
        "FAKEGLOBALATOM",	"KILLVSYNC",		"STRETCHPERFREQUENCY", "INJECTPROXY",
        "DSINITVOLUME",		"SCALECBTHOOK",		"REVERTDIBPALETTE",	"FIXDCALREADYCREATED",
        "SUPPRESSMENUS",	"ADAPTMOUSESPEED",	"KILLDEADLOCKS",	"BLUREFFECT",
        "TEXTUREPALETTE",	"DISABLEDWM",		"SUPPRESSCDAUDIO",	"RESERVEMCIDEVICE",
    }, {
        // Flags13
        "GLFIXCLAMP",		"",					"",					"",
        "",					"",					"",					"",
        "",					"",					"",					"",
        "",					"",					"",					"",
        "",					"",					"",					"",
        "",					"",					"",					"",
        "",					"",					"",					"",
        "",					"",					"",					"",
    }, {
        // Flags14
        "",					"",					"",					"",
        "",					"",					"",					"",
        "",					"",					"",					"",
        "",					"",					"",					"",
        "",					"",					"",					"",
        "",					"",					"",					"",
        "",					"",					"",					"",
        "",					"",					"",					"",
    }, {
        // Flags15
        "",					"",					"",					"",
        "",					"",					"",					"",
        "",					"",					"",					"",
        "",					"",					"",					"",
        "",					"",					"",					"",
        "",					"",					"",					"",
        "",					"",					"",					"",
        "",					"",					"",					"",
    }, {
        // Flags16
        "",					"",					"",					"",
        "",					"",					"",					"",
        "",					"",					"",					"",
        "",					"",					"",					"",
        "",					"",					"",					"",
        "",					"",					"",					"",
        "",					"",					"",					"",
        "",					"",					"",					"",
    }, {
        // TFlags
        "OUTTRACE",			"OUTDDRAWTRACE",	"OUTWINMESSAGES",	"OUTCURSORTRACE",
        "OUTSEPARATED",		"OUTCIRCULAR",		"ASSERTDIALOG",		"OUTIMPORTTABLE",
        "OUTDEBUG",			"OUTREGISTRY",		"TRACEHOOKS",		"OUTD3DTRACE",
        "OUTDXWINTRACE",	"OUTWINGTRACE",		"OUTOGLTRACE",		"OUTHEXTRACE",
        "OUTSDLTRACE",		"OUTTIMETRACE",		"OUTSOUNDTRACE",	"OUTINPUTS",
        "OUTSYSLIBS",		"OUTLOCALE",		"OUTFPS",			"",
        "",					"",					"",					"ADDRELATIVETIME",
        "ADDTHREADID",		"ADDTIMESTAMP",		"OUTDEBUGSTRING",	"ERASELOGFILE",
    }, {
        // DFlags
        "CPUIDBIT1",		"CPUIDBIT2",		"CPUIDBIT3",		"CPUDISABLECPUID",
        "CPUDISABLEMMX",	"CPUDISABLESSE",	"CPUDISABLESSE2",	"CAPTURESCREENS",
        "DUMPDIBSECTION",	"DUMPDEVCONTEXT",	"DUMPCPUID",		"MARKBLIT",
        "MARKLOCK",			"MARKWING32",		"MARKGDI32",		"DOFASTBLT",
        "CENTERTOWIN",		"DUMPSURFACES",		"NODDRAWBLT",		"NODDRAWFLIP",
        "NOGDIBLT",			"STRESSRESOURCES",	"CAPMASK",			"FULLRECTBLT",
        "ZBUFFERALWAYS",	"HOTPATCHALWAYS",	"FREEZEINJECTEDSON", "STARTWITHTOGGLE",
        "CAPTURESCREENS",	"PATCHEXECUTEBUFFER", "FORCEZBUFFERON",	"FORCEZBUFFEROFF",
    }, {
        // Dflags2
        "NOWINDOWHOOKS",	"EXPERIMENTAL",		"FIXRANDOMPALETTE",	"DUMPBITMAPS",
        "DUMPBLITSRC",		"",					"",					"",
        "",					"",					"",					"",
        "",					"",					"",					"",
        "",					"",					"",					"",
        "",					"",					"",					"",
        "",					"",					"",					"",
        "",					"",					"",					"",
    }
};

LPCSTR GetFlagCaption(int flag, int bit) {
    //if((flag<0) || (flag>(9*32))) return "";
    //return FlagNames[flag >> 5][flag & 0x1F];
    if((flag < 0) || (flag > 18)) return "";
    if((bit < 0) || (bit > 31)) return "";
    return FlagNames[flag][bit];
}
#else
LPCSTR GetFlagCaption(int flag, int bit) {
    return "";
}
#endif // DXW_NOTRACES