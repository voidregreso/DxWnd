
#include "stdafx.h"
#include "Digit.h"


//-----------------------------------------------------------------------------
// constructor
CDigit::CDigit() {
    memset( m_ptDG, 0, sizeof(POINT) * 42 );
    m_iWide = 0;
    m_activeBrush = CreateSolidBrush( RGB(0, 0, 0) );
    m_inactiveBrush = CreateSolidBrush( RGB(0, 0, 0) );
    m_hActPen = CreatePen( PS_SOLID, 1, RGB(0, 0, 0) );
    m_hInactPen = CreatePen( PS_SOLID, 1, RGB(0, 0, 0) );
}
//-----------------------------------------------------------------------------
// constructor
CDigit::CDigit( const RECT &rc, const int dgWide, COLORREF actColor, COLORREF inactColor,
                COLORREF actLineColor, COLORREF inactLineColor ) {
    setParameters( rc, dgWide );
    m_activeBrush = CreateSolidBrush( actColor );
    m_inactiveBrush = CreateSolidBrush( inactColor );
    m_hActPen = CreatePen( PS_SOLID, 1, actLineColor );
    m_hInactPen = CreatePen( PS_SOLID, 1, inactLineColor );
    m_iWide = dgWide;
}
//-----------------------------------------------------------------------------
CDigit::~CDigit() {
    DeleteObject( m_activeBrush );
    DeleteObject( m_inactiveBrush );
    DeleteObject( m_hActPen );
    DeleteObject( m_hInactPen );
}
//-----------------------------------------------------------------------------

