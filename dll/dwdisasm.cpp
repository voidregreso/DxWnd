#define _WIN32_WINNT 0x0600
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_DEPRECATE 1

#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <psapi.h>
#include <dbghelp.h>
#include "dxwnd.h"
#include "dxwcore.hpp"
#include "disasm.h"
#include "dwdisasm.h"
#include "syslibs.h"

Geterrwarnmessage_Type pGeterrwarnmessage;
Preparedisasm_Type pPreparedisasm;
Finishdisasm_Type pFinishdisasm;
Disasm_Type pDisasm;

HMODULE LoadDisasm() {
    HMODULE disasmlib;
    disasmlib = (*pLoadLibraryA)("disasm.dll");
    if(!disasmlib) {
        char msg[81];
        OutTraceDW("DXWND: Load lib=\"%s\" failed err=%d\n", "disasm.dll", GetLastError());
        switch (GetLastError()) {
        case ERROR_FILE_NOT_FOUND:
        case ERROR_MOD_NOT_FOUND:
            strcpy_s(msg, 80, "Library disasm.dll not found\nplease install it");
            break;
        default:
            sprintf_s(msg, 80, "DXWND: Load lib=\"%s\" failed err=%d", "disasm.dll", GetLastError());
            break;
        }
        MessageBox(NULL, msg, "DxWnd error", 0);
        return NULL;
    }
    pGeterrwarnmessage = (Geterrwarnmessage_Type)(*pGetProcAddress)(disasmlib, "Geterrwarnmessage");
    pPreparedisasm = (Preparedisasm_Type)(*pGetProcAddress)(disasmlib, "Preparedisasm");
    pFinishdisasm = (Finishdisasm_Type)(*pGetProcAddress)(disasmlib, "Finishdisasm");
    pDisasm = (Disasm_Type)(*pGetProcAddress)(disasmlib, "Disasm");
    //OutTraceDW("DXWND: Load disasm.dll ptrs=%#x,%#x,%#x,%#x\n", pGeterrwarnmessage, pPreparedisasm, pFinishdisasm, pDisasm);
    return disasmlib;
}
