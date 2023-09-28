#define _WIN32_WINNT 0x0600
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <assert.h>
#include "dxwnd.h"
#include "dxwcore.hpp"
#include "dxwlocale.h"
#include "dxhelper.h"

DWORD nTlsIndex = TLS_OUT_OF_INDEXES;

typedef struct {
    LONG  bInternalLockCreateAtom;
    int	  bNtleaAtomInvoked;
    HANDLE hHeap;
} Settings;

Settings settings = {0, 0, 0};

static LPCSTR szNtleaWndAscData = "NtleaWndAscData"; // CreateGlobalAtom -> ebx
static LPCSTR szNtleaDlgAscData = "NtleaDlgAscData";

// more controls see : http://msdn.microsoft.com/en-us/library/windows/desktop/ms633574(v=vs.85).aspx
char const *SystemClassNameA[] = {
    "BUTTON", "COMBOBOX", "ComboLBox", /*"#32770",*/ "EDIT", "LISTBOX", "MDICLIENT", "RichEdit", "RICHEDIT_CLASS",
    "SCROLLBAR", "STATIC", "SysTreeView32", "SysListView32", "SysAnimate32", "SysHeader32", "tooltips_class32",
    //	"SysTabControl32", "ToolbarWindow32", "ComboBoxEx32", "SysDateTimePick32", "SysMonthCal32", "ReBarWindow32",
    //	"msctls_progress32", "msctls_trackbar32", "msctls_statusbar32", "msctls_updown32", "msctls_hotkey32",
    /*NULL, */
};
wchar_t const *SystemClassNameW[] = {
    L"BUTTON", L"COMBOBOX", L"ComboLBox", /*L"32770",*/ L"EDIT", L"LISTBOX", L"MDICLIENT", L"RichEdit", L"RICHEDIT_CLASS",
    L"SCROLLBAR", L"STATIC", L"SysTreeView32", L"SysListView32", L"SysAnimate32", L"SysHeader32", L"tooltips_class32",
    //	L"SysTabControl32", L"ToolbarWindow32", L"ComboBoxEx32", L"SysDateTimePick32", L"SysMonthCal32", L"ReBarWindow32",
    //	L"msctls_progress32", L"msctls_trackbar32", L"msctls_statusbar32", L"msctls_updown32", L"msctls_hotkey32",
    /*NULL, */
};


static int CheckWindowStyle(HWND hWnd, DWORD type/*ebx*/) {
    LONG_PTR n = GetWindowLongPtrW(hWnd, GWL_STYLE);
    // window no needs conversion ??
    if (n == 0)
        return (0);
    else if (n == /*0x84C820E4*/(WS_POPUP | WS_CLIPSIBLINGS | WS_BORDER | WS_DLGFRAME | WS_SYSMENU |
                                 WS_EX_RTLREADING | WS_EX_TOOLWINDOW | WS_EX_MDICHILD | WS_EX_TRANSPARENT | WS_EX_NOPARENTNOTIFY))
        return (0);
    else if (!(n & (WS_EX_ACCEPTFILES | WS_EX_TRANSPARENT)))
        return (0);
    else if (!type && (n & WS_EX_CLIENTEDGE))
        return (0);
    else if (n & WS_EX_MDICHILD)
        return (0);
    // other case :
    return (-1); // xor ebx, ebx !
}

__inline NTLEA_WND_ASC_DATA *CheckProp(HWND hWnd) {
    NTLEA_WND_ASC_DATA *p = (NTLEA_WND_ASC_DATA *)GetPropA(hWnd, szNtleaWndAscData);
    if (!p) {
        OutTraceE("CheckProp: Unacceptable Empty Window Prop Detected, Force Exit.\n");
        exit(0);
    }
    return p;
}

__inline LRESULT CallProcAddress(LPVOID lpProcAddress, HWND hWnd, HWND hMDIClient,
                                 BOOL bMDIClientEnabled, INT uMsg, WPARAM wParam, LPARAM lParam) {
    typedef LRESULT(WINAPI * fnWNDProcAddress)(HWND, int, WPARAM, LPARAM);
    typedef LRESULT(WINAPI * fnMDIProcAddress)(HWND, HWND, int, WPARAM, LPARAM);
    // MDI or not ???
    return (bMDIClientEnabled) ? ((fnMDIProcAddress)(DWORD_PTR)lpProcAddress)(hWnd, hMDIClient, uMsg, wParam, lParam)
           : ((fnWNDProcAddress)(DWORD_PTR)lpProcAddress)(hWnd, uMsg, wParam, lParam);
}

