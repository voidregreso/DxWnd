// DigitalClock.cpp : implementation file
//

#include "stdafx.h"
#include "DigitalClock.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDigitalClock
//=============================================================================
//
//=============================================================================
CDigitalClock::CDigitalClock() {
    m_digitTID = 12346;
    m_digitElapse = 1000; //ms
    m_pCDAudio = NULL;
    m_colonState = false;
    m_stTime.hour = m_stTime.minute = m_stTime.second = 0;
    SetColour( RGB( 19, 240, 24 ), RGB( 5, 90, 7 ), RGB(0, 0, 0) );
}
//=============================================================================
//
//=============================================================================
CDigitalClock::~CDigitalClock() {
    StopTimer();
}
//=============================================================================
//
//=============================================================================

BEGIN_MESSAGE_MAP(CDigitalClock, CWnd)
    //{{AFX_MSG_MAP(CDigitalClock)
    ON_WM_TIMER()
    ON_WM_ERASEBKGND()
    ON_WM_PAINT()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDigitalClock message handlers
//=============================================================================
// Calculate size and offsets
//=============================================================================
void CDigitalClock::Init( const RECT &rc, const int nXOFF,
                          const int nYOFF, const int nOFF, const int nWidth ) {
    RECT rcDigits;
    memcpy( &rcDigits, &rc, sizeof(RECT) );
    InflateRect( &rcDigits, -nXOFF, -nYOFF );
    //въчисл€ю ширину двух индикаторов с пробелом между ними
    const int nDW = (((rcDigits.right - rcDigits.left) - 9 * nOFF) / 6);
    RECT rcDG[6];
    memset( &rcDG, 0, sizeof(RECT) * 6 );
    rcDG[0].left = rcDigits.left;
    rcDG[0].right = rcDigits.left + nDW;
    rcDG[0].top = rcDigits.top;
    rcDG[0].bottom = rcDigits.bottom;
    rcDG[1].left = rcDigits.left + nDW + nOFF;
    rcDG[1].right = rcDG[1].left + nDW;
    rcDG[1].top = rcDigits.top;
    rcDG[1].bottom = rcDigits.bottom;
    rcDG[2].left = rcDG[1].right + 3 * nOFF;
    rcDG[2].right = rcDG[2].left + nDW;
    rcDG[2].top = rcDigits.top;
    rcDG[2].bottom = rcDigits.bottom;
    rcDG[3].left = rcDG[2].right + nOFF;
    rcDG[3].right = rcDG[3].left + nDW;
    rcDG[3].top = rcDigits.top;
    rcDG[3].bottom = rcDigits.bottom;
    rcDG[4].left = rcDG[3].right + 3 * nOFF;
    rcDG[4].right = rcDG[4].left + nDW;
    rcDG[4].top = rcDigits.top;
    rcDG[4].bottom = rcDigits.bottom;
    rcDG[5].left = rcDG[4].right + nOFF;
    rcDG[5].right = rcDG[5].left + nDW;
    rcDG[5].top = rcDigits.top;
    rcDG[5].bottom = rcDigits.bottom;
    RECT rcCL[2];
    memset( &rcCL, 0, sizeof(RECT) * 2 );
    rcCL[0].left = rcDG[1].right + nOFF;
    rcCL[0].right = rcCL[0].left + nOFF;
    rcCL[0].top = rcDigits.top;
    rcCL[0].bottom = rcDigits.bottom;
    rcCL[1].left = rcDG[3].right + nOFF;
    rcCL[1].right = rcCL[1].left + nOFF;
    rcCL[1].top = rcDigits.top;
    rcCL[1].bottom = rcDigits.bottom;
    m_strClock.hourCL[0].setParameters( rcDG[0], nWidth );
    m_strClock.hourCL[1].setParameters( rcDG[1], nWidth );
    m_strClock.minCL[0].setParameters( rcDG[2], nWidth );
    m_strClock.minCL[1].setParameters( rcDG[3], nWidth );
    m_strClock.secondCL[0].setParameters( rcDG[4], nWidth );
    m_strClock.secondCL[1].setParameters( rcDG[5], nWidth );
    m_strClock.colon[0].setParameters( rcCL[0], nWidth );
    m_strClock.colon[1].setParameters( rcCL[1], nWidth );
}
//=============================================================================
//
//=============================================================================
void CDigitalClock::StartTimer() {
    ::SetTimer( this->m_hWnd, m_digitTID, m_digitElapse, NULL );
}
//=============================================================================
//
//=============================================================================
void CDigitalClock::StopTimer() {
    ::KillTimer( this->m_hWnd, m_digitTID );
}
//=============================================================================
//
//=============================================================================
void CDigitalClock::OnTimer(UINT nIDEvent) {
    if( nIDEvent == m_digitTID ) {
        if( m_pCDAudio ) {
            const int nPos = m_pCDAudio->GetCurrentPos();
            m_stTime.hour = nPos / 3600;
            m_stTime.minute = nPos / 60;
            m_stTime.second = nPos % 60;
        }
        // change the colon color
        m_colonState = !m_colonState;
        // redraw the window
        InvalidateWindow( FALSE );
    }
    CWnd::OnTimer(nIDEvent);
}
//=============================================================================
//
//=============================================================================
void CDigitalClock::ShowTime( HDC dc, const TIME &stTime ) {
    m_strClock.hourCL[0].drawDigit( dc, stTime.hour / 10 );
    m_strClock.hourCL[1].drawDigit( dc, stTime.hour % 10 );
    m_strClock.minCL[0].drawDigit( dc, stTime.minute / 10 );
    m_strClock.minCL[1].drawDigit( dc, stTime.minute % 10 );
    m_strClock.secondCL[0].drawDigit( dc, stTime.second / 10 );
    m_strClock.secondCL[1].drawDigit( dc, stTime.second % 10 );
}
//=============================================================================
//
//=============================================================================
BOOL CDigitalClock::OnEraseBkgnd(CDC *pDC) {
    if( ::IsWindow(this->m_hWnd) ) {
        RECT rcClient;
        ::GetClientRect( this->m_hWnd, &rcClient );
        FillRect( pDC->m_hDC, &rcClient, m_bgndBrush );
    }
    return TRUE;
}
//=============================================================================
//
//=============================================================================
void CDigitalClock::SetBackgroundColor( COLORREF bkgndColor ) {
    m_bgndBrush = ::CreateSolidBrush( bkgndColor );
    InvalidateWindow( TRUE );
}
//=============================================================================
//
//=============================================================================
void CDigitalClock::OnPaint() {
    CPaintDC dc(this); // device context for painting
    m_strClock.colon[0].drawColon( dc.m_hDC, m_colonState );
    m_strClock.colon[1].drawColon( dc.m_hDC, m_colonState );
    ShowTime( dc.m_hDC, m_stTime );
}
//=============================================================================
//
//=============================================================================
void CDigitalClock::SetColour( COLORREF clrActive, COLORREF clrInactive, COLORREF clrBgnd ) {
    m_colorActive = clrActive;
    m_colorInactive = clrInactive;
    m_bgndBrush = ::CreateSolidBrush( clrBgnd );
    m_strClock.hourCL[0].setDigitColor( m_colorActive, m_colorInactive );
    m_strClock.hourCL[1].setDigitColor( m_colorActive, m_colorInactive );
    m_strClock.minCL[0].setDigitColor( m_colorActive, m_colorInactive );
    m_strClock.minCL[1].setDigitColor( m_colorActive, m_colorInactive );
    m_strClock.secondCL[0].setDigitColor( m_colorActive, m_colorInactive );
    m_strClock.secondCL[1].setDigitColor( m_colorActive, m_colorInactive );
    m_strClock.hourCL[0].setLineColor( m_colorActive, m_colorInactive );
    m_strClock.hourCL[1].setLineColor( m_colorActive, m_colorInactive );
    m_strClock.minCL[0].setLineColor( m_colorActive, m_colorInactive );
    m_strClock.minCL[1].setLineColor( m_colorActive, m_colorInactive );
    m_strClock.secondCL[0].setLineColor( m_colorActive, m_colorInactive );
    m_strClock.secondCL[1].setLineColor( m_colorActive, m_colorInactive );
    m_strClock.colon[0].setColonColor( m_colorActive, m_colorInactive );
    m_strClock.colon[0].setLineColor( m_colorActive, m_colorInactive );
    m_strClock.colon[1].setColonColor( m_colorActive, m_colorInactive );
    m_strClock.colon[1].setLineColor( m_colorActive, m_colorInactive );
    InvalidateWindow( FALSE );
}
//=============================================================================
//
//=============================================================================
void CDigitalClock::InvalidateWindow( BOOL bErase ) {
    if( ::IsWindow(this->m_hWnd) ) {
        RECT rcClient;
        ::GetClientRect( this->m_hWnd, &rcClient );
        ::InvalidateRect( this->m_hWnd, &rcClient, bErase );
    }
}
//=============================================================================
//
//=============================================================================
void CDigitalClock::SetTime( const TIME &stTime ) {
    m_stTime.hour	= stTime.hour;
    m_stTime.minute	= stTime.minute;
    m_stTime.second	= stTime.second;
    InvalidateWindow( FALSE );
}
//=============================================================================
//
//=============================================================================
void CDigitalClock::SetDelegate(CCDAudio *pCDAudio) {
    _ASSERT( pCDAudio );
    m_pCDAudio = pCDAudio;
}