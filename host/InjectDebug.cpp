#include "stdafx.h"
#include "shlwapi.h"
#include "dxwndhost.h"

// For thread messaging
#define DEBUG_EVENT_MESSAGE		WM_APP + 0x100

static char *ExceptionCaption(DWORD ec) {
    char *c;
    switch(ec) {
    case EXCEPTION_ACCESS_VIOLATION:
        c = "Access Violation";
        break;
    case EXCEPTION_DATATYPE_MISALIGNMENT:
        c = "Datatype Misalignment";
        break;
    case EXCEPTION_BREAKPOINT:
        c = "Breakpoint";
        break;
    case EXCEPTION_SINGLE_STEP:
        c = "Single Step";
        break;
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
        c = "Array Bouds Exceeded";
        break;
    case EXCEPTION_FLT_DENORMAL_OPERAND:
        c = "Float Denormal Operand";
        break;
    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
        c = "Divide by Zero";
        break;
    case EXCEPTION_FLT_INEXACT_RESULT:
        c = "Inexact Result";
        break;
    case EXCEPTION_FLT_INVALID_OPERATION:
        c = "Invalid Operation";
        break;
    case EXCEPTION_FLT_OVERFLOW:
        c = "Float Overflow";
        break;
    case EXCEPTION_FLT_STACK_CHECK:
        c = "Float Stack Check";
        break;
    case EXCEPTION_FLT_UNDERFLOW:
        c = "Float Undeflow";
        break;
    case EXCEPTION_INT_DIVIDE_BY_ZERO:
        c = "Integer Divide by Zero";
        break;
    case EXCEPTION_INT_OVERFLOW:
        c = "Integer Overflow";
        break;
    case EXCEPTION_PRIV_INSTRUCTION:
        c = "Priviliged Instruction";
        break;
    case EXCEPTION_IN_PAGE_ERROR:
        c = "In Page Error";
        break;
    case EXCEPTION_ILLEGAL_INSTRUCTION:
        c = "Illegal Instruction";
        break;
    case EXCEPTION_NONCONTINUABLE_EXCEPTION:
        c = "Non-continuable exception";
        break;
    case EXCEPTION_STACK_OVERFLOW:
        c = "Stack Overflow";
        break;
    case EXCEPTION_INVALID_DISPOSITION:
        c = "Invalid Disposition";
        break;
    case EXCEPTION_GUARD_PAGE:
        c = "Guard Page Violation";
        break;
    case EXCEPTION_INVALID_HANDLE:
        c = "Invalid Handle";
        break;
    //case EXCEPTION_POSSIBLE_DEADLOCK:		c="Possible Deadlock"; break;
    default:
        c = "unknown";
        break;
    }
    return c;
}

static char *fmttime(DWORD t0) {
    static char sTime[20];
    DWORD dtime = GetTickCount() - t0;
    sprintf(sTime, "%05.5d:%03.3d", dtime / 1000, dtime % 1000);
    return sTime;
}

