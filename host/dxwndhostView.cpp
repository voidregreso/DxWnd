// dxwndhostView.cpp : CDxwndhostView Define the behavior of the class.
//

#include "stdafx.h"
#include "shlwapi.h"
#include "TlHelp32.h"
#include "CoolUtils.h"

#include "dxwndhost.h"

#include "dxwndhostDoc.h"
#include "dxwndhostView.h"
#include "TargetDlg.h"
#include "SystemTray.h"
#include "StatusDialog.h"
#include "DesktopDialog.h"
#include "PaletteDialog.h"
#include "TimeSliderDialog.h"
#include "VJoyDialog.h"
#include "ShimsDialog.h"
#include "CDChargerDialog.h"
#include "GammaRampDialog.h"
#include "GammaCtrlDialog.h"
#include "MousePositionDialog.h"
#include "CDPlayerDialog.h"
#include "ViewFlagsDialog.h"
#include "CGlobalSettings.h"
#include "CGlobalStartUp.h"
#include "GameIcons.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define	WM_ICON_NOTIFY WM_APP+10
#define strcasecmp lstrcmpi

extern BOOL IsWinXP(void);
extern BOOL gHideOnEscape;

extern UINT m_StartToTray;
extern UINT m_InitialState;
extern BOOL m_StayHidden;
extern int KillProcByName(char *, BOOL, BOOL);
extern BOOL gTransientMode;
extern BOOL gAutoHideMode;
extern BOOL gbDebug;
extern BOOL gbExpertMode;
extern BOOL gMustDie;
extern BOOL gGrayIcons;
extern BOOL gAutoSave;
extern BOOL gNoIconMode;
extern BOOL gNoIconSpinning;
extern int iProgIndex;
extern void CheckSafeDiscVersion(char *);

PRIVATEMAP *pTitles; // global ptr: get rid of it!!
TARGETMAP *pTargets; // idem.

#define LOCKINJECTIONTHREADS

char gInitPath[MAX_PATH + 1] = ""; // don't put it into the class because it must be used after destructor
char gExportPath[MAX_PATH + 1];
CWnd *pParent;

// beware: it must operate upon count+1 sized arrays
char *strnncpy(char *dest, char *src, size_t count) {
    dest[count] = 0;
    return strncpy(dest, src, count);
}

static char *Escape(char *s, char **dest) {
    if(!*dest)	*dest = (char *)malloc(strlen(s) + 100);
    else		*dest = (char *)realloc(*dest, strlen(s) + 100);
    char *t = *dest;
    for(; *s; s++) {
        switch(*s) {
        case '\n':
            *t++ = '\\';
            *t++ = 'n';
            break;
        case '\r':
            break;
        default:
            *t++ = *s;
            break;
        }
    }
    *t = 0;
    return *dest;
}

static char *Unescape(char *s, char **dest) {
    if(!*dest)	*dest = (char *)malloc(strlen(s) + 100);
    else		*dest = (char *)realloc(*dest, strlen(s) + 100);
    char *t = *dest;
    for(; *s; s++) {
        if((*s == '\\') && (*(s + 1) == 'n')) {
            *t++ = '\r';
            *t++ = '\n';
            s++;
        } else
            *t++ = *s;
    }
    *t = 0;
    return *dest;
}

static void RecoverGammaRamp() {
    WORD CurGammaRamp[256 * 3];
    extern WORD GammaRamp[];
    extern BOOL gQuietMode;
    if(!GetDeviceGammaRamp(GetDC(NULL), &CurGammaRamp)) return;
    if(memcmp((LPVOID)CurGammaRamp, (LPVOID)GammaRamp, sizeof(CurGammaRamp))) {
        if (gQuietMode)
            SetDeviceGammaRamp(GetDC(NULL), &GammaRamp);
        else {
            if (MessageBox(NULL, "Gamma ramp updated. Recover?", "warning", MB_YESNO | MB_ICONQUESTION) == IDYES)
                SetDeviceGammaRamp(GetDC(NULL), &GammaRamp);
        }
    }
}

static BOOL FileExists(char *sTargetPath) {
    BOOL ret;
    HANDLE hFile;
    hFile = CreateFile(sTargetPath, // file name
                       GENERIC_READ,          // open for reading
                       0,                     // do not share
                       NULL,                  // default security
                       OPEN_EXISTING,         // existing file only
                       FILE_ATTRIBUTE_NORMAL, // normal file
                       NULL);                 // no template
    if (hFile == INVALID_HANDLE_VALUE)
        ret = FALSE;
    else {
        CloseHandle(hFile);
        ret = TRUE;
    }
    return ret;
}

void GetFolderFromPath(char *path) {
    for(char *c = &path[strlen(path) - 1]; (c > path) && (*c != '\\'); c--) *c = 0;
}

static void FlushRegistryFile(char *Registry) {
    FILE *regfp;
    char path[MAX_PATH + 1];
    sprintf(path, "%s\\dxwnd.reg", gInitialWorkingDir);
    regfp = fopen(path, "w");
    if(regfp == NULL)MessageBoxLang(DXW_STRING_VREG_ERROR, DXW_STRING_ERROR, MB_ICONERROR | MB_OK);
    fwrite(Registry, strlen(Registry), 1, regfp);
    fputs("\n", regfp);
    fclose(regfp);
}
/////////////////////////////////////////////////////////////////////////////
// CDxwndhostView

IMPLEMENT_DYNCREATE(CDxwndhostView, CListView)

BEGIN_MESSAGE_MAP(CDxwndhostView, CListView)
    //{{AFX_MSG_MAP(CDxwndhostView)
    ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
    ON_COMMAND(ID_EXPLORE, OnExplore)
    ON_COMMAND(ID_LOG_VIEW, OnViewLog)
    ON_COMMAND(ID_PLOG_VIEWTAIL, OnViewLogTail)
    ON_COMMAND(ID_LOG_DELETE, OnDeleteLog)
    ON_COMMAND(ID_TASK_KILL, OnKill)
    ON_COMMAND(ID_TASK_PAUSE, OnPause)
    ON_COMMAND(ID_TASK_RESUME, OnResume)
    ON_COMMAND(ID_WINDOW_MINIMIZE, OnWindowMinimize)
    ON_COMMAND(ID_WINDOW_RESTORE, OnWindowRestore)
    ON_COMMAND(ID_WINDOW_CLOSE, OnWindowClose)
    ON_COMMAND(ID_WINDOW_MINIMIZE, OnWindowMinimize)
    ON_COMMAND(ID_TASKBAR_HIDE, OnTaskbarHide)
    ON_COMMAND(ID_TASKBAR_SHOW, OnTaskbarShow)
    ON_COMMAND(ID_MODIFY, OnModify)
    ON_COMMAND(ID_ADD, OnAdd)
    ON_COMMAND(ID_PDUPLICATE, OnDuplicate)
    ON_COMMAND(ID_PEXPORT, OnExport)
    ON_COMMAND(ID_PKILL, OnProcessKill)
    ON_COMMAND(ID_PKILLALL, OnProcessKillAll)
    ON_COMMAND(ID_FILE_IMPORT, OnImport)
    ON_COMMAND(ID_FILE_OPENCONFIG, OnOpenConfig)
    ON_COMMAND(ID_FILE_EXPLOREDXWND, OnExploreDxWnd)
    ON_COMMAND(ID_DELETE, OnDelete)
    ON_COMMAND(ID_FILE_SORTPROGRAMSLIST, OnSort)
    ON_COMMAND(ID_EDIT_GLOBALSETTINGS, OnGlobalSettings)
    ON_COMMAND(ID_EDIT_STARTUPSETTINGS, OnGlobalStartUp)
    ON_COMMAND(ID_OPTIONS_EXPERTMODE, OnExpertModeToggle)
    ON_COMMAND(ID_FILE_CLEARALLLOGS, OnClearAllLogs)
    ON_COMMAND(ID_FILE_GOTOTRAYICON, OnGoToTrayIcon)
    ON_COMMAND(ID_FILE_SAVE, OnSaveFile)
    ON_COMMAND(ID_HOOK_START, OnHookStart)
    ON_COMMAND(ID_HOOK_STOP, OnHookStop)
    ON_COMMAND(ID_DXAPP_EXIT, OnExit)
    ON_COMMAND(ID_RUN, OnRun)
    ON_COMMAND(ID_RUNPANIC, OnRunPanic)
    ON_COMMAND(ID_TRAY_RESTORE, OnTrayRestore)
    ON_COMMAND(ID_VIEW_STATUS, OnViewStatus)
    ON_COMMAND(ID_VIEW_SHIMS, OnViewShims)
    ON_COMMAND(ID_VIEW_GAMMARAMP, OnViewGammaRamp)
    ON_COMMAND(ID_VIEW_MOUSEPOSITION, OnViewMousePosition)
    ON_COMMAND(ID_VIEW_CDPLAYER, OnViewCDPlayer)
    ON_COMMAND(ID_VIEW_DESKTOP, OnViewDesktop)
    ON_COMMAND(ID_VIEW_PALETTE, OnViewPalette)
    ON_COMMAND(ID_VIEW_TIMESLIDER, OnViewTimeSlider)
    ON_COMMAND(ID_VIEW_VIRTUALJOYSTICK, OnViewVirtualJoystick)
    ON_COMMAND(ID_VIEW_DXWNDLOGS, OnViewGUILog)
    ON_COMMAND(ID_DESKTOPCOLORDEPTH_8BPP, OnDesktopcolordepth8bpp)
    ON_COMMAND(ID_DESKTOPCOLORDEPTH_16BPP, OnDesktopcolordepth16bpp)
    ON_COMMAND(ID_DESKTOPCOLORDEPTH_24BPP, OnDesktopcolordepth24bpp)
    ON_COMMAND(ID_DESKTOPCOLORDEPTH_32BPP, OnDesktopcolordepth32bpp)
    ON_COMMAND(ID_TOOLS_RECOVERSCREENMODE, OnRecoverScreenMode)
    ON_COMMAND(ID_TOOLS_CLEARCOMPATIBILITYFLAGS, OnClearCompatibilityFlags)
    ON_COMMAND(ID_TOOLS_BLACKER, OnRunBlacker)
    ON_COMMAND(ID_TOOLS_ASSOCIATE, OnAssociate)
    ON_COMMAND(ID_TOOLS_DISASSOCIATE, OnDisassociate)
    ON_COMMAND(ID_TOOLS_DISASSOCIATE, OnDisassociate)
    ON_COMMAND(ID_TOOLS_CDCHARGER, OnCDCharger)
    ON_COMMAND(ID_TOOLS_RECOVERGAMMARAMP, OnRecoverGammaRamp)
    ON_COMMAND(ID_TOOLS_GAMMACONTROL, OnGammaCtrl)
    ON_COMMAND(ID_MOVE_TOP, OnMoveTop)
    ON_COMMAND(ID_MOVE_UP, OnMoveUp)
    ON_COMMAND(ID_MOVE_DOWN, OnMoveDown)
    ON_COMMAND(ID_MOVE_BOTTOM, OnMoveBottom)
    ON_COMMAND(ID_VIEW_FLAGS, OnViewFlags)
    ON_COMMAND(ID_VIEW_IMPORT_TABLE, OnViewImportTable)
    ON_COMMAND(ID_PROXY_REMOVE, OnProxyRemove)
    ON_COMMAND(ID_PROXY_DDRAW, OnProxyDdraw)
    ON_COMMAND(ID_PROXY_D3D8, OnProxyD3D8)
    ON_COMMAND(ID_PROXY_D3D9, OnProxyD3D9)
    ON_COMMAND(ID_PROXY_WING32, OnProxyWinG32)
    ON_COMMAND(ID_PROXY_DINPUT, OnProxyDInput)
    ON_COMMAND(ID_PROXY_WINMM, OnProxyWinMM)
    ON_COMMAND(ID_LIST_ICON, OnListIcons)
    ON_COMMAND(ID_LIST_DETAIL, OnListDetails)
    ON_WM_KEYUP()
    ON_WM_KEYDOWN()
    ON_WM_RBUTTONDOWN()
    ON_WM_HOTKEY()
    ON_WM_DROPFILES()

    //ON_WM_SIZE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

// v2.1.68: dialog box for status check.
BOOL CheckStatus() {
    if(GetHookStatus(NULL) == DXW_RUNNING) {
        MessageBoxLang(DXW_STRING_WAITTASK, DXW_STRING_WARNING, MB_OK | MB_ICONEXCLAMATION);
        return TRUE;
    }
    return FALSE;
}

// v2.1.78: recover desktop settings.
static void RevertScreenChanges(DEVMODE *InitDevMode) {
    DEVMODE CurDevMode;
    EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &CurDevMode);
    if ((CurDevMode.dmPelsHeight != InitDevMode->dmPelsHeight) ||
            (CurDevMode.dmPelsWidth != InitDevMode->dmPelsWidth) ||
            (CurDevMode.dmBitsPerPel != InitDevMode->dmBitsPerPel)) {
        if (MessageBoxLang(DXW_STRING_RESTORE, DXW_STRING_WARNING, MB_YESNO | MB_ICONQUESTION) == IDYES)
            ChangeDisplaySettings(InitDevMode, 0);
    }
}

void SetPrivateMapFromDialog(PRIVATEMAP *p, CTargetDlg *dlg) {
    strnncpy(p->title, (char *)dlg->m_Title.GetString(), MAX_TITLE);
    strnncpy(p->cmdline, (char *)dlg->m_CmdLine.GetString(), MAX_PATH);
    if(p->notes)
        p->notes = (char *)realloc(p->notes, strlen(dlg->m_Notes.GetString()) + 1);
    else
        p->notes = (char *)malloc(strlen(dlg->m_Notes.GetString()) + 1);
    strcpy(p->notes, (char *)dlg->m_Notes.GetString());
    if(p->registry)
        p->registry = (char *)realloc(p->registry, strlen(dlg->m_Registry.GetString()) + 1);
    else
        p->registry = (char *)malloc(strlen(dlg->m_Registry.GetString()) + 1);
    strcpy(p->registry, (char *)dlg->m_Registry.GetString());
    strnncpy(p->launchpath, (char *)dlg->m_LaunchPath.GetString(), MAX_PATH);
    strnncpy(p->startfolder, (char *)dlg->m_StartFolder.GetString(), MAX_PATH);
    strnncpy(p->FakeHDPath, (char *)dlg->m_FakeHDPath.GetString(), MAX_PATH);
    strnncpy(p->FakeCDPath, (char *)dlg->m_FakeCDPath.GetString(), MAX_PATH);
    strnncpy(p->FakeCDLabel, (char *)dlg->m_FakeCDLabel.GetString(), 256);
    p->CommitAddress = dlg->m_CommitAddress;
    p->CommitLength = dlg->m_CommitLength;
    if(dlg->m_Icon) {
        CGameIcons *ic = new(CGameIcons);
        char *hex = ic->Serialize(dlg->m_Icon);
        if(hex) {
            p->IconString = (CString)hex;
            p->Icon = dlg->m_Icon;
            free(hex);
        }
    }
    p->updated = TRUE;
}

static void SetDlgFromPrivateMap(PRIVATEMAP *p, CTargetDlg *dlg) {
    dlg->m_Title = p->title;
    dlg->m_CmdLine = p->cmdline;
    dlg->m_Notes = CString(p->notes);
    dlg->m_Registry = CString(p->registry);
    dlg->m_LaunchPath = p->launchpath;
    dlg->m_StartFolder = p->startfolder;
    dlg->m_FakeHDPath = CString(p->FakeHDPath);
    dlg->m_FakeCDPath = CString(p->FakeCDPath);
    dlg->m_FakeCDLabel = CString(p->FakeCDLabel);
    dlg->m_CommitAddress = p->CommitAddress;
    dlg->m_CommitLength = p->CommitLength;
    CGameIcons *ic = new(CGameIcons);
    //MessageBox(p->IconString.GetBuffer(), "hex", 0);
    dlg->m_Icon = ic->DeSerialize(p->IconString.GetBuffer());
}

