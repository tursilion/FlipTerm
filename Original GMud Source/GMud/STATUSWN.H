// statuswn.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStatusWnd window

class CStatusWnd : public CWnd
{
	public:
		CStatusWnd();
		virtual ~CStatusWnd();
	
		CStatusWnd::SetName(LPCSTR newname);
		CStatusWnd::SetChannel(LPCSTR newchannel);
		CStatusWnd::SetChanMode(LPCSTR newmode);
		SetOpped(BOOL bOpped);
		NewFont(LPLOGFONT lf);
		LOGFONT m_LogFont;
		
	protected:
		// Generated message map functions
		//{{AFX_MSG(CStatusWnd)
		afx_msg void OnPaint();
		afx_msg BOOL OnEraseBkgnd(CDC* pDC);
		//}}AFX_MSG
			DECLARE_MESSAGE_MAP()
			
		CString m_sName;
		CString m_sChannel;
		CString m_sMode;
		BOOL m_bOpped;
};

/////////////////////////////////////////////////////////////////////////////
