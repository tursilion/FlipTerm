// textclr.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTextClrDlg dialog

#include "colorcom.h"

class CTextClrDlg : public CDialog
{
// Construction
public:
	CTextClrDlg(CWnd* pParent = NULL);   // standard constructor
	
	CColorCombo m_comboForeground;
	CColorCombo m_comboBackground;
public:
	bool m_bIsSystem;
	bool m_bIsAnsi;
	int m_colorBackGround;
	int m_colorForeGround;

// Dialog Data
	//{{AFX_DATA(CTextClrDlg)
	enum { IDD = IDD_TEXT_COLOR };
	CButton	m_ctrlFgBox;
	CButton	m_ctrlBgBox;
	CButton	m_ctrlAnsi;
	int		m_iForeground;
	int		m_iBackground;
	BOOL	m_bAnsi;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTextClrDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTextClrDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
