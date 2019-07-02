// textclr.cpp : implementation file
//

#include "stdafx.h"
#include "gmud32.h"
#include "textclr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

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
}


void CTextClrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTextClrDlg)
	DDX_CBIndex(pDX, IDC_COMBO1, m_iForeground);
	DDX_CBIndex(pDX, IDC_COMBO2, m_iBackground);
	DDX_Check(pDX, IDC_ENABLE_ANSI, m_bAnsi);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTextClrDlg, CDialog)
	//{{AFX_MSG_MAP(CTextClrDlg)
	ON_BN_CLICKED(IDC_CHANGE_FOREGROUND_COLOR, OnChangeForegroundColor)
	ON_BN_CLICKED(IDC_CHANGE_BACKGROUND_COLOR, OnChangeBackgroundColor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTextClrDlg message handlers

void CTextClrDlg::OnChangeForegroundColor() 
{
//	CColorDialog dlg(m_colorForeGround,0,this);
//	if(dlg.DoModal()==IDOK)
//	{
//		m_colorForeGround=dlg.GetColor();
//	}
}

void CTextClrDlg::OnChangeBackgroundColor() 
{
//	CColorDialog dlg(m_colorBackGround,0,this);
//	if(dlg.DoModal()==IDOK)
//	{
//		m_colorBackGround=dlg.GetColor();
//	}
}

BOOL CTextClrDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	VERIFY(m_comboForeground.SubclassDlgItem(IDC_COMBO1, this));
	VERIFY(m_comboBackground.SubclassDlgItem(IDC_COMBO2, this));

	int iIndex=0;
	// add 8 colors to the listbox (primary + secondary color only)
	for (int red = 0; red <= 255; red += 255)
		for (int green = 0; green <= 255; green += 255)
			for (int blue = 0; blue <= 255; blue += 255)
			{
				COLORREF col = RGB(red, green, blue);
				m_comboForeground.AddColorItem(col);
				m_comboBackground.AddColorItem(col);
				if(col == m_colorBackGround)
					m_comboBackground.SetCurSel(iIndex);
				if(col == m_colorForeGround)
					m_comboForeground.SetCurSel(iIndex);
				iIndex++;
			}
	CenterWindow();	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTextClrDlg::OnOK() 
{
	UpdateData(TRUE);
/*	int index=0;
	for (int red = 0; red <= 255; red += 255)
		for (int green = 0; green <= 255; green += 255)
			for (int blue = 0; blue <= 255; blue += 255)
			{
				if(index
*/
	m_colorBackGround=m_comboBackground.GetItemData(m_iBackground);
	m_colorForeGround=m_comboForeground.GetItemData(m_iForeground);
	CDialog::OnOK();
}
