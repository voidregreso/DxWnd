#define  _CRT_SECURE_NO_WARNINGS
#include "dxwnd.h"
#include "dxwcore.hpp"
#include "dxhook.h"
#include "dxhelper.h"
#include "stdio.h"

LPVOID gFakeLocaleData = 0;
extern DWORD dxwGetMajor();
extern DWORD dxwGetMinor();

typedef BOOL (WINAPI *GetFileVersionInfoA_Type)(LPCSTR, DWORD, DWORD, LPVOID);
GetFileVersionInfoA_Type pGetFileVersionInfoA;
BOOL WINAPI extGetFileVersionInfoA(LPCSTR, DWORD, DWORD, LPVOID);
typedef BOOL (WINAPI *VerQueryValueA_Type)(LPCVOID, LPCSTR, LPVOID *, PUINT);
VerQueryValueA_Type pVerQueryValueA;
BOOL WINAPI extVerQueryValueA(LPCVOID, LPCSTR, LPVOID *, PUINT);

static HookEntryEx_Type VersionHooks[] = {
    {HOOK_IAT_CANDIDATE, 0, "GetFileVersionInfoA", (FARPROC)NULL, (FARPROC *) &pGetFileVersionInfoA, (FARPROC)extGetFileVersionInfoA},
    {HOOK_IAT_CANDIDATE, 0, "VerQueryValueA", (FARPROC)NULL, (FARPROC *) &pVerQueryValueA, (FARPROC)extVerQueryValueA},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

static char *libname = "version.dll";

void HookVersion(HMODULE module) {
    if(dxw.dwFlags11 & CUSTOMLOCALE) HookLibraryEx(module, VersionHooks, libname);
}

FARPROC Remap_Version_ProcAddress(LPCSTR proc, HMODULE hModule) {
    FARPROC addr;
    if(dxw.dwFlags11 & CUSTOMLOCALE)
        if (addr = RemapLibraryEx(proc, hModule, VersionHooks)) return addr;
    return NULL;
}

BOOL WINAPI extGetFileVersionInfoA(LPCSTR lptstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData) {
    BOOL ret;
    ApiName("GetFileVersionInfoA");
    OutTraceSYS("%s: fname=\"%s\" handle=%#x len=%d lpdata=%#x\n",
                ApiRef, lptstrFilename, dwHandle, dwLen, lpData);
    ret = (*pGetFileVersionInfoA)(lptstrFilename, dwHandle, dwLen, lpData);
    if(dxw.dwFlags11 & CUSTOMLOCALE) {
        char Filename[MAX_PATH];
        char *s = (char *)lptstrFilename;
        char *d = Filename;
        for(; *s; s++, d++) *d = (char)tolower(*s);
        if(strstr(Filename, "kernel32.dll")) {
            OutTraceDW("%s: getting kernel info on path=\"%s\" data=%#x\n", ApiRef, lptstrFilename, lpData);
            gFakeLocaleData = lpData;
        }
    }
#ifndef DXW_NOTRACES
    if (!ret)
        OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
#endif // DXW_NOTRACES
    return ret;
}

BOOL WINAPI extVerQueryValueA(LPCVOID pBlock, LPCSTR lpSubBlock, LPVOID *lplpBuffer, PUINT puLen) {
    BOOL ret;
    ApiName("VerQueryValueA");
    OutTraceSYS("%s: block=%#x subblock=\"%s\"\n", ApiRef, pBlock, lpSubBlock);
    ret = (*pVerQueryValueA)(pBlock, lpSubBlock, lplpBuffer, puLen);
    if(ret && (dxw.dwFlags11 & CUSTOMLOCALE) && (pBlock == gFakeLocaleData)) {
        if(!strcmp(lpSubBlock, "\\") && (*puLen == sizeof(VS_FIXEDFILEINFO))) {
            OutTraceDW("%s: getting kernel32 root values\n", ApiRef);
            VS_FIXEDFILEINFO *vsfi = (VS_FIXEDFILEINFO *)(*lplpBuffer);
            OutTraceDW("%s: real major=%#x minor=%#x\n", ApiRef, HIWORD(vsfi->dwProductVersionMS), LOWORD(vsfi->dwProductVersionMS));
            vsfi->dwProductVersionMS = MAKEWORD(dxwGetMajor(), dxwGetMinor());
            OutTraceDW("%s: fake major=%#x minor=%#x\n", ApiRef, HIWORD(vsfi->dwProductVersionMS), LOWORD(vsfi->dwProductVersionMS));
        }
        if(!strncmp(lpSubBlock, "\\VarFileInfo\\Translation", strlen("\\VarFileInfo\\Translation"))) {
            OutTraceDW("%s: getting \\VarFileInfo\\Translation values\n", ApiRef);
            if(*puLen == 4) {
                WORD *langcodepage = (WORD *)*lplpBuffer;
                OutTraceDW("%s: original lang=%04.4X page=%04.4X\n", ApiRef, langcodepage[0], langcodepage[1]);
                // setting custom values here ... used in "Tang Poetry 2".
                if(dxw.Language) langcodepage[0] = (WORD)dxw.Language;
                if(dxw.CodePage) langcodepage[1] = (WORD)dxw.CodePage;
                OutTraceDW("%s: custom lang=%04.4X page=%04.4X\n", ApiRef, langcodepage[0], langcodepage[1]);
            }
        }
        if(!strncmp(lpSubBlock, "\\StringFileInfo\\", strlen("\\StringFileInfo\\"))) {
            OutTraceDW("%s: getting \\StringFileInfo values - TBD\n", ApiRef);
            // string format: \StringFileInfo\xxxxxxxx\<string> so that the last part of the version
            // is at index strlen("\StringFileInfo\") + 8 + 1
            LPBYTE key = (LPBYTE)lpSubBlock + strlen("\\StringFileInfo\\") + 9;
            OutTraceDW("%s: getting \\StringFileInfo key=%s\n", ApiRef, key);
            if(!strcmp((char *)key, "FileDescription")) {
                WORD *langcodepage = (WORD *)*lplpBuffer;
                OutTraceDW("%s: lang=%04.4X page=%04.4X\n", ApiRef, langcodepage[0], langcodepage[1]);
            }
        }
    }
#if 0
    if(!ret && (dxw.dwFlags11 & CUSTOMLOCALE) && !strncmp(lpSubBlock, "\\StringFileInfo\\", strlen("\\StringFileInfo\\"))) {
        static WORD langcodepage[] = {0, 0};
        langcodepage[0] = (WORD)(*pGetPrivateProfileIntA)("locale", "lang", 0, ".\\locale.ini");
        langcodepage[1] = (WORD)(*pGetPrivateProfileIntA)("locale", "page", -1, ".\\locale.ini");
        *(WORD **)lplpBuffer = langcodepage;
        ret = TRUE;
    }
#endif
#ifndef DXW_NOTRACES
    if(ret) {
        OutTraceSYS("%s: ok len=%d\n", ApiRef, *puLen);
        if(IsDebugSYS) {
            BOOL bPrintable = TRUE;
            LPBYTE p = (LPBYTE) * lplpBuffer;
            if(*puLen > 0) for(UINT i = 0; i < *puLen - 1; i++) {
                    if(!isprint(*p++)) {
                        bPrintable = FALSE;
                        break;
                    }
                }
            if(bPrintable)
                OutDebugSYS("%s: data=\"%s\"\n", ApiRef, (LPBYTE)*lplpBuffer);
            else
                OutHexSYS((LPBYTE)*lplpBuffer, *puLen);
        }
    } else
        OutTraceE("%s: ERROR err=%d\n", ApiRef, GetLastError());
#endif // DXW_NOTRACES
    return ret;
}
