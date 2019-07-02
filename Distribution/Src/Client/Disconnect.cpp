// Disconnect.cpp : implementation file
//

#include "stdafx.h"
#include "gmud32.h"
#include "Disconnect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDisconnect dialog


CDisconnect::CDisconnect(CWnd* pParent /*=NULL*/)
	: CDialog(CDisconnect::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDisconnect)
	m_Text = _T("");
	m_Check = FALSE;
	//}}AFX_DATA_INIT
}


void CDisconnect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDisconnect)
	DDX_Text(pDX, IDC_STATIC1, m_Text);
	DDX_Check(pDX, IDC_CHECK1, m_Check);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDisconnect, CDialog)
	//{{AFX_MSG_MAP(CDisconnect)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDisconnect message handlers
