#define  _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <tlhelp32.h>
#include <shlwapi.h>
#include <conio.h>
#include <stdio.h>

#define WIN32_LEAN_AND_MEAN

#define true 1
#define false 0

#include "Winternl.h"

BOOL Inject(DWORD pID, const char *DLL_NAME) {
    HANDLE hProc, hThread;
    HMODULE hLib;
    char buf[50] = {0};
    LPVOID RemoteString, LoadLibAddy;
    if(!pID) return false;
    //hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID); // not working on Win XP
    hProc = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, pID);
    if(!hProc) {
        sprintf(buf, "OpenProcess() failed: pid=%#x err=%d", pID, GetLastError());
        MessageBox(NULL, buf, "Loader", MB_OK);
        return false;
    }
    hLib = GetModuleHandle("kernel32.dll");
    if(!hLib) {
        sprintf(buf, "GetModuleHandle(\"kernel32.dll\") failed: err=%d", pID, GetLastError());
        MessageBox(NULL, buf, "Loader", MB_OK);
        return false;
    }
    LoadLibAddy = (LPVOID)GetProcAddress(hLib, "LoadLibraryA");
    if(!LoadLibAddy) {
        sprintf(buf, "GetProcAddress(\"LoadLibraryA\") failed: err=%d", pID, GetLastError());
        MessageBox(NULL, buf, "Loader", MB_OK);
        return false;
    }
    // Allocate space in the process for the DLL
    RemoteString = (LPVOID)VirtualAllocEx(hProc, NULL, strlen(DLL_NAME), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    // Write the string name of the DLL in the memory allocated
    if(!WriteProcessMemory(hProc, (LPVOID)RemoteString, DLL_NAME, strlen(DLL_NAME), NULL)) {
        sprintf(buf, "WriteProcessMemory() failed: err=%d", pID, GetLastError());
        MessageBox(NULL, buf, "Loader", MB_OK);
        return false;
    }
    // Load the DLL
    hThread = CreateRemoteThread(hProc, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibAddy, (LPVOID)RemoteString, 0, NULL);
    if(!hThread) {
        sprintf(buf, "CreateRemoteThread() failed: pid=%#x err=%d", pID, GetLastError());
        MessageBox(NULL, buf, "Loader", MB_OK);
        CloseHandle(hProc);
        return false;
    }
    if(!SetThreadPriority(hThread, THREAD_PRIORITY_TIME_CRITICAL)) {
        sprintf(buf, "SetThreadPriority() failed: err=%d", GetLastError());
        MessageBox(NULL, buf, "Loader", MB_OK);
    }
    // Free/Release/Close everything
    // v2.05.22 fix: must give some time to target program to use the allocated string
    // or otherwise DO NOT delete the string, or the target program will crash !!
    Sleep(500);
    if(!VirtualFreeEx(hProc, RemoteString, 0, MEM_RELEASE)) {
        sprintf(buf, "VirtualFreeEx() failed: err=%d", pID, GetLastError());
        MessageBox(NULL, buf, "Loader", MB_OK);
        return false;
    }
    CloseHandle(hThread);
    CloseHandle(hProc);
    //CloseHandle(hLib);
    return true;
}

