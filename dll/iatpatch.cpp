#include <windows.h>
#include <stdio.h>
#include "dxwnd.h"
#include "dxwcore.hpp"

static char *NextHint(char *fname) {
    static int WordAlign = 0; // 0 = still unknown
    if(WordAlign == 0) {
        WordAlign = 2; // default
        if(dxw.dwFlags9 & IATBYTEALIGNED) WordAlign = 1; // The Italian Job (crack) BYTE aligned with hint!!!
        if(dxw.dwFlags9 & IATWORDALIGNED) WordAlign = 2; // default case
    }
    for(; *fname; fname++);						// skip function name
    fname++;									// skip string terminator
    while((DWORD)fname % WordAlign) fname++;	// align to specified boundary
    //if((WordAlign == 2) && ((DWORD)fname % sizeof(WORD))) fname++; // align to specified boundary
    return fname;
}

static char *NextString(char *fname) {
    // found in "Al Unser Jr. Arcade Racing"
    for(; *fname; fname++);						// skip function name
    for(; !*fname; fname++);					// skip nulls
    return fname;
}

static BOOL UpdateFunctionAddress(ApiArg, DWORD *OldAddress, DWORD NewAddress) {
    DWORD oldprotect;
    if(!VirtualProtect(OldAddress, 4, PAGE_EXECUTE_READWRITE, &oldprotect)) {
        OutTraceDW("%s: VirtualProtect error %d at %d\n", ApiRef, GetLastError(), __LINE__);
        return FALSE;
    }
    *OldAddress = NewAddress;
    if(!VirtualProtect(OldAddress, 4, oldprotect, &oldprotect)) {
        OutTraceDW("%s: VirtualProtect error %d at %d\n", ApiRef, GetLastError(), __LINE__);
        return FALSE;
    }
    if (!FlushInstructionCache(GetCurrentProcess(), OldAddress, 4)) {
        OutTraceDW("%s: FlushInstructionCache error %d at %d\n", ApiRef, GetLastError(), __LINE__);
        return FALSE;
    }
    return TRUE;
}

