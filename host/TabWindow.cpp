// TabOpenGL.cpp : implementation file
//

#include "stdafx.h"
#include "TargetDlg.h"
#include "TabWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabWindow dialog


CTabWindow::CTabWindow(CWnd *pParent /*=NULL*/)
    : CDialog(CTabWindow::IDD, pParent) {
    //{{AFX_DATA_INIT(CTabWindow)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}

BOOL CTabWindow::PreTranslateMessage(MSG *pMsg) {
    //if(((pMsg->message == WM_KEYDOWN) || (pMsg->message == WM_KEYUP))
    if((pMsg->message == WM_KEYDOWN)
            && (pMsg->wParam == VK_RETURN))
        return TRUE;
    return CWnd::PreTranslateMessage(pMsg);
}

void CTabWindow::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
    CTargetDlg *cTarget = ((CTargetDlg *)(this->GetParent()->GetParent()));
    // window style
    DDX_Radio(pDX, IDC_DEFAULTSTYLE, cTarget->m_WindowStyle);
    DDX_Check(pDX, IDC_LOCKWINSTYLE, cTarget->m_LockWinStyle);
    DDX_Check(pDX, IDC_DISABLEDWM, cTarget->m_DisableDWM);
    // window management
    DDX_Check(pDX, IDC_PREVENTMAXIMIZE, cTarget->m_PreventMaximize);
    DDX_Check(pDX, IDC_EMULATEMAXIMIZE, cTarget->m_EmulateMaximize);
    DDX_Check(pDX, IDC_FORCEWINRESIZE, cTarget->m_ForceWinResize);
    DDX_Check(pDX, IDC_HIDEMULTIMONITOR, cTarget->m_HideMultiMonitor);
    DDX_Check(pDX, IDC_RECOVERSCREENMODE, cTarget->m_RecoverScreenMode);
    DDX_Check(pDX, IDC_REFRESHONRESIZE, cTarget->m_RefreshOnResize);
    DDX_Check(pDX, IDC_HIDETASKBAR, cTarget->m_HideTaskbar);
    DDX_Check(pDX, IDC_NOTASKBAROVERLAP, cTarget->m_NoTaskbarOverlap);
    DDX_Check(pDX, IDC_SETDPIAWARE, cTarget->m_SetDPIAware);
    DDX_Radio(pDX, IDC_COLORCURRENT, cTarget->m_InitColorDepth);
    DDX_Radio(pDX, IDC_ZORDER_DEFAULT, cTarget->m_ZOrder);
    DDX_Radio(pDX, IDC_FREEMOVE, cTarget->m_WinMovementType);
    // screen resolution management
    DDX_Check(pDX, IDC_LIMITSCREENRES, cTarget->m_LimitScreenRes);
    DDX_LBIndex(pDX, IDC_LISTRES, cTarget->m_MaxScreenRes);
    DDX_Radio(pDX, IDC_SUPPORTSVGA, cTarget->m_ResTypes);
    DDX_Check(pDX, IDC_INITIALRES, cTarget->m_InitialRes);
    DDX_Check(pDX, IDC_MAXIMUMRES, cTarget->m_MaximumRes);
    DDX_Check(pDX, IDC_CUSTOMRES, cTarget->m_CustomRes);
    DDX_Text(pDX, IDC_INITW, cTarget->m_InitResW);
    DDX_Text(pDX, IDC_INITH, cTarget->m_InitResH);
    // FPS indicator
    DDX_Radio(pDX, IDC_FPS_OFF, cTarget->m_ShowFPS);
}

static char *Resolutions[] = {
    "unlimited",
    "320x200",
    "400x300",
    "640x480",
    "800x600",
    "1024x768",
    "1280x960",
    "1280x1024",
    "" // terminator
};

char UnlimitedString[20 + 1];

BOOL CTabWindow::OnInitDialog() {
    AfxEnableControlContainer();
    CListBox *List;
    CTargetDlg *cTarget = ((CTargetDlg *)(this->GetParent()->GetParent()));
    int i;
    List = (CListBox *)this->GetDlgItem(IDC_LISTRES);
    List->ResetContent();
    if(LoadString(AfxGetResourceHandle(), DXW_STRING_UNLIMITED, UnlimitedString, sizeof(UnlimitedString)))
        Resolutions[0] = UnlimitedString;
    for(i = 0; strlen(Resolutions[i]); i++) List->AddString(Resolutions[i]);
    List->SetCurSel(cTarget->m_MaxScreenRes);
    CDialog::OnInitDialog();
    return TRUE;
}


BEGIN_MESSAGE_MAP(CTabWindow, CDialog)
    //{{AFX_MSG_MAP(CTabWindow)
    // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabWindow message handlers


