// CGlobalSettings.cpp : implementation file
//

#include "stdafx.h"
#include "dxwndhost.h"
//#include "dxwndhostView.h"
#include "CGlobalSettings.h"

extern char gInitPath[];
extern BOOL gbDebug;
extern BOOL gAutoHideMode;
extern BOOL gHideOnEscape;
extern BOOL gWarnOnExit;
extern BOOL g32BitIcons;
extern BOOL gGrayIcons;
extern BOOL gAutoSave;

Key_Type FKeys[] = {
    {IDC_KEY_LABEL1,	"Time toggle",	IDC_KEY_COMBO1,		"timetoggle", 0, 0},
    {IDC_KEY_LABEL2,	"Time fast",	IDC_KEY_COMBO2,		"timefast", 0, 0},
    {IDC_KEY_LABEL3,	"Time slow",	IDC_KEY_COMBO3,		"timeslow", 0, 0},
    {IDC_KEY_LABEL4,	"Alt-F4",		IDC_KEY_COMBO4,		"altf4", 0, 0},
    {IDC_KEY_LABEL5,	"Clip toggle",	IDC_KEY_COMBO5,		"cliptoggle", 0, 0},
    {IDC_KEY_LABEL6,	"Refresh",		IDC_KEY_COMBO6,		"refresh", 0, 0},
    {IDC_KEY_LABEL7,	"Log toggle",	IDC_KEY_COMBO7,		"logtoggle", 0, 0},
    {IDC_KEY_LABEL8,	"Position t.",	IDC_KEY_COMBO8,		"plogtoggle", 0, 0},
    {IDC_KEY_LABEL9,	"FPS toggle",	IDC_KEY_COMBO9,		"fpstoggle", 0, 0},
    {IDC_KEY_LABEL10,	"Print screen",	IDC_KEY_COMBO10,	"printscreen", 0, 0},
    {IDC_KEY_LABEL11,	"Corner tog.",	IDC_KEY_COMBO11,	"corner", 0, 0},
    {IDC_KEY_LABEL12,	"Time freeze",	IDC_KEY_COMBO12,	"freezetime", 0, 0},
    {IDC_KEY_LABEL13,	"Fullscr. t.",	IDC_KEY_COMBO13,	"fullscreen", 0, 0},
    {IDC_KEY_LABEL14,	"Work area t.",	IDC_KEY_COMBO14,	"workarea", 0, 0},
    {IDC_KEY_LABEL15,	"Desktop t.",	IDC_KEY_COMBO15,	"desktop", 0, 0},
    {IDC_KEY_LABEL16,	"Custom t.",	IDC_KEY_COMBO16,	"custom", 0, 0},
    {IDC_KEY_LABEL17,	"CD next",		IDC_KEY_COMBO17,	"cdnext", 0, 0},
    {IDC_KEY_LABEL18,	"CD prev",		IDC_KEY_COMBO18,	"cdprev", 0, 0},
    {0, "", 0, 0, 0, 0}
};

Key_Type HKeys[] = {
    {IDC_HKEY_LABEL1,	"Minimize",		IDC_HKEY_COMBO1,	"minimize", 0, 0},
    {IDC_HKEY_LABEL2,	"Restore",		IDC_HKEY_COMBO2,	"restore", 0, 0},
    {IDC_HKEY_LABEL3,	"Kill proc.",	IDC_HKEY_COMBO3,	"kill", 0, 0},
    {0, "", 0, 0, 0, 0}
};

KeyCombo_Type FKeyCombo[] = {
    {-1, "--"},
    {VK_F1, "F1"},
    {VK_F2, "F2"},
    {VK_F3, "F3"},
    {VK_F5, "F5"},
    {VK_F6, "F6"},
    {VK_F7, "F7"},
    {VK_F8, "F8"},
    {VK_F9, "F9"},
    {VK_F10, "F10"},
    {VK_F11, "F11"},
    {VK_F12, "F12"},
    {VK_PRIOR, "PgUp"},
    {VK_NEXT, "PgDown"},
    {VK_HOME, "Home"},
    {VK_END, "End"},
    {VK_INSERT, "Insert"},
    {VK_DELETE, "Delete"},
    {0, ""}
};

KeyCombo_Type HKeyCombo[] = {
    {-1, "--"},
    {VK_END, "END"},
    {VK_HOME, "HOME"},
    {VK_DELETE, "DEL"},
    {VK_ESCAPE, "ESC"},
    {VK_F1, "F1"},
    {VK_F2, "F2"},
    {VK_F3, "F3"},
    {VK_F5, "F5"},
    {VK_F6, "F6"},
    {VK_F7, "F7"},
    {VK_F8, "F8"},
    {VK_F9, "F9"},
    {VK_F10, "F10"},
    {VK_F11, "F11"},
    {VK_F12, "F12"},
    {0, ""}
};

