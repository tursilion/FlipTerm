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
	COLORREF m_colorBackGround;
	COLORREF m_colorForeGround;
// Dialog Data
	//{{AFX_DATA(CTextClrDlg)
	enum { IDD = IDD_TEXT_COLOR };
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
	afx_msg void OnChangeForegroundColor();
	afx_msg void OnChangeBackgroundColor();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
