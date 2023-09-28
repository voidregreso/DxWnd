//==================================
// source code derived from
// PEDUMP - Matt Pietrek 1994-1998
// FILE: PEDUMP.C
//==================================

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>

void DumpImportsSection(DWORD, PIMAGE_NT_HEADERS);
void DumpExeFile(PIMAGE_DOS_HEADER);
PIMAGE_SECTION_HEADER GetEnclosingSectionHeader(DWORD rva, PIMAGE_NT_HEADERS pNTHeader);
LPVOID GetPtrFromRVA(DWORD, PIMAGE_NT_HEADERS, DWORD);

#define MakePtr( cast, ptr, addValue ) (cast)( (DWORD)(ptr) + (DWORD)(addValue))
#define GetImgDirEntryRVA( pNTHdr, IDE ) \
	(pNTHdr->OptionalHeader.DataDirectory[IDE].VirtualAddress)

BOOL fShowIATentries = FALSE;
FILE *out = NULL;

//
// Open up a file, memory map it, and call the appropriate dumping routine
//
void DumpImportTable(LPSTR filename, LPSTR outfile) {
    HANDLE hFile;
    HANDLE hFileMapping;
    LPVOID lpFileBase;
    PIMAGE_DOS_HEADER dosHeader;
    hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (hFile == INVALID_HANDLE_VALUE) {
        MessageBox(0, "Couldn't open file with CreateFile()", "DxWnd", 0);
        return;
    }
    hFileMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
    if (hFileMapping == 0) {
        CloseHandle(hFile);
        MessageBox(0, "Couldn't open file mapping with CreateFileMapping()", "DxWnd", 0);
        return;
    }
    lpFileBase = MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, 0);
    if (lpFileBase == 0) {
        CloseHandle(hFileMapping);
        CloseHandle(hFile);
        MessageBox(0, "Couldn't map view of file with MapViewOfFile()", "DxWnd", 0);
        return;
    }
    out = fopen(outfile, "w");
    if(!out) {
        MessageBox(0, "Couldn't open text file", "DxWnd", 0);
        return;
    }
    fprintf(out, "Dump of file %s\n", filename);
    dosHeader = (PIMAGE_DOS_HEADER)lpFileBase;
    PIMAGE_FILE_HEADER pImgFileHdr = (PIMAGE_FILE_HEADER)lpFileBase;
    if ( dosHeader->e_magic == IMAGE_DOS_SIGNATURE )
        DumpExeFile( dosHeader );
    else
        MessageBox(0, "Unsupported file format", "DxWnd", 0);
    UnmapViewOfFile(lpFileBase);
    CloseHandle(hFileMapping);
    CloseHandle(hFile);
    fclose(out);
}

void DumpExeFile(PIMAGE_DOS_HEADER dosHeader) {
    PIMAGE_NT_HEADERS pNTHeader;
    DWORD base = (DWORD)dosHeader;
    pNTHeader = MakePtr( PIMAGE_NT_HEADERS, dosHeader, dosHeader->e_lfanew );
    // First, verify that the e_lfanew field gave us a reasonable
    // pointer, then verify the PE signature.
    if ( pNTHeader->Signature != IMAGE_NT_SIGNATURE ) {
        fprintf(out, "Not a Portable Executable (PE) EXE\n");
        return;
    }
    DumpImportsSection(base, pNTHeader);
}

