// dxwndhost.cpp : Application and class definitions
//

#include "stdafx.h"
#include "dxwndhost.h"

#include "MainFrm.h"
#include "dxwndhostDoc.h"
#include "dxwndhostView.h"
#include "CAboutDlg.h"
#include "afxwin.h"

#define g_szMailslot "\\\\.\\mailslot\\DxWndMailSlot"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#define FORCEDEBUGMODE

extern int KillProcByName(char *, BOOL, BOOL);

/////////////////////////////////////////////////////////////////////////////
// CDxwndhostApp

BEGIN_MESSAGE_MAP(CDxwndhostApp, CWinApp)
    //{{AFX_MSG_MAP(CDxwndhostApp)
    ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
    ON_COMMAND(ID_HELP_VIEWHELP, OnViewHelp)
    //}}AFX_MSG_MAP
    // Basic file command
    ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
    ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// custom command line parsing:
// CNewCommandLineInfo Class constructor

// too do: eliminate nasty global variables....
UINT m_StartToTray = FALSE;
BOOL m_StayHidden = FALSE;
UINT m_InitialState = DXW_ACTIVE;
#ifdef FORCEDEBUGMODE
BOOL gbDebug = TRUE;
#else
BOOL gbDebug = FALSE;
#endif
BOOL gbExpertMode = FALSE;
BOOL gWarnOnExit = FALSE;
BOOL g32BitIcons = FALSE;
BOOL gGrayIcons = TRUE;
BOOL gAutoSave = FALSE;
BOOL gTransientMode = FALSE;
BOOL gNoIconMode = FALSE;
BOOL gAutoHideMode = FALSE;
BOOL gHideOnEscape = FALSE;
BOOL gQuietMode = FALSE;
BOOL gReadOnlyMode = FALSE;
BOOL gNoIconSpinning = FALSE;
BOOL gMustDie = FALSE;
int iProgIndex;
DWORD SysColors[32];
char gInitialWorkingDir[MAX_PATH + 1];
WORD GammaRamp[256 * 3];

class CNewCommandLineInfo : public CCommandLineInfo {
    void ParseParam(LPCTSTR lpszParam, BOOL bFlag, BOOL bLast);
};

BOOL LangSelected = FALSE;
char LangString[20 + 1] = {0};

static void ShootCommand(char *szBuffer) {
    // from https://www.codeproject.com/Articles/13724/Windows-IPC
    HANDLE hMailslot;
    DWORD cbBytes;
    hMailslot = CreateFile(
                    g_szMailslot,          // mailslot name
                    GENERIC_WRITE,         // mailslot write only
                    FILE_SHARE_READ,       // required for mailslots
                    NULL,                  // default security attributes
                    OPEN_EXISTING,         // opens existing mailslot
                    FILE_ATTRIBUTE_NORMAL, // normal attributes
                    NULL);                 // no template file
    if (INVALID_HANDLE_VALUE == hMailslot) {
        MessageBox(0, "Error occurred while connecting to active DxWnd", "Error", 0);
        exit(1);
    }
    WriteFile(
        hMailslot,            // handle to mailslot
        szBuffer,             // buffer to write from
        strlen(szBuffer) + 1, // number of bytes to write, include the NULL
        &cbBytes,             // number of bytes written
        NULL);                // not overlapped I/O
    exit(0);
}

void RemoveFileSpec(char *path) {
    char *p = &path[strlen(path) - 1];
    while ((*p != '\\') && (p > path)) p--;
    if(p > path) *p = 0;
}

