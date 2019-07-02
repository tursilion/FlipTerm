// triggerd.cpp : implementation file
//

#include "stdafx.h"
#include "gmud32.h"
#include "triggerd.h"
#include "trigger.h"
#include "mmsystem.h"
#include "selarray.h"
#include "namedial.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern unsigned long colormap[];

BEGIN_MESSAGE_MAP(CTriggerDialog, CDialog)
	//{{AFX_MSG_MAP(CTriggerDialog)
	ON_BN_CLICKED(IDC_BROWSE_FILE, OnBrowseFile)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDADD, OnAdd)
	ON_LBN_SELCHANGE(IDC_TRIGGERLIST, OnSelchangeTriggerlist)
	ON_BN_CLICKED(IDC_TEST_SOUND, OnTestSound)
	ON_BN_CLICKED(IDC_CHANGECOLOR, OnChangecolor)
	ON_BN_CLICKED(IDC_RENAME, OnRename)
	ON_BN_CLICKED(IDC_NEWANTIIDLE, OnNewantiidle)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTriggerDialog dialog


CTriggerDialog::CTriggerDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CTriggerDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTriggerDialog)
	m_sFileName = _T("");
	m_sMudText = _T("");
	m_sTrigger = _T("");
	//}}AFX_DATA_INIT

	m_pTriggers=0;
	m_OldTrigger="";
}

void CTriggerDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTriggerDialog)
	DDX_Control(pDX, IDC_RENAME, m_Rename);
	DDX_Control(pDX, IDC_TRIGGERLIST, m_Triggers);
	DDX_Control(pDX, IDC_CHANGECOLOR, m_ChangeCol);
	DDX_Control(pDX, IDC_TEST_SOUND, m_Test);
	DDX_Control(pDX, IDC_PLAYFILE, m_PlayFile);
	DDX_Control(pDX, IDC_MUDTEXT, m_cText);
	DDX_Control(pDX, IDC_FILENAME, m_cFilename);
	DDX_Control(pDX, IDC_BROWSE_FILE, m_Browse);
	DDX_Control(pDX, IDC_DELETE, m_Delete);
	DDX_Text(pDX, IDC_FILENAME, m_sFileName);
	DDX_Text(pDX, IDC_MUDTEXT, m_sMudText);
	DDX_LBString(pDX, IDC_TRIGGERLIST, m_sTrigger);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CTriggerDialog message handlers

void CTriggerDialog::EnableControls() {
	CWnd *pCombo=GetDlgItem(IDC_COMBO1);
	if (pCombo) pCombo->EnableWindow(true);	// can't use an automatic var due to the subclassing
	m_Delete.EnableWindow(true);
	m_ChangeCol.EnableWindow(true);
	m_Browse.EnableWindow(true);
	m_cFilename.EnableWindow(true);
	m_cText.EnableWindow(true);
	m_PlayFile.EnableWindow(true);
	m_Test.EnableWindow(true);
	m_Rename.EnableWindow(true);
}

void CTriggerDialog::DisableControls() {
	CWnd *pCombo=GetDlgItem(IDC_COMBO1);
	if (pCombo) pCombo->EnableWindow(false);
	m_Delete.EnableWindow(false);
	m_ChangeCol.EnableWindow(false);
	m_Browse.EnableWindow(false);
	m_cFilename.EnableWindow(false);
	m_cText.EnableWindow(false);
	m_PlayFile.EnableWindow(false);
	m_Test.EnableWindow(false);
	m_Rename.EnableWindow(false);
}

BOOL CTriggerDialog::SetTriggers(CSelArray *pArray)
{
	m_pTriggers=pArray;
	return TRUE;
}

CTrigger *CTriggerDialog::GetTrigger(int index)
{	
	ASSERT_VALID(m_pTriggers);
	ASSERT(index >= 0 && index < m_pTriggers->GetSize());
	return (CTrigger *)m_pTriggers->Get(index);
}

CTrigger *CTriggerDialog::GetTrigger(CString sName)
{
	return (CTrigger *)m_pTriggers->Get(sName);
}

int CTriggerDialog::GetNumTriggers()
{
 	return m_pTriggers->GetSize();
}

