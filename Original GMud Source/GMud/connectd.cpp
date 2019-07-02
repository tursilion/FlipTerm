/////////////////////////////////////////////////////////////////////////////
//
// CConnectDialog dialog
//
// connectd.cpp : implementation file
//

#include "stdafx.h"
#include "gmud32.h"
#include "connectd.h"
#include "world.h"
#include "renamedi.h"
#include "namedial.h"
#include "selarraydlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

CConnectDialog::CConnectDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CConnectDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConnectDialog)
	m_sConnectString = _T("");
	m_sHostName = _T("");
	m_wPort = 0;
	m_sWorldName = _T("");
	m_sDescription = _T("");
	m_sTriggerSetName = _T("");
	m_sMacroSetName = _T("");
	m_sAliasSet = _T("");
	//}}AFX_DATA_INIT

	m_paWorlds=0;
	m_pWorld=0;
	m_bShowConnect=TRUE;
	m_pMacros=0;
	m_pTriggers=0;
	m_pAliases=0;
}

void CConnectDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConnectDialog)
	DDX_Text(pDX, IDC_CONNECTSTRING, m_sConnectString);
	DDX_Text(pDX, IDC_HOSTNAME, m_sHostName);
	DDX_Text(pDX, IDC_PORT, m_wPort);
	DDX_LBString(pDX, IDC_WORLDLIST, m_sWorldName);
	DDX_Text(pDX, IDC_DESCRIPTION, m_sDescription);
	DDX_Text(pDX, IDC_TRIGSETNAME, m_sTriggerSetName);
	DDX_Text(pDX, IDC_MACROSETNAME, m_sMacroSetName);
	DDX_Text(pDX, IDC_ALIAS_SET, m_sAliasSet);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConnectDialog, CDialog)
	//{{AFX_MSG_MAP(CConnectDialog)
	ON_CBN_DBLCLK(IDC_WORLDLIST, OnDblclkWorldlist)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_CBN_SELCHANGE(IDC_WORLDLIST, OnSelchangeWorldlist)
	ON_BN_CLICKED(IDC_DELETE_WORLD, OnDeleteWorld)
	ON_BN_CLICKED(IDC_RENAME_WORLD, OnRenameWorld)
	ON_BN_CLICKED(IDC_CHANGE_TRIGGER, OnChangeTrigger)
	ON_BN_CLICKED(IDC_CHANGE_MACRO, OnChangeMacro)
	ON_BN_CLICKED(IDC_CHANGE_ALIAS, OnChangeAlias)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConnectDialog message handlers

