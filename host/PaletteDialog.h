#pragma once
#include "bmpext.h"

// CPaletteDialog dialog

class CPaletteDialog : public CDialog {
    DECLARE_DYNAMIC(CPaletteDialog)

public:
    CPaletteDialog(CWnd *pParent = NULL);   // standard constructor
    virtual ~CPaletteDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    virtual BOOL OnInitDialog();
    enum { IDD = IDD_PALETTE };

protected:
    virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
    virtual void OnOK();

    DECLARE_MESSAGE_MAP()

private:
    CDib dib_Palette;
    CDib dib_Pixel;
    CWnd *myWin;
};
