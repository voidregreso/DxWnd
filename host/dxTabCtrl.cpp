// dxTabCtrl.cpp : implementation file
//
/////////////////////////////////////////////////////
// This class is provided as is and Ben Hill takes no
// responsibility for any loss of any kind in connection
// to this code.
/////////////////////////////////////////////////////
// Is is meant purely as a educational tool and may
// contain bugs.
/////////////////////////////////////////////////////
// ben@shido.fsnet.co.uk
// http://www.shido.fsnet.co.uk
/////////////////////////////////////////////////////
// Thanks to a mystery poster in the C++ forum on
// www.codeguru.com I can't find your name to say thanks
// for your Control drawing code. If you are that person
// thank you very much. I have been able to use some of
// you ideas to produce this sample application.
/////////////////////////////////////////////////////

#include "stdafx.h"
#include "dxTabCtrl.h"

#include "TabProgram.h"
#include "TabHook.h"
#include "TabDirectX.h"
#include "TabDirectX2.h"
#include "TabDirect3D.h"
#include "TabInput.h"
#include "TabMouse.h"
#include "TabMsgs.h"
#include "TabTiming.h"
#include "TabWindow.h"
#include "TabOpenGL.h"
#include "TabCompat.h"
#include "TabColor.h"
#include "TabLogs.h"
#include "TabRegistry.h"
#include "TabNotes.h"
#include "TabSysLibs.h"
#include "TabDebug.h"
#include "TabOpenGL.h"
#include "TabSDL.h"
#include "TabSound.h"
#include "TabTweaks.h"
#include "TabIO.h"
#include "TabHW.h"
#include "TabLocale.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BOOL gbDebug;
extern BOOL gbExpertMode;
extern DWORD GetDxWndCaps(void);

/////////////////////////////////////////////////////////////////////////////
// CDXTabCtrl

CDXTabCtrl::CDXTabCtrl() {
    // BEWARE: this part MUST be syncronized with TargetDld initialization in TargetDlg.cpp !!!
    // build helpers strings
    int i = 0;
    m_tabHelpers[i++] = "Main";
    if (gbExpertMode) m_tabHelpers[i++] = "Hook1";
    m_tabHelpers[i++] = "Video";
    m_tabHelpers[i++] = "Mouse";
    if (gbExpertMode) m_tabHelpers[i++] = "Input";
    if (gbExpertMode) m_tabHelpers[i++] = "Msgs";
    m_tabHelpers[i++] = "DirectX";
    m_tabHelpers[i++] = "DirectX2";
    m_tabHelpers[i++] = "Direct3D";
    if (gbExpertMode) m_tabHelpers[i++] = "Timing";
    if (gbExpertMode && (GetDxWndCaps() & DXWCAPS_CANLOG)) m_tabHelpers[i++] = "Logs";
    m_tabHelpers[i++] = "Libs";
    if (gbExpertMode) m_tabHelpers[i++] = "Compatibility";
    if (gbExpertMode) m_tabHelpers[i++] = "Registry1";
    m_tabHelpers[i++] = "Notes";
    if (gbExpertMode) m_tabHelpers[i++] = "OpenGL";
    if (gbExpertMode) m_tabHelpers[i++] = "SDL";
    if (gbExpertMode) m_tabHelpers[i++] = "Sound";
    if (gbExpertMode) m_tabHelpers[i++] = "Tweaks1";
    if (gbExpertMode) m_tabHelpers[i++] = "IOtweaks";
    if (gbDebug) m_tabHelpers[i++] = NULL;
    if (gbDebug) m_tabHelpers[i++] = NULL;
    if (gbExpertMode) m_tabHelpers[i++] = "Locale";
    // build tab control pages
    i = 0;
    m_tabPages[i++] = new CTabProgram;
    if (gbExpertMode) m_tabPages[i++] = new CTabHook;
    m_tabPages[i++] = new CTabWindow;
    m_tabPages[i++] = new CTabMouse;
    if (gbExpertMode) m_tabPages[i++] = new CTabInput;
    if (gbExpertMode) m_tabPages[i++] = new CTabMsgs;
    m_tabPages[i++] = new CTabDirectX;
    m_tabPages[i++] = new CTabDirectX2;
    m_tabPages[i++] = new CTabDirect3D;
    if (gbExpertMode) m_tabPages[i++] = new CTabTiming;
    if (gbExpertMode && (GetDxWndCaps() & DXWCAPS_CANLOG)) m_tabPages[i++] = new CTabLogs;
    m_tabPages[i++] = new CTabSysLibs;
    if (gbExpertMode) m_tabPages[i++] = new CTabCompat;
    if (gbExpertMode) m_tabPages[i++] = new CTabRegistry;
    m_tabPages[i++] = new CTabNotes;
    if (gbExpertMode) m_tabPages[i++] = new CTabOpenGL;
    if (gbExpertMode) m_tabPages[i++] = new CTabSDL;
    if (gbExpertMode) m_tabPages[i++] = new CTabSound;
    if (gbExpertMode) m_tabPages[i++] = new CTabTweaks;
    if (gbExpertMode) m_tabPages[i++] = new CTabIO;
    if (gbDebug) m_tabPages[i++] = new CTabDebug;
    if (gbDebug) m_tabPages[i++] = new CTabHW;
    if (gbExpertMode) m_tabPages[i++] = new CTabLocale;
    m_nNumberOfPages = i;
}

