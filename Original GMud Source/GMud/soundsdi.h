// soundsdi.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSoundsDialog dialog

class CSoundsDialog : public CDialog
{
// Construction
public:
	CSoundsDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSoundsDialog)
	enum { IDD = IDD_SOUNDS_DIALOG };
	BOOL	m_bBell;
	BOOL	m_bTriggerSound;
	BOOL	m_bConnectBeep;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSoundsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSoundsDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
