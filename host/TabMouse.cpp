// TabMouse.cpp : implementation file
//

#include "stdafx.h"
#include "TargetDlg.h"
#include "TabMouse.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabMouse dialog

CTabMouse::CTabMouse(CWnd *pParent /*=NULL*/)
//	: CTargetDlg(pParent)
    : CDialog(CTabMouse::IDD, pParent) {
    //{{AFX_DATA_INIT(CTabMouse)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}

BOOL CTabMouse::PreTranslateMessage(MSG *pMsg) {
    //if(((pMsg->message == WM_KEYDOWN) || (pMsg->message == WM_KEYUP))
    if((pMsg->message == WM_KEYDOWN)
            && (pMsg->wParam == VK_RETURN))
        return TRUE;
    return CWnd::PreTranslateMessage(pMsg);
}

void CTabMouse::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
    CTargetDlg *cTarget = ((CTargetDlg *)(this->GetParent()->GetParent()));
    // Cursor Visibility
    DDX_Radio(pDX, IDC_CURSORAUTOMATIC, cTarget->m_MouseVisibility);
    // Cursor Handling
    DDX_Check(pDX, IDC_MODIFYMOUSE, cTarget->m_ModifyMouse);
    DDX_Check(pDX, IDC_KEEPCURSORWITHIN, cTarget->m_KeepCursorWithin);
    DDX_Check(pDX, IDC_KEEPCURSORFIXED, cTarget->m_KeepCursorFixed);
    DDX_Check(pDX, IDC_MESSAGEPROC, cTarget->m_MessageProc);
    DDX_Check(pDX, IDC_FIXMOUSEHOOK, cTarget->m_FixMouseHook);
    DDX_Check(pDX, IDC_FIXMESSAGEHOOK, cTarget->m_FixMessageHook);
    DDX_Check(pDX, IDC_FIXNCHITTEST, cTarget->m_FixNCHITTEST);
    DDX_Check(pDX, IDC_RELEASEMOUSE, cTarget->m_ReleaseMouse);
    DDX_Check(pDX, IDC_NOMOUSEEVENTS, cTarget->m_NoMouseEvents);
    DDX_Check(pDX, IDC_FIXMOUSERAWINPUT, cTarget->m_FixMouseRawInput);
    DDX_Check(pDX, IDC_MOUSESHIELD, cTarget->m_MouseShield);
    DDX_Check(pDX, IDC_FIXMOUSELPARAM, cTarget->m_FixMouseLParam);
    DDX_Check(pDX, IDC_ADAPTMOUSESPEED, cTarget->m_AdaptMouseSpeed);
    // Cursor Clipper
    DDX_Radio(pDX, IDC_CLIPPERAUTO, cTarget->m_MouseClipper);
    DDX_Check(pDX, IDC_CLIPMENU, cTarget->m_ClipMenu);
    // Invert Mouse
    DDX_Check(pDX, IDC_INVERTMOUSEXAXIS, cTarget->m_InvertMouseXAxis);
    DDX_Check(pDX, IDC_INVERTMOUSEYAXIS, cTarget->m_InvertMouseYAxis);
}

BEGIN_MESSAGE_MAP(CTabMouse, CDialog)
    //{{AFX_MSG_MAP(CTabMouse)
    // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabMouse message handlers