// calculate the position and size of segments
// dgWide - the digit wide
// rc - control rectangle
void CDigit::setParameters( const RECT &rc, const int dgWide ) {
    int nMiddle = (rc.top + (rc.bottom - rc.top) / 2);
    RECT rcLev1, rcLev2;
    memcpy( &rcLev1, &rc, sizeof(RECT) );
    InflateRect( &rcLev1, -dgWide, -dgWide );
    memcpy( &rcLev2, &rcLev1, sizeof(RECT) );
    InflateRect( &rcLev2, -dgWide, -dgWide );
    // UET
    m_ptDG[UET][0].x = rcLev1.left;
    m_ptDG[UET][0].y = rcLev1.top;
    m_ptDG[UET][1].x = rcLev2.left;
    m_ptDG[UET][1].y = rc.top;
    m_ptDG[UET][2].x = rcLev2.right;
    m_ptDG[UET][2].y = rc.top;
    m_ptDG[UET][3].x = rcLev1.right;
    m_ptDG[UET][3].y = rcLev1.top;
    m_ptDG[UET][4].x = rcLev2.right;
    m_ptDG[UET][4].y = rcLev2.top;
    m_ptDG[UET][5].x = rcLev2.left;
    m_ptDG[UET][5].y = rcLev2.top;
    m_ptDG[UET][0].x += SEGMARGIN;
    m_ptDG[UET][1].x += SEGMARGIN;
    m_ptDG[UET][2].x -= SEGMARGIN;
    m_ptDG[UET][3].x -= SEGMARGIN;
    m_ptDG[UET][4].x -= SEGMARGIN;
    m_ptDG[UET][5].x += SEGMARGIN;
    // RUET
    m_ptDG[RUET][0].x = rcLev1.right;
    m_ptDG[RUET][0].y = rcLev1.top;
    m_ptDG[RUET][1].x = rc.right;
    m_ptDG[RUET][1].y = rcLev2.top;
    m_ptDG[RUET][2].x = rc.right;
    m_ptDG[RUET][2].y = (nMiddle - dgWide);
    m_ptDG[RUET][3].x = rcLev1.right;
    m_ptDG[RUET][3].y = nMiddle;
    m_ptDG[RUET][4].x = rcLev2.right;
    m_ptDG[RUET][4].y = (nMiddle - dgWide);
    m_ptDG[RUET][5].x = rcLev2.right;
    m_ptDG[RUET][5].y = rcLev2.top;
    m_ptDG[RUET][0].y += SEGMARGIN;
    m_ptDG[RUET][1].y += SEGMARGIN;
    m_ptDG[RUET][2].y -= SEGMARGIN;
    m_ptDG[RUET][3].y -= SEGMARGIN;
    m_ptDG[RUET][4].y -= SEGMARGIN;
    m_ptDG[RUET][5].y += SEGMARGIN;
    // RDET
    m_ptDG[RDET][0].x = rcLev1.right;
    m_ptDG[RDET][0].y = nMiddle;
    m_ptDG[RDET][1].x = rc.right;
    m_ptDG[RDET][1].y = (nMiddle + dgWide);
    m_ptDG[RDET][2].x = rc.right;
    m_ptDG[RDET][2].y = rcLev2.bottom;
    m_ptDG[RDET][3].x = rcLev1.right;
    m_ptDG[RDET][3].y = rcLev1.bottom;
    m_ptDG[RDET][4].x = rcLev2.right;
    m_ptDG[RDET][4].y = rcLev2.bottom;
    m_ptDG[RDET][5].x = rcLev2.right;
    m_ptDG[RDET][5].y = (nMiddle + dgWide);
    m_ptDG[RDET][0].y += SEGMARGIN;
    m_ptDG[RDET][1].y += SEGMARGIN;
    m_ptDG[RDET][2].y -= SEGMARGIN;
    m_ptDG[RDET][3].y -= SEGMARGIN;
    m_ptDG[RDET][4].y -= SEGMARGIN;
    m_ptDG[RDET][5].y += SEGMARGIN;
    // DET
    m_ptDG[DET][0].x = rcLev1.left;
    m_ptDG[DET][0].y = rcLev1.bottom;
    m_ptDG[DET][1].x = rcLev2.left;
    m_ptDG[DET][1].y = rcLev2.bottom;
    m_ptDG[DET][2].x = rcLev2.right;
    m_ptDG[DET][2].y = rcLev2.bottom;
    m_ptDG[DET][3].x = rcLev1.right;
    m_ptDG[DET][3].y = rcLev1.bottom;
    m_ptDG[DET][4].x = rcLev2.right;
    m_ptDG[DET][4].y = rc.bottom;
    m_ptDG[DET][5].x = rcLev2.left;
    m_ptDG[DET][5].y = rc.bottom;
    m_ptDG[DET][0].x += SEGMARGIN;
    m_ptDG[DET][1].x += SEGMARGIN;
    m_ptDG[DET][2].x -= SEGMARGIN;
    m_ptDG[DET][3].x -= SEGMARGIN;
    m_ptDG[DET][4].x -= SEGMARGIN;
    m_ptDG[DET][5].x += SEGMARGIN;
    // LDET
    m_ptDG[LDET][0].x = rcLev1.left;
    m_ptDG[LDET][0].y = nMiddle;
    m_ptDG[LDET][1].x = rcLev2.left;
    m_ptDG[LDET][1].y = (nMiddle + dgWide);
    m_ptDG[LDET][2].x = rcLev2.left;
    m_ptDG[LDET][2].y = rcLev2.bottom;
    m_ptDG[LDET][3].x = rcLev1.left;
    m_ptDG[LDET][3].y = rcLev1.bottom;
    m_ptDG[LDET][4].x = rc.left;
    m_ptDG[LDET][4].y = rcLev2.bottom;
    m_ptDG[LDET][5].x = rc.left;
    m_ptDG[LDET][5].y = (nMiddle + dgWide);
    m_ptDG[LDET][0].y += SEGMARGIN;
    m_ptDG[LDET][1].y += SEGMARGIN;
    m_ptDG[LDET][2].y -= SEGMARGIN;
    m_ptDG[LDET][3].y -= SEGMARGIN;
    m_ptDG[LDET][4].y -= SEGMARGIN;
    m_ptDG[LDET][5].y += SEGMARGIN;
    // LUET
    m_ptDG[LUET][0].x = rcLev1.left;
    m_ptDG[LUET][0].y = rcLev1.top;
    m_ptDG[LUET][1].x = rcLev2.left;
    m_ptDG[LUET][1].y = rcLev2.top;
    m_ptDG[LUET][2].x = rcLev2.left;
    m_ptDG[LUET][2].y = (nMiddle - dgWide);
    m_ptDG[LUET][3].x = rcLev1.left;
    m_ptDG[LUET][3].y = nMiddle;
    m_ptDG[LUET][4].x = rc.left;
    m_ptDG[LUET][4].y = (nMiddle - dgWide);
    m_ptDG[LUET][5].x = rc.left;
    m_ptDG[LUET][5].y = rcLev2.top;
    m_ptDG[LUET][0].y += SEGMARGIN;
    m_ptDG[LUET][1].y += SEGMARGIN;
    m_ptDG[LUET][2].y -= SEGMARGIN;
    m_ptDG[LUET][3].y -= SEGMARGIN;
    m_ptDG[LUET][4].y -= SEGMARGIN;
    m_ptDG[LUET][5].y += SEGMARGIN;
    // MET
    m_ptDG[MET][0].x = rcLev1.left;
    m_ptDG[MET][0].y = nMiddle;
    m_ptDG[MET][1].x = rcLev2.left;
    m_ptDG[MET][1].y = (nMiddle - dgWide);
    m_ptDG[MET][2].x = rcLev2.right;
    m_ptDG[MET][2].y = (nMiddle - dgWide);
    m_ptDG[MET][3].x = rcLev1.right;
    m_ptDG[MET][3].y = nMiddle;
    m_ptDG[MET][4].x = rcLev2.right;
    m_ptDG[MET][4].y = (nMiddle + dgWide);
    m_ptDG[MET][5].x = rcLev2.left;
    m_ptDG[MET][5].y = (nMiddle + dgWide);
    m_ptDG[MET][0].x += SEGMARGIN;
    m_ptDG[MET][1].x += SEGMARGIN;
    m_ptDG[MET][2].x -= SEGMARGIN;
    m_ptDG[MET][3].x -= SEGMARGIN;
    m_ptDG[MET][4].x -= SEGMARGIN;
    m_ptDG[MET][5].x += SEGMARGIN;
    m_iWide			= dgWide;
}
//-----------------------------------------------------------------------------
// set the digit active and inactive color
void CDigit::setDigitColor( COLORREF actColor, COLORREF inactColor ) {
    m_activeBrush = CreateSolidBrush( actColor );
    m_inactiveBrush = CreateSolidBrush( inactColor );
}
//-----------------------------------------------------------------------------
// set digit line color - active and inactive
void CDigit::setLineColor( COLORREF actLineColor, COLORREF inactLineColor ) {
    m_hActPen = CreatePen( PS_SOLID, 1, actLineColor );
    m_hInactPen = CreatePen( PS_SOLID, 1, inactLineColor );
}
//-----------------------------------------------------------------------------
// draw digit, form "0"-"9"
void CDigit::drawDigit( HDC dc, LPCTSTR szDigit ) {
    drawDigit( dc, ATOI( szDigit ) );
}
//-----------------------------------------------------------------------------
// draw digit, form 0 - 9
void CDigit::drawDigit( HDC dc, const int iDigit ) {
    switch( iDigit ) {
    case 0:
        drawSegment( dc, UET, true );
        drawSegment( dc, RUET, true );
        drawSegment( dc, RDET, true );
        drawSegment( dc, DET, true );
        drawSegment( dc, LDET, true );
        drawSegment( dc, LUET, true );
        drawSegment( dc, MET, false );
        break;
    case 1:
        drawSegment( dc, UET, false );
        drawSegment( dc, RUET, true );
        drawSegment( dc, RDET, true );
        drawSegment( dc, DET, false );
        drawSegment( dc, LDET, false );
        drawSegment( dc, LUET, false );
        drawSegment( dc, MET, false );
        break;
    case 2:
        drawSegment( dc, UET, true );
        drawSegment( dc, RUET, true );
        drawSegment( dc, RDET, false );
        drawSegment( dc, DET, true );
        drawSegment( dc, LDET, true );
        drawSegment( dc, LUET, false );
        drawSegment( dc, MET, true );
        break;
    case 3:
        drawSegment( dc, UET, true );
        drawSegment( dc, RUET, true );
        drawSegment( dc, RDET, true );
        drawSegment( dc, DET, true );
        drawSegment( dc, LDET, false );
        drawSegment( dc, LUET, false );
        drawSegment( dc, MET, true );
        break;
    case 4:
        drawSegment( dc, UET, false );
        drawSegment( dc, RUET, true );
        drawSegment( dc, RDET, true );
        drawSegment( dc, DET, false );
        drawSegment( dc, LDET, false );
        drawSegment( dc, LUET, true );
        drawSegment( dc, MET, true );
        break;
    case 5:
        drawSegment( dc, UET, true );
        drawSegment( dc, RUET, false );
        drawSegment( dc, RDET, true );
        drawSegment( dc, DET, true );
        drawSegment( dc, LDET, false );
        drawSegment( dc, LUET, true );
        drawSegment( dc, MET, true );
        break;
    case 6:
        drawSegment( dc, UET, true );
        drawSegment( dc, RUET, false );
        drawSegment( dc, RDET, true );
        drawSegment( dc, DET, true );
        drawSegment( dc, LDET, true );
        drawSegment( dc, LUET, true );
        drawSegment( dc, MET, true );
        break;
    case 7:
        drawSegment( dc, UET, true );
        drawSegment( dc, RUET, true );
        drawSegment( dc, RDET, true );
        drawSegment( dc, DET, false );
        drawSegment( dc, LDET, false );
        drawSegment( dc, LUET, false );
        drawSegment( dc, MET, false );
        break;
    case 8:
        drawSegment( dc, UET, true );
        drawSegment( dc, RUET, true );
        drawSegment( dc, RDET, true );
        drawSegment( dc, DET, true );
        drawSegment( dc, LDET, true );
        drawSegment( dc, LUET, true );
        drawSegment( dc, MET, true );
        break;
    case 9:
        drawSegment( dc, UET, true );
        drawSegment( dc, RUET, true );
        drawSegment( dc, RDET, true );
        drawSegment( dc, DET, true );
        drawSegment( dc, LDET, false );
        drawSegment( dc, LUET, true );
        drawSegment( dc, MET, true );
        break;
    }
}
//-----------------------------------------------------------------------------
// draw segment with selected color and pen
void CDigit::drawSegment( HDC dc, const DEType segType, const bool bState ) const {
    HBRUSH oldBrush = (HBRUSH) SelectObject( dc, (bState) ? m_activeBrush : m_inactiveBrush );
    HPEN oldPen = (HPEN) SelectObject( dc, (bState) ? m_hActPen : m_hInactPen );
    Polygon( dc, m_ptDG[ segType ], 6 );
    SelectObject( dc, oldPen );
    SelectObject( dc, oldBrush );
}
//-----------------------------------------------------------------------------