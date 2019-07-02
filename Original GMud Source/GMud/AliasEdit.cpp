// AliasEdit.cpp : implementation file
//

#include "stdafx.h"
#include "gmud32.h"
#include "AliasEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAliasEdit dialog


CAliasEdit::CAliasEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CAliasEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAliasEdit)
	m_sAlias = _T("");
	m_sOutput = _T("");
	//}}AFX_DATA_INIT
}


void CAliasEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAliasEdit)
	DDX_Text(pDX, IDC_INPUT, m_sAlias);
	DDX_Text(pDX, IDC_OUTPUT, m_sOutput);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAliasEdit, CDialog)
	//{{AFX_MSG_MAP(CAliasEdit)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAliasEdit message handlers
