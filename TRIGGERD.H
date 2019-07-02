// triggerd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTriggerDialog dialog

#include "colorcom.h"

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
	CButton	m_Rename;
	CListBox	m_Triggers;
	CButton	m_ChangeCol;
	CButton	m_Test;
	CStatic	m_PlayFile;
	CEdit	m_cText;
	CEdit	m_cFilename;
	CButton	m_Browse;
	CButton	m_Delete;
	CString	m_sFileName;
	CString	m_sMudText;
	CString	m_sTrigger;
	//}}AFX_DATA

	BOOL SetTriggers(CSelArray *pArray);
	CTrigger *GetTrigger(int index);
	CTrigger *GetTrigger(CString sName);
	int GetNumTriggers();
	CColorCombo m_comboForeground;
	int m_colorForeGround;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTriggerDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CSelArray *m_pTriggers;

	CString m_OldTrigger;

	// Generated message map functions
	//{{AFX_MSG(CTriggerDialog)
	afx_msg void OnBrowseFile();
	afx_msg void OnDelete();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnSelchangeTriggerlist();
	afx_msg void OnTestSound();
	afx_msg void OnChangecolor();
	afx_msg void OnRename();
	afx_msg void OnNewantiidle();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void EnableControls();
	void DisableControls();
};
