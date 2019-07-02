// replaced.cpp : implementation file
//

#include "stdafx.h"
#include "gmud32.h"
#include "replaced.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CReplaceDialog dialog


CReplaceDialog::CReplaceDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CReplaceDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CReplaceDialog)
	m_sFileName = _T("");
	//}}AFX_DATA_INIT
}


void CReplaceDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CReplaceDialog)
	DDX_Text(pDX, IDC_FILENAME, m_sFileName);
	DDV_MaxChars(pDX, m_sFileName, 255);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CReplaceDialog, CDialog)
	//{{AFX_MSG_MAP(CReplaceDialog)
	ON_BN_CLICKED(IDC_APPEND, OnAppend)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CReplaceDialog message handlers

void CReplaceDialog::OnAppend() 
{
	// TODO: Add your control notification handler code here
	EndDialog(IDOVERWRITE);	
}
