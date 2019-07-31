// mainfrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef MAINFRAME_DEFINED
#define MAINFRAME_DEFINED
#pragma once
#include "buttonba.h"
#include "conbar.h"

class CMainFrame : public CMDIFrameWndEx
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

	void OnUpdateFrameTitle(BOOL bAddToTitle);
// Attributes
public:

// Operations
public:
	int UpdateButtons();
	int ClearButtons();
	int SetButtons();
	int UpdateWorlds();
	int OutputMacro(CString);

// Overrides
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	CConnectedBar *GetWndConnected() { return &m_wndConnected; }

    void OnUpdateControlBarMenu(CCmdUI* pCmdUI);

    // this gives me a CMenu object to the m_wndMenuBar (once its created)
	CMFCMenuBar     m_wndMenuBar;

protected:  // control bar embedded members
	CMFCStatusBar 	m_wndStatusBar;
	CMFCToolBar 	m_wndToolBar;
	CMFCToolBar		m_wndNumbers;
	CButtonBar	    m_wndMacroBar;
	CConnectedBar 	m_wndConnected;
	BOOL 			m_bMacrosVisible;
	BOOL			m_bConnectedVisible;

	unsigned int nTimer;
	
// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowManager();
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnClose();
	afx_msg void OnButtonEdit();
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
	afx_msg void OnEditAliaslist();
	afx_msg void OnButton1();
	afx_msg void OnTimer(UINT nIDEvent);
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnViewToolbarMenu();
    afx_msg void OnUpdateViewToolbarMenu(CCmdUI *pCmdUI);
    afx_msg void OnViewWindowbar();
    afx_msg void OnViewMacrowindow();
    afx_msg void OnViewWorldwindow();
    afx_msg void OnInitMenuPopup(CMenu*, UINT, BOOL);
};

/////////////////////////////////////////////////////////////////////////////
#endif // MAINFRAME_DEFINED