void *IATPatchDefault(HMODULE module, DWORD ordinal, char *dll, void *apiproc, const char *apiname, void *hookproc) {
    PIMAGE_NT_HEADERS pnth;
    PIMAGE_IMPORT_DESCRIPTOR pidesc;
    DWORD base, rva;
    PSTR impmodule;
    PIMAGE_THUNK_DATA ptaddr;
    PIMAGE_THUNK_DATA ptname;
    PIMAGE_IMPORT_BY_NAME piname;
    void *org;
    ApiName("IATPatch");
    if(!module) return NULL; // nothing to free
    OutTraceH("%s: module=%#x ordinal=%#x name=%s dll=%s\n", ApiRef, module, ordinal, apiname, dll);
    base = (DWORD)module;
    org = 0; // by default, ret = 0 => API not found
    __try {
        pnth = PIMAGE_NT_HEADERS(PBYTE(base) + PIMAGE_DOS_HEADER(base)->e_lfanew);
        if(!pnth) {
            OutTraceH("%s: ERROR no PNTH at %d\n", ApiRef, __LINE__);
            return 0;
        }
        rva = pnth->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
        if(!rva) {
            OutTraceH("%s: ERROR no RVA at %d\n", ApiRef, __LINE__);
            return 0;
        }
        pidesc = (PIMAGE_IMPORT_DESCRIPTOR)(base + rva);
        while(pidesc->FirstThunk) {
            impmodule = (PSTR)(base + pidesc->Name);
            //OutTraceH("IATPatch: analyze impmodule=%s\n", impmodule);
            //OutTrace("+++ impmodule=%#x name=%%#x ft=%#x oft=%#x fc=%#x\n",  impmodule, pidesc->Name, pidesc->FirstThunk, pidesc->OriginalFirstThunk, pidesc->ForwarderChain);
            char *fname = impmodule;
            if(!lstrcmpi(dll, impmodule)) {
                OutTraceH("%s: dll=%s found at %#x\n", ApiRef, dll, impmodule);
                ptaddr = (PIMAGE_THUNK_DATA)(base + (DWORD)pidesc->FirstThunk);
                ptname = (pidesc->OriginalFirstThunk) ? (PIMAGE_THUNK_DATA)(base + (DWORD)pidesc->OriginalFirstThunk) : NULL;
                while(ptaddr->u1.Function) {
                    // OutTraceH("IATPatch: address=%#x ptname=%#x\n", ptaddr->u1.AddressOfData, ptname);
                    if (ptname) {
                        // examining by function name
                        if(!IMAGE_SNAP_BY_ORDINAL(ptname->u1.Ordinal)) {
                            piname = (PIMAGE_IMPORT_BY_NAME)(base + (DWORD)ptname->u1.AddressOfData);
                            OutTraceH("%s: BYNAME ordinal=%#x address=%#x name=%s hint=%#x\n",
                                      ApiRef, ptaddr->u1.Ordinal, ptaddr->u1.AddressOfData, (char *)piname->Name, piname->Hint);
                            if(!lstrcmpi(apiname, (char *)piname->Name)) break;
                        } else {
                            // OutTraceH("IATPatch: BYORD target=%#x ord=%#x\n", ordinal, IMAGE_ORDINAL32(ptname->u1.Ordinal));
                            if(ordinal && (IMAGE_ORDINAL32(ptname->u1.Ordinal) == ordinal)) { // skip unknow ordinal 0
                                OutTraceH("%s: BYORD ordinal=%#x addr=%#x\n",
                                          ApiRef, ptname->u1.Ordinal, ptaddr->u1.Function);
                                //OutTraceH("IATPatch: BYORD GetProcAddress=%#x\n", GetProcAddress(GetModuleHandle(dll), MAKEINTRESOURCE(IMAGE_ORDINAL32(ptname->u1.Ordinal))));
                                break;
                            }
                        }
                    } else if(!(dxw.dwFlags9 & NOIATSCAN)) {
                        WORD hint;
                        fname = NextHint(fname);
                        hint = *(WORD *)fname;
                        fname += sizeof(WORD);
                        if(!lstrcmpi(apiname, fname)) {
                            OutTraceH("%s: BYSCAN ordinal=%#x hint=%04.4x address=%#x name=%s\n",
                                      ApiRef, ptaddr->u1.Ordinal, hint, ptaddr->u1.AddressOfData, fname);
                            break;
                        }
                    }
                    if (apiproc) {
                        // examining by function addr
                        if(ptaddr->u1.Function == (DWORD)apiproc) break;
                    }
                    ptaddr ++;
                    if (ptname) ptname ++;
                }
                if(ptaddr->u1.Function) {
                    org = (void *)ptaddr->u1.Function;
                    if(org == hookproc) return 0; // already hooked
                    if(!UpdateFunctionAddress(ApiRef, &ptaddr->u1.Function, (DWORD)hookproc)) return 0;
                    OutTraceH("%s hook=%s address=%#x->%#x\n", ApiRef, apiname, org, hookproc);
                    return org;
                }
            }
            pidesc ++;
        }
        if(!pidesc->FirstThunk) {
            OutTraceH("%s: PE unreferenced function %s:%s\n", ApiRef, dll, apiname);
            return 0;
        }
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        OutTraceH("%s: EXCEPTION hook=%s:%s Hook Failed.\n", ApiRef, dll, apiname);
    }
    return org;
}

