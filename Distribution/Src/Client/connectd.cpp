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
#include "utils.h"
#include "textclr.h"

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
	m_bAutoLog = FALSE;
	m_Reconnect = FALSE;
	m_bAutoConnect = FALSE;
	m_b7Bit = TRUE;
	m_FontName = _T("");
	//}}AFX_DATA_INIT

	m_paWorlds=0;
	m_pWorld=0;
	m_bShowConnect=TRUE;
	m_pMacros=0;
	m_pTriggers=0;
	m_pAliases=0;
	csWorld="";
	m_sAutoLog="";
	m_nAutologMode=0;
}

void CConnectDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConnectDialog)
	DDX_Control(pDX, IDC_RESET, m_ctrlReset);
	DDX_Control(pDX, IDC_FONT_SETTING, m_ctrlFontName);
	DDX_Control(pDX, IDC_CHANGEFONT, m_ctrlFont);
	DDX_Control(pDX, IDC_CHANGECOLOR, m_ctrlColor);
	DDX_Control(pDX, IDC_7BIT, m_ctrl7Bit);
	DDX_Control(pDX, IDC_AUTOCONNECT, m_ctrlAutoConnect);
	DDX_Control(pDX, IDC_AUTORECONNECT, m_ctrlReconnect);
	DDX_Control(pDX, IDC_AUTOLOG, m_ctrlAutoLog);
	DDX_Control(pDX, IDC_RENAME_WORLD, m_ctrlRename);
	DDX_Control(pDX, IDC_DELETE_WORLD, m_ctrlDelete);
	DDX_Control(pDX, IDC_CHANGE_TRIGGER, m_ctrlTrigger);
	DDX_Control(pDX, IDC_CHANGE_MACRO, m_ctrlMacro);
	DDX_Control(pDX, IDC_CHANGE_ALIAS, m_ctrlAlias);
	DDX_Control(pDX, IDC_LOGAPPEND, m_ctrlAppend);
	DDX_Control(pDX, IDC_IRC, m_ctrlIRC);
	DDX_Control(pDX, IDC_LOGNEW, m_ctrlNew);
	DDX_Control(pDX, IDC_LOGDAILY, m_ctrlDaily);
	DDX_Control(pDX, IDC_DESCRIPTION, m_ctrlDescription);
	DDX_Control(pDX, IDC_CONNECTSTRING, m_ctrlConnectString);
	DDX_Control(pDX, IDC_PORT, m_ctrlPort);
	DDX_Control(pDX, IDC_HOSTNAME, m_ctrlHostName);
	DDX_Text(pDX, IDC_CONNECTSTRING, m_sConnectString);
	DDX_Text(pDX, IDC_HOSTNAME, m_sHostName);
	DDX_Text(pDX, IDC_PORT, m_wPort);
	DDX_LBString(pDX, IDC_WORLDLIST, m_sWorldName);
	DDX_Text(pDX, IDC_DESCRIPTION, m_sDescription);
	DDX_Text(pDX, IDC_TRIGSETNAME, m_sTriggerSetName);
	DDX_Text(pDX, IDC_MACROSETNAME, m_sMacroSetName);
	DDX_Text(pDX, IDC_ALIAS_SET, m_sAliasSet);
	DDX_Check(pDX, IDC_AUTOLOG, m_bAutoLog);
	DDX_Check(pDX, IDC_AUTORECONNECT, m_Reconnect);
	DDX_Check(pDX, IDC_AUTOCONNECT, m_bAutoConnect);
	DDX_Check(pDX, IDC_7BIT, m_b7Bit);
	DDX_Text(pDX, IDC_FONT_SETTING, m_FontName);
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
	ON_BN_CLICKED(IDC_AUTOLOG, OnAutolog)
	ON_BN_CLICKED(IDC_CHANGEFONT, OnChangefont)
	ON_BN_CLICKED(IDC_CHANGECOLOR, OnChangecolor)
	ON_BN_CLICKED(IDC_RESET, OnReset)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_IRC, &CConnectDialog::OnBnClickedIrc)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConnectDialog message handlers