CDXTabCtrl::~CDXTabCtrl() {
    for(int nCount = 0; nCount < m_nNumberOfPages; nCount++)
        delete m_tabPages[nCount];
}

void CDXTabCtrl::Init() {
    //CButton *pBtn;
    //pBtn = (CButton *)this->GetParent()->GetDlgItem(IDTRY);
    //if(pBtn) pBtn->ShowWindow(gbExpertMode ? SW_SHOW : SW_HIDE);
    //pBtn = (CButton *)this->GetParent()->GetDlgItem(IDKILL);
    //if(pBtn) pBtn->ShowWindow(gbExpertMode ? SW_SHOW : SW_HIDE);
    // BEWARE: this part MUST be syncronized with TargetDld initialization in TargetDlg.cpp !!!
    int i = 0;
    m_tabCurrent = 0;
    m_tabPages[i++]->Create(IDD_TAB_PROGRAM, this);
    if (gbExpertMode) m_tabPages[i++]->Create(IDD_TAB_HOOK, this);
    m_tabPages[i++]->Create(IDD_TAB_OUTPUT, this);
    m_tabPages[i++]->Create(IDD_TAB_MOUSE, this);
    if (gbExpertMode) m_tabPages[i++]->Create(IDD_TAB_INPUT, this);
    if (gbExpertMode) m_tabPages[i++]->Create(IDD_TAB_MESSAGES, this);
    m_tabPages[i++]->Create(IDD_TAB_DIRECTX, this);
    m_tabPages[i++]->Create(IDD_TAB_DIRECTX2, this);
    m_tabPages[i++]->Create(IDD_TAB_D3D, this);
    if (gbExpertMode) m_tabPages[i++]->Create(IDD_TAB_TIMING, this);
    if (gbExpertMode && (GetDxWndCaps() & DXWCAPS_CANLOG)) m_tabPages[i++]->Create(IDD_TAB_LOG, this);
    m_tabPages[i++]->Create(IDD_TAB_SYSLIBS, this);
    if (gbExpertMode) m_tabPages[i++]->Create(IDD_TAB_COMPAT, this);
    if (gbExpertMode) m_tabPages[i++]->Create(IDD_TAB_REGISTRY, this);
    m_tabPages[i++]->Create(IDD_TAB_NOTES, this);
    if (gbExpertMode) m_tabPages[i++]->Create(IDD_TAB_OPENGL, this);
    if (gbExpertMode) m_tabPages[i++]->Create(IDD_TAB_SDL, this);
    if (gbExpertMode) m_tabPages[i++]->Create(IDD_TAB_SOUND, this);
    if (gbExpertMode) m_tabPages[i++]->Create(IDD_TAB_TWEAKS, this);
    if (gbExpertMode) m_tabPages[i++]->Create(IDD_TAB_IO, this);
    if (gbDebug) m_tabPages[i++]->Create(IDD_TAB_DEBUG, this);
    if (gbDebug) m_tabPages[i++]->Create(IDD_TAB_HW, this);
    if (gbExpertMode) m_tabPages[i++]->Create(IDD_TAB_LOCALE, this);
    for(int nCount = 0; nCount < m_nNumberOfPages; nCount++)
        m_tabPages[nCount]->ShowWindow(nCount ? SW_HIDE : SW_SHOW);
    SetRectangle();
    // trick to invalidate and redraw initial rect - should be done better than this ....
    SwitchToTab(1);
    SwitchToTab(0);
}

#define DEFAULTDPI 96
#define PANELHEIGHT 424
#define FRAMEHEIGHT 524
#define BUTTONDISPL 20
#define InchToPixelsX(n) (((n)*dpiX)/100)
#define InchToPixelsY(n) (((n)*dpiY)/100)
#define PixelsToInchX(n) (((n)*100)/dpiX)
#define PixelsToInchY(n) (((n)*100)/dpiY)

