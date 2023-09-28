// TabSound.cpp : implementation file
//

#include "stdafx.h"
#include "TargetDlg.h"
#include "TabSound.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabCompat dialog

CTabSound::CTabSound(CWnd *pParent /*=NULL*/)
//	: CTargetDlg(pParent)
    : CDialog(CTabSound::IDD, pParent) {
    //{{AFX_DATA_INIT(CTabSound)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}

BOOL CTabSound::PreTranslateMessage(MSG *pMsg) {
    //if(((pMsg->message == WM_KEYDOWN) || (pMsg->message == WM_KEYUP))
    if((pMsg->message == WM_KEYDOWN)
            && (pMsg->wParam == VK_RETURN))
        return TRUE;
    return CWnd::PreTranslateMessage(pMsg);
}

BOOL CTabSound::OnInitDialog() {
    CSliderCtrl *Slider;
    CDialog::OnInitDialog();
    Slider = (CSliderCtrl *)this->GetDlgItem(IDC_CDSLIDER);
    Slider->SetRange(0, +100, 0);
    Slider->SetTicFreq(10);
    return TRUE;  // return TRUE unless you set the focus to a control
}

void CTabSound::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
    CTargetDlg *cTarget = ((CTargetDlg *)(this->GetParent()->GetParent()));
    DDX_Check(pDX, IDC_HOOKDIRECTSOUND, cTarget->m_HookDirectSound);
    DDX_Check(pDX, IDC_SOUNDMUTE, cTarget->m_SoundMute);
    DDX_Check(pDX, IDC_LOCKVOLUME, cTarget->m_LockVolume);
    DDX_Check(pDX, IDC_SAFEMIDIOUT, cTarget->m_SafeMidiOut);
    DDX_Check(pDX, IDC_SETCDVOLUME, cTarget->m_SetCDVolume);
    DDX_Radio(pDX, IDC_GFOCUSDEFAULT, cTarget->m_GFocusMode);
    DDX_Slider(pDX, IDC_CDSLIDER, cTarget->m_FixedVolume);
    DDX_Check(pDX, IDC_VIRTUALCDAUDIO, cTarget->m_VirtualCDAudio);
    DDX_Check(pDX, IDC_FORCETRACKREPEAT, cTarget->m_ForceTrackRepeat);
    DDX_Check(pDX, IDC_IGNOREMCIDEVID, cTarget->m_IgnoreMCIDevId);
    DDX_Check(pDX, IDC_CDROMPRESENT, cTarget->m_CDROMPresent);
    DDX_Check(pDX, IDC_HACKMCIFRAMES, cTarget->m_HackMCIFrames);
    DDX_Check(pDX, IDC_HOOKEARSOUND, cTarget->m_HookEARSound);
    DDX_Check(pDX, IDC_CDPAUSECAPABILITY, cTarget->m_CDPauseCapability);
    DDX_Check(pDX, IDC_SUPPRESSCDAUDIO, cTarget->m_SuppressCDAudio);
    DDX_Check(pDX, IDC_RESERVEMCIDEVICE, cTarget->m_ReserveMCIDevice);
    DDX_Check(pDX, IDC_FIXDEFAULTMCIID, cTarget->m_FixDefaultMCIId);
    DDX_Check(pDX, IDC_LOCKCDTRAY, cTarget->m_LockCDTray);
    DDX_Check(pDX, IDC_DSINITVOLUME, cTarget->m_DSInitVolume);
}

BEGIN_MESSAGE_MAP(CTabSound, CDialog)
    //{{AFX_MSG_MAP(CTabCompat)
    // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabCompat message handlers