BOOL CConnectDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	ASSERT_VALID(m_paWorlds);	
	ASSERT(m_pTriggers);
	ASSERT(m_pMacros);
	ASSERT(m_pAliases);
	PopulateList();
	CenterWindow();	
	int iLastIndex=AfxGetApp()->GetProfileInt("World List","selected",0);
	CListBox *pList = ((CListBox *)GetDlgItem(IDC_WORLDLIST));
	if(pList && pList->GetCount())
	{
		pList->SetCurSel(iLastIndex);
		pList->GetText(iLastIndex,m_sWorldName);
	}
	CWorld *pWorld = FindWorld(m_sWorldName);
	if(pWorld)
		ShowWorld(pWorld);
	if(!m_bShowConnect)
	{
		GetDlgItem(IDOK)->ShowWindow(SW_HIDE);
		SetWindowText("Edit World List");
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CConnectDialog::OnDblclkWorldlist() 
{
	if(m_bShowConnect)
		OnOK();
}

void CConnectDialog::OnOK() 
{
	UpdateData(TRUE);
	int iSel = ((CListBox *)GetDlgItem(IDC_WORLDLIST))->GetCurSel();
	if(iSel!=LB_ERR)
		AfxGetApp()->WriteProfileInt("World List","selected",iSel);
	m_pWorld = FindWorld(m_sWorldName);
	SaveWorld();
	if(m_pWorld || !m_bShowConnect)
		CDialog::OnOK();
}

void CConnectDialog::OnAdd() 
{
	// save the original world
	SaveWorld();

	CNameDialog dlg(this);
	dlg.m_sPrompt = "Enter a Name for the new world";
	if(dlg.DoModal()==IDOK && dlg.m_sName!="")
	{
		CWorld *pWorld = FindWorld(dlg.m_sName);
		if(pWorld)
		{
			AfxMessageBox("That world name already exists, choose another name.");
			return;
		}
		pWorld = new CWorld;
		pWorld->m_sName=dlg.m_sName;
		m_paWorlds->Add(pWorld);
		m_pWorld = pWorld;
		PopulateList();
		ShowWorld(pWorld);
	}
}

void CConnectDialog::GetWorld(CWorld *pWorld)
{
	UpdateData(TRUE);
	pWorld->m_sHostName = m_sHostName;
	pWorld->m_wPort = (WORD) m_wPort;
	pWorld->m_sConnectString = m_sConnectString;
	pWorld->m_sDescription = m_sDescription;
	pWorld->m_sTriggerList = m_sTriggerSetName;
	pWorld->m_sMacroList = m_sMacroSetName;
	pWorld->m_sAliasList = m_sAliasSet;
}

CConnectDialog::PopulateList()
{
	CListBox *pList = (CListBox *)GetDlgItem(IDC_WORLDLIST);
	ASSERT(pList);

	while(pList->DeleteString(0)>0)
		;;

	for(int loop=0;loop<m_paWorlds->GetSize();loop++)
	{
		CWorld *pWorld=(CWorld *)m_paWorlds->GetAt(loop);
		pList->AddString(pWorld->m_sName);
	}

 	return TRUE;
}

CWorld *CConnectDialog::FindWorld(CString sName)
{
	CWorld *pWorld=0;

	for(int loop=0;loop<m_paWorlds->GetSize();loop++)
	{
		pWorld=(CWorld *)m_paWorlds->GetAt(loop);
		if(pWorld->m_sName.CompareNoCase(sName)==0)
			return pWorld;
	}
	return 0;
}

int CConnectDialog::FindWorldIndex(CString sName)
{
	CWorld *pWorld=0;

	for(int loop=0;loop<m_paWorlds->GetSize();loop++)
	{
		pWorld=(CWorld *)m_paWorlds->GetAt(loop);
		if(pWorld->m_sName.CompareNoCase(sName)==0)
			return loop;
	}
	return -1;
}

void CConnectDialog::ShowWorld(CWorld *pWorld)
{
	m_sWorldName=pWorld->m_sName;
	m_sHostName=pWorld->m_sHostName ;
	m_wPort=pWorld->m_wPort ;
	m_sConnectString=pWorld->m_sConnectString ;

	m_sDescription=pWorld->m_sDescription ;
	m_sTriggerSetName=pWorld->m_sTriggerList;
	m_sMacroSetName=pWorld->m_sMacroList;
	m_sAliasSet=pWorld->m_sAliasList;

	UpdateData(FALSE);
}

void CConnectDialog::OnSelchangeWorldlist() 
{
	CWorld *pWorld = FindWorld(m_sWorldName);
	if(pWorld)
		GetWorld(pWorld);
	CListBox *pList = (CListBox *)GetDlgItem(IDC_WORLDLIST);
	ASSERT_VALID(pList);
	
	pList->GetText(pList->GetCurSel(),m_sWorldName);
//	UpdateData(TRUE);
	pWorld = FindWorld(m_sWorldName);
	ShowWorld(pWorld);
}

BOOL CConnectDialog::SaveWorld()
{
	UpdateData(TRUE);
	CWorld *pWorld = FindWorld(m_sWorldName);
	if(pWorld)
		GetWorld(pWorld);
	else
		TRACE("\nCouldn't find world [%s].",m_sWorldName);
	return TRUE;
}	

void CConnectDialog::OnDeleteWorld() 
{
	UpdateData(TRUE);
	CWorld *pWorld = FindWorld(m_sWorldName);
	if(pWorld
	&&
	AfxMessageBox("Delete world "+m_sWorldName+" ?",MB_OKCANCEL)==IDOK)
	{
		m_paWorlds->RemoveAt(FindWorldIndex(m_sWorldName));
		delete pWorld;
		PopulateList();
		m_sConnectString = _T("");
		m_sHostName = _T("");
		m_wPort = 0;
		m_sWorldName = _T("");
		m_sDescription="";
		m_sTriggerSetName="Default";
		m_sMacroSetName="Default";

		UpdateData(FALSE);
	}
}

void CConnectDialog::OnRenameWorld() 
{
	SaveWorld();
	CWorld *pWorld = FindWorld(m_sWorldName);
	if(pWorld)
	{
		CRenameDialog dlg(this);
		dlg.m_sOldName = pWorld->m_sName;
		if(dlg.DoModal()==IDOK)
		{
			pWorld->m_sName = dlg.m_sNewName;
			ShowWorld(pWorld);
			PopulateList();
			UpdateData(FALSE);
		}
	}
	else
		AfxMessageBox("Select a world to rename, first.");
}

void CConnectDialog::OnChangeTrigger() 
{
	SaveWorld();
	CWorld *pWorld = FindWorld(m_sWorldName);
	if(pWorld)
	{
		CSelArrayDialog dlg;
		dlg.Setup(m_pTriggers,TYPE_TRIGGER,m_paWorlds);
		dlg.m_sSelected=pWorld->m_sTriggerList;
		dlg.m_sTitle="Define / Select a Trigger set.";
		dlg.DoModal();
		pWorld->m_sTriggerList = dlg.m_sSelected;
		ShowWorld(pWorld);
	}
	else
		AfxMessageBox("Select a world first.");
}

void CConnectDialog::OnChangeMacro() 
{
	SaveWorld();
	CWorld *pWorld = FindWorld(m_sWorldName);
	if(pWorld)
	{
		CSelArrayDialog dlg;
		dlg.Setup(m_pMacros,TYPE_MACRO,m_paWorlds);
		dlg.m_sSelected=pWorld->m_sMacroList;
		dlg.m_sTitle="Define / Select a Macro set.";
		dlg.DoModal();
		pWorld->m_sMacroList = dlg.m_sSelected;
		ShowWorld(pWorld);
	}
	else
		AfxMessageBox("Select a world first.");
}

void CConnectDialog::OnChangeAlias() 
{
	SaveWorld();
	CWorld *pWorld = FindWorld(m_sWorldName);
	if(pWorld)
	{
		CSelArrayDialog dlg;
		dlg.Setup(m_pAliases,TYPE_ALIAS,m_paWorlds);
		dlg.m_sSelected=pWorld->m_sAliasList;
		dlg.m_sTitle="Define / Select an Alias set.";
		dlg.DoModal();
		pWorld->m_sAliasList = dlg.m_sSelected;
		ShowWorld(pWorld);
	}
	else
		AfxMessageBox("Select a world first.");
}
