//==================================
// this file contains source code from
// PEDUMP - Matt Pietrek 1994-1998
// FILE: PEDUMP.C
//==================================

#define  _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
//#include <time.h>
#include "dxwnd.h"
#include "dxwcore.hpp"

// MakePtr is a macro that allows you to easily add to values (including
// pointers) together without dealing with C's pointer arithmetic.  It
// essentially treats the last two parameters as DWORDs.  The first
// parameter is used to typecast the result to the appropriate pointer type.
#define MakePtr( cast, ptr, addValue ) (cast)( (DWORD)(ptr) + (DWORD)(addValue))

#define GetImgDirEntryRVA( pNTHdr, IDE ) \
	(pNTHdr->OptionalHeader.DataDirectory[IDE].VirtualAddress)

#define GetImgDirEntrySize( pNTHdr, IDE ) \
	(pNTHdr->OptionalHeader.DataDirectory[IDE].Size)

//
// Given an RVA, look up the section header that encloses it and return a
// pointer to its IMAGE_SECTION_HEADER
//
PIMAGE_SECTION_HEADER GetEnclosingSectionHeader(DWORD rva, PIMAGE_NT_HEADERS pNTHeader) {
    PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(pNTHeader);
    unsigned i;
    for ( i = 0; i < pNTHeader->FileHeader.NumberOfSections; i++, section++ ) {
        // Is the RVA within this section?
        if ( (rva >= section->VirtualAddress) &&
                (rva < (section->VirtualAddress + section->Misc.VirtualSize)))
            return section;
    }
    return 0;
}

LPVOID GetPtrFromRVA( DWORD rva, PIMAGE_NT_HEADERS pNTHeader, DWORD imageBase ) {
    PIMAGE_SECTION_HEADER pSectionHdr;
    INT delta;
    pSectionHdr = GetEnclosingSectionHeader( rva, pNTHeader );
    if ( !pSectionHdr ) return 0;
    delta = (INT)(pSectionHdr->VirtualAddress - pSectionHdr->PointerToRawData);
    return (PVOID) ( imageBase + rva - delta );
}

