// buttoned.cpp : implementation file
//

#include "stdafx.h"
#include "gmud32.h"
#include "buttoned.h"
#include "macro.h"
#include "macroarray.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CButtonEdit dialog


CButtonEdit::CButtonEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CButtonEdit::IDD, pParent)
{
	CWinAppEx *pApp = GetApp();
	ASSERT(pApp);
	//{{AFX_DATA_INIT(CButtonEdit)
	//}}AFX_DATA_INIT
}

int CButtonEdit::SetList(CMacroArray *pArray)
{
	m_pArray = pArray;
	m_s1 = ((CMacro *)(pArray->Get("F1")))->m_sText;
	m_s2 = ((CMacro *)(pArray->Get("F2")))->m_sText;
	m_s3 = ((CMacro *)(pArray->Get("F3")))->m_sText;
	m_s4 = ((CMacro *)(pArray->Get("F4")))->m_sText;
	m_s5 = ((CMacro *)(pArray->Get("F5")))->m_sText;
	m_s6 = ((CMacro *)(pArray->Get("F6")))->m_sText;
	m_s7 = ((CMacro *)(pArray->Get("F7")))->m_sText;
	m_s8 = ((CMacro *)(pArray->Get("F8")))->m_sText;
	m_s9 = ((CMacro *)(pArray->Get("F9")))->m_sText;
	m_s10 = ((CMacro *)(pArray->Get("F10")))->m_sText;
	return TRUE;
}

void CButtonEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CButtonEdit)
	DDX_Text(pDX, IDC_EDIT1, m_s1);
	DDX_Text(pDX, IDC_EDIT2, m_s2);
	DDX_Text(pDX, IDC_EDIT3, m_s3);
	DDX_Text(pDX, IDC_EDIT4, m_s4);
	DDX_Text(pDX, IDC_EDIT5, m_s5);
	DDX_Text(pDX, IDC_EDIT6, m_s6);
	DDX_Text(pDX, IDC_EDIT7, m_s7);
	DDX_Text(pDX, IDC_EDIT8, m_s8);
	DDX_Text(pDX, IDC_EDIT9, m_s9);
	DDX_Text(pDX, IDC_EDIT10, m_s10);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CButtonEdit, CDialog)
	//{{AFX_MSG_MAP(CButtonEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CButtonEdit message handlers

void CButtonEdit::OnOK() 
{
	UpdateData(TRUE);

	CWinAppEx *pApp = GetApp();
	ASSERT(pApp);
	;
	((CMacro *)(m_pArray->Get("F1")))->m_sText=m_s1;
	((CMacro *)(m_pArray->Get("F2")))->m_sText=m_s2;
	((CMacro *)(m_pArray->Get("F3")))->m_sText=m_s3;
	((CMacro *)(m_pArray->Get("F4")))->m_sText=m_s4;
	((CMacro *)(m_pArray->Get("F5")))->m_sText=m_s5;
	((CMacro *)(m_pArray->Get("F6")))->m_sText=m_s6;
	((CMacro *)(m_pArray->Get("F7")))->m_sText=m_s7;
	((CMacro *)(m_pArray->Get("F8")))->m_sText=m_s8;
	((CMacro *)(m_pArray->Get("F9")))->m_sText=m_s9;
	((CMacro *)(m_pArray->Get("F10")))->m_sText=m_s10;
	CDialog::OnOK();
}

CString &CrToSlashR(CString &str)
{
	int charat=0;
	while( (charat=str.Find("\r\n"))!=-1)
	{
		str=str.Left(charat)+"\\n"+str.Right((str.GetLength()-charat)-2);
	}
	return str;
}

CString SlashRToCr(CString &str)
{
	int charat=0;
	while( (charat=str.Find("\\n"))!=-1)
	{
		str=str.Left(charat)+"\r\n"+str.Right((str.GetLength()-charat)-2);
	}
	return str;
}

BOOL CButtonEdit::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CenterWindow();	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