void CTriggerDialog::OnBrowseFile() 
{
	UpdateData(TRUE);
	CFileDialog dlg(TRUE,
					NULL,
					m_sFileName,
					NULL, // flags
					"Wave Files (*.wav)|*.wav||",
					this // parent window
					);
	if(dlg.DoModal()==IDOK)
	{
		m_sFileName=dlg.GetPathName();
	}
	UpdateData(FALSE);
}

void CTriggerDialog::OnDelete() 
{
	UpdateData(TRUE);
	if(m_sTrigger=="")
	{
		AfxMessageBox("You must select a trigger first.");
	}
	else
	{
		if (IDYES == AfxMessageBox("Really delete this trigger?", MB_ICONQUESTION | MB_YESNO)) {
			m_pTriggers->Delete(m_sTrigger);
		} else {
			return;
		}
	}
	
	DisableControls();
	m_Triggers.ResetContent();
	m_OldTrigger="";
	for(int loop=0;loop<GetNumTriggers();loop++)
	{
		m_Triggers.AddString(GetTrigger(loop)->GetName());
	}
}

void CTriggerDialog::OnOK() 
{
	UpdateData(TRUE);
	CTrigger *pTrigger = GetTrigger(m_sTrigger);
	if(pTrigger)
	{
		pTrigger->m_sFileName  = m_sFileName;
		pTrigger->m_sMudOutput = m_sMudText;
		if (m_ChangeCol.GetCheck()) {
			pTrigger->m_fgColor=m_comboForeground.GetCurSel()+1;
		} else {
			pTrigger->m_fgColor=0;
		}
	}
	CDialog::OnOK();
}

