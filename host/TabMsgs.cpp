// TabInput.cpp : implementation file
//

#include "stdafx.h"
#include "TargetDlg.h"
#include "TabMsgs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabMsgs dialog

CTabMsgs::CTabMsgs(CWnd *pParent /*=NULL*/)
//	: CTargetDlg(pParent)
    : CDialog(CTabMsgs::IDD, pParent) {
    //{{AFX_DATA_INIT(CTabMsgs)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}

BOOL CTabMsgs::PreTranslateMessage(MSG *pMsg) {
    //if(((pMsg->message == WM_KEYDOWN) || (pMsg->message == WM_KEYUP))
    if((pMsg->message == WM_KEYDOWN)
            && (pMsg->wParam == VK_RETURN))
        return TRUE;
    return CWnd::PreTranslateMessage(pMsg);
}

void CTabMsgs::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
    CTargetDlg *cTarget = ((CTargetDlg *)(this->GetParent()->GetParent()));
    // Message processing
    DDX_Radio(pDX, IDC_PROCESSALLMESSAGES, cTarget->m_OffendingMessages);
    //DDX_Check(pDX, IDC_FILTERMESSAGES, cTarget->m_FilterMessages);
    DDX_Check(pDX, IDC_PEEKALLMESSAGES, cTarget->m_PeekAllMessages);
    DDX_Check(pDX, IDC_NOWINPOSCHANGES, cTarget->m_NoWinPosChanges);
    DDX_Check(pDX, IDC_MESSAGEPUMP, cTarget->m_MessagePump);
    DDX_Check(pDX, IDC_SWALLOWMOUSEMOVE, cTarget->m_SwallowMouseMove);
    DDX_Check(pDX, IDC_UNNOTIFYINACTIVE, cTarget->m_UnnotifyInactive);
}

BEGIN_MESSAGE_MAP(CTabMsgs, CDialog)
    //{{AFX_MSG_MAP(CTabMsgs)
    // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabMsgs message handlers