void SetTargetFromDlg(TARGETMAP *t, CTargetDlg *dlg) {
    if(dlg->m_DXVersion > 1) dlg->m_DXVersion += 5;
    t->dxversion = dlg->m_DXVersion;
    t->monitorid = (dlg->m_MonitorId == -1) ? -1 : dlg->m_MonitorId - 1;
    t->FilterId = dlg->m_FilterId;
    t->RendererId = dlg->m_RendererId;
    t->MaxDdrawInterface = dlg->m_MaxDdrawInterface + 1;
    t->SlowRatio = dlg->m_SlowRatio;
    t->coordinates = dlg->m_Coordinates;
    t->flags = 0;
    t->flags2 = 0;
    t->flags3 = 0;
    t->flags4 = 0;
    t->flags5 = 0;
    t->flags6 = 0;
    t->flags7 = 0;
    t->flags8 = 0;
    t->flags9 = 0;
    t->flags10 = 0;
    t->flags11 = 0;
    t->flags12 = 0;
    t->flags13 = 0;
    t->flags14 = 0;
    t->flags15 = 0;
    t->flags16 = 0;
    t->tflags = 0;
    t->dflags = 0;
    t->dflags2 = 0;
    if(dlg->m_UnNotify) t->flags |= UNNOTIFY;
    if(dlg->m_Windowize) t->flags2 |= WINDOWIZE;
    if(dlg->m_HookDLLs) t->flags3 |= HOOKDLLS;
    //if(dlg->m_AnsiWide) t->flags5 |= ANSIWIDE;
    if(dlg->m_HookNoRun) t->flags7 |= HOOKNORUN;
    if(dlg->m_CopyNoShims) t->flags7 |= COPYNOSHIMS;
    if(dlg->m_HotRegistry) t->flags9 |= HOTREGISTRY;
    if(dlg->m_HookNoUpdate) t->flags7 |= HOOKDOUPDATE;
    if(dlg->m_TerminateOnClose) t->flags6 |= TERMINATEONCLOSE;
    if(dlg->m_ConfirmOnClose) t->flags6 |= CONFIRMONCLOSE;
    if(dlg->m_EmulateRegistry) t->flags3 |= EMULATEREGISTRY;
    if(dlg->m_OverrideRegistry) t->flags4 |= OVERRIDEREGISTRY;
    if(dlg->m_Wow64Registry) t->flags6 |= WOW64REGISTRY;
    if(dlg->m_Wow32Registry) t->flags6 |= WOW32REGISTRY;
    if(dlg->m_HookEnabled) t->flags3 |= HOOKENABLED;
    //if(dlg->m_SetCmdLine) t->flags12 |= SETCMDLINE;
    if(dlg->m_NeedAdminCaps) t->flags |= NEEDADMINCAPS;
    if(dlg->m_UseShortPath) t->flags11 |= USESHORTPATH;
    if(dlg->m_NoBanner) t->flags2 |= NOBANNER;
    if(dlg->m_HotPatch) t->flags4 |= HOTPATCH;
    if(dlg->m_FullScreenOnly) t->flags3 |= FULLSCREENONLY;
    if(dlg->m_ShowHints) t->flags7 |= SHOWHINTS;
    if(dlg->m_Frontend) t->flags6 |= FRONTEND;
    if(dlg->m_BackgroundPriority) t->flags8 |= BACKGROUNDPRIORITY;
    if(dlg->m_PeekAllMessages) t->flags3 |= PEEKALLMESSAGES;
    if(dlg->m_NoWinPosChanges) t->flags5 |= NOWINPOSCHANGES;
    if(dlg->m_MessagePump) t->flags5 |= MESSAGEPUMP;
    if(dlg->m_ClipMenu) t->flags8 |= CLIPMENU;
    if(dlg->m_NoMouseEvents) t->flags9 |= NOMOUSEEVENTS;
    if(dlg->m_FixMouseRawInput) t->flags10 |= FIXMOUSERAWINPUT;
    if(dlg->m_MouseShield) t->flags10 |= MOUSESHIELD;
    if(dlg->m_AdaptMouseSpeed) t->flags12 |= ADAPTMOUSESPEED;
    if(dlg->m_FixMouseLParam) t->flags12 |= FIXMOUSELPARAM;
    if(dlg->m_SwallowMouseMove) t->flags5 |= SWALLOWMOUSEMOVE;
    if(dlg->m_UnnotifyInactive) t->flags11 |= UNNOTIFYINACTIVE;
    if(dlg->m_MouseMoveByEvent) t->flags9 |= MOUSEMOVEBYEVENT;
    if(dlg->m_LockFPSCorner) t->flags3 |= LOCKFPSCORNER;
    if(dlg->m_SetZBuffer16Bit) t->flags3 |= SETZBUFFER16BIT;
    if(dlg->m_SetZBuffer24Bit) t->flags3 |= SETZBUFFER24BIT;
    switch(dlg->m_InjectionMode) {
    case 0:
        break;
    case 1:
        t->flags2 |= STARTDEBUG;
        break;
    case 2:
        t->flags7 |= INJECTSUSPENDED;
        break;
    case 3:
        t->flags12 |= INJECTPROXY;
        break;
    }
    switch(dlg->m_InitColorDepth) {
    case 0:
        break;
    case 1:
        t->flags2 |= INIT8BPP;
        break;
    case 2:
        t->flags2 |= INIT16BPP;
        break;
    case 3:
        t->flags7 |= INIT24BPP;
        break;
    case 4:
        t->flags7 |= INIT32BPP;
        break;
    }
    switch(dlg->m_DCEmulationMode) {
    case 0:
        break;
    case 1:
        t->flags2 |= GDISTRETCHED;
        break;
    case 2:
        t->flags3 |= GDIEMULATEDC;
        break;
    case 3:
        t->flags6 |= SHAREDDC;
        break;
    }
    switch(dlg->m_ResTypes) {
    case 0:
        t->flags4 |= SUPPORTSVGA;
        break;
    case 1:
        t->flags4 |= SUPPORTHDTV;
        break;
    case 2:
        t->flags4 |= NATIVERES;
        break;
    case 3:
        t->flags4 |= (SUPPORTSVGA | SUPPORTHDTV);
        break;
    }
    switch(dlg->m_MouseVisibility) {
    case 0:
        break;
    case 1:
        t->flags |= HIDEHWCURSOR;
        break;
    case 2:
        t->flags2 |= SHOWHWCURSOR;
        break;
    }
    switch(dlg->m_MouseClipper) {
    case 0:
        break;
    case 1:
        t->flags |= DISABLECLIPPING;
        break;
    case 2:
        t->flags |= CLIPCURSOR;
        break;
    case 3:
        t->flags |= CLIPCURSOR;
        t->flags8 |= CLIPLOCKED;
        break;
    }
    switch(dlg->m_OffendingMessages) {
    case 0:
        break;
    case 1:
        t->flags3 |= FILTERMESSAGES;
        break;
    case 2:
        t->flags3 |= DEFAULTMESSAGES;
        break;
    }
    switch(dlg->m_TextureHandling) {
    case 0:
        break;
    case 1:
        t->flags5 |= TEXTUREHIGHLIGHT;
        break;
    case 2:
        t->flags5 |= TEXTUREDUMP;
        break;
    case 3:
        t->flags5 |= TEXTUREHACK;
        break;
    case 4:
        t->flags5 |= TEXTURETRANSP;
        break;
    }
    switch(dlg->m_SonProcessMode) {
    case 0:
        break;
    case 1:
        t->flags4 |= SUPPRESSCHILD;
        break;
    case 2:
        t->flags5 |= DEBUGSON;
        break;
    case 3:
        t->flags5 |= INJECTSON;
        break;
    }
    switch(dlg->m_IATAlignedMode) {
    case 1:
        t->flags9 |= IATBYTEALIGNED;
        break;
    case 0:
        t->flags9 |= IATWORDALIGNED;
        break;
    case 2:
        t->flags9 |= NOIATSCAN;
        break;
    case 3:
        t->flags10 |= PEFILEHOOK;
        break;
    }
    switch(dlg->m_LogMode) {
    case 0:
        break;
    case 1:
        t->tflags |= (OUTTRACE | ERASELOGFILE);
        break;
    case 2:
        t->tflags |= OUTTRACE;
        break;
    case 3:
        t->tflags |= (OUTTRACE | OUTSEPARATED);
        break;
    case 4:
        t->tflags |= (OUTTRACE | OUTCIRCULAR);
        break;
    }
    switch(dlg->m_WinMovementType) {
    case 0:
        break;
    case 1:
        t->flags |= LOCKWINPOS;
        break;
    case 2:
        t->flags |= LOCKWINPOS;
        t->flags2 |= LOCKEDSIZE;
        break;
    case 3:
        t->flags |= LOCKWINPOS;
        t->flags7 |= ANCHORED;
        break;
    }
    switch(dlg->m_VSyncMode) {
    case 0:
        break;
    case 1:
        t->flags8 |= FORCEVSYNC;
        break;
    case 2:
        t->flags8 |= FORCENOVSYNC;
        break;
    }
    switch(dlg->m_VSyncImpl) {
    case 0:
        break;
    case 1:
        t->flags8 |= VSYNCSCANLINE;
        break;
    case 2:
        t->flags |= SAVELOAD;
        break;
    }
    switch(dlg->m_WaitMode) {
    case 0:
        break;
    case 1:
        t->flags8 |= FORCEWAIT;
        break;
    case 2:
        t->flags8 |= FORCENOWAIT;
        break;
    }
    switch(dlg->m_ClipperMode) {
    case 0:
        break;
    case 1:
        t->flags3 |= FORCECLIPPER;
        break;
    case 2:
        t->flags |= SUPPRESSCLIPPING;
        break;
    case 3:
        t->flags5 |= MAXCLIPPER;
        break;
    }
    switch(dlg->m_FourCCMode) {
    case 0:
        break;
    case 1:
        t->flags10 |= HWFOURCC;
        break;
    case 2:
        t->flags10 |= SWFOURCC;
        break;
    }
    switch(dlg->m_TextureFileFormat) {
    case 0:
        break;
    case 1:
        t->flags8 |= RAWFORMAT;
        break;
    case 2:
        t->flags8 |= DDSFORMAT;
        break;
    }
    switch(dlg->m_WindowStyle) {
    case 0:
        break;
    case 1:
        t->flags2 |= MODALSTYLE;
        break;
    case 2:
        t->flags |= FIXWINFRAME;
        break;
    case 3:
        t->flags9 |= FIXTHINFRAME;
        break;
    }
    switch(dlg->m_ShowFPS) {
    case 0:
        break;
    case 1:
        t->flags2 |= SHOWFPSOVERLAY;
        break;
    case 2:
        t->flags2 |= SHOWFPS;
        break;
    }
    switch(dlg->m_ZOrder) {
    case 0:
        break;
    case 1:
        t->flags5 |= UNLOCKZORDER;
        break;
    case 2:
        t->flags9 |= LOCKTOPZORDER;
        break;
    }
    switch(dlg->m_VertexProcessing) {
    case 0:
        break;
    case 1:
        t->flags10 |= FORCEHWVERTEXPROC;
        break;
    case 2:
        t->flags10 |= FORCESWVERTEXPROC;
        break;
    case 3:
        t->flags10 |= FORCEMXVERTEXPROC;
        break;
    }
    switch(dlg->m_GFocusMode) {
    case 0:
        break;
    case 1:
        t->flags5 |= GLOBALFOCUSON;
        break;
    case 2:
        t->flags5 |= GLOBALFOCUSOFF;
        break;
    }
    switch(dlg->m_AltTabMode) {
    case 0:
        break;
    case 1:
        t->flags7 |= NODISABLEALTTAB;
        break;
    case 2:
        t->flags5 |= DISABLEALTTAB;
        break;
    }
    if(dlg->m_BilinearFilter) t->flags5 |= BILINEARFILTER;
    if(dlg->m_BlurEffect) t->flags12 |= BLUREFFECT;
    if(dlg->m_HookDI) t->flags |= HOOKDI;
    if(dlg->m_HookDI8) t->flags |= HOOKDI8;
    if(dlg->m_EmulateRelMouse) t->flags6 |= EMULATERELMOUSE;
    if(dlg->m_ScaleRelMouse) t->flags6 |= SCALERELMOUSE;
    if(dlg->m_SkipDevTypeHID) t->flags7 |= SKIPDEVTYPEHID;
    if(dlg->m_SuppressDIErrors) t->flags7 |= SUPPRESSDIERRORS;
    if(dlg->m_SharedKeyboard) t->flags7 |= SHAREDKEYBOARD;
    if(dlg->m_ModifyMouse) t->flags |= MODIFYMOUSE;
    if(dlg->m_VirtualJoystick) t->flags6 |= VIRTUALJOYSTICK;
    if(dlg->m_HideJoysticks) t->flags9 |= HIDEJOYSTICKS;
    if(dlg->m_Unacquire) t->flags6 |= UNACQUIRE;
    if(dlg->m_OutDebug) t->tflags |= OUTDEBUG;
    if(dlg->m_CursorTrace) t->tflags |= OUTCURSORTRACE;
    if(dlg->m_OutDebugString) t->tflags |= OUTDEBUGSTRING;
    if(dlg->m_AddTimeStamp) t->tflags |= ADDTIMESTAMP;
    if(dlg->m_AddRelativeTime) t->tflags |= ADDRELATIVETIME;
    if(dlg->m_AddThreadID) t->tflags |= ADDTHREADID;
    if(dlg->m_OutWinMessages) t->tflags |= OUTWINMESSAGES;
    if(dlg->m_OutDWTrace) t->tflags |= OUTDXWINTRACE;
    if(dlg->m_OutOGLTrace) t->tflags |= OUTOGLTRACE;
    if(dlg->m_OutHexTrace) t->tflags |= OUTHEXTRACE;
    if(dlg->m_OutSDLTrace) t->tflags |= OUTSDLTRACE;
    if(dlg->m_OutTimeTrace) t->tflags |= OUTTIMETRACE;
    if(dlg->m_OutSoundTrace) t->tflags |= OUTSOUNDTRACE;
    if(dlg->m_OutInputs) t->tflags |= OUTINPUTS;
    if(dlg->m_OutLocale) t->tflags |= OUTLOCALE;
    if(dlg->m_OutFPS) t->tflags |= OUTFPS;
    if(dlg->m_OutSysLibs) t->tflags |= OUTSYSLIBS;
    if(dlg->m_OutWGTrace) t->tflags |= OUTWINGTRACE;
    if(dlg->m_OutDDRAWTrace) t->tflags |= OUTDDRAWTRACE;
    if(dlg->m_OutD3DTrace) t->tflags |= OUTD3DTRACE;
    if(dlg->m_AssertDialog) t->tflags |= ASSERTDIALOG;
    if(dlg->m_StartWithToggle) t->dflags |= STARTWITHTOGGLE;
    if(dlg->m_ImportTable) t->tflags |= OUTIMPORTTABLE;
    if(dlg->m_RegistryOp) t->tflags |= OUTREGISTRY;
    if(dlg->m_TraceHooks) t->tflags |= TRACEHOOKS;
    if(dlg->m_HandleExceptions) t->flags |= HANDLEEXCEPTIONS;
    if(dlg->m_VirtualHeap) t->flags8 |= VIRTUALHEAP;
    if(dlg->m_VirtualProcHeap) t->flags11 |= VIRTUALPROCHEAP;
    if(dlg->m_NoBAADFOOD) t->flags9 |= NOBAADFOOD;
    if(dlg->m_LimitResources) t->flags2 |= LIMITRESOURCES;
    if(dlg->m_CDROMDriveType) t->flags3 |= CDROMDRIVETYPE;
    if(dlg->m_HideCDROMEmpty) t->flags4 |= HIDECDROMEMPTY;
    if(dlg->m_FakeHD) t->flags10 |= FAKEHDDRIVE;
    if(dlg->m_FakeCD) t->flags10 |= FAKECDDRIVE;
    if(dlg->m_HookGOGLibs) t->flags6 |= HOOKGOGLIBS;
    if(dlg->m_BypassGOGLibs) t->flags6 |= BYPASSGOGLIBS;
    if(dlg->m_FontBypass) t->flags3 |= FONTBYPASS;
    if(dlg->m_BufferedIOFix) t->flags3 |= BUFFEREDIOFIX;
    if(dlg->m_ZBufferClean) t->flags4 |= ZBUFFERCLEAN;
    if(dlg->m_ZBuffer0Clean) t->flags4 |= ZBUFFER0CLEAN;
    if(dlg->m_DynamicZClean) t->flags8 |= DYNAMICZCLEAN;
    if(dlg->m_ZBufferHardClean) t->flags8 |= ZBUFFERHARDCLEAN;
    if(dlg->m_ZBufferAlways) t->dflags |= ZBUFFERALWAYS;
    if(dlg->m_HotPatchAlways) t->dflags |= HOTPATCHALWAYS;
    if(dlg->m_FreezeInjectedSon) t->dflags |= FREEZEINJECTEDSON;
    if(dlg->m_StressResources) t->dflags |= STRESSRESOURCES;
    if(dlg->m_Experimental) t->dflags2 |= EXPERIMENTAL;
    if(dlg->m_FixRandomPalette) t->dflags2 |= FIXRANDOMPALETTE;
    if(dlg->m_NoPower2Fix) t->flags4 |= NOPOWER2FIX;
    if(dlg->m_NoPerfCounter) t->flags4 |= NOPERFCOUNTER;
    if(dlg->m_DisableFogging) t->flags4 |= DISABLEFOGGING;
    if(dlg->m_ClearTarget) t->flags5 |= CLEARTARGET;
    if(dlg->m_FixPitch) t->flags6 |= FIXPITCH;
    if(dlg->m_Power2Width) t->flags6 |= POWER2WIDTH;
    if(dlg->m_SuppressIME) t->flags2 |= SUPPRESSIME;
    if(dlg->m_SuppressD3DExt) t->flags3 |= SUPPRESSD3DEXT;
    if(dlg->m_Enum16bitModes) t->flags7 |= ENUM16BITMODES;
    if(dlg->m_TrimTextureFormats) t->flags8 |= TRIMTEXTUREFORMATS;
    if(dlg->m_TransformAndLight) t->flags11 |= TRANSFORMANDLIGHT;
    if(dlg->m_ForceD3DGammaRamp) t->flags10 |= FORCED3DGAMMARAMP;
    if(dlg->m_LightGammaRamp) t->flags10 |= LIGHTGAMMARAMP;
    if(dlg->m_D3D8Back16) t->flags8 |= D3D8BACK16;
    if(dlg->m_SetCompatibility) t->flags2 |= SETCOMPATIBILITY;
    if(dlg->m_AEROBoost) t->flags5 |= AEROBOOST;
    if(dlg->m_DiabloTweak) t->flags5 |= DIABLOTWEAK;
    if(dlg->m_HookDirectSound) t->flags7 |= HOOKDIRECTSOUND;
    if(dlg->m_VirtualCDAudio) t->flags8 |= VIRTUALCDAUDIO;
    if(dlg->m_ForceTrackRepeat) t->flags8 |= FORCETRACKREPEAT;
    if(dlg->m_CDROMPresent) t->flags10 |= CDROMPRESENT;
    if(dlg->m_HackMCIFrames) t->flags11 |= HACKMCIFRAMES;
    if(dlg->m_CDPauseCapability) t->flags11 |= CDPAUSECAPABILITY;
    if(dlg->m_SuppressCDAudio) t->flags12 |= SUPPRESSCDAUDIO;
    if(dlg->m_ReserveMCIDevice) t->flags12 |= RESERVEMCIDEVICE;
    if(dlg->m_FixDefaultMCIId) t->flags11 |= FIXDEFAULTMCIID;
    if(dlg->m_LockCDTray) t->flags12 |= LOCKCDTRAY;
    if(dlg->m_HookEARSound) t->flags11 |= HOOKEARSOUND;
    if(dlg->m_IgnoreMCIDevId) t->flags8 |= IGNOREMCIDEVID;
    if(dlg->m_SoundMute) t->flags9 |= SOUNDMUTE;
    if(dlg->m_DSInitVolume) t->flags12 |= DSINITVOLUME;
    if(dlg->m_LockVolume) t->flags9 |= LOCKVOLUME;
    if(dlg->m_SafeMidiOut) t->flags11 |= SAFEMIDIOUT;
    if(dlg->m_SetCDVolume) t->flags10 |= SETCDVOLUME;
    if(dlg->m_HookWinG32) t->flags8 |= HOOKWING32;
    if(dlg->m_HookXinput) t->flags |= HOOKXINPUT;
    if(dlg->m_HookSDLLib) t->flags9 |= HOOKSDLLIB;
    if(dlg->m_HookSDL2Lib) t->flags9 |= HOOKSDL2LIB;
    if(dlg->m_ExtendSDLHook) t->flags11 |= EXTENDSDLHOOK;
    if(dlg->m_SDLEmulation) t->flags9 |= SDLEMULATION;
    if(dlg->m_SDLForceStretch) t->flags9 |= SDLFORCESTRETCH;
    if(dlg->m_HookSmackW32) t->flags7 |= HOOKSMACKW32;
    if(dlg->m_HookBinkW32) t->flags5 |= HOOKBINKW32;
    if(dlg->m_FixSmackLoop) t->flags7 |= FIXSMACKLOOP;
    if(dlg->m_BlockPriorityClass) t->flags7 |= BLOCKPRIORITYCLASS;
    if(dlg->m_EASportsHack) t->flags5 |= EASPORTSHACK;
    if(dlg->m_LegacyAlloc) t->flags6 |= LEGACYALLOC;
    if(dlg->m_DisableMaxWinMode) t->flags6 |= DISABLEMAXWINMODE;
    if(dlg->m_NoImagehlp) t->flags5 |= NOIMAGEHLP;
    if(dlg->m_ForcesHEL) t->flags3 |= FORCESHEL;
    if(dlg->m_ForcesHAL) t->flags11 |= FORCESHAL;
    if(dlg->m_ForcesNULL) t->flags11 |= FORCESNULL;
    if(dlg->m_NoHALDevice) t->flags8 |= NOHALDEVICE;
    if(dlg->m_NoTnLDevice) t->flags12 |= NOTNLDEVICE;
    if(dlg->m_MinimalCaps) t->flags3 |= MINIMALCAPS;
    if(dlg->m_SetZBufferBitDepths) t->flags6 |= SETZBUFFERBITDEPTHS;
    if(dlg->m_ForcesSwapEffect) t->flags6 |= FORCESWAPEFFECT;
    if(dlg->m_ColorFix) t->flags3 |= COLORFIX;
    if(dlg->m_FixGlobalUnlock) t->flags7 |= FIXGLOBALUNLOCK;
    if(dlg->m_FixFreeLibrary) t->flags7 |= FIXFREELIBRARY;
    if(dlg->m_SkipFreeLibrary) t->flags3 |= SKIPFREELIBRARY;
    if(dlg->m_LoadLibraryErr) t->flags8 |= LOADLIBRARYERR;
    if(dlg->m_FixAlteredPath) t->flags3 |= FIXALTEREDPATH;
    if(dlg->m_FixAdjustWinRect) t->flags8 |= FIXADJUSTWINRECT;
    if(dlg->m_NoPixelFormat) t->flags3 |= NOPIXELFORMAT;
    if(dlg->m_NoAlphaChannel) t->flags4 |= NOALPHACHANNEL;
    if(dlg->m_FixRefCounter) t->flags4 |= FIXREFCOUNTER;
    if(dlg->m_ReturnNullRef) t->flags4 |= RETURNNULLREF;
    if(dlg->m_NoD3DReset) t->flags4 |= NOD3DRESET;
    if(dlg->m_HideDesktop) t->flags4 |= HIDEDESKTOP;
    if(dlg->m_HideTaskbar) t->flags6 |= HIDETASKBAR;
    if(dlg->m_NoTaskbarOverlap) t->flags9 |= NOTASKBAROVERLAP;
    if(dlg->m_SetDPIAware) t->flags |= SETDPIAWARE;
    if(dlg->m_ActivateApp) t->flags6 |= ACTIVATEAPP;
    if(dlg->m_D3DResolutionHack) t->flags9 |= D3DRESOLUTIONHACK;
    if(dlg->m_FixAILSoundLocks) t->flags9 |= FIXAILSOUNDLOCKS;
    if(dlg->m_NoDestroyWindow) t->flags6 |= NODESTROYWINDOW;
    if(dlg->m_LockSysColors) t->flags3 |= LOCKSYSCOLORS;
    if(dlg->m_LockReservedPalette) t->flags5 |= LOCKRESERVEDPALETTE;
    if(dlg->m_LimitScreenRes) t->flags4 |= LIMITSCREENRES;
    if(dlg->m_SingleProcAffinity) t->flags3 |= SINGLEPROCAFFINITY;
    if(dlg->m_UseLastCore) t->flags5 |= USELASTCORE;
    if(dlg->m_SlowDown) t->flags |= SLOWDOWN;
    if(dlg->m_BlitFromBackBuffer) t->flags |= BLITFROMBACKBUFFER;
    if(dlg->m_NoFlipEmulation) t->flags4 |= NOFLIPEMULATION;
    if(dlg->m_OffscreenZBuffer) t->flags8 |= OFFSCREENZBUFFER;
    if(dlg->m_NoZBufferAttach) t->flags10 |= NOZBUFATTACH;
    if(dlg->m_TexturePalette) t->flags12 |= TEXTUREPALETTE;
    if(dlg->m_LockColorDepth) t->flags7 |= LOCKCOLORDEPTH;
    if(dlg->m_Lock24BitDepth) t->flags |= LOCK24BITDEPTH;
    if(dlg->m_FullPaintRect) t->flags3 |= FULLPAINTRECT;
    if(dlg->m_PushActiveMovie) t->flags5 |= PUSHACTIVEMOVIE;
    if(dlg->m_ForceClipChildren) t->flags4 |= FORCECLIPCHILDREN;
    if(dlg->m_PreventMinimize) t->flags4 |= PREVENTMINIMIZE;
    if(dlg->m_NoAccessibility) t->flags5 |= NOACCESSIBILITY;
    if(dlg->m_IgnoreDebOutput) t->flags4 |= IGNOREDEBOUTPUT;
    if(dlg->m_NoOleInitialize) t->flags10 |= NOOLEINITIALIZE;
    if(dlg->m_ChaosOverlordsFix) t->flags10 |= CHAOSOVERLORDSFIX;
    if(dlg->m_FixFolderPaths) t->flags10 |= FIXFOLDERPATHS;
    if(dlg->m_NoComplexMipmaps) t->flags10 |= NOCOMPLEXMIPMAPS;
    if(dlg->m_InvalidateClient) t->flags10 |= INVALIDATECLIENT;
    if(dlg->m_CreateDCHook) t->flags10 |= CREATEDCHOOK;
    if(dlg->m_SafePrimLock) t->flags10 |= SAFEPRIMLOCK;
    if(dlg->m_SmackBufferNoDepth) t->flags11 |= SMACKBUFFERNODEPTH;
    if(dlg->m_CustomLocale) t->flags11 |= CUSTOMLOCALE;
    if(dlg->m_ClassLocale) t->flags12 |= CLASSLOCALE;
    if(dlg->m_LockSysSettings) t->flags11 |= LOCKSYSSETTINGS;
    if(dlg->m_ProjectBuffer) t->flags12 |= PROJECTBUFFER;
    if(dlg->m_ForceRelAxis) t->flags12 |= FORCERELAXIS;
    if(dlg->m_ForceAbsAxis) t->flags12 |= FORCEABSAXIS;
    if(dlg->m_DirectXReplace) t->flags12 |= DIRECTXREPLACE;
    if(dlg->m_W98OpaqueFont) t->flags12 |= W98OPAQUEFONT;
    if(dlg->m_FakeGlobalAtom) t->flags12 |= FAKEGLOBALATOM;
    if(dlg->m_RevertDIBPalette) t->flags12 |= REVERTDIBPALETTE;
    if(dlg->m_FixDCAlreadyCreated) t->flags12 |= FIXDCALREADYCREATED;
    if(dlg->m_SuppressMenus) t->flags12 |= SUPPRESSMENUS;
    if(dlg->m_KillDeadlocks) t->flags12 |= KILLDEADLOCKS;
    if(dlg->m_CommitPage) t->flags12 |= COMMITPAGE;
    if(dlg->m_D3D8MaxWinMode) t->flags11 |= D3D8MAXWINMODE;
    if(dlg->m_Mutex4CritSection) t->flags11 |= MUTEX4CRITSECTION;
    if(dlg->m_DelayCritSection) t->flags11 |= DELAYCRITSECTION;
    if(dlg->m_RemapNumKeypad) t->flags11 |= REMAPNUMKEYPAD;
    if(dlg->m_SetUSKeyDescr) t->flags11 |= SETUSKEYDESCR;
    if(dlg->m_DisableGammaRamp) t->flags2 |= DISABLEGAMMARAMP;
    if(dlg->m_PALDIBEmulation) t->flags2 |= PALDIBEMULATION;
    if(dlg->m_RefreshOnRealize) t->flags3 |= REFRESHONREALIZE;
    if(dlg->m_LoadGammaRamp) t->flags8 |= LOADGAMMARAMP;
    if(dlg->m_AutoRefresh) t->flags |= AUTOREFRESH;
    if(dlg->m_IndependentRefresh) t->flags2 |= INDEPENDENTREFRESH;
    if(dlg->m_TextureFormat) t->flags5 |= TEXTUREFORMAT;
    if(dlg->m_VideoToSystemMem) t->flags |= SWITCHVIDEOMEMORY;
    if(dlg->m_FixTextOut) t->flags |= FIXTEXTOUT;
    if(dlg->m_ShrinkFontWidth) t->flags11 |= SHRINKFONTWIDTH;
    if(dlg->m_HookGlide) t->flags4 |= HOOKGLIDE;
    if(dlg->m_SuppressGlide) t->flags12 |= SUPPRESSGLIDE;
    if(dlg->m_RemapMCI) t->flags5 |= REMAPMCI;
    if(dlg->m_NoMovies) t->flags6 |= NOMOVIES;
    if(dlg->m_FixMoviesColor) t->flags6 |= FIXMOVIESCOLOR;
    if(dlg->m_StretchMovies) t->flags6 |= STRETCHMOVIES;
    if(dlg->m_BypassMCI) t->flags6 |= BYPASSMCI;
    if(dlg->m_FixPCMAudio) t->flags8 |= FIXAUDIOPCM;
    if(dlg->m_SuspendTimeStretch) t->flags10 |= SUSPENDTIMESTRETCH;
    if(dlg->m_SuppressRelease) t->flags6 |= SUPPRESSRELEASE;
    if(dlg->m_KeepCursorWithin) t->flags |= KEEPCURSORWITHIN;
    if(dlg->m_KeepCursorFixed) t->flags2 |= KEEPCURSORFIXED;
    if(dlg->m_UseRGB565) t->flags |= USERGB565;
    if(dlg->m_SuppressDXErrors) t->flags |= SUPPRESSDXERRORS;
    if(dlg->m_FlipEmulation) t->flags6 |= FLIPEMULATION;
    if(dlg->m_MarkBlit) t->dflags |= MARKBLIT;
    if(dlg->m_MarkLock) t->dflags |= MARKLOCK;
    if(dlg->m_MarkWinG32) t->dflags |= MARKWING32;
    if(dlg->m_MarkGDI32) t->dflags |= MARKGDI32;
    if(dlg->m_DumpDIBSection) t->dflags |= DUMPDIBSECTION;
    if(dlg->m_CaptureScreens) t->dflags |= CAPTURESCREENS;
    if(dlg->m_PatchExecuteBuffer) t->dflags |= PATCHEXECUTEBUFFER;
    if(dlg->m_ForceZBufferOn) t->dflags |= FORCEZBUFFERON;
    if(dlg->m_ForceZBufferOff) t->dflags |= FORCEZBUFFEROFF;
    if(dlg->m_DumpDevContext) t->dflags |= DUMPDEVCONTEXT;
    if(dlg->m_DumpCPUID) t->dflags |= DUMPCPUID;
    if(dlg->m_NoSysMemPrimary) t->flags6 |= NOSYSMEMPRIMARY;
    if(dlg->m_NoSysMemBackBuf) t->flags6 |= NOSYSMEMBACKBUF;
    if(dlg->m_DumpSurfaces) t->dflags |= DUMPSURFACES;
    if(dlg->m_DumpBlitSrc) t->dflags2 |= DUMPBLITSRC;
    if(dlg->m_DumpBitmaps) t->dflags2 |= DUMPBITMAPS;
    if(dlg->m_FastBlt) t->dflags |= DOFASTBLT;
    //if(dlg->m_GDIColorConv) t->flags5 |= GDICOLORCONV;
    if(dlg->m_PreventMaximize) t->flags |= PREVENTMAXIMIZE;
    if(dlg->m_EmulateMaximize) t->flags9 |= EMULATEMAXIMIZE;
    if(dlg->m_ClientRemapping) t->flags |= CLIENTREMAPPING;
    if(dlg->m_LockWinStyle) t->flags |= LOCKWINSTYLE;
    if(dlg->m_DisableDWM) t->flags12 |= DISABLEDWM;
    if(dlg->m_FixParentWin) t->flags |= FIXPARENTWIN;
    if(dlg->m_KeepAspectRatio) t->flags2 |= KEEPASPECTRATIO;
    if(dlg->m_AdaptiveRatio) t->flags11 |= ADAPTIVERATIO;
    if(dlg->m_ForceWinResize) t->flags2 |= FORCEWINRESIZE;
    if(dlg->m_HideMultiMonitor) t->flags2 |= HIDEMULTIMONITOR;
    if(dlg->m_FixD3DFrame) t->flags3 |= FIXD3DFRAME;
    if(dlg->m_NoWindowMove) t->flags3 |= NOWINDOWMOVE;
    if(dlg->m_HookChildWin) t->flags |= HOOKCHILDWIN;
    if(dlg->m_HookDlgWin) t->flags8 |= HOOKDLGWIN;
    if(dlg->m_MessageProc) t->flags |= MESSAGEPROC;
    if(dlg->m_FixMouseHook) t->flags8 |= FIXMOUSEHOOK;
    if(dlg->m_FixMessageHook) t->flags11 |= FIXMESSAGEHOOK;
    if(dlg->m_FixNCHITTEST) t->flags2 |= FIXNCHITTEST;
    if(dlg->m_RecoverScreenMode) t->flags2 |= RECOVERSCREENMODE;
    if(dlg->m_RefreshOnResize) t->flags2 |= REFRESHONRESIZE;
    if(dlg->m_BackBufAttach) t->flags2 |= BACKBUFATTACH;
    if(dlg->m_ClearTextureFourCC) t->flags7 |= CLEARTEXTUREFOURCC;
    if(dlg->m_SuppressFourCCBlt) t->flags10 |= SUPPRESSFOURCCBLT;
    if(dlg->m_NoDDExclusiveMode) t->flags7 |= NODDEXCLUSIVEMODE;
    if(dlg->m_LockFullscreenCoop) t->flags9 |= LOCKFULLSCREENCOOP;
    if(dlg->m_CreateDesktop) t->flags6 |= CREATEDESKTOP;
    if(dlg->m_SafePaletteUsage) t->flags9 |= SAFEPALETTEUSAGE;
    if(dlg->m_AllowSysmemOn3DDev) t->flags8 |= ALLOWSYSMEMON3DDEV;
    if(dlg->m_HandleAltF4) t->flags |= HANDLEALTF4;
    if(dlg->m_LimitFPS) t->flags2 |= LIMITFPS;
    if(dlg->m_LimitDIBOperations) t->flags10 |= LIMITDIBOPERATIONS;
    if(dlg->m_LimitBeginScene) t->flags5 |= LIMITBEGINSCENE;
    if(dlg->m_LimitFlipOnly) t->flags12 |= LIMITFLIPONLY;
    if(dlg->m_SkipFPS) t->flags2 |= SKIPFPS;
    if(dlg->m_ShowTimeStretch) t->flags4 |= SHOWTIMESTRETCH;
    if(dlg->m_TimeStretch) t->flags2 |= TIMESTRETCH;
    if(dlg->m_StretchTimers) t->flags4 |= STRETCHTIMERS;
    if(dlg->m_NormalizePerfCount) t->flags5 |= NORMALIZEPERFCOUNT;
    if(dlg->m_StretchPerFrequency) t->flags12 |= STRETCHPERFREQUENCY;
    if(dlg->m_SlowWinPolling) t->flags10 |= SLOWWINPOLLING;
    if(dlg->m_CPUSlowDown) t->flags7 |= CPUSLOWDOWN;
    if(dlg->m_CPUMaxUsage) t->flags7 |= CPUMAXUSAGE;
    if(dlg->m_PreciseTiming) t->flags10 |= PRECISETIMING;
    if(dlg->m_KillVSync) t->flags12 |= KILLVSYNC;
    if(dlg->m_QuarterBlt) t->flags5 |= QUARTERBLT;
    if(dlg->m_MakeWinVisible) t->flags9 |= MAKEWINVISIBLE;
    if(dlg->m_FixEmpireOFS) t->flags9 |= FIXEMPIREOFS;
    if(dlg->m_KillBlackWin) t->flags9 |= KILLBLACKWIN;
    if(dlg->m_ZeroDisplayCounter) t->flags9 |= ZERODISPLAYCOUNTER;
    if(dlg->m_InvertMouseXAxis) t->flags11 |= INVERTMOUSEXAXIS;
    if(dlg->m_InvertMouseYAxis) t->flags11 |= INVERTMOUSEYAXIS;
    if(dlg->m_ReplaceDialogs) t->flags10 |= REPLACEDIALOGS;
    if(dlg->m_HandleFourCC) t->flags10 |= HANDLEFOURCC;
    if(dlg->m_CacheD3DSession) t->flags9 |= CACHED3DSESSION;
    if(dlg->m_FineTiming) t->flags4 |= FINETIMING;
    if(dlg->m_ReleaseMouse) t->flags4 |= RELEASEMOUSE;
    if(dlg->m_EnableTimeFreeze) t->flags4 |= ENABLETIMEFREEZE;
    if(dlg->m_EnableHotKeys) t->flags4 |= ENABLEHOTKEYS;
    if(dlg->m_NoDisablePrint) t->flags11 |= NODISABLEPRINT;
    if(dlg->m_FixAsyncKeyState) t->flags11 |= FIXASYNCKEYSTATE;
    if(dlg->m_InterceptRDTSC) t->flags4 |= INTERCEPTRDTSC;
    if(dlg->m_HookOpenGL) t->flags2 |= HOOKOPENGL;
    if(dlg->m_ForceHookOpenGL) t->flags3 |= FORCEHOOKOPENGL;
    if(dlg->m_FixPixelZoom) t->flags6 |= FIXPIXELZOOM;
    if(dlg->m_FixBindTexture) t->flags7 |= FIXBINDTEXTURE;
    if(dlg->m_HookGlut32) t->flags7 |= HOOKGLUT32;
    if(dlg->m_HookWGLContext) t->flags9 |= HOOKWGLCONTEXT;
    if(dlg->m_ScaleMainViewport) t->flags11 |= SCALEMAINVIEWPORT;
    if(dlg->m_LockGLViewport) t->flags12 |= LOCKGLVIEWPORT;
    if(dlg->m_GLExtensionsLie) t->flags12 |= GLEXTENSIONSLIE;
    if(dlg->m_GLFixClamp) t->flags13 |= GLFIXCLAMP;
    if(dlg->m_ScaleglBitmaps) t->flags9 |= SCALEGLBITMAPS;
    if(dlg->m_WireFrame) t->flags2 |= WIREFRAME;
    if(dlg->m_NoTextures) t->flags4 |= NOTEXTURES;
    if(dlg->m_BlackWhite) t->flags3 |= BLACKWHITE;
    if(dlg->m_FakeVersion) t->flags2 |= FAKEVERSION;
    if(dlg->m_FullRectBlt) t->dflags |= FULLRECTBLT;
    if(dlg->m_CenterToWin) t->dflags |= CENTERTOWIN;
    if(dlg->m_LimitDdraw) t->flags7 |= LIMITDDRAW;
    if(dlg->m_SuppressOverlay) t->flags7 |= SUPPRESSOVERLAY;
    if(dlg->m_NoPaletteUpdate) t->flags2 |= NOPALETTEUPDATE;
    if(dlg->m_CapMask) t->dflags |= CAPMASK;
    if(dlg->m_NoWindowHooks) t->dflags2 |= NOWINDOWHOOKS;
    if(dlg->m_DisableWinHooks) t->dflags2 |= DISABLEWINHOOKS;
    if(dlg->m_HideWindowChanges) t->flags10 |= HIDEWINDOWCHANGES;
    if(dlg->m_SkipIATHint) t->flags3 |= SKIPIATHINT;
    if(dlg->m_NoDDRAWBlt) t->dflags |= NODDRAWBLT;
    if(dlg->m_NoDDRAWFlip) t->dflags |= NODDRAWFLIP;
    if(dlg->m_NoGDIBlt) t->dflags |= NOGDIBLT;
    if(dlg->m_NoFillRect) t->flags4 |= NOFILLRECT;
    if(dlg->m_FixClipperArea) t->flags7 |= FIXCLIPPERAREA;
    if(dlg->m_SharedDCHybrid) t->flags8 |= SHAREDDCHYBRID;
    if(dlg->m_SyncPalette) t->flags6 |= SYNCPALETTE;
    if(dlg->m_NoWinErrors) t->flags7 |= NOWINERRORS;
    if(dlg->m_NoDialogs) t->flags9 |= NODIALOGS;
    if(dlg->m_InvalidateFullRect) t->flags11 |= INVALIDATEFULLRECT;
    if(dlg->m_NoSetPixelFormat) t->flags12 |= NOSETPIXELFORMAT;
    if(dlg->m_ScaleCBTHook) t->flags12 |= SCALECBTHOOK;
    if(dlg->m_StretchDialogs) t->flags10 |= STRETCHDIALOGS;
    if(dlg->m_PretendVisible) t->flags8 |= PRETENDVISIBLE;
    if(dlg->m_WinInsulation) t->flags8 |= WININSULATION;
    if(dlg->m_DisableMMX) t->flags5 |= DISABLEMMX;
    if(dlg->m_SafeAllocs) t->flags11 |= SAFEALLOCS;
    if(dlg->m_QualityFonts) t->flags8 |= QUALITYFONTS;
    if(dlg->m_ReplacePrivOps) t->flags5 |= REPLACEPRIVOPS;
    if(dlg->m_InitialRes) t->flags7 |= INITIALRES;
    if(dlg->m_MaximumRes) t->flags7 |= MAXIMUMRES;
    if(dlg->m_CustomRes) t->flags10 |= CUSTOMRES;
    if(dlg->m_RemapSysFolders) t->flags11 |= REMAPSYSFOLDERS;
    t->posx = dlg->m_PosX;
    t->posy = dlg->m_PosY;
    t->sizx = dlg->m_SizX;
    t->sizy = dlg->m_SizY;
    t->ScanLine = dlg->m_ScanLine;
    t->MaxFPS = dlg->m_MaxFPS;
    t->InitTS = dlg->m_InitTS - 8;
    t->FakeVersionId = dlg->m_FakeVersionId;
    t->MaxScreenRes = dlg->m_MaxScreenRes;
    t->SwapEffect = dlg->m_SwapEffect;
    t->resw = dlg->m_InitResW;
    t->resh = dlg->m_InitResH;
    t->FixedVolume = dlg->m_FixedVolume;
    strcpy_s(t->module, sizeof(t->module), dlg->m_Module);
    strcpy_s(t->OpenGLLib, sizeof(t->OpenGLLib), dlg->m_OpenGLLib);
    strcpy_s(t->path, sizeof(t->path), dlg->m_FilePath);
    if(IsWinXP()) {
        if(dlg->m_UseShortPath) {
            // v2.05.03: on WinXP and programs with "EmulateGetCommandLine" shim (included in
            // Win95 compatibility layer) you may have to use short paths
            GetShortPathNameA(t->path, t->path, MAX_PATH);
        } else
            GetLongPathNameA(t->path, t->path, MAX_PATH);
    }
    t->FakeHDDrive = dlg->m_FakeHDDrive[0];
    t->FakeCDDrive = dlg->m_FakeCDDrive[0];
    t->CodePage = dlg->m_CodePage;
    t->Language = dlg->m_Language;
    t->Country = dlg->m_Country;
    // debug
    if(dlg->m_DisableCPUID) t->dflags |= CPUDISABLECPUID;
    if(dlg->m_CPUDisableMMX) t->dflags |= CPUDISABLEMMX;
    if(dlg->m_CPUDisableSSE) t->dflags |= CPUDISABLESSE;
    if(dlg->m_CPUDisableSSE2) t->dflags |= CPUDISABLESSE2;
    if(dlg->m_CPUDisablePBE) t->dflags |= CPUDISABLEPBE;
    switch(dlg->m_CPUVendorId) {
    case 0:
        break;
    case 1:
        t->dflags |= CPUFAKEDXWNDCPU;
        break;
    case 2:
        t->dflags |= CPUGENUINEINTEL;
        break;
    case 3:
        t->dflags |= CPUAUTHENTICAMD;
        break;
    case 4:
        t->dflags |= CPUCYRIXINSTEAD;
        break;
    }
}

