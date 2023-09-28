// TabHW.cpp : implementation file
//

#include "stdafx.h"
#include "TargetDlg.h"
#include "TabHW.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabCompat dialog

CTabHW::CTabHW(CWnd *pParent /*=NULL*/)
//	: CTargetDlg(pParent)
    : CDialog(CTabHW::IDD, pParent) {
    //{{AFX_DATA_INIT(CTabHW)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}

BOOL CTabHW::PreTranslateMessage(MSG *pMsg) {
    //if(((pMsg->message == WM_KEYDOWN) || (pMsg->message == WM_KEYUP))
    if((pMsg->message == WM_KEYDOWN)
            && (pMsg->wParam == VK_RETURN))
        return TRUE;
    return CWnd::PreTranslateMessage(pMsg);
}

void CTabHW::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
    CTargetDlg *cTarget = ((CTargetDlg *)(this->GetParent()->GetParent()));
    DDX_Check(pDX, IDC_DISABLECPUID, cTarget->m_DisableCPUID);
    DDX_Check(pDX, IDC_CPU_DISABLEMMX, cTarget->m_CPUDisableMMX);
    DDX_Check(pDX, IDC_CPU_DISABLESSE, cTarget->m_CPUDisableSSE);
    DDX_Check(pDX, IDC_CPU_DISABLESSE2, cTarget->m_CPUDisableSSE2);
    DDX_Check(pDX, IDC_CPU_DISABLEPBE, cTarget->m_CPUDisablePBE);
    DDX_Radio(pDX, IDC_CPU_DEFAULT, cTarget->m_CPUVendorId);
}

BEGIN_MESSAGE_MAP(CTabHW, CDialog)
    //{{AFX_MSG_MAP(CTabCompat)
    // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabCompat message handlers