__inline NTLEA_TLS_DATA *GetTlsValueInternal(void) {
    DWORD n = GetLastError();
    if(nTlsIndex == TLS_OUT_OF_INDEXES) nTlsIndex = TlsAlloc();
    NTLEA_TLS_DATA *p = (NTLEA_TLS_DATA *)TlsGetValue(nTlsIndex);
    SetLastError(n); // thus the tlsgetvalue won't affect the env
    if (!p) {
        p = (NTLEA_TLS_DATA *)AllocateZeroedMemory(sizeof(NTLEA_TLS_DATA));
        TlsSetValue(nTlsIndex, p);
        for (int i = 0; i < MAXSYSCLASSDESC; ++i) {
            WNDCLASSA wndclassa;
            if (GetClassInfoA(NULL, SystemClassNameA[i], &wndclassa))
                p->SystemClassDesc[i].AnsiSystemClassProc = wndclassa.lpfnWndProc;
            WNDCLASSW wndclassw;
            if (GetClassInfoW(NULL, SystemClassNameW[i], &wndclassw))
                p->SystemClassDesc[i].UnicodeSystemClassProc = wndclassw.lpfnWndProc;
            //	ntprintfA(256, 1, "info: %s - %p %p\n", SystemClassNameA[i], wndclassa.lpfnWndProc, wndclassw.lpfnWndProc);
        }
        SetLastError(0); // also restore the errorstate !
    }
    return p;
}

static void CreateGlobalAtom(void) {
    while (!InterlockedCompareExchange(&settings.bInternalLockCreateAtom, 1/*ecx*/, 0/*eax*/))
        Sleep(0);
    if (!settings.bNtleaAtomInvoked) {
        if (!GlobalFindAtomA(szNtleaWndAscData))
            GlobalAddAtomA(szNtleaWndAscData);
        ++settings.bNtleaAtomInvoked;
        if (!GlobalFindAtomA(szNtleaDlgAscData))
            GlobalAddAtomA(szNtleaDlgAscData);
    }
    InterlockedDecrement(&settings.bInternalLockCreateAtom);
}

typedef LRESULT(CALLBACK *CALLPROC)(WNDPROC, HWND, UINT, WPARAM, LPARAM);
#define WM_UNKNOWN				0x43E