BOOL CTriggerDialog::OnInitDialog() 
{
	VERIFY(m_comboForeground.SubclassDlgItem(IDC_COMBO1, this));

	ASSERT_VALID(m_pTriggers);
	CDialog::OnInitDialog();
	
	for(int loop=0;loop<GetNumTriggers();loop++)
	{
		m_Triggers.AddString(GetTrigger(loop)->GetName());
	}
	
	int iIndex=0;
	// We just manually add the colors available to the ANSI color routine
	for (iIndex=0; iIndex<16; iIndex++) {
		m_comboForeground.AddColorItem(colormap[iIndex]);
	}

	CenterWindow();	
	DisableControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTriggerDialog::OnAdd() 
{
	CNameDialog Dlg;
	CTrigger *pTrigger;

	Dlg.m_sPrompt="Please enter the text for this new trigger.";
	if (IDOK == Dlg.DoModal()) {
		pTrigger = GetTrigger(Dlg.m_sName);
		if (pTrigger) {
			AfxMessageBox("That trigger already exists! Triggers are no longer case-sensitive.", MB_ICONSTOP|MB_OK);
			return;
		}

		pTrigger=new CTrigger;
		pTrigger->SetName(Dlg.m_sName);
		m_pTriggers->Add(pTrigger);
		DisableControls();
		m_Triggers.ResetContent();
		m_OldTrigger="";
		for(int loop=0;loop<GetNumTriggers();loop++)
		{
			m_Triggers.AddString(GetTrigger(loop)->GetName());
		}
	}
}

void CTriggerDialog::OnSelchangeTriggerlist() 
{
	UpdateData(TRUE);
	CTrigger *pTrigger = GetTrigger(m_OldTrigger);
	if(pTrigger)
	{
		pTrigger->m_sFileName  = m_sFileName;
		pTrigger->m_sMudOutput = m_sMudText;
		if (m_ChangeCol.GetCheck()) {
			pTrigger->m_fgColor=m_comboForeground.GetCurSel()+1;
		} else {
			pTrigger->m_fgColor=0;
		}
	}

	pTrigger=GetTrigger(m_sTrigger);
	if(pTrigger)
	{
		m_sFileName=pTrigger->m_sFileName ;
		m_sMudText=pTrigger->m_sMudOutput;
		if (pTrigger->m_fgColor > 0) {
			m_colorForeGround=pTrigger->m_fgColor-1;
			m_comboForeground.SetCurSel(m_colorForeGround);
			m_comboForeground.EnableWindow(true);
			m_ChangeCol.SetCheck(true);
		} else {
			m_comboForeground.SetCurSel(0);
			m_comboForeground.EnableWindow(false);
			m_ChangeCol.SetCheck(false);
		}
		EnableControls();
		m_OldTrigger=m_sTrigger;
	}
	
	UpdateData(FALSE);
}

void CTriggerDialog::OnTestSound() 
{
	UpdateData(TRUE);
	sndPlaySound(m_sFileName,SND_ASYNC);
}

void CTriggerDialog::OnChangecolor() 
{
	if (m_ChangeCol.GetCheck()) {
		m_comboForeground.EnableWindow(true);
	} else {
		m_comboForeground.EnableWindow(false);
	}
}

void CTriggerDialog::OnRename() 
{
	// Rename the macro
	CNameDialog Dlg;
	int nIdleTrigger=-1;

	UpdateData(TRUE);
	CTrigger *pTrigger = GetTrigger(m_sTrigger);

	if (NULL == pTrigger) {
		return;
	}

	nIdleTrigger=pTrigger->GetName().Find(ANTIIDLETRIGGERTEXT);

	if (nIdleTrigger > -1) {
		Dlg.m_sPrompt="Enter the number of minutes of idle.";
		int n=atoi(m_sTrigger.Mid(nIdleTrigger+ANTIIDLETRIGGERLENGTH));
		Dlg.m_sName.Format("%d", n);
	} else {
		Dlg.m_sPrompt="Update the trigger text.";
		Dlg.m_sName=m_sTrigger;
	}

	if (IDOK != Dlg.DoModal()) {
		return;
	}

	if (nIdleTrigger > -1) {
		int n=atoi(Dlg.m_sName);
		if (n<=0) {
			AfxMessageBox("Duration must be a positive number - change ignored.");
			return;
		}
		m_sTrigger=ANTIIDLETRIGGERTEXT;
		m_sTrigger+=Dlg.m_sName;
		m_sTrigger+=" mins.";
	} else {
		m_sTrigger=Dlg.m_sName;
	}

	pTrigger->SetName(m_sTrigger);
	pTrigger->m_sFileName  = m_sFileName;
	pTrigger->m_sMudOutput = m_sMudText;

	if (m_ChangeCol.GetCheck()) {
		pTrigger->m_fgColor=m_comboForeground.GetCurSel()+1;
	} else {
		pTrigger->m_fgColor=0;
	}

	DisableControls();
	m_Triggers.ResetContent();
	m_OldTrigger="";
	for(int loop=0;loop<GetNumTriggers();loop++)
	{
		m_Triggers.AddString(GetTrigger(loop)->GetName());
	}
}

void CTriggerDialog::OnNewantiidle() 
{
	CNameDialog Dlg;
	CTrigger *pTrigger;

	// First test whether we already have one, so we can update it instead
	for (int loop=0; loop < m_Triggers.GetCount(); loop++)
	{
		CString csName;
		m_Triggers.GetText(loop, csName);
		int x=csName.Find(ANTIIDLETRIGGERTEXT);
		if (x > -1) {
			// Ah ha! This one is the anti-idle one
			// be nice and bring up the edit dialog for it
			m_Triggers.SetCurSel(loop);
			OnSelchangeTriggerlist();
			OnRename();
			return;
		}
	}

	Dlg.m_sPrompt="Please enter the number of minutes before triggering.";
	if (IDOK == Dlg.DoModal()) {
		int n=atoi(Dlg.m_sName);
		if (n<=0) {
			AfxMessageBox("Duration must be a positive number - add ignored.");
			return;
		}
		CString csTmp=ANTIIDLETRIGGERTEXT;
		csTmp+=Dlg.m_sName;
		csTmp+=" mins.";

		pTrigger=new CTrigger;
		pTrigger->SetName(csTmp);
		m_pTriggers->Add(pTrigger);
		DisableControls();
		m_Triggers.ResetContent();
		m_OldTrigger="";
		for(int loop=0;loop<GetNumTriggers();loop++)
		{
			m_Triggers.AddString(GetTrigger(loop)->GetName());
		}
	}
}
