/////////////////////////////////////////////////////////////////////////////
//
//		Mud32vw.h : interface of the CMudView class
//

class CInWnd;
class COutWnd;
class CSmartSocket;
class CStatusWnd;
class CWorld;
class CMudDoc;

class CMudView : public CView
{
protected: // create from serialization only
	CMudView();
	DECLARE_DYNCREATE(CMudView)

// Attributes
public:
	CMudDoc* GetDocument();
	BOOL m_bActive;
	BOOL m_bUnSeen;
	CWorld *m_pWorld;

// Operations
public:
	CString GetTitle();
	Connect(CWorld *pWorld);
	BOOL __cdecl CMudView::Printf(LPSTR format, ...);
  long CMudView::ProcessOneLine(CString sStr);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMudView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
protected:
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

// Implementation
public:
	bool ProcessUserCommandLine(CString sCommand);
	CMapStringToString  * GetUserVariables();
	virtual ~CMudView();
	BOOL Pause(BOOL bPause);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
	//{{AFX_MSG(CMudView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnServerConnect();
	afx_msg void OnUpdateServerConnect(CCmdUI* pCmdUI);
	afx_msg void OnServerDisconnect();
	afx_msg void OnUpdateServerDisconnect(CCmdUI* pCmdUI);
	afx_msg LONG OnSocketDisconnected(UINT nErrCode,LONG);
	afx_msg LONG OnSocketConnected(UINT nErrCode,LONG);
	afx_msg LONG OnStringReceived(UINT ,LONG lparam);
	afx_msg LONG OnAsynchGetHostCompleted(UINT,LONG);
	afx_msg LONG OnUserInput(UINT,LONG lParam);
	afx_msg LONG OnFontChanged(UINT,LONG);
	afx_msg LONG OnColorChanged(UINT,LONG);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnFileEdittriggerlist();
	afx_msg void OnUpdateFileEdittriggerlist(CCmdUI* pCmdUI);
	afx_msg void OnWindowPause();
	afx_msg void OnUpdateWindowPause(CCmdUI* pCmdUI);
	afx_msg void OnFileTogglelogging();
	afx_msg void OnUpdateFileTogglelogging(CCmdUI* pCmdUI);
	afx_msg void OnPasteFile();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:

	void SetUnSeen( BOOL bUnSeen  = TRUE  );

	CInWnd *m_pInWnd;
	COutWnd *m_pOutWnd;
	CStatusWnd *m_pStatusWnd;
	CSmartSocket *m_pSocket;

	PSTR m_pHostBuf;
	BOOL m_bIsLogging;
	bool m_bConnectionPending;
	CStdioFile *m_fLogFile;

	CBrush *m_pBGBrush;
	CString m_sAddress;
	CString m_sName;
	WORD m_wPort;
};

#ifndef _DEBUG  // debug version in Mud32vw.cpp
inline CMudDoc* CMudView::GetDocument()
   { return (CMudDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