static void *DoPatching(HMODULE module, DWORD base, PIMAGE_NT_HEADERS pNTHeader, DWORD ordinal, char *dll, void *apiproc, const char *apiname, void *hookproc) {
    // PE file pointers
    PIMAGE_IMPORT_DESCRIPTOR importDesc;
    PIMAGE_SECTION_HEADER pSection;
    PIMAGE_THUNK_DATA thunk, thunkIAT = 0;
    PIMAGE_IMPORT_BY_NAME pOrdinalName;
    DWORD importsStartRVA;
    // executable pointers
    PIMAGE_NT_HEADERS pnth;
    PIMAGE_IMPORT_DESCRIPTOR pidesc;
    DWORD rva;
    PIMAGE_THUNK_DATA ptaddr;
    DWORD mbase;
    __try {
        mbase = (DWORD)module;
        pnth = PIMAGE_NT_HEADERS(PBYTE(mbase) + PIMAGE_DOS_HEADER(mbase)->e_lfanew);
        if(!pnth) {
            OutTraceE("IATPatchByFT: ERROR no pnth at %d\n", __LINE__);
            return NULL;
        }
        rva = pnth->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
        if(!rva) {
            OutTraceE("IATPatchByFT: ERROR no rva at %d\n", __LINE__);
            return NULL;
        }
        pidesc = (PIMAGE_IMPORT_DESCRIPTOR)(mbase + rva);
    } __except( TRUE ) { // Should only get here if pNTHeader (above) is bogus
        OutTraceE("IATPatchByFT ERROR: exception at %d\n", __LINE__);
        return NULL;
    }
    // Look up where the imports section is (normally in the .idata section)
    // but not necessarily so.  Therefore, grab the RVA from the data dir.
    importsStartRVA = GetImgDirEntryRVA(pNTHeader, IMAGE_DIRECTORY_ENTRY_IMPORT);
    if ( !importsStartRVA ) return NULL;
    // Get the IMAGE_SECTION_HEADER that contains the imports.  This is
    // usually the .idata section, but doesn't have to be.
    pSection = GetEnclosingSectionHeader( importsStartRVA, pNTHeader );
    if ( !pSection ) return NULL;
    importDesc = (PIMAGE_IMPORT_DESCRIPTOR) GetPtrFromRVA(importsStartRVA, pNTHeader, base);
    if ( !importDesc ) return NULL;
    while ( TRUE ) {
        char *dllname;
        // See if we've reached an empty IMAGE_IMPORT_DESCRIPTOR
        if ( (importDesc->TimeDateStamp == 0 ) && (importDesc->Name == 0) ) break;
        ptaddr = (PIMAGE_THUNK_DATA)(mbase + (DWORD)pidesc->FirstThunk);
        dllname = (char *)GetPtrFromRVA(importDesc->Name, pNTHeader, base);
        if ( _stricmp(dll, dllname)) {
            OutDebugH("IATPatchByFT: skip name=\"%s\"\n", dllname);
            importDesc++;   // advance to next IMAGE_IMPORT_DESCRIPTOR
            pidesc++;
            continue;
        }
        thunk = (PIMAGE_THUNK_DATA)importDesc->Characteristics;
        thunkIAT = (PIMAGE_THUNK_DATA)importDesc->FirstThunk;
        if ( thunk == 0 ) {  // No Characteristics field?
            // Yes! Gotta have a non-zero FirstThunk field then.
            thunk = thunkIAT;
            if ( thunk == 0 ) return NULL;  // No FirstThunk field?  Ooops!!!
        }
        // Adjust the pointer to point where the tables are in the mem mapped file.
        thunk = (PIMAGE_THUNK_DATA)GetPtrFromRVA((DWORD)thunk, pNTHeader, base);
        if (!thunk ) return NULL;
        thunkIAT = (PIMAGE_THUNK_DATA)GetPtrFromRVA((DWORD)thunkIAT, pNTHeader, base);
        while ( TRUE ) { // Loop forever (or until we break out)
            if ( thunk->u1.AddressOfData == 0 ) break; // end of IMAGE_THUNK_DATA array
            if ( thunk->u1.Ordinal & IMAGE_ORDINAL_FLAG ) { // find by ordinal ....
                if (thunk->u1.Ordinal == (ordinal | IMAGE_ORDINAL_FLAG)) {
                    OutTraceH( "IATPatchByFT: matching addr=%08x ord=%#x\n",
                               thunkIAT->u1.Function, IMAGE_ORDINAL(thunk->u1.Ordinal) );
                    break;
                }
            } else {	// find by name ...
                pOrdinalName = (PIMAGE_IMPORT_BY_NAME)thunk->u1.AddressOfData;
                pOrdinalName = (PIMAGE_IMPORT_BY_NAME)GetPtrFromRVA((DWORD)pOrdinalName, pNTHeader, base);
                if(!strcmp((char *)&pOrdinalName->Name[0], apiname)) {
                    OutTraceH("IATPatchByFT: matching addr=%08x hint=%#x name=\"%s\"\n",
                              thunkIAT->u1.Function, pOrdinalName->Hint, pOrdinalName->Name);
                    break;
                }
            }
            thunk++;            // Advance to next thunk
            thunkIAT++;         // advance to next thunk
            ptaddr ++;			// advance also in-memory thunk
        }
        if(thunk->u1.AddressOfData) {
            void *org;
            DWORD oldprotect;
            OutDebugH("IATPatchByFT: hooking %s\n", apiname);
            org = GetPtrFromRVA(thunk->u1.AddressOfData, pNTHeader, base);
            if(org == hookproc) {
                OutTraceH("IATPatchByFT: %s already hooked (1)\n", apiname);
                return NULL; // already hooked
            }
            org = (void *)ptaddr->u1.Function;
            if(org == hookproc) {
                OutTraceH("IATPatchByFT: %s already hooked (2)\n", apiname);
                return NULL; // already hooked
            }
            if(!VirtualProtect(&ptaddr->u1.Function, 4, PAGE_EXECUTE_READWRITE, &oldprotect)) {
                OutTraceDW("IATPatchByFT: VirtualProtect error %d at %d\n", GetLastError(), __LINE__);
                return 0;
            }
            ptaddr->u1.Function = (DWORD)hookproc;
            if(!VirtualProtect(&ptaddr->u1.Function, 4, oldprotect, &oldprotect)) {
                OutTraceDW("IATPatchByFT: VirtualProtect error %d at %d\n", GetLastError(), __LINE__);
                return 0;
            }
            if (!FlushInstructionCache(GetCurrentProcess(), &ptaddr->u1.Function, 4)) {
                OutTraceDW("IATPatchByFT: FlushInstructionCache error %d at %d\n", GetLastError(), __LINE__);
                return 0;
            }
            OutTraceH("IATPatchByFT hook=%s address=%#x->%#x\n", apiname, org, hookproc);
            return org;
        } else
            OutTraceH("IATPatchByFT: unreferenced name=\"%s\"\n", apiname);
        importDesc++;		// advance to next IMAGE_IMPORT_DESCRIPTOR
        pidesc++;			// advance to next thunk
    }
    return NULL;
}

