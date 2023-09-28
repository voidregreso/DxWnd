#pragma once
#include "bmpext.h"


// CGammaRampDialog dialog

class CGammaRampDialog : public CDialog {
    DECLARE_DYNAMIC(CGammaRampDialog)

public:
    CGammaRampDialog(void);   // standard constructor
    virtual ~CGammaRampDialog();

    // Dialog Data
    enum { IDD = IDD_GAMMARAMP };
    int iTimeSlider;

protected:
    virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
protected:
    virtual void OnOK();
private:
    char *ExePath;
    CDib dib_Gamma;
};
