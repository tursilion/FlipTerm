/////////////////////////////////////////////////////////////////////////////
// CSelArrayDialog dialog

#ifndef CSelArrayDialog_DEFINED
#define CSelArrayDialog_DEFINED

class CSelArray;
#define TYPE_TRIGGER	1
#define TYPE_MACRO		2
#define TYPE_ALIAS		3

class CSelArrayDialog : public CDialog
{
// Construction
public:
	CSelArrayDialog(CWnd* pParent = NULL);   // standard constructor

	int CSelArrayDialog::PopulateList();
	BOOL Setup(CSelArray *pArray,int iType, CObArray *pWorlds);
// Dialog Data
	//{{AFX_DATA(CSelArrayDialog)
	enum { IDD = IDD_SELARRAY_EDIT };
	CString	m_sSelected;
	//}}AFX_DATA
	CString m_sTitle;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelArrayDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelArrayDialog)
	afx_msg void OnEdit();
	afx_msg void OnCopy();
	afx_msg void OnNew();
	afx_msg void OnDelete();
	afx_msg void OnOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkArrayList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CSelArray *m_pArray;
	CObArray *m_pWorlds;
	int m_iType;
};

#endif // CSelArrayDialog_DEFINED
