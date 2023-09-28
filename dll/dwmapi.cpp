#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include "dxwnd.h"
#include "dxwcore.hpp"
//#include "syslibs.h"
#include "dxhook.h"
#include "dxhelper.h"
#include "dwmapi.h"

//#define TRACEALL

typedef HRESULT (WINAPI *DwmSetWindowAttribute_Type)(HWND, DWORD, LPCVOID, DWORD);
typedef HRESULT (WINAPI *DwmGetWindowAttribute_Type)(HWND, DWORD, PVOID, DWORD);

DwmSetWindowAttribute_Type pDwmSetWindowAttribute;
DwmGetWindowAttribute_Type pDwmGetWindowAttribute;

HRESULT WINAPI extDwmSetWindowAttribute(HWND, DWORD, LPCVOID, DWORD);
HRESULT WINAPI extDwmGetWindowAttribute(HWND, DWORD, PVOID, DWORD);

static HookEntryEx_Type Hooks[] = {
    // v2.04.68.fx1: fixed prototypes of AVIFileOpenA and AVIFileOpenW
    {HOOK_IAT_CANDIDATE, 0, "DwmSetWindowAttribute", NULL, (FARPROC *) &pDwmSetWindowAttribute, (FARPROC)extDwmSetWindowAttribute},
    {HOOK_IAT_CANDIDATE, 0, "DwmGetWindowAttribute", NULL, (FARPROC *) &pDwmGetWindowAttribute, (FARPROC)extDwmGetWindowAttribute},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

void HookDwmApi(HMODULE module) {
    HookLibraryEx(module, Hooks, "dwmapi.dll");
}

FARPROC Remap_DwmApi_ProcAddress(LPCSTR proc, HMODULE hModule) {
    FARPROC addr;
    if (addr = RemapLibraryEx(proc, hModule, Hooks)) return addr;
    return NULL;
}

HRESULT WINAPI extDwmSetWindowAttribute(HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute) {
    ApiName("DwmSetWindowAttribute");
    HRESULT res;
    OutTraceDW("%s: hwnd=%#x att=%#x size=%d\n", ApiRef, hwnd, dwAttribute, cbAttribute);
    res = (*pDwmSetWindowAttribute)(hwnd, dwAttribute, pvAttribute, cbAttribute);
    _if(res) OutTraceE("%s: ERROR res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    return res;
}

HRESULT WINAPI extDwmGetWindowAttribute(HWND hwnd, DWORD dwAttribute, PVOID pvAttribute, DWORD cbAttribute) {
    ApiName("DwmGetWindowAttribute");
    HRESULT res;
    OutTraceDW("%s: hwnd=%#x att=%#x size=%d\n", ApiRef, hwnd, dwAttribute, cbAttribute);
    res = (*pDwmGetWindowAttribute)(hwnd, dwAttribute, pvAttribute, cbAttribute);
    _if(res) OutTraceE("%s: ERROR res=%#x(%s)\n", ApiRef, res, ExplainDDError(res));
    return res;
}
