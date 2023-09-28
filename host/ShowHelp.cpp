#include "stdafx.h"

extern char gInitPath[MAX_PATH + 1];

void ShowHelp(char *topic) {
    HINSTANCE ret;
    char sHelpPath[MAX_PATH];
    GetPrivateProfileString("window", "help", ".\\help\\DxWnd.html", sHelpPath, MAX_PATH, gInitPath);
    if(topic) {
        sHelpPath[strlen(sHelpPath) - strlen("DxWnd.html")] = 0;
        strcat(sHelpPath, topic);
        strcat(sHelpPath, ".html");
    }
    ret = ShellExecute(NULL, "open", sHelpPath, NULL, NULL, SW_SHOWNORMAL);
    if((DWORD)ret <= 32) {
        char *m;
        switch((DWORD)ret) {
        case 0:
            m = "Out of memory resources";
            break;
        case ERROR_BAD_FORMAT:
            m = "Invalid .exe file";
            break;
        case SE_ERR_ACCESSDENIED:
            m = "Access denied";
            break;
        case SE_ERR_ASSOCINCOMPLETE:
            m = "File name association incomplete or invalid";
            break;
        case SE_ERR_DDEBUSY:
            m = "DDE busy";
            break;
        case SE_ERR_DDEFAIL:
            m = "DDE failed";
            break;
        case SE_ERR_DDETIMEOUT:
            m = "DDE timed out";
            break;
        case SE_ERR_DLLNOTFOUND:
            m = "DLL not found";
            break;
        case SE_ERR_FNF:
            m = "File not found";
            break;
        case SE_ERR_NOASSOC:
            m = "No association with file extension";
            break;
        case SE_ERR_OOM:
            m = "Not enough memory";
            break;
        case SE_ERR_PNF:
            m = "Path not found";
            break;
        case SE_ERR_SHARE:
            m = "Sharing violation";
            break;
        default:
            m = "Unknown error";
            break;
        }
        MessageBox(0, m, "DxWnd error", MB_ICONEXCLAMATION | MB_OK);
    }
}