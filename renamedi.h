// renamedi.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRenameDialog dialog

class CRenameDialog : public CDialog
{
// Construction
public:
	CRenameDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRenameDialog)
	enum { IDD = IDD_RENAME };
	CString	m_sOldName;
	CString	m_sNewName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRenameDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRenameDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