static void SetDlgFromTarget(TARGETMAP *t, CTargetDlg *dlg) {
    dlg->m_DXVersion = t->dxversion;
    dlg->m_MonitorId = t->monitorid + 1;
    dlg->m_FilterId = t->FilterId;
    dlg->m_RendererId = t->RendererId;
    dlg->m_MaxDdrawInterface = t->MaxDdrawInterface - 1;
    dlg->m_SlowRatio = t->SlowRatio;
    if(dlg->m_DXVersion > 6) dlg->m_DXVersion -= 5;
    dlg->m_Coordinates = t->coordinates;
    dlg->m_FilePath = t->path;
    dlg->m_Module = t->module;
    dlg->m_OpenGLLib = t->OpenGLLib;
    dlg->m_UnNotify = t->flags & UNNOTIFY ? 1 : 0;
    dlg->m_Windowize = t->flags2 & WINDOWIZE ? 1 : 0;
    dlg->m_HotPatch = t->flags4 & HOTPATCH ? 1 : 0;
    dlg->m_HookDLLs = t->flags3 & HOOKDLLS ? 1 : 0;
    //dlg->m_AnsiWide = t->flags5 & ANSIWIDE ? 1 : 0;
    dlg->m_HookNoRun = t->flags7 & HOOKNORUN ? 1 : 0;
    dlg->m_CopyNoShims = t->flags7 & COPYNOSHIMS ? 1 : 0;
    dlg->m_HotRegistry = t->flags9 & HOTREGISTRY ? 1 : 0;
    dlg->m_HookNoUpdate = t->flags7 & HOOKDOUPDATE ? 1 : 0;
    dlg->m_TerminateOnClose = t->flags6 & TERMINATEONCLOSE ? 1 : 0;
    dlg->m_ConfirmOnClose = t->flags6 & CONFIRMONCLOSE ? 1 : 0;
    dlg->m_EmulateRegistry = t->flags3 & EMULATEREGISTRY ? 1 : 0;
    dlg->m_OverrideRegistry = t->flags4 & OVERRIDEREGISTRY ? 1 : 0;
    dlg->m_Wow64Registry = t->flags6 & WOW64REGISTRY ? 1 : 0;
    dlg->m_Wow32Registry = t->flags6 & WOW32REGISTRY ? 1 : 0;
    dlg->m_HookEnabled = t->flags3 & HOOKENABLED ? 1 : 0;
    //dlg->m_SetCmdLine = t->flags12 & SETCMDLINE ? 1 : 0;
    dlg->m_NeedAdminCaps = t->flags & NEEDADMINCAPS ? 1 : 0;
    dlg->m_UseShortPath = t->flags11 & USESHORTPATH ? 1 : 0;
    dlg->m_NoBanner = t->flags2 & NOBANNER ? 1 : 0;
    dlg->m_FullScreenOnly = t->flags3 & FULLSCREENONLY ? 1 : 0;
    dlg->m_ShowHints = t->flags7 & SHOWHINTS ? 1 : 0;
    dlg->m_Frontend = t->flags6 & FRONTEND ? 1 : 0;
    dlg->m_BackgroundPriority = t->flags8 & BACKGROUNDPRIORITY ? 1 : 0;
    dlg->m_PeekAllMessages = t->flags3 & PEEKALLMESSAGES ? 1 : 0;
    dlg->m_NoWinPosChanges = t->flags5 & NOWINPOSCHANGES ? 1 : 0;
    dlg->m_MessagePump = t->flags5 & MESSAGEPUMP ? 1 : 0;
    dlg->m_ClipMenu = t->flags8 & CLIPMENU ? 1 : 0;
    dlg->m_NoMouseEvents = t->flags9 & NOMOUSEEVENTS ? 1 : 0;
    dlg->m_FixMouseRawInput = t->flags10 & FIXMOUSERAWINPUT ? 1 : 0;
    dlg->m_MouseShield = t->flags10 & MOUSESHIELD ? 1 : 0;
    dlg->m_AdaptMouseSpeed = t->flags12 & ADAPTMOUSESPEED ? 1 : 0;
    dlg->m_FixMouseLParam = t->flags12 & FIXMOUSELPARAM ? 1 : 0;
    dlg->m_SwallowMouseMove = t->flags5 & SWALLOWMOUSEMOVE ? 1 : 0;
    dlg->m_UnnotifyInactive = t->flags11 & UNNOTIFYINACTIVE ? 1 : 0;
    dlg->m_MouseMoveByEvent = t->flags9 & MOUSEMOVEBYEVENT ? 1 : 0;
    dlg->m_LockFPSCorner = t->flags3 & LOCKFPSCORNER ? 1 : 0;
    dlg->m_SetZBuffer16Bit = t->flags3 & SETZBUFFER16BIT ? 1 : 0;
    dlg->m_SetZBuffer24Bit = t->flags3 & SETZBUFFER24BIT ? 1 : 0;
    dlg->m_InjectionMode = 0;
    if(t->flags2  & STARTDEBUG) dlg->m_InjectionMode = 1;
    if(t->flags7  & INJECTSUSPENDED) dlg->m_InjectionMode = 2;
    if(t->flags12 & INJECTPROXY) dlg->m_InjectionMode = 3;
    dlg->m_DCEmulationMode = 0;
    if(t->flags2 & GDISTRETCHED) dlg->m_DCEmulationMode = 1;
    if(t->flags3 & GDIEMULATEDC) dlg->m_DCEmulationMode = 2;
    if(t->flags6 & SHAREDDC    ) dlg->m_DCEmulationMode = 3;
    dlg->m_ResTypes = 0;
    if(t->flags4 & SUPPORTSVGA) dlg->m_ResTypes = 0;
    if(t->flags4 & SUPPORTHDTV) dlg->m_ResTypes = 1;
    if(t->flags4 & NATIVERES) dlg->m_ResTypes = 2;
    if((t->flags4 & SUPPORTHDTV) && (t->flags4 & SUPPORTSVGA)) dlg->m_ResTypes = 3;
    dlg->m_MouseVisibility = 0;
    if(t->flags & HIDEHWCURSOR) dlg->m_MouseVisibility = 1;
    if(t->flags2 & SHOWHWCURSOR) dlg->m_MouseVisibility = 2;
    dlg->m_MouseClipper = 0;
    if(t->flags & DISABLECLIPPING) dlg->m_MouseClipper = 1;
    if(t->flags & CLIPCURSOR) dlg->m_MouseClipper = 2;
    if(t->flags8 & CLIPLOCKED) dlg->m_MouseClipper = 3;
    dlg->m_OffendingMessages = 0;
    if(t->flags3 & FILTERMESSAGES) dlg->m_OffendingMessages = 1;
    if(t->flags3 & DEFAULTMESSAGES) dlg->m_OffendingMessages = 2;
    dlg->m_TextureHandling = 0;
    if(t->flags5 & TEXTUREHIGHLIGHT) dlg->m_TextureHandling = 1;
    if(t->flags5 & TEXTUREDUMP) dlg->m_TextureHandling = 2;
    if(t->flags5 & TEXTUREHACK) dlg->m_TextureHandling = 3;
    if(t->flags5 & TEXTURETRANSP) dlg->m_TextureHandling = 4;
    dlg->m_SonProcessMode = 0;
    if(t->flags4 & SUPPRESSCHILD) dlg->m_SonProcessMode = 1;
    if(t->flags5 & DEBUGSON) dlg->m_SonProcessMode = 2;
    if(t->flags5 & INJECTSON) dlg->m_SonProcessMode = 3;
    dlg->m_IATAlignedMode = 0; // WORD alignement, most common case
    if(t->flags9 & IATBYTEALIGNED) dlg->m_IATAlignedMode = 1;
    if(t->flags9 & IATWORDALIGNED) dlg->m_IATAlignedMode = 0;
    if(t->flags9 & NOIATSCAN)	   dlg->m_IATAlignedMode = 2;
    if(t->flags10 & PEFILEHOOK)	   dlg->m_IATAlignedMode = 3;
    dlg->m_InitColorDepth = 0;
    if(t->flags2 & INIT8BPP)  dlg->m_InitColorDepth = 1;
    if(t->flags2 & INIT16BPP) dlg->m_InitColorDepth = 2;
    if(t->flags7 & INIT24BPP) dlg->m_InitColorDepth = 3;
    if(t->flags7 & INIT32BPP) dlg->m_InitColorDepth = 4;
    dlg->m_LogMode = 0;
    if(t->tflags & OUTTRACE)		dlg->m_LogMode = 2; // must be first one ....
    if(t->tflags & ERASELOGFILE)	dlg->m_LogMode = 1;
    if(t->tflags & OUTSEPARATED)	dlg->m_LogMode = 3;
    if(t->tflags & OUTCIRCULAR)		dlg->m_LogMode = 4;
    dlg->m_WinMovementType = 0;
    if(t->flags & LOCKWINPOS)		dlg->m_WinMovementType = 1;
    if(t->flags2 & LOCKEDSIZE)		dlg->m_WinMovementType = 2;
    if(t->flags7 & ANCHORED)		dlg->m_WinMovementType = 3;
    dlg->m_VSyncMode = 0;
    if(t->flags8 & FORCEVSYNC) dlg->m_VSyncMode = 1;
    if(t->flags8 & FORCENOVSYNC) dlg->m_VSyncMode = 2;
    dlg->m_VSyncImpl = 0;
    if(t->flags8 & VSYNCSCANLINE) dlg->m_VSyncImpl = 1;
    if(t->flags & SAVELOAD) dlg->m_VSyncImpl = 2;
    dlg->m_WaitMode = 0;
    if(t->flags8 & FORCEWAIT) dlg->m_WaitMode = 1;
    if(t->flags8 & FORCENOWAIT) dlg->m_WaitMode = 2;
    dlg->m_ClipperMode = 0;
    if (t->flags3 & FORCECLIPPER) dlg->m_ClipperMode = 1;
    if (t->flags & SUPPRESSCLIPPING) dlg->m_ClipperMode = 2;
    if (t->flags5 & MAXCLIPPER) dlg->m_ClipperMode = 3;
    dlg->m_FourCCMode = 0;
    if (t->flags10 & HWFOURCC) dlg->m_FourCCMode = 1;
    if (t->flags10 & SWFOURCC) dlg->m_FourCCMode = 2;
    dlg->m_TextureFileFormat = 0;
    if( t->flags8 & RAWFORMAT) dlg->m_TextureFileFormat = 1;
    if( t->flags8 & DDSFORMAT) dlg->m_TextureFileFormat = 2;
    dlg->m_WindowStyle = 0;
    if(t->flags2 & MODALSTYLE) dlg->m_WindowStyle = 1;
    if(t->flags & FIXWINFRAME) dlg->m_WindowStyle = 2;
    if(t->flags9 & FIXTHINFRAME) dlg->m_WindowStyle = 3;
    dlg->m_ShowFPS = 0;
    if(t->flags2 & SHOWFPSOVERLAY) dlg->m_ShowFPS = 1;
    if(t->flags2 & SHOWFPS) dlg->m_ShowFPS = 2;
    dlg->m_ZOrder = 0;
    if(t->flags5 & UNLOCKZORDER) dlg->m_ZOrder = 1;
    if(t->flags9 & LOCKTOPZORDER) dlg->m_ZOrder = 2;
    dlg->m_VertexProcessing = 0;
    if(t->flags10 & FORCEHWVERTEXPROC) dlg->m_VertexProcessing = 1;
    if(t->flags10 & FORCESWVERTEXPROC) dlg->m_VertexProcessing = 2;
    if(t->flags10 & FORCEMXVERTEXPROC) dlg->m_VertexProcessing = 3;
    dlg->m_GFocusMode = 0;
    if(t->flags5 & GLOBALFOCUSON) dlg->m_GFocusMode = 1;
    if(t->flags5 & GLOBALFOCUSOFF) dlg->m_GFocusMode = 2;
    dlg->m_AltTabMode = 0;
    if(t->flags7 & NODISABLEALTTAB) dlg->m_AltTabMode = 1;
    if(t->flags5 & DISABLEALTTAB) dlg->m_AltTabMode = 2;
    dlg->m_BilinearFilter = t->flags5 & BILINEARFILTER ? 1 : 0;
    dlg->m_BlurEffect = t->flags12 & BLUREFFECT ? 1 : 0;
    dlg->m_HookDI = t->flags & HOOKDI ? 1 : 0;
    dlg->m_HookDI8 = t->flags & HOOKDI8 ? 1 : 0;
    dlg->m_EmulateRelMouse = t->flags6 & EMULATERELMOUSE ? 1 : 0;
    dlg->m_ScaleRelMouse = t->flags6 & SCALERELMOUSE ? 1 : 0;
    dlg->m_SkipDevTypeHID = t->flags7 & SKIPDEVTYPEHID ? 1 : 0;
    dlg->m_SuppressDIErrors = t->flags7 & SUPPRESSDIERRORS ? 1 : 0;
    dlg->m_SharedKeyboard = t->flags7 & SHAREDKEYBOARD ? 1 : 0;
    dlg->m_ModifyMouse = t->flags & MODIFYMOUSE ? 1 : 0;
    dlg->m_VirtualJoystick = t->flags6 & VIRTUALJOYSTICK ? 1 : 0;
    dlg->m_HideJoysticks = t->flags9 & HIDEJOYSTICKS ? 1 : 0;
    dlg->m_Unacquire = t->flags6 & UNACQUIRE ? 1 : 0;
    dlg->m_OutDebug = t->tflags & OUTDEBUG ? 1 : 0;
    dlg->m_CursorTrace = t->tflags & OUTCURSORTRACE ? 1 : 0;
    dlg->m_OutDebugString = t->tflags & OUTDEBUGSTRING ? 1 : 0;
    dlg->m_AddTimeStamp = t->tflags & ADDTIMESTAMP ? 1 : 0;
    dlg->m_AddRelativeTime = t->tflags & ADDRELATIVETIME ? 1 : 0;
    dlg->m_AddThreadID = t->tflags & ADDTHREADID ? 1 : 0;
    dlg->m_OutWinMessages = t->tflags & OUTWINMESSAGES ? 1 : 0;
    dlg->m_OutDWTrace = t->tflags & OUTDXWINTRACE ? 1 : 0;
    dlg->m_OutOGLTrace = t->tflags & OUTOGLTRACE ? 1 : 0;
    dlg->m_OutHexTrace = t->tflags & OUTHEXTRACE ? 1 : 0;
    dlg->m_OutSDLTrace = t->tflags & OUTSDLTRACE ? 1 : 0;
    dlg->m_OutTimeTrace = t->tflags & OUTTIMETRACE ? 1 : 0;
    dlg->m_OutSoundTrace = t->tflags & OUTSOUNDTRACE ? 1 : 0;
    dlg->m_OutInputs = t->tflags & OUTINPUTS ? 1 : 0;
    dlg->m_OutLocale = t->tflags & OUTLOCALE ? 1 : 0;
    dlg->m_OutFPS = t->tflags & OUTFPS ? 1 : 0;
    dlg->m_OutSysLibs = t->tflags & OUTSYSLIBS ? 1 : 0;
    dlg->m_OutWGTrace = t->tflags & OUTWINGTRACE ? 1 : 0;
    dlg->m_OutD3DTrace = t->tflags & OUTD3DTRACE ? 1 : 0;
    dlg->m_OutDDRAWTrace = t->tflags & OUTDDRAWTRACE ? 1 : 0;
    dlg->m_AssertDialog = t->tflags & ASSERTDIALOG ? 1 : 0;
    dlg->m_StartWithToggle = t->dflags & STARTWITHTOGGLE ? 1 : 0;
    dlg->m_ImportTable = t->tflags & OUTIMPORTTABLE ? 1 : 0;
    dlg->m_RegistryOp = t->tflags & OUTREGISTRY ? 1 : 0;
    dlg->m_TraceHooks = t->tflags & TRACEHOOKS ? 1 : 0;
    dlg->m_HandleExceptions = t->flags & HANDLEEXCEPTIONS ? 1 : 0;
    dlg->m_VirtualHeap = t->flags8 & VIRTUALHEAP ? 1 : 0;
    dlg->m_VirtualProcHeap = t->flags11 & VIRTUALPROCHEAP ? 1 : 0;
    dlg->m_NoBAADFOOD = t->flags9 & NOBAADFOOD ? 1 : 0;
    dlg->m_SuppressIME = t->flags2 & SUPPRESSIME ? 1 : 0;
    dlg->m_SuppressD3DExt = t->flags3 & SUPPRESSD3DEXT ? 1 : 0;
    dlg->m_Enum16bitModes = t->flags7 & ENUM16BITMODES ? 1 : 0;
    dlg->m_TrimTextureFormats = t->flags8 & TRIMTEXTUREFORMATS ? 1 : 0;
    dlg->m_TransformAndLight = t->flags11 & TRANSFORMANDLIGHT ? 1 : 0;
    dlg->m_ForceD3DGammaRamp = t->flags10 & FORCED3DGAMMARAMP ? 1 : 0;
    dlg->m_LightGammaRamp = t->flags10 & LIGHTGAMMARAMP ? 1 : 0;
    dlg->m_D3D8Back16 = t->flags8 & D3D8BACK16 ? 1 : 0;
    dlg->m_SetCompatibility = t->flags2 & SETCOMPATIBILITY ? 1 : 0;
    dlg->m_AEROBoost = t->flags5 & AEROBOOST ? 1 : 0;
    dlg->m_DiabloTweak = t->flags5 & DIABLOTWEAK ? 1 : 0;
    dlg->m_HookDirectSound = t->flags7 & HOOKDIRECTSOUND ? 1 : 0;
    dlg->m_VirtualCDAudio = t->flags8 & VIRTUALCDAUDIO ? 1 : 0;
    dlg->m_ForceTrackRepeat = t->flags8 & FORCETRACKREPEAT ? 1 : 0;
    dlg->m_CDROMPresent = t->flags10 & CDROMPRESENT ? 1 : 0;
    dlg->m_HackMCIFrames = t->flags11 & HACKMCIFRAMES ? 1 : 0;
    dlg->m_CDPauseCapability = t->flags11 & CDPAUSECAPABILITY ? 1 : 0;
    dlg->m_SuppressCDAudio = t->flags12 & SUPPRESSCDAUDIO ? 1 : 0;
    dlg->m_ReserveMCIDevice = t->flags12 & RESERVEMCIDEVICE ? 1 : 0;
    dlg->m_FixDefaultMCIId = t->flags11 & FIXDEFAULTMCIID ? 1 : 0;
    dlg->m_LockCDTray = t->flags12 & LOCKCDTRAY ? 1 : 0;
    dlg->m_HookEARSound = t->flags11 & HOOKEARSOUND ? 1 : 0;
    dlg->m_IgnoreMCIDevId = t->flags8 & IGNOREMCIDEVID ? 1 : 0;
    dlg->m_SoundMute = t->flags9 & SOUNDMUTE ? 1 : 0;
    dlg->m_DSInitVolume = t->flags12 & DSINITVOLUME ? 1 : 0;
    dlg->m_LockVolume = t->flags9 & LOCKVOLUME ? 1 : 0;
    dlg->m_SafeMidiOut = t->flags11 & SAFEMIDIOUT ? 1 : 0;
    dlg->m_SetCDVolume = t->flags10 & SETCDVOLUME ? 1 : 0;
    dlg->m_HookWinG32 = t->flags8 & HOOKWING32 ? 1 : 0;
    dlg->m_HookXinput = t->flags & HOOKXINPUT ? 1 : 0;
    dlg->m_HookSDLLib = t->flags9 & HOOKSDLLIB ? 1 : 0;
    dlg->m_HookSDL2Lib = t->flags9 & HOOKSDL2LIB ? 1 : 0;
    dlg->m_ExtendSDLHook = t->flags11 & EXTENDSDLHOOK ? 1 : 0;
    dlg->m_SDLEmulation = t->flags9 & SDLEMULATION ? 1 : 0;
    dlg->m_SDLForceStretch = t->flags9 & SDLFORCESTRETCH ? 1 : 0;
    dlg->m_HookSmackW32 = t->flags7 & HOOKSMACKW32 ? 1 : 0;
    dlg->m_HookBinkW32 = t->flags5 & HOOKBINKW32 ? 1 : 0;
    dlg->m_FixSmackLoop = t->flags7 & FIXSMACKLOOP ? 1 : 0;
    dlg->m_BlockPriorityClass = t->flags7 & BLOCKPRIORITYCLASS ? 1 : 0;
    dlg->m_EASportsHack = t->flags5 & EASPORTSHACK ? 1 : 0;
    dlg->m_LegacyAlloc = t->flags6 & LEGACYALLOC ? 1 : 0;
    dlg->m_DisableMaxWinMode = t->flags6 & DISABLEMAXWINMODE ? 1 : 0;
    dlg->m_NoImagehlp = t->flags5 & NOIMAGEHLP ? 1 : 0;
    dlg->m_ForcesHEL = t->flags3 & FORCESHEL ? 1 : 0;
    dlg->m_ForcesHAL = t->flags11 & FORCESHAL ? 1 : 0;
    dlg->m_ForcesNULL = t->flags11 & FORCESNULL ? 1 : 0;
    dlg->m_NoHALDevice = t->flags8 & NOHALDEVICE ? 1 : 0;
    dlg->m_NoTnLDevice = t->flags12 & NOTNLDEVICE ? 1 : 0;
    dlg->m_MinimalCaps = t->flags3 & MINIMALCAPS ? 1 : 0;
    dlg->m_SetZBufferBitDepths = t->flags6 & SETZBUFFERBITDEPTHS ? 1 : 0;
    dlg->m_ForcesSwapEffect = t->flags6 & FORCESWAPEFFECT ? 1 : 0;
    dlg->m_ColorFix = t->flags3 & COLORFIX ? 1 : 0;
    dlg->m_FixGlobalUnlock = t->flags7 & FIXGLOBALUNLOCK ? 1 : 0;
    dlg->m_FixFreeLibrary = t->flags7 & FIXFREELIBRARY ? 1 : 0;
    dlg->m_SkipFreeLibrary = t->flags3 & SKIPFREELIBRARY ? 1 : 0;
    dlg->m_LoadLibraryErr = t->flags8 & LOADLIBRARYERR ? 1 : 0;
    dlg->m_FixAlteredPath = t->flags3 & FIXALTEREDPATH ? 1 : 0;
    dlg->m_FixAdjustWinRect = t->flags8 & FIXADJUSTWINRECT ? 1 : 0;
    dlg->m_NoPixelFormat = t->flags3 & NOPIXELFORMAT ? 1 : 0;
    dlg->m_NoAlphaChannel = t->flags4 & NOALPHACHANNEL ? 1 : 0;
    dlg->m_FixRefCounter = t->flags4 & FIXREFCOUNTER ? 1 : 0;
    dlg->m_ReturnNullRef = t->flags4 & RETURNNULLREF ? 1 : 0;
    dlg->m_NoD3DReset = t->flags4 & NOD3DRESET ? 1 : 0;
    dlg->m_HideDesktop = t->flags4 & HIDEDESKTOP ? 1 : 0;
    dlg->m_HideTaskbar = t->flags6 & HIDETASKBAR ? 1 : 0;
    dlg->m_NoTaskbarOverlap = t->flags9 & NOTASKBAROVERLAP ? 1 : 0;
    dlg->m_SetDPIAware = t->flags & SETDPIAWARE ? 1 : 0;
    dlg->m_ActivateApp = t->flags6 & ACTIVATEAPP ? 1 : 0;
    dlg->m_D3DResolutionHack = t->flags9 & D3DRESOLUTIONHACK ? 1 : 0;
    dlg->m_FixAILSoundLocks = t->flags9 & FIXAILSOUNDLOCKS ? 1 : 0;
    dlg->m_NoDestroyWindow = t->flags6 & NODESTROYWINDOW ? 1 : 0;
    dlg->m_LockSysColors = t->flags3 & LOCKSYSCOLORS ? 1 : 0;
    dlg->m_LockReservedPalette = t->flags5 & LOCKRESERVEDPALETTE ? 1 : 0;
    dlg->m_LimitScreenRes = t->flags4 & LIMITSCREENRES ? 1 : 0;
    dlg->m_SingleProcAffinity = t->flags3 & SINGLEPROCAFFINITY ? 1 : 0;
    dlg->m_UseLastCore = t->flags5 & USELASTCORE ? 1 : 0;
    dlg->m_LimitResources = t->flags2 & LIMITRESOURCES ? 1 : 0;
    dlg->m_CDROMDriveType = t->flags3 & CDROMDRIVETYPE ? 1 : 0;
    dlg->m_HideCDROMEmpty = t->flags4 & HIDECDROMEMPTY ? 1 : 0;
    dlg->m_FakeHD = t->flags10 & FAKEHDDRIVE ? 1 : 0;
    dlg->m_FakeCD = t->flags10 & FAKECDDRIVE ? 1 : 0;
    dlg->m_HookGOGLibs = t->flags6 & HOOKGOGLIBS ? 1 : 0;
    dlg->m_BypassGOGLibs = t->flags6 & BYPASSGOGLIBS ? 1 : 0;
    dlg->m_FontBypass = t->flags3 & FONTBYPASS ? 1 : 0;
    dlg->m_BufferedIOFix = t->flags3 & BUFFEREDIOFIX ? 1 : 0;
    dlg->m_ZBufferClean = t->flags4 & ZBUFFERCLEAN ? 1 : 0;
    dlg->m_ZBuffer0Clean = t->flags4 & ZBUFFER0CLEAN ? 1 : 0;
    dlg->m_DynamicZClean = t->flags8 & DYNAMICZCLEAN ? 1 : 0;
    dlg->m_ZBufferHardClean = t->flags8 & ZBUFFERHARDCLEAN ? 1 : 0;
    dlg->m_ZBufferAlways = t->dflags & ZBUFFERALWAYS ? 1 : 0;
    dlg->m_HotPatchAlways = t->dflags & HOTPATCHALWAYS ? 1 : 0;
    dlg->m_FreezeInjectedSon = t->dflags & FREEZEINJECTEDSON ? 1 : 0;
    dlg->m_StressResources = t->dflags & STRESSRESOURCES ? 1 : 0;
    dlg->m_Experimental = t->dflags2 & EXPERIMENTAL ? 1 : 0;
    dlg->m_FixRandomPalette = t->dflags2 & FIXRANDOMPALETTE ? 1 : 0;
    dlg->m_NoPower2Fix = t->flags4 & NOPOWER2FIX ? 1 : 0;
    dlg->m_NoPerfCounter = t->flags4 & NOPERFCOUNTER ? 1 : 0;
    dlg->m_DisableFogging = t->flags4 & DISABLEFOGGING ? 1 : 0;
    dlg->m_ClearTarget = t->flags5 & CLEARTARGET ? 1 : 0;
    dlg->m_FixPitch = t->flags6 & FIXPITCH ? 1 : 0;
    dlg->m_Power2Width = t->flags6 & POWER2WIDTH ? 1 : 0;
    dlg->m_SlowDown = t->flags & SLOWDOWN ? 1 : 0;
    dlg->m_BlitFromBackBuffer = t->flags & BLITFROMBACKBUFFER ? 1 : 0;
    dlg->m_NoFlipEmulation = t->flags4 & NOFLIPEMULATION ? 1 : 0;
    dlg->m_OffscreenZBuffer = t->flags8 & OFFSCREENZBUFFER ? 1 : 0;
    dlg->m_NoZBufferAttach = t->flags10 & NOZBUFATTACH ? 1 : 0;
    dlg->m_TexturePalette = t->flags12 & TEXTUREPALETTE ? 1 : 0;
    dlg->m_LockColorDepth = t->flags7 & LOCKCOLORDEPTH ? 1 : 0;
    dlg->m_Lock24BitDepth = t->flags & LOCK24BITDEPTH ? 1 : 0;
    dlg->m_FullPaintRect = t->flags3 & FULLPAINTRECT ? 1 : 0;
    dlg->m_PushActiveMovie = t->flags5 & PUSHACTIVEMOVIE ? 1 : 0;
    dlg->m_ForceClipChildren = t->flags4 & FORCECLIPCHILDREN ? 1 : 0;
    dlg->m_PreventMinimize = t->flags4 & PREVENTMINIMIZE ? 1 : 0;
    dlg->m_NoAccessibility = t->flags5 & NOACCESSIBILITY ? 1 : 0;
    dlg->m_IgnoreDebOutput = t->flags4 & IGNOREDEBOUTPUT ? 1 : 0;
    dlg->m_NoOleInitialize = t->flags10 & NOOLEINITIALIZE ? 1 : 0;
    dlg->m_ChaosOverlordsFix = t->flags10 & CHAOSOVERLORDSFIX ? 1 : 0;
    dlg->m_FixFolderPaths = t->flags10 & FIXFOLDERPATHS ? 1 : 0;
    dlg->m_NoComplexMipmaps = t->flags10 & NOCOMPLEXMIPMAPS ? 1 : 0;
    dlg->m_InvalidateClient = t->flags10 & INVALIDATECLIENT ? 1 : 0;
    dlg->m_CreateDCHook = t->flags10 & CREATEDCHOOK ? 1 : 0;
    dlg->m_SafePrimLock = t->flags10 & SAFEPRIMLOCK ? 1 : 0;
    dlg->m_SmackBufferNoDepth = t->flags11 & SMACKBUFFERNODEPTH ? 1 : 0;
    dlg->m_CustomLocale = t->flags11 & CUSTOMLOCALE ? 1 : 0;
    dlg->m_ClassLocale = t->flags12 & CLASSLOCALE ? 1 : 0;
    dlg->m_LockSysSettings = t->flags11 & LOCKSYSSETTINGS ? 1 : 0;
    dlg->m_ProjectBuffer = t->flags12 & PROJECTBUFFER ? 1 : 0;
    dlg->m_ForceRelAxis = t->flags12 & FORCERELAXIS ? 1 : 0;
    dlg->m_ForceAbsAxis = t->flags12 & FORCEABSAXIS ? 1 : 0;
    dlg->m_DirectXReplace = t->flags12 & DIRECTXREPLACE ? 1 : 0;
    dlg->m_W98OpaqueFont = t->flags12 & W98OPAQUEFONT ? 1 : 0;
    dlg->m_FakeGlobalAtom = t->flags12 & FAKEGLOBALATOM ? 1 : 0;
    dlg->m_RevertDIBPalette = t->flags12 & REVERTDIBPALETTE ? 1 : 0;
    dlg->m_FixDCAlreadyCreated = t->flags12 & FIXDCALREADYCREATED ? 1 : 0;
    dlg->m_SuppressMenus = t->flags12 & SUPPRESSMENUS ? 1 : 0;
    dlg->m_KillDeadlocks = t->flags12 & KILLDEADLOCKS ? 1 : 0;
    dlg->m_CommitPage = t->flags12 & COMMITPAGE ? 1 : 0;
    dlg->m_D3D8MaxWinMode = t->flags11 & D3D8MAXWINMODE ? 1 : 0;
    dlg->m_Mutex4CritSection = t->flags11 & MUTEX4CRITSECTION ? 1 : 0;
    dlg->m_DelayCritSection = t->flags11 & DELAYCRITSECTION ? 1 : 0;
    dlg->m_RemapNumKeypad = t->flags11 & REMAPNUMKEYPAD ? 1 : 0;
    dlg->m_SetUSKeyDescr = t->flags11 & SETUSKEYDESCR ? 1 : 0;
    dlg->m_DisableGammaRamp = t->flags2 & DISABLEGAMMARAMP ? 1 : 0;
    dlg->m_PALDIBEmulation = t->flags2 & PALDIBEMULATION ? 1 : 0;
    dlg->m_RefreshOnRealize = t->flags3 & REFRESHONREALIZE ? 1 : 0;
    dlg->m_LoadGammaRamp = t->flags8 & LOADGAMMARAMP ? 1 : 0;
    dlg->m_AutoRefresh = t->flags & AUTOREFRESH ? 1 : 0;
    dlg->m_IndependentRefresh = t->flags2 & INDEPENDENTREFRESH ? 1 : 0;
    dlg->m_TextureFormat = t->flags5 & TEXTUREFORMAT ? 1 : 0;
    dlg->m_VideoToSystemMem = t->flags & SWITCHVIDEOMEMORY ? 1 : 0;
    dlg->m_FixTextOut = t->flags & FIXTEXTOUT ? 1 : 0;
    dlg->m_ShrinkFontWidth = t->flags11 & SHRINKFONTWIDTH ? 1 : 0;
    dlg->m_SharedDC = t->flags6 & SHAREDDC ? 1 : 0;
    dlg->m_HookGlide = t->flags4 & HOOKGLIDE ? 1 : 0;
    dlg->m_SuppressGlide = t->flags12 & SUPPRESSGLIDE ? 1 : 0;
    dlg->m_RemapMCI = t->flags5 & REMAPMCI ? 1 : 0;
    dlg->m_NoMovies = t->flags6 & NOMOVIES ? 1 : 0;
    dlg->m_FixMoviesColor = t->flags6 & FIXMOVIESCOLOR ? 1 : 0;
    dlg->m_StretchMovies = t->flags6 & STRETCHMOVIES ? 1 : 0;
    dlg->m_BypassMCI = t->flags6 & BYPASSMCI ? 1 : 0;
    dlg->m_FixPCMAudio = t->flags8 & FIXAUDIOPCM ? 1 : 0;
    dlg->m_SuspendTimeStretch = t->flags10 & SUSPENDTIMESTRETCH ? 1 : 0;
    dlg->m_SuppressRelease = t->flags6 & SUPPRESSRELEASE ? 1 : 0;
    dlg->m_KeepCursorWithin = t->flags & KEEPCURSORWITHIN ? 1 : 0;
    dlg->m_KeepCursorFixed = t->flags2 & KEEPCURSORFIXED ? 1 : 0;
    dlg->m_UseRGB565 = t->flags & USERGB565 ? 1 : 0;
    dlg->m_SuppressDXErrors = t->flags & SUPPRESSDXERRORS ? 1 : 0;
    dlg->m_FlipEmulation = t->flags6 & FLIPEMULATION ? 1 : 0;
    dlg->m_MarkBlit = t->dflags & MARKBLIT ? 1 : 0;
    dlg->m_MarkLock = t->dflags & MARKLOCK ? 1 : 0;
    dlg->m_MarkWinG32 = t->dflags & MARKWING32 ? 1 : 0;
    dlg->m_MarkGDI32 = t->dflags & MARKGDI32 ? 1 : 0;
    dlg->m_DumpDIBSection = t->dflags & DUMPDIBSECTION ? 1 : 0;
    dlg->m_CaptureScreens = t->dflags & CAPTURESCREENS ? 1 : 0;
    dlg->m_PatchExecuteBuffer = t->dflags & PATCHEXECUTEBUFFER ? 1 : 0;
    dlg->m_ForceZBufferOn = t->dflags & FORCEZBUFFERON ? 1 : 0;
    dlg->m_ForceZBufferOff = t->dflags & FORCEZBUFFEROFF ? 1 : 0;
    dlg->m_DumpDevContext = t->dflags & DUMPDEVCONTEXT ? 1 : 0;
    dlg->m_DumpCPUID = t->dflags & DUMPCPUID ? 1 : 0;
    dlg->m_NoSysMemPrimary = t->flags6 & NOSYSMEMPRIMARY ? 1 : 0;
    dlg->m_NoSysMemBackBuf = t->flags6 & NOSYSMEMBACKBUF ? 1 : 0;
    dlg->m_DumpSurfaces = t->dflags & DUMPSURFACES ? 1 : 0;
    dlg->m_DumpBlitSrc = t->dflags2 & DUMPBLITSRC ? 1 : 0;
    dlg->m_DumpBitmaps = t->dflags2 & DUMPBITMAPS ? 1 : 0;
    dlg->m_FastBlt = t->dflags & DOFASTBLT ? 1 : 0;
    //dlg->m_GDIColorConv = t->flags5 & GDICOLORCONV ? 1 : 0;
    dlg->m_PreventMaximize = t->flags & PREVENTMAXIMIZE ? 1 : 0;
    dlg->m_EmulateMaximize = t->flags9 & EMULATEMAXIMIZE ? 1 : 0;
    dlg->m_ClientRemapping = t->flags & CLIENTREMAPPING ? 1 : 0;
    dlg->m_LockWinStyle = t->flags & LOCKWINSTYLE ? 1 : 0;
    dlg->m_DisableDWM = t->flags12 & DISABLEDWM ? 1 : 0;
    dlg->m_FixParentWin = t->flags & FIXPARENTWIN ? 1 : 0;
    dlg->m_KeepAspectRatio = t->flags2 & KEEPASPECTRATIO ? 1 : 0;
    dlg->m_AdaptiveRatio = t->flags11 & ADAPTIVERATIO ? 1 : 0;
    dlg->m_ForceWinResize = t->flags2 & FORCEWINRESIZE ? 1 : 0;
    dlg->m_HideMultiMonitor = t->flags2 & HIDEMULTIMONITOR ? 1 : 0;
    dlg->m_FixD3DFrame = t->flags3 & FIXD3DFRAME ? 1 : 0;
    dlg->m_NoWindowMove = t->flags3 & NOWINDOWMOVE ? 1 : 0;
    dlg->m_HookChildWin = t->flags & HOOKCHILDWIN ? 1 : 0;
    dlg->m_HookDlgWin = t->flags8 & HOOKDLGWIN ? 1 : 0;
    dlg->m_MessageProc = t->flags & MESSAGEPROC ? 1 : 0;
    dlg->m_FixMouseHook = t->flags8 & FIXMOUSEHOOK ? 1 : 0;
    dlg->m_FixMessageHook = t->flags11 & FIXMESSAGEHOOK ? 1 : 0;
    dlg->m_FixNCHITTEST = t->flags2 & FIXNCHITTEST ? 1 : 0;
    dlg->m_RecoverScreenMode = t->flags2 & RECOVERSCREENMODE ? 1 : 0;
    dlg->m_RefreshOnResize = t->flags2 & REFRESHONRESIZE ? 1 : 0;
    dlg->m_BackBufAttach = t->flags2 & BACKBUFATTACH ? 1 : 0;
    dlg->m_ClearTextureFourCC = t->flags7 & CLEARTEXTUREFOURCC ? 1 : 0;
    dlg->m_SuppressFourCCBlt = t->flags10 & SUPPRESSFOURCCBLT ? 1 : 0;
    dlg->m_NoDDExclusiveMode = t->flags7 & NODDEXCLUSIVEMODE ? 1 : 0;
    dlg->m_LockFullscreenCoop = t->flags9 & LOCKFULLSCREENCOOP ? 1 : 0;
    dlg->m_CreateDesktop = t->flags6 & CREATEDESKTOP ? 1 : 0;
    dlg->m_SafePaletteUsage = t->flags9 & SAFEPALETTEUSAGE ? 1 : 0;
    dlg->m_AllowSysmemOn3DDev = t->flags8 & ALLOWSYSMEMON3DDEV ? 1 : 0;
    dlg->m_HandleAltF4 = t->flags & HANDLEALTF4 ? 1 : 0;
    dlg->m_LimitFPS = t->flags2 & LIMITFPS ? 1 : 0;
    dlg->m_LimitDIBOperations = t->flags10 & LIMITDIBOPERATIONS ? 1 : 0;
    dlg->m_LimitBeginScene = t->flags5 & LIMITBEGINSCENE ? 1 : 0;
    dlg->m_LimitFlipOnly = t->flags12 & LIMITFLIPONLY ? 1 : 0;
    dlg->m_SkipFPS = t->flags2 & SKIPFPS ? 1 : 0;
    dlg->m_ShowTimeStretch = t->flags4 & SHOWTIMESTRETCH ? 1 : 0;
    dlg->m_TimeStretch = t->flags2 & TIMESTRETCH ? 1 : 0;
    dlg->m_StretchTimers = t->flags4 & STRETCHTIMERS ? 1 : 0;
    dlg->m_NormalizePerfCount = t->flags5 & NORMALIZEPERFCOUNT ? 1 : 0;
    dlg->m_StretchPerFrequency = t->flags12 & STRETCHPERFREQUENCY ? 1 : 0;
    dlg->m_SlowWinPolling = t->flags10 & SLOWWINPOLLING ? 1 : 0;
    dlg->m_CPUSlowDown = t->flags7 & CPUSLOWDOWN ? 1 : 0;
    dlg->m_CPUMaxUsage = t->flags7 & CPUMAXUSAGE ? 1 : 0;
    dlg->m_PreciseTiming = t->flags10 & PRECISETIMING ? 1 : 0;
    dlg->m_KillVSync = t->flags12 & KILLVSYNC ? 1 : 0;
    dlg->m_QuarterBlt = t->flags5 & QUARTERBLT ? 1 : 0;
    dlg->m_MakeWinVisible = t->flags9 & MAKEWINVISIBLE ? 1 : 0;
    dlg->m_FixEmpireOFS = t->flags9 & FIXEMPIREOFS ? 1 : 0;
    dlg->m_KillBlackWin = t->flags9 & KILLBLACKWIN ? 1 : 0;
    dlg->m_ZeroDisplayCounter = t->flags9 & ZERODISPLAYCOUNTER ? 1 : 0;
    dlg->m_InvertMouseXAxis = t->flags11 & INVERTMOUSEXAXIS ? 1 : 0;
    dlg->m_InvertMouseYAxis = t->flags11 & INVERTMOUSEYAXIS ? 1 : 0;
    dlg->m_ReplaceDialogs = t->flags10 & REPLACEDIALOGS ? 1 : 0;
    dlg->m_HandleFourCC = t->flags10 & HANDLEFOURCC ? 1 : 0;
    dlg->m_CacheD3DSession = t->flags9 & CACHED3DSESSION ? 1 : 0;
    dlg->m_FineTiming = t->flags4 & FINETIMING ? 1 : 0;
    dlg->m_ReleaseMouse = t->flags4 & RELEASEMOUSE ? 1 : 0;
    dlg->m_EnableTimeFreeze = t->flags4 & ENABLETIMEFREEZE ? 1 : 0;
    dlg->m_EnableHotKeys = t->flags4 & ENABLEHOTKEYS ? 1 : 0;
    dlg->m_NoDisablePrint = t->flags11 & NODISABLEPRINT ? 1 : 0;
    dlg->m_FixAsyncKeyState = t->flags11 & FIXASYNCKEYSTATE ? 1 : 0;
    dlg->m_InterceptRDTSC = t->flags4 & INTERCEPTRDTSC ? 1 : 0;
    dlg->m_HookOpenGL = t->flags2 & HOOKOPENGL ? 1 : 0;
    dlg->m_ForceHookOpenGL = t->flags3 & FORCEHOOKOPENGL ? 1 : 0;
    dlg->m_FixPixelZoom = t->flags6 & FIXPIXELZOOM ? 1 : 0;
    dlg->m_FixBindTexture = t->flags7 & FIXBINDTEXTURE ? 1 : 0;
    dlg->m_HookGlut32 = t->flags7 & HOOKGLUT32 ? 1 : 0;
    dlg->m_HookWGLContext = t->flags9 & HOOKWGLCONTEXT ? 1 : 0;
    dlg->m_ScaleMainViewport = t->flags11 & SCALEMAINVIEWPORT ? 1 : 0;
    dlg->m_LockGLViewport = t->flags12 & LOCKGLVIEWPORT ? 1 : 0;
    dlg->m_GLExtensionsLie = t->flags12 & GLEXTENSIONSLIE ? 1 : 0;
    dlg->m_GLFixClamp = t->flags13 & GLFIXCLAMP ? 1 : 0;
    dlg->m_ScaleglBitmaps = t->flags9 & SCALEGLBITMAPS ? 1 : 0;
    dlg->m_WireFrame = t->flags2 & WIREFRAME ? 1 : 0;
    dlg->m_NoTextures = t->flags4 & NOTEXTURES ? 1 : 0;
    dlg->m_BlackWhite = t->flags3 & BLACKWHITE ? 1 : 0;
    dlg->m_FakeVersion = t->flags2 & FAKEVERSION ? 1 : 0;
    dlg->m_FullRectBlt = t->dflags & FULLRECTBLT ? 1 : 0;
    dlg->m_CenterToWin = t->dflags & CENTERTOWIN ? 1 : 0;
    dlg->m_LimitDdraw = t->flags7 & LIMITDDRAW ? 1 : 0;
    dlg->m_SuppressOverlay = t->flags7 & SUPPRESSOVERLAY ? 1 : 0;
    dlg->m_NoPaletteUpdate = t->flags2 & NOPALETTEUPDATE ? 1 : 0;
    dlg->m_CapMask = t->dflags & CAPMASK ? 1 : 0;
    dlg->m_NoWindowHooks = t->dflags2 & NOWINDOWHOOKS ? 1 : 0;
    dlg->m_DisableWinHooks = t->dflags2 & DISABLEWINHOOKS ? 1 : 0;
    dlg->m_HideWindowChanges = t->flags10 & HIDEWINDOWCHANGES ? 1 : 0;
    dlg->m_SkipIATHint = t->flags3 & SKIPIATHINT ? 1 : 0;
    dlg->m_NoDDRAWBlt = t->dflags & NODDRAWBLT ? 1 : 0;
    dlg->m_NoDDRAWFlip = t->dflags & NODDRAWFLIP ? 1 : 0;
    dlg->m_NoGDIBlt = t->dflags & NOGDIBLT ? 1 : 0;
    dlg->m_NoFillRect = t->flags4 & NOFILLRECT ? 1 : 0;
    dlg->m_FixClipperArea = t->flags7 & FIXCLIPPERAREA ? 1 : 0;
    dlg->m_SharedDCHybrid = t->flags8 & SHAREDDCHYBRID ? 1 : 0;
    dlg->m_SyncPalette = t->flags6 & SYNCPALETTE ? 1 : 0;
    dlg->m_NoWinErrors = t->flags7 & NOWINERRORS ? 1 : 0;
    dlg->m_NoDialogs = t->flags9 & NODIALOGS ? 1 : 0;
    dlg->m_InvalidateFullRect = t->flags11 & INVALIDATEFULLRECT ? 1 : 0;
    dlg->m_NoSetPixelFormat = t->flags12 & NOSETPIXELFORMAT ? 1 : 0;
    dlg->m_ScaleCBTHook = t->flags12 & SCALECBTHOOK ? 1 : 0;
    dlg->m_StretchDialogs = t->flags10 & STRETCHDIALOGS ? 1 : 0;
    dlg->m_PretendVisible = t->flags8 & PRETENDVISIBLE ? 1 : 0;
    dlg->m_WinInsulation = t->flags8 & WININSULATION ? 1 : 0;
    dlg->m_DisableMMX = t->flags5 & DISABLEMMX ? 1 : 0;
    dlg->m_SafeAllocs = t->flags11 & SAFEALLOCS ? 1 : 0;
    dlg->m_QualityFonts = t->flags8 & QUALITYFONTS ? 1 : 0;
    dlg->m_ReplacePrivOps = t->flags5 & REPLACEPRIVOPS ? 1 : 0;
    dlg->m_InitialRes = t->flags7 & INITIALRES ? 1 : 0;
    dlg->m_MaximumRes = t->flags7 & MAXIMUMRES ? 1 : 0;
    dlg->m_CustomRes = t->flags10 & CUSTOMRES ? 1 : 0;
    dlg->m_RemapSysFolders = t->flags11 & REMAPSYSFOLDERS ? 1 : 0;
    dlg->m_PosX = t->posx;
    dlg->m_PosY = t->posy;
    dlg->m_SizX = t->sizx;
    dlg->m_SizY = t->sizy;
    dlg->m_ScanLine = t->ScanLine;
    dlg->m_FixedVolume = t->FixedVolume;
    dlg->m_MaxFPS = t->MaxFPS;
    dlg->m_InitTS = t->InitTS + 8;
    dlg->m_FakeVersionId = t->FakeVersionId;
    dlg->m_MaxScreenRes = t->MaxScreenRes;
    dlg->m_InitResW = t->resw;
    dlg->m_InitResH = t->resh;
    dlg->m_SwapEffect = t->SwapEffect;
    dlg->m_FakeHDDrive.Format("%c:", t->FakeHDDrive);
    dlg->m_FakeCDDrive.Format("%c:", t->FakeCDDrive);
    dlg->m_CodePage = t->CodePage;
    dlg->m_Language = t->Language;
    dlg->m_Country = t->Country;
    // debug
    dlg->m_DisableCPUID = t->dflags & CPUDISABLECPUID ? 1 : 0;
    dlg->m_CPUDisableMMX = t->dflags & CPUDISABLEMMX ? 1 : 0;
    dlg->m_CPUDisableSSE = t->dflags & CPUDISABLESSE ? 1 : 0;
    dlg->m_CPUDisableSSE2 = t->dflags & CPUDISABLESSE2 ? 1 : 0;
    dlg->m_CPUDisablePBE = t->dflags & CPUDISABLEPBE ? 1 : 0;
    dlg->m_CPUVendorId = 0;
    switch(t->dflags & CPUVENDORMASK) {
    case 0:
        break;
    case CPUFAKEDXWNDCPU:
        dlg->m_CPUVendorId = 1;
        break;
    case CPUGENUINEINTEL:
        dlg->m_CPUVendorId = 2;
        break;
    case CPUAUTHENTICAMD:
        dlg->m_CPUVendorId = 3;
        break;
    case CPUCYRIXINSTEAD:
        dlg->m_CPUVendorId = 4;
        break;
    }
}