// CGlobalSettings dialog

IMPLEMENT_DYNAMIC(CGlobalSettings, CDialog)

CGlobalSettings::CGlobalSettings(CWnd *pParent /*=NULL*/)
    : CDialog(CGlobalSettings::IDD, pParent) {
}

CGlobalSettings::~CGlobalSettings() {
}

void CGlobalSettings::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CTargetDlg)
    DDX_Control(pDX, IDC_CONF_TABPANEL, m_tabdxTabConfCtrl);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CGlobalSettings, CDialog)
    ON_COMMAND(IDHELP, OnHelp)
END_MESSAGE_MAP()

BOOL CGlobalSettings::OnInitDialog() {
    //MessageBox("CGlobalSettings::OnInitDialog","trace",0);
    AfxEnableControlContainer();
    CDialog::OnInitDialog();
    m_DebugMode = GetPrivateProfileInt("window", "debug", 0, gInitPath);
    m_AutoHideMode = GetPrivateProfileInt("window", "autohide", 0, gInitPath);
    m_HideOnEscape = GetPrivateProfileInt("window", "hideonesc", 0, gInitPath);
    m_CheckAdminRights = GetPrivateProfileInt("window", "checkadmin", 0, gInitPath);
    m_NameFromFolder = GetPrivateProfileInt("window", "namefromfolder", 0, gInitPath);
    m_MultiHooks = GetPrivateProfileInt("window", "multiprocesshook", 0, gInitPath);
    m_WarnOnExit = GetPrivateProfileInt("window", "warnonexit", 0, gInitPath);
    m_32BitIcons = GetPrivateProfileInt("window", "32biticons", 0, gInitPath);
    m_GrayIcons = GetPrivateProfileInt("window", "grayicons", 1, gInitPath);
    m_UpdatePaths = GetPrivateProfileInt("window", "updatepaths", 1, gInitPath);
    m_AutoSave = GetPrivateProfileInt("window", "autosave", 0, gInitPath);
    // texture limits
    m_TexMinX = GetPrivateProfileInt("texture", "MinTexX", 0, gInitPath);
    m_TexMinY = GetPrivateProfileInt("texture", "MinTexY", 0, gInitPath);
    m_TexMaxX = GetPrivateProfileInt("texture", "MaxTexX", 0, gInitPath);
    m_TexMaxY = GetPrivateProfileInt("texture", "MaxTexY", 0, gInitPath);
    // defaults
    m_DefaultCoordinates = GetPrivateProfileInt("window", "defaultcoord", 0, gInitPath);
    m_DefaultPosX = GetPrivateProfileInt("window", "defaultposx", 50, gInitPath);
    m_DefaultPosY = GetPrivateProfileInt("window", "defaultposy", 50, gInitPath);
    m_DefaultSizX = GetPrivateProfileInt("window", "defaultsizx", 800, gInitPath);
    m_DefaultSizY = GetPrivateProfileInt("window", "defaultsizy", 600, gInitPath);
    // overlay
    m_OverlayPosition = GetPrivateProfileInt("window", "overlaypos", 0, gInitPath);
    m_OverlayStyle = GetPrivateProfileInt("window", "overlaystyle", 0, gInitPath);
    // load fkeys
    for(int i = 0; FKeys[i].iLabelResourceId; i++)
        FKeys[i].dwKey = GetPrivateProfileIntA("keymapping", FKeys[i].sIniLabel, 0,  gInitPath);
    // load hot keys
    for(int i = 0; HKeys[i].iLabelResourceId; i++)
        HKeys[i].dwKey = GetPrivateProfileIntA("keymapping", HKeys[i].sIniLabel, 0,  gInitPath);
    m_tabdxTabConfCtrl.Init();
    return TRUE;  // return TRUE unless you set the focus to a control
}

