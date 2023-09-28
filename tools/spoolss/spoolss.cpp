#include <windows.h>
#include <stdio.h>
#pragma pack(1)

HINSTANCE hLThis = 0;
HINSTANCE hL = 0;
FARPROC p[368] = {0};

void InitDxWnd(char *);
void DxWndEndHook(void);

BOOL WINAPI DllMain(HINSTANCE hInst, DWORD reason, LPVOID) {
    return TRUE;
}

// AbortPrinter
DWORD WINAPI AbortPrinter(DWORD hdl) {
    return TRUE;
}
