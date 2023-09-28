#include "stdafx.h"
#include "dxTabConfCtrl.h"
#include "TabConfFlags.h"
#include "TabConfKeys.h"
#include "TabConfDefaults.h"

CDXTabConfCtrl::CDXTabConfCtrl() {
    int i;
    //MessageBox("CDXTabConfCtrl:CDXTabConfCtrl","trace",0);
    // BEWARE: this part MUST be syncronized with TargetConfDlg initialization in GlobalSettings.cpp !!!
    i = 0;
    //char sCaption[48+1];
    ////LoadString(AfxGetResourceHandle(), DXW_TAB_MAIN, sCaption, sizeof(sCaption));
    //InsertItem(i++, _T("Flags"));
    ////LoadString(AfxGetResourceHandle(), DXW_TAB_HOOK, sCaption, sizeof(sCaption));
    //InsertItem(i++, _T("Hotkeys"));
    ////LoadString(AfxGetResourceHandle(), DXW_TAB_VIDEO, sCaption, sizeof(sCaption));
    //InsertItem(i++, _T("Defaults"));
    // build helpers strings - temporarily identical
    i = 0;
    m_tabHelpers[i++] = "config";
    m_tabHelpers[i++] = "config";
    m_tabHelpers[i++] = "config";
    // build tab control pages
    i = 0;
    m_tabPages[i++] = new CTabConfFlags;
    m_tabPages[i++] = new CTabConfKeys;
    m_tabPages[i++] = new CTabConfDefaults;
    m_nNumberOfPages = i;
}

CDXTabConfCtrl::~CDXTabConfCtrl() {
    for(int nCount = 0; nCount < m_nNumberOfPages; nCount++)
        delete m_tabPages[nCount];
}

void CDXTabConfCtrl::Init() {
    int i = 0;
    m_tabCurrent = 0;
    //char sCaption[48+1];
    //LoadString(AfxGetResourceHandle(), DXW_TAB_MAIN, sCaption, sizeof(sCaption));
    InsertItem(i++, _T("Flags"));
    //LoadString(AfxGetResourceHandle(), DXW_TAB_HOOK, sCaption, sizeof(sCaption));
    InsertItem(i++, _T("Hotkeys"));
    //LoadString(AfxGetResourceHandle(), DXW_TAB_VIDEO, sCaption, sizeof(sCaption));
    InsertItem(i++, _T("Defaults"));
    i = 0;
    m_tabPages[i++]->Create(IDD_CONFTAB_FLAGS, this);
    m_tabPages[i++]->Create(IDD_CONFTAB_HOTKEYS, this);
    m_tabPages[i++]->Create(IDD_CONFTAB_DEFAULTS, this);
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

void CDXTabConfCtrl::SetRectangle() {
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
    pWnd = Frame->GetDlgItem(IDC_CONF_TABPANEL);
    pWnd->GetClientRect(&PanelRect);
    pWnd->SetWindowPos(&wndTop, 0, 0, PanelRect.right - PanelRect.left, InchToPixelsY(PANELHEIGHT) + TabHeight, SWP_SHOWWINDOW | SWP_NOMOVE);
    // move buttons
    RECT BtnRect;
    int BtnY = InchToPixelsY(PANELHEIGHT + BUTTONDISPL) + TabHeight;
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

BEGIN_MESSAGE_MAP(CDXTabConfCtrl, CTabCtrl)
    //{{AFX_MSG_MAP(CDXTabCtrl)
    ON_WM_LBUTTONDOWN()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDXTabConfCtrl message handlers

void CDXTabConfCtrl::OnLButtonDown(UINT nFlags, CPoint point) {
    CTabCtrl::OnLButtonDown(nFlags, point);
    if(m_tabCurrent != GetCurFocus()) {
        m_tabPages[m_tabCurrent]->ShowWindow(SW_HIDE);
        m_tabCurrent = GetCurFocus();
        m_tabPages[m_tabCurrent]->ShowWindow(SW_SHOW);
        m_tabPages[m_tabCurrent]->SetFocus();
    }
}

void CDXTabConfCtrl::SwitchToTab(int pos) {
    m_tabPages[m_tabCurrent]->ShowWindow(SW_HIDE);
    SetCurSel(pos);
    m_tabPages[pos]->ShowWindow(SW_SHOW);
    m_tabPages[pos]->SetFocus();
    m_tabCurrent = GetCurFocus();
}

void CDXTabConfCtrl::OnOK() {
    //MessageBox("CDXTabConfCtrl:OnOK","trace",0);
    for(int nCount = 0; nCount < m_nNumberOfPages; nCount++)
        m_tabPages[nCount]->UpdateData(TRUE);
}

//BEGIN_MESSAGE_MAP(CDXTabConfCtrl, CTabCtrl)
//	//{{AFX_MSG_MAP(CDXTabCtrl)
//	ON_WM_LBUTTONDOWN()
//	//}}AFX_MSG_MAP
//END_MESSAGE_MAP()
