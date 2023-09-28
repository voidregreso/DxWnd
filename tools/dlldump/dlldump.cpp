// dlldump.cpp : Defines the entry point for the console application.
//

#include <windows.h>
#include <winnt.h>
#include <assert.h>
#include "stdafx.h"

#define DLLDUMP_HEXORDINALS 0x00000001
#define DLLDUMP_DECORDINALS 0x00000002
#define DLLDUMP_LOADENTRY   0x00000004
//#define DLLDUMP_FUNCADDRESS 0x00000004

void exportsdump(char *path, DWORD flags) {
    printf("=== %s ===\n", path);
    HMODULE lib = LoadLibraryExA(path, NULL, DONT_RESOLVE_DLL_REFERENCES);
    if(!lib) {
        printf("LoadLibrary(%s) failed err=%d\n", path, GetLastError());
        return;
    }
    assert(((PIMAGE_DOS_HEADER)lib)->e_magic == IMAGE_DOS_SIGNATURE);
    PIMAGE_NT_HEADERS header = (PIMAGE_NT_HEADERS)((BYTE *)lib + ((PIMAGE_DOS_HEADER)lib)->e_lfanew);
    assert(header->Signature == IMAGE_NT_SIGNATURE);
    assert(header->OptionalHeader.NumberOfRvaAndSizes > 0);
    PIMAGE_EXPORT_DIRECTORY exports = (PIMAGE_EXPORT_DIRECTORY)((BYTE *)lib + header->
                                      OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
    assert(exports->AddressOfNames != 0);
    BYTE **names = (BYTE **)((int)lib + exports->AddressOfNames);
    WORD *ordinals = (WORD *)((int)lib + exports->AddressOfNameOrdinals);
    DWORD *address = (DWORD *)((int)lib + exports->AddressOfFunctions);
    for (DWORD i = 0; i < exports->NumberOfNames; i++) {
        if(flags & DLLDUMP_HEXORDINALS) printf("0x%04.4X ", ordinals[i] + 1);
        if(flags & DLLDUMP_DECORDINALS) printf("%04.4d ", ordinals[i] + 1);
        //if(flags & DLLDUMP_FUNCADDRESS) printf("@%08.8X ", address[i]);
        if(flags & DLLDUMP_LOADENTRY) {
            FARPROC addr;
            addr = GetProcAddress(lib, (LPCSTR)lib + (int)names[i]);
            printf("%#8.8x %s %s\n", addr, (BYTE *)lib + (int)names[i], addr ? "ok" : "FAILED!!!");
        } else
            printf("%s\n", (BYTE *)lib + (int)names[i]);
    }
    FreeLibrary(lib);
}

int main(int argc, char *argv[]) {
    DWORD flags = 0;
    if(argc == 1) {
        printf("dlldump: (C)gho 2019\n\nUsage:\n"
               "dlldump [/H] [/D] path ....\n"
               "\t /L: try loading each entry\n"
               "\t /H: print ordinal in hex format\n"
               "\t /D: print ordinal in decimal format\n");
    }
    for(int i = 1; i < argc; i++) {
        if(argv[i][0] == '/') {
            switch(argv[i][1]) {
            case 'L':
                flags |= DLLDUMP_LOADENTRY;
                break;
            case 'H':
                flags |= DLLDUMP_HEXORDINALS;
                break;
            case 'D':
                flags |= DLLDUMP_DECORDINALS;
                break;
                //case 'A': flags |= DLLDUMP_FUNCADDRESS; break;
                // others ??
            }
        } else
            exportsdump(argv[i], flags);
    }
    return 0;
}