static LRESULT CALLBACK TopLevelWindowProcEx(CALLPROC DefaultCallWindowProc, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    LPCSTR lpAnsiWindowName = NULL, lpAnsiClassName = NULL;
    CHAR CharBuffer[2]; // for unicode conversion -> local
    CHAR ClassNameBuffer[MAX_PATH];
    int type = 0;
    // debug log - to delete
    OutDebugLOC("TopLevelWindowProcEx: hwnd=%x(%s) msg=%x(%s) wparam=%x lparam=%x\n",
                hWnd, IsWindowUnicode(hWnd) ? "WIDE" : "ASCII", uMsg, ExplainWinMessage(uMsg), wParam, lParam);
    NTLEA_WND_ASC_DATA *wndasc = CheckProp(hWnd);
    WNDPROC PrevWndProc = wndasc->PrevAnsiWindowProc;
    ++GetTlsValueInternal()->InternalCall; // <---
    //	char classname[256]; GetClassNameA(hWnd, classname, sizeof(classname));
    //	if (lstrcmpiA(classname, "TRadioButton") == 0) {
    //	ntprintfA(256, 1, "%s: proc-%p hwnd=%p, msg=%04x, wParam=%d, lParam=%d\n", __FUNCTION__, PrevWndProc, hWnd, uMsg, wParam, lParam);
    //	}
    switch (uMsg) {
    case WM_CREATE: // L103
    case WM_NCCREATE: { // general case !!
        if (lParam) {
            CREATEWNDEX *p = (CREATEWNDEX *)AllocateHeapInternal(sizeof(CREATEWNDEX));
            memcpy(p, (LPVOID)lParam, sizeof(CREATEWNDEX));
            if (p->lpWindowName)   // L103
                p->lpWindowName = (LPVOID)(lpAnsiWindowName = WideCharToMultiByteInternal((LPCWSTR)p->lpWindowName));
            if ((DWORD_PTR)p->lpClassName & WM_CLASSMASK)   // L101
                p->lpClassName = (LPVOID)(lpAnsiClassName = WideCharToMultiByteInternal((LPCWSTR)p->lpClassName));
            // LN999
            LRESULT hr = CallWindowProcA(PrevWndProc, hWnd, uMsg, wParam, (LPARAM)p);
            // LN995
            if (lpAnsiWindowName) FreeStringInternal((LPVOID)lpAnsiWindowName);
            if (lpAnsiClassName) FreeStringInternal((LPVOID)lpAnsiClassName);
            if (p/*ebx*/) FreeStringInternal(p);
            return (hr);
        }
    }
    break;
    case WM_MDICREATE: { // LN106
        if (lParam) {
            CREATEMDIWND *p = (CREATEMDIWND *)AllocateHeapInternal(sizeof(CREATEMDIWND));
            memcpy(p, (LPVOID)lParam, sizeof(CREATEMDIWND));
            if (p->szTitle)
                p->szTitle = lpAnsiWindowName = WideCharToMultiByteInternal((LPCWSTR)p->szTitle);
            if ((DWORD_PTR)p->szClass & WM_CLASSMASK)
                p->szClass = lpAnsiClassName = WideCharToMultiByteInternal((LPCWSTR)p->szClass);
            // LN999
            LRESULT hr = CallWindowProcA(PrevWndProc, hWnd, uMsg, wParam, (LPARAM)p);
            // LN995
            if (lpAnsiWindowName) FreeStringInternal((LPVOID)lpAnsiWindowName);
            if (lpAnsiClassName) FreeStringInternal((LPVOID)lpAnsiClassName);
            if (p/*ebx*/) FreeStringInternal(p);
            return (hr);
        }
    }
    break;
    case EM_GETLINE: { // LN124
        int siz = *(short *)(DWORD_PTR)lParam + 1;
        LPSTR lParamA = (LPSTR)AllocateZeroedMemory(siz * sizeof(wchar_t));
        *(short *)(DWORD_PTR)lParam = (short)(siz - 1);
        int len = (int)CallWindowProcA(PrevWndProc, hWnd, uMsg, wParam, (LPARAM)lParamA);
        if (len) // if success :
            len = MultiByteToWideChar(dxw.CodePage, 0, lParamA, len + 1, (LPWSTR)lParam, siz); // --- bugfixed
        if (!len && lParam) *(LPWSTR)lParam = L'\0'; // handle failed case
        else if (len) --len; // report not-including null-terminate string !
        if (lParamA) FreeStringInternal((LPVOID)lParamA);
        return len;
    }//	break;
    case WM_GETFONT: {
        LRESULT hr = CallWindowProcA(PrevWndProc, hWnd, uMsg, wParam, lParam);
        return (hr) ? (hr) : (LRESULT)GetStockObject(SYSTEM_FONT);
    }//	break;
    case EM_REPLACESEL: // LN113
    case WM_SETTEXT: // LN113
    case WM_SETTINGCHANGE: // LN113
    case WM_DEVMODECHANGE: { // LN113
        LPCSTR lParamA = lParam ? WideCharToMultiByteInternal((LPCWSTR)lParam) : NULL;
        //	ntprintfA(256, 1, "1. W(%S) -> A(%s)", (LPCWSTR)lParam, lParamA);
        LRESULT hr = CallWindowProcA(PrevWndProc, hWnd, uMsg, wParam, (LPARAM)lParamA); // lParamA null also requied ???
        // LN999 LN995
        if (lParamA) FreeStringInternal((LPVOID)lParamA);
        return hr;
    }//	break;
    case WM_GETTEXTLENGTH: { // LN127
        LRESULT len = CallWindowProcA(PrevWndProc, hWnd, WM_GETTEXTLENGTH, 0, 0);
        if (len > 0) {
            GetTlsValueInternal()->InternalCall++;
            LPSTR lParamA = (LPSTR)AllocateZeroedMemory((len + 1) * sizeof(wchar_t));
            CallWindowProcA(PrevWndProc, hWnd, WM_GETTEXT, (len + 1) * sizeof(wchar_t), (LPARAM)lParamA);
            len = MultiByteToWideChar(dxw.CodePage, 0, lParamA, -1, NULL, 0) - 1;
            // LN995
            if (lParamA/*ebx*/) FreeStringInternal((LPVOID)lParamA);
        }
        return len;
    }//	break;
    case WM_GETTEXT: // LN114
    case WM_UNKNOWN: { // LN114
        if (IsBadWritePtr((LPVOID)lParam, 1)) {
            --GetTlsValueInternal()->InternalCall;
            return (0);
        } else {
            LRESULT len = CallWindowProcA(PrevWndProc, hWnd, WM_GETTEXTLENGTH, 0, 0);
            if (len == 0)
                *((LPWSTR)lParam) = L'\0';
            else { // L116
                GetTlsValueInternal()->InternalCall++;
                LPSTR lParamA = (LPSTR)AllocateZeroedMemory((len + 1) * sizeof(wchar_t));
                CallWindowProcA(PrevWndProc, hWnd, uMsg, (len + 1) * sizeof(wchar_t), (LPARAM)lParamA);
                if (uMsg == WM_UNKNOWN) wParam = len + 1;
                len = MultiByteToWideChar(dxw.CodePage, 0, lParamA, -1, (LPWSTR)lParam, (int)wParam) - 1;
                if (len > 0) {
                    // L997
                    if (lParamA) FreeStringInternal((LPVOID)lParamA);
                } else {
                    // L119
                    if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
                        *((LPWSTR)lParam + wParam - 1) = L'\0';
                    else {
                        // L120
                        *((LPWSTR)lParam) = L'\0';
                    }
                    // LN994
                    FreeStringInternal((LPVOID)lParamA);
                }
            }
            return len;
        }
    }//	break;
    case WM_IME_CHAR: // LN109
    case WM_CHAR: { // LN109
        if ((wchar_t)wParam > 0x7F) {
            WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)&wParam, 1, CharBuffer, 2, NULL, NULL);
            // here we exchange the order right ?
            *((char *)&wParam + 0) = CharBuffer[1];
            *((char *)&wParam + 1) = CharBuffer[0];
            //	wParam = (CharBuffer[1] << 8) | (CharBuffer[0] << 0);
        }
    }
    break;
    case WM_NOTIFYFORMAT: { // LN121
        --GetTlsValueInternal()->InternalCall; // fix
        GetClassNameA(hWnd, ClassNameBuffer, MAX_PATH);
        if (lstrcmpiA(ClassNameBuffer, "SysTreeView32") == 0) {
            DWORD_PTR n = GetWindowLongPtrW(hWnd, DWLP_MSGRESULT/*0*/);
            if (n && *(LPBYTE)(n + 0x10) == 1) { // what ???
                *(LPBYTE)(n + 0x10) &= -2; //
            }
        }
        // L122
        return (lParam == NF_QUERY) ? NFR_ANSI : (0);
    }//	break;
    case WM_NCDESTROY: { // check steps move out !!
        LRESULT hr = CallWindowProcA(PrevWndProc, hWnd, uMsg, wParam, lParam);
        FreeStringInternal((LPVOID)GetPropA(hWnd, szNtleaWndAscData));
        RemovePropA(hWnd, szNtleaWndAscData);
        return hr;
    }//	break;
    // --------------------------------
    case LB_GETTEXTLEN: { // LN131
        int ret = CheckWindowStyle(hWnd, 1); // 1, inc ebx
        if (ret != -1) {
            LRESULT len = CallWindowProcA(PrevWndProc, hWnd, LB_GETTEXTLEN, wParam, 0);
            if (len > 0) {
                GetTlsValueInternal()->InternalCall++;
                LPSTR lParamA = (LPSTR)AllocateZeroedMemory((len + 1) * sizeof(wchar_t));
                CallWindowProcA(PrevWndProc, hWnd, LB_GETTEXT, wParam, (LPARAM)lParamA);
                len = MultiByteToWideChar(dxw.CodePage, 0, lParamA, -1, NULL, 0) - 1;
                // LN995
                if (lParamA/*ebx*/) FreeStringInternal((LPVOID)lParamA);
            }
            return len;
        }
    }
    break;
    case LB_GETTEXT: { // LN110
        type = 1;
    }//	break;
    case CB_GETLBTEXT: { // LN129
        int ret = CheckWindowStyle(hWnd, type); // 0
        if (ret != -1) {
            LPSTR lParamA = (LPSTR)AllocateZeroedMemory(MAX_PATH * sizeof(wchar_t));
            int len = (int)CallWindowProcA(PrevWndProc, hWnd, uMsg, wParam, (LPARAM)lParamA);
            if (len) // if success :
                len = MultiByteToWideChar(dxw.CodePage, 0, lParamA, len + 1, (LPWSTR)lParam, MAX_PATH); // --- bugfixed
            if (!len && lParam) *(LPWSTR)lParam = L'\0'; // handle failed case !
            else if (len) --len; // report not-including null-terminate string !
            if (lParamA/*ebx*/) FreeStringInternal((LPVOID)lParamA);
            return len;
        }
    }
    break;
    case LB_FINDSTRINGEXACT: // LN104
    case LB_ADDSTRING: // LN104
    case LB_INSERTSTRING: // L104
    case LB_SELECTSTRING: // L104
    case LB_DIR: // L104
    case LB_FINDSTRING: // L104
    case LB_ADDFILE: // L104
        type = 1; // ebx = 1
    //	break;
    case CB_FINDSTRINGEXACT: // LN105
    case CB_ADDSTRING: // LN105
    case CB_INSERTSTRING: // LN105
    case CB_SELECTSTRING: // LN105
    case CB_DIR: // LN105
    case CB_FINDSTRING: { // LN105
        int ret = CheckWindowStyle(hWnd, type); // ebx = 0 / 1
        if (ret != -1) { // send op
            // LN113
            LPCSTR lParamA = lParam ? WideCharToMultiByteInternal((LPCWSTR)lParam) : NULL;
            // LN999
            LRESULT hr = CallWindowProcA(PrevWndProc, hWnd, uMsg, wParam, (LPARAM)lParamA);
            // LN995
            if (lParamA/*ebx*/) FreeStringInternal((LPVOID)lParamA);
            return hr;
        } else { // recv op
            // LN110
            ret = CheckWindowStyle(hWnd, type + 1); // ebx = 1 / 2
            if (ret != -1) {
                // <-- ebx
                LPSTR lParamA = (LPSTR)AllocateZeroedMemory(MAX_PATH * sizeof(wchar_t));
                // LN999
                int len = (int)CallWindowProcA(PrevWndProc, hWnd, uMsg, wParam, (LPARAM)lParamA);
                if (len) // if success :
                    len = MultiByteToWideChar(dxw.CodePage, 0, lParamA, len + 1, (LPWSTR)lParam, MAX_PATH); // --- bugfixed
                if (!len && lParam) *(LPWSTR)lParam = L'\0'; // L111
                else if (len) --len; // report not-including null-terminate string !
                // LN995
                if (lParamA) FreeStringInternal((LPVOID)lParamA);
                return len;
            }
        }
    }
    break;
    default: // LN100
        break;
    }
    // ---------
    return DefaultCallWindowProc(PrevWndProc, hWnd, uMsg, wParam, lParam);
}