static void SaveConfigItem(TARGETMAP *TargetMap, PRIVATEMAP *PrivateMap, int i, char *InitPath) {
    char key[32], val[32];
    char *EscBuf = NULL;
    sprintf_s(key, sizeof(key), "title%i", i);
    WritePrivateProfileString("target", key, PrivateMap->title, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "path%i", i);
    WritePrivateProfileString("target", key, TargetMap->path, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "startfolder%i", i);
    WritePrivateProfileString("target", key, PrivateMap->startfolder, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "launchpath%i", i);
    WritePrivateProfileString("target", key, PrivateMap->launchpath, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "cmdline%i", i);
    WritePrivateProfileString("target", key, PrivateMap->cmdline, InitPath);
    // -------
    if(PrivateMap->IconString) {
        sprintf_s(key, sizeof(key), "icon%i", i);
        WritePrivateProfileString("target", key, PrivateMap->IconString.GetBuffer(), InitPath);
    } else
        WritePrivateProfileString("target", key, NULL, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "module%i", i);
    WritePrivateProfileString("target", key, TargetMap->module, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "opengllib%i", i);
    WritePrivateProfileString("target", key, TargetMap->OpenGLLib, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "notes%i", i);
    WritePrivateProfileString("target", key, Escape(PrivateMap->notes, &EscBuf), InitPath);
    // -------
    sprintf_s(key, sizeof(key), "registry%i", i);
    WritePrivateProfileString("target", key, Escape(PrivateMap->registry, &EscBuf), InitPath);
    // -------
    sprintf_s(key, sizeof(key), "ver%i", i);
    sprintf_s(val, sizeof(val), "%i", TargetMap->dxversion);
    WritePrivateProfileString("target", key, val, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "monitorid%i", i);
    sprintf_s(val, sizeof(val), "%i", TargetMap->monitorid);
    WritePrivateProfileString("target", key, val, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "filterid%i", i);
    sprintf_s(val, sizeof(val), "%i", TargetMap->FilterId);
    WritePrivateProfileString("target", key, val, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "renderer%i", i);
    sprintf_s(val, sizeof(val), "%i", TargetMap->RendererId);
    WritePrivateProfileString("target", key, val, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "coord%i", i);
    sprintf_s(val, sizeof(val), "%i", TargetMap->coordinates);
    WritePrivateProfileString("target", key, val, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "flag%i", i);
    sprintf_s(val, sizeof(val), "%i", TargetMap->flags);
    WritePrivateProfileString("target", key, val, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "flagg%i", i);
    sprintf_s(val, sizeof(val), "%i", TargetMap->flags2);
    WritePrivateProfileString("target", key, val, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "flagh%i", i);
    sprintf_s(val, sizeof(val), "%i", TargetMap->flags3);
    WritePrivateProfileString("target", key, val, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "flagi%i", i);
    sprintf_s(val, sizeof(val), "%i", TargetMap->flags4);
    WritePrivateProfileString("target", key, val, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "flagj%i", i);
    sprintf_s(val, sizeof(val), "%i", TargetMap->flags5);
    WritePrivateProfileString("target", key, val, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "flagk%i", i);
    sprintf_s(val, sizeof(val), "%i", TargetMap->flags6);
    WritePrivateProfileString("target", key, val, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "flagl%i", i);
    sprintf_s(val, sizeof(val), "%i", TargetMap->flags7);
    WritePrivateProfileString("target", key, val, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "flagm%i", i);
    sprintf_s(val, sizeof(val), "%i", TargetMap->flags8);
    WritePrivateProfileString("target", key, val, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "flagn%i", i);
    sprintf_s(val, sizeof(val), "%i", TargetMap->flags9);
    WritePrivateProfileString("target", key, val, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "flago%i", i);
    sprintf_s(val, sizeof(val), "%i", TargetMap->flags10);
    WritePrivateProfileString("target", key, val, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "flagp%i", i);
    sprintf_s(val, sizeof(val), "%i", TargetMap->flags11);
    WritePrivateProfileString("target", key, val, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "flagq%i", i);
    sprintf_s(val, sizeof(val), "%i", TargetMap->flags12);
    WritePrivateProfileString("target", key, val, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "flagr%i", i);
    sprintf_s(val, sizeof(val), "%i", TargetMap->flags13);
    WritePrivateProfileString("target", key, val, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "flags%i", i);
    sprintf_s(val, sizeof(val), "%i", TargetMap->flags14);
    WritePrivateProfileString("target", key, val, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "flagt%i", i);
    sprintf_s(val, sizeof(val), "%i", TargetMap->flags15);
    WritePrivateProfileString("target", key, val, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "flagu%i", i);
    sprintf_s(val, sizeof(val), "%i", TargetMap->flags16);
    WritePrivateProfileString("target", key, val, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "tflag%i", i);
    sprintf_s(val, sizeof(val), "%i", TargetMap->tflags);
    WritePrivateProfileString("target", key, val, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "dflag%i", i);
    sprintf_s(val, sizeof(val), "%i", TargetMap->dflags);
    WritePrivateProfileString("target", key, val, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "dflagb%i", i);
    sprintf_s(val, sizeof(val), "%i", TargetMap->dflags2);
    WritePrivateProfileString("target", key, val, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "posx%i", i);
    sprintf_s(val, sizeof(val), "%i", TargetMap->posx);
    WritePrivateProfileString("target", key, val, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "posy%i", i);
    sprintf_s(val, sizeof(val), "%i", TargetMap->posy);
    WritePrivateProfileString("target", key, val, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "sizx%i", i);
    sprintf_s(val, sizeof(val), "%i", TargetMap->sizx);
    WritePrivateProfileString("target", key, val, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "sizy%i", i);
    sprintf_s(val, sizeof(val), "%i", TargetMap->sizy);
    WritePrivateProfileString("target", key, val, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "maxfps%i", i);
    sprintf_s(val, sizeof(val), "%i", TargetMap->MaxFPS);
    WritePrivateProfileString("target", key, val, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "initts%i", i);
    sprintf_s(val, sizeof(val), "%i", TargetMap->InitTS);
    WritePrivateProfileString("target", key, val, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "winver%i", i);
    sprintf_s(val, sizeof(val), "%i", TargetMap->FakeVersionId);
    WritePrivateProfileString("target", key, val, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "maxres%i", i);
    sprintf_s(val, sizeof(val), "%i", TargetMap->MaxScreenRes);
    WritePrivateProfileString("target", key, val, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "swapeffect%i", i);
    sprintf_s(val, sizeof(val), "%i", TargetMap->SwapEffect);
    WritePrivateProfileString("target", key, val, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "maxddinterface%i", i);
    sprintf_s(val, sizeof(val), "%i", TargetMap->MaxDdrawInterface);
    WritePrivateProfileString("target", key, val, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "slowratio%i", i);
    sprintf_s(val, sizeof(val), "%i", TargetMap->SlowRatio);
    WritePrivateProfileString("target", key, val, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "scanline%i", i);
    sprintf_s(val, sizeof(val), "%i", TargetMap->ScanLine);
    WritePrivateProfileString("target", key, val, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "initresw%i", i);
    sprintf_s(val, sizeof(val), "%i", TargetMap->resw);
    WritePrivateProfileString("target", key, val, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "initresh%i", i);
    sprintf_s(val, sizeof(val), "%i", TargetMap->resh);
    WritePrivateProfileString("target", key, val, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "fakehddrive%i", i);
    sprintf_s(val, sizeof(val), "%c:", TargetMap->FakeHDDrive);
    WritePrivateProfileString("target", key, val, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "fakecddrive%i", i);
    sprintf_s(val, sizeof(val), "%c:", TargetMap->FakeCDDrive);
    WritePrivateProfileString("target", key, val, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "cdvol%i", i);
    sprintf_s(val, sizeof(val), "%i", TargetMap->FixedVolume);
    WritePrivateProfileString("target", key, val, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "fakehd%i", i);
    if(strlen(PrivateMap->FakeHDPath))
        WritePrivateProfileString("target", key, PrivateMap->FakeHDPath, InitPath);
    else
        WritePrivateProfileString("target", key, NULL, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "fakecd%i", i);
    if(strlen(PrivateMap->FakeCDPath))
        WritePrivateProfileString("target", key, PrivateMap->FakeCDPath, InitPath);
    else
        WritePrivateProfileString("target", key, NULL, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "fakecdlabel%i", i);
    if(strlen(PrivateMap->FakeCDLabel))
        WritePrivateProfileString("target", key, PrivateMap->FakeCDLabel, InitPath);
    else
        WritePrivateProfileString("target", key, NULL, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "codepage%i", i);
    sprintf_s(val, sizeof(val), "%d", TargetMap->CodePage);
    WritePrivateProfileString("target", key, val, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "lang%i", i);
    sprintf_s(val, sizeof(val), "%d", TargetMap->Language);
    WritePrivateProfileString("target", key, val, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "country%i", i);
    sprintf_s(val, sizeof(val), "%d", TargetMap->Country);
    WritePrivateProfileString("target", key, val, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "commitaddress%i", i);
    sprintf_s(val, sizeof(val), "%d", PrivateMap->CommitAddress);
    WritePrivateProfileString("target", key, val, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "commitlength%i", i);
    sprintf_s(val, sizeof(val), "%d", PrivateMap->CommitLength);
    WritePrivateProfileString("target", key, val, InitPath);
    free(EscBuf);
    EscBuf = NULL;
    // beware: when using SaveConfigItem to export or proxy, the updated flag should not be updated
    // but we save & recover the previous state instead.
    PrivateMap->updated = FALSE;
}

static void SaveExportStamp(char *InitPath) {
    char val[32];
    GetDllVersion(val);
    time_t _tm = time(NULL);
    struct tm *curtime = localtime(& _tm);
    WritePrivateProfileString("stamp", "version", val, InitPath);
    WritePrivateProfileString("stamp", "time", asctime(curtime), InitPath);
}

static void ClearTarget(int i, char *InitPath) {
    char key[32];
    char *keys[] = {
        "title", "icon", "module", "opengllib", "monitorid", "filterid", "renderer",
        "dflag", "dflagb", "scanline", "initresw", "initresh", "fakehddrive", "fakecddrive",
        "path", "launchpath", "startfolder", "ver", "coord", "flag", "flagg", "flagh", "flagi",
        "flagj", "flagk", "flagl", "flagm", "flagn", "flago", "flagp", "flagq", "flagr", "flags",
        "flagt", "flagu", "tflag", "initx", "cmdline",
        "inity", "minx", "miny", "maxx", "maxy", "posx", "posy", "sizx", "sizy", "maxfps",
        "swapeffect", "initts", "winver", "maxres", "notes", "registry", "maxddinterface", "slowratio",
        "fakec", "faked", "fakecd", "fakehd", "fakecdlabel", "cdvol", "codepage", "country", "lang",
        "commitaddress", "commitlength",
        NULL
    };
    for(int k = 0; keys[k]; k++) {
        sprintf_s(key, sizeof(key), "%s%i", keys[k], i);
        WritePrivateProfileString("target", key, 0, InitPath);
    }
}

