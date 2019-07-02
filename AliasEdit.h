#if !defined(AFX_ALIASEDIT_H__5FA8CD44_A153_11D0_97DB_0000C037E924__INCLUDED_)
#define AFX_ALIASEDIT_H__5FA8CD44_A153_11D0_97DB_0000C037E924__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// AliasEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAliasEdit dialog

class CAliasEdit : public CDialog
{
// Construction
public:
	CAliasEdit(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAliasEdit)
	enum { IDD = IDD_ALIAS_SINGLE_EDIT };
	CString	m_sAlias;
	CString	m_sOutput;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAliasEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAliasEdit)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALIASEDIT_H__5FA8CD44_A153_11D0_97DB_0000C037E924__INCLUDED_)