void *IATPatchByFT(HMODULE module, DWORD ordinal, char *dll, void *apiproc, const char *apiname, void *hookproc) {
    static HANDLE hFile = NULL;
    static HANDLE hFileMapping = NULL;
    static LPVOID lpFileBase = NULL;
    PIMAGE_DOS_HEADER dosHeader;
    static HMODULE hLastModule = NULL;
    if(module == NULL) { // destroy chached elements and terminate
        if (lpFileBase) UnmapViewOfFile(lpFileBase);
        if (hFileMapping) CloseHandle(hFileMapping);
        if (hFile) CloseHandle(hFile);
        lpFileBase = NULL;
        hFileMapping = NULL;
        hFile = NULL;
        hLastModule = NULL;
        return NULL;
    }
    OutTraceH("IATPatchByFT: module=%#x ordinal=%#x name=%s dll=%s\n", module, ordinal, apiname, dll);
    if(module != hLastModule) { // destroy chached elements to load new ones
        if (lpFileBase) UnmapViewOfFile(lpFileBase);
        if (hFileMapping) CloseHandle(hFileMapping);
        if (hFile) CloseHandle(hFile);
        lpFileBase = NULL;
        hFileMapping = NULL;
        hFile = NULL;
        hLastModule = module;
    }
    if(!lpFileBase) { // first time through, build cached elements
        char filename[MAX_PATH + 1];
        GetModuleFileName(module, filename, MAX_PATH);
        OutDebugH("IATPatchByFT: path=\"%s\"\n", filename);
        hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
        if ( hFile == INVALID_HANDLE_VALUE ) {
            OutTraceE("IATPatchByFT ERROR: path=\"%s\" err=%d\n", filename, GetLastError());
            hFile = NULL;
            return NULL;
        }
        hFileMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
        if ( hFileMapping == NULL ) {
            CloseHandle(hFile);
            OutTraceE("IATPatchByFT CreateFileMapping ERROR: err=%d\n", GetLastError());
            hFile = NULL;
            return NULL;
        }
        lpFileBase = MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, 0);
        if ( lpFileBase == NULL ) {
            CloseHandle(hFileMapping);
            CloseHandle(hFile);
            OutTraceE("IATPatchByFT MapViewOfFile ERROR: err=%d\n", GetLastError());
            hFile = NULL;
            hFileMapping = NULL;
            return NULL;
        }
    }
    dosHeader = (PIMAGE_DOS_HEADER)lpFileBase;
    PIMAGE_FILE_HEADER pImgFileHdr = (PIMAGE_FILE_HEADER)lpFileBase;
    if ( dosHeader->e_magic == IMAGE_DOS_SIGNATURE ) {
        PIMAGE_NT_HEADERS pNTHeader;
        DWORD base = (DWORD)dosHeader;
        pNTHeader = (PIMAGE_NT_HEADERS) MakePtr( PIMAGE_NT_HEADERS, dosHeader, dosHeader->e_lfanew );
        // First, verify that the e_lfanew field gave us a reasonable
        // pointer, then verify the PE signature.
        __try {
            if ( pNTHeader->Signature != IMAGE_NT_SIGNATURE ) {
                OutTraceE("IATPatchByFT ERROR: Not a Portable Executable (PE) EXE\n");
                return NULL;
            }
        } __except( TRUE ) { // Should only get here if pNTHeader (above) is bogus
            OutTraceE("IATPatchByFT ERROR: invalid .EXE\n");
            return NULL;
        }
        // patch here ...
        return DoPatching(module, base, pNTHeader, ordinal, dll, apiproc, apiname, hookproc);
    } else
        OutTraceE("IATPatchByFT ERROR: invalid PE exe header=%#x\n", dosHeader->e_magic);
    return NULL;
}