static int LoadConfigItem(TARGETMAP *TargetMap, PRIVATEMAP *PrivateMap, int i, char *InitPath) {
    char key[32];
    char *EscBuf = NULL;
    char *sBuf;
    sBuf = (char *)malloc(1000000);
    extern BOOL gbDebug;
    extern BOOL gbExpertMode;
    PrivateMap->updated = FALSE;
    TargetMap->index = i;
    // -------
    sprintf_s(key, sizeof(key), "path%i", i);
    GetPrivateProfileString("target", key, "", TargetMap->path, MAX_PATH, InitPath);
    if(!TargetMap->path[0]) return FALSE;
    // -------
    sprintf_s(key, sizeof(key), "launchpath%i", i);
    GetPrivateProfileString("target", key, "", PrivateMap->launchpath, MAX_PATH, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "startfolder%i", i);
    GetPrivateProfileString("target", key, "", PrivateMap->startfolder, MAX_PATH, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "cmdline%i", i);
    GetPrivateProfileString("target", key, "", PrivateMap->cmdline, MAX_PATH, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "title%i", i);
    GetPrivateProfileString("target", key, "", PrivateMap->title, sizeof(PRIVATEMAP) - 1, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "module%i", i);
    GetPrivateProfileString("target", key, "", TargetMap->module, sizeof(TargetMap->module) - 1, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "opengllib%i", i);
    GetPrivateProfileString("target", key, "", TargetMap->OpenGLLib, sizeof(TargetMap->OpenGLLib) - 1, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "notes%i", i);
    GetPrivateProfileString("target", key, "", sBuf, 1000000, InitPath);
    Unescape(sBuf, &EscBuf);
    PrivateMap->notes = (char *)malloc(strlen(EscBuf) + 1);
    strcpy(PrivateMap->notes, EscBuf);
    // -------
    sprintf_s(key, sizeof(key), "registry%i", i);
    GetPrivateProfileString("target", key, "", sBuf, 1000000, InitPath);
    Unescape(sBuf, &EscBuf);
    PrivateMap->registry = (char *)malloc(strlen(EscBuf) + 1);
    strcpy(PrivateMap->registry, EscBuf);
    // -------
    sprintf_s(key, sizeof(key), "ver%i", i);
    TargetMap->dxversion = GetPrivateProfileInt("target", key, 0, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "monitorid%i", i);
    TargetMap->monitorid = GetPrivateProfileInt("target", key, 0, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "filterid%i", i);
    TargetMap->FilterId = GetPrivateProfileInt("target", key, 0, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "renderer%i", i);
    TargetMap->RendererId = GetPrivateProfileInt("target", key, 3, InitPath); // beware: set 3 (surface emulation) as default!!
    // -------
    sprintf_s(key, sizeof(key), "coord%i", i);
    TargetMap->coordinates = GetPrivateProfileInt("target", key, 0, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "flag%i", i);
    TargetMap->flags = GetPrivateProfileInt("target", key, 0, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "flagg%i", i);
    TargetMap->flags2 = GetPrivateProfileInt("target", key, 0, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "flagh%i", i);
    TargetMap->flags3 = GetPrivateProfileInt("target", key, 0, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "flagi%i", i);
    TargetMap->flags4 = GetPrivateProfileInt("target", key, 0, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "flagj%i", i);
    TargetMap->flags5 = GetPrivateProfileInt("target", key, 0, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "flagk%i", i);
    TargetMap->flags6 = GetPrivateProfileInt("target", key, 0, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "flagl%i", i);
    TargetMap->flags7 = GetPrivateProfileInt("target", key, 0, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "flagm%i", i);
    TargetMap->flags8 = GetPrivateProfileInt("target", key, 0, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "flagn%i", i);
    TargetMap->flags9 = GetPrivateProfileInt("target", key, 0, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "flago%i", i);
    TargetMap->flags10 = GetPrivateProfileInt("target", key, 0, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "flagp%i", i);
    TargetMap->flags11 = GetPrivateProfileInt("target", key, 0, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "flagq%i", i);
    TargetMap->flags12 = GetPrivateProfileInt("target", key, 0, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "flagr%i", i);
    TargetMap->flags13 = GetPrivateProfileInt("target", key, 0, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "flags%i", i);
    TargetMap->flags14 = GetPrivateProfileInt("target", key, 0, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "flagt%i", i);
    TargetMap->flags15 = GetPrivateProfileInt("target", key, 0, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "flagu%i", i);
    TargetMap->flags16 = GetPrivateProfileInt("target", key, 0, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "tflag%i", i);
    TargetMap->tflags = GetPrivateProfileInt("target", key, 0, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "dflag%i", i);
    TargetMap->dflags = GetPrivateProfileInt("target", key, 0, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "dflagb%i", i);
    TargetMap->dflags2 = GetPrivateProfileInt("target", key, 0, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "posx%i", i);
    TargetMap->posx = GetPrivateProfileInt("target", key, 0, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "posy%i", i);
    TargetMap->posy = GetPrivateProfileInt("target", key, 0, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "sizx%i", i);
    TargetMap->sizx = GetPrivateProfileInt("target", key, 0, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "sizy%i", i);
    TargetMap->sizy = GetPrivateProfileInt("target", key, 0, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "maxfps%i", i);
    TargetMap->MaxFPS = GetPrivateProfileInt("target", key, 0, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "initts%i", i);
    TargetMap->InitTS = GetPrivateProfileInt("target", key, 0, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "swapeffect%i", i);
    TargetMap->SwapEffect = GetPrivateProfileInt("target", key, 0, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "winver%i", i);
    TargetMap->FakeVersionId = GetPrivateProfileInt("target", key, 0, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "maxres%i", i);
    TargetMap->MaxScreenRes = GetPrivateProfileInt("target", key, 0, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "maxddinterface%i", i);
    TargetMap->MaxDdrawInterface = GetPrivateProfileInt("target", key, 7, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "slowratio%i", i);
    TargetMap->SlowRatio = GetPrivateProfileInt("target", key, 1, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "scanline%i", i);
    TargetMap->ScanLine = GetPrivateProfileInt("target", key, 1, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "initresw%i", i);
    TargetMap->resw = GetPrivateProfileInt("target", key, 0, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "initresh%i", i);
    TargetMap->resh = GetPrivateProfileInt("target", key, 0, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "fakehd%i", i);
    GetPrivateProfileString("target", key, "", PrivateMap->FakeHDPath, MAX_PATH, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "fakecd%i", i);
    GetPrivateProfileString("target", key, "", PrivateMap->FakeCDPath, MAX_PATH, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "fakecdlabel%i", i);
    GetPrivateProfileString("target", key, "", PrivateMap->FakeCDLabel, 256, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "fakehddrive%i", i);
    GetPrivateProfileString("target", key, "C:", sBuf, 32, InitPath);
    TargetMap->FakeHDDrive = sBuf[0];
    // -------
    sprintf_s(key, sizeof(key), "fakecddrive%i", i);
    GetPrivateProfileString("target", key, "D:", sBuf, 32, InitPath);
    TargetMap->FakeCDDrive = sBuf[0];
    // -------
    sprintf_s(key, sizeof(key), "cdvol%i", i);
    TargetMap->FixedVolume = GetPrivateProfileInt("target", key, 0, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "codepage%i", i);
    TargetMap->CodePage = GetPrivateProfileInt("target", key, 0, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "lang%i", i);
    TargetMap->Language = GetPrivateProfileInt("target", key, 0, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "country%i", i);
    TargetMap->Country = GetPrivateProfileInt("target", key, 0, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "commitaddress%i", i);
    PrivateMap->CommitAddress = GetPrivateProfileInt("target", key, 0, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "commitlength%i", i);
    PrivateMap->CommitLength = GetPrivateProfileInt("target", key, 0, InitPath);
    // -------
    sprintf_s(key, sizeof(key), "icon%i", i);
    GetPrivateProfileString("target", key, "", sBuf, 1000000, InitPath);
    PrivateMap->IconString = (CString)sBuf;
    if(strlen(sBuf)) {
        CGameIcons *ic = new(CGameIcons);
        HICON Icon;
        Icon = ic->DeSerialize(sBuf);
        if(Icon) {
            if(PrivateMap->Icon) ::DestroyIcon(PrivateMap->Icon);
            PrivateMap->Icon = Icon;
        }
    } else
        PrivateMap->Icon = NULL;
    if (!gbDebug) {
        // clear debug flags
        TargetMap->dflags = 0;
        TargetMap->dflags2 = 0;
    }
    free(EscBuf);
    EscBuf = NULL;
    free(sBuf);
    return TRUE;
}

static int SetTargetIcon(TARGETMAP tm) {
    FILE *target;
    target = fopen(tm.path, "r");
    if (target == NULL) return 3;
    fclose(target);
    if (tm.flags7 & HOOKNORUN) return 5;
    if (tm.flags3 & HOOKENABLED) return ((tm.flags2 & STARTDEBUG) || (tm.flags7 & INJECTSUSPENDED)) ? 2 : 1;
    return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CDxwndhostView class constructor / destructor

CDxwndhostView::CDxwndhostView() {
    // initialize structures
    memset(&StartupStickyKeys, 0, sizeof(STICKYKEYS));
    memset(&StartupToggleKeys, 0, sizeof(TOGGLEKEYS));
    memset(&StartupFilterKeys, 0, sizeof(FILTERKEYS));
    StartupStickyKeys.cbSize = sizeof(STICKYKEYS);
    StartupToggleKeys.cbSize = sizeof(TOGGLEKEYS);
    StartupFilterKeys.cbSize = sizeof(FILTERKEYS);
    // get all needed info
    EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &this->InitDevMode);
    SystemParametersInfo(SPI_GETCLEARTYPE, NULL, &InitClearType, NULL);
    SystemParametersInfo(SPI_GETFONTSMOOTHING, NULL, &InitFontSmoothing, NULL);
    SystemParametersInfo(SPI_GETFONTSMOOTHINGCONTRAST, NULL, &InitFontSmoothingContrast, NULL);
    SystemParametersInfo(SPI_GETFONTSMOOTHINGORIENTATION, NULL, &InitFontSmoothingOrientation, NULL);
    SystemParametersInfo(SPI_GETFONTSMOOTHINGTYPE, NULL, &InitFontSmoothingType, NULL);
    SystemParametersInfo(SPI_GETSTICKYKEYS, sizeof(STICKYKEYS), &StartupStickyKeys, 0);
    SystemParametersInfo(SPI_GETTOGGLEKEYS, sizeof(TOGGLEKEYS), &StartupToggleKeys, 0);
    SystemParametersInfo(SPI_GETFILTERKEYS, sizeof(FILTERKEYS), &StartupFilterKeys, 0);
    // log values
    OutTrace("Init ClearType=%d\n", InitClearType);
    OutTrace("Init FontSmoothing=%d\n", InitFontSmoothing);
    OutTrace("Init FontSmoothingContrast=%d\n", InitFontSmoothingContrast);
    OutTrace("Init FontSmoothingOrientation=%d\n", InitFontSmoothingOrientation);
    OutTrace("Init FontSmoothingType=%d\n", InitFontSmoothingType);
    OutTrace("Init StickyKeys=%x\n", StartupStickyKeys.dwFlags);
    OutTrace("Init ToggleKeys=%x\n", StartupToggleKeys.dwFlags);
    OutTrace("Init FilterKeys=%x:%d:%d:%d:%d\n",
             StartupFilterKeys.dwFlags,
             StartupFilterKeys.iBounceMSec,
             StartupFilterKeys.iDelayMSec,
             StartupFilterKeys.iRepeatMSec,
             StartupFilterKeys.iWaitMSec);
}

void CDxwndhostView::SaveConfigFile() {
    int i;
    DWORD tick0 = GetTickCount();
    int iUpdates = 0;
    BOOL bConfigLost = TRUE;
    extern BOOL gReadOnlyMode;
    if(gReadOnlyMode) return;
    // check if still there's a config file (it could have been deleted!)
    if(FileExists(gInitPath)) {
        bConfigLost = FALSE;
        // make a backup copy
        char sBakPath[MAX_PATH];
        strcpy(sBakPath, gInitPath);
        strcpy(&sBakPath[strlen(sBakPath) - 4], ".bak");
        CopyFile(gInitPath, sBakPath, FALSE);
    }
    // optimization: update only changed entries, or all in case of missing config file
    for(i = 0; i < MAXTARGETS; i ++) {
        if(!TargetMaps[i].path[0]) break;
        if(PrivateMaps[i].updated || bConfigLost) {
            SaveConfigItem(&TargetMaps[i], &PrivateMaps[i], i, gInitPath);
            iUpdates ++;
        }
    }
    // optimization: don't try to clear after first void entry
    //for(; i < MAXTARGETS; i ++) ClearTarget(i, gInitPath);
    ClearTarget(i, gInitPath);
    this->isUpdated = FALSE;
    this->isRegistryUpdated = FALSE;
    this->isDriversUpdated = FALSE;
    OutTrace("SaveConfigFile: %d updates in %d msec\n", iUpdates, GetTickCount() - tick0);
}

void CDxwndhostView::ZapConfigFile() {
    for(int i = 0; i < MAXTARGETS; i ++) {
        if(!TargetMaps[i].path[0]) break;
        TargetMaps[i].path[0] = 0;
        ClearTarget(i, gInitPath);
    }
    CListCtrl &listctrl = GetListCtrl();
    RenewItemList(&listctrl);
}

CDxwndhostView::~CDxwndhostView() {
    extern BOOL gReadOnlyMode;
    EndHook();
    RevertScreenChanges(&this->InitDevMode);
    SystemParametersInfo(SPI_SETSTICKYKEYS, sizeof(STICKYKEYS), &StartupStickyKeys, 0);
    SystemParametersInfo(SPI_SETTOGGLEKEYS, sizeof(TOGGLEKEYS), &StartupToggleKeys, 0);
    SystemParametersInfo(SPI_SETFILTERKEYS, sizeof(FILTERKEYS), &StartupFilterKeys, 0);
    if (this->isUpdated && !gReadOnlyMode) {
        if (MessageBoxLang(DXW_STRING_LISTUPDATE, DXW_STRING_WARNING, MB_YESNO | MB_ICONQUESTION) == IDYES)
            this->SaveConfigFile();
    }
    if(this->SystemTray.Enabled()) {
        this->SystemTray.StopAnimation();
        this->SystemTray.HideIcon();
    }
    RecoverGammaRamp();
}

#define KEYDELAY 1000

void CDxwndhostView::OnKeyUp(UINT c, UINT nRepCnt, UINT Flags) {
    switch(c) {
    case VK_DELETE:
        if((KeyPressed == VK_DELETE) && (GetTickCount() - KeyTime < KEYDELAY)) this->OnDelete();
        break;
    case VK_INSERT:
        if((KeyPressed == VK_INSERT) && (GetTickCount() - KeyTime < KEYDELAY))  this->OnAdd();
        break;
    case VK_RETURN:
        if((KeyPressed == VK_RETURN) && (GetTickCount() - KeyTime < KEYDELAY))  this->OnRun();
        break;
    case VK_CANCEL:
        if((KeyPressed == VK_CANCEL) && (GetTickCount() - KeyTime < KEYDELAY))  this->OnKill();
        break;
    case VK_ESCAPE:
        if((KeyPressed == VK_ESCAPE) && (GetTickCount() - KeyTime < KEYDELAY))  this->OnGoToTrayIcon();
        break;
    default:
        KeyPressed = 0;
        break;
    }
    CListView::OnKeyUp(c, nRepCnt, Flags);
}

void CDxwndhostView::OnKeyDown(UINT c, UINT nRepCnt, UINT Flags) {
    KeyTime = 0; // very early back in time!!
    switch(c) {
    case VK_ESCAPE: // v2.05.43 - "Hide on ESC Key" flag
        if(!gHideOnEscape) {
            KeyPressed = 0;
            break;
        }
    // otherwise, fall through ...
    case VK_DELETE:
    case VK_INSERT:
    case VK_RETURN:
    case VK_CANCEL:
        KeyPressed = c;
        KeyTime = GetTickCount();
        break;
    default:
        KeyPressed = 0;
        break;
    }
    CListView::OnKeyDown(c, nRepCnt, Flags);
}

void CDxwndhostView::OnExit() {
    // check for running apps ....
    if (GetHookStatus(NULL) == DXW_RUNNING) {
        if (MessageBoxLang(DXW_STRING_EXIT_BUSY, DXW_STRING_WARNING, MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL)
            return;
    }
    delete(this->GetParent());
}

BOOL CDxwndhostView::PreCreateWindow(CREATESTRUCT &cs) {
    cs.style |= LVS_REPORT | LVS_SINGLESEL | LVS_NOCOLUMNHEADER;
    cs.style |= LVS_AUTOARRANGE; // v2.05.43: always re-arrange icons
    //cs.style |= LVS_SHOWSELALWAYS; // v2.05.43 ???
    return CListView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CDxwndhostView Drawing Classes

void CDxwndhostView::OnDraw(CDC *pDC) {
    CDxwndhostDoc *pDoc = GetDocument();
    ASSERT_VALID(pDoc);
}

void CDxwndhostView::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2) {
    switch(nHotKeyId) {
    case 0: // minimize
        this->OnWindowMinimize();
        break;
    case 1: // restore
        this->OnWindowRestore();
        break;
    case 2: // kill
        this->OnEmergencyKill();
        break;
    }
}

void VKeyError(char *key) {
    char Msg[81];
    sprintf(Msg, "RegisterHotKey(%s) failed err=%d", key, GetLastError());
    MessageBox(NULL, Msg, "DxWnd error", MB_OK + MB_ICONWARNING);
}

void CDxwndhostView::UpdateHotKeys() {
    // Hot Keys
    DWORD dwKey;
    extern Key_Type HKeys[];
    extern KeyCombo_Type HKeyCombo[];
    // MessageBox("update global settings", "debug", MB_OK);
    for(int i = 0; HKeys[i].iLabelResourceId; i++) {
        dwKey = GetPrivateProfileInt("keymapping", HKeys[i].sIniLabel, 0, gInitPath);
        if(dwKey) {
            UnregisterHotKey(this->GetSafeHwnd(), i);
            if(!RegisterHotKey(this->GetSafeHwnd(), i, MOD_ALT + MOD_SHIFT + MOD_CONTROL, dwKey)) VKeyError(HKeys[i].sIniLabel);
        }
    }
}

void CDxwndhostView::AppendItemToList(TARGETMAP *t, PRIVATEMAP *p, int i, LV_ITEM *listitem) {
#ifdef ICONDEBUG
    char msg[256];
#endif
    listitem->mask = LVIF_TEXT | LVIF_IMAGE;
    listitem->iItem = i;
    listitem->iSubItem = 0;
    listitem->iImage = bIconViewMode ? i : SetTargetIcon(*t);
    listitem->pszText = p->title;
    CGameIcons *ic = new(CGameIcons); // gameiconz begin
    HICON Icon = NULL;
    if(!p->Icon) {
        Icon = ic->Extract(t->path);
        if(Icon) {
#ifdef ICONDEBUG
            sprintf(msg, "added icon=%x path=\"%s\"", Icon, t->path);
            MessageBox(msg, "debug", 0);
#endif
            p->Icon = Icon;
            char *buffer = ic->Serialize(Icon);
            if(buffer) {
                p->IconString = (CString)buffer;
                free(buffer);
            }
        } else
            p->Icon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_PROGRAM));
    }
    if(p->Icon) {
        if(!gGrayIcons || FileExists(t->path))
            m_cIconList.Add(p->Icon);
        else {
            HICON hico;
            // add a grayed icon
            CGameIcons *ci = new(CGameIcons);
            hico = ci->GrayIcon(p->Icon);
            // v2.05.23: in case of failure, use internal icon IDB_GRAYICON. Good for Wine/Linux.
            if(hico)
                m_cIconList.Add(hico);
            else {
                CBitmap *bmap = new(CBitmap);
                bmap->LoadBitmapA(IDB_GRAYICON);
                m_cIconList.Add(bmap, RGB(255, 0, 255));
            }
        }
    }
    //else {
    //	CBitmap *bmap = new(CBitmap);
    //	bmap->LoadBitmapA(IDB_PROGRAM);
    //	m_cIconList.Add(bmap, RGB(255, 0, 255));
    //}
#ifdef ICONDEBUG
    sprintf(msg, "item=\"%s\" icon=%x iconstr=%-8.8s", p->title, p->Icon, p->IconString);
    MessageBox(msg, "debug", 0);
#endif
}

void CDxwndhostView::RenewItemList(CListCtrl *listctrl) {
    listctrl->DeleteAllItems();
    for(int i = 0; TargetMaps[i].path[0]; i++) {
        TargetMaps[i].index = i; // renumber
        LV_ITEM listitem;
        listitem.mask = LVIF_TEXT | LVIF_IMAGE;
        listitem.iItem = i;
        listitem.iSubItem = 0;
        listitem.iImage = bIconViewMode ? i : SetTargetIcon(TargetMaps[i]);
        listitem.pszText = PrivateMaps[i].title;
        listctrl->SetItem(&listitem);
        listctrl->InsertItem(&listitem);
    }
}

void CDxwndhostView::RenewIconList(CListCtrl *listctrl) {
    m_cIconList.DeleteImageList();
    LV_ITEM listitem;
    HIMAGELIST hList = ImageList_Create(32, 32, ILC_COLOR24 | ILC_MASK, 0, 1);
    m_cIconList.Attach(hList);
    for(int i = 0; TargetMaps[i].path[0]; i++)
        AppendItemToList(&TargetMaps[i], &PrivateMaps[i], i, &listitem);
    listctrl->SetImageList(&m_cIconList, LVSIL_NORMAL);
    listctrl->ModifyStyle(LVS_ICON | LVS_LIST | LVS_REPORT | LVS_SMALLICON, 0);
    listctrl->ModifyStyle(0, bIconViewMode ? LVS_ICON : LVS_REPORT);
}

#define g_szMailslot "\\\\.\\mailslot\\DxWndMailSlot"

void CDxwndhostView::Import(char *path) {
    this->OnImport(path);
}

DWORD WINAPI DxWndServer(LPVOID lpParameter) {
    CDxwndhostView *lpView = (CDxwndhostView *)lpParameter;
    HANDLE hMailslot;
    char msg[81];
    // server thread
    hMailslot = CreateMailslot(
                    g_szMailslot,           // mailslot name
                    MAX_PATH + 2,				// input buffer size
                    MAILSLOT_WAIT_FOREVER,  // no timeout
                    NULL);                  // default security attribute
    if (INVALID_HANDLE_VALUE == hMailslot) {
        sprintf_s(msg, 80, "Error %d occurred while creating the mailslot", GetLastError());
        OutTrace("%s\n", msg);
        MessageBox(0, msg, "Error", 0);
        return 1;
    }
    char szBuffer[MAX_PATH + 1];
    DWORD cbBytes;
    BOOL bResult;
    int iEntry;
    while(1) { //Infinite, till user terminates the console app
        //Read client message
        bResult = ReadFile(
                      hMailslot,            // handle to mailslot
                      szBuffer,             // buffer to receive data
                      sizeof(szBuffer),     // size of buffer
                      &cbBytes,             // number of bytes read
                      NULL);                // not overlapped I/O
        if ((!bResult) || (0 == cbBytes)) {
            sprintf_s(msg, 80, "Error %d occurred while reading from the mailslot", GetLastError());
            MessageBox(0, msg, "Error", 0);
            OutTrace("%s\n", msg);
            CloseHandle(hMailslot);
            return 1;  //Error
        }
        switch (toupper(szBuffer[0])) {
        case 'L':
            // syntax: L:<fullpath>
            OutTrace("C/S:Import \"%s\"\n", &szBuffer[2]);
            lpView->Import(&szBuffer[2]);
            break;
        case 'Z':
            OutTrace("C/S:Zap configuration\n");
            lpView->ZapConfigFile();
            break;
        case 'S':
            OutTrace("C/S:Save configuration\n");
            lpView->SaveConfigFile();
            break;
        case 'Y':
            // syntax: Y:<fullpath>
            OutTrace("C/S:Yeld \"%s\"\n", &szBuffer[2]);
            if(szBuffer[2] == '#')
                iEntry = atoi(&szBuffer[3]);
            else {
                lpView->Import(&szBuffer[2]);
                iEntry = lpView->GetNumTargets();
                OutTrace("C/S:Running entry %d\n", iEntry);
            }
            lpView->OnRun(RUN_DEFAULT, iEntry - 1);
            break;
        }
    }
    CloseHandle(hMailslot);
    return 0; //Success
}

void CDxwndhostView::OnInitialUpdate() {
    CListView::OnInitialUpdate();
    CListCtrl &listctrl = GetListCtrl();
    LV_COLUMN listcol;
    LV_ITEM listitem;
    int i;
    DXWNDSTATUS StatusMap;
    typedef BOOL (WINAPI * ChangeWindowMessageFilter_Type)(UINT, DWORD);
    ChangeWindowMessageFilter_Type pChangeWindowMessageFilter;
    //bIconViewMode = FALSE;
    bIconViewMode = GetPrivateProfileInt("window", "iconview", 1, gInitPath);
    DragAcceptFiles();
    // the ChangeWindowMessageFilter is not available, nor necessary, on XP
    HMODULE hUser32;
    hUser32 = GetModuleHandle("user32.dll");
    pChangeWindowMessageFilter = (ChangeWindowMessageFilter_Type)GetProcAddress(hUser32, "ChangeWindowMessageFilter");
    if(pChangeWindowMessageFilter) {
        (*pChangeWindowMessageFilter)(WM_DROPFILES, MSGFLT_ADD);
        (*pChangeWindowMessageFilter)(WM_COPYDATA, MSGFLT_ADD);
        (*pChangeWindowMessageFilter)(0x0049, MSGFLT_ADD);
    }
    // Create RGB24 color image lists
    HIMAGELIST hList = ImageList_Create(32, 32, ILC_COLOR24 | ILC_MASK, 6, 1);
    m_cImageListNormal.Attach(hList);
    hList = ImageList_Create(16, 16, ILC_COLOR24 | ILC_MASK, 6, 1);
    m_cImageListSmall.Attach(hList);
    hList = ImageList_Create(32, 32, ILC_COLOR24 | ILC_MASK, 0, 1);
    m_cIconList.Attach(hList);
    // Load the large icons (unused)
    CBitmap cBmp;
    cBmp.LoadBitmap(IDB_BIGICONS);
    m_cImageListNormal.Add(&cBmp, RGB(255, 0, 255));
    cBmp.DeleteObject();
    // Load the small icons
    cBmp.LoadBitmap(IDB_SMALLICONS);
    m_cImageListSmall.Add(&cBmp, RGB(255, 0, 255));
    // Attach them
    listctrl.SetImageList(&m_cImageListNormal, LVSIL_NORMAL);
    listctrl.SetImageList(&m_cImageListSmall, LVSIL_SMALL);
    listcol.mask = LVCF_WIDTH;
    listcol.cx = 100;
    listctrl.InsertColumn(0, &listcol);
    listctrl.InsertColumn(0, &listcol);
    memset(&StatusMap, 0, sizeof(StatusMap)); // initialize
    StatusMap.VJoyStatus = GetPrivateProfileInt("joystick", "flags", VJOYENABLED | CROSSENABLED | INVERTYAXIS | VJMOUSEMAP | VJKEYBOARDMAP, gInitPath);
    StatusMap.VJoySensivity = GetPrivateProfileInt("joystick", "sensivity", 100, gInitPath);
    StatusMap.VJoyDeadZone = GetPrivateProfileInt("joystick", "deadzone", 5, gInitPath); // default 5%
    StatusMap.LockOnMidBtn = GetPrivateProfileInt("joystick", "lock", 1, gInitPath); // default TRUE
    OutTrace("joystick initial deadzone = %d%%\n", StatusMap.VJoyDeadZone);
    // initialize defined entries
    for(i = 0; i < MAXTARGETS; i ++) {
        if (!LoadConfigItem(&TargetMaps[i], &PrivateMaps[i], i, gInitPath)) break;
        AppendItemToList(&TargetMaps[i], &PrivateMaps[i], i, &listitem);
        listctrl.InsertItem(&listitem);
    }
    listctrl.SetImageList(&m_cIconList, LVSIL_NORMAL);
    listctrl.ModifyStyle(LVS_ICON | LVS_LIST | LVS_REPORT | LVS_SMALLICON, 0);
    listctrl.ModifyStyle(0, bIconViewMode ? LVS_ICON : LVS_REPORT);
    // clear empty ones
    for(; i < MAXTARGETS; i ++) {
        TargetMaps[i].path[0] = 0;
        PrivateMaps[i].title[0] = 0;
        PrivateMaps[i].notes = NULL;
        PrivateMaps[i].registry = NULL;
    }
    Resize();
    SetTarget(&StatusMap, TargetMaps);
    if(m_InitialState == DXW_ACTIVE)
        this->OnHookStart();
    else
        this->OnHookStop();
    if(m_StartToTray) this->OnGoToTrayIcon();
    this->isUpdated = FALSE;
    this->isRegistryUpdated = FALSE;
    this->isDriversUpdated = FALSE;
    pTitles = &PrivateMaps[0];
    pTargets = &TargetMaps[0];
    UpdateHotKeys();
    // Transient mode
    if(gTransientMode) {
        int i = iProgIndex - 1;
        if(i < 0) i = 0;
        this->OnRun(RUN_DEFAULT, i); // v2.05.30 bug fix
    }
    ::CreateThread(NULL, 0, DxWndServer, (LPVOID)this, 0, NULL);
}

void CDxwndhostView::OnListIcons() {
    CTargetDlg dlg;
    CListCtrl &listctrl = GetListCtrl();
    listctrl.ModifyStyle(LVS_ICON | LVS_LIST | LVS_REPORT | LVS_SMALLICON, 0);
    listctrl.ModifyStyle(0, LVS_ICON);
    bIconViewMode = TRUE;
    RenewItemList(&listctrl);
    RenewIconList(&listctrl); // to update gray color setting
    WritePrivateProfileString("window", "iconview", "1", gInitPath);
}

void CDxwndhostView::OnListDetails() {
    CTargetDlg dlg;
    CListCtrl &listctrl = GetListCtrl();
    listctrl.ModifyStyle(LVS_ICON | LVS_LIST | LVS_REPORT | LVS_SMALLICON, 0);
    listctrl.ModifyStyle(0, LVS_REPORT); // LVS_REPORT is the traditional DxWnd style.
    bIconViewMode = FALSE;
    RenewItemList(&listctrl);
    Resize(); // column width may need resizing!
    WritePrivateProfileString("window", "iconview", "0", gInitPath);
}

/////////////////////////////////////////////////////////////////////////////
// CDxwndhostView Diagnostic Class

#ifdef _DEBUG
void CDxwndhostView::AssertValid() const {
    CListView::AssertValid();
}

void CDxwndhostView::Dump(CDumpContext &dc) const {
    CListView::Dump(dc);
}

CDxwndhostDoc *CDxwndhostView::GetDocument() { // Non-debug version is inline.
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDxwndhostDoc)));
    return (CDxwndhostDoc *)m_pDocument;
}
#endif //_DEBUG


void CDxwndhostView::OnDblclk(NMHDR *pNMHDR, LRESULT *pResult) {
    // TODO: Please add your control notification handler code here.
    OnRun();
    *pResult = 0;
}

//static char ImportExportPath[4096] = {0};

void CDxwndhostView::OnExport() {
    int i;
    char path[MAX_PATH];
    CListCtrl &listctrl = GetListCtrl();
    POSITION pos;
    if(!listctrl.GetSelectedCount()) return;
    pos = listctrl.GetFirstSelectedItemPosition();
    i = listctrl.GetNextSelectedItem(pos);
    GetPrivateProfileString("window", "exportpath", NULL, path, MAX_PATH, gInitPath);
    //strcat_s(path, MAX_PATH, "\\");
    strcat_s(path, MAX_PATH, PrivateMaps[i].title);
    CFileDialog dlg( FALSE, "*.dxw", path, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                     "DxWnd config (*.dxw)|*.dxw|All files (*.*)|*.*||",  this);
    dlg.m_ofn.lpstrTitle = "Save task config. as ...";
    if( dlg.DoModal() == IDOK) {
        DWORD TFlags, DFlags;
        TARGETMAP *TargetMap;
        strcpy(path, dlg.GetPathName().GetBuffer());
        // XP fix:
        if(strlen(path) > 4) {
            char *p;
            p = &path[strlen(path) - 4];
            if(strcasecmp(p, ".dxw")) strcat(path, ".dxw");
        } else
            strcat(path, ".dxw");
        //MessageBox(path, "PathName", MB_OK);
        // export with no trace flags active
        TargetMap = &TargetMaps[i];
        TFlags = TargetMap->tflags;
        DFlags = TargetMap->dflags;
        TargetMap->tflags = 0;
        TargetMap->dflags = 0;
        BOOL updated = PrivateMaps[i].updated; // save & recover ...
        SaveConfigItem(&TargetMaps[i], &PrivateMaps[i], 0, path);
        PrivateMaps[i].updated = updated;
        SaveExportStamp(path);
        TargetMap->tflags = TFlags;
        TargetMap->dflags = DFlags;
        if(GetPrivateProfileInt("window", "updatepaths", 1, gInitPath)) {
            GetFolderFromPath(path);
            WritePrivateProfileString("window", "exportpath", path, gInitPath);
        }
    }
}

