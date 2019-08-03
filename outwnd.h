/////////////////////////////////////////////////////////////////////////////
// COutWnd window
// outwnd.h : header file
//

#pragma once

class COutWnd : public CWnd
{
	// Construction
public:
	bool m_fMouseWasDown;
	COutWnd();
	virtual ~COutWnd();
	
	void AnsiReset();
	int CalcFontHeight();
	int GetLineHeight();
	int InvalidateLine(int nLine);
	int NewFont(LPLOGFONT lf);
	void OnEnterPressed();
	CPoint PointToChar( CPoint pt);
	int PutString(char * str, int InPaused, bool Is7Bit, bool bufferOnly = false);
	int ScrollUp(int numlines  = 1 );
	int VTPutString(const char * minstr);
	void AbortLine();
	void LoadLine();
	void GoToCol();
	BOOL IsInSelection(int x,int y);
	void ColorString(CString *pStr, int nCol);
	void ParseColor(CString *pStr, int nColored);
    void reformatAll();

    BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	int GetNCols() { return m_nCols; }
	int GetOffset() { return m_iOffset; }
	void ScrollTo(int nPos) { OnVScroll(SB_THUMBPOSITION, nPos, NULL); }

    // data
    LOGFONT m_LogFont;
	CStringList m_Lines;	// Color is now stored as a byte after every character, high nibble foreground
    CStringList m_rawLines; // the received and colored data as above, but without word wrap applied
		
protected:
	// Generated message map functions
	//{{AFX_MSG(COutWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
    afx_msg LRESULT handleReformat(WPARAM, LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
		
	char *mystr;
	char *mycstr;
	LPCSTR orignull;
	
	int m_iMaxLines;
	int m_iOffset;
	int m_iFontHeight;
	int m_iCharWidth;
	int m_nCols;
	int toggle;
	int m_nRow;
	int m_nCol;
	int m_nSavedCol;
	int m_nSavedRow;
	int m_iFGIndex;
	int m_bLastEOL;

	CPoint m_ptDown;
	CPoint m_ptUp;
	
	BOOL m_bBold;
	BOOL m_bInverse;
	BOOL m_bNextNew;
	BOOL m_bUnPauseWhenSelectionDone;
	BOOL m_bInitialized;
	int m_AnsiColorForeGround;
	int m_AnsiColorBackGround;
};

/////////////////////////////////////////////////////////////////////////////