void *IATPatchSequential(HMODULE module, DWORD ordinal, char *dll, void *apiproc, const char *apiname, void *hookproc) {
    PIMAGE_NT_HEADERS pnth;
    PIMAGE_IMPORT_DESCRIPTOR pidesc;
    DWORD base, rva;
    PSTR impmodule;
    PIMAGE_THUNK_DATA ptaddr;
    PIMAGE_THUNK_DATA ptname;
    PIMAGE_IMPORT_BY_NAME piname;
    void *org;
    ApiName("IATPatchSeq");
    if(!module) return NULL; // nothing to free
    OutTraceH("%s: module=%#x ordinal=%#x name=%s dll=%s\n", ApiRef, module, ordinal, apiname, dll);
    base = (DWORD)module;
    org = 0; // by default, ret = 0 => API not found
    __try {
        pnth = PIMAGE_NT_HEADERS(PBYTE(base) + PIMAGE_DOS_HEADER(base)->e_lfanew);
        if(!pnth) {
            OutTraceH("%s: ERROR no PNTH at %d\n", ApiRef, __LINE__);
            return 0;
        }
        rva = pnth->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
        if(!rva) {
            OutTraceH("%s: ERROR no RVA at %d\n", ApiRef, __LINE__);
            return 0;
        }
        pidesc = (PIMAGE_IMPORT_DESCRIPTOR)(base + rva);
        // move fname pointer to first API name
        char *fname;
        while(pidesc->FirstThunk) {
            impmodule = (PSTR)(base + pidesc->Name);
            fname  = NextString(impmodule);
            pidesc ++;
        }
        pidesc = (PIMAGE_IMPORT_DESCRIPTOR)(base + rva);
        //OutDebugH("IATPatch: first call=%s\n", fname);
        while(pidesc->FirstThunk) {
            impmodule = (PSTR)(base + pidesc->Name);
            if(!lstrcmpi(dll, impmodule)) {
                OutTraceH("%s: dll=%s found at %#x\n", ApiRef, dll, impmodule);
                //OutTraceH("IATPatch: first call=%s\n", fname);
                ptaddr = (PIMAGE_THUNK_DATA)(base + (DWORD)pidesc->FirstThunk);
                ptname = (pidesc->OriginalFirstThunk) ? (PIMAGE_THUNK_DATA)(base + (DWORD)pidesc->OriginalFirstThunk) : NULL;
                while(ptaddr->u1.Function) {
                    // OutTraceH("IATPatch: address=%#x ptname=%#x\n", ptaddr->u1.AddressOfData, ptname);
                    if (ptname) {
                        // examining by function name
                        if(!IMAGE_SNAP_BY_ORDINAL(ptname->u1.Ordinal)) {
                            piname = (PIMAGE_IMPORT_BY_NAME)(base + (DWORD)ptname->u1.AddressOfData);
                            OutTraceH("%s: BYNAME ordinal=%#x address=%#x name=%s hint=%#x\n",
                                      ApiRef, ptaddr->u1.Ordinal, ptaddr->u1.AddressOfData, (char *)piname->Name, piname->Hint);
                            if(!lstrcmpi(apiname, (char *)piname->Name)) break;
                        } else {
                            // OutTraceH("IATPatch: BYORD target=%#x ord=%#x\n", ordinal, IMAGE_ORDINAL32(ptname->u1.Ordinal));
                            if(ordinal && (IMAGE_ORDINAL32(ptname->u1.Ordinal) == ordinal)) { // skip unknow ordinal 0
                                OutTraceH("%s: BYORD ordinal=%#x addr=%#x\n",
                                          ApiRef, ptname->u1.Ordinal, ptaddr->u1.Function);
                                //OutTraceH("IATPatch: BYORD GetProcAddress=%#x\n", GetProcAddress(GetModuleHandle(dll), MAKEINTRESOURCE(IMAGE_ORDINAL32(ptname->u1.Ordinal))));
                                break;
                            }
                        }
                    } else {
                        // OutTraceH("IATPatch: fname=%s\n", fname);
                        if(!lstrcmpi(apiname, fname)) {
                            OutTraceH("%s: BYSCAN ordinal=%#x address=%#x name=%s\n",
                                      ApiRef, ptaddr->u1.Ordinal, ptaddr->u1.AddressOfData, fname);
                            break;
                        }
                        fname = NextString(fname);
                    }
                    if (apiproc) {
                        // examining by function addr
                        if(ptaddr->u1.Function == (DWORD)apiproc) break;
                    }
                    ptaddr ++;
                    if (ptname) ptname ++;
                }
                if(ptaddr->u1.Function) {
                    OutDebugH("%s: hooking %s\n", ApiRef, apiname);
                    org = (void *)ptaddr->u1.Function;
                    if(org == hookproc) return 0; // already hooked
                    if(!UpdateFunctionAddress(ApiRef, &ptaddr->u1.Function, (DWORD)hookproc)) return 0;
                    OutTraceH("%s: hook=%s address=%#x->%#x\n", ApiRef, apiname, org, hookproc);
                    return org;
                }
            } else {
                //OutTraceDW("IATPatch: skip dll=%s first call=%s\n", impmodule, fname);
                // skip dll fnames ...
                ptaddr = (PIMAGE_THUNK_DATA)(base + (DWORD)pidesc->FirstThunk);
                //ptname = (pidesc->OriginalFirstThunk) ? (PIMAGE_THUNK_DATA)(base + (DWORD)pidesc->OriginalFirstThunk) : NULL;
                while(ptaddr->u1.Function) {
                    ptaddr ++;
                    fname = NextString(fname);
                }
            }
            pidesc ++;
        }
        if(!pidesc->FirstThunk) {
            OutTraceH("%s: PE unreferenced function %s:%s\n", ApiRef, dll, apiname);
            return 0;
        }
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        OutTraceH("%s: EXCEPTION hook=%s:%s Hook Failed.\n", ApiRef, dll, apiname);
    }
    return org;
}

