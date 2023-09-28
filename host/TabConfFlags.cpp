// TabConfFlags.cpp : implementation file
//

#include "stdafx.h"
#include "TargetDlg.h"
#include "TabConfFlags.h"
#include "CGlobalSettings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabCompat dialog

CTabConfFlags::CTabConfFlags(CWnd *pParent /*=NULL*/)
    : CDialog(CTabConfFlags::IDD, pParent) {
}

BOOL CTabConfFlags::PreTranslateMessage(MSG *pMsg) {
    if((pMsg->message == WM_KEYDOWN)
            && (pMsg->wParam == VK_RETURN))
        return TRUE;
    return CWnd::PreTranslateMessage(pMsg);
}

void CTabConfFlags::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
    CGlobalSettings *cTarget = ((CGlobalSettings *)(this->GetParent()->GetParent()));
    DDX_Check(pDX, IDC_CONFIG_DEBUGMODE, cTarget->m_DebugMode);
    DDX_Check(pDX, IDC_CONFIG_CHECKADMIN, cTarget->m_CheckAdminRights);
    DDX_Check(pDX, IDC_CONFIG_NAMEFROMFOLDER, cTarget->m_NameFromFolder);
    DDX_Check(pDX, IDC_CONFIG_MULTIHOOKS, cTarget->m_MultiHooks);
    DDX_Check(pDX, IDC_CONFIG_WARNONEXIT, cTarget->m_WarnOnExit);
    DDX_Check(pDX, IDC_CONFIG_SAVEPATHS, cTarget->m_UpdatePaths);
    DDX_Check(pDX, IDC_CONFIG_32BITICONS, cTarget->m_32BitIcons);
    DDX_Check(pDX, IDC_CONFIG_GRAYICONS, cTarget->m_GrayIcons);
    DDX_Check(pDX, IDC_CONFIG_AUTOSAVE, cTarget->m_AutoSave);
    DDX_Check(pDX, IDC_CONFIG_AUTOHIDE, cTarget->m_AutoHideMode);
    DDX_Check(pDX, IDC_CONFIG_HIDEONESCAPE, cTarget->m_HideOnEscape);
    DDX_CBIndex(pDX, IDC_OVERLAY_POS, cTarget->m_OverlayPosition);
    DDX_CBIndex(pDX, IDC_OVERLAY_STYLE, cTarget->m_OverlayStyle);
}

BEGIN_MESSAGE_MAP(CTabConfFlags, CDialog)
    //{{AFX_MSG_MAP(CTabCompat)
    // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabCompat message handlers


