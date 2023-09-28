// CodePageMFCSampleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CodePageMFCSample.h"
#include "CodePageMFCSampleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog {
public:
    CAboutDlg();

    // Dialog Data
    enum { IDD = IDD_ABOUTBOX };

protected:
    virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support

    // Implementation
protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD) {
}

void CAboutDlg::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CCodePageDlg dialog




CCodePageDlg::CCodePageDlg(CWnd *pParent /*=NULL*/)
    : CDialog(CCodePageDlg::IDD, pParent) {
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCodePageDlg::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_INSTALLED, m_wndInstalled);
    DDX_Control(pDX, IDC_SUPPORTED, m_wndSupported);
}

BEGIN_MESSAGE_MAP(CCodePageDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CCodePageDlg message handlers

BOOL CCodePageDlg::OnInitDialog() {
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
    // Initialise list controls
    rec::CCodePage objCodePages;
    InitListControl(m_wndInstalled, objCodePages.GetInstalled());
    InitListControl(m_wndSupported, objCodePages.GetSupported());
    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CCodePageDlg::OnSysCommand(UINT nID, LPARAM lParam) {
    if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    } else
        CDialog::OnSysCommand(nID, lParam);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCodePageDlg::OnPaint() {
    if (IsIconic()) {
        CPaintDC dc(this); // device context for painting
        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
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

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCodePageDlg::OnQueryDragIcon() {
    return static_cast<HCURSOR>(m_hIcon);
}

void CCodePageDlg::InitListControl(CListCtrl &listControl, const rec::CCodePage::CCodePageObject &cpObject) {
    // Full row select looks best
    listControl.SetExtendedStyle(listControl.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
    // Get the control width
    CRect rect;
    listControl.GetClientRect(&rect);
    // Take vertical scroll bar into account
    unsigned int nColWidth = (rect.Width() - ::GetSystemMetrics(SM_CXVSCROLL)) / 2;
    // Add columns
    listControl.InsertColumn(0, _T("Code Page"), LVCFMT_LEFT, nColWidth);
    listControl.InsertColumn(1, _T("Name"), LVCFMT_LEFT, nColWidth);
    // Add each code page
    for (int  i = 0; i < cpObject.GetCount(); i++) {
        // ID
        CString strCodePageID;
        strCodePageID.Format(_T("%u"), cpObject[i]);
        int nIndex = listControl.InsertItem(i, strCodePageID);
        // Name
        listControl.SetItemText(nIndex, 1, cpObject.GetName(i));
        // Store the code page ID as the item data ready for sorting the list
        listControl.SetItemData(nIndex, cpObject[i]);
    }
    listControl.SortItems(CompareFunc, 0);
}

int CALLBACK CCodePageDlg::CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM /*lParamSort*/) {
    // Sort in ascending order of code page ID
    return lParam1 > lParam2;
}