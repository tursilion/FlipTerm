/////////////////////////////////////////////////////////////////////////////
// COutWnd window
// outwnd.h : header file
//


class COutWnd : public CWnd
{
  // Construction
  public:
	  COutWnd();
	  virtual ~COutWnd();

	  void AnsiReset();
	  int CalcFontHeight();
	  int GetLineHeight();
	  InvalidateLine(int nLine);
	  NewFont(LPLOGFONT lf);
	  void OnEnterPressed();
	  CPoint PointToChar( CPoint pt);
	  PutString(char * str);
	  ScrollUp(int numlines  = 1 );
	  VTPutString(const char * minstr);
	  void AbortLine();
	  void LoadLine();
	  void GoToCol();
	  BOOL IsInSelection(int x,int y);

    // data
    LOGFONT m_LogFont;


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
	  //}}AFX_MSG
	  DECLARE_MESSAGE_MAP()
	  
	  CStringList m_Lines;

	  char *str;
	  char * cstr;
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
	  CPoint m_ptDown;
	  CPoint m_ptUp;

	  BOOL m_bBold;
	  BOOL m_iFGIndex;
	  BOOL m_bNextNew;
	  BOOL m_bLastcr;
	  BOOL m_bUnPauseWhenSelectionDone;
	  COLORREF m_AnsiColorForeGround;
	  COLORREF m_AnsiColorBackGround;
};

/////////////////////////////////////////////////////////////////////////////
