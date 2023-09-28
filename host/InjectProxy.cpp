#include "stdafx.h"
#include "shlwapi.h"
#include "dxwndhost.h"

//#define STATICPROXYLIST
#define MAX_PROXYLIST 32

#ifdef STATICPROXYLIST
char *ProxyList[] = {
    "ddraw.dll",
    "d3d8.dll",
    "d3d9.dll",
    "winmm.dll",
    "opengl32.dll",
    "dinput.dll",
    NULL
};
static BOOL BuildProxyList() {}
#else
char *ProxyList[MAX_PROXYLIST + 1];
static BOOL BuildProxyList() {
    WIN32_FIND_DATA FindFileData;
    HANDLE hFind;
    char path[MAX_PATH];
    int i = 0;
    ProxyList[i] = NULL;
    sprintf_s(path, MAX_PATH, "%s\\proxy\\*.dll", GetDxWndPath());
    hFind = FindFirstFile(path, &FindFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        OutTrace("FindFirstFile failed (%d)\n", GetLastError());
        return FALSE;
    }
    OutTrace("ProxyList[%d]=%s\n", i, FindFileData.cFileName);
    ProxyList[i] = (char *)malloc(strlen(FindFileData.cFileName) + 1);
    strcpy(ProxyList[i++], FindFileData.cFileName);
    while (FindNextFile(hFind, &FindFileData) && (i < MAX_PROXYLIST)) {
        OutTrace("ProxyList[%d]=%s\n", i, FindFileData.cFileName);
        ProxyList[i] = (char *)malloc(strlen(FindFileData.cFileName));
        strcpy(ProxyList[i++], FindFileData.cFileName);
    }
    ProxyList[i] = NULL;
    FindClose(hFind);
    return TRUE;
}
#endif

static BOOL IsProcessRunning(DWORD pid) {
    HANDLE process = OpenProcess(SYNCHRONIZE, FALSE, pid);
    DWORD ret = WaitForSingleObject(process, 0);
    CloseHandle(process);
    return ret == WAIT_TIMEOUT;
}

static char *GetDxWndPath() {
    static BOOL DoOnce = TRUE;
    static char sFolderPath[MAX_PATH];
    if(DoOnce) {
        GetModuleFileName(GetModuleHandle("dxwnd"), sFolderPath, MAX_PATH);
        sFolderPath[strlen(sFolderPath) - strlen("dxwnd.dll")] = 0; // terminate the string just before "dxwnd.dll"
        DoOnce = FALSE;
    }
    return sFolderPath;
}

typedef struct {
    DWORD pid;
    DWORD dwProxyMask;
    char path[MAX_PATH];
} ProcessPollerArgs;

BOOL bProxyIsRunning = FALSE;

DWORD WINAPI ProcessPoller(LPVOID lpParameter) {
#define DXWREFRESHINTERVAL 200
    // wait for injected process termination
    ProcessPollerArgs *args = (ProcessPollerArgs *)lpParameter;
    char sPath[MAX_PATH];
    DWORD dwProxyMask = args->dwProxyMask;
    Sleep(2000); // give 2 sec to be sure the process is up & running
    // wait until process termination
    while(IsProcessRunning(args->pid))
        Sleep(DXWREFRESHINTERVAL);
    OutTrace("Proxed task terminated\n");
    // remove all proxy files
    for(int i = 0; ProxyList[i]; i++) {
        sprintf(sPath, "%s\\%s", args->path, ProxyList[i]);
        if((dwProxyMask >> i) & 0x1) {
            if(!DeleteFileA(sPath))
                OutTrace("Error deleting path=\"%s\" err=%d\n", sPath, GetLastError());
        } else
            OutTrace("Skip failed proxy=\"%s\"\n", sPath);
    }
    free(lpParameter);
    return 0;
}