DWORD WINAPI InjectDebug(void *p) {
    ThreadInfo_Type *ThInfo;
    STARTUPINFO sinfo;
    PROCESS_INFORMATION pinfo;
    char path[MAX_PATH];
    extern char *GetFileNameFromHandle(HANDLE);
    char sRunTargetPath[MAX_PATH + 1];
#ifdef DXWDEBUGSTEPPING
    PROCESS_INFORMATION *pi;
    CREATE_THREAD_DEBUG_INFO *ti;
    LOAD_DLL_DEBUG_INFO *li;
    UNLOAD_DLL_DEBUG_INFO *ui;
    EXCEPTION_DEBUG_INFO *ei;
    EXIT_PROCESS_DEBUG_INFO *xpi;
    EXIT_THREAD_DEBUG_INFO *xti;
    int res;
    BOOL step = TRUE; // initialize to TRUE to enable
    BOOL stepdll = FALSE; // initialize to TRUE to enable
#endif
    bool bContinueDebugging;
    char DebugMessage[MAX_PATH + 256 + 1];
    bool bMustCommit;
    LPSTR lpCommandLine = NULL;
    DWORD time0 = GetTickCount();
    ThInfo = (ThreadInfo_Type *)p;
    strcpy_s(sRunTargetPath, MAX_PATH, ThInfo->ExePath);
    ZeroMemory(&sinfo, sizeof(sinfo));
    sinfo.cb = sizeof(sinfo);
    strcpy_s(path, sizeof(path), sRunTargetPath);
    PathRemoveFileSpec(path);
    // v2.05.21: set lpCommandLine together with lpApplicationName for games that requires it,
    // for instance "Star Wars Rogue Squadron 3D"
    // v2.05.23: made this configurable with SETCMDLINE flag
    // v2.05.53: made configurable by field copy and fixed argument order!!
    if(ThInfo->PM.cmdline[0])
        lpCommandLine = ThInfo->PM.cmdline;
    if(!CreateProcess(sRunTargetPath, lpCommandLine,
                      NULL, NULL, false, DEBUG_ONLY_THIS_PROCESS, NULL, path, &sinfo, &pinfo)) {
        DWORD dwLastErr = GetLastError();
        switch (dwLastErr) {
        case ERROR_ELEVATION_REQUIRED:
            sprintf(DebugMessage, "Create process error=%d: DxWnd must run as administrator", dwLastErr);
            MessageBoxEx(0, DebugMessage, "ERROR", MB_ICONEXCLAMATION | MB_OK, NULL);
            OutTrace("%s\n", DebugMessage);
            break;
        case ERROR_FILE_NOT_FOUND:
        case ERROR_INVALID_NAME:
            sprintf(DebugMessage, "Create process error=%d:\nThe system cannot find the file\"%s\"", dwLastErr, sRunTargetPath);
            MessageBoxEx(0, DebugMessage, "ERROR", MB_ICONEXCLAMATION | MB_OK, NULL);
            OutTrace("%s\n", DebugMessage);
            break;
        default:
            sprintf(DebugMessage, "CREATE PROCESS error=%d", dwLastErr);
            MessageBoxEx(0, DebugMessage, "ERROR", MB_ICONEXCLAMATION | MB_OK, NULL);
            OutTrace("%s\n", DebugMessage);
            break;
        }
    }
    OutTrace("[%s] create process: path=\"%s\" cmdline=\"%s\"\n", fmttime(time0), path, lpCommandLine);
    CString strEventMessage;
    DEBUG_EVENT debug_event = {0};
    bContinueDebugging = true;
    DWORD dwContinueStatus = DBG_CONTINUE;
    bMustCommit = (ThInfo->TM.flags12 & COMMITPAGE) ? TRUE : FALSE;
    while(bContinueDebugging) {
        BOOL ret;
        PVOID dwLastExceptionAddress = 0;
        DWORD dwLastExceptionCode = 0;
        dwContinueStatus = DBG_CONTINUE; // from "Crude Debugger"
        if (!WaitForDebugEvent(&debug_event, INFINITE)) {
            sprintf(DebugMessage, "[%s] Injection error: WaitForDebugEvent error=%d",  fmttime(time0), GetLastError());
            MessageBoxEx(0, DebugMessage, "Injection", MB_ICONEXCLAMATION | MB_OK, NULL);
            OutTrace("%s\n", DebugMessage);
            break; // must release pinfo handles
        }
        //OutTrace("Injection: WaitForDebugEvent code=%d(%s)\n", debug_event.dwDebugEventCode, sEventCode(debug_event.dwDebugEventCode));
        switch(debug_event.dwDebugEventCode) {
        // -------------------------------------//
        // EXIT_PROCESS_DEBUG_EVENT             //
        // -------------------------------------//
        case EXIT_PROCESS_DEBUG_EVENT:
            OutTrace("[%s] exit process\n", fmttime(time0));
            bContinueDebugging = false;
            break;
        // -------------------------------------//
        // CREATE_PROCESS_DEBUG_EVENT           //
        // -------------------------------------//
        case CREATE_PROCESS_DEBUG_EVENT:
            OutTrace("[%s] create process: base=0x%X path=\"%s\"\n",
                     fmttime(time0), debug_event.u.CreateProcessInfo.lpBaseOfImage,
                     GetFileNameFromHandle(debug_event.u.CreateProcessInfo.hFile));
            GetFullPathName("dxwnd.dll", MAX_PATH, path, NULL);
            ret = bMustCommit ?
                  CommitAndInject(pinfo.dwProcessId, path, (LPVOID)ThInfo->PM.CommitAddress, ThInfo->PM.CommitLength) :
                  Inject(pinfo.dwProcessId, path);
            if(!ret) {
                // DXW_STRING_INJECTION
                sprintf(DebugMessage, "Injection error: pid=%x dll=%s", pinfo.dwProcessId, path);
                MessageBoxEx(0, DebugMessage, "Injection", MB_ICONEXCLAMATION | MB_OK, NULL);
                OutTrace("[%s] %s\n", fmttime(time0), DebugMessage);
            }
            CloseHandle(debug_event.u.CreateProcessInfo.hProcess);
            CloseHandle(debug_event.u.CreateProcessInfo.hThread);
            CloseHandle(debug_event.u.CreateProcessInfo.hFile);
            break;
        // -------------------------------------//
        // CREATE_THREAD_DEBUG_EVENT            //
        // -------------------------------------//
        case CREATE_THREAD_DEBUG_EVENT:
            OutTrace("[%s] create thread: th=0x%X base=0x%X start=0x%X\n",
                     fmttime(time0),
                     debug_event.u.CreateThread.hThread,
                     debug_event.u.CreateThread.lpThreadLocalBase,
                     debug_event.u.CreateThread.lpStartAddress);
            //SuspendThread(debug_event.u.CreateThread.hThread);
            //CloseHandle(debug_event.u.CreateThread.hThread);
            break;
        // -------------------------------------//
        // EXIT_THREAD_DEBUG_EVENT              //
        // -------------------------------------//
        case EXIT_THREAD_DEBUG_EVENT:
            OutTrace("[%s] exit thread: exitcode=0x%X\n",
                     fmttime(time0),
                     debug_event.u.ExitThread.dwExitCode);
            // commented out: apparently, there must be some previous thread that starts before the hooker
            // bContinueDebugging=false;
            break;
        // -------------------------------------//
        // LOAD_DLL_DEBUG_EVENT                 //
        // -------------------------------------//
        case LOAD_DLL_DEBUG_EVENT:
            OutTrace("[%s] load dll: base=0x%X path=\"%s\"\n",
                     fmttime(time0),
                     debug_event.u.LoadDll.lpBaseOfDll,
                     GetFileNameFromHandle(debug_event.u.LoadDll.hFile));
            //CloseHandle(debug_event.u.LoadDll.hFile);
            break;
        // -------------------------------------//
        // UNLOAD_DLL_DEBUG_EVENT               //
        // -------------------------------------//
        case UNLOAD_DLL_DEBUG_EVENT:
            OutTrace("[%s] unload dll: base=0x%X\n",
                     fmttime(time0),
                     debug_event.u.UnloadDll.lpBaseOfDll);
            break;
        // -------------------------------------//
        // OUTPUT_DEBUG_STRING_EVENT            //
        // -------------------------------------//
        case OUTPUT_DEBUG_STRING_EVENT: {
            // thanks to https://www.codeproject.com/Articles/43682/Writing-a-basic-Windows-debugger
            CStringW strEventMessage;  // Force Unicode
            OUTPUT_DEBUG_STRING_INFO &DebugString = debug_event.u.DebugString;
            WCHAR *msg = new WCHAR[DebugString.nDebugStringLength];
            // Don't care if string is ANSI, and we allocate double...
            ReadProcessMemory(pinfo.hProcess,       // HANDLE to Debuggee
                              DebugString.lpDebugStringData, // Target process' valid pointer
                              msg,                           // Copy to this address space
                              DebugString.nDebugStringLength, NULL);
            if ( DebugString.fUnicode )
                strEventMessage = msg;
            else
                strEventMessage = (char *)msg; // char* to CStringW (Unicode) conversion.
            // delete trailing \n if any
            strEventMessage.TrimRight(L" \n");
            //strEventMessage.TrimLeft(L" \n");
            OutTrace("[%s] output debug: len=%d unicode=%x msg=\"%*.*ls\"\n",
                     fmttime(time0),
                     DebugString.nDebugStringLength,
                     debug_event.u.DebugString.fUnicode,
                     DebugString.nDebugStringLength, DebugString.nDebugStringLength, strEventMessage);
            delete []msg;
        }
        break;
        // -------------------------------------//
        // EXCEPTION_DEBUG_EVENT                //
        // -------------------------------------//
        case EXCEPTION_DEBUG_EVENT: {
            // thanks to https://www.codeproject.com/Articles/43682/Writing-a-basic-Windows-debugger
            EXCEPTION_DEBUG_INFO &exception = debug_event.u.Exception;
            OutTrace("[%s] exception: code=0x%X(%s) addr=0x%X first=%x\n",
                     fmttime(time0),
                     exception.ExceptionRecord.ExceptionCode,
                     ExceptionCaption(exception.ExceptionRecord.ExceptionCode),
                     exception.ExceptionRecord.ExceptionAddress,
                     exception.dwFirstChance);
            switch( exception.ExceptionRecord.ExceptionCode) {
            case STATUS_BREAKPOINT:
                dwContinueStatus = DBG_CONTINUE;
                break;
            default:
                // There are cases where OS ignores the dwContinueStatus,
                // and executes the process in its own way.
                // For first chance exceptions, this parameter is not-important
                // but still we are saying that we have NOT handled this event.
                // Changing this to DBG_CONTINUE (for 1st chance exception also),
                // may cause same debugging event to occur continously.
                // In short, this debugger does not handle debug exception events
                // efficiently, and let's keep it simple for a while!
                // v2.05.53: try to give a grace time of 5 seconds of exception ignore
                dwContinueStatus = DBG_EXCEPTION_NOT_HANDLED;
                if((GetTickCount() - time0) < 5000) {
                    dwContinueStatus = DBG_CONTINUE;
                    Sleep(5);
                }
#if 0
                if(exception.dwFirstChance == 1) {
                    OutTrace("[%s] First chance exception at %x, exception-code: 0x%08x\n",
                             fmttime(time0),
                             exception.ExceptionRecord.ExceptionAddress,
                             exception.ExceptionRecord.ExceptionCode);
                    dwContinueStatus = DBG_EXCEPTION_NOT_HANDLED;
                    // try DBG_CONTINUE just once to avoid loops
                    if((exception.ExceptionRecord.ExceptionAddress != dwLastExceptionAddress) &&
                            (exception.ExceptionRecord.ExceptionCode != dwLastExceptionCode)) {
                        dwContinueStatus = DBG_CONTINUE;
                        dwLastExceptionAddress = exception.ExceptionRecord.ExceptionAddress;
                        dwLastExceptionCode = exception.ExceptionRecord.ExceptionCode;
                    } else
                        dwContinueStatus = DBG_EXCEPTION_NOT_HANDLED;
                    break;
                }
#endif
            }
        }
        break;
        // -------------------------------------//
        // default ???                          //
        // -------------------------------------//
        default:
            sprintf(DebugMessage, "Unknown eventcode=%x", debug_event.dwDebugEventCode);
            MessageBoxEx(0, DebugMessage, "Injection", MB_ICONEXCLAMATION, NULL);
            OutTrace("[%s] %s\n", fmttime(time0), DebugMessage);
            break;
        }
        if(bContinueDebugging)
            ContinueDebugEvent(debug_event.dwProcessId, debug_event.dwThreadId, dwContinueStatus);
        else {
            DebugSetProcessKillOnExit(FALSE);
            ContinueDebugEvent(debug_event.dwProcessId, debug_event.dwThreadId, DBG_CONTINUE);
            DebugActiveProcessStop(debug_event.dwProcessId);
        }
    }
    CloseHandle(pinfo.hThread); // no longer needed, avoid handle leakage
    CloseHandle(pinfo.hProcess); // no longer needed, avoid handle leakage
    free(p); // don't forget to free the allocated area
    return TRUE;
}