void DumpImportsSection(DWORD base, PIMAGE_NT_HEADERS pNTHeader) {
    PIMAGE_IMPORT_DESCRIPTOR importDesc;
    PIMAGE_SECTION_HEADER pSection;
    PIMAGE_THUNK_DATA thunk, thunkIAT = 0;
    PIMAGE_IMPORT_BY_NAME pOrdinalName;
    DWORD importsStartRVA;
    // Look up where the imports section is (normally in the .idata section)
    // but not necessarily so.  Therefore, grab the RVA from the data dir.
    importsStartRVA = GetImgDirEntryRVA(pNTHeader, IMAGE_DIRECTORY_ENTRY_IMPORT);
    if ( !importsStartRVA ) return;
    // Get the IMAGE_SECTION_HEADER that contains the imports.  This is
    // usually the .idata section, but doesn't have to be.
    pSection = GetEnclosingSectionHeader( importsStartRVA, pNTHeader );
    if ( !pSection ) return;
    importDesc = (PIMAGE_IMPORT_DESCRIPTOR) GetPtrFromRVA(importsStartRVA, pNTHeader, base);
    if ( !importDesc ) return;
    while ( TRUE ) {
        // See if we've reached an empty IMAGE_IMPORT_DESCRIPTOR
        if ( (importDesc->TimeDateStamp == 0 ) && (importDesc->Name == 0) ) break;
        OutTrace("DumpIAT: name=\"%s\" TimeDateStamp=%08X OrigFirstThunk=%08X ForwarderChain=%08X FirstThunk(RVA)=%08X\n",
                 GetPtrFromRVA(importDesc->Name, pNTHeader, base),
                 importDesc->TimeDateStamp,
                 importDesc->ForwarderChain,
                 importDesc->FirstThunk
                );
        thunk = (PIMAGE_THUNK_DATA)importDesc->Characteristics;
        thunkIAT = (PIMAGE_THUNK_DATA)importDesc->FirstThunk;
        if ( thunk == 0 ) {  // No Characteristics field?
            // Yes! Gotta have a non-zero FirstThunk field then.
            thunk = thunkIAT;
            if ( thunk == 0 ) return;  // No FirstThunk field?  Ooops!!!
        }
        // Adjust the pointer to point where the tables are in the mem mapped file.
        thunk = (PIMAGE_THUNK_DATA)GetPtrFromRVA((DWORD)thunk, pNTHeader, base);
        if (!thunk ) return;
        thunkIAT = (PIMAGE_THUNK_DATA)GetPtrFromRVA((DWORD)thunkIAT, pNTHeader, base);
        while ( TRUE ) { // Loop forever (or until we break out)
            if ( thunk->u1.AddressOfData == 0 ) break;
            if ( thunk->u1.Ordinal & IMAGE_ORDINAL_FLAG )
                OutTrace( "addr=%08x ord=%#x\n", thunkIAT->u1.Function, IMAGE_ORDINAL(thunk->u1.Ordinal) );
            else {
                pOrdinalName = (PIMAGE_IMPORT_BY_NAME)thunk->u1.AddressOfData;
                pOrdinalName = (PIMAGE_IMPORT_BY_NAME)GetPtrFromRVA((DWORD)pOrdinalName, pNTHeader, base);
                OutTrace("addr=%08x hint=%04.4x name=\"%s\"\n", thunkIAT->u1.Function, pOrdinalName->Hint, pOrdinalName->Name);
            }
            thunk++;            // Advance to next thunk
            thunkIAT++;         // advance to next thunk
        }
        importDesc++;   // advance to next IMAGE_IMPORT_DESCRIPTOR
        OutTrace("*** EOT ***\n");
    }
}

