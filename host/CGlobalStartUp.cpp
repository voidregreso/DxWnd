// CGlobalStartUp.cpp : implementation file
//

#include "stdafx.h"
#include "dxwndhost.h"
#include "CGlobalStartUp.h"

BOOL gRunAtStartUp;

BOOL HasAdminCaps() {
    HKEY hKey;
    LONG lnRes = RegOpenKeyExA(
                     HKEY_LOCAL_MACHINE,
                     "SOFTWARE\\Microsoft\\windows\\CurrentVersion\\Run",
                     0L, KEY_WRITE,
                     &hKey);
    switch(lnRes) {
    case ERROR_SUCCESS:
        RegCloseKey(hKey);
        OutTrace("admin caps detected\n");
        return TRUE;
        break;
    case ERROR_ACCESS_DENIED:
        OutTrace("admin caps denied\n");
        return FALSE;
        break;
    default:
        OutTrace("RegOpenKeyEx error %d\n", GetLastError());
        return FALSE;
        break;
    }
}

BOOL GetStartUp(BOOL bAdmin) {
    HKEY hKey;
    LONG lnRes = RegOpenKeyExA(
                     bAdmin ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER,
                     "SOFTWARE\\Microsoft\\windows\\CurrentVersion\\Run",
                     0L, KEY_READ,
                     &hKey);
    if(lnRes != ERROR_SUCCESS) {
        OutTrace("RegOpenKeyEx error %d\n", GetLastError());
        return FALSE;
    }
    char sPath[MAX_PATH + 1];
    DWORD dwData = MAX_PATH;
    DWORD dwType = REG_SZ;
    strcpy(sPath, "");
    lnRes = RegQueryValueExA(
                hKey,
                "DxWnd",
                NULL,
                &dwType,
                (LPBYTE)sPath,
                &dwData);
    RegCloseKey(hKey);
    if(lnRes != ERROR_SUCCESS) {
        OutTrace("RegQueryValueEx error %d\n", GetLastError());
        return FALSE;
    };
    OutTrace("RegQueryValueEx path=\"%s\" len=%d\n", sPath, dwData);
    //MessageBox(NULL, sPath, "DxWnd", 0);
    return (strlen(sPath) > 0);
}

void SetStartUp(BOOL bAdmin, BOOL bStartInTray, BOOL bStartIdle) {
    HKEY hKey;
    LONG lnRes = RegOpenKeyExA(
                     bAdmin ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER,
                     "SOFTWARE\\Microsoft\\windows\\CurrentVersion\\Run",
                     0L, KEY_WRITE,
                     &hKey);
    if(lnRes != ERROR_SUCCESS) OutTrace("RegOpenKeyEx error %d\n", GetLastError());
    char sPath[MAX_PATH + 20 + 1]; // add 20 bytes for options -m, -t, -i
    if(!GetModuleFileName(NULL, sPath, MAX_PATH)) {
        OutTrace("GetModuleFileName error %d\n", GetLastError());
        return;
    }
    // options ...
    strcat(sPath, " -m"); // move to DxWnd folder
    if(bStartInTray) strcat(sPath, " -t");
    if(bStartIdle) strcat(sPath, " -i");
    lnRes = RegSetValueExA(
                hKey,
                "DxWnd",
                NULL,
                REG_SZ,
                (LPBYTE)sPath,
                strlen(sPath));
    RegCloseKey(hKey);
    if(lnRes != ERROR_SUCCESS) {
        OutTrace("RegSetValueEx error %d\n", GetLastError());
        return;
    }
    OutTrace("RegSetValueEx path=\"%s\"\n", sPath);
    //MessageBox(NULL, sPath, "DxWnd", 0);
}