// variant of IATPatchSequential to scan IAT sequentially where function names are preceeded by the hint field
// good (enough) to scan "The Black Mirror 1"

void *IATPatchSeqHint(HMODULE module, DWORD ordinal, char *dll, void *apiproc, const char *apiname, void *hookproc) {
    PIMAGE_NT_HEADERS pnth;
    PIMAGE_IMPORT_DESCRIPTOR pidesc;
    DWORD base, rva;
    PSTR impmodule;
    PIMAGE_THUNK_DATA ptaddr;
    PIMAGE_THUNK_DATA ptname;
    PIMAGE_IMPORT_BY_NAME piname;
    void *org;
    ApiName("IATPatchSeqH");
    if(!module) return NULL; // nothing to free
    OutTraceH("%s: module=%#x ordinal=%#x name=%s dll=%s\n", ApiRef, module, ordinal, apiname, dll);
    base = (DWORD)module;
    org = 0; // by default, ret = 0 => API not found
    __try {
        pnth = PIMAGE_NT_HEADERS(PBYTE(base) + PIMAGE_DOS_HEADER(base)->e_lfanew);
        if(!pnth) {
            OutTraceH("%s: ERROR no PNTH at %d\n", ApiRef, __LINE__);
            return 0;
        }
        rva = pnth->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
        if(!rva) {
            OutTraceH("%s: ERROR no RVA at %d\n", ApiRef, __LINE__);
            return 0;
        }
        pidesc = (PIMAGE_IMPORT_DESCRIPTOR)(base + rva);
        // move fname pointer to first API name
        char *fname;
        while(pidesc->FirstThunk) {
            impmodule = (PSTR)(base + pidesc->Name);
            fname  = NextString(impmodule);
            pidesc ++;
        }
        fname += sizeof(WORD); // skip hint field
        pidesc = (PIMAGE_IMPORT_DESCRIPTOR)(base + rva);
        //OutDebugH("IATPatch: first call=%s\n", fname);
        while(pidesc->FirstThunk) {
            impmodule = (PSTR)(base + pidesc->Name);
            if(!lstrcmpi(dll, impmodule)) {
                OutTraceH("%s: dll=%s found at %#x\n", ApiRef, dll, impmodule);
                //OutTraceH("IATPatch: first call=%s\n", fname);
                ptaddr = (PIMAGE_THUNK_DATA)(base + (DWORD)pidesc->FirstThunk);
                ptname = (pidesc->OriginalFirstThunk) ? (PIMAGE_THUNK_DATA)(base + (DWORD)pidesc->OriginalFirstThunk) : NULL;
                while(ptaddr->u1.Function) {
                    // OutTraceH("IATPatch: address=%#x ptname=%#x\n", ptaddr->u1.AddressOfData, ptname);
                    if (ptname) {
                        // examining by function name
                        if(!IMAGE_SNAP_BY_ORDINAL(ptname->u1.Ordinal)) {
                            piname = (PIMAGE_IMPORT_BY_NAME)(base + (DWORD)ptname->u1.AddressOfData);
                            OutTraceH("%s: BYNAME ordinal=%#x address=%#x name=%s hint=%#x\n",
                                      ApiRef, ptaddr->u1.Ordinal, ptaddr->u1.AddressOfData, (char *)piname->Name, piname->Hint);
                            if(!lstrcmpi(apiname, (char *)piname->Name)) break;
                        } else {
                            // OutTraceH("IATPatch: BYORD target=%#x ord=%#x\n", ordinal, IMAGE_ORDINAL32(ptname->u1.Ordinal));
                            if(ordinal && (IMAGE_ORDINAL32(ptname->u1.Ordinal) == ordinal)) { // skip unknow ordinal 0
                                OutTraceH("%s: BYORD ordinal=%#x addr=%#x\n",
                                          ApiRef, ptname->u1.Ordinal, ptaddr->u1.Function);
                                //OutTraceH("IATPatch: BYORD GetProcAddress=%#x\n", GetProcAddress(GetModuleHandle(dll), MAKEINTRESOURCE(IMAGE_ORDINAL32(ptname->u1.Ordinal))));
                                break;
                            }
                        }
                    } else {
                        // OutTraceH("IATPatch: fname=%s\n", fname);
                        if(!lstrcmpi(apiname, fname)) {
                            OutTraceH("%s: BYSCAN ordinal=%#x address=%#x name=%s\n",
                                      ApiRef, ptaddr->u1.Ordinal, ptaddr->u1.AddressOfData, fname);
                            break;
                        }
                        fname = NextString(fname) + sizeof(WORD); // skip hint field
                    }
                    if (apiproc) {
                        // examining by function addr
                        if(ptaddr->u1.Function == (DWORD)apiproc) break;
                    }
                    ptaddr ++;
                    if (ptname) ptname ++;
                }
                if(ptaddr->u1.Function) {
                    OutTraceDW("%s: hooking %s\n", ApiRef, apiname);
                    org = (void *)ptaddr->u1.Function;
                    if(org == hookproc) return 0; // already hooked
                    if(!UpdateFunctionAddress(ApiRef, &ptaddr->u1.Function, (DWORD)hookproc)) return 0;
                    OutTraceH("%s: hook=%s address=%#x->%#x\n", ApiRef, apiname, org, hookproc);
                    return org;
                }
            } else {
                //OutTraceDW("IATPatch: skip dll=%s first call=%s\n", impmodule, fname);
                // skip dll fnames ...
                ptaddr = (PIMAGE_THUNK_DATA)(base + (DWORD)pidesc->FirstThunk);
                //ptname = (pidesc->OriginalFirstThunk) ? (PIMAGE_THUNK_DATA)(base + (DWORD)pidesc->OriginalFirstThunk) : NULL;
                while(ptaddr->u1.Function) {
                    ptaddr ++;
                    fname = NextString(fname) + sizeof(WORD); // skip hint field
                }
            }
            pidesc ++;
        }
        if(!pidesc->FirstThunk) {
            OutTraceH("%s: PE unreferenced function %s:%s\n", ApiRef, dll, apiname);
            return 0;
        }
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        OutTraceH("%s: EXCEPTION hook=%s:%s Hook Failed.\n", ApiRef, dll, apiname);
    }
    return org;
}