void CGlobalSettings::OnOK() {
    //MessageBox("CGlobalSettings::OnOK","trace",0);
    char val[32];
    CDialog::OnOK();
    m_tabdxTabConfCtrl.OnOK(); // important !!
    // boolean flags
    sprintf_s(val, sizeof(val), "%i", m_DebugMode);
    WritePrivateProfileString("window", "debug", val, gInitPath);
    sprintf_s(val, sizeof(val), "%i", m_AutoHideMode);
    WritePrivateProfileString("window", "autohide", val, gInitPath);
    sprintf_s(val, sizeof(val), "%i", m_HideOnEscape);
    WritePrivateProfileString("window", "hideonesc", val, gInitPath);
    sprintf_s(val, sizeof(val), "%i", m_CheckAdminRights);
    WritePrivateProfileString("window", "checkadmin", val, gInitPath);
    sprintf_s(val, sizeof(val), "%i", m_NameFromFolder);
    WritePrivateProfileString("window", "namefromfolder", val, gInitPath);
    sprintf_s(val, sizeof(val), "%i", m_MultiHooks);
    WritePrivateProfileString("window", "multiprocesshook", val, gInitPath);
    sprintf_s(val, sizeof(val), "%i", m_WarnOnExit);
    WritePrivateProfileString("window", "warnonexit", val, gInitPath);
    sprintf_s(val, sizeof(val), "%i", m_32BitIcons);
    WritePrivateProfileString("window", "32biticons", val, gInitPath);
    sprintf_s(val, sizeof(val), "%i", m_GrayIcons);
    WritePrivateProfileString("window", "grayicons", val, gInitPath);
    sprintf_s(val, sizeof(val), "%i", m_AutoSave);
    WritePrivateProfileString("window", "autosave", val, gInitPath);
    sprintf_s(val, sizeof(val), "%i", m_UpdatePaths);
    WritePrivateProfileString("window", "updatepaths", val, gInitPath);
    // texture limits
    sprintf_s(val, sizeof(val), "%i", m_TexMinX);
    WritePrivateProfileString("texture", "MinTexX", val, gInitPath);
    sprintf_s(val, sizeof(val), "%i", m_TexMinY);
    WritePrivateProfileString("texture", "MinTexY", val, gInitPath);
    sprintf_s(val, sizeof(val), "%i", m_TexMaxX);
    WritePrivateProfileString("texture", "MaxTexX", val, gInitPath);
    sprintf_s(val, sizeof(val), "%i", m_TexMaxY);
    WritePrivateProfileString("texture", "MaxTexY", val, gInitPath);
    // defaults
    sprintf_s(val, sizeof(val), "%i", m_DefaultCoordinates);
    WritePrivateProfileString("window", "defaultcoord", val, gInitPath);
    sprintf_s(val, sizeof(val), "%i", m_DefaultPosX);
    WritePrivateProfileString("window", "defaultposx", val, gInitPath);
    sprintf_s(val, sizeof(val), "%i", m_DefaultPosY);
    WritePrivateProfileString("window", "defaultposy", val, gInitPath);
    sprintf_s(val, sizeof(val), "%i", m_DefaultSizX);
    WritePrivateProfileString("window", "defaultsizx", val, gInitPath);
    sprintf_s(val, sizeof(val), "%i", m_DefaultSizY);
    WritePrivateProfileString("window", "defaultsizy", val, gInitPath);
    // overlay
    sprintf_s(val, sizeof(val), "%i", m_OverlayPosition);
    WritePrivateProfileString("window", "overlaypos", val, gInitPath);
    sprintf_s(val, sizeof(val), "%i", m_OverlayStyle);
    WritePrivateProfileString("window", "overlaystyle", val, gInitPath);
    // save fkeys
    for(int i = 0; FKeys[i].iLabelResourceId; i++) {
        //DWORD dwKey = FKeys[i].dwKey;
        DWORD dwKey = FKeyCombo[FKeys[i].cursor].dwVKeyCode;
        if(dwKey != -1) {
            char sKNum[20];
            sprintf_s(sKNum, sizeof(sKNum), "%i", dwKey);
            WritePrivateProfileString("keymapping", FKeys[i].sIniLabel, sKNum, gInitPath);
        } else
            WritePrivateProfileString("keymapping", FKeys[i].sIniLabel, "", gInitPath);
    }
    // save hot keys
    for(int i = 0; HKeys[i].iLabelResourceId; i++) {
        //DWORD dwKey = HKeys[i].dwKey;
        DWORD dwKey = HKeyCombo[HKeys[i].cursor].dwVKeyCode;
        if(dwKey != -1) {
            char sKNum[20];
            sprintf_s(sKNum, sizeof(sKNum), "%i", dwKey);
            WritePrivateProfileString("keymapping", HKeys[i].sIniLabel, sKNum, gInitPath);
        } else
            WritePrivateProfileString("keymapping", HKeys[i].sIniLabel, "", gInitPath);
    }
    gbDebug = m_DebugMode;
    gAutoHideMode = m_AutoHideMode;
    gHideOnEscape = m_HideOnEscape;
    gWarnOnExit = m_WarnOnExit;
    g32BitIcons = m_32BitIcons;
    gGrayIcons = m_GrayIcons;
    gAutoSave = m_AutoSave;
}

void CGlobalSettings::OnHelp() {
    //MessageBox("CGlobalSettings::OnHelp","trace",0);
    extern void ShowHelp(char *);
    ShowHelp("GlobalSettings");
}
