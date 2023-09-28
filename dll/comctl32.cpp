#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include "dxwnd.h"
#include "dxwcore.hpp"
#include "syslibs.h"
#include "dxhook.h"
#include "dxhelper.h"

typedef BOOL (WINAPI *InitializeFlatSB_Type)(HWND);
InitializeFlatSB_Type pInitializeFlatSB = NULL;
BOOL WINAPI extInitializeFlatSB(HWND);
typedef BOOL (WINAPI *UninitializeFlatSB_Type)(HWND);
InitializeFlatSB_Type pUninitializeFlatSB = NULL;
BOOL WINAPI extUninitializeFlatSB(HWND);

static HookEntryEx_Type Hooks[] = {
    {HOOK_IAT_CANDIDATE, 0, "InitializeFlatSB", NULL, (FARPROC *) &pInitializeFlatSB, (FARPROC)extInitializeFlatSB},
    {HOOK_IAT_CANDIDATE, 0, "UninitializeFlatSB", NULL, (FARPROC *) &pUninitializeFlatSB, (FARPROC)extUninitializeFlatSB},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

void HookComCtl32(HMODULE module) {
    HookLibraryEx(module, Hooks, "comctl32.dll");
}

FARPROC Remap_ComCtl32_ProcAddress(LPCSTR proc, HMODULE hModule) {
    FARPROC addr;
    if (addr = RemapLibraryEx(proc, hModule, Hooks)) return addr;
    return NULL;
}

BOOL WINAPI extInitializeFlatSB(HWND hwnd) {
    BOOL ret;
    ApiName("InitializeFlatSB");
    OutTraceSYS("%s: hwnd=%#x\n", api, hwnd);
    if(dxw.IsFullScreen() && dxw.IsRealDesktop(hwnd)) {
        OutTraceDW("%s: hwnd=%#x->%#x\n", api, hwnd, dxw.GethWnd());
        hwnd = dxw.GethWnd();
    }
    ret = (*pInitializeFlatSB)(hwnd);
#ifndef DXW_NOTRACES
    if(!ret)OutTraceSYS("InitializeFlatSB: ret=%#x\n", ret);
#endif // DXW_NOTRACES
    return ret;
}

BOOL WINAPI extUninitializeFlatSB(HWND hwnd) {
    BOOL ret;
    ApiName("UninitializeFlatSB");
    OutTraceSYS("%s: hwnd=%#x\n", api, hwnd);
    if(dxw.IsFullScreen() && dxw.IsRealDesktop(hwnd)) {
        OutTraceDW("%s: hwnd=%#x->%#x\n", api, hwnd, dxw.GethWnd());
        hwnd = dxw.GethWnd();
    }
    ret = (*pUninitializeFlatSB)(hwnd);
#ifndef DXW_NOTRACES
    if(!ret)OutTraceSYS("%s: ret=%#x\n", api, ret);
#endif // DXW_NOTRACES
    return ret;
}
