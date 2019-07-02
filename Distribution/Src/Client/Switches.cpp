// Switches.cpp : implementation file
//

#include "stdafx.h"
#include "gmud32.h"
#include "Switches.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSwitches dialog


CSwitches::CSwitches(CWnd* pParent /*=NULL*/)
	: CDialog(CSwitches::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSwitches)
	m_CommandChar = _T("");
	m_CommandStacking = FALSE;
	m_ConfirmDisconnect = FALSE;
	m_ForceAnsiReset = FALSE;
	m_LocalEcho = FALSE;
	m_MultilineInput = FALSE;
	m_ShowActivity = FALSE;
	m_ShowCopyNotice = FALSE;
	m_UseCLI = FALSE;
	m_NewWindow = FALSE;
	m_ReviewEnd = FALSE;
	m_MapKeys = FALSE;
	m_ConfirmClose = FALSE;
	//}}AFX_DATA_INIT
}


void CSwitches::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSwitches)
	DDX_Text(pDX, IDC_COMMANDCHAR, m_CommandChar);
	DDV_MaxChars(pDX, m_CommandChar, 1);
	DDX_Check(pDX, IDC_COMMANDSTACKING, m_CommandStacking);
	DDX_Check(pDX, IDC_CONFIRMDISCONNECT, m_ConfirmDisconnect);
	DDX_Check(pDX, IDC_FORCEANSIRESET, m_ForceAnsiReset);
	DDX_Check(pDX, IDC_LOCALECHO, m_LocalEcho);
	DDX_Check(pDX, IDC_MULTILINEINPUT, m_MultilineInput);
	DDX_Check(pDX, IDC_SHOWACTIVITY, m_ShowActivity);
	DDX_Check(pDX, IDC_SHOWCOPYNOTICE, m_ShowCopyNotice);
	DDX_Check(pDX, IDC_USECLI, m_UseCLI);
	DDX_Check(pDX, IDC_NEWWINDOW, m_NewWindow);
	DDX_Check(pDX, IDC_REVIEWEND, m_ReviewEnd);
	DDX_Check(pDX, IDC_MAPKEYS, m_MapKeys);
	DDX_Check(pDX, IDC_CONFIRMCLOSE, m_ConfirmClose);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSwitches, CDialog)
	//{{AFX_MSG_MAP(CSwitches)
	ON_BN_CLICKED(IDC_MULTILINEINPUT, OnMultilineinput)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSwitches message handlers

void CSwitches::OnMultilineinput() 
{
	UpdateData(true);

	if ((m_MultilineInput != GetApp()->m_bMultiline)) {
		AfxMessageBox("To change the multi-line input setting, you will have to start a new session.");
	}
}

BOOL CSwitches::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_CommandChar=GetApp()->m_CommandChar;
	m_CommandStacking=GetApp()->m_bCommandStacking;
	m_ConfirmDisconnect=GetApp()->GetProfileInt("Helpers", "ConfirmDisconnect", 1);
	m_ConfirmClose=GetApp()->GetProfileInt("Helpers", "ConfirmClose", 1);
	m_ForceAnsiReset=GetApp()->m_bANSIReset;
	m_LocalEcho=GetApp()->m_bEcho;
	m_MultilineInput=GetApp()->m_bMultiline;
	m_ShowActivity=GetApp()->m_bShowActivity;
	m_ShowCopyNotice=GetApp()->GetProfileInt("Helpers", "ShowCopy", 1);
	m_UseCLI=GetApp()->m_bUseCLI;
	m_ReviewEnd=GetApp()->m_bReviewEnd;
	m_NewWindow=GetApp()->m_bNewWindow;
	m_MapKeys=GetApp()->m_bMapKeys;

	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSwitches::OnOK() 
{
	if (false == UpdateData(true)) return;

	GetApp()->m_CommandChar=*(LPCSTR)m_CommandChar;
	GetApp()->m_bCommandStacking=m_CommandStacking;
	GetApp()->WriteProfileInt("Helpers", "ConfirmDisconnect", m_ConfirmDisconnect);
	GetApp()->WriteProfileInt("Helpers", "ConfirmClose", m_ConfirmClose);
	GetApp()->m_bANSIReset=m_ForceAnsiReset;
	GetApp()->m_bEcho=m_LocalEcho;
	GetApp()->m_bMultiline=m_MultilineInput;
	GetApp()->m_bShowActivity=m_ShowActivity;
	GetApp()->WriteProfileInt("Helpers", "ShowCopy", m_ShowCopyNotice);
	GetApp()->m_bUseCLI=m_UseCLI;
	GetApp()->m_bReviewEnd=m_ReviewEnd;
	GetApp()->m_bNewWindow=m_NewWindow;
	GetApp()->m_bMapKeys=m_MapKeys;
	
	CDialog::OnOK();
}