static LRESULT CALLBACK TopLevelWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // for normal custom window case : other messages also needs handled by named-controls (children)
    return TopLevelWindowProcEx(pCallWindowProcA, hWnd, uMsg, wParam, lParam);
}

static LRESULT CALLBACK TopLevelSimpleProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // for predefined window case : other messages won't pass to named-controls (children)
    return TopLevelWindowProcEx(pCallWindowProcA, hWnd, uMsg, wParam, lParam);
}

static void SetTopWindowProc(NTLEA_TLS_DATA *p, HWND hWnd, LONG_PTR TopLevelWndProc) {
    OutTraceLOC("SetTopWindowProc: hwnd=%#x\n", hWnd);
    NTLEA_WND_ASC_DATA *wndasc = (NTLEA_WND_ASC_DATA *)AllocateZeroedMemory(sizeof(NTLEA_WND_ASC_DATA));
    ++p->InternalCall;
    WNDPROC wndproca = (WNDPROC)GetWindowLongPtrA(hWnd, GWLP_WNDPROC);
    OutDebugLOC("SetTopWindowProc: wndproca=%#x\n", wndproca);
    if (wndproca) {
        wndasc->PrevAnsiWindowProc = wndproca;
        (*pSetWindowLongW)(hWnd, GWLP_WNDPROC, TopLevelWndProc);
        SetPropA(hWnd, szNtleaWndAscData, (HANDLE)wndasc); // save previous wndproc
    } else // no wndproc, no needs ???
        FreeStringInternal(wndasc);
    OutDebugLOC("SetTopWindowProc: DONE\n");
}