void DumpImportTableByFT(HMODULE module) {
    HANDLE hFile;
    HANDLE hFileMapping;
    LPVOID lpFileBase;
    PIMAGE_DOS_HEADER dosHeader;
    char filename[MAX_PATH + 1];
    GetModuleFileName(module, filename, MAX_PATH);
    hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL,
                       OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if ( hFile == INVALID_HANDLE_VALUE ) {
        OutTraceE("DumpByFT ERROR: path=\"%s\" err=%d\n", filename, GetLastError());
        return;
    }
    hFileMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
    if ( hFileMapping == 0 ) {
        CloseHandle(hFile);
        OutTraceE("DumpByFT CreateFileMapping ERROR: err=%d\n", GetLastError());
        return;
    }
    lpFileBase = MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, 0);
    if ( lpFileBase == 0 ) {
        CloseHandle(hFileMapping);
        CloseHandle(hFile);
        OutTraceE("DumpByFT MapViewOfFile ERROR: err=%d\n", GetLastError());
        return;
    }
    OutTrace("DumpByFT: base=%#x path=\"%s\"\n", module, filename);
    dosHeader = (PIMAGE_DOS_HEADER)lpFileBase;
    PIMAGE_FILE_HEADER pImgFileHdr = (PIMAGE_FILE_HEADER)lpFileBase;
    if ( dosHeader->e_magic == IMAGE_DOS_SIGNATURE ) {
        PIMAGE_NT_HEADERS pNTHeader;
        DWORD base = (DWORD)dosHeader;
        pNTHeader = (PIMAGE_NT_HEADERS) MakePtr( PIMAGE_NT_HEADERS, dosHeader, dosHeader->e_lfanew );
        // First, verify that the e_lfanew field gave us a reasonable
        // pointer, then verify the PE signature.
        __try {
            if ( pNTHeader->Signature != IMAGE_NT_SIGNATURE ) {
                OutTraceE("DumpByFT ERROR: Not a Portable Executable (PE) EXE\n");
                return;
            }
        } __except( TRUE ) { // Should only get here if pNTHeader (above) is bogus
            OutTraceE("DumpByFT ERROR: invalid .EXE\n");
            return;
        }
        DumpImportsSection(base, pNTHeader);
    } else
        OutTraceE("DumpByFT ERROR: invalid PE exe header=%#x\n", dosHeader->e_magic);
    UnmapViewOfFile(lpFileBase);
    CloseHandle(hFileMapping);
    CloseHandle(hFile);
}
#if 0


