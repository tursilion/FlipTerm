// replaced.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CReplaceDialog dialog

class CReplaceDialog : public CDialog
{
// Construction
public:
	CReplaceDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CReplaceDialog)
	enum { IDD = IDD_REPLACE_FILE };
	CString	m_sFileName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReplaceDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CReplaceDialog)
	afx_msg void OnAppend();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