// Note: when pidesc->OriginalFirstThunk is NULL, the pidesc->FirstThunk points to an array of
// RVA for imported function names in the PE file, but when the loader loads the program these
// values gets replaced by the function addresses. The only way to retrieve the function names
// after that event is to point to the dll name and get the list of the following strings sequentially
// taking in account that the function names have variable length and are aligned to a WORD
// boundary, so that a practical way to retrieve the next name is in NextHint() subroutine that
// points to next hint value. Next name is 2 bytes (1 WORD) ahead.

// Note (2): the above condition is not always true. The original version of "Al Unser Jr Arcade Racing"
// executable ALUNSER.EXE seems to have all dll names first, then followed by the names of all the dll
// entries, so that it is still possible to retrieve the function name, but a different schema must be used.
// Here we use NextString() subroutine.

// Note (3): another variant of the basic schema, maybe introduced by some UPX tool, is found in
// The Italian Job (cracked exe) where the hints values are not WORD aligned so that there is always
// nly one NULL BYTE as string terminator and no padding. Fortunately, this is quite infrequent.

#ifndef DXW_NOTRACES
void DumpImportTableDefault(HMODULE module) {
    PIMAGE_NT_HEADERS pnth;
    PIMAGE_IMPORT_DESCRIPTOR pidesc;
    DWORD base, rva;
    PSTR impmodule;
    PIMAGE_THUNK_DATA ptaddr;
    PIMAGE_THUNK_DATA ptname;
    PIMAGE_IMPORT_BY_NAME piname;
    ApiName("DumpImportTable");
    base = (DWORD)module;
    OutTraceH("%s: base=%#x\n", ApiRef, base);
    __try {
        pnth = PIMAGE_NT_HEADERS(PBYTE(base) + PIMAGE_DOS_HEADER(base)->e_lfanew);
        if(!pnth) {
            OutTrace("%s: ERROR no pnth at %d\n", ApiRef, __LINE__);
            return;
        }
        rva = pnth->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
        if(!rva) {
            OutTrace("%s: ERROR no rva at %d\n", ApiRef, __LINE__);
            return;
        }
        pidesc = (PIMAGE_IMPORT_DESCRIPTOR)(base + rva);
        while(pidesc->FirstThunk) {
            char *fname;
            impmodule = (PSTR)(base + pidesc->Name);
            OutTrace("%s: ENTRY timestamp=%#x module=%s forwarderchain=%#x\n",
                     ApiRef, pidesc->TimeDateStamp, impmodule, pidesc->ForwarderChain);
            if(pidesc->OriginalFirstThunk)
                ptname = (PIMAGE_THUNK_DATA)(base + (DWORD)pidesc->OriginalFirstThunk);
            else {
                ptname = 0;
                fname = impmodule;
                OutTrace("%s: no PE OFTs - stripped module=%s\n", ApiRef, impmodule);
            }
            ptaddr = (PIMAGE_THUNK_DATA)(base + (DWORD)pidesc->FirstThunk);
            while(ptaddr->u1.Function) {
                char sTraceBuf[256];
                sprintf_s(sTraceBuf, 256, "addr=%#x", ptaddr->u1.Function);
                ptaddr ++;
                if(ptname) {
                    if(!IMAGE_SNAP_BY_ORDINAL(ptname->u1.Ordinal)) {
                        piname = (PIMAGE_IMPORT_BY_NAME)(base + (DWORD)ptname->u1.AddressOfData);
                        OutTrace("%s hint=%04.4x name=\"%s\"\n", sTraceBuf, piname->Hint, piname->Name);
                        ptname ++;
                    } else
                        OutTrace("%s ???\n", sTraceBuf);
                } else {
                    WORD hint;
                    fname = NextHint(fname);
                    hint = *(WORD *)fname;
                    fname += sizeof(WORD);
                    OutTrace("%s hint=%04.4X name=\"%s\"\n", sTraceBuf, hint, fname);
                }
            }
            OutTrace("*** EOT ***\n");
            pidesc ++;
        }
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        OutTrace("%s: EXCEPTION\n", ApiRef);
    }
    return;
}

