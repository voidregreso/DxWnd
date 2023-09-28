// CDPlayerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CDPlayer.h"
#include "CDPlayerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog {
public:
    CAboutDlg();

    // Dialog Data
    //{{AFX_DATA(CAboutDlg)
    enum { IDD = IDD_ABOUTBOX };
    //}}AFX_DATA

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CAboutDlg)
protected:
    virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

    // Implementation
protected:
    //{{AFX_MSG(CAboutDlg)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD) {
    //{{AFX_DATA_INIT(CAboutDlg)
    //}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAboutDlg)
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
    //{{AFX_MSG_MAP(CAboutDlg)
    // No message handlers
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCDPlayerDlg dialog

CCDPlayerDlg::CCDPlayerDlg(CWnd *pParent /*=NULL*/)
    : CDialog(CCDPlayerDlg::IDD, pParent) {
    //{{AFX_DATA_INIT(CCDPlayerDlg)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
    // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_volumeTID = 12347;
    m_volumeElapse = 500; //ms
}

void CCDPlayerDlg::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CCDPlayerDlg)
    // NOTE: the ClassWizard will add DDX and DDV calls here
    //}}AFX_DATA_MAP
    DDX_Control(pDX, IDB_CD_EJECT, m_btnEject);
    DDX_Control(pDX, IDB_STOP, m_btnStop);
    DDX_Control(pDX, ID_HELP, m_btnHelp);
    DDX_Control(pDX, IDB_FORWARD, m_btnNext);
    DDX_Control(pDX, IDB_PLAY, m_btnPlay);
    DDX_Control(pDX, IDB_BACKWARD, m_btnPrevious);
    DDX_Control(pDX, IDB_EXIT, m_btnExit);
    DDX_Control(pDX, IDB_PAUSE, m_btnPause);
    DDX_Control(pDX, IDB_TRACK_LIST, m_btnTrackList);
    DDX_Slider (pDX, IDC_VOLUMESLIDER, i_Volume);
}

BEGIN_MESSAGE_MAP(CCDPlayerDlg, CDialog)
    //{{AFX_MSG_MAP(CCDPlayerDlg)
    ON_WM_SYSCOMMAND()
    ON_WM_DESTROY()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    //}}AFX_MSG_MAP
    ON_WM_CTLCOLOR()
    ON_BN_CLICKED(IDB_PLAY, OnBnClickedPlay)
    ON_BN_CLICKED(IDB_PAUSE, OnBnClickedPause)
    ON_BN_CLICKED(IDB_STOP, OnBnClickedStop)
    ON_BN_CLICKED(IDB_BACKWARD, OnBnClickedBackward)
    ON_BN_CLICKED(IDB_FORWARD, OnBnClickedForward)
    ON_BN_CLICKED(IDB_CD_EJECT, OnBnClickedCdEject)
    ON_BN_CLICKED(IDB_EXIT, OnBnClickedExit)
    ON_BN_CLICKED(IDB_TRACK_LIST, OnBnClickedTrackList)
    ON_COMMAND_RANGE((WM_USER + 1), (WM_USER + 100), OnTrackChange )
    ON_WM_TIMER()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCDPlayerDlg message handlers

BOOL CCDPlayerDlg::OnInitDialog() {
    CDialog::OnInitDialog();
    // Add "About..." menu item to system menu.
    // IDM_ABOUTBOX must be in the system command range.
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);
    CMenu *pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL) {
        CString strAboutMenu;
        strAboutMenu.LoadString(IDS_ABOUTBOX);
        if (!strAboutMenu.IsEmpty()) {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }
    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);			// Set big icon
    SetIcon(m_hIcon, FALSE);		// Set small icon
    // init controls
    RECT rect;
    ::GetClientRect( this->m_hWnd, &rect );
    ::InflateRect( &rect, -16, -15 );
    rect.right = (rect.left + 160);
    // init timer
    ::SetTimer( this->m_hWnd, m_volumeTID, m_volumeElapse, NULL );
    m_LastVolume = -1; // impossible value
    LPCTSTR szMyClass = NULL;
    try {
        szMyClass = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW,
                                        ::LoadCursor(NULL, IDC_ARROW), (HBRUSH) ::GetStockObject(BLACK_BRUSH));
    } catch (CResourceException *pEx) {
        AfxMessageBox(_T("Couldn't register class! (Already registered?)"));
        pEx->Delete();
    }
    BOOL bRes = m_DClock.CreateEx(WS_EX_CLIENTEDGE,	szMyClass, "DC",
                                  WS_CHILD | WS_VISIBLE, rect, this, (UINT)1234);
    ::GetClientRect( m_DClock.m_hWnd, &rect );
    m_DClock.Init( rect, 5, 15, 5, 1 );
    m_DClock.SetDelegate( &m_CDAudio );
    m_DClock.StartTimer();
    m_VolumeSlider.SetRange(0, 100, 0);
    m_VolumeSlider.SetPos(50);
    // setup the tooltips
    //RECT rect;
    m_ToolTip.Create( this, TTS_ALWAYSTIP );
    m_btnEject.GetClientRect( &rect );
    m_ToolTip.AddTool( &m_btnEject, IDB_CD_EJECT, &rect, IDB_CD_EJECT );
    m_btnStop.GetClientRect( &rect );
    m_ToolTip.AddTool( &m_btnStop, IDB_STOP, &rect, IDB_STOP );
    m_btnHelp.GetClientRect( &rect );
    m_ToolTip.AddTool( &m_btnHelp, ID_HELP, &rect, ID_HELP );
    m_btnPrevious.GetClientRect( &rect );
    m_ToolTip.AddTool( &m_btnPrevious, IDB_BACKWARD, &rect, IDB_BACKWARD );
    m_btnPlay.GetClientRect( &rect );
    m_ToolTip.AddTool( &m_btnPlay, IDB_PLAY, &rect, IDB_PLAY );
    m_btnNext.GetClientRect( &rect );
    m_ToolTip.AddTool( &m_btnNext, IDB_FORWARD, &rect, IDB_FORWARD );
    m_btnExit.GetClientRect( &rect );
    m_ToolTip.AddTool( &m_btnExit, IDB_EXIT, &rect, IDB_EXIT );
    m_btnPause.GetClientRect( &rect );
    m_ToolTip.AddTool( &m_btnPause, IDB_PAUSE, &rect, IDB_PAUSE );
    m_btnTrackList.GetClientRect( &rect );
    m_ToolTip.AddTool( &m_btnTrackList, IDB_TRACK_LIST, &rect, IDB_TRACK_LIST );
    m_ToolTip.Activate(TRUE);
    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CCDPlayerDlg::OnSysCommand(UINT nID, LPARAM lParam) {
    if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    } else
        CDialog::OnSysCommand(nID, lParam);
}