void DumpImportTableByFT(HMODULE module) {
    TCHAR sPath[MAX_PATH + 1];
    FILE *fpExe;
    DWORD dwPEOffset;
    BYTE dwPEHeader[4];
    WORD wPEType;
    DWORD dwBaseAddr;
    DWORD dwNumOfRVAandSizes;
    DWORD dwIDataRawAddress;
    DWORD dwISizeOfRawData;
    DWORD delta;
    IMAGE_DATA_DIRECTORY imdd;
    IMAGE_DATA_DIRECTORY iatd;
    IMAGE_SECTION_HEADER ish;
    IMAGE_IMPORT_DESCRIPTOR iid;
    IMAGE_THUNK_DATA itd;
    IMAGE_IMPORT_BY_NAME iibn;
    GetModuleFileName(module, sPath, MAX_PATH);
    OutTrace(">>> Path=%s\n", sPath);
    fpExe = fopen(sPath, "rb");
    if(!fpExe) {
        OutTraceE("DumpByFT ERROR: path=\"%s\" err=%d\n", sPath, GetLastError());
        return;
    }
    fseek(fpExe, 0x03C, SEEK_SET);
    fread((void *)&dwPEOffset, sizeof(DWORD), 1, fpExe);
    OutTrace(">>> PEOffset=%#x\n", dwPEOffset);
    fseek(fpExe, dwPEOffset, SEEK_SET);
    fread((void *)dwPEHeader, 4, 1, fpExe);
    if(memcmp(dwPEHeader, "PE\0\0", 4)) {
        OutTraceE("DumpByFT ERROR: invalid PE header=%#x-%#x-%#x-%#x\n", dwPEHeader[0], dwPEHeader[1], dwPEHeader[2], dwPEHeader[3]);
        return;
    }
    fseek(fpExe, dwPEOffset + 0x18, SEEK_SET);
    fread(&wPEType, sizeof(WORD), 1, fpExe);
    if(wPEType != 0x10b) {
        OutTraceE("DumpByFT ERROR: not a PE32 magic=%#x\n", wPEType);
        return;
    }
    //fseek(fpExe, dwPEOffset + 0x5C, SEEK_SET);
    fseek(fpExe, dwPEOffset + 0x74, SEEK_SET);
    fread(&dwNumOfRVAandSizes, sizeof(DWORD), 1, fpExe);
    OutTraceE(">>> NumOfRVAandSizes=%#x\n", dwNumOfRVAandSizes);
    fseek(fpExe, dwPEOffset + 0x18 + 0x1C, SEEK_SET);
    fread(&dwBaseAddr, sizeof(DWORD), 1, fpExe);
    OutTrace(">>> BaseAddr=%#x\n", dwBaseAddr);
    fseek(fpExe, dwPEOffset + 0x18 + 0x68, SEEK_SET);
    fread(&imdd, sizeof(IMAGE_DATA_DIRECTORY), 1, fpExe);
    OutTrace(">>> IFT offset=%#x size=%#x fileoffset=%#x\n", imdd.VirtualAddress, imdd.Size, imdd.VirtualAddress - dwBaseAddr);
    if(imdd.Size == 0) {
        OutTraceE("DumpByFT ERROR: no IMPORT DATA\n");
        return;
    }
    fseek(fpExe, dwPEOffset + 0x18 + 0xC0, SEEK_SET);
    fread(&iatd, sizeof(IMAGE_DATA_DIRECTORY), 1, fpExe);
    OutTrace(">>> IAT offset=%#x size=%#x fileoffset=%#x\n", iatd.VirtualAddress, iatd.Size, iatd.VirtualAddress - dwBaseAddr);
    if(iatd.Size == 0) {
        OutTraceE("DumpByFT ERROR: no IMPORT ADDRESS TABLE\n");
        return;
    }
    fseek(fpExe, dwPEOffset + 0x18 + 0x60 + (dwNumOfRVAandSizes * sizeof(IMAGE_DATA_DIRECTORY)), SEEK_SET);
    OutTrace(">>> SECTION BEGIN @%04.4X\n", ftell(fpExe));
    dwIDataRawAddress = 0;
    dwISizeOfRawData = 0;
    while(TRUE) {
        long lSeek;
        lSeek = ftell(fpExe);
        fread(&ish, sizeof(IMAGE_SECTION_HEADER), 1, fpExe);
        if(!ish.Name[0]) break;
        OutTrace(">>> SECTION @%04.4X: %8.8s PointerToRawData=%#x SizeOfRawData=%#x\n", lSeek, ish.Name, ish.PointerToRawData, ish.SizeOfRawData);
        if(!strcmp((char *)ish.Name, ".idata")) {
            dwIDataRawAddress = ish.PointerToRawData;
            dwISizeOfRawData = ish.SizeOfRawData;
        }
    }
    if(!dwIDataRawAddress) {
        OutTraceE("DumpByFT ERROR: no .idata section\n");
        return;
    }
    OutTrace(">>> .idata rawaddr=%#x rawsize=%#x\n", dwIDataRawAddress, dwISizeOfRawData);
    delta = imdd.VirtualAddress - dwIDataRawAddress;
    OutTrace(">>> delta=%#x\n", delta);
    fseek(fpExe, dwIDataRawAddress, SEEK_SET);
    while(TRUE) {
        long lSeek;
        lSeek = ftell(fpExe);
        fread(&iid, sizeof(IMAGE_IMPORT_DESCRIPTOR), 1, fpExe);
        lSeek = ftell(fpExe);
        if(!iid.Name) break;
        OutTrace(">>> IMPORT @%04.4X: name=%#x(%s) FTs(IAT)=%#x ForwardedChain=%#x OFT=%#x char=%#x\n",
                 lSeek, iid.Name, (char *)(dwBaseAddr + iid.Name), iid.FirstThunk, iid.ForwarderChain, iid.OriginalFirstThunk, iid.Characteristics);
        IMAGE_THUNK_DATA *pitd;
        pitd = (IMAGE_THUNK_DATA *)(dwBaseAddr + iid.FirstThunk);
        while(pitd->u1.AddressOfData) {
            //IMAGE_IMPORT_BY_NAME *piibn = (IMAGE_IMPORT_BY_NAME *)(dwBaseAddr + pitd->u1.AddressOfData);
            IMAGE_IMPORT_BY_NAME *piibn = (IMAGE_IMPORT_BY_NAME *)(pitd->u1.AddressOfData + delta);
            OutTrace("\t> hint=%#x name=%s\n", piibn->Hint, piibn->Name);
            pitd++;
        }
    }
    OutTrace("*** DumpByFT END ***\n");
    exit(0);
}
#endif