void DumpImportTableSequential(HMODULE module) {
    PIMAGE_NT_HEADERS pnth;
    PIMAGE_IMPORT_DESCRIPTOR pidesc;
    DWORD base, rva;
    PSTR impmodule;
    PIMAGE_THUNK_DATA ptaddr;
    PIMAGE_THUNK_DATA ptname;
    PIMAGE_IMPORT_BY_NAME piname;
    ApiName("DumpImportTableSequential");
    base = (DWORD)module;
    OutTrace("%s: base=%#x\n", ApiRef, base);
    __try {
        pnth = PIMAGE_NT_HEADERS(PBYTE(base) + PIMAGE_DOS_HEADER(base)->e_lfanew);
        if(!pnth) {
            OutTrace("%s: ERROR no pnth at %d\n", ApiRef, __LINE__);
            return;
        }
        rva = pnth->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
        if(!rva) {
            OutTrace("%s: ERROR no rva at %d\n", ApiRef, __LINE__);
            return;
        }
        pidesc = (PIMAGE_IMPORT_DESCRIPTOR)(base + rva);
        OutTrace("%s: pidesc=%#x\n", ApiRef, pidesc);
        char *fname;
        PIMAGE_IMPORT_DESCRIPTOR savepidesc = pidesc;
        while(pidesc->FirstThunk) {
            impmodule = (PSTR)(base + pidesc->Name);
            fname  = NextString(impmodule);
            pidesc ++;
        }
        pidesc = savepidesc;
        OutTrace("%s: no PE OFTs - first call=%s\n", ApiRef, fname);
        while(pidesc->FirstThunk) {
            impmodule = (PSTR)(base + pidesc->Name);
            OutTrace("%s: ENTRY timestamp=%#x module=%s forwarderchain=%#x\n",
                     ApiRef, pidesc->TimeDateStamp, impmodule, pidesc->ForwarderChain);
            if(pidesc->OriginalFirstThunk)
                ptname = (PIMAGE_THUNK_DATA)(base + (DWORD)pidesc->OriginalFirstThunk);
            else {
                ptname = 0;
                OutTrace("%s: no PE OFTs - stripped module=%s\n", ApiRef, impmodule);
            }
            ptaddr = (PIMAGE_THUNK_DATA)(base + (DWORD)pidesc->FirstThunk);
            while(ptaddr->u1.Function) {
                char sTraceBuf[256];
                sprintf_s(sTraceBuf, 256, "addr=%#x", ptaddr->u1.Function);
                ptaddr ++;
                if(ptname) {
                    if(!IMAGE_SNAP_BY_ORDINAL(ptname->u1.Ordinal)) {
                        piname = (PIMAGE_IMPORT_BY_NAME)(base + (DWORD)ptname->u1.AddressOfData);
                        sprintf_s(sTraceBuf, 256, "%s hint=%04.4x name=\"%s\"", sTraceBuf, piname->Hint, piname->Name);
                        ptname ++;
                    }
                } else {
                    sprintf_s(sTraceBuf, 256, "%s name=\"%s\"", sTraceBuf, fname);
                    fname = NextString(fname);
                }
                strcat_s(sTraceBuf, 256, "\n");
                OutTrace(sTraceBuf);
            }
            OutTrace("*** EOT ***\n");
            pidesc ++;
        }
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        OutTrace("%s: EXCEPTION\n", ApiRef);
    }
    return;
}

