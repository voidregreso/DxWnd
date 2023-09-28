#include "stdafx.h"
#include "shlwapi.h"
#include "dxwndhost.h"

// this one mustdo very little, since all the job is done by the hook set by SetWindowsHookEx
// in the main program. Neverthless, there could be some advantage in doing this way by making
// the process activation in a separate thread, for instance to avoid DxWnd.exe hangs in case
// of an AV examination of the target process.

DWORD WINAPI InjectHook(void *p) {
    ThreadInfo_Type *ThInfo;
    STARTUPINFO sinfo;
    PROCESS_INFORMATION pinfo;
    char path[MAX_PATH];
    extern char *GetFileNameFromHandle(HANDLE);
    char sRunTargetPath[MAX_PATH + 1];
    LPSTR lpCommandLine = NULL;
    ThInfo = (ThreadInfo_Type *)p;
    strcpy_s(sRunTargetPath, MAX_PATH, ThInfo->ExePath);
    strcpy_s(path, sizeof(path), sRunTargetPath);
    PathRemoveFileSpec(path);
    // v2.05.21: setlpCommandLine together with lpApplicationName for games that requires it,
    // for instance "Star Wars Rogue Squadron 3D"
    // v2.05.23: made this configurable with SETCMDLINE flag
    // v2.05.53: made configurable by field copy and fixed argument order!!
    if(ThInfo->PM.cmdline[0])
        lpCommandLine = ThInfo->PM.cmdline;
    ZeroMemory(&sinfo, sizeof(sinfo));
    sinfo.cb = sizeof(sinfo);
    CreateProcess(ThInfo->ExePath, lpCommandLine,
                  0, 0, false, CREATE_DEFAULT_ERROR_MODE, NULL, path, &sinfo, &pinfo);
    CloseHandle(pinfo.hProcess); // no longer needed, avoid handle leakage
    CloseHandle(pinfo.hThread); // no longer needed, avoid handle leakage
    free(p); // don't forget to free the allocated area
    return 0;
}
