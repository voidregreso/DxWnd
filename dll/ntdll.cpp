#define _WIN32_WINNT 0x0600
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS

#include <stdio.h>
#include <stdlib.h>
#include "dxwnd.h"
#include "dxwcore.hpp"
#include "dxhook.h"
typedef UINT (WINAPI *GetProfileIntA_Type)(LPCTSTR, LPCTSTR, INT);

typedef LONG (WINAPI *RtlMultiByteToUnicodeN_Type)(PWCH, ULONG, PULONG, const CHAR *, ULONG);
RtlMultiByteToUnicodeN_Type pRtlMultiByteToUnicodeN;
LONG WINAPI extRtlMultiByteToUnicodeN(PWCH, ULONG, PULONG, const CHAR *, ULONG);
typedef LONG (WINAPI *RtlMultiByteToUnicodeSize_Type)(PULONG, const CHAR *, ULONG);
RtlMultiByteToUnicodeSize_Type pRtlMultiByteToUnicodeSize;
LONG WINAPI extRtlMultiByteToUnicodeSize(PULONG, const CHAR *, ULONG);
typedef LONG (WINAPI *RtlUnicodeToMultiByteSize_Type)(PULONG, PWCH, ULONG);
RtlUnicodeToMultiByteSize_Type pRtlUnicodeToMultiByteSize;
LONG WINAPI extRtlUnicodeToMultiByteSize(PULONG, PWCH, ULONG);
typedef LONG (WINAPI *RtlUnicodeToOemN_Type)(PCHAR, ULONG, PULONG, PCWCH, ULONG);
RtlUnicodeToOemN_Type pRtlUnicodeToOemN;
LONG WINAPI extRtlUnicodeToOemN(PCHAR, ULONG, PULONG, PCWCH, ULONG);
typedef LONG (WINAPI *RtlOemToUnicodeN_Type)(PWCH, ULONG, PULONG, PCCH, ULONG);
RtlOemToUnicodeN_Type pRtlOemToUnicodeN;
LONG WINAPI extRtlOemToUnicodeN(PWCH, ULONG, PULONG, PCCH, ULONG);

static HookEntryEx_Type NlsHooks[] = {
    {HOOK_HOT_REQUIRED, 0, "RtlMultiByteToUnicodeN", (FARPROC)NULL, (FARPROC *) &pRtlMultiByteToUnicodeN, (FARPROC)extRtlMultiByteToUnicodeN},
    {HOOK_HOT_REQUIRED, 0, "RtlMultiByteToUnicodeSize", (FARPROC)NULL, (FARPROC *) &pRtlMultiByteToUnicodeSize, (FARPROC)extRtlMultiByteToUnicodeSize},
    {HOOK_HOT_REQUIRED, 0, "RtlUnicodeToMultiByteSize", (FARPROC)NULL, (FARPROC *) &pRtlUnicodeToMultiByteSize, (FARPROC)extRtlUnicodeToMultiByteSize},
    {HOOK_HOT_REQUIRED, 0, "RtlUnicodeToOemN", (FARPROC)NULL, (FARPROC *) &pRtlUnicodeToOemN, (FARPROC)extRtlUnicodeToOemN},
    {HOOK_HOT_REQUIRED, 0, "RtlOemToUnicodeN", (FARPROC)NULL, (FARPROC *) &pRtlOemToUnicodeN, (FARPROC)extRtlOemToUnicodeN},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static char *libname = "ntdll.dll";
//static DWORD dwDebug = FALSE;
static DWORD dwDebug2 = FALSE;

void HookNtDll(HMODULE hModule) {
    HookLibraryEx(hModule, NlsHooks, libname);
    if(!(	pRtlMultiByteToUnicodeN &&
            pRtlMultiByteToUnicodeSize &&
            pRtlUnicodeToMultiByteSize &&
            pRtlUnicodeToOemN &&
            pRtlOemToUnicodeN
        )) MessageBox(NULL, "ntdll hook failed", "DxWnd", 0);
}

LONG WINAPI extRtlMultiByteToUnicodeN(PWCH UnicodeString, ULONG MaxBytes, PULONG pBytes, const CHAR *MBString, ULONG MBBytes) {
    //if(dwDebug) MessageBox(0,"RtlMultiByteToUnicodeN", "!!!", 0);
    _if(dwDebug2) OutTrace("RtlMultiByteToUnicodeN\n");
    return (*pRtlMultiByteToUnicodeN)(UnicodeString, MaxBytes, pBytes, MBString, MBBytes);
}

LONG WINAPI extRtlMultiByteToUnicodeSize(PULONG BytesInUnicodeString, const CHAR *MultiByteString, ULONG BytesInMultiByteString) {
    //if(dwDebug) MessageBox(0,"RtlMultiByteToUnicodeSize", "!!!", 0);
    _if(dwDebug2) OutTrace("RtlMultiByteToUnicodeSize\n");
    return (*pRtlMultiByteToUnicodeSize)(BytesInUnicodeString, MultiByteString, BytesInMultiByteString);
}

LONG WINAPI extRtlUnicodeToMultiByteSize(PULONG BytesInMultiByteString, PWCH UnicodeString, ULONG BytesInUnicodeString) {
    //if(dwDebug) MessageBox(0,"RtlUnicodeToMultiByteSize", "!!!", 0);
    _if(dwDebug2) OutTrace("RtlUnicodeToMultiByteSize\n");
    return (*pRtlUnicodeToMultiByteSize)(BytesInMultiByteString, UnicodeString, BytesInUnicodeString);
}

LONG WINAPI extRtlUnicodeToOemN(PCHAR OemString, ULONG MaxBytesInOemString, PULONG BytesInOemString, PCWCH UnicodeString, ULONG BytesInUnicodeString) {
    //if(dwDebug) MessageBox(0,"RtlUnicodeToOemN", "!!!", 0);
    _if(dwDebug2) OutTrace("RtlUnicodeToOemN\n");
    return (*pRtlUnicodeToOemN)(OemString, MaxBytesInOemString, BytesInOemString, UnicodeString, BytesInUnicodeString);
}

LONG WINAPI extRtlOemToUnicodeN(PWCH UnicodeString, ULONG MaxBytesInUnicodeString, PULONG BytesInUnicodeString, PCCH OemString, ULONG BytesInOemString) {
    //if(dwDebug) MessageBox(0,"RtlOemToUnicodeN", "!!!", 0);
    _if(dwDebug2) OutTrace("RtlOemToUnicodeN\n");
    return (*pRtlOemToUnicodeN)(UnicodeString, MaxBytesInUnicodeString, BytesInUnicodeString, OemString, BytesInOemString);
}