void CNewCommandLineInfo::ParseParam(LPCTSTR lpszParam, BOOL bFlag, BOOL bLast) {
    // syntax:
    // /l:<path> -- tells a running DxWnd session to load <path> config file to be merged in dxwnd.ini
    // /y:<path> -- same as /l:<path> plus execute the program referred into the loaded configuration
    // /y:#<n> -- execute the <n>th program referred into the loaded configuration
    // /z -- tells a running DxWnd session to completely clear (zap) its configuration
    // /s -- tells a running DxWnd session to save the current configuration
    // /t -- start minimized in tray icon
    // /i -- start in idle state
    // /debug -- start in debug mode
    // /lang=<XX> -- loads the language resources in Resources_<XX>.dll extension
    // /c:<path> -- loads <path> config file instead of default dxwnd.ini
    // /e -- terminates (Ends) the active dxwnd session
    // /r:<n> -- run the n-th game in configuration and terminate together with it
    // /a -- auto-hide mode while a game is running
    // /q -- quiet mode, no error/message dialog boxes
    // /h -- stay hidden in icon tray
    // /n -- no icon Spinning
    // /x -- do not show the program's icon in the icon tray
    // /o -- read-only mode, no warning on save
    if(bFlag) {
        CString sParam(lpszParam);
        if (sParam.MakeLower() == "h") {
            // stay hidden: starts iconized and returns there whenever you run a program
            m_StayHidden = TRUE;
            m_StartToTray = TRUE;
            gAutoHideMode = TRUE;
            return;
        }
        if (sParam.MakeLower() == "t") {
            m_StartToTray = TRUE;
            return;
        }
        if (sParam.MakeLower() == "i") {
            m_InitialState = DXW_IDLE;
            return;
        }
        if (sParam.MakeLower() == "a") {
            gAutoHideMode = TRUE;
            return;
        }
        if (sParam.MakeLower() == "o") {
            gReadOnlyMode = TRUE;
            return;
        }
        if (sParam.MakeLower() == "debug") {
            gbDebug = TRUE;
            return;
        }
        if (sParam.MakeLower().Left(5) == "lang=") {
            HMODULE ResLib;
            CString Lang;
            Lang = sParam.MakeLower().Right(2);
            ResLib = LoadLibrary("Resources_" + Lang + ".dll");
            if(ResLib) {
                AfxSetResourceHandle(ResLib);
                LangSelected = TRUE;
            } else MessageBoxEx(NULL, "Missing language \"" + Lang + "\"\nUsing default language \"en\"", "Warning", MB_OK, NULL);
            //strcpy(LangString, sParam.MakeLower().Mid(5));
            return;
        }
        if (sParam.Left(2).MakeLower() == "c:") {
            //strcpy_s(gInitPath, sizeof(gInitPath)-1, sParam.Mid(2,sizeof(gInitPath)-1));
            strcpy(gInitPath, gInitialWorkingDir);
            strcat_s(gInitPath, sizeof(gInitPath) - 1, "\\");
            strcat_s(gInitPath, sizeof(gInitPath) - 1, sParam.Mid(2, sizeof(gInitPath) - 1));
            return;
        }
        if ((sParam.Left(2).MakeLower() == "l:") ||
                (sParam.Left(2).MakeLower() == "y:"))
            ShootCommand(sParam.GetBuffer());
        if (sParam.MakeLower() == "z")
            ShootCommand(sParam.GetBuffer());
        if (sParam.MakeLower() == "s")
            ShootCommand(sParam.GetBuffer());
        if (sParam.MakeLower() == "e") {
            // Exit (kill) existing DxWnd session
            KillProcByName("DxWnd.exe", TRUE, FALSE);
            exit(0);
        }
        if (sParam.Left(2).MakeLower() == "r:") {
            gTransientMode = TRUE;
            m_StartToTray = TRUE;
            char *p = (char *)sParam.GetString();
            iProgIndex = atoi(&p[2]);
            return;
        }
        if (sParam.MakeLower() == "x") {
            gNoIconMode = TRUE;
            return;
        }
        if (sParam.MakeLower() == "q") {
            // (Q)uiet mode: no message dialogs on screen
            gQuietMode = TRUE;
        }
        if (sParam.MakeLower() == "n") {
            // (N)o icon spinning - active icon in icon tray does not spin.
            gNoIconSpinning = TRUE;
        }
        if (sParam.MakeLower() == "m") {
            // (M)ove working directory to DxWnd folder. Needed to avoid DxWnd
            // starting in System32 folder when run at startup!
            char sPath[MAX_PATH + 1];
            GetModuleFileName(NULL, sPath, MAX_PATH);
            //MessageBox(NULL, sPath, "path", 0);
            RemoveFileSpec(sPath);
            //MessageBox(NULL, sPath, "folder", 0);
            if(!SetCurrentDirectory(sPath))
                MessageBox(NULL, "SetCurrentDirectory error", "DxWnd", 0);
            ;
            strcpy(gInitialWorkingDir, sPath);
            strcpy(gInitPath, gInitialWorkingDir);
            strcat_s(gInitPath, sizeof(gInitPath) - 1, "\\dxwnd.ini");
            //MessageBox(NULL, gInitPath, "inipath", 0);
        }
    }
    // Call the base class to ensure proper command line processing
    CCommandLineInfo::ParseParam(lpszParam, bFlag, bLast);
}

/////////////////////////////////////////////////////////////////////////////
// CDxwndhostApp Class constructor

CDxwndhostApp::CDxwndhostApp() {
}

/////////////////////////////////////////////////////////////////////////////
// The only CDxwndhostApp object

CDxwndhostApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CDxwndhostApp Class initialization

