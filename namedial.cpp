// namedial.cpp : implementation file
//

#include "stdafx.h"
#include "gmud32.h"
#include "namedial.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNameDialog dialog


CNameDialog::CNameDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CNameDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNameDialog)
	m_sName = _T("");
	m_sPrompt = _T("");
	//}}AFX_DATA_INIT
}


void CNameDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNameDialog)
	DDX_Text(pDX, IDC_NAME, m_sName);
	DDX_Text(pDX, IDC_PROMPT, m_sPrompt);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNameDialog, CDialog)
	//{{AFX_MSG_MAP(CNameDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CNameDialog message handlers

BOOL CNameDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CenterWindow();	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
