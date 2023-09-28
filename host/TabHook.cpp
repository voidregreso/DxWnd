// TabHook.cpp : implementation file
//

#include "stdafx.h"
#include "TargetDlg.h"
#include "TabHook.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabHook dialog

CTabHook::CTabHook(CWnd *pParent /*=NULL*/)
//	: CTargetDlg(pParent)
    : CDialog(CTabHook::IDD, pParent) {
    //{{AFX_DATA_INIT(CTabHook)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}

BOOL CTabHook::PreTranslateMessage(MSG *pMsg) {
    //if(((pMsg->message == WM_KEYDOWN) || (pMsg->message == WM_KEYUP))
    if((pMsg->message == WM_KEYDOWN)
            && (pMsg->wParam == VK_RETURN))
        return TRUE;
    return CWnd::PreTranslateMessage(pMsg);
}

void CTabHook::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
    CTargetDlg *cTarget = ((CTargetDlg *)(this->GetParent()->GetParent()));
    DDX_Text(pDX, IDC_MODULE, cTarget->m_Module);
    DDX_Text(pDX, IDC_CMDLINE, cTarget->m_CmdLine);
    DDX_Check(pDX, IDC_HOOKENABLED, cTarget->m_HookEnabled);
    //DDX_Check(pDX, IDC_SETCMDLINE, cTarget->m_SetCmdLine);
    DDX_Radio(pDX, IDC_INJECT_WINDOWSHOOK, cTarget->m_InjectionMode);
    DDX_Check(pDX, IDC_HOTPATCH, cTarget->m_HotPatch);
    DDX_Check(pDX, IDC_HOOKDLLS, cTarget->m_HookDLLs);
    //DDX_Check(pDX, IDC_ANSIWIDE, cTarget->m_AnsiWide);
    DDX_Check(pDX, IDC_HOOKNORUN, cTarget->m_HookNoRun);
    DDX_Check(pDX, IDC_COPYNOSHIMS, cTarget->m_CopyNoShims);
    DDX_Check(pDX, IDC_HOTREGISTRY, cTarget->m_HotRegistry);
    DDX_Check(pDX, IDC_HOOKNOUPDATE, cTarget->m_HookNoUpdate);
    DDX_Check(pDX, IDC_HOOKCHILDWIN, cTarget->m_HookChildWin);
    DDX_Check(pDX, IDC_HOOKDLGWIN, cTarget->m_HookDlgWin);
    DDX_Check(pDX, IDC_SHOWHINTS, cTarget->m_ShowHints);
    DDX_Check(pDX, IDC_HIDEWINDOWCHANGES, cTarget->m_HideWindowChanges);
    DDX_Check(pDX, IDC_FRONTEND, cTarget->m_Frontend);
    // IAT Alignment
    DDX_Radio(pDX, IDC_IATWORDALIGNED, cTarget->m_IATAlignedMode);
    DDX_Check(pDX, IDC_SKIPIATHINT, cTarget->m_SkipIATHint);
    // Kernel32
    DDX_Radio(pDX, IDC_SONDEFAULT, cTarget->m_SonProcessMode);
    // additional hooks
    DDX_Check(pDX, IDC_HOOKWING32, cTarget->m_HookWinG32);
    DDX_Check(pDX, IDC_HOOKGLIDE, cTarget->m_HookGlide);
    DDX_Check(pDX, IDC_SUPPRESSGLIDE, cTarget->m_SuppressGlide);
    DDX_Check(pDX, IDC_HOOKSMACKW32, cTarget->m_HookSmackW32);
    DDX_Check(pDX, IDC_HOOKBINKW32, cTarget->m_HookBinkW32);
    // Page Commit
    DDX_Check(pDX, IDC_COMMITPAGE, cTarget->m_CommitPage);
    DDX_Text(pDX, IDC_COMMITADDRESS, cTarget->m_CommitAddress);
    DDX_Text(pDX, IDC_COMMITLENGTH, cTarget->m_CommitLength);
}

BEGIN_MESSAGE_MAP(CTabHook, CDialog)
    //{{AFX_MSG_MAP(CTabHook)
    // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabHook message handlers
