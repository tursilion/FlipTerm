// AliasDialog.cpp : implementation file
//

#include "stdafx.h"
#include "gmud32.h"
#include "AliasDialog.h"
#include "AliasArray.h"
#include "Alias.h"
#include "AliasEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAliasDialog dialog


CAliasDialog::CAliasDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CAliasDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAliasDialog)
	m_sAliasText = _T("");
	m_sAliasSelected = _T("");
	//}}AFX_DATA_INIT

	m_pAliasArray=0;
}


void CAliasDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAliasDialog)
	DDX_Control(pDX, IDC_ALIAS_LIST, m_ctlAliasList);
	DDX_Text(pDX, IDC_ALIAS_OUTPUT, m_sAliasText);
	DDX_LBString(pDX, IDC_ALIAS_LIST, m_sAliasSelected);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAliasDialog, CDialog)
	//{{AFX_MSG_MAP(CAliasDialog)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_INSERT, OnInsert)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_LBN_SELCHANGE(IDC_ALIAS_LIST, OnSelchangeAliasList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAliasDialog message handlers

void CAliasDialog::OnDelete() 
{
	UpdateData(TRUE);
	CAlias *pAlias = GetAlias(m_sAliasSelected);
	if(!pAlias)
		return;
	m_pAliasArray->Delete(m_sAliasSelected);
	RefillListBox();
}

void CAliasDialog::SetAliases(CAliasArray * pSel)
{
	m_pAliasArray = pSel;
}

CAlias * CAliasDialog::GetAlias(CString sName)
{
	for(int loop=0;loop<m_pAliasArray->GetSize();loop++)
	{
		if(m_pAliasArray->Get(loop)->GetName()==sName)
			return (CAlias *)m_pAliasArray->Get(loop);
	}
	return NULL;
}

void CAliasDialog::RefillListBox()
{
	m_ctlAliasList.ResetContent();

	for(int loop=0;loop<m_pAliasArray->GetSize();loop++)
	{
		m_ctlAliasList.AddString(m_pAliasArray->Get(loop)->GetName());
	}
  UpdateData(FALSE);
}

BOOL CAliasDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	RefillListBox();	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAliasDialog::OnInsert() 
{
	UpdateData(TRUE);
	CAliasEdit dlg(this);
	if(dlg.DoModal()==IDOK)
	{
    if(GetAlias(dlg.m_sAlias))
    {
      AfxMessageBox("That alias already exists, please choose a different one!");
      return;
    }
  	CAlias *pAlias = new CAlias;
    pAlias->SetName(dlg.m_sAlias);
    pAlias->m_sAliasOutput=dlg. m_sOutput;
    m_pAliasArray->Add(pAlias);
  }
	RefillListBox();
}

void CAliasDialog::OnEdit() 
{
	UpdateData(TRUE);
	CAlias *pAlias = GetAlias(m_sAliasSelected);
	if(!pAlias)
		return;

	CAliasEdit dlg(this);
  dlg.m_sAlias=pAlias->GetName();
  dlg.m_sOutput=pAlias->m_sAliasOutput;
	if(dlg.DoModal()==IDOK)
	{
    pAlias->SetName(dlg.m_sAlias);
    pAlias->m_sAliasOutput=dlg. m_sOutput;
  }
	RefillListBox();
	
}

void CAliasDialog::OnSelchangeAliasList() 
{
	UpdateData(TRUE);

  m_ctlAliasList.GetText(m_ctlAliasList.GetCurSel(),m_sAliasSelected);
	CAlias *pAlias=GetAlias(m_sAliasSelected);
	if(!pAlias)
    m_sAliasText="";
  else
    m_sAliasText=pAlias->m_sAliasOutput;

  UpdateData(FALSE);
	
}
