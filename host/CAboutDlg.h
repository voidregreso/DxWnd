/////////////////////////////////////////////////////////////////////////////
// Used in application version information dialog CAboutDlg

class CAboutDlg : public CDialog {
public:
    CAboutDlg();
    virtual BOOL OnInitDialog();

    // Data Dialog
    //{{AFX_DATA(CAboutDlg)
    enum { IDD = IDD_ABOUTBOX };
    CString	m_Version;
    CString m_DxwPlayVersion;
    CString	m_Thanks;
    CPictureEx m_Picture;
    //}}AFX_DATA

    // ClassWizard generated virtual function overrides.
    //{{AFX_VIRTUAL(CAboutDlg)
protected:
    virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV Support
    virtual void OnTimer(UINT_PTR);
    //}}AFX_VIRTUAL

    // Implementation
protected:
    //{{AFX_MSG(CAboutDlg)
    // There is no message handler.
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
public:
    CPictureEx m_Animation;
    afx_msg void OnStnClickedAnimation();
};

#define ID_HELP_SCROLL 999
#define ID_HELP_SCROLL2 998