void CDXTabCtrl::SetRectangle() {
    CRect tabRect, itemRect;
    int nX, nY, nXc, nYc;
    RECT PanelRect, FrameRect;
    CWnd *pWnd;
    // get dpi scaling factors.
    // WARNING: default dpi value for 100% scaling is 96 dpi
    int dpiX = this->GetDC()->GetDeviceCaps(LOGPIXELSX);
    int dpiY = this->GetDC()->GetDeviceCaps(LOGPIXELSY);
#ifdef TRACEVALUES
    {
        char buf[81];
        sprintf(buf, "dpiX=%d dpiY=%d", dpiX, dpiY);
        MessageBox(buf, "dpi", 0);
        sprintf(buf, "scaling=%d%% x %d%%", dpiX * 100 / DEFAULTDPI, dpiY * 100 / DEFAULTDPI);
        MessageBox(buf, "dpi", 0);
    }
#endif
    // get info for tabs height measure.
    // Beware: the value is valid only if following window scaling won't alter the form width.
    // TabHeight & TabWidth expressed in pixels
    GetClientRect(&tabRect);
    GetItemRect(0, &itemRect);
    nX = itemRect.left;
    nY = itemRect.bottom + 1;
    nXc = tabRect.right - itemRect.left - 1;
    nYc = tabRect.bottom - nY - 1 ;
    int TabHeight =  (itemRect.top - tabRect.top);
    int TabWidth =  (itemRect.right - tabRect.left);
#ifdef TRACEVALUES
    {
        char buf[81];
        sprintf(buf, "tab size=(%d x %d)", TabWidth, TabHeight);
        MessageBox(buf, "tab", 0);
    }
#endif
    // resize external frame
    CWnd *Frame = m_tabPages[0]->GetParent()->GetParent();	// Frame = external window
    Frame->GetWindowRect(&FrameRect);
    Frame->SetWindowPos(&wndTop, 0, 0, FrameRect.right - FrameRect.left, (FRAMEHEIGHT * dpiY / 100) + TabHeight, SWP_SHOWWINDOW | SWP_NOMOVE);
    // resize tabs frame (must grow in height)
    pWnd = Frame->GetDlgItem(IDC_TABPANEL);
    pWnd->GetClientRect(&PanelRect);
    pWnd->SetWindowPos(&wndTop, 0, 0, PanelRect.right - PanelRect.left, InchToPixelsY(PANELHEIGHT) + TabHeight, SWP_SHOWWINDOW | SWP_NOMOVE);
    // move buttons
    RECT BtnRect;
    int BtnY = InchToPixelsY(PANELHEIGHT + BUTTONDISPL) + TabHeight;
    pWnd = Frame->GetDlgItem(IDTRY);
    pWnd->GetWindowRect(&BtnRect);
    pWnd->SetWindowPos(&wndTop, BtnRect.left - FrameRect.left, BtnY, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
    pWnd = Frame->GetDlgItem(IDKILL);
    pWnd->GetWindowRect(&BtnRect);
    pWnd->SetWindowPos(&wndTop, BtnRect.left - FrameRect.left, BtnY, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
    pWnd = Frame->GetDlgItem(IDCONTEXTHELP);
    pWnd->GetWindowRect(&BtnRect);
    pWnd->SetWindowPos(&wndTop, BtnRect.left - FrameRect.left, BtnY, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
    pWnd = Frame->GetDlgItem(IDCANCEL);
    pWnd->GetWindowRect(&BtnRect);
    pWnd->SetWindowPos(&wndTop, BtnRect.left - FrameRect.left, BtnY, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
    pWnd = Frame->GetDlgItem(IDOK);
    pWnd->GetWindowRect(&BtnRect);
    pWnd->SetWindowPos(&wndTop, BtnRect.left - FrameRect.left, BtnY, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
    m_tabPages[0]->SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_SHOWWINDOW);
    for(int nCount = 1; nCount < m_nNumberOfPages; nCount++)
        m_tabPages[nCount]->SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_HIDEWINDOW);
}

BEGIN_MESSAGE_MAP(CDXTabCtrl, CTabCtrl)
    //{{AFX_MSG_MAP(CDXTabCtrl)
    ON_WM_LBUTTONDOWN()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDXTabCtrl message handlers

void CDXTabCtrl::OnLButtonDown(UINT nFlags, CPoint point) {
    CTabCtrl::OnLButtonDown(nFlags, point);
    if(m_tabCurrent != GetCurFocus()) {
        m_tabPages[m_tabCurrent]->ShowWindow(SW_HIDE);
        m_tabCurrent = GetCurFocus();
        m_tabPages[m_tabCurrent]->ShowWindow(SW_SHOW);
        m_tabPages[m_tabCurrent]->SetFocus();
    }
}

void CDXTabCtrl::SwitchToTab(int pos) {
    m_tabPages[m_tabCurrent]->ShowWindow(SW_HIDE);
    SetCurSel(pos);
    m_tabPages[pos]->ShowWindow(SW_SHOW);
    m_tabPages[pos]->SetFocus();
    m_tabCurrent = GetCurFocus();
}

void CDXTabCtrl::OnOK() {
    for(int nCount = 0; nCount < m_nNumberOfPages; nCount++)
        m_tabPages[nCount]->UpdateData(TRUE);
}
