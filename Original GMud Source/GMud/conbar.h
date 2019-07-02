// conbarh : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConnectedBar dialog

#include "mylistbo.h"

class CConnectedBar : public CDialogBar
{

// Construction
public:
	CConnectedBar();   // standard constructor
	int CConnectedBar::GetCurSel();
	BOOL AddName(BOOL bUnseen,CString sTitle);
	void EmptyList();
// Dialog Data
	//{{AFX_DATA(CConnectedBar)
	enum { IDD = IDD_CONNECTED_WORLD_BAR };
	CString	m_sWorldName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConnectedBar)
	public:
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConnectedBar)
	afx_msg void OnSelchangeConnectList();
	afx_msg void OnSetfocusConnectList();
	afx_msg void OnDblclkConnectList();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CMyListBox m_ListBox;
};
