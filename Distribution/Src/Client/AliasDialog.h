// AliasDialog.h : header file
//
#if !defined(AFX_ALIASDIALOG_H__5FA8CD43_A153_11D0_97DB_0000C037E924__INCLUDED_)
#define AFX_ALIASDIALOG_H__5FA8CD43_A153_11D0_97DB_0000C037E924__INCLUDED_


class CAlias;
class CAliasArray;

/////////////////////////////////////////////////////////////////////////////
// CAliasDialog dialog

class CAliasDialog : public CDialog
{
// Construction
public:
	void RefillListBox();
	CAlias * GetAlias(CString sName);
	CAliasDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAliasDialog)
	enum { IDD = IDD_ALIAS_EDIT };
	CListBox	m_ctlAliasList;
	CString	m_sAliasText;
	CString	m_sAliasSelected;
	//}}AFX_DATA

	void SetAliases(CAliasArray *pSel);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAliasDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAliasDialog)
	afx_msg void OnDelete();
	virtual BOOL OnInitDialog();
	afx_msg void OnInsert();
	afx_msg void OnEdit();
	afx_msg void OnSelchangeAliasList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

			CAliasArray * m_pAliasArray;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALIASDIALOG_H__5FA8CD43_A153_11D0_97DB_0000C037E924__INCLUDED_)
