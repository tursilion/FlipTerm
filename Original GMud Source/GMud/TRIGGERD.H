// triggerd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTriggerDialog dialog

class CTrigger;
class CSelArray;

class CTriggerDialog : public CDialog
{
// Construction
public:
	CTriggerDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTriggerDialog)
	enum { IDD = IDD_TRIGGER_DIALOG };
	CString	m_sFileName;
	CString	m_sMudText;
	CString	m_sTrigger;
	//}}AFX_DATA

	BOOL SetTriggers(CSelArray *pArray);
	CTrigger *GetTrigger(int index);
	CTrigger *GetTrigger(CString sName);
	int GetNumTriggers();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTriggerDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CSelArray *m_pTriggers;
	// Generated message map functions
	//{{AFX_MSG(CTriggerDialog)
	afx_msg void OnBrowseFile();
	afx_msg void OnDelete();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnSelchangeTriggerList();
	afx_msg void OnTestSound();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