void DumpImportTableSeqHint(HMODULE module) {
    PIMAGE_NT_HEADERS pnth;
    PIMAGE_IMPORT_DESCRIPTOR pidesc;
    DWORD base, rva;
    PSTR impmodule;
    PIMAGE_THUNK_DATA ptaddr;
    PIMAGE_THUNK_DATA ptname;
    PIMAGE_IMPORT_BY_NAME piname;
    ApiName("DumpImportTableSeqHint");
    base = (DWORD)module;
    OutTrace("%s: base=%#x\n", ApiRef, base);
    __try {
        pnth = PIMAGE_NT_HEADERS(PBYTE(base) + PIMAGE_DOS_HEADER(base)->e_lfanew);
        if(!pnth) {
            OutTrace("%s: ERROR no pnth at %d\n", ApiRef, __LINE__);
            return;
        }
        rva = pnth->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
        if(!rva) {
            OutTrace("%s: ERROR no rva at %d\n", ApiRef, __LINE__);
            return;
        }
        pidesc = (PIMAGE_IMPORT_DESCRIPTOR)(base + rva);
        OutTrace("%s: pidesc=%#x\n", ApiRef, pidesc);
        char *fname;
        PIMAGE_IMPORT_DESCRIPTOR savepidesc = pidesc;
        while(pidesc->FirstThunk) {
            impmodule = (PSTR)(base + pidesc->Name);
            fname  = NextString(impmodule);
            pidesc ++;
        }
        fname += sizeof(WORD); // skip
        pidesc = savepidesc;
        OutTrace("%s: no PE OFTs - first call=%s\n", ApiRef, fname);
        while(pidesc->FirstThunk) {
            impmodule = (PSTR)(base + pidesc->Name);
            OutTrace("%s: ENTRY timestamp=%#x module=%s forwarderchain=%#x\n",
                     ApiRef, pidesc->TimeDateStamp, impmodule, pidesc->ForwarderChain);
            if(pidesc->OriginalFirstThunk)
                ptname = (PIMAGE_THUNK_DATA)(base + (DWORD)pidesc->OriginalFirstThunk);
            else {
                ptname = 0;
                OutTrace("%s: no PE OFTs - stripped module=%s\n", ApiRef, impmodule);
            }
            ptaddr = (PIMAGE_THUNK_DATA)(base + (DWORD)pidesc->FirstThunk);
            while(ptaddr->u1.Function) {
                char sTraceBuf[256];
                sprintf_s(sTraceBuf, 256, "addr=%#x", ptaddr->u1.Function);
                ptaddr ++;
                if(ptname) {
                    if(!IMAGE_SNAP_BY_ORDINAL(ptname->u1.Ordinal)) {
                        piname = (PIMAGE_IMPORT_BY_NAME)(base + (DWORD)ptname->u1.AddressOfData);
                        sprintf_s(sTraceBuf, 256, "%s hint=%04.4x name=\"%s\"", sTraceBuf, piname->Hint, piname->Name);
                        ptname ++;
                    }
                } else {
                    sprintf_s(sTraceBuf, 256, "%s name=\"%s\"", sTraceBuf, fname);
                    fname = NextString(fname);
                }
                strcat_s(sTraceBuf, 256, "\n");
                OutTrace(sTraceBuf);
            }
            OutTrace("*** EOT ***\n");
            pidesc ++;
        }
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        OutTraceDW("%s: EXCEPTION\n", ApiRef);
    }
    return;
}
#endif // DXW_NOTRACES

