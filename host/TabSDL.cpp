// TabSDL.cpp : implementation file
//

#include "stdafx.h"
#include "TargetDlg.h"
#include "TabSDL.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabCompat dialog

CTabSDL::CTabSDL(CWnd *pParent /*=NULL*/)
//	: CTargetDlg(pParent)
    : CDialog(CTabSDL::IDD, pParent) {
    //{{AFX_DATA_INIT(CTabSDL)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}

BOOL CTabSDL::PreTranslateMessage(MSG *pMsg) {
    //if(((pMsg->message == WM_KEYDOWN) || (pMsg->message == WM_KEYUP))
    if((pMsg->message == WM_KEYDOWN)
            && (pMsg->wParam == VK_RETURN))
        return TRUE;
    return CWnd::PreTranslateMessage(pMsg);
}

void CTabSDL::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
    CTargetDlg *cTarget = ((CTargetDlg *)(this->GetParent()->GetParent()));
    DDX_Check(pDX, IDC_HOOKSDLLIB, cTarget->m_HookSDLLib);
    DDX_Check(pDX, IDC_HOOKSDL2LIB, cTarget->m_HookSDL2Lib);
    DDX_Check(pDX, IDC_EXTENDSDLHOOK, cTarget->m_ExtendSDLHook);
    DDX_Check(pDX, IDC_SDLEMULATION, cTarget->m_SDLEmulation);
    DDX_Check(pDX, IDC_SDLFORCESTRETCH, cTarget->m_SDLForceStretch);
}

BEGIN_MESSAGE_MAP(CTabSDL, CDialog)
    //{{AFX_MSG_MAP(CTabCompat)
    // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabCompat message handlers


