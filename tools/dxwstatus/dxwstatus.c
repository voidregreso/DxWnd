#define  _CRT_SECURE_NO_WARNINGS
#include "windows.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "ctype.h"

char *status(DWORD s) {
    char *ret = "unknown";
    switch(s) {
    case 0:
        ret = "idle";
        break;
    case 128:
        ret = "aborted";
        break;
    case 258:
        ret = "active";
        break;
    }
    return ret;
}

int main(int argc, char *argv[]) {
    HANDLE hLockMutex;
    DWORD ret;
    hLockMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, "Lock_Mutex");
    if(!hLockMutex) hLockMutex = CreateMutex(0, FALSE, "Lock_Mutex");
    while(1) {
        ret = WaitForSingleObject(hLockMutex, 0);
        ReleaseMutex(hLockMutex);
        printf("wait=%d status=%s\n", ret, status(ret));
        Sleep(100);
    }
}