BOOL CDxwndhostView::OnImport(CString sFilePath) {
    LV_ITEM listitem;
    int i;
    for (i = 0; strlen(TargetMaps[i].path) && i < MAXTARGETS; i++)
        ;
    if (i == MAXTARGETS) {
        MessageBoxLang(DXW_STRING_MAXENTRIES, DXW_STRING_WARNING, MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }
    CListCtrl &listctrl = GetListCtrl();
    if(LoadConfigItem(&TargetMaps[i], &PrivateMaps[i], 0, (char *)sFilePath.GetString())) {
        PrivateMaps[i].updated = TRUE;
        TargetMaps[i].index = i; // v2.05.40 fix - added missing initialization
        AppendItemToList(&TargetMaps[i], &PrivateMaps[i], i, &listitem);
        listctrl.InsertItem(&listitem);
    }
    Resize();
    SetTarget(NULL, TargetMaps);
    this->isUpdated = TRUE;
    this->isRegistryUpdated = TRUE;
    this->isDriversUpdated = TRUE;
    return TRUE;
}

void CDxwndhostView::OnOpenConfig() {
    //MessageBox("OpenConfig", "path", 0);
    char ConfigPath[MAX_PATH + 1] = {0};
    CFileDialog dlg( TRUE, "*.ini", ConfigPath, OFN_OVERWRITEPROMPT, NULL, this);
    //dlg.m_ofn.lpstrFilter = "Config (*.ini)\0*.ini\0All files (*.*)\0*.*\0\0";
    dlg.m_ofn.lpstrFilter = "DxWnd config (*.ini)\0*.ini\0All files (*.*)\0*.*\0\0";
    dlg.m_ofn.lpstrTitle = "Open DxWnd config. file";
    if(dlg.DoModal() != IDOK) return;
    this->OnSaveFile();
    strcpy(gInitPath, dlg.GetPathName().GetBuffer());
    // missing something here ...
    m_cImageListNormal.DeleteImageList();
    m_cImageListSmall.DeleteImageList();
    m_cIconList.DeleteImageList();
    CListCtrl &listctrl = GetListCtrl();
    listctrl.DeleteAllItems();
    Resize();
    // missing something here ...
    this->OnInitialUpdate();
    Resize();
    SetTarget(NULL, TargetMaps);
    this->isUpdated = FALSE;
    this->isRegistryUpdated = TRUE;
    this->isDriversUpdated = TRUE;
}

void CDxwndhostView::OnExploreDxWnd() {
    HINSTANCE ret;
    ret = ShellExecute(NULL, IsWinXP() ? "open" : "explore", gInitialWorkingDir, NULL, NULL, SW_SHOW);
    if((int)ret <= 32) {
        char message[MAX_PATH];
        sprintf(message, "ShellExecute(\"%s\", \"%s\") failed.\nerror=%d",
                IsWinXP() ? "open" : "explore", gInitialWorkingDir, ret);
        MessageBox(message, "error", 0);
    }
}

void CDxwndhostView::OnImport() {
    char ConfigPath[MAX_PATH + 1] = {0};
    int i;
    LV_ITEM listitem;
    CListCtrl &listctrl = GetListCtrl();
    for (i = 0; strlen(TargetMaps[i].path) && i < MAXTARGETS; i++) /* do nothing */ ;
    if (i == MAXTARGETS) {
        MessageBoxLang(DXW_STRING_MAXENTRIES, DXW_STRING_WARNING, MB_OK | MB_ICONEXCLAMATION);
        return;
    }
    CFileDialog FileDlg( TRUE, "*.dxw", ConfigPath, OFN_ALLOWMULTISELECT, NULL, this);
    FileDlg.GetOFN().lpstrFilter = "DxWnd config (*.dxw)\0*.dxw\0All files (*.*)\0*.*\0\0";
    FileDlg.GetOFN().lpstrTitle = "Import task config.";
    CString str;
    int nMaxFiles = 256;
    int nBufferSz = nMaxFiles * 256 + 1;
    FileDlg.GetOFN().lpstrFile = str.GetBuffer(nBufferSz);
    if(FileDlg.DoModal() != IDOK) return;
    POSITION pos (FileDlg.GetStartPosition());
    while(pos) {
        CString filename = FileDlg.GetNextPathName(pos);
        OutTrace("Import file \"%s\"\n", filename.GetBuffer());
        if(LoadConfigItem(&TargetMaps[i], &PrivateMaps[i], 0, filename.GetBuffer())) {
            TargetMaps[i].index = i; // v2.05.40 fix - added missing initialization
            AppendItemToList(&TargetMaps[i], &PrivateMaps[i], i, &listitem);
            listctrl.InsertItem(&listitem);
            // v2.05.06: autosave
            if(gAutoSave)
                SaveConfigItem(&TargetMaps[i], &PrivateMaps[i], i, gInitPath);
            else
                PrivateMaps[i].updated = TRUE;
            i++;
        }
    }
    if(!gAutoSave) {
        this->isUpdated = TRUE;
        this->isRegistryUpdated = TRUE;
        this->isDriversUpdated = TRUE;
    }
    Resize();
    SetTarget(NULL, TargetMaps);
}

void CDxwndhostView::OnModify() {
    int i;
    CTargetDlg dlg;
    POSITION pos;
    LV_ITEM listitem;
    CListCtrl &listctrl = GetListCtrl();
    if(!listctrl.GetSelectedCount()) return;
    pos = listctrl.GetFirstSelectedItemPosition();
    i = listctrl.GetNextSelectedItem(pos);
    SetDlgFromPrivateMap(&PrivateMaps[i], &dlg);
    SetDlgFromTarget(&TargetMaps[i], &dlg);
    // v2.04.74: item numeration
    dlg.m_ItemIndex = i + 1;
    if(dlg.DoModal() == IDOK && dlg.m_FilePath.GetLength()) {
        // check for changes to be saved to the dxwnd.ini configuration file
        if(strcmp((char *)dlg.m_Registry.GetString(), PrivateMaps[i].registry)) this->isRegistryUpdated = TRUE;
        if(strcmp((char *)dlg.m_FakeHDPath.GetString(), PrivateMaps[i].FakeHDPath)) this->isDriversUpdated = TRUE;
        if(strcmp((char *)dlg.m_FakeCDPath.GetString(), PrivateMaps[i].FakeCDPath)) this->isDriversUpdated = TRUE;
        if(strcmp((char *)dlg.m_FakeCDLabel.GetString(), PrivateMaps[i].FakeCDLabel)) this->isDriversUpdated = TRUE;
        if(dlg.m_FakeCDDrive != TargetMaps[i].FakeCDDrive) this->isDriversUpdated = TRUE;
        if(dlg.m_FakeHDDrive != TargetMaps[i].FakeHDDrive) this->isDriversUpdated = TRUE;
        // update maps
        SetPrivateMapFromDialog(&PrivateMaps[i], &dlg);
        SetTargetFromDlg(&TargetMaps[i], &dlg);
        CListCtrl &listctrl = GetListCtrl();
        listitem.mask = LVIF_TEXT | LVIF_IMAGE;
        listitem.iItem = i;
        listitem.iSubItem = 0;
        listitem.iImage = bIconViewMode ? i : SetTargetIcon(TargetMaps[i]);
        listitem.pszText = PrivateMaps[i].title;
        listctrl.SetItem(&listitem);
        // v2.05.05: autosave
        if(gAutoSave)
            SaveConfigItem(&TargetMaps[i], &PrivateMaps[i], i, gInitPath);
        else
            this->isUpdated = TRUE;
        Resize();
        if(gGrayIcons) RenewIconList(&listctrl);
        SetTarget(NULL, TargetMaps);
    }
}

void CDxwndhostView::OnExplore() {
    int i;
    CTargetDlg dlg;
    POSITION pos;
    int len;
    CString	FilePath;
    HINSTANCE ret;
    CListCtrl &listctrl = GetListCtrl();
    if(!listctrl.GetSelectedCount()) return;
    pos = listctrl.GetFirstSelectedItemPosition();
    i = listctrl.GetNextSelectedItem(pos);
    FilePath = TargetMaps[i].path;
    len = FilePath.ReverseFind('\\');
    if (len == 0) return;
    FilePath.Truncate(len);
    //MessageBox(FilePath, "path", 0);
    ret = ShellExecute(NULL, IsWinXP() ? "open" : "explore", FilePath, NULL, NULL, SW_SHOW);
    if((int)ret <= 32) {
        char message[MAX_PATH];
        if((int)ret == 2) {
            // common case
            sprintf(message, "folder \"%s\" does not exist", FilePath);
        } else {
            sprintf(message, "ShellExecute(\"%s\", \"%s\") failed.\nerror=%d",
                    IsWinXP() ? "open" : "explore", FilePath, ret);
        }
        MessageBox(message, "error", 0);
    }
}

void CDxwndhostView::OnViewLog() {
    int i;
    CTargetDlg dlg;
    POSITION pos;
    int len;
    CString	FilePath;
    CListCtrl &listctrl = GetListCtrl();
    if(!listctrl.GetSelectedCount()) return;
    pos = listctrl.GetFirstSelectedItemPosition();
    i = listctrl.GetNextSelectedItem(pos);
    if(PrivateMaps[i].startfolder[0]) {
        FilePath = PrivateMaps[i].startfolder;
        FilePath.Append("\\dxwnd.log");
    } else {
        FilePath = TargetMaps[i].path;
        len = FilePath.ReverseFind('\\');
        if (len == 0) return;
        FilePath.Truncate(len);
        FilePath.Append("\\dxwnd.log");
    }
    ShellExecute(NULL, "open", FilePath, NULL, NULL, SW_SHOW);
}

void CDxwndhostView::OnViewLogTail() {
    int i;
    CTargetDlg dlg;
    POSITION pos;
    int len;
    CString	FilePath;
    CString	CmdLine;
    CListCtrl &listctrl = GetListCtrl();
    if(!listctrl.GetSelectedCount()) return;
    pos = listctrl.GetFirstSelectedItemPosition();
    i = listctrl.GetNextSelectedItem(pos);
    if(PrivateMaps[i].startfolder[0]) {
        FilePath = PrivateMaps[i].startfolder;
        FilePath.Append("\\dxwnd.log");
    } else {
        FilePath = TargetMaps[i].path;
        len = FilePath.ReverseFind('\\');
        if (len == 0) return;
        FilePath.Truncate(len);
        FilePath.Append("\\dxwnd.log");
    }
    ShellExecute(NULL, "open", "tail", "-98 /f /w \"" + FilePath + "\"", NULL, SW_SHOW);
}

void CDxwndhostView::OnViewGUILog() {
    ShellExecute(NULL, "open", ".\\dxwnd.gui.log", NULL, NULL, SW_SHOW);
}

void CDxwndhostView::OnDeleteLog() {
    int i;
    CTargetDlg dlg;
    POSITION pos;
    int len;
    CString	FilePath;
    HRESULT res;
    FILE *logfp;
    CListCtrl &listctrl = GetListCtrl();
    if(!listctrl.GetSelectedCount()) return;
    pos = listctrl.GetFirstSelectedItemPosition();
    i = listctrl.GetNextSelectedItem(pos);
    FilePath = TargetMaps[i].path;
    len = FilePath.ReverseFind('\\');
    if (len == 0) return;
    FilePath.Truncate(len);
    FilePath.Append("\\dxwnd.log");
    if((logfp = fopen(FilePath, "rb")) != NULL) { // if the file exists ....
        fclose(logfp);
        // ... ask confirmation.
        res = MessageBoxLang(DXW_STRING_CLEARLOG, DXW_STRING_WARNING, MB_YESNO | MB_ICONQUESTION);
        if(res != IDYES) return;
    }
    _unlink(FilePath);
}

void CDxwndhostView::OnDebugView() {
    PROCESS_INFORMATION pinfo;
    STARTUPINFO sinfo;
    char exepath[MAX_PATH + 1];
    char folderpath[MAX_PATH + 1];
    ZeroMemory(&sinfo, sizeof(sinfo));
    sinfo.cb = sizeof(sinfo);
    GetPrivateProfileString("window", "debugview", "DbgView.exe", exepath, MAX_PATH, gInitPath);
    strcpy_s(folderpath, sizeof(folderpath), exepath);
    PathRemoveFileSpec(folderpath);
    if(strlen(folderpath) == 0) strcpy(folderpath, ".\\");
    if(!CreateProcessA(NULL, exepath, 0, 0, false, CREATE_DEFAULT_ERROR_MODE, NULL, folderpath, &sinfo, &pinfo)) {
        char sInfo[81];
        sprintf(sInfo, "Error %d starting DebugView", GetLastError());
        MessageBox(sInfo, "Error", MB_ICONERROR | MB_OK);
        return;
    }
    CloseHandle(pinfo.hProcess);
    CloseHandle(pinfo.hThread);
}

void CDxwndhostView::OnSetPath() {
    int i;
    CTargetDlg dlg;
    POSITION pos;
    char *lpProcName, *lpNext;
    extern BOOL GetProcByName(char *, char *);
    char FullPath[MAX_PATH + 1];
    char sMessage[1000];
    CListCtrl &listctrl = GetListCtrl();
    if(!listctrl.GetSelectedCount()) return;
    pos = listctrl.GetFirstSelectedItemPosition();
    i = listctrl.GetNextSelectedItem(pos);
    //if(TargetMaps[i].path[0] != '*') return;
    lpProcName = &(TargetMaps[i].path[1]);
    while (lpNext = strchr(lpProcName, '\\')) lpProcName = lpNext + 1;
    strcpy(FullPath, "");
    if(GetProcByName(lpProcName, FullPath) == 0) {
        int res;
        if(strcmp(TargetMaps[i].path, FullPath)) {
            sprintf(sMessage, "Found replacement for \"%s\", "
                    "full path = \"%s\", "
                    "do you want to update the configuration?", lpProcName, FullPath);
            res = MessageBox(sMessage, "DxWnd set path", MB_YESNO);
            if(res != IDYES) return;
            strcpy(TargetMaps[i].path, FullPath);
            this->isUpdated = TRUE;
        } else {
            sprintf(sMessage, "Path \"%s\" was already set.", FullPath);
            MessageBox(sMessage, "DxWnd set path", MB_OK);
        }
    } else {
        sprintf(sMessage, "Replacement for \"%s\" not found, run the game!", lpProcName);
        MessageBox(sMessage, "DxWnd set path", MB_ICONEXCLAMATION);
    }
}

void CDxwndhostView::OnSetRegistry() {
    int i;
    CTargetDlg dlg;
    POSITION pos;
    CListCtrl &listctrl = GetListCtrl();
    if(!listctrl.GetSelectedCount()) return;
    pos = listctrl.GetFirstSelectedItemPosition();
    i = listctrl.GetNextSelectedItem(pos);
    FlushRegistryFile(PrivateMaps[i].registry);
}

BOOL PauseResumeThreadList(DWORD dwOwnerPID, bool bResumeThread) {
    HANDLE        hThreadSnap = NULL;
    BOOL          bRet        = FALSE;
    THREADENTRY32 te32        = {0};
    // Take a snapshot of all threads currently in the system.
    hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hThreadSnap == INVALID_HANDLE_VALUE)
        return (FALSE);
    // Fill in the size of the structure before using it.
    te32.dwSize = sizeof(THREADENTRY32);
    // Walk the thread snapshot to find all threads of the process.
    // If the thread belongs to the process, add its information
    // to the display list.
    if (Thread32First(hThreadSnap, &te32)) {
        do {
            if (te32.th32OwnerProcessID == dwOwnerPID) {
                HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te32.th32ThreadID);
                if (bResumeThread)
                    ResumeThread(hThread);
                else
                    SuspendThread(hThread);
                CloseHandle(hThread);
            }
        } while (Thread32Next(hThreadSnap, &te32));
        bRet = TRUE;
    } else
        bRet = FALSE;          // could not walk the list of threads
    // Do not forget to clean up the snapshot object.
    CloseHandle (hThreadSnap);
    return (bRet);
}

void CDxwndhostView::OnPause() {
    CTargetDlg dlg;
    HRESULT res;
    DXWNDSTATUS DxWndStatus;
    if ((GetHookStatus(&DxWndStatus) != DXW_RUNNING) /*|| (DxWndStatus.hWnd==NULL)*/ )
        MessageBoxLang(DXW_STRING_NOPAUSETASK, DXW_STRING_INFO, MB_ICONEXCLAMATION);
    else {
        wchar_t *wcstring = new wchar_t[48 + 1];
        mbstowcs_s(NULL, wcstring, 48, PrivateMaps[DxWndStatus.OrigIdx].title, _TRUNCATE);
        res = MessageBoxLangArg(DXW_STRING_PAUSETASK, DXW_STRING_INFO, MB_YESNO | MB_ICONQUESTION, wcstring);
        if(res != IDYES) return;
        PauseResumeThreadList(DxWndStatus.dwPid, FALSE);
    }
}

void CDxwndhostView::OnResume() {
    CTargetDlg dlg;
    HRESULT res;
    DXWNDSTATUS DxWndStatus;
    if ((GetHookStatus(&DxWndStatus) != DXW_RUNNING) /*|| (DxWndStatus.hWnd==NULL)*/)
        MessageBoxLang(DXW_STRING_NORESUMETASK, DXW_STRING_INFO, MB_ICONEXCLAMATION);
    else {
        wchar_t *wcstring = new wchar_t[48 + 1];
        mbstowcs_s(NULL, wcstring, 48, PrivateMaps[DxWndStatus.OrigIdx].title, _TRUNCATE);
        res = MessageBoxLangArg(DXW_STRING_RESUMETASK, DXW_STRING_WARNING, MB_YESNO | MB_ICONQUESTION, wcstring);
        if(res != IDYES) return;
        PauseResumeThreadList(DxWndStatus.dwPid, TRUE);
    }
}

extern HWND find_main_window(unsigned long);

static void SendMessageToHookedWin(DWORD message) {
    DXWNDSTATUS DxWndStatus;
    HWND TargethWnd;
    if (GetHookStatus(&DxWndStatus) == DXW_RUNNING) {
        TargethWnd = (DxWndStatus.hWnd) ? DxWndStatus.hWnd : find_main_window(DxWndStatus.dwPid);
        ::PostMessage(TargethWnd, WM_SYSCOMMAND, message, 0);
    }
}

void CDxwndhostView::OnEmergencyKill() {
    // v2.05.43: stronger emergency policy
    // kill currently hooked process without asking for confirmation
    OnKill(FALSE); // FASE: do not ask confirmation
    Sleep(500);
    // just in case, kill also the process configured in the selected DxWnd entry
    OnProcessKill(TRUE); // TRUE: kill all instances
}

void CDxwndhostView::OnWindowMinimize() {
    SendMessageToHookedWin(SC_MINIMIZE);
}

void CDxwndhostView::OnWindowRestore() {
    SendMessageToHookedWin(SC_RESTORE);
}

void CDxwndhostView::OnWindowClose() {
    SendMessageToHookedWin(SC_CLOSE);
}

void CDxwndhostView::OnTaskbarHide() {
    gShowHideTaskBar(TRUE);
}

void CDxwndhostView::OnTaskbarShow() {
    gShowHideTaskBar(FALSE);
}

void CDxwndhostView::OnKill() {
    OnKill(TRUE);
}

void CDxwndhostView::OnKill(BOOL bConfirm) {
    CTargetDlg dlg;
    HRESULT res;
    HANDLE TargetHandle;
    DXWNDSTATUS DxWndStatus;
    if ((GetHookStatus(&DxWndStatus) != DXW_RUNNING) /*|| (DxWndStatus.hWnd==NULL)*/)
        MessageBoxLang(DXW_STRING_NOKILLTASK, DXW_STRING_INFO, MB_ICONEXCLAMATION);
    else {
        if(bConfirm) {
            wchar_t *wcstring = new wchar_t[48 + 1];
            mbstowcs_s(NULL, wcstring, 48, PrivateMaps[DxWndStatus.OrigIdx].title, _TRUNCATE);
            res = MessageBoxLangArg(DXW_STRING_KILLTASK, DXW_STRING_WARNING,
                                    MB_YESNO | MB_ICONQUESTION | MB_SYSTEMMODAL | MB_TOPMOST, wcstring);
            if(res != IDYES) return;
        }
        DxWndStatus.dwPid;
        TargetHandle = OpenProcess(PROCESS_TERMINATE, FALSE, DxWndStatus.dwPid);
        if(!TargetHandle) {
            MessageBoxLangArg(DXW_STRING_OPENPROCESS, DXW_STRING_ERROR, MB_ICONEXCLAMATION, DxWndStatus.dwPid, GetLastError());
            return;
        }
        if(!TerminateProcess(TargetHandle, 0)) {
            MessageBoxLangArg(DXW_STRING_TERMINATE, DXW_STRING_ERROR, MB_ICONEXCLAMATION, TargetHandle, GetLastError());
            return;
        }
        CloseHandle(TargetHandle);
        ClipCursor(NULL);
    }
    RevertScreenChanges(&this->InitDevMode);
}

// void CDxwndhostView::OnProcessKill(BOOL bAll): kills one instance (bAll==FALSE) or all instances (bAll==TRUE)
// of the process whose name corresponds to the selected program's list entry.
void CDxwndhostView::OnProcessKill(BOOL bAll) {
    int i;
    POSITION pos;
    CListCtrl &listctrl = GetListCtrl();
    char FilePath[MAX_PATH + 1];
    char *lpProcName, *lpNext;
    HRESULT res;
    if(!listctrl.GetSelectedCount()) return ;
    pos = listctrl.GetFirstSelectedItemPosition();
    i = listctrl.GetNextSelectedItem(pos);
    strnncpy(FilePath, TargetMaps[i].path, MAX_PATH);
    lpProcName = FilePath;
    while (lpNext = strchr(lpProcName, '\\')) lpProcName = lpNext + 1;
    if(TargetMaps[i].flags7 & COPYNOSHIMS)
        strcat(lpProcName, ".noshim");
    if(!KillProcByName(lpProcName, FALSE, FALSE)) {
        wchar_t *wcstring = new wchar_t[48 + 1];
        mbstowcs_s(NULL, wcstring, 48, PrivateMaps[i].title, _TRUNCATE);
        res = MessageBoxLangArg(DXW_STRING_KILLTASK, DXW_STRING_WARNING,
                                MB_YESNO | MB_ICONQUESTION | MB_SYSTEMMODAL | MB_TOPMOST, wcstring);
        if(res != IDYES) return;
        KillProcByName(lpProcName, TRUE, bAll);
    } else
        MessageBoxLang(DXW_STRING_NOKILLTASK, DXW_STRING_INFO, MB_ICONEXCLAMATION);
    ClipCursor(NULL);
    RevertScreenChanges(&this->InitDevMode);
}

void CDxwndhostView::OnProcessKill() {
    OnProcessKill(FALSE);
}

void CDxwndhostView::OnProcessKillAll() {
    OnProcessKill(TRUE);
}

void CDxwndhostView::OnAdd() {
    OnAdd(NULL);
}

void CDxwndhostView::OnAdd(char *sInitialPath) {
    // TODO: Please add a command handler code here.
    int i;
    CTargetDlg *dlg = new(CTargetDlg);
    LV_ITEM listitem;
    TARGETMAP *tm;
    PRIVATEMAP *pm;
    for(i = 0; i < MAXTARGETS; i ++) {
        tm = &TargetMaps[i];
        if(!tm->path[0]) break;
    }
    if(i >= MAXTARGETS) {
        MessageBoxLang(DXW_STRING_MAXENTRIES, DXW_STRING_WARNING, MB_OK | MB_ICONEXCLAMATION);
        return;
    }
    pm = &PrivateMaps[i];
    memset((LPVOID)tm, 0, sizeof(TARGETMAP)); // clean up, just in case....
    if(sInitialPath) dlg->m_FilePath = CString(sInitialPath);
    dlg->m_Coordinates = GetPrivateProfileInt("window", "defaultcoord", 0, gInitPath);
    dlg->m_PosX = GetPrivateProfileInt("window", "defaultposx", 50, gInitPath);
    dlg->m_PosY = GetPrivateProfileInt("window", "defaultposy", 50, gInitPath);
    dlg->m_SizX = GetPrivateProfileInt("window", "defaultsizx", 800, gInitPath);
    dlg->m_SizY = GetPrivateProfileInt("window", "defaultsizy", 600, gInitPath);
    dlg->m_LogMode = 0; // ???
    if(dlg->DoModal() == IDOK && dlg->m_FilePath.GetLength()) {
        SetPrivateMapFromDialog(pm, dlg);
        SetTargetFromDlg(tm, dlg);
        tm->index = i; // v2.05.40 fix - added missing initialization
        CListCtrl &listctrl = GetListCtrl();
        // if left empty, build a default entry title
        if (strlen(pm->title) == 0) {
            int len;
            CString	FilePath;
            //FilePath=tm->path;
            // don't build default entry title from short paths!
            FilePath = dlg->m_FilePath;
            len = FilePath.ReverseFind('\\');
            if(GetPrivateProfileInt("window", "namefromfolder", 0, gInitPath)) {
                FilePath = FilePath.Left(len);
                len = FilePath.ReverseFind('\\');
            }
            FilePath = FilePath.Right(FilePath.GetLength() - len - 1);
            strncpy_s(pm->title, sizeof(pm->title), FilePath.GetString(), sizeof(pm->title) - 1);
            // eliminate .exe suffix
            len = strlen(pm->title);
            if(len > 4) {
                char *p = &pm->title[len - 4];
                if(!strcmp(p, ".exe") || !strcmp(p, ".EXE")) *p = 0;
            }
        }
#ifdef REBUILDLIST
        listctrl.DeleteAllItems();
        m_cIconList.DeleteImageList();
        HIMAGELIST hList = ImageList_Create(32, 32, ILC_COLOR24 | ILC_MASK, 0, 1);
        m_cIconList.Attach(hList);
        CBitmap *bmap = new(CBitmap);
        bmap->LoadBitmapA(IDB_PROGRAM);
        for(int j = 0; j < i; j ++) {
            AppendItemToList(&TargetMaps[j], &PrivateMaps[j], j, &listitem);
            listctrl.InsertItem(&listitem);
        }
#endif
        AppendItemToList(tm, pm, i, &listitem);
        pm->updated = TRUE;
        listctrl.InsertItem(&listitem);
        // v2.05.05: autosave
        if(gAutoSave)
            SaveConfigItem(tm, pm, i, gInitPath);
        else {
            this->isUpdated = TRUE;
            this->isRegistryUpdated = TRUE;
            this->isDriversUpdated = TRUE;
        }
        Resize();
        SetTarget(NULL, TargetMaps);
    }
}

void CDxwndhostView::OnDuplicate() {
    POSITION pos;
    int i;
    CListCtrl &listctrl = GetListCtrl();
    if(!(pos = listctrl.GetFirstSelectedItemPosition())) return;
    i = listctrl.GetNextSelectedItem(pos);
    if (listctrl.GetItemCount() >= MAXTARGETS - 1) return; // too many entries to duplicate one!
    for(int j = listctrl.GetItemCount(); j > i; j--) {
        TargetMaps[j + 1] = TargetMaps[j];
        PrivateMaps[j + 1] = PrivateMaps[j];
        PrivateMaps[j + 1].updated = TRUE;
    }
    TargetMaps[i + 1] = TargetMaps[i];
    PrivateMaps[i + 1] = PrivateMaps[i];
    PrivateMaps[i + 1].updated = TRUE;
    // duplicate dynamic areas
    int len;
    if(PrivateMaps[i].notes) {
        len = strlen(PrivateMaps[i].notes);
        PrivateMaps[i + 1].notes = (char *)malloc(len + 1);
        strcpy(PrivateMaps[i + 1].notes, PrivateMaps[i].notes);
    }
    if(PrivateMaps[i].registry) {
        len = strlen(PrivateMaps[i].registry);
        PrivateMaps[i + 1].registry = (char *)malloc(len + 1);
        strcpy(PrivateMaps[i + 1].registry, PrivateMaps[i].registry);
    }
    // v2.05.05: autosave
    if(gAutoSave) {
        // since all entries get shifted up 1 position, it is necessary to
        // save up to the end of the list.
        for(int j = i + 1; j < listctrl.GetItemCount() + 1; j++)
            SaveConfigItem(&TargetMaps[j], &PrivateMaps[j], j, gInitPath);
    } else {
        this->isUpdated = TRUE;
        this->isRegistryUpdated = TRUE;
        this->isDriversUpdated = TRUE;
    }
    // renew Item & Icon lists
    RenewItemList(&listctrl);
    RenewIconList(&listctrl);
    Resize();
    SetTarget(NULL, TargetMaps);
}

void CDxwndhostView::OnMoveTop() {
    TARGETMAP MapEntry;
    PRIVATEMAP TitEntry;
    POSITION pos;
    int i;
    CListCtrl &listctrl = GetListCtrl();
    if(!(pos = listctrl.GetFirstSelectedItemPosition())) return;
    i = listctrl.GetNextSelectedItem(pos);
    if (i == 0)return;
    MapEntry = TargetMaps[i];
    TitEntry = PrivateMaps[i];
    for(int j = i; j; j--) {
        TargetMaps[j] = TargetMaps[j - 1];
        PrivateMaps[j] = PrivateMaps[j - 1];
        PrivateMaps[j].updated = TRUE;
    }
    TargetMaps[0] = MapEntry;
    PrivateMaps[0] = TitEntry;
    PrivateMaps[0].updated = TRUE;
    // renew Item & Icon lists
    RenewItemList(&listctrl);
    RenewIconList(&listctrl);
    // v2.05.05: autosave
    if(gAutoSave) {
        for (int k = 0; k <= i; k++)
            SaveConfigItem(&TargetMaps[k], &PrivateMaps[k], k, gInitPath);
    } else {
        this->isUpdated = TRUE;
        this->isRegistryUpdated = TRUE;
        this->isDriversUpdated = TRUE;
    }
    SetTarget(NULL, TargetMaps);
}

void CDxwndhostView::OnMoveUp() {
    TARGETMAP MapEntry;
    PRIVATEMAP TitEntry;
    POSITION pos;
    int i;
    CListCtrl &listctrl = GetListCtrl();
    if(!(pos = listctrl.GetFirstSelectedItemPosition())) return;
    i = listctrl.GetNextSelectedItem(pos);
    if (i == 0)return;
    MapEntry = TargetMaps[i - 1];
    TargetMaps[i - 1] = TargetMaps[i];
    TargetMaps[i] = MapEntry;
    TitEntry = PrivateMaps[i - 1];
    PrivateMaps[i - 1] = PrivateMaps[i];
    PrivateMaps[i] = TitEntry;
    PrivateMaps[i].updated = TRUE;
    PrivateMaps[i - 1].updated = TRUE;
    // renew Item & Icon lists
    RenewItemList(&listctrl);
    RenewIconList(&listctrl);
    // v2.05.05: autosave
    if(gAutoSave) {
        SaveConfigItem(&TargetMaps[i], &PrivateMaps[i], i, gInitPath);
        SaveConfigItem(&TargetMaps[i - 1], &PrivateMaps[i - 1], i - 1, gInitPath);
    } else {
        this->isUpdated = TRUE;
        this->isRegistryUpdated = TRUE;
        this->isDriversUpdated = TRUE;
    }
    SetTarget(NULL, TargetMaps);
}

void CDxwndhostView::OnMoveDown() {
    TARGETMAP MapEntry;
    PRIVATEMAP TitEntry;
    POSITION pos;
    int i;
    CListCtrl &listctrl = GetListCtrl();
    if(!(pos = listctrl.GetFirstSelectedItemPosition())) return;
    i = listctrl.GetNextSelectedItem(pos);
    if (i == listctrl.GetItemCount() - 1)return;
    MapEntry = TargetMaps[i + 1];
    TargetMaps[i + 1] = TargetMaps[i];
    TargetMaps[i] = MapEntry;
    TitEntry = PrivateMaps[i + 1];
    PrivateMaps[i + 1] = PrivateMaps[i];
    PrivateMaps[i] = TitEntry;
    PrivateMaps[i].updated = TRUE;
    PrivateMaps[i + 1].updated = TRUE;
    // renew Item & Icon lists
    RenewItemList(&listctrl);
    RenewIconList(&listctrl);
    // v2.05.05: autosave
    if(gAutoSave) {
        SaveConfigItem(&TargetMaps[i], &PrivateMaps[i], i, gInitPath);
        SaveConfigItem(&TargetMaps[i + 1], &PrivateMaps[i + 1], i + 1, gInitPath);
    } else {
        this->isUpdated = TRUE;
        this->isRegistryUpdated = TRUE;
        this->isDriversUpdated = TRUE;
    }
    SetTarget(NULL, TargetMaps);
}

