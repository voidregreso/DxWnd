
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <winbase.h>
#include <tchar.h>
#include <stdio.h>
#pragma comment(lib, "User32.lib")

extern LPCSTR GetFlagCaption(int, int);

void syntax() {
    printf("usage:\n");
    printf("\tExportFinder [+|-]<flagname> | /dx=<n> | /p\n");
}

void doFinder(char *flagspec) {
    int mask;
    int i;
    WIN32_FIND_DATA ffd;
    TCHAR szDir[MAX_PATH];
    HANDLE hFind = INVALID_HANDLE_VALUE;
    DWORD dwError = 0;
    BOOL NotMode = FALSE;
    int dxversion = -1;
    char *key = NULL;
    BOOL bSearchFlag = FALSE;
    BOOL bPause = FALSE;
    BOOL bPathMatch = FALSE;
#if 0
    sscanf(flagspec, "%d:%x", &flag, &mask);
    printf("looking for flag%d:%x\n", flag, mask);
#endif
    switch(flagspec[0]) {
    case '+':
        bSearchFlag = TRUE;
        flagspec++;
        break;
    case '-':
        bSearchFlag = TRUE;
        flagspec++;
        NotMode = TRUE;
        break;
    case '/':
        if(flagspec[1] == 'd') sscanf(flagspec, "/dx=%d", &dxversion);
        if(flagspec[1] == 'p') bPathMatch = TRUE;
        break;
    default:
        break;
    }
    if(bSearchFlag) {
        LPCSTR GetFlagCaption(int, int);
        for(i = 0; i < 32; i++) if(!strcmp(flagspec, GetFlagCaption(0, i))) {
                key = "flag0";
                mask = (0x1 << i);
            }
        for(i = 0; i < 32; i++) if(!strcmp(flagspec, GetFlagCaption(1, i))) {
                key = "flagg0";
                mask = (0x1 << i);
            }
        for(i = 0; i < 32; i++) if(!strcmp(flagspec, GetFlagCaption(2, i))) {
                key = "flagh0";
                mask = (0x1 << i);
            }
        for(i = 0; i < 32; i++) if(!strcmp(flagspec, GetFlagCaption(3, i))) {
                key = "flagi0";
                mask = (0x1 << i);
            }
        for(i = 0; i < 32; i++) if(!strcmp(flagspec, GetFlagCaption(4, i))) {
                key = "flagj0";
                mask = (0x1 << i);
            }
        for(i = 0; i < 32; i++) if(!strcmp(flagspec, GetFlagCaption(5, i))) {
                key = "flagk0";
                mask = (0x1 << i);
            }
        for(i = 0; i < 32; i++) if(!strcmp(flagspec, GetFlagCaption(6, i))) {
                key = "flagl0";
                mask = (0x1 << i);
            }
        for(i = 0; i < 32; i++) if(!strcmp(flagspec, GetFlagCaption(7, i))) {
                key = "flagm0";
                mask = (0x1 << i);
            }
        for(i = 0; i < 32; i++) if(!strcmp(flagspec, GetFlagCaption(8, i))) {
                key = "flagn0";
                mask = (0x1 << i);
            }
        for(i = 0; i < 32; i++) if(!strcmp(flagspec, GetFlagCaption(9, i))) {
                key = "flago0";
                mask = (0x1 << i);
            }
        for(i = 0; i < 32; i++) if(!strcmp(flagspec, GetFlagCaption(10, i))) {
                key = "flagp0";
                mask = (0x1 << i);
            }
        for(i = 0; i < 32; i++) if(!strcmp(flagspec, GetFlagCaption(11, i))) {
                key = "flagq0";
                mask = (0x1 << i);
            }
        if(!key) {
            printf("error: unknown flag \"%s\"\n", flagspec);
            return;
        }
    }
    strcpy(szDir, TEXT(".\\exports\\*.DXW"));
    // Find the first file in the directory.
    hFind = FindFirstFile(szDir, &ffd);
    if (INVALID_HANDLE_VALUE == hFind) {
        printf("error %d\n", dwError);
        if(bPause) getchar();
        return;
    }
    // List all the files in the directory with some info about them.
    do {
        DWORD iflag;
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            //_tprintf(TEXT("  %s   <DIR>\n"), ffd.cFileName);
        } else {
            char fullpath[MAX_PATH];
            int dx;
            strcpy(fullpath, ".\\exports\\");
            strcat(fullpath, ffd.cFileName);
            if(bPathMatch) {
                char path[MAX_PATH + 1];
                GetPrivateProfileString("target", "path0", "", path, MAX_PATH, fullpath);
                if(path[1] == ':') { // check only absolute paths
                    FILE *fp;
                    if(fp = fopen(path, "rb")) {
                        printf("%s: %s\n", ffd.cFileName, path);
                        fclose(fp);
                    }
                }
            } else {
                iflag = GetPrivateProfileInt("target", key, 0, fullpath);
                dx = GetPrivateProfileInt("target", "ver0", 0, fullpath);
                //printf("iflag=%x mask=%x file=%s\n", iflag, mask, ffd.cFileName);
                if(NotMode) {
                    if(!(iflag & mask)) printf("~ %s\n", ffd.cFileName);
                } else {
                    if((iflag & mask) || (dx == dxversion)) printf("%s\n", ffd.cFileName);
                }
            }
        }
    } while (FindNextFile(hFind, &ffd) != 0);
    dwError = GetLastError();
    if (dwError != ERROR_NO_MORE_FILES)
        printf("error %d\n", dwError);
    FindClose(hFind);
    printf("*** END ***\n");
    if(bPause) getchar();
}

int main(int argc, char *argv[]) {
    char cmdbuf[80];
    printf("DxWnd export finder (c)GHO 2014-2020\n");
    if (argc < 2) {
        syntax();
        printf("enter command:\n");
        while(TRUE) {
            gets(cmdbuf);
            if(strlen(cmdbuf) < 2) break;
            doFinder(cmdbuf);
            printf("enter command or press ENTER to quit:\n");
        }
    } else
        doFinder(argv[1]);
}