void ClearStartUp(BOOL bAdmin) {
    HKEY hKey;
    LONG lnRes = RegOpenKeyExA(
                     bAdmin ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER,
                     "SOFTWARE\\Microsoft\\windows\\CurrentVersion\\Run",
                     0L, KEY_ALL_ACCESS,
                     &hKey);
    if(lnRes != ERROR_SUCCESS) OutTrace("RegOpenKeyEx error %d\n", GetLastError());
    char sPath[MAX_PATH + 1];
    DWORD dwData = MAX_PATH;
    DWORD dwType = REG_SZ;
    strcpy(sPath, "");
    lnRes = RegQueryValueExA(
                hKey,
                "DxWnd",
                NULL,
                &dwType,
                (LPBYTE)sPath,
                &dwData);
    if(lnRes != ERROR_SUCCESS) {
        OutTrace("RegQueryValueEx error %d\n", GetLastError());
        RegCloseKey(hKey);
        return;
    }
    lnRes = RegDeleteValueA(
                hKey,
                "DxWnd");
    RegCloseKey(hKey);
    if(lnRes != ERROR_SUCCESS) {
        OutTrace("RegDeleteValue error ret=%d\n", lnRes);
        return;
    }
    OutTrace("DxWnd startup cleared path=\"%s\" len=%d\n", sPath, dwData);
    return;
}

// CGlobalSettings dialog

IMPLEMENT_DYNAMIC(CGlobalStartUp, CDialog)

CGlobalStartUp::CGlobalStartUp(CWnd *pParent /*=NULL*/)
    : CDialog(CGlobalStartUp::IDD, pParent) {
    //m_RunAtStartUp = GetStartUp();
    //m_PreviousSetting = m_RunAtStartUp;
    //OutTrace("CGlobalStartUp::init m_RunAtStartUp=%#x\n", m_RunAtStartUp);
}

CGlobalStartUp::~CGlobalStartUp() {
}

void CGlobalStartUp::DoDataExchange(CDataExchange *pDX) {
    //OutTrace("CGlobalStartUp::DoDataExchange begin m_RunAtStartUp=%#x\n", m_RunAtStartUp);
    m_PreviousSetting = m_RunAtStartUp;
    DDX_Check(pDX, IDC_CONFIG_RUNASSTARTUP, m_RunAtStartUp);
    DDX_Check(pDX, IDC_CONFIG_STARTINTRAY, m_StartInTray);
    DDX_Check(pDX, IDC_CONFIG_STARTIDLE, m_StartIdle);
    CDialog::DoDataExchange(pDX);
    //OutTrace("CGlobalStartUp::DoDataExchange end m_RunAtStartUp=%#x\n", m_RunAtStartUp);
}

BEGIN_MESSAGE_MAP(CGlobalStartUp, CDialog)
    ON_COMMAND(IDHELP, OnHelp)
    ON_COMMAND(IDOK, OnOK)
END_MESSAGE_MAP()

BOOL CGlobalStartUp::OnInitDialog() {
    m_HasAdminCaps = HasAdminCaps();
    m_RunAtStartUp = GetStartUp(m_HasAdminCaps);
    m_PreviousSetting = m_RunAtStartUp;
    m_StartInTray = GetPrivateProfileInt("window", "startintray", 0, gInitPath);
    m_StartIdle = GetPrivateProfileInt("window", "startidle", 0, gInitPath);
    OutTrace("CGlobalStartUp::OnInitDialog HasAdminCaps=%#x RunAtStartUp=%#x\n",
             m_HasAdminCaps, m_RunAtStartUp);
    CDialog::OnInitDialog();
    return TRUE;  // return TRUE unless you set the focus to a control
}

void CGlobalStartUp::OnOK() {
    char val[32];
    CDialog::OnOK();
    // boolean flags
    sprintf_s(val, sizeof(val), "%i", m_StartInTray);
    WritePrivateProfileString("window", "startintray", val, gInitPath);
    sprintf_s(val, sizeof(val), "%i", m_StartIdle);
    WritePrivateProfileString("window", "startidle", val, gInitPath);
    // registry settings
    if(m_RunAtStartUp) {
        //if(!m_PreviousSetting) MessageBox("Run at startup enabled", "DxWnd", 0);
        ClearStartUp(!m_HasAdminCaps);
        SetStartUp(m_HasAdminCaps, m_StartInTray, m_StartIdle);
    } else {
        //if(m_PreviousSetting) MessageBox("Run at startup disabled", "DxWnd", 0);
        if(m_HasAdminCaps) ClearStartUp(TRUE);
        ClearStartUp(FALSE);
    }
}

void CGlobalStartUp::OnCancel() {
    CDialog::OnCancel();
}

void CGlobalStartUp::OnHelp() {
    extern void ShowHelp(char *);
    ShowHelp("GlobalStartUp");
}