static char *stristr(char *str1, const char *str2) {
    char *p1 = str1 ;
    const char *p2 = str2 ;
    char *r = *p2 == 0 ? str1 : 0 ;
    while(*p1 != 0 && *p2 != 0) {
        if(tolower(*p1) == tolower(*p2)) {
            if(r == 0) r = p1;
            p2++ ;
        } else {
            p2 = str2;
            if(tolower(*p1) == tolower(*p2)) {
                r = p1;
                p2++ ;
            } else
                r = 0;
        }
        p1++ ;
    }
    return *p2 == 0 ? r : 0 ;
}

BOOL IsIATSequential(HMODULE module) {
    PIMAGE_NT_HEADERS pnth;
    PIMAGE_IMPORT_DESCRIPTOR pidesc;
    DWORD base, rva;
    ApiName("IsIATSequential");
    base = (DWORD)module;
    __try {
        pnth = PIMAGE_NT_HEADERS(PBYTE(base) + PIMAGE_DOS_HEADER(base)->e_lfanew);
        if(!pnth) {
            OutTrace("%s: ERROR no pnth at %d\n", ApiRef, __LINE__);
            return FALSE;
        }
        rva = pnth->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
        if(!rva) {
            OutTrace("%s: ERROR no rva at %d\n", ApiRef, __LINE__);
            return FALSE;
        }
        pidesc = (PIMAGE_IMPORT_DESCRIPTOR)(base + rva);
        //OutTrace("IsIATSequential: pidesc=%#x\n", pidesc);
        char *fname = (PSTR)(base + pidesc->Name);
        // first string should be a DLL in both cases
        if(!stristr(fname, ".DLL")) {
            OutTrace("%s: ERROR no .DLL at %d\n", ApiRef, __LINE__);
            return FALSE;
        }
        // skip first string
        for(; *fname; fname++);
        for(; !*fname; fname++);
        // if second string is another DLL it is sequential, otherwise not.
        //OutDebugH("IsIATSequential: second entry=%s\n", fname);
        return (BOOL)stristr(fname, ".DLL");
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        OutTraceDW("%s: EXCEPTION\n", ApiRef);
    }
    return FALSE;
}

