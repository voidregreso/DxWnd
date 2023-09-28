#if !defined(AFX_TABSDL_H__798A9124_C906_446C_822D_322B5AB6C4C5__INCLUDED_)
#define AFX_TABSDL_H__798A9124_C906_446C_822D_322B5AB6C4C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabSound.h : header file
//
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CTabSound dialog

class CTabSound : public CDialog {
    // Construction
public:
    CTabSound(CWnd *pParent = NULL);   // standard constructor
    //BOOL OnInitDialog();

private:
    BOOL PreTranslateMessage(MSG *);

    // Dialog Data
    //{{AFX_DATA(CTabSound)
    enum { IDD = IDD_TAB_SOUND };
    // NOTE: the ClassWizard will add data members here
    //}}AFX_DATA


    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CTabSound)
protected:
    virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    //}}AFX_VIRTUAL

    // Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CTabSound)
    // NOTE: the ClassWizard will add member functions here
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif