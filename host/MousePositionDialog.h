#pragma once
#include "bmpext.h"

// CMousePositionDialog dialog

class CMousePositionDialog : public CDialog {
    DECLARE_DYNAMIC(CMousePositionDialog)

public:
    CMousePositionDialog(void);   // standard constructor
    virtual ~CMousePositionDialog();

    // Dialog Data
    enum { IDD = IDD_MOUSEPOSITION };

    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
protected:
    virtual void OnOK();
    virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
    DECLARE_MESSAGE_MAP()
private:
    int iCount;
    CDib dib_Background;
    CImageList m_imageList;
};
