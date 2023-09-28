#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "dxwnd.h"
#include "dxwcore.hpp"

const void *memmem(const void *l, size_t l_len, const void *s, size_t s_len) {
    register char *cur, *last;
    const char *cl = (const char *)l;
    const char *cs = (const char *)s;
    // we need something to compare
    if (!l_len || !s_len) return NULL;
    // "s" must be smaller or equal to "l"
    if (l_len < s_len) return NULL;
    // special case where s_len == 1
    if (s_len == 1) return memchr(l, (int) * cs, l_len);
    // the last position where it's possible to find "s" in "l"
    last = (char *)cl + l_len - s_len;
    for (cur = (char *)cl; cur <= last; cur++)
        if (cur[0] == cs[0] && !memcmp(cur, cs, s_len))
            return cur;
    return NULL;
}

extern char *hexdump(unsigned char *, int);

void dxPatchModule(HMODULE hmod, const BYTE *wantedBytes, int wantedLen, const BYTE *updatedBytes, int updatedLen, BOOL bRepeat) {
    PIMAGE_DOS_HEADER pDosHeader;
    PIMAGE_NT_HEADERS pNtHeader;
    DWORD dwCodeBase;
    DWORD dwCodeSize;
    BYTE *dwPatchBase;
    DWORD dwOldProtect;
    //DisableThreadLibraryCalls(hinstDLL);
    pDosHeader = (PIMAGE_DOS_HEADER)hmod;
    pNtHeader = (PIMAGE_NT_HEADERS)((char *)pDosHeader + pDosHeader->e_lfanew);
    dwCodeBase = (DWORD)hmod + pNtHeader->OptionalHeader.BaseOfCode;
    dwCodeSize = pNtHeader->OptionalHeader.SizeOfCode;
    OutTrace("dxPatchModule: patching hmod=%#x wantedlen=%d updatedlen=%d base=%#x size=%#x\n",
             hmod, wantedLen, updatedLen, dwCodeBase, dwCodeSize);
    do {
        dwPatchBase = (BYTE *)memmem((void *)dwCodeBase, dwCodeSize, wantedBytes, wantedLen);
        if(!dwPatchBase) break;
        OutTrace("dxPatchModule: patching at=%#x mem={%s}\n", dwPatchBase, hexdump(dwPatchBase, updatedLen));
        VirtualProtect((LPVOID)dwPatchBase, updatedLen, PAGE_EXECUTE_READWRITE, &dwOldProtect);
        memcpy(dwPatchBase, updatedBytes, updatedLen);
        VirtualProtect((LPVOID)dwPatchBase, updatedLen, dwOldProtect, &dwOldProtect);
        dwCodeSize = dwCodeBase + dwCodeSize - (DWORD)dwPatchBase;
        dwCodeBase = (DWORD)dwPatchBase;
    } while (bRepeat);
}