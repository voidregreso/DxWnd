#if !defined(AFX_TABGAMMA_H__798A9124_C906_446C_822D_322B5AB6C4C4__INCLUDED_)
#define AFX_TABGAMMA_H__798A9124_C906_446C_822D_322B5AB6C4C4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabGammaCtrl.h : header file
//
#include "resource.h"
#include "bmpext.h"

/////////////////////////////////////////////////////////////////////////////
// CTabDirectX dialog

class CTabGammaCtrl : public CDialog {
    // Construction
public:
    CTabGammaCtrl(CWnd *pParent = NULL);   // standard constructor
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    BOOL OnInitDialog();

    // Dialog Data
    //{{AFX_DATA(CTabDirectX)
    enum { IDD = IDD_GAMMACONTROL };
    // NOTE: the ClassWizard will add data members here
    //}}AFX_DATA


    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CTabDirectX)
protected:
    virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
    virtual void OnOK();
    virtual void OnGammaReset();
    virtual void OnGammaSave();
    virtual void OnGammaLoad();
    //}}AFX_VIRTUAL

    // Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CTabDirectX)
    // NOTE: the ClassWizard will add member functions here
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
    int i_RGamma;
    int i_GGamma;
    int i_BGamma;
    BOOL b_Enabled;
    BOOL b_Reversed;
    BOOL b_Locked;
    HWND hWinActive;
    CDib dib_Gamma;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABTHREE_H__798A9124_C906_446C_822D_322B5AB6C4C4__INCLUDED_)
