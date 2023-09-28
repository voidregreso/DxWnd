#pragma once

// CGlobalStartUp dialog

class CGlobalStartUp : public CDialog {
    DECLARE_DYNAMIC(CGlobalStartUp)

public:
    CGlobalStartUp(CWnd *pParent = NULL);   // standard constructor
    virtual ~CGlobalStartUp();

    // Dialog Data
    enum { IDD = IDD_GLOBAL_STARTUP };

protected:
    virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()
public:
    BOOL m_RunAtStartUp;
    BOOL m_PreviousSetting;
    BOOL m_HasAdminCaps;
    BOOL m_StartInTray;
    BOOL m_StartIdle;
    virtual BOOL OnInitDialog();
protected:
    virtual void OnOK();
    virtual void OnCancel();
    virtual void OnHelp();
};
