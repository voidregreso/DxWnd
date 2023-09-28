// Colon.h: interface for the CColon class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLON_H__4C6C7D90_B830_4675_8694_678231C62784__INCLUDED_)
#define AFX_COLON_H__4C6C7D90_B830_4675_8694_678231C62784__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CColon {
public:
    CColon();
    CColon( const RECT &rc, const int iSize,
            COLORREF actColor, COLORREF inactColor,
            COLORREF actLineColor, COLORREF inactLineColor );
    virtual ~CColon();

    void setColonColor( COLORREF actColor, COLORREF inactColor );
    void setLineColor( COLORREF actLineColor, COLORREF inactLineColor );
    void setParameters( const RECT &rc, const int iSize );

    void drawColon( HDC dc, const bool bState ) const;

protected:
    int m_iSize;
    HBRUSH m_activeBrush;
    HBRUSH m_inactiveBrush;
    HPEN m_hActPen;
    HPEN m_hInactPen;
    POINT m_ptCL[2][4];
};

#endif // !defined(AFX_COLON_H__4C6C7D90_B830_4675_8694_678231C62784__INCLUDED_)
