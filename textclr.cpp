// textclr.cpp : implementation file
//

#include "stdafx.h"
#include "gmud32.h"
#include "textclr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern unsigned long colormap[];

/////////////////////////////////////////////////////////////////////////////
// CTextClrDlg dialog


CTextClrDlg::CTextClrDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTextClrDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTextClrDlg)
	m_iForeground = -1;
	m_iBackground = -1;
	m_bAnsi = FALSE;
	//}}AFX_DATA_INIT
	m_bIsSystem=true;
}

void CTextClrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTextClrDlg)
	DDX_Control(pDX, IDC_FGBOX, m_ctrlFgBox);
	DDX_Control(pDX, IDC_BGBOX, m_ctrlBgBox);
	DDX_Control(pDX, IDC_ENABLE_ANSI, m_ctrlAnsi);
	DDX_CBIndex(pDX, IDC_COMBO1, m_iForeground);
	DDX_CBIndex(pDX, IDC_COMBO2, m_iBackground);
	DDX_Check(pDX, IDC_ENABLE_ANSI, m_bAnsi);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTextClrDlg, CDialog)
	//{{AFX_MSG_MAP(CTextClrDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTextClrDlg message handlers

extern unsigned long colormap[];

BOOL CTextClrDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	VERIFY(m_comboForeground.SubclassDlgItem(IDC_COMBO1, this));
	VERIFY(m_comboBackground.SubclassDlgItem(IDC_COMBO2, this));

	int iIndex=0;
	// We just manually add the colors available to the ANSI color routine
	for (iIndex=0; iIndex<16; iIndex++) {
		m_comboForeground.AddColorItem(colormap[iIndex]);
		m_comboBackground.AddColorItem(colormap[iIndex]);
		if(iIndex == m_colorBackGround)
			m_comboBackground.SetCurSel(iIndex);
		if(iIndex == m_colorForeGround)
			m_comboForeground.SetCurSel(iIndex);
	}
	CenterWindow();	

	if (!m_bIsSystem) {
		m_ctrlAnsi.ModifyStyle(WS_VISIBLE, 0);
		m_ctrlFgBox.SetWindowText("System message color");
		m_ctrlBgBox.SetWindowText("Input text color");
		SetWindowText("Set system message and input text colors");
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTextClrDlg::OnOK() 
{
	UpdateData(TRUE);
	m_colorBackGround=m_iBackground;
	m_colorForeGround=m_iForeground;
	CDialog::OnOK();
}
