// namedial.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNameDialog dialog

class CNameDialog : public CDialog
{
// Construction
public:
	CNameDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNameDialog)
	enum { IDD = IDD_NAME_DIALOG };
	CString	m_sName;
	CString	m_sPrompt;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNameDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNameDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
