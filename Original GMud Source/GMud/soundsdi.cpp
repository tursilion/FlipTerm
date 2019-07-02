// soundsdi.cpp : implementation file
//

#include "stdafx.h"
#include "gmud32.h"
#include "soundsdi.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSoundsDialog dialog


CSoundsDialog::CSoundsDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSoundsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSoundsDialog)
	m_bBell = FALSE;
	m_bTriggerSound = FALSE;
	m_bConnectBeep = FALSE;
	//}}AFX_DATA_INIT
}


void CSoundsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSoundsDialog)
	DDX_Check(pDX, IDC_BELL_SOUND, m_bBell);
	DDX_Check(pDX, IDC_TRIGGER_SOUND, m_bTriggerSound);
	DDX_Check(pDX, IDC_CONNECT_BEEP, m_bConnectBeep);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSoundsDialog, CDialog)
	//{{AFX_MSG_MAP(CSoundsDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSoundsDialog message handlers

BOOL CSoundsDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CenterWindow();	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
