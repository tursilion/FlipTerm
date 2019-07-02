// buttoned.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CButtonEdit dialog

class CMacroArray;

class CButtonEdit : public CDialog
{
// Construction
public:
	CButtonEdit(CWnd* pParent = NULL);   // standard constructor
	SetList(CMacroArray *pArray);
// Dialog Data
	//{{AFX_DATA(CButtonEdit)
	enum { IDD = IDD_BUTTON_EDIT };
	CString	m_s1;
	CString	m_s10;
	CString	m_s2;
	CString	m_s3;
	CString	m_s4;
	CString	m_s5;
	CString	m_s6;
	CString	m_s7;
	CString	m_s8;
	CString	m_s9;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CButtonEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	CMacroArray *m_pArray;
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CButtonEdit)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
CString &CrToSlashR(CString &str);
CString SlashRToCr(CString &str);
