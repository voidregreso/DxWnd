#if !defined(AFX_TABSDL_H__798A9124_C906_446C_822D_322B5AB6C4C4__INCLUDED_)
#define AFX_TABSDL_H__798A9124_C906_446C_822D_322B5AB6C4C4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabSDL.h : header file
//
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CTabSDL dialog

class CTabSDL : public CDialog {
    // Construction
public:
    CTabSDL(CWnd *pParent = NULL);   // standard constructor
    //BOOL OnInitDialog();

private:
    BOOL PreTranslateMessage(MSG *);

    // Dialog Data
    //{{AFX_DATA(CTabSDL)
    enum { IDD = IDD_TAB_SDL };
    // NOTE: the ClassWizard will add data members here
    //}}AFX_DATA


    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CTabSDL)
protected:
    virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

    // Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CTabSDL)
    // NOTE: the ClassWizard will add member functions here
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif