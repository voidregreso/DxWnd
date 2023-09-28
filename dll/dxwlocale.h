
typedef int (WINAPI *MultiByteToWideChar_Type)(UINT, DWORD, LPCSTR, int, LPWSTR, int);
extern MultiByteToWideChar_Type pMultiByteToWideChar;
typedef int (WINAPI *WideCharToMultiByte_Type)(UINT, DWORD, LPCWSTR, int, LPSTR, int, LPCSTR, LPBOOL);
extern WideCharToMultiByte_Type pWideCharToMultiByte;

__inline LPCWSTR MultiByteToWideCharInternal(LPCSTR lpString) {
    int size = lstrlenA(lpString)/* size without '\0' */, n = 0;
    LPWSTR wstr = (LPWSTR)malloc((size + 1) << 1);
    if (wstr) {
        n = (*pMultiByteToWideChar)(CP_ACP, 0, lpString, size, wstr, size);
        wstr[n] = L'\0'; // make tail !
    }
    return wstr;
}

__inline LPCSTR WideCharToMultiByteInternal(LPCWSTR lpString) {
    int size = lstrlenW(lpString)/* size without '\0' */, n = 0;
    LPSTR str = (LPSTR)malloc((size + 1) << 1); // TODO: support UTF-8 3bytes ???
    if (str) {
        n = (*pWideCharToMultiByte)(CP_ACP, 0, lpString, size, str, size << 1, NULL, NULL);
        str[n] = '\0'; // make tail !
    }
    return str;
}

__inline void FreeStringInternal(LPVOID lpWString) {
    free(lpWString);
}

__inline LPVOID AllocateHeapInternal(SIZE_T size/*ecx*/) {
    LPVOID ret = malloc(size);
    memset(ret, 0, size);
    return ret;
}

__inline LPVOID AllocateZeroedMemory(SIZE_T size/*eax*/) {
    //assert(size);
    //return HeapAlloc(settings.hHeap, HEAP_ZERO_MEMORY, size);
    LPVOID ret;
    ret = malloc(size);
    if(ret) memset(ret, 0, size);
    return ret;
}

#define WM_CLASSMASK 0xFFFF0000
#ifndef EM_GETSELTEXT
#define EM_GETSELTEXT	 0x43E
#endif

typedef struct { // same as CREATESTRUCT
    LPVOID    	lpParam;
    HINSTANCE	hInstance;
    HMENU		hMenu;
    HWND		hWndParent;
    INT			nHeight;
    INT			nWidth;
    INT			y;
    INT			x;
    DWORD		dwStyle;
    LPVOID		lpWindowName;	// convert !!
    LPVOID		lpClassName;	// convert !!
    DWORD		dwExStyle;
} CREATEWNDEX;

typedef struct { // same as MDICREATESTRUCT
    LPCSTR		szClass;
    LPCSTR		szTitle;
    HWND		hOwner;
    INT			x;
    INT			y;
    INT			cx;
    INT			cy;
    DWORD		style;
    LPARAM		lParam;
} CREATEMDIWND;

#define MAXSYSCLASSDESC 15

typedef struct {
    WNDPROC		AnsiSystemClassProc;
    WNDPROC		UnicodeSystemClassProc;
} SYSTEM_CLASS_WNDPROC;

typedef struct {
    DWORD		InternalCall;
    DWORD		DBCSLeadByte;
    DWORD		IsFontAvailable;
    DWORD		CurrentCallType;
    DLGPROC		DialogProc;
    DWORD		IsCreateFileCall;
    LONG		hWindowHooking; // counter for avoiding setup/uninst hook-recursive
    HHOOK		hWindowCbtHookAnsi;
    HHOOK		hWindowCbtHookUnicode;
    SYSTEM_CLASS_WNDPROC SystemClassDesc[MAXSYSCLASSDESC];
} NTLEA_TLS_DATA;

#define CT_CREATE_NULL			0	// used for checking system create without known yet!
#define CT_CREATE_WINDOW        1	// used by any window
#define CT_CREATE_DIALOG        2	// used by dialog & dialogindirect series
#define CT_CREATE_PRESET		3	// used by SHBrowseForFolderA

typedef struct {
    WNDPROC		PrevAnsiWindowProc;
    WNDPROC		PrevUnicodeWindowProc;
} NTLEA_WND_ASC_DATA;
