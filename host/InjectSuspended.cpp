#include "stdafx.h"
#include "dxwndhost.h"

void InjectSuspended(char *exepath, char *dirpath, BOOL bSuspended, BOOL bCommitPage, TARGETMAP *tm, PRIVATEMAP *pm) {
    STARTUPINFO sinfo;
    PROCESS_INFORMATION pinfo;
    char StartingCode[4];
    DWORD EndlessLoop;
    EndlessLoop = 0x9090FEEB; // careful: it's BIG ENDIAN: EB FE 90 90
    DWORD BytesCount;
    char DebugMessage[1024];
    DWORD OldProt;
    char dllpath[MAX_PATH];
    LPVOID StartAddress;
    HANDLE TargetHandle;
    FILE *fExe = NULL;
    BOOL bKillProcess = FALSE;
    HANDLE hEvent;
    char *eventName = "DxWndInjectCompleted";
    DWORD t0 = GetTickCount();
    BOOL ret;
    LPSTR lpCommandLine = NULL;
    OutTrace("InjectSuspended: exe=\"%s\" dir=\"%s\" commit=%x\n", exepath, dirpath, bCommitPage);
    hEvent = CreateEvent(NULL, FALSE, FALSE, eventName);
    if(!hEvent) {
        if(ERROR_ALREADY_EXISTS == GetLastError())
            hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, eventName);
        if(!hEvent)
            OutTrace("CreateEvent %s failed: err=%d\n", eventName, GetLastError());
    }
    ZeroMemory(&sinfo, sizeof(sinfo));
    sinfo.cb = sizeof(sinfo);
    // attempt to load the specified target
    // v2.05.21: setlpCommandLine together with lpApplicationName for games that requires it,
    // for instance "Star Wars Rogue Squadron 3D"
    // v2.05.23: made this configurable with SETCMDLINE flag
    // v2.05.53: made configurable by field copy and fixed argument order!!
    if(pm->cmdline[0])
        lpCommandLine = pm->cmdline;
    if (!CreateProcess(exepath, lpCommandLine, 0, 0, false, CREATE_SUSPENDED, NULL, dirpath, &sinfo, &pinfo)) {
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
        sprintf(DebugMessage, "CreateProcess \"%s\" \nerror=%d %s", exepath, err, errmsg);
        MessageBoxEx(0, DebugMessage, "Injection", MB_ICONEXCLAMATION | MB_OK, NULL);
        OutTrace("%s\n", DebugMessage);
        return;
    }
    while(TRUE) { // fake loop
        bKillProcess = TRUE;
        extern LPVOID GetThreadStartAddress(HANDLE);
        // locate the entry point
        TargetHandle = OpenProcess(
                           PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_SUSPEND_RESUME,
                           FALSE,
                           pinfo.dwProcessId);
        OutTrace("Target handle=%x\n", TargetHandle);
        // You can't just fetch the start address from the exe because it might be relocated
        // due to ASLR (Address Space Layout Randomization) www.wikipedia.org/wiki/ASLR
        // GetThreadStartAddress fixes Injection run for bstone (windows port of "Blak Stone").
        StartAddress = GetThreadStartAddress(pinfo.hThread);
        if(StartAddress) {
            // patch the entry point with infinite loop
            if(!VirtualProtectEx(TargetHandle, StartAddress, 4, PAGE_EXECUTE_READWRITE, &OldProt )) {
                sprintf(DebugMessage, "VirtualProtectEx error=%d", GetLastError());
                MessageBoxEx(0, DebugMessage, "Injection", MB_ICONEXCLAMATION | MB_OK, NULL);
                OutTrace("%s\n", DebugMessage);
                break;
            }
            if(!ReadProcessMemory(TargetHandle, StartAddress, &StartingCode, 4, &BytesCount)) {
                sprintf(DebugMessage, "ReadProcessMemory error=%d", GetLastError());
                MessageBoxEx(0, DebugMessage, "Injection", MB_ICONEXCLAMATION | MB_OK, NULL);
                OutTrace("%s\n", DebugMessage);
                break;
            }
            if(!WriteProcessMemory(TargetHandle, StartAddress, &EndlessLoop, 4, &BytesCount)) {
                sprintf(DebugMessage, "WriteProcessMemory error=%d", GetLastError());
                MessageBoxEx(0, DebugMessage, "Injection", MB_ICONEXCLAMATION | MB_OK, NULL);
                OutTrace("%s\n", DebugMessage);
                break;
            }
            if(!FlushInstructionCache(TargetHandle, StartAddress, 4)) {
                sprintf(DebugMessage, "FlushInstructionCache error=%d", GetLastError());
                MessageBoxEx(0, DebugMessage, "Injection", MB_ICONEXCLAMATION | MB_OK, NULL);
                OutTrace("%s\n", DebugMessage);
                break; // error condition
            }
            // resume the main thread
            if(ResumeThread(pinfo.hThread) == (DWORD) - 1) {
                sprintf(DebugMessage, "ResumeThread error=%d at:%d", GetLastError(), __LINE__);
                MessageBoxEx(0, DebugMessage, "Injection", MB_ICONEXCLAMATION | MB_OK, NULL);
                OutTrace("%s\n", DebugMessage);
                break;
            }
        } else {
            sprintf(DebugMessage, "GetThreadStartAddress failed");
            MessageBoxEx(0, DebugMessage, "Injection", MB_ICONEXCLAMATION | MB_OK, NULL);
            OutTrace("%s\n", DebugMessage);
        }
        // wait until the thread stuck at entry point
        CONTEXT context;
        context.Eip = (DWORD)0; // initialize to impossible value
        if(bSuspended) {
            MessageBox(NULL, "Click when the debugger is attached", "debug", 0);
            // read the thread context
            context.ContextFlags = CONTEXT_CONTROL;
            if(!GetThreadContext(pinfo.hThread, &context)) {
                sprintf(DebugMessage, "GetThreadContext error=%d", GetLastError());
                MessageBoxEx(0, DebugMessage, "Injection", MB_ICONEXCLAMATION | MB_OK, NULL);
                OutTrace("%s\n", DebugMessage);
                break;
            }
        } else {
            BOOL bFailed = FALSE;
            for ( unsigned int i = 0; i < 80 && context.Eip != (DWORD)StartAddress; ++i ) {
                // patience.
                Sleep(50);
                // read the thread context
                context.ContextFlags = CONTEXT_CONTROL;
                if(!GetThreadContext(pinfo.hThread, &context)) {
                    sprintf(DebugMessage, "GetThreadContext error=%d", GetLastError());
                    MessageBoxEx(0, DebugMessage, "Injection", MB_ICONEXCLAMATION | MB_OK, NULL);
                    OutTrace("%s\n", DebugMessage);
                    bFailed = TRUE;
                    break;
                }
                OutTrace("wait cycle %d eip=%x\n", i, context.Eip);
            }
            if(bFailed) break;
        }
        if (context.Eip != (DWORD)StartAddress) {
            // wait timed out
            sprintf(DebugMessage, "thread blocked eip=%x addr=%x", context.Eip, StartAddress);
            MessageBoxEx(0, DebugMessage, "Injection", MB_ICONEXCLAMATION | MB_OK, NULL);
            OutTrace("%s\n", DebugMessage);
            break;
        }
        // inject DLL payload into remote process
        GetFullPathName("dxwnd.dll", MAX_PATH, dllpath, NULL);
        ret = bCommitPage ?
              CommitAndInject(pinfo.dwProcessId, dllpath, (LPVOID)pm->CommitAddress, pm->CommitLength) :
              Inject(pinfo.dwProcessId, dllpath);
        if(!ret) {
            // DXW_STRING_INJECTION
            sprintf(DebugMessage, "Injection error: pid=%x dll=%s", pinfo.dwProcessId, dllpath);
            MessageBoxEx(0, DebugMessage, "Injection", MB_ICONEXCLAMATION | MB_OK, NULL);
            OutTrace("%s\n", DebugMessage);
            break;
        }
#if 0
        // v2.04.65: IMPORTANT - give some time to the injected thread to complete its job!!
        // to do: find a syncronization event to be sure that the operation is done safely in minimum time.
        Sleep(200);
#else
        DWORD dwWaitResult = WaitForSingleObject(hEvent, 500);
        if(dwWaitResult == WAIT_OBJECT_0)
            OutTrace("WaitForSingleObject ok after %d mSec\n", GetTickCount() - t0);
        else
            OutTrace("WaitForSingleObject returns %d\n", dwWaitResult);
#endif
        // pause
        if(SuspendThread(pinfo.hThread) == (DWORD) - 1) {
            sprintf(DebugMessage, "SuspendThread error=%d", GetLastError());
            MessageBoxEx(0, DebugMessage, "Injection", MB_ICONEXCLAMATION | MB_OK, NULL);
            OutTrace("%s\n", DebugMessage);
            break;
        }
        // restore original entry point
        if(!WriteProcessMemory(TargetHandle, StartAddress, &StartingCode, 4, &BytesCount)) {
            sprintf(DebugMessage, "WriteProcessMemory error=%d", GetLastError());
            MessageBoxEx(0, DebugMessage, "Injection", MB_ICONEXCLAMATION | MB_OK, NULL);
            OutTrace("%s\n", DebugMessage);
            break;
        }
        if(!FlushInstructionCache(TargetHandle, StartAddress, 4)) {
            sprintf(DebugMessage, "FlushInstructionCache error=%d", GetLastError());
            MessageBoxEx(0, DebugMessage, "Injection", MB_ICONEXCLAMATION | MB_OK, NULL);
            OutTrace("%s\n", DebugMessage);
            break; // error condition
        }
        // you are ready to go
        // pause and restore original entry point
        if(ResumeThread(pinfo.hThread) == (DWORD) - 1) {
            sprintf(DebugMessage, "ResumeThread error=%d at:%d", GetLastError(), __LINE__);
            MessageBoxEx(0, DebugMessage, "Injection", MB_ICONEXCLAMATION | MB_OK, NULL);
            OutTrace("%s\n", DebugMessage);
            break;
        }
        bKillProcess = FALSE;
        break; // exit fake loop
    }
    // cleanup ....
    if(fExe) fclose(fExe);
    if(TargetHandle) CloseHandle(TargetHandle);
    // terminate the newly spawned process
    if(bKillProcess) {
        if(!TerminateProcess( pinfo.hProcess, -1 ))
            OutTrace("failed to kill hproc=%x err=%d\n", pinfo.hProcess, GetLastError());
    }
    OutTrace("InjectSuspended done!\n");
}
