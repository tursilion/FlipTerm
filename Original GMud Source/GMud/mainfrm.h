// mainfrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef MAINFRAME_DEFINED
#define MAINFRAME_DEFINED

#include "buttonba.h"
#include "conbar.h"
class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

	void OnUpdateFrameTitle(BOOL bAddToTitle);
// Attributes
public:

// Operations
public:
	UpdateButtons();
	ClearButtons();
	SetButtons();
	UpdateWorlds();
	OutputMacro(CString);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CButtonBar	m_wndMacroBar;
	CConnectedBar m_wndConnected;
	BOOL 		m_bMacrosVisible;
	BOOL		m_bConnectedVisible;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg void OnSelchangeConnectList();
	afx_msg void OnSetfocusConnectList();
	afx_msg void OnDblclkConnectList();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnButtonEdit();
	afx_msg void OnButton1();
	afx_msg void OnButton10();
	afx_msg void OnButton2();
	afx_msg void OnButton3();
	afx_msg void OnButton4();
	afx_msg void OnButton5();
	afx_msg void OnButton6();
	afx_msg void OnButton7();
	afx_msg void OnButton8();
	afx_msg void OnButton9();
	afx_msg LONG OnViewSelected(UINT,LONG);
	afx_msg LONG OnUpdateWorlds(UINT,LONG);
	afx_msg void OnEditCopy();
	afx_msg void OnEditAliaslist();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif // MAINFRAME_DEFINED