static void CbtHookWindowProc(NTLEA_TLS_DATA *p, HWND hWnd) {
    CreateGlobalAtom();
    // initialize custom window proc storage :
    if (!GetPropA(hWnd, szNtleaWndAscData)) {
        OutTraceLOC("CbtHookWindowProc: no prop for hwnd=%#x\n", hWnd);
        SetTopWindowProc(p, hWnd, (LONG_PTR)TopLevelWindowProc);
    }
}

static LRESULT CbtHookProc(NTLEA_TLS_DATA *p, HHOOK hhook, int nCode, WPARAM wParam, LPARAM lParam) {
    // we filter msg we don't take care :
    if (nCode != HCBT_CREATEWND || // or a unicode dialog won't cause hook !!
            (p->CurrentCallType != CT_CREATE_WINDOW && IsWindowUnicode((HWND)wParam)))
        return CallNextHookEx(hhook, nCode, wParam, lParam);
    // for createwindow ansi window :
    OutTraceLOC("CbtHookProc: hooking hwnd=%#x\n", wParam);
    WCHAR ClassNameBuffer[MAX_PATH];
    HWND hwnd = (HWND)wParam;
    if (GetClassNameW(hwnd, ClassNameBuffer, sizeof(ClassNameBuffer)) > 0) {
        // not a speicified dialog, we have to hook the whole custom window msg proc!!
        if (lstrcmpiW(L"NewDlgClass", ClassNameBuffer) != 0)
            CbtHookWindowProc(p, hwnd);
        // maybe more ???
    }
    return CallNextHookEx(hhook, nCode, wParam, lParam);
}

