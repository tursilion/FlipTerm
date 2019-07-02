// triggerd.cpp : implementation file
//

#include "stdafx.h"
#include "gmud32.h"
#include "triggerd.h"
#include "trigger.h"
#include "mmsystem.h"
#include "selarray.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CTriggerDialog, CDialog)
	//{{AFX_MSG_MAP(CTriggerDialog)
	ON_BN_CLICKED(IDC_BROWSE_FILE, OnBrowseFile)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDADD, OnAdd)
	ON_CBN_SELCHANGE(IDC_TRIGGER_LIST, OnSelchangeTriggerList)
	ON_BN_CLICKED(IDC_TEST_SOUND, OnTestSound)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define new DEBUG_NEW

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
}

void CTriggerDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTriggerDialog)
	DDX_Text(pDX, IDC_FILENAME, m_sFileName);
	DDX_Text(pDX, IDC_MUDTEXT, m_sMudText);
	DDX_CBString(pDX, IDC_TRIGGER_LIST, m_sTrigger);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CTriggerDialog message handlers

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
					"Wave Files (*.wav)|*.wav|AVI Files (*.avi)|*.avi|All Files (*.*) |*.*||",
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
	CTrigger *pTrigger = GetTrigger(m_sTrigger);
	if(m_sTrigger=="")
	{
		AfxMessageBox("Select a trigger, then hit delete");
	}
	else
	{
		m_pTriggers->Delete(m_sTrigger);
	}

	CComboBox *pCb= (CComboBox *)GetDlgItem(IDC_TRIGGER_LIST);
	ASSERT_VALID(pCb);
	pCb->ResetContent();
	for(int loop=0;loop<GetNumTriggers();loop++)
	{
		pCb->AddString(GetTrigger(loop)->GetName());
	}

}

void CTriggerDialog::OnOK() 
{
	UpdateData(TRUE);
	CTrigger *pTrigger = GetTrigger(m_sTrigger);
	if(!pTrigger)
	{
		TRACE("OnSelChangeTriggerList():Couldn't save trigger\n");
	}
	else
	{
		pTrigger->SetName(m_sTrigger);
		pTrigger->m_sFileName  = m_sFileName;
		pTrigger->m_sMudOutput = m_sMudText;
	}
	CDialog::OnOK();
}

BOOL CTriggerDialog::OnInitDialog() 
{
	ASSERT_VALID(m_pTriggers);
	CDialog::OnInitDialog();
	
	CComboBox *pCb= (CComboBox *)GetDlgItem(IDC_TRIGGER_LIST);
	ASSERT_VALID(pCb);

	for(int loop=0;loop<GetNumTriggers();loop++)
	{
		pCb->AddString(GetTrigger(loop)->GetName());
	}
	CenterWindow();	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTriggerDialog::OnAdd() 
{
	UpdateData(TRUE);
	if(m_sTrigger=="")
	{
		AfxMessageBox("Enter the trigger text in the combo box, then select ADD");
		return;
	}
	CTrigger *pTrigger = GetTrigger(m_sTrigger);
	if(!pTrigger)
	{
		pTrigger = new CTrigger;
		m_pTriggers->Add(pTrigger);
	}
	pTrigger->SetName(m_sTrigger);
	CComboBox *pCb= (CComboBox *)GetDlgItem(IDC_TRIGGER_LIST);
	ASSERT_VALID(pCb);
	pCb->ResetContent();
	for(int loop=0;loop<GetNumTriggers();loop++)
	{
		pCb->AddString(GetTrigger(loop)->GetName());
	}
	UpdateData(FALSE);
}

void CTriggerDialog::OnSelchangeTriggerList() 
{
	UpdateData(TRUE);
	CTrigger *pTrigger = GetTrigger(m_sTrigger);
	if(!pTrigger)
	{
		TRACE("OnSelChangeTriggerList():Couldn't save trigger\n");
	}
	else
	{
		pTrigger->SetName(m_sTrigger);
		pTrigger->m_sFileName  = m_sFileName;
		pTrigger->m_sMudOutput = m_sMudText;
	}

	CComboBox *pCombo = (CComboBox *)GetDlgItem(IDC_TRIGGER_LIST);
	ASSERT_VALID(pCombo);
	pCombo->GetLBText(pCombo->GetCurSel(),m_sTrigger);
	pTrigger=GetTrigger(m_sTrigger);
	if(!pTrigger)
	{
		TRACE("OnSelChangeTriggerList():Couldn't Load trigger\n");
	}
	else
	{
		m_sTrigger=pTrigger->GetName() ;
		m_sFileName=pTrigger->m_sFileName  ;
		m_sMudText=pTrigger->m_sMudOutput ;
	}
	UpdateData(FALSE);
}

void CTriggerDialog::OnTestSound() 
{
	UpdateData(TRUE);
	sndPlaySound(m_sFileName,SND_ASYNC);
}