BOOL CommitAndInject(DWORD pID, const char *DLL_NAME, LPVOID lpAddress, DWORD dwSize) {
    HANDLE hProc, hThread;
    HMODULE hLib;
    char buf[50] = {0};
    LPVOID RemoteString, LoadLibAddy;
    LPVOID lpReserved;
    if(!pID) return false;
    //hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID); // not working on Win XP
    hProc = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, pID);
    if(!hProc) {
        sprintf(buf, "OpenProcess() failed: pid=%#x err=%d", pID, GetLastError());
        MessageBox(NULL, buf, "Loader", MB_OK);
        return false;
    }
    hLib = GetModuleHandle("kernel32.dll");
    if(!hLib) {
        sprintf(buf, "GetModuleHandle(\"kernel32.dll\") failed: err=%d", pID, GetLastError());
        MessageBox(NULL, buf, "Loader", MB_OK);
        return false;
    }
    LoadLibAddy = (LPVOID)GetProcAddress(hLib, "LoadLibraryA");
    if(!LoadLibAddy) {
        sprintf(buf, "GetProcAddress(\"LoadLibraryA\") failed: err=%d", pID, GetLastError());
        MessageBox(NULL, buf, "Loader", MB_OK);
        return false;
    }
    //MessageBox(NULL, "Reservation", "DxWnd", 0);
    lpReserved = (LPVOID)VirtualAllocEx(hProc, lpAddress, dwSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if(!lpReserved) {
        sprintf(buf, "VirtualAllocEx(%#x, %d) commit failed: err=%d", lpAddress, dwSize, GetLastError());
        MessageBox(NULL, buf, "Loader", MB_OK);
    }
    // Allocate space in the process for the DLL
    RemoteString = (LPVOID)VirtualAllocEx(hProc, NULL, strlen(DLL_NAME), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    // Write the string name of the DLL in the memory allocated
    if(!WriteProcessMemory(hProc, (LPVOID)RemoteString, DLL_NAME, strlen(DLL_NAME), NULL)) {
        sprintf(buf, "WriteProcessMemory() failed: err=%d", pID, GetLastError());
        MessageBox(NULL, buf, "Loader", MB_OK);
        return false;
    }
    // Load the DLL
    hThread = CreateRemoteThread(hProc, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibAddy, (LPVOID)RemoteString, 0, NULL);
    // give it a little time to safely use the RemoteString data
    Sleep(20);
    // Free/Release/Close everything
    if(!VirtualFreeEx(hProc, RemoteString, 0, MEM_RELEASE)) {
        sprintf(buf, "VirtualFreeEx() failed: err=%d", pID, GetLastError());
        MessageBox(NULL, buf, "Loader", MB_OK);
        return false;
    }
    if(!VirtualFreeEx(hProc, lpAddress, dwSize, MEM_DECOMMIT)) {
        sprintf(buf, "VirtualFreeEx(%#x) decommit failed: err=%d", pID, lpAddress, GetLastError());
        MessageBox(NULL, buf, "Loader", MB_OK);
        return false;
    }
    if(!hThread) {
        sprintf(buf, "CreateRemoteThread() failed: pid=%#x err=%d", pID, GetLastError());
        MessageBox(NULL, buf, "Loader", MB_OK);
        CloseHandle(hProc);
        return false;
    }
    if(!SetThreadPriority(hThread, THREAD_PRIORITY_TIME_CRITICAL)) {
        sprintf(buf, "SetThreadPriority() failed: err=%d", GetLastError());
        MessageBox(NULL, buf, "Loader", MB_OK);
    }
    CloseHandle(hThread);
    CloseHandle(hProc);
    //CloseHandle(hLib);
    return true;
}

#define STATUS_SUCCESS    ((NTSTATUS)0x000 00000L)
#define ThreadQuerySetWin32StartAddress 9

LPVOID GetThreadStartAddress(HANDLE hThread) {
    NTSTATUS ntStatus;
    HANDLE hDupHandle;
    HMODULE hLibNTHandle;
    LPVOID dwStartAddress;
    typedef NTSTATUS (WINAPI * NtQueryInformationThread_Type)(HANDLE, THREADINFOCLASS, PVOID, ULONG, PULONG);
    hLibNTHandle = GetModuleHandle("ntdll.dll");
    if(!hLibNTHandle) return 0;
    NtQueryInformationThread_Type NtQueryInformationThread =
        (NtQueryInformationThread_Type)GetProcAddress(hLibNTHandle, "NtQueryInformationThread");
    CloseHandle(hLibNTHandle);
    if(NtQueryInformationThread == NULL) return 0;
    HANDLE hCurrentProcess = GetCurrentProcess();
    if(!DuplicateHandle(hCurrentProcess, hThread, hCurrentProcess, &hDupHandle, THREAD_QUERY_INFORMATION, FALSE, 0)) {
        SetLastError(ERROR_ACCESS_DENIED);
        return 0;
    }
    ntStatus = NtQueryInformationThread(hDupHandle, (THREADINFOCLASS)ThreadQuerySetWin32StartAddress, &dwStartAddress, sizeof(DWORD), NULL);
    CloseHandle(hDupHandle);
    //if(ntStatus != STATUS_SUCCESS) return 0;
    return dwStartAddress;
}