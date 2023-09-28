//  Exam28.cpp
//  9/21/2000  (rk)
//  Last modified: 3/10/2002  (RK)
//  test the routine KILL_PROC_BY_NAME to terminate a process

#include "stdafx.h"
#include <windows.h>
#include <tlhelp32.h>

// v2.05.46 fix: since iCb is espressed in bytes, its value must be multiplied by sizeof(DWORD)
// or, better yet, use sizeof(aiPID)

#define MAXPROCESSARRAYSIZE 2000

int KillProcByName(char *sProcessTail, BOOL bKill, BOOL bKillAll)
// Created: 6/23/2000  (RK)
// Last modified: 3/10/2002  (RK)
// Please report any problems or bugs to kochhar@physiology.wisc.edu
// The latest version of this routine can be found at:
//     http://www.neurophys.wisc.edu/ravi/software/killproc/
// Terminate the process "sProcessTail" if it is currently running
// This works for Win/95/98/ME and also Win/NT/2000/XP
// The process name is case-insensitive, i.e. "notepad.exe" and "NOTEPAD.EXE"
// will both work (for sProcessTail)
// Return codes are as follows:
//   0   = Process was successfully terminated
//   603 = Process was not currently running
//   604 = No permission to terminate process
//   605 = Unable to load PSAPI.DLL
//   602 = Unable to terminate process for some other reason
//   606 = Unable to identify system type
//   607 = Unsupported OS
//   632 = Invalid process name
//   700 = Unable to get procedure address from PSAPI.DLL
//   701 = Unable to get process list, EnumProcesses failed
//   702 = Unable to load KERNEL32.DLL
//   703 = Unable to get procedure address from KERNEL32.DLL
//   704 = CreateToolhelp32Snapshot failed
// Change history:
//   modified 3/8/2002  - Borland-C compatible if BORLANDC is defined as
//                        suggested by Bob Christensen
//   modified 3/10/2002 - Removed memory leaks as suggested by
//					      Jonathan Richard-Brochu (handles to Proc and Snapshot
//                        were not getting closed properly in some cases)
{
    BOOL bResult;
    int iLen, iLenP, indx;
    OSVERSIONINFO osvi;
    char szToTermUpper[MAX_PATH];
    DWORD iFound = 0;
    HINSTANCE hInstLib;
    // Transfer Process name into "szToTermUpper" and
    // convert it to upper case
    iLenP = strlen(sProcessTail);
    if(iLenP < 1 || iLenP > MAX_PATH) return 632;
    for(indx = 0; indx < iLenP; indx++) szToTermUpper[indx] = toupper(sProcessTail[indx]);
    szToTermUpper[iLenP] = 0;
    // First check what version of Windows we're in
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    bResult = GetVersionEx(&osvi);
    if(!bResult) return 606;    // Unable to identify system version
    // At Present we only support Win/NT/2000/XP/...Win10
    if(osvi.dwPlatformId != VER_PLATFORM_WIN32_NT) return 607;
    // PSAPI Function Pointers.
    BOOL (WINAPI * lpfEnumProcesses)( DWORD *, DWORD cb, DWORD * );
    BOOL (WINAPI * lpfEnumProcessModules)( HANDLE, HMODULE *, DWORD, LPDWORD );
    DWORD (WINAPI * lpfGetModuleBaseName)( HANDLE, HMODULE, LPTSTR, DWORD );
    // allow for up to MAXPROCESSARRAYSIZE entries.
    DWORD aiPID[MAXPROCESSARRAYSIZE], iNumProc;
    DWORD iCbneeded, i;
    char szName[MAX_PATH];
    HANDLE hProc;
    HMODULE hMod;
    // Load library and get the procedures explicitly. We do
    // this so that we don't have to worry about modules using
    // this code failing to load under Windows 9x, because
    // it can't resolve references to the PSAPI.DLL.
    hInstLib = LoadLibraryA("PSAPI.DLL");
    if(hInstLib == NULL) return 605;
    // Get procedure addresses.
    lpfEnumProcesses = (BOOL(WINAPI *)(DWORD *, DWORD, DWORD *))GetProcAddress(hInstLib, "EnumProcesses");
    lpfEnumProcessModules = (BOOL(WINAPI *)(HANDLE, HMODULE *, DWORD, LPDWORD))GetProcAddress(hInstLib, "EnumProcessModules");
    lpfGetModuleBaseName = (DWORD (WINAPI *)(HANDLE, HMODULE, LPTSTR, DWORD ))GetProcAddress(hInstLib, "GetModuleBaseNameA");
    if (lpfEnumProcesses == NULL ||
            lpfEnumProcessModules == NULL ||
            lpfGetModuleBaseName == NULL) {
        FreeLibrary(hInstLib);
        return 700;
    }
    bResult = lpfEnumProcesses(aiPID, sizeof(aiPID), &iCbneeded);
    if(!bResult) {
        // Unable to get process list, EnumProcesses failed
        FreeLibrary(hInstLib);
        return 701;
    }
    // How many processes are there?
    iNumProc = iCbneeded / sizeof(DWORD);
    //OutTrace("KillProcByName: checking %d processes\n", iNumProc);
    // Get and match the name of each process
    for(i = 0; i < iNumProc; i++) {
        // Get the (module) name for this process
        iLen = 0;
        // First, get a handle to the process
        hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, aiPID[i]);
        // Now, get the process name
        if(hProc) {
            if(lpfEnumProcessModules(hProc, &hMod, sizeof(hMod), &iCbneeded))
                iLen = lpfGetModuleBaseName(hProc, hMod, szName, MAX_PATH);
        }
        CloseHandle(hProc);
        //OutTrace("KillProcByName: checking against \"%s\"...\n", szName);
        // We will match regardless of lower or upper case
        if(iLen && strcmp(_strupr(szName), szToTermUpper) == 0) {
            // Process found, now terminate it
            iFound = 1;
            if(!bKill) {
                FreeLibrary(hInstLib);
                return 0; // just tell you found it.
            }
            if(osvi.dwMajorVersion == 5) {
                // First open for termination
                hProc = OpenProcess(PROCESS_TERMINATE, FALSE, aiPID[i]);
                if(hProc) {
                    if(TerminateProcess(hProc, 0)) {
                        // process terminated
                        CloseHandle(hProc);
                        if (!bKillAll) {
                            FreeLibrary(hInstLib);
                            return 0;
                        }
                    } else {
                        // Unable to terminate process
                        CloseHandle(hProc);
                        if (!bKillAll) {
                            FreeLibrary(hInstLib);
                            return 602;
                        }
                    }
                } else {
                    // Unable to open process for termination
                    if (!bKillAll) {
                        FreeLibrary(hInstLib);
                        return 604;
                    }
                }
            } else {
                // this will kill the whole process tree - though with no feedback retcode.
                extern void KillProcessTree(DWORD);
                KillProcessTree(aiPID[i]);
                if (!bKillAll) {
                    FreeLibrary(hInstLib);
                    return 0;
                }
            }
        }
    }
    FreeLibrary(hInstLib);
    if(iFound == 0) return 603;
    return 0;
}

