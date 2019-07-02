/////////////////////////////////////////////////////////////////////////////
// CInWnd window
// inwnd.h : header file
//


#define WM_ENTER_PRESSED WM_USER+1

class CInWnd : public CEdit
{
// Construction
public:
	CInWnd();
	virtual ~CInWnd();
	
	NewFont(LPLOGFONT lf);
	LOGFONT m_LogFont;
protected:
	// Generated message map functions
	//{{AFX_MSG(CInWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	CStringArray m_aLines;
	int m_lastline;
};

/////////////////////////////////////////////////////////////////////////////