void CDxwndhostView::OnMoveBottom() {
    TARGETMAP MapEntry;
    PRIVATEMAP TitEntry;
    POSITION pos;
    int i, last;
    CListCtrl &listctrl = GetListCtrl();
    if(!(pos = listctrl.GetFirstSelectedItemPosition())) return;
    i = listctrl.GetNextSelectedItem(pos);
    last = listctrl.GetItemCount() - 1;
    if (i == last)return;
    MapEntry = TargetMaps[i];
    TitEntry = PrivateMaps[i];
    for(int j = i; j < last; j++) {
        TargetMaps[j] = TargetMaps[j + 1];
        PrivateMaps[j] = PrivateMaps[j + 1];
        PrivateMaps[j].updated = TRUE;
    }
    TargetMaps[last] = MapEntry;
    PrivateMaps[last] = TitEntry;
    PrivateMaps[last].updated = TRUE;
    // renew Item & Icon lists
    RenewItemList(&listctrl);
    RenewIconList(&listctrl);
    // v2.05.05: autosave
    if(gAutoSave) {
        for (int k = i; k <= last; k++)
            SaveConfigItem(&TargetMaps[k], &PrivateMaps[k], k, gInitPath);
    } else {
        this->isUpdated = TRUE;
        this->isRegistryUpdated = TRUE;
        this->isDriversUpdated = TRUE;
    }
    SetTarget(NULL, TargetMaps);
}

void CDxwndhostView::OnDelete() {
    int i, index, last, len;
    POSITION pos;
    CListCtrl &listctrl = GetListCtrl();
    CString	FilePath;
    HRESULT res;
    if(!listctrl.GetSelectedCount()) return ;
    pos = listctrl.GetFirstSelectedItemPosition();
    i = index = listctrl.GetNextSelectedItem(pos);
    FilePath = PrivateMaps[i].title;
    if (FilePath.GetLength() == 0) {
        FilePath = TargetMaps[i].path;
        len = FilePath.ReverseFind('\\');
        if (len == 0) return;
        FilePath = FilePath.Right(FilePath.GetLength() - len - 1);
    }
    wchar_t *wcstring = new wchar_t[MAX_PATH + 1];
    mbstowcs_s(NULL, wcstring, MAX_PATH, FilePath, _TRUNCATE);
    res = MessageBoxLangArg(DXW_STRING_DELENTRY, DXW_STRING_WARNING, MB_YESNO | MB_ICONQUESTION, wcstring);
    if(res != IDYES) return;
    listctrl.DeleteItem(i);
    m_cIconList.Remove(i);
    // clear dynamic areas
    if(PrivateMaps[i].notes) {
        free(PrivateMaps[i].notes);
        PrivateMaps[i].notes = NULL;
    }
    if(PrivateMaps[i].registry) {
        free(PrivateMaps[i].registry);
        PrivateMaps[i].registry = NULL;
    }
    // move the rest of the items
    for(; i < MAXTARGETS  - 1; i ++) {
        PrivateMaps[i] = PrivateMaps[i + 1]; // V2.1.74 fix
        PrivateMaps[i].updated = TRUE;
        TargetMaps[i] = TargetMaps[i + 1];
        if(TargetMaps[i].path[0] == 0) break;
    }
    last = i;
    TargetMaps[i].path[0] = 0; // clear last one, in case there were MAXTARGETS entries
    RenewItemList(&listctrl);
    // v2.05.05: autosave
    if(gAutoSave) {
        for (int k = index; k < last; k++)
            SaveConfigItem(&TargetMaps[k], &PrivateMaps[k], k, gInitPath);
        ClearTarget(last, gInitPath);
    } else {
        this->isUpdated = TRUE;
        this->isRegistryUpdated = TRUE;
        this->isDriversUpdated = TRUE;
    }
    SetTarget(NULL, TargetMaps);
}

void CDxwndhostView::OnSort() {
    int i, itemcount;
    TARGETMAP MapEntry;
    PRIVATEMAP TitEntry;
    BOOL swapped;
    HRESULT res;
    CListCtrl &listctrl = GetListCtrl();
    // find list entries number and ignore the command when less than 2
    for(i = 0; i < MAXTARGETS; i++) if(strlen(TargetMaps[i].path) == 0) break;
    itemcount = i;
    if (itemcount < 2) return;
    // ask for confirmation
    res = MessageBoxLang(DXW_STRING_SORTLIST, DXW_STRING_WARNING, MB_YESNO | MB_ICONQUESTION);
    if(res != IDYES) return;
    // bubble sort the list
    swapped = 1;
    while(swapped) {
        swapped = 0;
        for(i = 0; i < itemcount - 1; i++) {
            if(strcasecmp(PrivateMaps[i].title, PrivateMaps[i + 1].title) > 0) {
                // swap entries
                MapEntry = TargetMaps[i];
                TargetMaps[i] = TargetMaps[i + 1];
                TargetMaps[i + 1] = MapEntry;
                TitEntry = PrivateMaps[i];
                PrivateMaps[i] = PrivateMaps[i + 1];
                PrivateMaps[i + 1] = TitEntry;
                PrivateMaps[i].updated = TRUE;
                PrivateMaps[i + 1].updated = TRUE;
                swapped = 1;
            }
        }
    }
    // renew Item & Icon lists
    RenewItemList(&listctrl);
    RenewIconList(&listctrl);
    // v2.05.05: autosave
    if(gAutoSave) {
        for(i = 0; i < itemcount; i++) {
            if(PrivateMaps[i].updated)
                SaveConfigItem(&TargetMaps[i], &PrivateMaps[i], i, gInitPath);
        }
    } else {
        this->isUpdated = TRUE;
        this->isRegistryUpdated = TRUE;
        this->isDriversUpdated = TRUE;
    }
    SetTarget(NULL, TargetMaps);
}

int CDxwndhostView::GetNumTargets(void) {
    for(int i = 0; i < MAXTARGETS; i++) if(strlen(TargetMaps[i].path) == 0) return i;
    return MAXTARGETS;
}

void CDxwndhostView::OnHookStart() {
    CMenu *menu;
    HICON hIcon, hPrevIcon;
    menu = this->GetParent()->GetMenu();
    menu->CheckMenuItem(ID_HOOK_START, MF_CHECKED);
    menu->CheckMenuItem(ID_HOOK_STOP, MF_UNCHECKED);
    StartHook();
    this->GetParent()->SetWindowTextA("DxWnd");
    hIcon = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE);
    hPrevIcon = this->GetParent()->SetIcon(hIcon, TRUE);
    if(hPrevIcon)
        DestroyIcon(hPrevIcon);
    if(this->SystemTray.Enabled()) {
        UINT IdResource = IDI_DXWAIT;
        this->SystemTray.SetIcon(IdResource);
    }
}

void CDxwndhostView::OnHookStop() {
    CMenu *menu;
    HICON hIcon, hPrevIcon;
    // v.68: status control!!
    if (CheckStatus()) return;
    menu = this->GetParent()->GetMenu();
    menu->CheckMenuItem(ID_HOOK_START, MF_UNCHECKED);
    menu->CheckMenuItem(ID_HOOK_STOP, MF_CHECKED);
    EndHook();
    this->GetParent()->SetWindowTextA("DxWnd (off)");
    hIcon = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_DXOFF), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE);
    hPrevIcon = this->GetParent()->SetIcon(hIcon, TRUE);
    if(hPrevIcon)
        DestroyIcon(hPrevIcon);
    if(this->SystemTray.Enabled()) {
        UINT IdResource = IDI_DXIDLE;
        this->SystemTray.SetIcon(IdResource);
    }
}

void CDxwndhostView::OnClearAllLogs() {
    // TODO: Please add a command handler code here.
    int i, len;
    CListCtrl &listctrl = GetListCtrl();
    CString	FilePath;
    HRESULT res;
    res = MessageBoxLang(DXW_STRING_CLEARALL, DXW_STRING_WARNING, MB_YESNO | MB_ICONQUESTION);
    if(res != IDYES) return;
    for(i = 0; i < MAXTARGETS; i ++) {
        if(!TargetMaps[i].path[0]) break;
        TargetMaps[i].tflags &= ~OUTTRACE;
        FilePath = TargetMaps[i].path;
        len = FilePath.ReverseFind('\\');
        if (len == 0) continue;
        FilePath.Truncate(len);
        FilePath.Append("\\dxwnd.log");
        _unlink(FilePath);
    }
    // v2.1.92: clean %TEMP% folder
    FilePath = getenv("TEMP");
    FilePath.Append("\\dxwnd.log");
    _unlink(FilePath);
}

DWORD WINAPI WaitForDeath(LPVOID arg) {
    while(!gMustDie) Sleep(1000);
    exit(0);
}

typedef struct {
    CSystemTray *Tray;
    CDxwndhostView *HostView;
} arg_type;

DWORD WINAPI TrayIconUpdate(arg_type *arg) {
    CSystemTray *Tray = arg->Tray;
    CDxwndhostView *HostView = arg->HostView;
    int DxStatus, PrevDxStatus;
    UINT IconId;
    char sMsg[1024];
    char *Status;
    char DllVersion[21];
    int TickCount, IdleCount;
    PrevDxStatus = -1; // a different one...
    TickCount = 0;
    IdleCount = 0;
    while (TRUE) {
        // once a second ...
        Sleep(1000);
        if(Tray->IsIconPending() && IsWinXP()) {
            CWnd *pParent = HostView->GetParent();
            Tray->MaximiseFromTray(pParent, FALSE);
            Tray->Reset();
            return(0);
        }
        if(gTransientMode && gMustDie) {
            Tray->HideIcon();
            delete(Tray->GetAncestor(GA_ROOTOWNER));
            RecoverGammaRamp();
            exit(0);
        }
        DxStatus = GetHookStatus(NULL);
        switch (DxStatus) {
        case DXW_IDLE:
            IconId = IDI_DXIDLE;
            Status = "DISABLED";
            break;
        case DXW_ACTIVE:
            IconId = IDI_DXWAIT;
            Status = "READY";
            break;
        case DXW_RUNNING:
            IconId = IDI_DXRUN;
            Status = "RUNNING";
            break;
        default:
            IconId = IDI_DXIDLE;
            Status = "???";
            break;
        }
        if (DxStatus != DXW_RUNNING) {
            TickCount = 0;
            Tray->StopAnimation();
            Tray->SetIcon(IconId);
            IdleCount++;
            if(IdleCount == 2) {
                if(gAutoHideMode && !m_StayHidden)
                    Tray->MaximiseFromTray(pParent, FALSE);
            }
        } else {
            // animation state machine ....
            TickCount++;
            IdleCount = 0;
            if (DxStatus != PrevDxStatus)
                Tray->SetIcon(IconId);
            if(!gNoIconSpinning) {
                if(TickCount > 4) {
                    Tray->SetIconList(IDI_RUN1, IDI_RUNA);
                    Tray->Animate(100, -1);
                }
                if(TickCount > 6) {
                    TickCount = 0;
                    Tray->StopAnimation();
                    Tray->SetIcon(IDI_RUN0);
                }
            }
        }
        DXWNDSTATUS DxWndStatus;
        GetDllVersion(DllVersion);
        DxWndStatus.Status = DxStatus;
        if(DxStatus == DXW_RUNNING) {
            GetHookStatus(&DxWndStatus);
            sprintf_s(sMsg, 1024,
                      "Running \"%s\"\nScreen = (%dx%d) %dBPP\nFullScreen = %s\nDX version = %d",
                      pTitles[DxWndStatus.OrigIdx].title,
                      DxWndStatus.Width, DxWndStatus.Height, DxWndStatus.ColorDepth,
                      DxWndStatus.IsFullScreen ? "Yes" : "No", DxWndStatus.DXVersion);
        } else
            sprintf_s(sMsg, 1024, "DxWnd %s\nHook status: %s", DllVersion, Status);
        Tray->SetTooltipText(sMsg);
        PrevDxStatus = DxStatus;
    }
}

void CDxwndhostView::OnGoToTrayIcon() {
    CMenu *menu = this->GetParent()->GetMenu();
    UINT IconId;
    pParent = this->GetParent();
    if(!gNoIconMode && !this->SystemTray.Enabled()) {
        HANDLE StatusThread;
        DWORD dwThrdId;
        // Create the tray icon
        if(!this->SystemTray.Create(NULL,
                                    WM_ICON_NOTIFY, "DxWnd", NULL, IDR_MENU_SYSTRAY,
                                    FALSE,
                                    m_StartToTray ? NULL : "DxWnd DirectDraw windowizer", // NULL inhibits the Tray banner
                                    "DxWnd",
                                    NIIF_INFO, 10)) {
            MessageBoxLang(DXW_STRING_TRAYFAIL, DXW_STRING_ERROR, MB_OK);
            // error path: if can't create a system tray icon, transient logic must be silently placed here
            if(gTransientMode) StatusThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WaitForDeath, (LPVOID)NULL, 0, &dwThrdId);
            return;
        }
        IconId = (menu->GetMenuState(ID_HOOK_START, MF_BYCOMMAND) == MF_CHECKED) ? IDI_DXWAIT : IDI_DXIDLE;
        if(!this->SystemTray.SetIcon(IconId)) {
            MessageBoxLang(DXW_STRING_ICONFAIL, DXW_STRING_ERROR, MB_OK);
            return;
        }
        arg_type arg;
        arg.HostView = this;
        arg.Tray = &(this->SystemTray);
        StatusThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TrayIconUpdate, (LPVOID)&arg, 0, &dwThrdId);
    }
    if(gNoIconMode)
        this->SystemTray.HideIcon();
    else
        this->SystemTray.ShowIcon();
    this->SystemTray.SetMenuDefaultItem(0, TRUE);
    this->SystemTray.SetTargetWnd(pParent);		// Send all menu messages here.
    this->SystemTray.MinimiseToTray(pParent, FALSE);
}

void CDxwndhostView::OnSaveFile() {
    if (this->isUpdated)
        if (MessageBoxLang(DXW_STRING_SAVELIST, DXW_STRING_WARNING, MB_YESNO | MB_ICONQUESTION) == IDYES)
            this->SaveConfigFile();
}

void CDxwndhostView::OnTrayRestore() {
    CWnd *pParent = this->GetParent();
    this->SystemTray.MaximiseFromTray(pParent, FALSE);
}

void CDxwndhostView::OnViewStatus() {
    CStatusDialog *pDlg = new CStatusDialog();
    BOOL ret = pDlg->Create(CStatusDialog::IDD, this);
    pDlg->ShowWindow(SW_SHOW);
}

void CDxwndhostView::OnViewShims() {
    POSITION pos;
    int i;
    char *ExePath;
    CListCtrl &listctrl = GetListCtrl();
    if(!(pos = listctrl.GetFirstSelectedItemPosition())) return;
    i = listctrl.GetNextSelectedItem(pos);
    ExePath = TargetMaps[i].path;
    CShimsDialog *pDlg = new CShimsDialog(NULL, ExePath);
    BOOL ret = pDlg->Create(CShimsDialog::IDD, this);
    pDlg->ShowWindow(SW_SHOW);
}

void CDxwndhostView::OnViewGammaRamp() {
    CGammaRampDialog *pDlg = new CGammaRampDialog();
    BOOL ret = pDlg->Create(CGammaRampDialog::IDD, this);
    pDlg->ShowWindow(SW_SHOW);
}

void CDxwndhostView::OnViewMousePosition() {
    CMousePositionDialog *pDlg = new CMousePositionDialog();
    BOOL ret = pDlg->Create(CMousePositionDialog::IDD, this);
    pDlg->ShowWindow(SW_SHOW);
}

void CDxwndhostView::OnViewCDPlayer() {
    CCDPlayerDialog *pDlg = new CCDPlayerDialog();
    BOOL ret = pDlg->Create(CCDPlayerDialog::IDD, this);
    pDlg->ShowWindow(SW_SHOW);
}

void CDxwndhostView::OnGammaCtrl() {
    CTabGammaCtrl *pDlg = new CTabGammaCtrl();
    BOOL ret = pDlg->Create(CTabGammaCtrl::IDD, this);
    pDlg->ShowWindow(SW_SHOW);
}

TARGETMAP *ViewTarget; // dirty !!!!

void CDxwndhostView::OnViewFlags() {
    int i;
    CTargetDlg dlg;
    POSITION pos;
    CListCtrl &listctrl = GetListCtrl();
    if(!listctrl.GetSelectedCount()) return;
    pos = listctrl.GetFirstSelectedItemPosition();
    i = listctrl.GetNextSelectedItem(pos);
    ViewTarget = &TargetMaps[i];
    CViewFlagsDialog *pDlg = new CViewFlagsDialog();
    BOOL ret = pDlg->Create(CViewFlagsDialog::IDD, this);
    pDlg->ShowWindow(SW_SHOW);
}

void CDxwndhostView::OnViewImportTable() {
    int i;
    CTargetDlg dlg;
    POSITION pos;
    CListCtrl &listctrl = GetListCtrl();
    if(!listctrl.GetSelectedCount()) return;
    pos = listctrl.GetFirstSelectedItemPosition();
    i = listctrl.GetNextSelectedItem(pos);
    ViewTarget = &TargetMaps[i];
    //MessageBox(ViewTarget->path, "View Import Table", 0);
    extern void DumpImportTable(LPSTR, LPSTR);
    DumpImportTable(ViewTarget->path, "./pedump.txt");
    ShellExecute(NULL, "open", ".\\pedump.txt", NULL, NULL, SW_SHOWNORMAL);
}

void CDxwndhostView::OnViewDesktop() {
    CDesktopDialog *pDlg = new CDesktopDialog();
    BOOL ret = pDlg->Create(CDesktopDialog::IDD, this);
    pDlg->ShowWindow(SW_SHOW);
}

void CDxwndhostView::OnViewPalette() {
    CPaletteDialog *pDlg = new CPaletteDialog();
    BOOL ret = pDlg->Create(CPaletteDialog::IDD, this);
    pDlg->ShowWindow(SW_SHOW);
}

void CDxwndhostView::OnGlobalSettings() {
    //CGlobalSettings *pDlg = new CGlobalSettings();
    //BOOL ret = pDlg->Create(CGlobalSettings::IDD, this);
    //pDlg->ShowWindow(SW_SHOW);
    CGlobalSettings *pDlg = new(CGlobalSettings);
    pDlg->DoModal();
}

void CDxwndhostView::OnGlobalStartUp() {
    CGlobalStartUp *pDlg = new CGlobalStartUp();
    BOOL ret = pDlg->Create(CGlobalStartUp::IDD, this);
    pDlg->ShowWindow(SW_SHOW);
}

void CDxwndhostView::OnExpertModeToggle() {
    CMenu *menu;
    extern BOOL gReadOnlyMode;
    gbExpertMode = !gbExpertMode;
    menu = this->GetParent()->GetMenu();
    ::DestroyMenu(menu->GetSafeHmenu());
    menu->LoadMenu(gbExpertMode ? IDR_MAINFRAME_EX : IDR_MAINFRAME);
    this->GetParent()->SetMenu(menu);
    menu->CheckMenuItem(ID_OPTIONS_EXPERTMODE, gbExpertMode ? MF_CHECKED : MF_UNCHECKED);
    if(gReadOnlyMode) menu->EnableMenuItem(ID_FILE_SAVE, MF_DISABLED | MF_GRAYED);
    WritePrivateProfileString("window", "expert", (gbExpertMode ? "1" : "0"), gInitPath);
}

void CDxwndhostView::OnViewTimeSlider() {
    CTimeSliderDialog *pDlg = new CTimeSliderDialog();
    BOOL ret = pDlg->Create(CTimeSliderDialog::IDD, this);
    pDlg->ShowWindow(SW_SHOW);
}

void CDxwndhostView::OnViewVirtualJoystick() {
    CVJoyDialog *pDlg = new CVJoyDialog();
    BOOL ret = pDlg->Create(CVJoyDialog::IDD, this);
    pDlg->ShowWindow(SW_SHOW);
}

void CDxwndhostView::Resize() {
    if(bIconViewMode) return;
    CListCtrl &listctrl = GetListCtrl();
    LV_COLUMN listcol;
    int i, tmp, size = 0;
    for(i = 0; i < MAXTARGETS; i ++) {
        TargetMaps[i].index = i; // renumber the items
        if(strlen(TargetMaps[i].path) == 0) break;
        tmp = listctrl.GetStringWidth(PrivateMaps[i].title);
        if(size < tmp) size = tmp;
    }
    listcol.mask = LVCF_WIDTH;
    listcol.cx = size + 32;
    listctrl.SetColumn(0, &listcol);
}

void CDxwndhostView::OnRButtonDown(UINT nFlags, CPoint point) {
    CMenu popup;
    int res;
    CPoint menuPoint = point;
    // v2.05.43: R-Click auto-selects the item
    UINT nHitFlags;
    CListCtrl &listctrl = GetListCtrl();
    int iItem = listctrl.HitTest(point, &nHitFlags);
    // v2.05.43: if clicked outside a selection, propose a short menu with the "Add" command only
    if((iItem == -1) || (nHitFlags & LVHT_NOWHERE))
        popup.LoadMenu(IDR_MENU_VOID);
    else {
        popup.LoadMenu(gbExpertMode ? IDR_MENU_POPUP_EX : IDR_MENU_POPUP);
        listctrl.SetItemState(iItem, LVIS_SELECTED, LVIS_SELECTED);
    }
    ClientToScreen(&menuPoint);
    res = popup.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD, menuPoint.x, menuPoint.y, this);
    switch(res) {
    case ID_PRUN:
        OnRun(RUN_DEFAULT);
        break;
    case ID_RUNUNHOOKED:
        OnRun(RUN_UNHOOKED);
        break;
    case ID_RUNSUSPENDED:
        OnRun(RUN_SUSPENDED);
        break;
    case ID_PMODIFY:
        OnModify();
        break;
    case ID_PDELETE:
        OnDelete();
        break;
    case ID_PADD:
        OnAdd();
        break;
    case ID_PDUPLICATE:
        OnDuplicate();
        break;
    case ID_PEXPLORE:
        OnExplore();
        break;
    case ID_PLOG_VIEW:
        OnViewLog();
        break;
    case ID_PLOG_VIEWTAIL:
        OnViewLogTail();
        break;
    case ID_PLOG_DELETE:
        OnDeleteLog();
        break;
    case ID_PLOG_DEBUGVIEW:
        OnDebugView();
        break;
    case ID_SETREGISTRY:
        OnSetRegistry();
        break;
    case ID_SETPATH:
        OnSetPath();
        break;
    case ID_TASK_KILL:
        OnKill();
        break;
    case ID_PKILL:
        OnProcessKill();
        break;
    case ID_PKILLALL:
        OnProcessKillAll();
        break;
    case ID_TASK_PAUSE:
        OnPause();
        break;
    case ID_TASK_RESUME:
        OnResume();
        break;
    case ID_PEXPORT:
        OnExport();
        break;
    case ID_FILE_IMPORT:
        OnImport();
        break;
    case ID_FILE_OPENCONFIG:
        OnOpenConfig();
        break;
    case ID_FILE_EXPLOREDXWND:
        OnExploreDxWnd();
        break;
    case ID_DESKTOPCOLORDEPTH_8BPP:
        OnDesktopcolordepth8bpp();
        break;
    case ID_DESKTOPCOLORDEPTH_16BPP:
        OnDesktopcolordepth16bpp();
        break;
    case ID_DESKTOPCOLORDEPTH_24BPP:
        OnDesktopcolordepth24bpp();
        break;
    case ID_DESKTOPCOLORDEPTH_32BPP:
        OnDesktopcolordepth32bpp();
        break;
    case ID_TOOLS_RECOVERSCREENMODE:
        OnRecoverScreenMode();
        break;
    case ID_TOOLS_CLEARCOMPATIBILITYFLAGS:
        OnClearCompatibilityFlags();
        break;
    case ID_TOOLS_BLACKER:
        OnRunBlacker();
        break;
    case ID_TOOLS_RECOVERSYSTEMCOLORS:
        OnRecoverSystemColors();
        break;
    case ID_TOOLS_RECOVERGAMMARAMP:
        OnRecoverGammaRamp();
        break;
    case ID_MOVE_TOP:
        OnMoveTop();
        break;
    case ID_MOVE_UP:
        OnMoveUp();
        break;
    case ID_MOVE_DOWN:
        OnMoveDown();
        break;
    case ID_MOVE_BOTTOM:
        OnMoveBottom();
        break;
    case ID_VIEW_FLAGS:
        OnViewFlags();
        break;
    case ID_VIEW_IMPORT_TABLE:
        OnViewImportTable();
        break;
    case ID_PROXY_REMOVE:
        OnProxyRemove();
        break;
    case ID_PROXY_DDRAW:
        OnProxyDdraw();
        break;
    case ID_PROXY_D3D8:
        OnProxyD3D8();
        break;
    case ID_PROXY_D3D9:
        OnProxyD3D9();
        break;
    case ID_PROXY_WING32:
        OnProxyWinG32();
        break;
    case ID_PROXY_DINPUT:
        OnProxyDInput();
        break;
    case ID_PROXY_WINMM:
        OnProxyWinMM();
        break;
    }
    CListView::OnRButtonDown(nFlags, menuPoint);
}

typedef struct {
    char *path;
    char *launch;
} PROCESSMAP;

static DWORD WINAPI TransientWaitForChildDeath(void *p) {
    PROCESSMAP *ProcessMap = (PROCESSMAP *)p;
    BOOL bIsSomeoneAlive;
    int ret;
    char sPath[MAX_PATH];
    char sLaunch[MAX_PATH];
    char sTemp[MAX_PATH];
    char *lpProcName, *lpNext;
    // strip full pathname and keep executable name only
    strcpy(sPath, "");
    if(ProcessMap->path[0]) {
        strncpy(sTemp, ProcessMap->path, MAX_PATH);
        lpProcName = sTemp;
        while (lpNext = strchr(lpProcName, '\\')) lpProcName = lpNext + 1;
        strncpy(sPath, lpProcName, MAX_PATH);
    }
    strcpy(sLaunch, "");
    if(ProcessMap->launch[0]) {
        strncpy(sTemp, ProcessMap->launch, MAX_PATH);
        lpProcName = sTemp;
        while (lpNext = strchr(lpProcName, '\\')) lpProcName = lpNext + 1;
        strncpy(sLaunch, lpProcName, MAX_PATH);
    }
    Sleep(3000); // Wait for process creation - necessary?
    while(TRUE) {
        Sleep(2000);
        //OutTrace("status check loop ...\n");
        bIsSomeoneAlive = FALSE;
        // v2.05.46: replaced (ret!=0) condition with (ret==603) to keep DxWnd.exe alive in error cases
        if(sPath[0]) {
            ret = KillProcByName(sPath, FALSE, FALSE);
            if(ret) OutTrace("KillProcByName path=\"%s\" ret=%d %s\n", sPath, ret, ret == 603 ? "terminated" : "error!");
            if(ret != 603) bIsSomeoneAlive = TRUE;
        }
        if(sLaunch[0]) {
            ret = KillProcByName(sLaunch, FALSE, FALSE);
            if(ret) OutTrace("KillProcByName launch=\"%s\" ret=%d %s\n", sLaunch, ret, ret == 603 ? "terminated" : "error!");
            if(ret != 603) bIsSomeoneAlive = TRUE;
        }
        if(!bIsSomeoneAlive)
            break;
    }
    gMustDie = TRUE;
    return 0;
}

static char *sEventCode(DWORD ec) {
    char *c;
    switch(ec) {
    case EXCEPTION_DEBUG_EVENT:
        c = "exception";
        break;
    case CREATE_THREAD_DEBUG_EVENT:
        c = "create thread";
        break;
    case CREATE_PROCESS_DEBUG_EVENT:
        c = "create process";
        break;
    case EXIT_THREAD_DEBUG_EVENT:
        c = "exit thread";
        break;
    case EXIT_PROCESS_DEBUG_EVENT:
        c = "exit process";
        break;
    case LOAD_DLL_DEBUG_EVENT:
        c = "load dll";
        break;
    case UNLOAD_DLL_DEBUG_EVENT:
        c = "unload dll";
        break;
    case OUTPUT_DEBUG_STRING_EVENT:
        c = "out debug";
        break;
    case RIP_EVENT:
        c = "rip";
        break;
    default:
        c = "unknown";
        break;
    }
    return c;
}

DWORD RecoverTargetMaps(LPVOID TargetMaps) {
    // v2.04.77: fixed logic, wait until program termination before clearing DXWNDSTATUS shared area
    Sleep(2000);
    while(TRUE) {
        Sleep(500);
        if(GetHookInfo()->Status != DXW_RUNNING) {
            SetTarget(NULL, (TARGETMAP *)TargetMaps);
            break;
        }
    }
    return 0;
}

#define FILECOPYBUFSIZE 1024

static void MakeHiddenFile(char *sTargetPath) {
    HANDLE hFile     = INVALID_HANDLE_VALUE;
    HANDLE hTempFile = INVALID_HANDLE_VALUE;
    BOOL fSuccess  = FALSE;
    DWORD dwRetVal = 0;
    UINT uRetVal   = 0;
    DWORD dwBytesRead    = 0;
    DWORD dwBytesWritten = 0;
    char  chBuffer[FILECOPYBUFSIZE];
    hFile = CreateFile(sTargetPath,           // file name
                       GENERIC_READ,          // open for reading
                       0,                     // do not share
                       NULL,                  // default security
                       OPEN_EXISTING,         // existing file only
                       FILE_ATTRIBUTE_NORMAL, // normal file
                       NULL);                 // no template
    if (hFile == INVALID_HANDLE_VALUE) {
        //PrintError(TEXT("First CreateFile failed"));
        return;
    }
    strcat(sTargetPath, ".noshim");
    //  Deletes last copy of the file, just in case it was updated (patched?)
    // DeleteFile(sTargetPath);
    //  Creates the new file to write to for the upper-case version.
    hTempFile = CreateFile((LPTSTR) sTargetPath,	// file name
                           GENERIC_WRITE,			// open for write
                           0,						// do not share
                           NULL,					// default security
                           CREATE_ALWAYS,			// overwrite existing
                           //FILE_ATTRIBUTE_HIDDEN,	// hidden file
                           FILE_ATTRIBUTE_NORMAL, // normal file
                           NULL);					// no template
    if (hTempFile == INVALID_HANDLE_VALUE) {
        char msg[120];
        sprintf(msg, "Creation of hidden copy of target file with no SHIMs failed\nError=%d", GetLastError());
        MessageBox(0, msg, "Warning", 0);
        //PrintError(TEXT("Second CreateFile failed"));
        if (!CloseHandle(hFile)) {
            sprintf(msg, "CloseHandle failed\nError=%d", GetLastError());
            MessageBox(0, msg, "Error", 0);
        }
        return;
    }
    //  Reads BUFSIZE blocks to the buffer and copy to the temporary
    //  file.
    do {
        if (ReadFile(hFile, chBuffer, FILECOPYBUFSIZE, &dwBytesRead, NULL)) {
            fSuccess = WriteFile(hTempFile, chBuffer, dwBytesRead, &dwBytesWritten, NULL);
            if (!fSuccess) {
                //PrintError(TEXT("WriteFile failed"));
                break;
            }
        } else {
            //PrintError(TEXT("ReadFile failed"));
            break;
        }
        //  Continues until the whole file is processed.
    } while (dwBytesRead == FILECOPYBUFSIZE);
    CloseHandle(hFile);
    CloseHandle(hTempFile);
}

