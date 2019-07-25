#if !defined(AFX_SWITCHES_H__E090FAA4_97D7_426B_9E8B_93D1555325F8__INCLUDED_)
#define AFX_SWITCHES_H__E090FAA4_97D7_426B_9E8B_93D1555325F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Switches.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSwitches dialog

class CSwitches : public CDialogEx
{
// Construction
public:
	CSwitches(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSwitches)
	enum { IDD = IDD_SWITCHES };
	CString	m_CommandChar;
	BOOL	m_CommandStacking;
	BOOL	m_ConfirmDisconnect;
	BOOL	m_ForceAnsiReset;
	BOOL	m_LocalEcho;
	BOOL	m_MultilineInput;
	BOOL	m_ShowActivity;
	BOOL	m_ShowCopyNotice;
	BOOL	m_UseCLI;
	BOOL	m_NewWindow;
	BOOL	m_ReviewEnd;
	BOOL	m_MapKeys;
	BOOL	m_ConfirmClose;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSwitches)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSwitches)
	afx_msg void OnMultilineinput();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SWITCHES_H__E090FAA4_97D7_426B_9E8B_93D1555325F8__INCLUDED_)
