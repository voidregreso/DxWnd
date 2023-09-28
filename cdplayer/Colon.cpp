// Colon.cpp: implementation of the CColon class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Colon.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// constructor
CColon::CColon() {
    memset( m_ptCL, 0, sizeof(POINT) * 8 );
    m_iSize = 0;
    m_activeBrush = CreateSolidBrush( RGB(0, 0, 0) );
    m_inactiveBrush = CreateSolidBrush( RGB(0, 0, 0) );
    m_hActPen = CreatePen( PS_SOLID, 1, RGB(0, 0, 0) );
    m_hInactPen = CreatePen( PS_SOLID, 1, RGB(0, 0, 0) );
}
//-----------------------------------------------------------------------------
// constructor
CColon::CColon( const RECT &rc, const int iSize,
                COLORREF actColor, COLORREF inactColor,
                COLORREF actLineColor, COLORREF inactLineColor ) {
    memset( m_ptCL, 0, sizeof(POINT) * 8 );
    m_activeBrush = CreateSolidBrush( actColor );
    m_inactiveBrush = CreateSolidBrush( inactColor );
    m_hActPen = CreatePen( PS_SOLID, 1, actLineColor );
    m_hInactPen = CreatePen( PS_SOLID, 1, inactLineColor );
    setParameters( rc, iSize );
}
//-----------------------------------------------------------------------------
// destructor
CColon::~CColon() {
    DeleteObject( m_activeBrush );
    DeleteObject( m_inactiveBrush );
    DeleteObject( m_hActPen );
    DeleteObject( m_hInactPen );
}
//-----------------------------------------------------------------------------
// set the colon colours - active and inactive
void CColon::setColonColor( COLORREF actColor, COLORREF inactColor ) {
    m_activeBrush = CreateSolidBrush( actColor );
    m_inactiveBrush = CreateSolidBrush( inactColor );
}
//-----------------------------------------------------------------------------
// set the colon pen's color
void CColon::setLineColor( COLORREF actLineColor, COLORREF inactLineColor ) {
    m_hActPen = CreatePen( PS_SOLID, 1, actLineColor );
    m_hInactPen = CreatePen( PS_SOLID, 1, inactLineColor );
}
//-----------------------------------------------------------------------------
//
// top
// nYM1
// nYM2
// nYM3
// bottom
// calculate the position and sizes of colon
void CColon::setParameters( const RECT &rc, const int iSize ) {
    m_iSize = iSize;
    const int nYM2 = (rc.top + ((rc.bottom - rc.top) / 2));
    const int nYM1 = (rc.top + ((nYM2 - rc.top) / 2));
    const int nYM3 = (nYM2 + ((rc.bottom - nYM2) / 2));
    const int nXM  = (rc.left + ((rc.right - rc.left) / 2));
    m_ptCL[0][0].x = (nXM - iSize);
    m_ptCL[0][0].y = (nYM1 - iSize);
    m_ptCL[0][1].x = (nXM + iSize);
    m_ptCL[0][1].y = (nYM1 - iSize);
    m_ptCL[0][2].x = (nXM + iSize);
    m_ptCL[0][2].y = (nYM1 + iSize);
    m_ptCL[0][3].x = (nXM - iSize);
    m_ptCL[0][3].y = (nYM1 + iSize);
    m_ptCL[1][0].x = (nXM - iSize);
    m_ptCL[1][0].y = (nYM3 - iSize);
    m_ptCL[1][1].x = (nXM + iSize);
    m_ptCL[1][1].y = (nYM3 - iSize);
    m_ptCL[1][2].x = (nXM + iSize);
    m_ptCL[1][2].y = (nYM3 + iSize);
    m_ptCL[1][3].x = (nXM - iSize);
    m_ptCL[1][3].y = (nYM3 + iSize);
}
//-----------------------------------------------------------------------------
// draw the colon with selected colour and pen
void CColon::drawColon( HDC dc, const bool bState ) const {
    HBRUSH oldBrush = (HBRUSH) SelectObject( dc, (bState) ? m_activeBrush : m_inactiveBrush );
    HPEN oldPen = (HPEN) SelectObject( dc, (bState) ? m_hActPen : m_hInactPen );
    Polygon( dc, m_ptCL[0], 4 );
    Polygon( dc, m_ptCL[1], 4 );
    SelectObject( dc, oldPen );
    SelectObject( dc, oldBrush );
}
//-----------------------------------------------------------------------------