BOOL CConnectDialog::OnInitDialog() 
{
	CWorld *pWorld=NULL;

	CDialog::OnInitDialog();
	ASSERT_VALID(m_paWorlds);	
	ASSERT(m_pTriggers);
	ASSERT(m_pMacros);
	ASSERT(m_pAliases);
	PopulateList();
	CenterWindow();	
	m_ctrlConnectString.EnableWindow(false);
	m_ctrlDescription.EnableWindow(false);
	m_ctrlHostName.EnableWindow(false);
	m_ctrlPort.EnableWindow(false);
	m_ctrlAlias.EnableWindow(false);
	m_ctrlMacro.EnableWindow(false);
	m_ctrlTrigger.EnableWindow(false);
	m_ctrlDelete.EnableWindow(false);
	m_ctrlRename.EnableWindow(false);
	m_ctrlAutoLog.EnableWindow(false);
	m_ctrlAppend.EnableWindow(false);
	m_ctrlIRC.EnableWindow(false);
	m_ctrlDaily.EnableWindow(false);
	m_ctrlNew.EnableWindow(false);
	m_ctrlReconnect.EnableWindow(false);
	m_ctrlAutoConnect.EnableWindow(false);
	m_ctrl7Bit.EnableWindow(false);
	m_ctrlColor.EnableWindow(false);
	m_ctrlFont.EnableWindow(false);
	m_ctrlReset.EnableWindow(false);

	int iLastIndex=GetProfileInt("World List","selected",0);
	
	CListBox *pList = ((CListBox *)GetDlgItem(IDC_WORLDLIST));
	if (strlen(m_sWorldName)) {
		if(pList && pList->GetCount())
		{
			pList->SetCurSel(iLastIndex);
			pList->GetText(iLastIndex,m_sWorldName);
		}
		pWorld = FindWorld(m_sWorldName);
	}
	if(pWorld)
		ShowWorld(pWorld);
	if(!m_bShowConnect)
	{
		GetDlgItem(IDOK)->ShowWindow(SW_HIDE);
		SetWindowText("Edit World List");
	}

	if (csWorld != "") {
		m_pWorld = FindWorld(csWorld);
		EndDialog(IDOK);
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
	if(iSel!=LB_ERR) {
		GetApp()->WriteProfileInt("World List","selected",iSel);
	}
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
		m_ctrlHostName.SetFocus();
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
	pWorld->m_bReconnect = m_Reconnect ? 1 : 0;
	pWorld->m_bAutoConnect = m_bAutoConnect ? 1:0;
	pWorld->m_bIs7Bit = m_b7Bit ? 1:0;

	if (m_bAutoLog) {
		// Ensure that m_sAutoLog doesn't contain any weird chars
		pWorld->m_sAutologFile=m_sAutoLog;
	} else {
		pWorld->m_sAutologFile="";
	}

	if (m_ctrlIRC.GetCheck()) {
		pWorld->m_bIsIRC = 1;
	} else {
		pWorld->m_bIsIRC = 0;
	}
	
	if (m_ctrlAppend.GetCheck()) {
		pWorld->m_nAutologMode=0;
		m_nAutologMode=0;
	}

	if (m_ctrlNew.GetCheck()) {
		pWorld->m_nAutologMode=1;
		m_nAutologMode=1;
	}

	if (m_ctrlDaily.GetCheck()) {
		pWorld->m_nAutologMode=2;
		m_nAutologMode=2;
	}
}

int CConnectDialog::PopulateList()
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
	if (NULL == pWorld) {
		return;
	}

	m_ctrlConnectString.EnableWindow(true);
	m_ctrlDescription.EnableWindow(true);
	m_ctrlHostName.EnableWindow(true);
	m_ctrlPort.EnableWindow(true);
	m_ctrlAlias.EnableWindow(true);
	m_ctrlMacro.EnableWindow(true);
	m_ctrlTrigger.EnableWindow(true);
	m_ctrlDelete.EnableWindow(true);
	m_ctrlRename.EnableWindow(true);
	m_ctrlAutoLog.EnableWindow(true);
	m_ctrlReconnect.EnableWindow(true);
	m_ctrlAutoConnect.EnableWindow(true);	
	m_ctrl7Bit.EnableWindow(true);
	m_ctrlIRC.EnableWindow(true);
	m_ctrlColor.EnableWindow(true);
	m_ctrlFont.EnableWindow(true);
	m_ctrlReset.EnableWindow(true);

	m_sWorldName=pWorld->m_sName;
	m_sHostName=pWorld->m_sHostName ;
	m_wPort=pWorld->m_wPort ;
	m_sConnectString=pWorld->m_sConnectString ;

	m_sDescription=pWorld->m_sDescription ;
	m_sTriggerSetName=pWorld->m_sTriggerList;
	m_sMacroSetName=pWorld->m_sMacroList;
	m_sAliasSet=pWorld->m_sAliasList;

	m_Reconnect = pWorld->m_bReconnect ? true : false;
	m_bAutoConnect = pWorld->m_bAutoConnect ? true:false;
	m_b7Bit = pWorld->m_bIs7Bit ? true:false;

	m_FontName=pWorld->m_LogFont.lfFaceName;

	if ((pWorld->m_colorBackGround != -1) || (pWorld->m_colorForeGround != -1) || (pWorld->m_bAnsi != -2)) {
		m_ctrlColor.SetWindowText("Color*");
	} else {
		m_ctrlColor.SetWindowText("Color");
	}

	if (pWorld->m_bIsIRC) {
		m_ctrlIRC.SetCheck(true);
	} else {
		m_ctrlIRC.SetCheck(false);
	}

	m_sAutoLog=pWorld->m_sAutologFile;
	if (m_sAutoLog != "") {
		CString tmp;

		if (m_sAutoLog.GetLength() > 15) {
			tmp.Format("Log: ...%s", m_sAutoLog.Right(14));
		} else {
			tmp.Format("Log: %s", m_sAutoLog);
		}
		m_ctrlAutoLog.SetWindowText(tmp);
		m_bAutoLog=true;

		m_ctrlAppend.EnableWindow(true);
		m_ctrlNew.EnableWindow(true);
		m_ctrlDaily.EnableWindow(true);

		m_nAutologMode=pWorld->m_nAutologMode;
		m_ctrlNew.SetCheck(false);
		m_ctrlDaily.SetCheck(false);
		m_ctrlAppend.SetCheck(false);
		switch (m_nAutologMode) {
		case 1:	m_ctrlNew.SetCheck(true); break;
		case 2: m_ctrlDaily.SetCheck(true); break;
		default:m_ctrlAppend.SetCheck(true); break;
		}
	} else {
		m_ctrlAutoLog.SetWindowText("Enable Autolog");
		m_bAutoLog=false;
		m_ctrlAppend.EnableWindow(false);
		m_ctrlDaily.EnableWindow(false);
		m_ctrlNew.EnableWindow(false);
	}

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
	pWorld = FindWorld(m_sWorldName);
	ShowWorld(pWorld);
}

