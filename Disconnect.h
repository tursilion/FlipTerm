#if !defined(AFX_DISCONNECT_H__8BF760F1_77B4_4300_8B6D_26561E562CDF__INCLUDED_)
#define AFX_DISCONNECT_H__8BF760F1_77B4_4300_8B6D_26561E562CDF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Disconnect.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDisconnect dialog

class CDisconnect : public CDialog
{
// Construction
public:
	CDisconnect(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDisconnect)
	enum { IDD = IDD_DISCONNECT };
	CString	m_Text;
	BOOL	m_Check;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDisconnect)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDisconnect)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DISCONNECT_H__8BF760F1_77B4_4300_8B6D_26561E562CDF__INCLUDED_)
