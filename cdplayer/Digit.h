
#ifndef _DIGIT_H_
#define _DIGIT_H_


#ifdef UNICODE
#define ATOI _wtoi
#else
#define ATOI atoi
#endif

/*
    #define MTRACE(a)
    (
    #ifdef _DEBUG

    #endif
    )
*/
#define SEGMENTS	7
#define SEGMARGIN	2

// type digit elements
enum DEType {
    UET  = 0,
    RUET = 1,
    RDET = 2,
    DET  = 3,
    LDET = 4,
    LUET = 5,
    MET  = 6
};
/*
    enum SegState
    {
	ACTIVE = 0,
	INACTIVE = 1
    };
*/
/*
		UET
	     _
    LUET	|_|	RUET
    LDET	|_|	RDET

		DET
*/

class CDigit {
public:

    CDigit();
    CDigit( const RECT &rc, const int dgWide, COLORREF actColor, COLORREF inactColor,
            COLORREF actLineColor, COLORREF inactLineColor );
    virtual ~CDigit();

    void setParameters( const RECT &rc, const int dgWide );
    void setDigitColor( COLORREF actColor, COLORREF inactColor );
    void setLineColor( COLORREF actLineColor, COLORREF inactLineColor );
    void drawDigit( HDC dc, LPCTSTR szDigit );
    void drawDigit( HDC dc, const int iDigit );

protected:

    void CDigit::drawSegment( HDC dc, const DEType segType, const bool bState ) const;

protected:

    int m_iWide;
    HBRUSH m_activeBrush;
    HBRUSH m_inactiveBrush;
    HPEN m_hActPen;
    HPEN m_hInactPen;
    POINT m_ptDG[SEGMENTS][6];
};

#endif