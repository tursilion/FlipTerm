/////////////////////////////////////////////////////////////////////////////
// CInWnd window
// inwnd.h : header file
//

#define WM_ENTER_PRESSED WM_USER+1
#define WM_DIRECT_CONNECT WM_USER+2

class CInWnd : public CEdit
{
// Construction
public:
	bool bControlDown;
	bool bSentMapkey;
	CInWnd();
	virtual ~CInWnd();
	
	int NewFont(LPLOGFONT lf);
	LOGFONT m_LogFont;

	void HistoryPrev();
	void HistoryNext();

protected:
	// Generated message map functions
	//{{AFX_MSG(CInWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	CStringArray m_aLines;
	int m_lastline;
	CFont *myFont;
};

/////////////////////////////////////////////////////////////////////////////
