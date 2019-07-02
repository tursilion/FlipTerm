// wrapopti.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWrapOptions dialog

class CWrapOptions : public CDialog
{
// Construction
public:
	CWrapOptions(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWrapOptions)
	enum { IDD = IDD_VIEW_SETTINGS };
	int		m_iWrapWidth;
	int		m_iWordWrap;
	BOOL	m_bIndent;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWrapOptions)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWrapOptions)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