//
// Dump the imports table (the .idata section) of a PE file
//
void DumpImportsSection(DWORD base, PIMAGE_NT_HEADERS pNTHeader) {
    PIMAGE_IMPORT_DESCRIPTOR importDesc;
    PIMAGE_SECTION_HEADER pSection;
    PIMAGE_THUNK_DATA thunk, thunkIAT = 0;
    PIMAGE_IMPORT_BY_NAME pOrdinalName;
    DWORD importsStartRVA;
    PSTR pszTimeDate;
    // Look up where the imports section is (normally in the .idata section)
    // but not necessarily so.  Therefore, grab the RVA from the data dir.
    importsStartRVA = GetImgDirEntryRVA(pNTHeader, IMAGE_DIRECTORY_ENTRY_IMPORT);
    if (!importsStartRVA) {
        fprintf(out, "No start RVA!\n");
        return;
    }
    // Get the IMAGE_SECTION_HEADER that contains the imports.  This is
    // usually the .idata section, but doesn't have to be.
    pSection = GetEnclosingSectionHeader( importsStartRVA, pNTHeader );
    if (!pSection) {
        fprintf(out, "No enclosing section header!\n");
        return;
    }
    importDesc = (PIMAGE_IMPORT_DESCRIPTOR) GetPtrFromRVA(importsStartRVA, pNTHeader, base);
    if (!importDesc) {
        fprintf(out, "No import descriptor!\n");
        return;
    }
    fprintf(out, "Imports Table:\n");
    while (TRUE) {
        // See if we've reached an empty IMAGE_IMPORT_DESCRIPTOR
        if ((importDesc->TimeDateStamp == 0) && (importDesc->Name == 0)) break;
        fprintf(out, "%s\n", GetPtrFromRVA(importDesc->Name, pNTHeader, base) );
        fprintf(out, "OrigFirstThunk:  %08X (Unbound IAT)\n",
                importDesc->Characteristics);
        pszTimeDate = ctime((time_t *)&importDesc->TimeDateStamp);
        fprintf(out, "TimeDateStamp:   %08X", importDesc->TimeDateStamp );
        fprintf(out,  pszTimeDate ?  " -> %s" : "\n", pszTimeDate );
        fprintf(out, "ForwarderChain:  %08X\n", importDesc->ForwarderChain);
        fprintf(out, "First thunk RVA: %08X\n", importDesc->FirstThunk);
        thunk = (PIMAGE_THUNK_DATA)importDesc->Characteristics;
        thunkIAT = (PIMAGE_THUNK_DATA)importDesc->FirstThunk;
        if (thunk == 0) {  // No Characteristics field?
            // Yes! Gotta have a non-zero FirstThunk field then.
            thunk = thunkIAT;
            if (thunk == 0) return;   // No FirstThunk field?  Ooops!!!
        }
        // Adjust the pointer to point where the tables are in the
        // mem mapped file.
        thunk = (PIMAGE_THUNK_DATA)GetPtrFromRVA((DWORD)thunk, pNTHeader, base);
        if (!thunk ) return;
        thunkIAT = (PIMAGE_THUNK_DATA) GetPtrFromRVA((DWORD)thunkIAT, pNTHeader, base);
        fprintf(out, "Ordn  Name\n");
        while (TRUE) { // Loop forever (or until we break out)
            if ( thunk->u1.AddressOfData == 0 ) break;
            if ( thunk->u1.Ordinal & IMAGE_ORDINAL_FLAG )
                fprintf(out, "%4u", IMAGE_ORDINAL(thunk->u1.Ordinal) );
            else {
                pOrdinalName = (PIMAGE_IMPORT_BY_NAME)thunk->u1.AddressOfData;
                pOrdinalName = (PIMAGE_IMPORT_BY_NAME)GetPtrFromRVA((DWORD)pOrdinalName, pNTHeader, base);
                fprintf(out, "%4u  %s", pOrdinalName->Hint, pOrdinalName->Name);
            }
            // If the user explicitly asked to see the IAT entries, or
            // if it looks like the image has been bound, append the address
            if ( fShowIATentries || importDesc->TimeDateStamp )
                fprintf(out,  " (Bound to: %08X)", thunkIAT->u1.Function );
            fprintf(out,  "\n" );
            thunk++;            // Advance to next thunk
            thunkIAT++;         // advance to next thunk
        }
        importDesc++;   // advance to next IMAGE_IMPORT_DESCRIPTOR
        fprintf(out, "\n");
    }
}

//
// Given an RVA, look up the section header that encloses it and return a
// pointer to its IMAGE_SECTION_HEADER
//
PIMAGE_SECTION_HEADER GetEnclosingSectionHeader(DWORD rva, PIMAGE_NT_HEADERS pNTHeader) {
    PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(pNTHeader);
    unsigned i;
    //fprintf(out, "searching for rva=%08.8x\n", rva);
    for (i = 0; i < pNTHeader->FileHeader.NumberOfSections; i++, section++) {
        //fprintf(out, "section %d vaddr=(%08.8x-%08.8x)\n", i, section->VirtualAddress, section->VirtualAddress + section->Misc.VirtualSize);
        // Is the RVA within this section?
        if ( (rva >= section->VirtualAddress) &&
                (rva < (section->VirtualAddress + section->Misc.VirtualSize)))
            return section;
    }
    return 0;
}

LPVOID GetPtrFromRVA(DWORD rva, PIMAGE_NT_HEADERS pNTHeader, DWORD imageBase) {
    PIMAGE_SECTION_HEADER pSectionHdr;
    INT delta;
    pSectionHdr = GetEnclosingSectionHeader( rva, pNTHeader );
    if ( !pSectionHdr ) return 0;
    delta = (INT)(pSectionHdr->VirtualAddress - pSectionHdr->PointerToRawData);
    return (PVOID) ( imageBase + rva - delta );
}