PROCESSMAP ProcMap;

void CDxwndhostView::OnRun() {
    OnRun(RUN_DEFAULT);
}

void CDxwndhostView::OnRun(int runmode) {
    CListCtrl &listctrl = GetListCtrl();
    POSITION pos;
    int i;
    if(gTransientMode) {
        i = iProgIndex - 1;
        if(i < 0) i = 0;
    } else {
        if(!listctrl.GetSelectedCount()) return;
        pos = listctrl.GetFirstSelectedItemPosition();
        i = listctrl.GetNextSelectedItem(pos);
    }
    OnRun(runmode, i);
}

void CDxwndhostView::OnRunPanic() {
    int choice = MessageBox(
                     "Autolog mode: this command will run the program selected in the DxWnd panel "
                     "enabling basic logging to collect diagnostic information. "
                     "After the program terminates, save the dxwnd.log file in the program's folder before it "
                     "could be overwritten!",
                     "DxWnd AUTOLOG MODE", MB_YESNO | MB_ICONEXCLAMATION);
    if(choice == IDYES)
        OnRun(RUN_PANIC);
}

void CDxwndhostView::OnRun(int runmode, int i) {
    CListCtrl &listctrl = GetListCtrl();
    STARTUPINFO sinfo;
    char folderpath[MAX_PATH + 20]; // max + space for ".noshim"
    char exepath[MAX_PATH + 20]; // max + space for ".noshim"
    TARGETMAP RestrictedMaps[2];
    char *sRunTargetPath;
    BOOL bRestrictedMap;
    BOOL bHookEnabled;
    BOOL bMustCommit;
    BOOL bPanicMode = FALSE;
    LPCSTR lpCommandLine = NULL;
    TARGETMAP *tm = &TargetMaps[i];
    PRIVATEMAP *pm = &PrivateMaps[i];
    BOOL MultiHooks = GetPrivateProfileInt("window", "multiprocesshook", 0, gInitPath);
    // when hook disabled do not try to use debug or inject modes
    bHookEnabled = (GetHookStatus(NULL) != DXW_IDLE);
    if(tm->path[0] == '?') {
        int choice = MessageBox("Incomplete entry\nplease, update the path field before running",
                                "DxWnd", MB_YESNO | MB_ICONEXCLAMATION);
        if(choice == IDYES) this->OnModify();
        return;
    }
    // check for single-hook mode & hook still busy ...
    if(!MultiHooks) {
        //  && (DXW_RUNNING == GetHookStatus(NULL)))
        // MessageBox("running", "DxWnd", 0);
        if(CheckStatus()) return;
    }
    sRunTargetPath = (strlen(pm->launchpath) > 0) ? pm->launchpath : tm->path;
    // can't use restricted map when hooking multiple father-son processes!
    bRestrictedMap = !(tm->flags5 & (INJECTSON | DEBUGSON));
    ZeroMemory(&sinfo, sizeof(sinfo));
    sinfo.cb = sizeof(sinfo);
    // create a virtually single entry in the targetmap array
    if(bRestrictedMap) {
        memcpy(&RestrictedMaps[0], tm, sizeof(TARGETMAP));
        memset(&RestrictedMaps[1], 0, sizeof(TARGETMAP));
    }
    if(!(pm->startfolder[0])) {
        strcpy_s(folderpath, sizeof(folderpath), sRunTargetPath);
        strcpy_s(exepath, sizeof(exepath), sRunTargetPath);
        PathRemoveFileSpec(folderpath);
    } else {
        strcpy_s(folderpath, sizeof(folderpath), pm->startfolder);
        strcpy_s(exepath, sizeof(exepath), sRunTargetPath);
    }
    if(bRestrictedMap) {
        if (tm->flags7 & COPYNOSHIMS) {
            MakeHiddenFile(exepath);
            strncpy(RestrictedMaps[0].path, exepath, MAX_PATH);
        }
        if(runmode == RUN_UNHOOKED) RestrictedMaps[0].flags3 &= ~HOOKENABLED;
        SetTarget(NULL, RestrictedMaps);
    }
    OutTrace("OnRun idx=%d prog=\"%s\" mode=%x\n", i, tm->path, runmode);
    if(tm->flags7 & HOOKNORUN) {
        MessageBoxLang(DXW_STRING_CANT_RUN, DXW_STRING_WARNING, MB_ICONERROR | MB_OK);
        return;
    }
    // handle PANIC MODE
    if(runmode == RUN_PANIC) {
        DWORD tflags = RestrictedMaps[0].tflags;
        tflags &= ~(OUTSEPARATED | OUTCIRCULAR); // ghogho .... missing append?
        tflags |= OUTTRACE;
        tflags |= (OUTDDRAWTRACE | OUTD3DTRACE | OUTDXWINTRACE | OUTOGLTRACE | OUTSDLTRACE | OUTSOUNDTRACE | OUTINPUTS | OUTSYSLIBS);
        RestrictedMaps[0].tflags = tflags;
        runmode = RUN_DEFAULT;
        bPanicMode = TRUE; // let's remember the original mode
        SetTarget(NULL, RestrictedMaps);
    }
    // self-elevation if configured and necessary
    if(tm->flags & NEEDADMINCAPS) {
        extern BOOL DxSelfElevate(CDxwndhostView *);
        OSVERSIONINFO osver = { sizeof(osver) };
        if (GetVersionEx(&osver) && (osver.dwMajorVersion >= 6)) {
            OutTrace("self elevation\n");
            DxSelfElevate(this);
        }
    }
    if((tm->flags3 & EMULATEREGISTRY) || (tm->flags4 & OVERRIDEREGISTRY)) {
        if(isRegistryUpdated) {
            if(MessageBoxLang(DXW_STRING_VREG_UPDATE, DXW_STRING_WARNING, MB_OKCANCEL | MB_ICONINFORMATION) == IDOK) {
                // v2.05.02: optimized, save current game only
                SaveConfigItem(tm, pm, i, gInitPath);
                isRegistryUpdated = FALSE;
                isDriversUpdated = FALSE; // when saving, save all ...
            }
        }
        OutTrace("export virtual registry entry[%d]\n");
        FlushRegistryFile(pm->registry);
    }
    if((tm->flags10 & FAKEHDDRIVE) || (tm->flags10 & FAKECDDRIVE)) {
        if(isDriversUpdated) {
            if(MessageBoxLang(DXW_STRING_DRV_UPDATE, DXW_STRING_WARNING, MB_OKCANCEL | MB_ICONINFORMATION) == IDOK) {
                // v2.05.02: optimized, save current game only
                SaveConfigItem(tm, pm, i, gInitPath);
                isRegistryUpdated = FALSE; // when saving, save all ...
                isDriversUpdated = FALSE;
            }
        }
    }
    if(tm->flags7 & SHOWHINTS) {
        OutTrace("checksafediscversion\n");
        CheckSafeDiscVersion(tm->path);
    }
    if(tm->flags & SETDPIAWARE)
        _putenv("__COMPAT_LAYER=HIGHDPIAWARE");
    bMustCommit = (tm->flags12 & COMMITPAGE) ? TRUE : FALSE;
    // v2.04.04: fix - STARTDEBUG and INJECTSUSPENDED must take place only when HOOKENABLED
    if ((tm->flags2 & STARTDEBUG) &&
            (tm->flags3 & HOOKENABLED) &&
            bHookEnabled) {
        // Debugger mode
        OutTrace("debugger mode\n");
        if(runmode == RUN_SUSPENDED)
            MessageBox("Suspended mode not available for debug hook", "Warning", MB_ICONWARNING | MB_OK);
        else {
            ThreadInfo_Type *pThreadInfo;
            pThreadInfo = (ThreadInfo_Type *)malloc(sizeof(ThreadInfo_Type));
            memcpy((void *) & (pThreadInfo->TM), (void *)tm, sizeof(TARGETMAP));
            memcpy((void *) & (pThreadInfo->PM), (void *)pm, sizeof(PRIVATEMAP));
            strcpy_s(pThreadInfo->ExePath, MAX_PATH, exepath);
            CloseHandle(CreateThread( NULL, 0, InjectDebug, pThreadInfo, 0, NULL));
        }
    } else if ((tm->flags7 & INJECTSUSPENDED) &&
               (tm->flags3 & HOOKENABLED) &&
               bHookEnabled) {
        // Inject mode
        OutTrace("injectsuspended mode=%d(%s)\n",
                 runmode, (runmode == RUN_SUSPENDED) ? "SUSPENDED" : ((runmode == RUN_UNHOOKED) ? "UNHOOKED" : "DEFAULT"));
        InjectSuspended(exepath, folderpath, (runmode == RUN_SUSPENDED), bMustCommit, tm, pm);
    } else if ((tm->flags12 & INJECTPROXY) &&
               (tm->flags3 & HOOKENABLED) &&
               bHookEnabled) {
        // Proxy mode
        //OutTrace("injectProxy mode=%d(%s)\n",
        //	runmode, (runmode==RUN_SUSPENDED)?"SUSPENDED":((runmode==RUN_UNHOOKED)?"UNHOOKED":"DEFAULT"));
        //InjectProxy(exepath, folderpath, (runmode == RUN_SUSPENDED), bMustCommit, tm, pm);
        if(runmode == RUN_SUSPENDED)
            MessageBox("Suspended mode not available for proxy hook", "Warning", MB_ICONWARNING | MB_OK);
        else {
            ThreadInfo_Type *pThreadInfo;
            pThreadInfo = (ThreadInfo_Type *)malloc(sizeof(ThreadInfo_Type));
            memcpy((void *) & (pThreadInfo->TM), (void *)tm, sizeof(TARGETMAP));
            memcpy((void *) & (pThreadInfo->PM), (void *)pm, sizeof(PRIVATEMAP));
            strcpy_s(pThreadInfo->ExePath, MAX_PATH, exepath);
            CloseHandle(CreateThread( NULL, 0, InjectProxy, pThreadInfo, 0, NULL));
        }
    } else {
        // windowshook mode
        OutTrace("setwindowshook mode\n");
        if(runmode == RUN_SUSPENDED)
            MessageBox("Suspended mode useless for windows hook", "Warning", MB_ICONWARNING | MB_OK);
        else {
            ThreadInfo_Type *pThreadInfo;
            pThreadInfo = (ThreadInfo_Type *)malloc(sizeof(ThreadInfo_Type));
            memcpy((void *) & (pThreadInfo->TM), (void *)tm, sizeof(TARGETMAP));
            memcpy((void *) & (pThreadInfo->PM), (void *)pm, sizeof(PRIVATEMAP));
            strcpy_s(pThreadInfo->ExePath, MAX_PATH, exepath);
            CloseHandle(CreateThread( NULL, 0, InjectHook, pThreadInfo, 0, NULL));
        }
    }
    if(tm->flags & SETDPIAWARE)
        _putenv("__COMPAT_LAYER=");
    if(gTransientMode) {
        ProcMap.launch = &(pm->launchpath[0]);
        ProcMap.path   = &(tm->path[0]);
        CreateThread( NULL, 0, TransientWaitForChildDeath, &ProcMap, 0, NULL);
    }
    if(bPanicMode) {
        CString FilePath;
        int len;
        char sMessage[MAX_PATH + 220];
        if(pm->startfolder[0]) {
            FilePath.SetString(pm->startfolder);
            FilePath.Append("\\dxwnd.log");
        } else {
            FilePath.SetString(tm->path);
            len = FilePath.ReverseFind('\\');
            if (len == 0) return;
            FilePath.Truncate(len);
            FilePath.Append("\\dxwnd.log");
        }
        sprintf(sMessage, "When the task is finished, get the logfile here:\n"
                "\"%s\"\n"
                "Do you want to open the file now?", FilePath);
        int choice = MessageBox(sMessage, "DxWnd AUTOLOG MODE", MB_YESNO | MB_ICONEXCLAMATION);
        if(choice == IDYES)
            ShellExecute(NULL, "open", FilePath, NULL, NULL, SW_SHOW);
    }
    // wait & recover
    if(bRestrictedMap) CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RecoverTargetMaps, (LPVOID)TargetMaps, 0, NULL);
    // go to tray icon mode when autohide is set, but only if hooking is enabled. If not enable it
    // is not possible to monitor the process status, so the window will never show automatically!
    if(gAutoHideMode && (runmode != RUN_UNHOOKED)) this->OnGoToTrayIcon();
    // not working: the file is opened, can't be deleted
    //if(tm->flags7 & COPYNOSHIMS){
    //	DeleteFile(exepath);
    //}
}

void SwitchToColorDepth(int bpp) {
    DEVMODE CurrentDevMode;
    BOOL res;
    EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &CurrentDevMode);
    if(MessageBoxLangArg(DXW_STRING_NEWCOLOR, DXW_STRING_WARNING, MB_OKCANCEL | MB_ICONQUESTION, CurrentDevMode.dmBitsPerPel, bpp) != IDOK)
        return;
    //OutTraceDW("ChangeDisplaySettings: CURRENT wxh=(%dx%d) BitsPerPel=%d -> 16\n",
    //	CurrentDevMode.dmPelsWidth, CurrentDevMode.dmPelsHeight, CurrentDevMode.dmBitsPerPel);
    CurrentDevMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
    CurrentDevMode.dmBitsPerPel = bpp;
    res = ChangeDisplaySettings(&CurrentDevMode, CDS_UPDATEREGISTRY);
    if(res != DISP_CHANGE_SUCCESSFUL) {
        WCHAR *err;
        switch(res) {
        case DISP_CHANGE_BADDUALVIEW:
            err = L"BADDUALVIEW";
            break;
        case DISP_CHANGE_BADFLAGS:
            err = L"BADFLAGS";
            break;
        case DISP_CHANGE_BADMODE:
            err = L"BADMODE";
            break;
        case DISP_CHANGE_BADPARAM:
            err = L"BADPARAM";
            break;
        case DISP_CHANGE_FAILED:
            err = L"FAILED";
            break;
        case DISP_CHANGE_NOTUPDATED:
            err = L"NOTUPDATED";
            break;
        case DISP_CHANGE_RESTART:
            err = L"RESTART";
            break;
        default:
            err = L"???";
            break;
        }
        MessageBoxLangArg(DXW_STRING_ERRCOLOR, DXW_STRING_ERROR, MB_OK | MB_ICONEXCLAMATION, err, GetLastError());
    }
}

void CDxwndhostView::OnDesktopcolordepth8bpp() {
    SwitchToColorDepth(8);
}

void CDxwndhostView::OnDesktopcolordepth16bpp() {
    SwitchToColorDepth(16);
}

void CDxwndhostView::OnDesktopcolordepth24bpp() {
    SwitchToColorDepth(24);
}

void CDxwndhostView::OnDesktopcolordepth32bpp() {
    SwitchToColorDepth(32);
}

void CDxwndhostView::OnRecoverScreenMode() {
    RevertScreenChanges(&this->InitDevMode);
    if(!SystemParametersInfo(SPI_SETCLEARTYPE, NULL, (PVOID)InitClearType, NULL)) OutTrace("Set SPI_SETCLEARTYPE error %d\n", GetLastError());
    if(!SystemParametersInfo(SPI_SETFONTSMOOTHING, InitFontSmoothing, NULL, NULL)) OutTrace("Set SPI_SETFONTSMOOTHING error %d\n", GetLastError());
    if(!SystemParametersInfo(SPI_SETFONTSMOOTHINGCONTRAST, NULL, (PVOID)InitFontSmoothingContrast, NULL)) OutTrace("Set SPI_SETFONTSMOOTHINGCONTRAST error %d\n", GetLastError());
    if(!SystemParametersInfo(SPI_SETFONTSMOOTHINGORIENTATION, NULL, (PVOID)InitFontSmoothingOrientation, NULL)) OutTrace("Set SPI_SETFONTSMOOTHINGORIENTATION error %d\n", GetLastError());
    if(!SystemParametersInfo(SPI_SETFONTSMOOTHINGTYPE, NULL, (PVOID)InitFontSmoothingType, NULL)) OutTrace("Set SPI_SETFONTSMOOTHINGTYPE error %d\n", GetLastError());
}

void CDxwndhostView::OnRecoverSystemColors() {
    extern DWORD SysColors[];
    //for(int index=COLOR_SCROLLBAR; index<=COLOR_MENUBAR; index++) {
    for(int index = COLOR_SCROLLBAR; index <= COLOR_BTNHIGHLIGHT; index++) { // don't mess with taskbar ...
        if(SysColors[index]) {
            INT colors[1];
            colors[0] = index;
            ::SetSysColors(1, colors, &SysColors[index]);
        }
    }
}

void CDxwndhostView::OnRecoverGammaRamp() {
    extern WORD GammaRamp[];
    SetDeviceGammaRamp(::GetDC(NULL), &GammaRamp);
}

void CDxwndhostView::OnRunBlacker() {
    //ShellExecute(NULL, "open blacker.exe", gInitialWorkingDir, NULL, NULL, SW_SHOW);
    STARTUPINFO sinfo;
    PROCESS_INFORMATION pinfo;
    ZeroMemory(&sinfo, sizeof(sinfo));
    sinfo.cb = sizeof(sinfo);
    if(CreateProcess(NULL,
                     ".\\blacker.exe",
                     0, 0, false, CREATE_DEFAULT_ERROR_MODE, NULL, gInitialWorkingDir, &sinfo, &pinfo)) {
        CloseHandle(pinfo.hProcess); // no longer needed, avoid handle leakage
        CloseHandle(pinfo.hThread); // no longer needed, avoid handle leakage
    } else
        MessageBox("blacker run failed", "error", 0);
}

void CDxwndhostView::OnAssociate() {
    extern int SetUpRegistry();
    SetUpRegistry();
}

void CDxwndhostView::OnDisassociate() {
    extern int ClearRegistry();
    ClearRegistry();
}

void CDxwndhostView::OnCDCharger() {
    CCDChargerDialog *pDlg = new CCDChargerDialog();
    BOOL ret = pDlg->Create(CCDChargerDialog::IDD, this);
    pDlg->ShowWindow(SW_SHOW);
}

void CDxwndhostView::OnClearCompatibilityFlags() {
    int i;
    POSITION pos;
    CListCtrl &listctrl = GetListCtrl();
    CString	FilePath;
    HRESULT res;
    char sMessage[500];
    DWORD lKeyLength;
    LONG ret;
    if(!listctrl.GetSelectedCount()) return ;
    pos = listctrl.GetFirstSelectedItemPosition();
    i = listctrl.GetNextSelectedItem(pos);
    //sprintf(sMessage, "Clear all compatibility flags for \"%s\"?", PrivateMaps[i].title);
    //res=MessageBox(sMessage, "DxWnd", MB_YESNO | MB_ICONQUESTION);
    res = MessageBoxLangArg(DXW_STRING_CLEAR_COMP, DXW_STRING_DXWND, MB_YESNO | MB_ICONQUESTION, PrivateMaps[i].title);
    if(res != IDYES) return;
    FilePath = TargetMaps[i].path;
    if (FilePath.GetLength() == 0) return;
    HKEY hk;
    lKeyLength = 0L;
    DWORD lType;
    ret = RegOpenKeyEx(HKEY_CURRENT_USER,
                       "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers", 0, KEY_ALL_ACCESS, &hk);
    if(ret) {
        if(ret != ERROR_FILE_NOT_FOUND) {
            sprintf(sMessage, "error %ld in RegOpenKeyEx HKEY_CURRENT_USER", ret);
            MessageBox(sMessage, "DxWnd", MB_OK);
        }
    } else {
        ret = RegQueryValueEx(hk, TargetMaps[i].path, NULL, &lType, NULL, &lKeyLength);
        if(ret && (ret != ERROR_FILE_NOT_FOUND)) {
            sprintf(sMessage, "error %ld in RegQueryValue hk=%lx \"%s\"", ret, hk, TargetMaps[i].path);
            MessageBox(sMessage, "DxWnd", MB_OK);
        }
        if(lKeyLength > 0L) {
            MessageBox("Found compatibility flag in HKEY_CURRENT_USER", "DxWnd", MB_OK);
            if(ret = RegDeleteValue(hk, TargetMaps[i].path)) {
                sprintf(sMessage, "error %ld in RegDeleteValue hk=%lx \"%s\"", ret, hk, TargetMaps[i].path);
                MessageBox(sMessage, "DxWnd", MB_OK);
            }
        }
        RegCloseKey(hk);
    }
    hk = 0L;
    lKeyLength = 0L;
    ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                       "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers", 0, KEY_ALL_ACCESS, &hk);
    if(ret) {
        if(ret != ERROR_FILE_NOT_FOUND) {
            sprintf(sMessage, "error %ld in RegOpenKeyEx HKEY_LOCAL_MACHINE", ret);
            MessageBox(sMessage, "DxWnd", MB_OK);
        }
    } else {
        ret = RegQueryValueEx(hk, TargetMaps[i].path, NULL, &lType, NULL, &lKeyLength);
        if(ret && (ret != ERROR_FILE_NOT_FOUND)) {
            sprintf(sMessage, "error %ld in RegQueryValue hk=%lx \"%s\"", ret, hk, TargetMaps[i].path);
            MessageBox(sMessage, "DxWnd", MB_OK);
        }
        if(lKeyLength > 0L) {
            MessageBox("Found compatibility flag in HKEY_LOCAL_MACHINE", "DxWnd", MB_OK);
            if(ret = RegDeleteValue(hk, TargetMaps[i].path)) {
                sprintf(sMessage, "error %ld in RegDeleteValue hk=%lx \"%s\"", ret, hk, TargetMaps[i].path);
                MessageBox(sMessage, "DxWnd", MB_OK);
            }
        }
        RegCloseKey(hk);
    }
}

void CDxwndhostView::OnDropFiles(HDROP dropInfo) {
    CString sFile;
    DWORD nBuffer = 0;
    char *p;
    // Get number of files
    UINT nFilesDropped = DragQueryFile(dropInfo, 0xFFFFFFFF, NULL, 0);
    if(nFilesDropped > 0) {
        nBuffer = DragQueryFile(dropInfo, 0, NULL, 0);
        DragQueryFile(dropInfo, 0, sFile.GetBuffer(nBuffer + 1), nBuffer + 1);
        p = (char *)sFile.GetString();
        p += (strlen(p) - 4);
        if(!_strnicmp(p, ".dxw", 4)) {
            for(UINT i = 1; i <= nFilesDropped; i++) {
                if(!this->OnImport(sFile)) break;
                nBuffer = DragQueryFile(dropInfo, i, NULL, 0);
                DragQueryFile(dropInfo, i, sFile.GetBuffer(nBuffer + 1), nBuffer + 1);
                p = (char *)sFile.GetString();
                p += (strlen(p) - 4);
                if(_strnicmp(p, ".dxw", 4)) break;
            }
        } else
            this->OnAdd(sFile.GetBuffer());
        sFile.ReleaseBuffer();
    }
    DragFinish(dropInfo);
}

void CDxwndhostView::OnProxyRemove() {
    CListCtrl &listctrl = GetListCtrl();
    POSITION pos;
    int i;
    char folderpath[MAX_PATH + 1];
    char filepath[MAX_PATH + 1];
    char *sRunTargetPath;
    // 1: select the program entry
    if(!listctrl.GetSelectedCount()) return;
    pos = listctrl.GetFirstSelectedItemPosition();
    i = listctrl.GetNextSelectedItem(pos);
    // 2: find the execution path
    sRunTargetPath = (strlen(PrivateMaps[i].launchpath) > 0) ? PrivateMaps[i].launchpath : TargetMaps[i].path;
    if(!(PrivateMaps[i].startfolder[0])) {
        strcpy_s(folderpath, sizeof(folderpath), sRunTargetPath);
        PathRemoveFileSpec(folderpath);
    } else
        strcpy_s(folderpath, sizeof(folderpath), PrivateMaps[i].startfolder);
    // 3: remove the definition file
    sprintf(filepath, "%s\\dxwnd.dxw", folderpath);
    DeleteFile(filepath);
    // 4: remove all proxy files
    sprintf(filepath, "%s\\%s.dll", folderpath, "ddraw");
    DeleteFile(filepath);
    sprintf(filepath, "%s\\%s.dll", folderpath, "d3d8");
    DeleteFile(filepath);
    sprintf(filepath, "%s\\%s.dll", folderpath, "d3d9");
    DeleteFile(filepath);
    sprintf(filepath, "%s\\%s.dll", folderpath, "dinput");
    DeleteFile(filepath);
    sprintf(filepath, "%s\\%s.dll", folderpath, "winmm");
    DeleteFile(filepath);
    // be careful with this one ....
    //sprintf(filepath, "%s\\%s.dll", folderpath, "wing32");
    //DeleteFile(filepath);
    // 5: remove dxwnd dll (if possible ...)
    sprintf(filepath, "%s\\%s.dll", folderpath, "dxwnd");
    DeleteFile(filepath);
    // 6: remove extensions dll (if present ...)
    sprintf(filepath, "%s\\%s.dll", folderpath, "disasm");
    DeleteFile(filepath);
    // 7: some interaction
    MessageBox("DxWnd Proxies uninstalled", "DxWnd Proxy", 0);
}

static void CopyConfiguration(char *target, char *source, char *room, char *tokens[]) {
    char val[81];
    for(int i = 0; ; i++) {
        char *token = tokens[i];
        if(!token) break;
        GetPrivateProfileString(room, token, "", val, 80, source);
        WritePrivateProfileString(room, token, val, target);
    }
}

static void SaveJoystick(char *targetpath) {
    char *tokens[] = {"flags", "sensivity", "deadzone", NULL};
    CopyConfiguration(targetpath, gInitPath, "joystick", tokens);
}

void SaveHotKeys(char *targetpath) {
    char *tokens[] = {
        "timetoggle", "timefast", "timeslow", "altf4", "cliptoggle",
        "refresh", "logtoggle", "plogtoggle", "fpstoggle", "printscreen",
        "corner", "freezetime", "fullscreen", "workarea", "desktop",
        "custom", "cdprev", "cdnext", "minimize", "restore", "kill", NULL
    };
    CopyConfiguration(targetpath, gInitPath, "keymapping", tokens);
}

void CDxwndhostView::OnProxyInstall(char *dllfile) {
    CListCtrl &listctrl = GetListCtrl();
    POSITION pos;
    int i;
    char folderpath[MAX_PATH + 1];
    char filepath[MAX_PATH + 1];
    char dllpath[MAX_PATH + 1];
    char *sRunTargetPath;
    DWORD bFailed = 0;
    // 0: be sure what you're doing ...
    sprintf(filepath, "Do you want to install a %s.dll proxy for this game?", dllfile);
    if(MessageBox(filepath, "DxWnd proxy", MB_YESNO | MB_ICONQUESTION) != IDYES) return;
    // 1: select the program entry
    if(!listctrl.GetSelectedCount()) return;
    pos = listctrl.GetFirstSelectedItemPosition();
    i = listctrl.GetNextSelectedItem(pos);
    // 2: find the execution path
    sRunTargetPath = (strlen(PrivateMaps[i].launchpath) > 0) ? PrivateMaps[i].launchpath : TargetMaps[i].path;
    if(!(PrivateMaps[i].startfolder[0])) {
        strcpy_s(folderpath, sizeof(folderpath), sRunTargetPath);
        PathRemoveFileSpec(folderpath);
    } else
        strcpy_s(folderpath, sizeof(folderpath), PrivateMaps[i].startfolder);
    // 3: export a definition file
    sprintf(filepath, "%s\\dxwnd.dxw", folderpath);
    BOOL updated = PrivateMaps[i].updated; // save & recover ...
    SaveConfigItem(&TargetMaps[i], &PrivateMaps[i], 0, filepath);
    PrivateMaps[i].updated = updated;
    // in case, save also joystick configuration ...
    if(TargetMaps[i].flags6 & VIRTUALJOYSTICK) SaveJoystick(filepath);
    // in case, save also virtual keys configuration ...
    if(TargetMaps[i].flags4 & ENABLEHOTKEYS) SaveHotKeys(filepath);
    // add a timestamp / version
    SaveExportStamp(filepath);
    // 4: copy proxy file
    sprintf(filepath, "%s\\%s.dll", folderpath, dllfile);
    sprintf(dllpath, "%s\\proxy\\%s.dll", gInitialWorkingDir, dllfile);
    if(!CopyFile(dllpath, filepath, FALSE)) bFailed |= 0x1;
    // 5: copy dxwnd dll
    sprintf(filepath, "%s\\dxwnd.dll", folderpath);
    sprintf(dllpath, "%s\\dxwnd.dll", gInitialWorkingDir);
    if(!CopyFile(dllpath, filepath, FALSE)) bFailed |= 0x2;
    // 6: copy necessary extensions
    if ((TargetMaps[i].flags & HANDLEEXCEPTIONS) ||
            (TargetMaps[i].flags4 & INTERCEPTRDTSC) ||
            (TargetMaps[i].flags5 & DISABLEMMX) ||
            (TargetMaps[i].dflags & CPUALLFLAGSMASK)) {
        sprintf(filepath, "%s\\disasm.dll", folderpath);
        sprintf(dllpath, "%s\\disasm.dll", gInitialWorkingDir);
        if(!CopyFile(dllpath, filepath, FALSE)) bFailed |= 0x4;
    }
    // 7: some interaction
    switch(bFailed) {
    case 0:
        sprintf(filepath, "Proxy %s installed\nQuit DxWnd or stop hook before running the program", dllfile);
        break;
    case 0x1:
        sprintf(filepath, "Proxy %s install failed while copying %s.dll file\nCheck files on game folder", dllfile, dllfile);
        break;
    case 0x2:
        sprintf(filepath, "Proxy %s install failed while copying dxwnd.dll file\nCheck files on game folder", dllfile);
        break;
    case 0x3:
        sprintf(filepath, "Proxy %s install failed while copying %s.dll and dxwnd.dll files\nCheck files on game folder", dllfile, dllfile);
        break;
    case 0x4:
        sprintf(filepath, "Proxy %s install failed while copying disasm.dll file\nCheck files on game folder");
        break;
    }
    MessageBox(filepath, "DxWnd Proxy", 0);
}

void CDxwndhostView::OnProxyDdraw() {
    OnProxyInstall("ddraw");
}

void CDxwndhostView::OnProxyD3D8() {
    OnProxyInstall("d3d8");
}

void CDxwndhostView::OnProxyD3D9() {
    OnProxyInstall("d3d9");
}

void CDxwndhostView::OnProxyWinG32() {
    OnProxyInstall("wing32");
}

void CDxwndhostView::OnProxyDInput() {
    OnProxyInstall("dinput");
}

void CDxwndhostView::OnProxyWinMM() {
    OnProxyInstall("winmm");
}