// tries to get the full pathname of a running process
// returning 0 if succeeds.

BOOL GetProcByName(char *sProcessTail, char *sFullPath) {
    BOOL bResult;
    DWORD aiPID[MAXPROCESSARRAYSIZE], iNumProc;
    DWORD iCbneeded, i, iFound = 0;
    char szName[MAX_PATH], szToTermUpper[MAX_PATH];
    HANDLE hProc;
    OSVERSIONINFO osvi;
    HINSTANCE hInstLib;
    int iLen, iLenP, indx;
    HMODULE hMod;
    // Transfer Process name into "szToTermUpper" and
    // convert it to upper case
    iLenP = strlen(sProcessTail);
    if(iLenP < 1 || iLenP > MAX_PATH) return FALSE;
    for(indx = 0; indx < iLenP; indx++)
        szToTermUpper[indx] = toupper(sProcessTail[indx]);
    szToTermUpper[iLenP] = 0;
    // PSAPI Function Pointers.
    BOOL (WINAPI * lpfEnumProcesses)( DWORD *, DWORD cb, DWORD * );
    BOOL (WINAPI * lpfEnumProcessModules)( HANDLE, HMODULE *, DWORD, LPDWORD );
    DWORD (WINAPI * lpfGetModuleBaseName)( HANDLE, HMODULE, LPTSTR, DWORD );
    DWORD (WINAPI * lpfGetModuleFileName)( HANDLE, HMODULE, LPTSTR, DWORD );
    // First check what version of Windows we're in
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    bResult = GetVersionEx(&osvi);
    if(!bResult)     // Unable to identify system version
        return 606;
    // At Present we only support Win/NT/2000/XP/.../Win10
    if(osvi.dwPlatformId != VER_PLATFORM_WIN32_NT) return 607;
    // Load library and get the procedures explicitly. We do
    // this so that we don't have to worry about modules using
    // this code failing to load under Windows 9x, because
    // it can't resolve references to the PSAPI.DLL.
    hInstLib = LoadLibraryA("PSAPI.DLL");
    if(hInstLib == NULL) return 605;
    // Get procedure addresses.
    lpfEnumProcesses = (BOOL(WINAPI *)(DWORD *, DWORD, DWORD *)) GetProcAddress(hInstLib, "EnumProcesses");
    lpfEnumProcessModules = (BOOL(WINAPI *)(HANDLE, HMODULE *, DWORD, LPDWORD)) GetProcAddress(hInstLib, "EnumProcessModules");
    lpfGetModuleBaseName = (DWORD (WINAPI *)(HANDLE, HMODULE, LPTSTR, DWORD )) GetProcAddress(hInstLib, "GetModuleBaseNameA");
    lpfGetModuleFileName = (DWORD (WINAPI *)(HANDLE, HMODULE, LPTSTR, DWORD )) GetProcAddress(hInstLib, "GetModuleFileNameExA");
    if(lpfEnumProcesses == NULL ||
            lpfEnumProcessModules == NULL ||
            lpfGetModuleBaseName == NULL ||
            lpfGetModuleFileName == NULL) {
        FreeLibrary(hInstLib);
        return 700;
    }
    bResult = lpfEnumProcesses(aiPID, sizeof(aiPID), &iCbneeded);
    if(!bResult) {
        // Unable to get process list, EnumProcesses failed
        FreeLibrary(hInstLib);
        return 701;
    }
    // How many processes are there?
    iNumProc = iCbneeded / sizeof(DWORD);
    // Get and match the name of each process
    for(i = 0; i < iNumProc; i++) {
        // Get the (module) name for this process
        iLen = 0;
        // First, get a handle to the process
        hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, aiPID[i]);
        // Now, get the process name
        if(hProc) {
            if(lpfEnumProcessModules(hProc, &hMod, sizeof(hMod), &iCbneeded) )
                iLen = lpfGetModuleBaseName(hProc, hMod, szName, MAX_PATH);
        }
        // We will match regardless of lower or upper case
        if(iLen && strcmp(_strupr(szName), szToTermUpper) == 0) {
            // Process found
            iFound = 1;
            lpfGetModuleFileName(hProc, hMod, sFullPath, MAX_PATH);
            FreeLibrary(hInstLib);
            CloseHandle(hProc);
            return 0; // just tell you found it.
        }
        CloseHandle(hProc);
    }
    FreeLibrary(hInstLib);
    if(iFound == 0) return 603;
    return 0;
}

