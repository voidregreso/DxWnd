#pragma once


// CShimsDialog dialog

class CCDPlayerDialog : public CDialog {
    DECLARE_DYNAMIC(CCDPlayerDialog)

public:
    CCDPlayerDialog(void);   // standard constructor
    virtual ~CCDPlayerDialog();

    // Dialog Data
    enum { IDD = IDD_CDPLAYER };

protected:
    virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
protected:
    virtual void OnOK();
private:
};
