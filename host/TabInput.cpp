// TabInput.cpp : implementation file
//

#include "stdafx.h"
#include "TargetDlg.h"
#include "TabInput.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabInput dialog

CTabInput::CTabInput(CWnd *pParent /*=NULL*/)
//	: CTargetDlg(pParent)
    : CDialog(CTabInput::IDD, pParent) {
    //{{AFX_DATA_INIT(CTabInput)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}

BOOL CTabInput::PreTranslateMessage(MSG *pMsg) {
    //if(((pMsg->message == WM_KEYDOWN) || (pMsg->message == WM_KEYUP))
    if((pMsg->message == WM_KEYDOWN)
            && (pMsg->wParam == VK_RETURN))
        return TRUE;
    return CWnd::PreTranslateMessage(pMsg);
}

void CTabInput::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
    CTargetDlg *cTarget = ((CTargetDlg *)(this->GetParent()->GetParent()));
    // Control keys
    DDX_Radio(pDX, IDC_ALTTAB_DEFAULT, cTarget->m_AltTabMode);
    // DirectInput
    DDX_Check(pDX, IDC_HOOKDI, cTarget->m_HookDI);
    DDX_Check(pDX, IDC_HOOKDI8, cTarget->m_HookDI8);
    DDX_Check(pDX, IDC_UNACQUIRE, cTarget->m_Unacquire);
    DDX_Check(pDX, IDC_EMULATERELMOUSE, cTarget->m_EmulateRelMouse);
    DDX_Check(pDX, IDC_SCALERELMOUSE, cTarget->m_ScaleRelMouse);
    DDX_Check(pDX, IDC_SKIPDEVTYPEHID, cTarget->m_SkipDevTypeHID);
    DDX_Check(pDX, IDC_SUPPRESSDIERRORS, cTarget->m_SuppressDIErrors);
    DDX_Check(pDX, IDC_SHAREDKEYBOARD, cTarget->m_SharedKeyboard);
    // Joystick
    DDX_Check(pDX, IDC_VIRTUALJOYSTICK, cTarget->m_VirtualJoystick);
    DDX_Check(pDX, IDC_HIDEJOYSTICKS, cTarget->m_HideJoysticks);
    // Xinput
    DDX_Check(pDX, IDC_HOOKXINPUT, cTarget->m_HookXinput);
    // Keyboard handling
    DDX_Check(pDX, IDC_ENABLEHOTKEYS, cTarget->m_EnableHotKeys);
    DDX_Check(pDX, IDC_HANDLEALTF4, cTarget->m_HandleAltF4);
    DDX_Check(pDX, IDC_NODISABLEPRINT, cTarget->m_NoDisablePrint);
    DDX_Check(pDX, IDC_FIXASYNCKEYSTATE, cTarget->m_FixAsyncKeyState);
}

BEGIN_MESSAGE_MAP(CTabInput, CDialog)
    //{{AFX_MSG_MAP(CTabInput)
    // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabInput message handlers
