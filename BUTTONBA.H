// buttonba.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CButtonBar window

class CButtonBar : public CPaneDialog
{
// Construction
public:
	CButtonBar();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CButtonBar)
	public:
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CButtonBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CButtonBar)
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnButton1();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnChildActivate();
	afx_msg LRESULT OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu);
	afx_msg void OnParentNotify(UINT message, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