BOOL CDxwndhostApp::InitInstance() {
    AfxEnableControlContainer();
    // save gInitialWorkingDir for later use to always access files by absolute path
    // save gInitPath, but the value could be overwritten by command line parsing
    GetCurrentDirectory(MAX_PATH, gInitialWorkingDir);
    strcpy(gInitPath, gInitialWorkingDir);
    strcat_s(gInitPath, sizeof(gInitPath), "\\dxwnd.ini");
    // standard initialization
    // DDE file open: Parse command line for standard shell commands and so on.
    // Parse command line for standard shell commands, DDE, file open
    // and user-defined flags. The CCommandLine class has been replaced
    CNewCommandLineInfo cmdInfo;
    ParseCommandLine(cmdInfo);
    if(!gbDebug) gbDebug = GetPrivateProfileInt("window", "debug", 0, gInitPath); // debug flag set from config file
    if(!gbExpertMode) gbExpertMode = GetPrivateProfileInt("window", "expert", 0, gInitPath); // debug flag set from config file
    if(!gAutoHideMode) gAutoHideMode = GetPrivateProfileInt("window", "autohide", 0, gInitPath); // debug flag set from config file
    if(!gHideOnEscape) gHideOnEscape = GetPrivateProfileInt("window", "hideonesc", 0, gInitPath); // debug flag set from config file
    gWarnOnExit = GetPrivateProfileInt("window", "warnonexit", 0, gInitPath); // WarnOnExit flag set from config file
    g32BitIcons = GetPrivateProfileInt("window", "32biticons", 0, gInitPath); // 32 bit icons flag set from config file
    gGrayIcons = GetPrivateProfileInt("window", "grayicons", 1, gInitPath); // gray icons of missing or invalid program entries
    gAutoSave = GetPrivateProfileInt("window", "autosave", 0, gInitPath); // automatically saves configuration when updated
    if(gReadOnlyMode) gAutoSave = FALSE; // don't auto-save in read-only mode
    // Show initial banner to publicize help
    char sHelp[5 + 1];
    char *sHelpBanner =
        "ATTENTION: to read DxWnd documentation\n"
        "click on Help -> View Help menu command\n"
        "or open .\\help\\DxWnd.html in your browser";
    GetPrivateProfileStringA("window", "showhelp", "YES", sHelp, 5, gInitPath);
    if(!strcmp(sHelp, "YES")) {
        MessageBox(NULL, sHelpBanner, "DxWnd", MB_ICONEXCLAMATION | MB_OK);
        WritePrivateProfileString("window", "showhelp", "NO", gInitPath);
    }
    // Check for write capabilities on configuration file
    if(!gReadOnlyMode) {
        if(WritePrivateProfileString("window", "writetest", "OK", gInitPath))
            WritePrivateProfileString("window", "writetest", NULL, gInitPath);
        else {
            MessageBoxEx(NULL,
                         "No write capability on dxwnd.ini file\nWarning: can't save your configuration!",
                         "Warning", MB_OK, NULL);
        }
    }
    // Register the document template.
    CSingleDocTemplate *pDocTemplate;
    pDocTemplate = new CSingleDocTemplate(
        gbExpertMode ? IDR_MAINFRAME_EX : IDR_MAINFRAME,
        RUNTIME_CLASS(CDxwndhostDoc),
        RUNTIME_CLASS(CMainFrame),       // SDI main frame window
        RUNTIME_CLASS(CDxwndhostView));
    AddDocTemplate(pDocTemplate);
    if(!LangSelected) {
        LANGID LangId;
        char LangString[20 + 1];
        GetPrivateProfileString("window", "lang", "", LangString, 20 + 1, gInitPath);
        if(!strcmp(LangString, "default") || !strlen(LangString)) { // if no specification, or lang=default
            // do nothing
        }
        if(!strcmp(LangString, "automatic")) { // lang=automatic
            HMODULE ResLib;
            LangId = GetUserDefaultUILanguage();
            // other codes to be implemented:
            // 409 (2-9): english (default)
            // 411 (2-11): japanese
            switch(LangId & 0x1FF) {
            case 0x04: // chinese family
                ResLib = LoadLibrary("Resources_CN.dll");
                if(ResLib) AfxSetResourceHandle(ResLib);
                else MessageBoxEx(NULL, "Missing language \"CN\"\nUsing default language \"en\"", "Warning", MB_OK, NULL);
                break;
            case 0x10: // 410 - italian, 810 - switzerland italian
                ResLib = LoadLibrary("Resources_IT.dll");
                if(ResLib) AfxSetResourceHandle(ResLib);
                else MessageBoxEx(NULL, "Missing language \"IT\"\nUsing default language \"en\"", "Warning", MB_OK, NULL);
                break;
            case 0x19: // 419 - russian
                ResLib = LoadLibrary("Resources_RU.dll");
                if(ResLib) AfxSetResourceHandle(ResLib);
                else MessageBoxEx(NULL, "Missing language \"RU\"\nUsing default language \"en\"", "Warning", MB_OK, NULL);
                break;
            default:
                //char sBuf[81];
                //sprintf(sBuf, "Got Lang=%x(%x-%x)", LangId, LangId>>9, (LangId & 0x1FF));
                //MessageBox(NULL, sBuf, "LangId", MB_OK);
                break;
            }
        }
        if(strcmp(LangString, "automatic") && strcmp(LangString, "default") && strlen(LangString)) { // lang=something different from both automatic and default
            HMODULE ResLib;
            CString Lang;
            Lang.SetString(LangString);
            ResLib = LoadLibrary("Resources_" + Lang + ".dll");
            if(ResLib) AfxSetResourceHandle(ResLib);
            else MessageBoxEx(NULL, "Missing language \"" + Lang + "\"\nUsing default language \"en\"", "Warning", MB_OK, NULL);
        }
    }
    // Dispatch commands specified on the command line.
    if (!ProcessShellCommand(cmdInfo))
        return FALSE;
    m_pMainWnd->SetWindowText("DXWnd");
    RECT rect;
    m_pMainWnd->GetWindowRect(&rect);
    rect.right = rect.left + 320;
    rect.bottom = rect.top + 200;
    //m_pMainWnd->MoveWindow(&rect, TRUE);
    m_pMainWnd->ShowWindow(SW_SHOW);
    m_pMainWnd->UpdateWindow();
    BOOL bCheckAdminRights = GetPrivateProfileInt("window", "checkadmin", 0, gInitPath);
    extern BOOL DxSelfElevate(CDxwndhostView *);
    OSVERSIONINFO osver = { sizeof(osver) };
    // self elevation at start if configured for the whole DxWnd session
    if (bCheckAdminRights && (GetVersionEx(&osver)) && (osver.dwMajorVersion >= 6))
        DxSelfElevate((CDxwndhostView *)NULL);
    // save system colors for later recovery
    for(int index = COLOR_SCROLLBAR; index <= COLOR_MENUBAR; index++) SysColors[index] = 0; // initialize
    for(int index = COLOR_SCROLLBAR; index <= COLOR_MENUBAR; index++) SysColors[index] =::GetSysColor(index);
    GetDeviceGammaRamp(GetDC(NULL), &GammaRamp);
    return TRUE;
}