void CCDPlayerDlg::OnDestroy() {
    ::KillTimer( this->m_hWnd, m_volumeTID );
    WinHelp(0L, HELP_QUIT);
    CDialog::OnDestroy();
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCDPlayerDlg::OnPaint() {
    if (IsIconic()) {
        CPaintDC dc(this); // device context for painting
        SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);
        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;
        // Draw the icon
        dc.DrawIcon(x, y, m_hIcon);
    } else
        CDialog::OnPaint();
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCDPlayerDlg::OnQueryDragIcon() {
    return (HCURSOR) m_hIcon;
}

HBRUSH CCDPlayerDlg::OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor) {
    HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
    switch( nCtlColor ) {
    case CTLCOLOR_DLG: {
        hbr = ::CreateSolidBrush( RGB(0, 0, 0) );
    }
    break;
    }
    // TODO:  Return a different brush if the default is not desired
    return hbr;
}

void CCDPlayerDlg::OnBnClickedPlay() {
    m_CDAudio.Play();
}

void CCDPlayerDlg::OnBnClickedPause() {
    m_CDAudio.Pause();
}

void CCDPlayerDlg::OnBnClickedStop() {
    m_CDAudio.Stop();
}

void CCDPlayerDlg::OnBnClickedBackward() {
    m_CDAudio.Backward();
}

void CCDPlayerDlg::OnBnClickedForward() {
    m_CDAudio.Forward();
}

void CCDPlayerDlg::OnBnClickedCdEject() {
    m_CDAudio.EjectCDROM();
}

void CCDPlayerDlg::OnBnClickedExit() {
    CDialog::OnOK();
}

void CCDPlayerDlg::OnBnClickedTrackList() {
    if( m_CDAudio.IsReady() && m_CDAudio.IsMediaInsert() ) {
        CWnd *pBtn = GetDlgItem( IDB_TRACK_LIST );
        _ASSERT(pBtn);
        CRect rect;
        pBtn->GetWindowRect( rect );
        CMenu menu;
        if(menu.CreatePopupMenu()) {
            DWORD dwFlags = MF_ENABLED | MF_STRING;
            TCHAR szTrack[32];
            lstrcpy( szTrack, _T("Track ") );
            const int nTracks = m_CDAudio.GetTracksCount();
            for( int nTrack = 1; nTrack <= nTracks; nTrack++ ) {
                const int nLength = m_CDAudio.GetTrackLength( nTrack );
                _stprintf( szTrack, _T("Track %d\t%02d:%02d"), nTrack, (nLength / 60), (nLength % 60) );
                menu.AppendMenu(dwFlags, (WM_USER + nTrack), szTrack);
            }
            menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, rect.right, rect.top, this);
        }
    }
}

void CCDPlayerDlg::OnTrackChange(UINT nID) {
    const int nTrack = (nID - WM_USER);
    const int nCurrTrack = m_CDAudio.GetCurrentTrack();
    if( nCurrTrack == nTrack )
        return;
    m_CDAudio.Play( m_CDAudio.GetTrackBeginTime( nTrack ) );
}


BOOL CCDPlayerDlg::PreTranslateMessage(MSG *pMsg) {
    if( m_ToolTip.GetSafeHwnd() )
        m_ToolTip.RelayEvent( pMsg );
    return CDialog::PreTranslateMessage(pMsg);
}

void CCDPlayerDlg::OnTimer(UINT nIDEvent) {
    CSliderCtrl *Slider;
    Slider = (CSliderCtrl *)this->GetDlgItem(IDC_VOLUMESLIDER);
    // subtract pos from 100 to have growing values bottom to up
    i_Volume = 100 - Slider->GetPos();
    if(i_Volume != m_LastVolume) {
        m_LastVolume = i_Volume;
        m_CDAudio.SetVolume(i_Volume);
    }
}