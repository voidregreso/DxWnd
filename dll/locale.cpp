#define _WIN32_WINNT 0x0600
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_DEPRECATE 1

#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <psapi.h>
#include <dbghelp.h>
#include <locale.h>
#include "dxwnd.h"
#include "dxwcore.hpp"
#include "shareddc.hpp"
#include "dxhook.h"
#include "glhook.h"
#include "glidehook.h"
#include "msvfwhook.h"
#include "syslibs.h"
#include "dxhelper.h"
#include "Ime.h"
#include "Winnls32.h"
#include "Mmsystem.h"
#include "disasm.h"
#include "dwdisasm.h"
#include "MinHook.h"

// from WinNls.h:
// #define CTRY_JAPAN 81
// #define CAL_JAPAN                      3      // Japanese Emperor Era calendar
// #define LGRPID_JAPANESE              0x0007   // Japanese
// 	dxw.CodePage = 932 for Shift-JIS japanese (0x3A4);

void HackLocale() {
    HMODULE hDLL;
    FARPROC symbol;
    OutTraceLOC("Current codepage: %d\n", _get_current_locale()->locinfo->lc_codepage);
    OutTraceLOC("Current locale: %s\n", setlocale(LC_ALL, NULL));
    OutTraceLOC("custom country=%d lang=%d codepage=%d\n", dxw.Country, dxw.Language, dxw.CodePage);
    hDLL = (*pLoadLibraryA)("ntdll.dll");
    symbol = (*pGetProcAddress)(hDLL, "NlsAnsiCodePage");
    if (symbol) {
        OutTraceLOC("NlsAnsiCodePage: %d -> %d\n", *(DWORD *)(DWORD_PTR)symbol, dxw.CodePage);
        *(DWORD *)(DWORD_PTR)symbol = dxw.CodePage;
        //symbol = (*pGetProcAddress)(hDLL, "NlsAnsiCodePage");
        OutTraceLOC("NlsAnsiCodePage: %d\n", *(DWORD *)(DWORD_PTR)symbol);
    }
}