BOOL CConnectDialog::SaveWorld()
{
	UpdateData(TRUE);
	CWorld *pWorld = FindWorld(m_sWorldName);
	if(pWorld) {
		GetWorld(pWorld);
	}
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
		m_FontName = _T("");
		m_sDescription="";
		m_sTriggerSetName="Default";
		m_sMacroSetName="Default";
		m_sAliasSet="Default";
		m_Reconnect=false;
		m_bAutoConnect=false;
		m_b7Bit = true;

		m_ctrlConnectString.EnableWindow(false);
		m_ctrlDescription.EnableWindow(false);
		m_ctrlHostName.EnableWindow(false);
		m_ctrlPort.EnableWindow(false);
		m_ctrlAlias.EnableWindow(false);
		m_ctrlMacro.EnableWindow(false);
		m_ctrlTrigger.EnableWindow(false);
		m_ctrlDelete.EnableWindow(false);
		m_ctrlRename.EnableWindow(false);
		m_ctrlAutoLog.EnableWindow(false);
		m_ctrlAppend.EnableWindow(false);
		m_ctrlIRC.EnableWindow(false);
		m_ctrlDaily.EnableWindow(false);
		m_ctrlNew.EnableWindow(false);
		m_ctrlReconnect.EnableWindow(false);
		m_ctrlAutoConnect.EnableWindow(false);
		m_ctrl7Bit.EnableWindow(false);
		m_ctrlColor.EnableWindow(false);
		m_ctrlFont.EnableWindow(false);
		m_ctrlReset.EnableWindow(false);

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
	} else {
		AfxMessageBox("Select a world to rename, first.");
	}
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
	} else {
		AfxMessageBox("Select a world first.");
	}
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
	} else {
		AfxMessageBox("Select a world first.");
	}
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
	} else {
		AfxMessageBox("Select a world first.");
	}
}

void CConnectDialog::OnCancel() 
{
	SaveWorld();
	CDialog::OnCancel();
}

