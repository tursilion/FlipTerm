/////////////////////////////////////////////////////////////////////////////
// CSelArrayDialog dialog
#include "stdafx.h"
#include "resource.h"
#include "selarraydlg.h"
#include "selectable.h"
#include "selarray.h"
#include "triggerd.h"
#include "namedial.h"
#include "triggerarray.h"
#include "macroarray.h"
#include "buttoned.h"
#include "world.h"
#include "macro.h"
#include "AliasArray.h"

#include "AliasDialog.h"

BEGIN_MESSAGE_MAP(CSelArrayDialog, CDialog)
	//{{AFX_MSG_MAP(CSelArrayDialog)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_COPY, OnCopy)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDOK, OnOk)
	ON_LBN_DBLCLK(IDC_ARRAY_LIST, OnDblclkArrayList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CSelArrayDialog::CSelArrayDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSelArrayDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelArrayDialog)
	m_sSelected = _T("");
	//}}AFX_DATA_INIT

	m_pArray=NULL;
}


void CSelArrayDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelArrayDialog)
	DDX_LBString(pDX, IDC_ARRAY_LIST, m_sSelected);
	//}}AFX_DATA_MAP
}


BOOL CSelArrayDialog::OnInitDialog() 
{
	ASSERT(m_pArray);
	CString sTemp=m_sSelected;
	CDialog::OnInitDialog();
	PopulateList();
		
	m_sSelected=sTemp;
	UpdateData(FALSE);
	SetWindowText(m_sTitle);
	return TRUE;  // return TRUE unless you set the focus to a control
}

BOOL CSelArrayDialog::Setup(CSelArray *pArray, int iType, CObArray *pWorlds)
{
	m_pArray=pArray;
	m_iType=iType;
	m_pWorlds=pWorlds;
	return TRUE;
}

