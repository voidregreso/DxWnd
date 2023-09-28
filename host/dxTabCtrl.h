#if !defined(AFX_MYTABCTRL_H__F3E8650F_019C_479F_9E0F_60FE1181F49F__INCLUDED_)
#define AFX_MYTABCTRL_H__F3E8650F_019C_479F_9E0F_60FE1181F49F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dxTabCtrl.h : header file
//
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CDXTabCtrl window

class CDXTabCtrl : public CTabCtrl {
    // Construction
public:
    CDXTabCtrl();
    virtual ~CDXTabCtrl();

    void Init();
    void SetRectangle();
    void SwitchToTab(int);
    void OnOK();

    CDialog *m_tabPages[23]; // this must match the total number of tab elements!
    char *m_tabHelpers[23]; // this must match the total number of tab elements!
    int m_tabCurrent;
    int m_nNumberOfPages;
    enum { IDD = IDC_TABPANEL };

    // Generated message map functions
protected:
    //{{AFX_MSG(CDXTabCtrl)
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
