
#define  _CRT_SECURE_NO_WARNINGS
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "ctype.h"
#include <windows.h>

void syntax() {
    printf("windump: (c)gho 2019\n");
    printf("syntax: windump <window title>\n");
    exit(0);
}

char *ExplainStyle(DWORD c) {
    static char eb[512];
    unsigned int l;
    strcpy(eb, "WS_");
    if (c & WS_BORDER) strcat(eb, "BORDER+");
    if (c & WS_CAPTION) strcat(eb, "CAPTION+");
    if (c & WS_CHILD) strcat(eb, "CHILD+");
    if (c & WS_CLIPCHILDREN) strcat(eb, "CLIPCHILDREN+");
    if (c & WS_CLIPSIBLINGS) strcat(eb, "CLIPSIBLINGS+");
    if (c & WS_DISABLED) strcat(eb, "DISABLED+");
    if (c & WS_DLGFRAME) strcat(eb, "DLGFRAME+");
    if (c & WS_GROUP) strcat(eb, "GROUP+");
    if (c & WS_HSCROLL) strcat(eb, "HSCROLL+");
    if (c & WS_MAXIMIZE) strcat(eb, "MAXIMIZE+");
    if (c & WS_MAXIMIZEBOX) strcat(eb, "MAXIMIZEBOX+");
    if (c & WS_MINIMIZE) strcat(eb, "MINIMIZE+");
    if (c & WS_MINIMIZEBOX) strcat(eb, "MINIMIZEBOX+");
    if (c & WS_POPUP) strcat(eb, "POPUP+");
    if (c & WS_SIZEBOX) strcat(eb, "SIZEBOX+");
    if (c & WS_SYSMENU) strcat(eb, "SYSMENU+");
    if (c & WS_TABSTOP) strcat(eb, "TABSTOP+");
    if (c & WS_THICKFRAME) strcat(eb, "THICKFRAME+");
    if (c & WS_TILED) strcat(eb, "TILED+");
    if (c & WS_VISIBLE) strcat(eb, "VISIBLE+");
    if (c & WS_VSCROLL) strcat(eb, "VSCROLL+");
    l = strlen(eb);
    if (l > strlen("WS_")) eb[l - 1] = 0; // delete last '+' if any
    else strcpy(eb, "WS_OVERLAPPED"); // when zero ...
    char *s = &eb[strlen(eb)];
    strcat(s, "+CS_");
    if (c & CS_VREDRAW) strcat(eb, "VREDRAW+");
    if (c & CS_HREDRAW) strcat(eb, "HREDRAW+");
    if (c & CS_DBLCLKS) strcat(eb, "DBLCLKS+");
    if (c & CS_OWNDC) strcat(eb, "OWNDC+");
    if (c & CS_CLASSDC) strcat(eb, "CLASSDC+");
    if (c & CS_PARENTDC) strcat(eb, "PARENTDC+");
    if (c & CS_NOCLOSE) strcat(eb, "NOCLOSE+");
    if (c & CS_SAVEBITS) strcat(eb, "SAVEBITS+");
    if (c & CS_BYTEALIGNCLIENT) strcat(eb, "BYTEALIGNCLIENT+");
    if (c & CS_BYTEALIGNWINDOW) strcat(eb, "BYTEALIGNWINDOW+");
    if (c & CS_GLOBALCLASS) strcat(eb, "GLOBALCLASS+");
    if (c & CS_IME) strcat(eb, "IME+");
    if (c & CS_DROPSHADOW) strcat(eb, "DROPSHADOW+");
    l = strlen(s);
    if (l > strlen("+CS_")) s[l - 1] = 0; // delete last '+' if any
    else *s = 0; // when zero ...
    return(eb);
}