static void AddDxWndPath() {
    DWORD dwRet, dwLen;
    dwRet = GetEnvironmentVariable("Path", NULL, 0);
    if(!dwRet) OutTrace("no Path !!\n");
    dwLen = strlen(GetDxWndPath());
    dwLen = dwRet + dwLen + 2; // +1 for ';' and +1 for \0 terminator
    char *pEnv = (char *)malloc(dwLen);
    if(!pEnv) OutTrace("no memory !!\n");
    dwLen = GetEnvironmentVariable("Path", pEnv, dwLen);
    strcat(pEnv, ";");
    strcat(pEnv, GetDxWndPath());
    SetEnvironmentVariable("Path", pEnv);
    free(pEnv);
}

DWORD WINAPI InjectProxy(void *p) {
    ThreadInfo_Type *ThInfo;
    STARTUPINFO sinfo;
    PROCESS_INFORMATION pinfo;
    char path[MAX_PATH];
    extern char *GetFileNameFromHandle(HANDLE);
    char sRunTargetPath[MAX_PATH + 1];
    LPSTR lpCommandLine = NULL;
    static BOOL bInitialized = FALSE;
    DWORD dwProxyMask;
    ThInfo = (ThreadInfo_Type *)p;
    OutTrace("InjectProxy: exe=\"%s\" dir=\"%s\"\n", ThInfo->ExePath, ThInfo->PM.startfolder);
    strcpy_s(path, MAX_PATH, ThInfo->ExePath);
    strcpy_s(sRunTargetPath, sizeof(path), sRunTargetPath);
    PathRemoveFileSpec(path);
    if(!bInitialized) {
        AddDxWndPath();
        BuildProxyList();
        bInitialized = TRUE;
    }
    dwProxyMask = 0;
    for(int i = 0; ProxyList[i]; i++) {
        char sCopyFrom[MAX_PATH], sCopyTo[MAX_PATH];
        sprintf(sCopyFrom, "%s\\proxy\\%s", GetDxWndPath(), ProxyList[i]);
        sprintf(sCopyTo, "%s\\%s", path, ProxyList[i]);
        OutTrace("copy %s to %s ...\n",  sCopyFrom, sCopyTo);
        if(!CopyFile(sCopyFrom, sCopyTo, TRUE))
            OutTrace("copy %s to %s failed: err=%d\n", sCopyFrom, sCopyTo, GetLastError());
        else {
            dwProxyMask |= (0x1 << i);
            OutTrace("Mark copied proxy=\"%s\"\n", ProxyList[i]);
        }
    }
    ZeroMemory(&sinfo, sizeof(sinfo));
    sinfo.cb = sizeof(sinfo);
    // v2.05.21: set lpCommandLine together with lpApplicationName for games that requires it,
    // for instance "Star Wars Rogue Squadron 3D"
    // v2.05.23: made this configurable with SETCMDLINE flag
    // v2.05.53: made configurable by field copy and fixed argument order!!
    if(ThInfo->PM.cmdline[0])
        lpCommandLine = ThInfo->PM.cmdline;
    if (!CreateProcessA(ThInfo->ExePath, lpCommandLine, 0, 0, false, 0, NULL, path, &sinfo, &pinfo)) {
        int err = GetLastError();
        char *errmsg;
        switch(err) {
        case ERROR_DIRECTORY:
            errmsg = "(invalid directory name)";
            break;
        case ERROR_FILE_NOT_FOUND:
            errmsg = "(file not found)";
            break;
        default:
            errmsg = "";
            break;
        }
        OutTrace("CreateProcess appname=\"%s\" cmdline=\"%s\"\nerror=%d %s\n", ThInfo->ExePath, lpCommandLine, err, errmsg);
    } else {
        OutTrace("CreateProcess OK\n");
        ProcessPollerArgs *args = (ProcessPollerArgs *)malloc(sizeof(ProcessPollerArgs));
        args->pid = pinfo.dwProcessId;
        args->dwProxyMask = dwProxyMask;
        strcpy(args->path, path);
        CreateThread(NULL, 0, ProcessPoller, (VOID *)args, 0, NULL);
        free(p);
    }
    return 0;
}
