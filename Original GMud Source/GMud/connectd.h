/////////////////////////////////////////////////////////////////////////////
// CConnectDialog dialog
//
// connectd.h : header file
//

class CWorld;
class CSelArray;

class CConnectDialog : public CDialog
{
// Construction
public:
	CConnectDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConnectDialog)
	enum { IDD = IDD_CONNECT };
	CString	m_sConnectString;
	CString	m_sHostName;
	UINT	m_wPort;
	CString	m_sWorldName;
	CString	m_sDescription;
	CString	m_sTriggerSetName;
	CString	m_sMacroSetName;
	CString	m_sAliasSet;
	//}}AFX_DATA

	PopulateList();
// Overrides
public:
	CObArray *m_paWorlds;
	CWorld *m_pWorld;
	CSelArray *m_pTriggers;
	CSelArray *m_pMacros;
	CSelArray *m_pAliases;
	CWorld *FindWorld(CString sName);
	BOOL m_bShowConnect;

	void GetWorld(CWorld *pWorld);
	void ShowWorld(CWorld *pWorld);
	int FindWorldIndex(CString sName);
	BOOL SaveWorld();
// Implementation
protected:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConnectDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	// Generated message map functions
	//{{AFX_MSG(CConnectDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkWorldlist();
	virtual void OnOK();
	afx_msg void OnAdd();
	afx_msg void OnSelchangeWorldlist();
	afx_msg void OnDeleteWorld();
	afx_msg void OnRenameWorld();
	afx_msg void OnChangeTrigger();
	afx_msg void OnChangeMacro();
	afx_msg void OnChangeAlias();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