char *ExplainExStyle(DWORD c) {
    static char eb[256];
    unsigned int l;
    strcpy(eb, "WS_EX_");
    if (c & WS_EX_ACCEPTFILES) strcat(eb, "ACCEPTFILES+");
    if (c & WS_EX_APPWINDOW) strcat(eb, "APPWINDOW+");
    if (c & WS_EX_CLIENTEDGE) strcat(eb, "CLIENTEDGE+");
    //if (c & WS_EX_COMPOSITED) strcat(eb, "COMPOSITED+");
    if (c & WS_EX_CONTEXTHELP) strcat(eb, "CONTEXTHELP+");
    if (c & WS_EX_CONTROLPARENT) strcat(eb, "CONTROLPARENT+");
    if (c & WS_EX_DLGMODALFRAME) strcat(eb, "DLGMODALFRAME+");
    //if (c & WS_EX_LAYERED) strcat(eb, "LAYERED+");
    if (c & WS_EX_LAYOUTRTL) strcat(eb, "LAYOUTRTL+");
    if (c & WS_EX_LEFT) strcat(eb, "LEFT+");
    if (c & WS_EX_LEFTSCROLLBAR) strcat(eb, "LEFTSCROLLBAR+");
    if (c & WS_EX_LTRREADING) strcat(eb, "LTRREADING+");
    if (c & WS_EX_MDICHILD) strcat(eb, "MDICHILD+");
    //if (c & WS_EX_NOACTIVATE) strcat(eb, "NOACTIVATE+");
    if (c & WS_EX_NOINHERITLAYOUT) strcat(eb, "NOINHERITLAYOUT+");
    if (c & WS_EX_NOPARENTNOTIFY) strcat(eb, "NOPARENTNOTIFY+");
    if (c & WS_EX_RIGHT) strcat(eb, "RIGHT+");
    if (c & WS_EX_RTLREADING) strcat(eb, "RTLREADING+");
    if (c & WS_EX_STATICEDGE) strcat(eb, "STATICEDGE+");
    if (c & WS_EX_TOOLWINDOW) strcat(eb, "TOOLWINDOW+");
    if (c & WS_EX_TOPMOST) strcat(eb, "TOPMOST+");
    if (c & WS_EX_TRANSPARENT) strcat(eb, "TRANSPARENT+");
    if (c & WS_EX_WINDOWEDGE) strcat(eb, "WINDOWEDGE+");
    l = strlen(eb);
    if (l > strlen("WS_EX_")) eb[l - 1] = 0; // delete last '+' if any
    else strcpy(eb, "WS_EX_RIGHTSCROLLBAR"); // when zero ...
    return(eb);
}

BOOL CALLBACK EnumChildWindowsProc(HWND hwnd, LPARAM lparam) {
    char *lpWTitle = (char *)lparam;
    char ThisTitle[80 + 1];
    RECT rect;
    DWORD style, exstyle;
    GetWindowText(hwnd, ThisTitle, 80);
    GetWindowRect(hwnd, &rect);
    style = GetWindowLong(hwnd, GWL_STYLE);
    exstyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    printf(">> hwnd=%x hparent=%x title=\"%s\"\n\tstyle=%08.8X(%s)\n\texstyle=%08.8X(%s)\n\tpos=(%d,%d)-(%d,%d)\n",
           hwnd,
           GetWindowLong(hwnd, GWL_HWNDPARENT),
           ThisTitle,
           style, ExplainStyle(style),
           exstyle, ExplainStyle(exstyle),
           rect.left, rect.top, rect.right, rect.bottom
          );
    return true;
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lparam) {
    char *lpWTitle = (char *)lparam;
    char ThisTitle[80 + 1];
    RECT rect;
    DWORD style, exstyle;
    GetWindowText(hwnd, ThisTitle, 80);
    if(!strcmp(lpWTitle, ThisTitle)) {
        style = GetWindowLong(hwnd, GWL_STYLE);
        exstyle = GetWindowLong(hwnd, GWL_EXSTYLE);
        GetWindowRect(hwnd, &rect);
        printf("hwnd=%x title=\"%s\"\n\tstyle=%08.8X(%s)\n\texstyle=%08.8X(%s)\n\tpos=(%d,%d)-(%d,%d)\n",
               hwnd,
               lpWTitle,
               style, ExplainStyle(style),
               exstyle, ExplainStyle(exstyle),
               rect.left, rect.top, rect.right, rect.bottom);
        EnumChildWindows(hwnd, EnumChildWindowsProc, NULL);
    }
    return true;
}

int main(int argc, char *argv[]) {
    char *lpWTitle;
    if(argc < 2) syntax();
    lpWTitle = argv[1];
    while(true) {
        printf("---------\n");
        EnumWindows(EnumWindowsProc, (LPARAM)lpWTitle);
        Sleep(1000);
    }
}