extern __inline NTLEA_TLS_DATA *GetTlsValueInternal(void);

static LRESULT CALLBACK CbtHookProcA(int code, WPARAM wParam, LPARAM lParam) {
    NTLEA_TLS_DATA *p = GetTlsValueInternal();
    return CbtHookProc(p, p->hWindowCbtHookAnsi, code, wParam, lParam);
}
static LRESULT CALLBACK CbtHookProcW(int code, WPARAM wParam, LPARAM lParam) {
    NTLEA_TLS_DATA *p = GetTlsValueInternal();
    return CbtHookProc(p, p->hWindowCbtHookUnicode, code, wParam, lParam);
}

void InstallCbtHook(NTLEA_TLS_DATA *ptls) {  // ebx
    // most hook-based Locale Emulator used to hook : computer-based training (CBT) message
    // for more info, see : http://msdn.microsoft.com/en-us/library/windows/desktop/ms644977(v=vs.85).aspx
    OutTraceLOC("InstallCbtHook: guard=%d\n", ptls->hWindowHooking);
    if (ptls->hWindowHooking == 0) { // use tls-reference on window hooking, only unblock the most outside !
        ptls->hWindowCbtHookAnsi = (*pSetWindowsHookExA)(WH_CBT, CbtHookProcA, NULL, GetCurrentThreadId());
        ptls->hWindowCbtHookUnicode = (*pSetWindowsHookExW)(WH_CBT, CbtHookProcW, NULL, GetCurrentThreadId());
    }
    ptls->hWindowHooking++;
}

void UninstallCbtHook(NTLEA_TLS_DATA *ptls) {  // ebx
    ptls->hWindowHooking--; // guard
    OutTraceLOC("UninstallCbtHook: guard=%d\n", ptls->hWindowHooking);
    if (ptls->hWindowHooking == 0) {
        UnhookWindowsHookEx(ptls->hWindowCbtHookAnsi);
        UnhookWindowsHookEx(ptls->hWindowCbtHookUnicode);
    } else if (ptls->hWindowHooking < 0) {
        // reference error occured :
        OutTraceE("UninstallCbtHook: unmatching hWindowHooking=%d\n", ptls->hWindowHooking < 0);
    }
}

