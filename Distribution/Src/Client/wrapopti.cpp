// wrapopti.cpp : implementation file
//

#include "stdafx.h"
#include "gmud32.h"
#include "wrapopti.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWrapOptions dialog


CWrapOptions::CWrapOptions(CWnd* pParent /*=NULL*/)
	: CDialog(CWrapOptions::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWrapOptions)
	m_iWrapWidth = -1;
	m_iWordWrap = -1;
	m_bIndent = FALSE;
	//}}AFX_DATA_INIT
}


void CWrapOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWrapOptions)
	DDX_Radio(pDX, IDC_80columns, m_iWrapWidth);
	DDX_Radio(pDX,IDC_characterwrap, m_iWordWrap);
	DDX_Check(pDX, IDC_CHECK1, m_bIndent);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWrapOptions, CDialog)
	//{{AFX_MSG_MAP(CWrapOptions)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWrapOptions message handlers

BOOL CWrapOptions::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CenterWindow();		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
