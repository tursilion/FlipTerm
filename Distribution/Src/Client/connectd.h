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
	CButton	m_ctrlReset;
	CStatic	m_ctrlFontName;
	CButton	m_ctrlFont;
	CButton	m_ctrlColor;
	CButton	m_ctrl7Bit;
	CButton	m_ctrlAutoConnect;
	CButton	m_ctrlReconnect;
	CButton	m_ctrlAutoLog;
	CButton	m_ctrlRename;
	CButton	m_ctrlDelete;
	CButton	m_ctrlTrigger;
	CButton	m_ctrlMacro;
	CButton	m_ctrlAlias;
	CButton m_ctrlAppend;
	CButton m_ctrlIRC;
	CButton m_ctrlNew;
	CButton m_ctrlDaily;
	CEdit	m_ctrlDescription;
	CEdit	m_ctrlConnectString;
	CEdit	m_ctrlPort;
	CEdit	m_ctrlHostName;
	CString	m_sConnectString;
	CString	m_sHostName;
	UINT	m_wPort;
	CString	m_sWorldName;
	CString	m_sDescription;
	CString	m_sTriggerSetName;
	CString	m_sMacroSetName;
	CString	m_sAliasSet;
	BOOL	m_bAutoLog;
	BOOL	m_Reconnect;
	BOOL	m_bAutoConnect;
	BOOL	m_b7Bit;
	CString	m_FontName;
	//}}AFX_DATA

	int PopulateList();
// Overrides
public:
	CObArray *m_paWorlds;
	CWorld *m_pWorld;
	CSelArray *m_pTriggers;
	CSelArray *m_pMacros;
	CSelArray *m_pAliases;
	CWorld *FindWorld(CString sName);
	BOOL m_bShowConnect;
	CString csWorld;
	CString m_sAutoLog;
	int m_nAutologMode;

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
	virtual void OnCancel();
	afx_msg void OnAutolog();
	afx_msg void OnChangefont();
	afx_msg void OnChangecolor();
	afx_msg void OnReset();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedIrc();
};
