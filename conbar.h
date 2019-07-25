// conbarh : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConnectedBar dialog

class CConnectedBar : public CPaneDialog
{

// Construction
public:
	CConnectedBar();   // standard constructor
	int CConnectedBar::GetCurSel();
	BOOL AddName(BOOL bUnseen,CString sTitle);
	void EmptyList();
	void DoDataExchange(CDataExchange* pDX);

// Dialog Data
	//{{AFX_DATA(CConnectedBar)
	enum { IDD = IDD_CONNECTED_WORLD_BAR };
	CString	m_sWorldName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConnectedBar)
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConnectedBar)
	afx_msg void OnSelchangeConnectList();
	afx_msg void OnDblclkConnectList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CListBox m_ListBox;
};
