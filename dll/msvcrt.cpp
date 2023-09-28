#define  _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include "dxwnd.h"
#include "dxwcore.hpp"
#include "syslibs.h"
#include "dxhook.h"

extern LPCSTR dxwTranslatePathA(LPCSTR, DWORD *);

#define ASSERTDIALOGS

typedef LPVOID (CDECL *malloc_Type)(size_t);
typedef LPVOID (CDECL *realloc_Type)(LPVOID, size_t);
typedef VOID (CDECL *free_Type)(LPVOID);
typedef FILE *(CDECL *fopen_Type)(const char *, const char *);

malloc_Type pmalloc;
realloc_Type prealloc;
free_Type pfree;
fopen_Type pfopen;

LPVOID CDECL extmalloc(size_t);
LPVOID CDECL extrealloc(LPVOID, size_t);
VOID CDECL extfree(LPVOID);
FILE *CDECL extfopen(const char *, const char *);

static HookEntryEx_Type HooksAlloc[] = {
    {HOOK_IAT_CANDIDATE, 0x0000, "malloc", (FARPROC)NULL, (FARPROC *) &pmalloc, (FARPROC)extmalloc},
    {HOOK_IAT_CANDIDATE, 0x0000, "realloc", (FARPROC)NULL, (FARPROC *) &prealloc, (FARPROC)extrealloc},
    {HOOK_IAT_CANDIDATE, 0x0000, "free", (FARPROC)NULL, (FARPROC *) &pfree, (FARPROC)extfree},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static HookEntryEx_Type HooksFiles[] = {
    {HOOK_IAT_CANDIDATE, 0x0000, "fopen", (FARPROC)NULL, (FARPROC *) &pfopen, (FARPROC)extfopen},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static char *libname = "msvcrt.dll";

void HookMSVCRT(HMODULE hModule) {
    if(dxw.dwFlags11 & SAFEALLOCS) HookLibraryEx(hModule, HooksAlloc, libname);
    if(dxw.dwFlags10 & (FAKECDDRIVE | FAKEHDDRIVE)) HookLibraryEx(hModule, HooksFiles, libname);
    return;
}

FARPROC Remap_MSVCRT_ProcAddress(LPCSTR proc, HMODULE hModule) {
    FARPROC addr;
    if(dxw.dwFlags11 & SAFEALLOCS) if(addr = RemapLibraryEx(proc, hModule, HooksAlloc)) return addr;
    if(dxw.dwFlags10 & (FAKECDDRIVE | FAKEHDDRIVE)) if(addr = RemapLibraryEx(proc, hModule, HooksFiles)) return addr;
    return NULL;
}

LPVOID CDECL extmalloc(size_t size) {
    LPVOID ret;
    OutTraceSYS("malloc: size=%d\n", size);
    ret = (*pmalloc)(size + 2); // add an extra word
    memcpy(ret, "AA", 2);
    return (LPBYTE)ret + 2;
}

LPVOID CDECL extrealloc(LPVOID addr, size_t size) {
    LPVOID ret;
    OutTraceSYS("realloc: addr=%#x size=%d\n", addr, size);
    if(!addr) {
        // caveat: realloc of NULL address behaves like malloc
        ret = (*prealloc)(NULL, size + 2); // add an extra word
        memcpy(ret, "AA", 2);
        return (LPBYTE)ret + 2;
    }
    if(memcmp((LPBYTE)addr - 2, "AA", 2)) {
        OutTraceE("realloc: corrupted buffer @%#x - skip\n", addr);
#ifdef ASSERTDIALOGS
        MessageBox(NULL, "Warning: realloc on corrupted buffer", "DxWnd", 0);
#endif // ASSERTDIALOGS
        // how to find the original buffer length?
        //ret = (*pmalloc)(size + 2);
        //memcpy(ret, "AA", 2);
        ////memcpy(ret + 2, addr, ???);
        ret = (*prealloc)((LPBYTE)addr - 2, size);
        memcpy(ret, "AA", 2);
        return (LPBYTE)ret + 2;
    }
    ret = (*prealloc)((LPBYTE)addr - 2, size + 2);
    return (LPBYTE)ret + 2;
}

VOID CDECL extfree(LPVOID addr) {
    OutTraceSYS("free: addr=%#x\n", addr);
    if(memcmp((LPBYTE)addr - 2, "AA", 2)) {
        if(!memcmp((LPBYTE)addr - 2, "FF", 2)) {
            OutTraceE("free: freed twice buffer @%#x - skip\n", addr);
#ifdef ASSERTDIALOGS
            MessageBox(NULL, "Warning: double freed buffer", "DxWnd", 0);
#endif // ASSERTDIALOGS
        } else {
            OutTraceE("free: corrupted buffer @%#x - skip\n", addr);
#ifdef ASSERTDIALOGS
            MessageBox(NULL, "Warning: free on corrupted buffer", "DxWnd", 0);
#endif // ASSERTDIALOGS
        }
        return;
    }
    memcpy((LPBYTE)addr - 2, "FF", 2);
    (*pfree)((LPBYTE)addr - 2);
}

FILE *CDECL extfopen(const char *fname, const char *mode) {
    ApiName("fopen");
    OutTraceSYS("%s: path=\"%s\" mode=\"%s\"\n", ApiRef, fname, mode);
    if(dxw.dwFlags10 & (FAKEHDDRIVE | FAKECDDRIVE)) {
        DWORD mapping;
        fname = dxwTranslatePathA(fname, &mapping);
        // if mapped on virtual CD and write access required, you should fake a no access error code
        if(mapping == DXW_FAKE_CD) {
            if(strpbrk(mode, "wa+")) { // write, append or overwrite are all failing on a CD drive
                OutTraceDW("%s: simulate ERROR_ACCESS_DENIED on CD\n", ApiRef);
                // should set lasterror here?
                SetLastError(ERROR_ACCESS_DENIED);// assuming the file was there, ERROR_FILE_NOT_FOUND otherwise ?
                return NULL;
            }
        }
    }
    return (*pfopen)(fname, mode);
}