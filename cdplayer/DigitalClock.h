#if !defined(AFX_DIGITALCLOCK_H__1E750CCC_7E13_4943_924E_993ECD703C65__INCLUDED_)
#define AFX_DIGITALCLOCK_H__1E750CCC_7E13_4943_924E_993ECD703C65__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DigitalClock.h : header file
//
#include "Digit.h"
#include "Colon.h"

#include "CDAudio.h"

typedef struct _TIME_ {
    BYTE hour;
    BYTE minute;
    BYTE second;
} TIME;

/////////////////////////////////////////////////////////////////////////////
// CDigitalClock window
class CDigitalClock : public CWnd {
    // Construction
public:
    CDigitalClock();

    // Attributes
public:

    // Operations
public:

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CDigitalClock)
    //}}AFX_VIRTUAL

    // Implementation
public:
    virtual ~CDigitalClock();

    // Generated message map functions
protected:
    //{{AFX_MSG(CDigitalClock)
    afx_msg void OnTimer(UINT nIDEvent);
    afx_msg BOOL OnEraseBkgnd(CDC *pDC);
    afx_msg void OnPaint();
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()

public:

    void StartTimer();
    void StopTimer();

    void Init( const RECT &rc, const int nXOFF,
               const int nYOFF, const int nOFF, const int nWidth );

    void SetBackgroundColor( COLORREF bkgndColor );
    void SetColour( COLORREF clrActive, COLORREF clrInactive, COLORREF clrBgnd );

    void SetTime( const TIME &stTime );
    void SetDelegate(CCDAudio *pCDAudio);
protected:

    void ShowTime( HDC dc, const TIME &stTime );
    inline void InvalidateWindow( BOOL bErase );

protected:

    struct Clock {
        CDigit hourCL[2];
        CDigit minCL[2];
        CDigit secondCL[2];
        CColon colon[2];
    };

    UINT m_digitTID;
    UINT m_digitElapse;

    Clock m_strClock;

    HBRUSH m_bgndBrush;
    bool m_colonState;
    COLORREF m_colorActive;
    COLORREF m_colorInactive;

    TIME m_stTime;

    CCDAudio *m_pCDAudio;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIGITALCLOCK_H__1E750CCC_7E13_4943_924E_993ECD703C65__INCLUDED_)
