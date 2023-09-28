#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ctype.h>
#include <stdio.h>

extern void trace(const char *, ...);

#if 1
void SetHook(void *target, void *hookproc, void **hookedproc, char *hookname) {
    void *tmp;
    DWORD dwTmp, oldprot;
    static DWORD MinHook = 0xFFFFFFFF;
    static DWORD MaxHook = 0;
    // keep track of hooked call range to avoid re-hooking of hooked addresses !!!
    dwTmp = *(DWORD *)target;
    if(hookproc == *hookedproc) {
        char sMsg[161];
        sprintf(sMsg, "Circular reference for method \"%s\" addr=%x", hookname, hookproc);
        MessageBox(0, sMsg, "hook", MB_OK | MB_ICONEXCLAMATION);
        return;
    }
    //if(*(DWORD *)hookedproc && **(DWORD **)hookedproc && (**(DWORD **)hookedproc != *(DWORD *)target)){
    //	char sMsg[161];
    //	sprintf(sMsg, "Alterate reference for method \"%s\" addr=%x->%x", hookname, **(DWORD **)hookedproc, *(DWORD *)target);
    //	MessageBox(0, sMsg, "hook", MB_OK | MB_ICONEXCLAMATION);
    //}
    if(*(DWORD *)target == *(DWORD *)hookproc) {
        trace("!!! *target == *hookproc\n");
        return; // already hooked
    }
    if(hookproc == *hookedproc) {
        trace("!!! hookproc == *hookedproc\n");
        return; // already hooked
    }
    if((dwTmp <= MaxHook) && (dwTmp >= MinHook)) {
        trace("!!! dwTmp in MinHook-MaxHook range\n");
        return; // already hooked
    }
    if(*(DWORD *)target == 0) { // error condition
        char sMsg[161];
        sprintf(sMsg, "NULL reference for method \"%s\"", hookname);
        MessageBox(0, sMsg, "hook", MB_OK | MB_ICONEXCLAMATION);
        return;
    }
    if(!VirtualProtect(target, 4, PAGE_READWRITE, &oldprot)) return; // error condition
    *(DWORD *)target = (DWORD)hookproc;
    if(!VirtualProtect(target, 4, oldprot, &oldprot)) return; // error condition
    if(!FlushInstructionCache(GetCurrentProcess(), target, 4)) return; // error condition
    tmp = (void *)dwTmp;
    if(!*hookedproc) *hookedproc = tmp; // changed from DxWnd!!!
}
#else
void SetHook(void *target, void *hookproc, void **hookedproc, char *hookname) {
    void *tmp;
    DWORD dwTmp, oldprot;
    static DWORD MinHook = 0xFFFFFFFF;
    static DWORD MaxHook = 0;
    // keep track of hooked call range to avoid re-hooking of hooked addresses !!!
    if ((DWORD)hookproc < MinHook) MinHook = (DWORD)hookproc;
    if ((DWORD)hookproc > MaxHook) MaxHook = (DWORD)hookproc;
    dwTmp = *(DWORD *)target;
    if(dwTmp == (DWORD)hookproc) return; // already hooked
    if(hookproc == *hookedproc) return; // already hooked
    if((dwTmp <= MaxHook) && (dwTmp >= MinHook)) return; // already hooked
    if(dwTmp == 0) return; // error condition
    if(!VirtualProtect(target, 4, PAGE_READWRITE, &oldprot)) return; // error condition
    *(DWORD *)target = (DWORD)hookproc;
    if(!VirtualProtect(target, 4, oldprot, &oldprot)) return; // error condition
    if(!FlushInstructionCache(GetCurrentProcess(), target, 4)) return; // error condition
    tmp = (void *)dwTmp;
    if(!*hookedproc) *hookedproc = tmp; // changed from DxWnd!!!
}
#endif