int CSelArrayDialog::PopulateList()
{
	ASSERT(m_pArray);

	CListBox *pList = (CListBox *)GetDlgItem(IDC_ARRAY_LIST);
	ASSERT_VALID(pList);

	while(pList->DeleteString(0)>0)
		;;

	for(int loop=0;loop<m_pArray->GetSize();loop++)
	{
		CSelectable *pSel=m_pArray->Get(loop);
		ASSERT_VALID(pSel);
		pList->AddString(pSel->GetName());
	}

 	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CSelArrayDialog message handlers

void CSelArrayDialog::OnEdit() 
{
	UpdateData(TRUE);
	switch(m_iType)
	{
		case TYPE_TRIGGER:
		{
			CSelArray *pSel = (CSelArray *)m_pArray->Get(m_sSelected);
			if(pSel)
			{
				CTriggerDialog dlg(this);
				dlg.SetTriggers(pSel);
				dlg.DoModal();
			}
			else
				AfxMessageBox("Select A trigger list, then select edit.");
			break;
		}
		case TYPE_MACRO:
		{
			CMacroArray *pSel = (CMacroArray *)m_pArray->Get(m_sSelected);
			if(pSel)
			{
				CButtonEdit dlg(this);
				dlg.SetList(pSel);
				dlg.DoModal();
			}
			else
				AfxMessageBox("Select A Macro list, then select edit.");
			break;
		}
		case TYPE_ALIAS:
		{
			CAliasArray *pSel = (CAliasArray *)m_pArray->Get(m_sSelected);
			if(pSel)
			{
				CAliasDialog dlg(this);
				dlg.SetAliases(pSel);
				dlg.DoModal();
			}
			else
				AfxMessageBox("Select An Alias list, then select edit.");
			break;
		}
		default:
			ASSERT(0);
	}
	
}

void CSelArrayDialog::OnCopy() 
{
	UpdateData(TRUE);
	CSelArray *pSel = (CSelArray *)m_pArray->Get(m_sSelected);
	if(!pSel)
	{
		AfxMessageBox("Please select a set to copy first.");
		return;
	}
	
	CNameDialog dlg(this);
	dlg.m_sPrompt="Enter a name for the set you are copying to.";
	if(dlg.DoModal()==IDOK)
	{
		if(m_pArray->Get(dlg.m_sName))
		{
			AfxMessageBox("A set with that name already exists, please choose another name.");
			return;
		}
		if(dlg.m_sName == "")
		{
			AfxMessageBox("You must give a name to a set. Operation aborted.");
			return;
		}
		CSelArray *pArray = (CSelArray *)(pSel->CreateCopy());
		pArray->SetName(dlg.m_sName);
		m_pArray->Add(pArray);
		PopulateList();
		m_sSelected = dlg.m_sName;

	}
	UpdateData(FALSE);
}

void CSelArrayDialog::OnNew() 
{
	CNameDialog dlg(this);
	CSelArray *pArray=NULL;
	switch(m_iType)
	{
		case TYPE_TRIGGER:
			dlg.m_sPrompt="Enter a name for the new trigger set";
			pArray= new CTriggerArray;
			break;
		case TYPE_MACRO:
		{
			dlg.m_sPrompt="Enter a name for the new Macro set";
			pArray= new CMacroArray;
			for(int mloop=1;mloop<11;mloop++)
			{
				char temps[5];
				wsprintf(temps,"F%d",mloop);
				CMacro *pMac = new CMacro;
				pMac->SetName(temps);
				pMac->m_sText="";
				pArray->Add(pMac);
			}
			break;
		}
		case TYPE_ALIAS:
			dlg.m_sPrompt="Enter a name for the new alias set";
			pArray= new CAliasArray;
			break;
		default:
			ASSERT(0);
			return;
	}
	if(dlg.DoModal()==IDOK)
	{
		if(m_pArray->Exists(dlg.m_sName))
		{
			AfxMessageBox("A set with that name already exists, please choose another name.");
			delete pArray;
			return;
		}
		if(dlg.m_sName == "")
		{
			AfxMessageBox("You must give a name to a set. Operation aborted.");
			delete pArray;
			return;
		}

		pArray->SetName(dlg.m_sName);
		m_pArray->Add(pArray);
		PopulateList();
		m_sSelected = dlg.m_sName;
		UpdateData(FALSE);
	} else {
		delete pArray;
	}
}

void CSelArrayDialog::OnDelete() 
{
	UpdateData(TRUE);
	CSelArray *pSel = (CSelArray *)m_pArray->Get(m_sSelected);
	if(!pSel)
	{
		AfxMessageBox("Please select a set to delete first.");
		return;
	}
	if(pSel->GetName()=="Default")
	{	
		AfxMessageBox("You cannot delete the default set.");
		return;
	}
	CString sOutPut;
	for(int loop=0;loop<m_pWorlds->GetSize();loop++)
	{
		CWorld *pWorld = (CWorld *)m_pWorlds->GetAt(loop);
		CString sWorldList;
		switch(m_iType)
		{
			case TYPE_TRIGGER:
				sWorldList = pWorld->m_sTriggerList;
				break;
			case TYPE_MACRO:
				sWorldList = pWorld->m_sMacroList;
				break;
			case TYPE_ALIAS:
				sWorldList = pWorld->m_sAliasList;
		}
		if(sWorldList==m_sSelected)
		{
			if(sOutPut=="")
				sOutPut="The following worlds are using this list, and will be set to \n'default' if you delete this list.\n\n";
			sOutPut+=pWorld->m_sName;
			sOutPut+="\n";
		}
	}
	if(sOutPut!="")
		sOutPut+="\n";
	if(AfxMessageBox(sOutPut+"Delete set "+m_sSelected+" ?",MB_OKCANCEL)==IDOK)
	{
		for(int loop=0;loop<m_pWorlds->GetSize();loop++)
		{
			CWorld *pWorld = (CWorld *)m_pWorlds->GetAt(loop);
			CString *psWorldList=NULL;

			switch(m_iType)
			{
				case TYPE_TRIGGER:
					psWorldList = &pWorld->m_sTriggerList;
					break;
				case TYPE_MACRO:
					psWorldList = &pWorld->m_sMacroList;
					break;
				case TYPE_ALIAS:
					psWorldList = &pWorld->m_sAliasList;
			}
			if(*psWorldList==m_sSelected) {
				*psWorldList="Default";
			}
		}
		m_pArray->Delete(m_sSelected);
		m_sSelected="Default";			// duplicated incase of background processing interruption
		PopulateList();					// duplicated incase of background processing interruption
		m_sSelected="Default";	
		UpdateData(FALSE);
	}
}

void CSelArrayDialog::OnOk() 
{
	UpdateData(TRUE);
	if(m_sSelected == "")
	{
		AfxMessageBox("You must select a set before exiting this dialog.");
		return;
	}
	CDialog::OnOK();	
}


void CSelArrayDialog::OnDblclkArrayList() 
{
	OnOK();
}