// The application command to run the dialog
void CDxwndhostApp::OnAppAbout() {
    char tmp[32], ver[32];
    CAboutDlg aboutDlg;
    GetDllVersion(tmp);
    sprintf(ver, "DxWnd version %s", tmp);
    OutTrace("OnAppAbout: DxWnd version %s\n", tmp);
    aboutDlg.m_Version = ver;
    HMODULE hplay = LoadLibrary("dxwplay.dll");
    if(hplay) {
        typedef LPCSTR (WINAPI * pplr_version_type)(void);
        pplr_version_type pplr_version;
        pplr_version = (pplr_version_type)GetProcAddress(hplay, "plr_version");
        if(pplr_version) {
            strcpy(tmp, (*pplr_version)());
            sprintf(ver, "DxwPlay version %s", tmp);
            OutTrace("OnAppAbout: DxwPlay version %s\n", tmp);
            aboutDlg.m_DxwPlayVersion = ver;
        } else {
            // older versions without plr_version interface!!
            OutTrace("OnAppAbout: DxwPlay unknown version v0.00\n");
            aboutDlg.m_DxwPlayVersion = "DxwPlay version v0.00";
        }
    } else
        OutTrace("LoadLibrary dxwplay failed: error=%d\n", GetLastError());
    aboutDlg.m_Thanks = "";
    aboutDlg.DoModal();
    aboutDlg.KillTimer(ID_HELP_SCROLL);
}

void CDxwndhostApp::OnViewHelp() {
    extern void ShowHelp(char *);
    ShowHelp(NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CDxwndhostApp Message Handler

char *GetTSCaption(int shift) {
    static char *sTSCaption[17] = {
        "x16", "x12", "x8", "x6",
        "x4", "x3", "x2", "x1.5",
        "x1",
        ":1.5", ":2", ":3", ":4",
        ":6", ":8", ":12", ":16"
    };
    if (shift < -8 || shift > 8) return "???";
    return sTSCaption[shift + 8];
}

void CAboutDlg::OnStnClickedAnimation() {
    // TODO: Add your control notification handler code here
}
