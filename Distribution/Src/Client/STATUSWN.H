// statuswn.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStatusWnd window

class CStatusWnd : public CWnd
{
	public:
		CStatusWnd();
		virtual ~CStatusWnd();
	
	protected:
		// Generated message map functions
		//{{AFX_MSG(CStatusWnd)
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	//}}AFX_MSG
			DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
