// renamedi.cpp : implementation file
//

#include "stdafx.h"
#include "gmud32.h"
#include "renamedi.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRenameDialog dialog


CRenameDialog::CRenameDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CRenameDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRenameDialog)
	m_sOldName = _T("");
	m_sNewName = _T("");
	//}}AFX_DATA_INIT
}


void CRenameDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRenameDialog)
	DDX_Text(pDX, IDC_OLDNAME, m_sOldName);
	DDX_Text(pDX, IDC_NEWNAME, m_sNewName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRenameDialog, CDialog)
	//{{AFX_MSG_MAP(CRenameDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CRenameDialog message handlers

BOOL CRenameDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CenterWindow();	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