LRESULT CALLBACK DefConversionProc(LPVOID lpProcAddress, HWND hWnd, HWND hMDIClient,
                                   BOOL bMDIClientEnabled, INT uMsg, WPARAM wParam, LPARAM lParam) {
    // debug log - to delete
    HWND target = bMDIClientEnabled ? hMDIClient : hWnd;
    OutDebugLOC("DefConversionProc: hwnd=%#x hmdiclient=%#x midi=%#x msg=%#x(%s) wparam=%#x lparam=%#x type=%s\n",
                hWnd, hMDIClient, bMDIClientEnabled, uMsg, ExplainWinMessage(uMsg), wParam, lParam,
                IsWindowUnicode(target) ? "WIDE" : "ANSI");
    LPCWSTR lpUnicodeWindowName = NULL, lpUnicodeClassName = NULL;
    WCHAR CharBuffer[2];
    int type = 0;
    //	char classname[256]; GetClassNameA(hWnd, classname, sizeof(classname));
    //	if (lstrcmpiA(classname, "TRadioButton") == 0) {
    //	ntprintfA(256, 1, "%s: proc-%p hwnd=%p, msg=%04x, wParam=%d, lParam=%d\n", __FUNCTION__, lpProcAddress, hWnd, uMsg, wParam, lParam);
    //	}
    // tofix !!!
    // some game will give us hWnd == NULL, oops, patch it now ?!
    if (hWnd && !IsWindowUnicode(hWnd)) {
        OutTraceLOC("DefConversionProc: hwnd=%#x is ANSI\n", hWnd);
        //	MessageBox(0, "Is Not Unicode Window!!", "DxWnd", 0);
        //	exit(0); // exit !!!
    }
    // the translation of window class & title has been commented out since there's a high
    // possibility of a doubled translation (the former in CreateWindowExA). NtleasWin
    // handles this possibility by registering the nature of the operation, but this is not
    // 100% rekliable (according to the authors) if I got it right, so better comment this out.
    switch (uMsg) {
#if 1
    case WM_CREATE: // L204
    case WM_NCCREATE: { //
        if (lParam) { // simplified ...
            CREATEWNDEX *p = (CREATEWNDEX *)malloc(sizeof(CREATEWNDEX));
            memcpy(p, (LPVOID)lParam, sizeof(CREATEWNDEX));
            // special handler :
            // for preset window such as SHBrowseForFolder/..., the window name won't be changed !!
            if (p->lpWindowName) {
                OutTrace("DefConversionProc: wname=\"%s\"\n", p->lpWindowName);
                p->lpWindowName = (LPVOID)(lpUnicodeWindowName = MultiByteToWideCharInternal((LPCSTR)p->lpWindowName));
            }
            if ((DWORD_PTR)p->lpClassName & WM_CLASSMASK) {
                OutTrace("DefConversionProc: cname=\"%s\"\n", p->lpClassName);
                p->lpClassName = (LPVOID)(lpUnicodeClassName = MultiByteToWideCharInternal((LPCSTR)p->lpClassName));
            }
            // LN899
            LRESULT hr = CallProcAddress(lpProcAddress, hWnd, hMDIClient, bMDIClientEnabled, uMsg, wParam, (LPARAM)p);
            // LN893
            if (lpUnicodeWindowName) free((LPVOID)lpUnicodeWindowName);
            if (lpUnicodeClassName) free((LPVOID)lpUnicodeClassName);
            if (p/*ebx*/) free((LPVOID)p);
            return (hr);
        }
    }
    break;
#endif
    case WM_MDICREATE: { // LN207
        if (lParam) {
            CREATEMDIWND *p = (CREATEMDIWND *)malloc(sizeof(CREATEMDIWND));
            memcpy(p, (LPVOID)lParam, sizeof(CREATEMDIWND));
            if (p->szTitle) {
                OutTrace("DefConversionProc: title=\"%s\"\n", p->szTitle);
                p->szTitle = (LPCSTR)(lpUnicodeWindowName = MultiByteToWideCharInternal(p->szTitle));
            }
            if ((DWORD_PTR)p->szClass & WM_CLASSMASK) {
                OutTrace("DefConversionProc: midiclass=\"%s\"\n", p->szClass);
                p->szClass = (LPCSTR)(lpUnicodeClassName = MultiByteToWideCharInternal(p->szClass));
            }
            // LN899
            LRESULT hr = CallProcAddress(lpProcAddress, hWnd, hMDIClient, bMDIClientEnabled, uMsg, wParam, (LPARAM)p);
            // LN893
            if (lpUnicodeWindowName) FreeStringInternal((LPVOID)lpUnicodeWindowName);
            if (lpUnicodeClassName) FreeStringInternal((LPVOID)lpUnicodeClassName);
            if (p/*ebx*/) FreeStringInternal((LPVOID)p);
            return (hr);
        }
    }
    break;
    case WM_IME_CHAR: // LN209
    case WM_CHAR: { // LN209
        if ((wchar_t)wParam > 0x7F) {
            MultiByteToWideChar(dxw.CodePage, 0, (LPCSTR)&wParam, -1, CharBuffer, 2);
            *((char *)&wParam + 0) = (char)CharBuffer[1];
            *((char *)&wParam + 1) = (char)CharBuffer[0];
        }
    }
    break;
    case EM_REPLACESEL: // LN220
    case WM_SETTEXT: // LN220
    case WM_SETTINGCHANGE: // LN220
    case WM_DEVMODECHANGE: { // LN220
        LPCWSTR lParamW = lParam ? MultiByteToWideCharInternal((LPCSTR)lParam) : NULL;
        OutTraceLOC("DefConversionProc: A(%s) -> W(%S)\n", (LPCSTR)lParam, lParamW);
        LRESULT hr = CallProcAddress(lpProcAddress, hWnd, hMDIClient, bMDIClientEnabled, uMsg, wParam, (LPARAM)lParamW);
        // LN893
        if (lParamW) FreeStringInternal((LPVOID)lParamW);
        return hr;
    }// break;
    case WM_GETTEXTLENGTH: { // LN223
        LRESULT len = CallProcAddress(lpProcAddress, hWnd, hMDIClient, bMDIClientEnabled, WM_GETTEXTLENGTH, 0, 0);
        if (len > 0) {
            LPWSTR lParamW = (LPWSTR)AllocateZeroedMemory((len + 1) * sizeof(wchar_t));
            CallProcAddress(lpProcAddress, hWnd, hMDIClient, bMDIClientEnabled, WM_GETTEXT, (len + 1) * sizeof(wchar_t), (LPARAM)lParamW);
            len = WideCharToMultiByte(CP_ACP, 0, lParamW, -1, NULL, 0, NULL, NULL) - 1; // required
            // LN893
            if (lParamW/*ebx*/) FreeStringInternal((LPVOID)lParamW);
        }
        return len;
    }// break;
    case WM_GETTEXT: // LN210
    case EM_GETSELTEXT: { // LN210
        if (IsBadWritePtr((LPVOID)lParam, 1))
            return (0);
        else {
            // L211
            int len = (int)CallProcAddress(lpProcAddress, hWnd, hMDIClient, bMDIClientEnabled, WM_GETTEXTLENGTH, 0, 0);
            // no needs check len == 0 ??
            LPSTR lParamW = (LPSTR)AllocateZeroedMemory((len + 1) * sizeof(wchar_t));
            CallProcAddress(lpProcAddress, hWnd, hMDIClient, bMDIClientEnabled, uMsg, wParam, (LPARAM)lParamW);
            if (uMsg != EM_GETSELTEXT) len = (int)wParam; // wParam should be len + 1 !!
            len = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)lParamW, -1, (LPSTR)lParam, len, NULL, NULL) - 1; // required
            if (len > 0) {
                // LN893
                if (lParamW) FreeStringInternal((LPVOID)lParamW);
            } else {
                // L216
                if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
                    *((LPSTR)lParam + wParam - 1) = '\0';
                else {
                    // L217
                    *((LPSTR)lParam) = '\0';
                }
            }
            return len;
        }
    }//	break;
    case LB_GETTEXTLEN: { // LN226
        int ret = CheckWindowStyle(hWnd, 1);
        if (ret != -1) {
            int len = (int)CallProcAddress(lpProcAddress, hWnd, hMDIClient, bMDIClientEnabled, LB_GETTEXTLEN, wParam, 0);
            if (len > 0) {
                LPWSTR lParamW = (LPWSTR)AllocateZeroedMemory((len + 1) * sizeof(wchar_t));
                CallProcAddress(lpProcAddress, hWnd, hMDIClient, bMDIClientEnabled, LB_GETTEXT, wParam, (LPARAM)lParamW);
                len = WideCharToMultiByte(CP_ACP, 0, lParamW, -1, NULL, 0, NULL, NULL) - 1;
                // LN893
                if (lParamW/*ebx*/) FreeStringInternal((LPVOID)lParamW);
            }
            return len;
        }
    }
    break;
    case LB_GETTEXT: // LN221
        type = 1;
    //	break;
    case CB_GETLBTEXT: { // LN222
        int ret = CheckWindowStyle(hWnd, type); // 0
        if (ret != -1) {
            LPWSTR lParamW = (LPWSTR)AllocateZeroedMemory(MAX_PATH * sizeof(wchar_t));
            int len = (int)CallProcAddress(lpProcAddress, hWnd, hMDIClient, bMDIClientEnabled, uMsg, wParam, (LPARAM)lParamW);
            if (!len || !WideCharToMultiByte(CP_ACP, 0, lParamW, len + 1, (LPSTR)lParam, MAX_PATH * sizeof(wchar_t), NULL, NULL)) {
                if (lParam) *(LPSTR)lParam = '\0';
            }
            // LN893
            if (lParamW/*ebx*/) FreeStringInternal((LPVOID)lParamW);
            return len;
        }
    }
    break;
    case LB_FINDSTRINGEXACT: // LN205
    case LB_ADDSTRING: // LN205
    case LB_INSERTSTRING: // L205
    case LB_FINDSTRING: // L205
    case LB_ADDFILE: // L205
    case LB_SELECTSTRING: // L205
    case LB_DIR: // L205
        type = 1; // ebx = 1
    //	break;
    case CB_FINDSTRINGEXACT: // LN206
    case CB_ADDSTRING: // LN206
    case CB_INSERTSTRING: // LN206
    case CB_SELECTSTRING: // LN206
    case CB_DIR: // LN206
    case CB_FINDSTRING: { // LN206
        int ret = CheckWindowStyle(hWnd, type); // ebx = 0 / 1
        if (ret != -1) {
            LPCWSTR lParamW = lParam ? MultiByteToWideCharInternal((LPCSTR)lParam) : NULL;
            // LN899
            LRESULT hr = CallProcAddress(lpProcAddress, hWnd, hMDIClient, bMDIClientEnabled, uMsg, wParam, (LPARAM)lParamW);
            if (lParamW/*ebx*/) FreeStringInternal((LPVOID)lParamW);
            return hr;
        }
    }
    break;
    default: // LN201
        break;
    }
    // ---------
    return CallProcAddress(lpProcAddress, hWnd, hMDIClient, bMDIClientEnabled, uMsg, wParam, lParam);
}