void CConnectDialog::OnAutolog() 
{
	CString fn;

	if (m_sAutoLog.GetLength() == 0) {
		CString tmp;
		tmp=m_sWorldName;
		tmp.Replace(':', '-');
		tmp.Replace('\\', '-');
		fn.Format("%s\\%s.log", GetApp()->m_sLogFolder, tmp);
	} else {
		fn=m_sAutoLog;
	}
	CFileDialog dlg(false, "log", fn, OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, "Log Files|*.log;*.txt||", this);

	if (m_ctrlAutoLog.GetCheck()) {
		// Browse for a filename - put in m_sAutoLog
		dlg.m_ofn.lpstrTitle="Select log file for this world";
		if (IDOK == dlg.DoModal()) {
			CString tmp;

			m_sAutoLog=dlg.GetPathName();
			if (m_sAutoLog.GetLength() > 15) {
				tmp.Format("Log: ...%s", m_sAutoLog.Right(14));
			} else {
				tmp.Format("Log: %s", m_sAutoLog);
			}
			m_ctrlAutoLog.SetWindowText(tmp);
			m_bAutoLog=true;
			m_ctrlAppend.EnableWindow(true);
			m_ctrlDaily.EnableWindow(true);
			m_ctrlNew.EnableWindow(true);
			
			m_ctrlNew.SetCheck(false);
			m_ctrlDaily.SetCheck(false);
			m_ctrlAppend.SetCheck(false);
			switch (m_nAutologMode) {
			case 1: m_ctrlNew.SetCheck(true); break;
			case 2: m_ctrlDaily.SetCheck(true); break;
			default:m_ctrlAppend.SetCheck(true); break;
			}
		} else {
			m_bAutoLog=false;
			m_ctrlAutoLog.SetCheck(false);
			m_ctrlAppend.EnableWindow(false);
			m_ctrlDaily.EnableWindow(false);
			m_ctrlNew.EnableWindow(false);
		}
	} else {
		m_ctrlAutoLog.SetWindowText("Enable Autolog");
		m_bAutoLog=false;
		m_ctrlAppend.EnableWindow(false);
		m_ctrlDaily.EnableWindow(false);
		m_ctrlNew.EnableWindow(false);
	}		
}

void CConnectDialog::OnChangefont() 
{
	UpdateData(true);
	CWorld *pWorld = FindWorld(m_sWorldName);
	if (!pWorld) return;
	
	LOGFONT *pFont;

	if (stricmp(pWorld->m_LogFont.lfFaceName, "default")==0) {
		pFont=&GetApp()->m_LogFont;
	} else {
		pFont=&pWorld->m_LogFont;
	}

	CFontDialog dlg(pFont,CF_FIXEDPITCHONLY|CF_FORCEFONTEXIST|CF_INITTOLOGFONTSTRUCT|CF_SCREENFONTS);
	if(dlg.DoModal()==IDOK)
	{
		memcpy(&pWorld->m_LogFont, dlg.m_cf.lpLogFont, sizeof(LOGFONT));
		m_FontName=pWorld->m_LogFont.lfFaceName;
		UpdateData(false);
		GetApp()->m_pMainWnd->SendMessageToDescendants(WM_FONTCHANGED);
	}
}

void CConnectDialog::OnChangecolor() 
{
	UpdateData(true);
	CWorld *pWorld = FindWorld(m_sWorldName);
	if (!pWorld) return;

	CTextClrDlg dlg;
	if (pWorld->m_colorBackGround != -1) {
		dlg.m_colorBackGround=pWorld->m_colorBackGround;
	} else {
		dlg.m_colorBackGround=GetApp()->m_colorBackGround;
	}
	if (pWorld->m_colorForeGround != -1) {
		dlg.m_colorForeGround=pWorld->m_colorForeGround;
	} else {
		dlg.m_colorForeGround=GetApp()->m_colorForeGround;
	}
	if (pWorld->m_bAnsi != -2) {
		dlg.m_bAnsi = pWorld->m_bAnsi ? true:false;
	} else {
		dlg.m_bAnsi=GetApp()->m_bAnsi;
	}

	if(dlg.DoModal()==IDOK)
	{
		pWorld->m_colorBackGround=dlg.m_colorBackGround;
		pWorld->m_colorForeGround=dlg.m_colorForeGround;
		pWorld->m_bAnsi=dlg.m_bAnsi;
		GetApp()->m_pMainWnd->SendMessageToDescendants(WM_COLORCHANGED);
	}
}

void CConnectDialog::OnReset() 
{
	UpdateData(true);
	CWorld *pWorld = FindWorld(m_sWorldName);
	if (!pWorld) return;

	if (IDYES == AfxMessageBox("Reset font settings for this world to default?", MB_ICONQUESTION | MB_YESNO)) {
		strcpy(pWorld->m_LogFont.lfFaceName, "Default");	// use program defaults
		pWorld->m_colorBackGround=-1;				
		pWorld->m_colorForeGround=-1;
		pWorld->m_bAnsi=-2;
		ShowWorld(pWorld);
		GetApp()->m_pMainWnd->SendMessageToDescendants(WM_FONTCHANGED);
		GetApp()->m_pMainWnd->SendMessageToDescendants(WM_COLORCHANGED);
	}
}

void CConnectDialog::OnBnClickedIrc()
{
	if (m_ctrlIRC.GetCheck()) {
		AfxMessageBox("Warning: IRC support is limited to basic parsing and PING replies. You will need to log in manually!\r\nUse this command to register:\r\n\r\n/NICK <your nick>\r\n\r\nUse /JOIN <channel> for to join a channel - join only ONE channel per window!", MB_OK);
	}
}
