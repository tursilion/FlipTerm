// Mud32doc.cpp : implementation of the CMudDoc class
//

#include "stdafx.h"
#include "gmud32.h"

#include "gmud32doc.h"

#include "csmartsocket.h"
#include "connectd.h"
#include "selarray.h"
#include "triggerd.h"
#include "selarraydlg.h"
#include "world.h"
#include "triggerarray.h"
#include "macroarray.h"
#include "macro.h"
#include "AliasArray.h"
#include "AliasDialog.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMudDoc

IMPLEMENT_DYNCREATE(CMudDoc, CDocument)

BEGIN_MESSAGE_MAP(CMudDoc, CDocument)
	//{{AFX_MSG_MAP(CMudDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMudDoc construction/destruction

#define new DEBUG_NEW


CMudDoc::CMudDoc()
{
	// TODO: add one-time construction code here

}

CMudDoc::~CMudDoc()
{
	for(int loop=0;loop<m_aWorlds.GetSize();loop++)
		delete m_aWorlds[loop];
}

BOOL CMudDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	CTriggerArray *pArray = new CTriggerArray;
	pArray->SetName("Default");
	m_aTriggers.Add(pArray);

	CMacroArray *pMacroArray = new CMacroArray;
	pMacroArray->SetName("Default");
	m_aMacros.Add(pMacroArray);
	for(int mloop=1;mloop<11;mloop++)
	{
		char temps[4];
		wsprintf(temps,"F%d",mloop);
		CMacro *pMac = new CMacro;
		pMac->SetName(temps);
		pMac->m_sText="";
		pMacroArray->Add(pMac);
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMudDoc serialization

void CMudDoc::Serialize(CArchive& ar)
{
	#define DOCVER 3

	CObject::Serialize(ar);

	WORD wVer=DOCVER;
	if (ar.IsStoring())
	{
		ar << wVer;
		m_aWorlds.Serialize(ar);
		m_aTriggers.Serialize(ar);
		m_aMacros.Serialize(ar);
		m_aAliases.Serialize(ar);
	}
	else
	{
		if (ar.GetFile()->GetLength() == 0)
		{
			CTriggerArray *pArray = new CTriggerArray;
			pArray->SetName("Default");
			m_aTriggers.Add(pArray);

			CMacroArray *pMacroArray = new CMacroArray;
			pMacroArray->SetName("Default");
			m_aMacros.Add(pMacroArray);

			CAliasArray *pAliases = new CAliasArray;
			pAliases->SetName("Default");
			m_aAliases.Add(pAliases);

			for(int mloop=1;mloop<11;mloop++)
			{
				char temps[4];
				wsprintf(temps,"F%d",mloop);
				CMacro *pMac = new CMacro;
				pMac->SetName(temps);
				pMac->m_sText="";
				pMacroArray->Add(pMac);
			}
    }
    else
    {
			ar >> wVer;
			if(wVer >DOCVER)
			{
				AfxMessageBox("Error: World List file is a newer version than program, can not load, aborting load.");
				return;
			}
			m_aWorlds.Serialize(ar);
			if(wVer<2)
			{
				AfxMessageBox("Upgrading your world list to new format with multiple macro / trigger setups.");
				CTriggerArray *pArray = new CTriggerArray;
				pArray->SetName("Default");
				m_aTriggers.Add(pArray);
				CObArray ary;
				ary.Serialize(ar);
				for(int loop=0;loop<ary.GetSize();loop++)
					pArray->Add( (CSelectable *)(ary.GetAt(loop)));

				CMacroArray *pMacroArray = new CMacroArray;
				pMacroArray->SetName("Default");
				m_aMacros.Add(pMacroArray);

				for(int mloop=1;mloop<11;mloop++)
				{
					char temps[4];
					wsprintf(temps,"F%d",mloop);
					CMacro *pMac = new CMacro;
					pMac->SetName(temps);
					pMac->m_sText=AfxGetApp()->GetProfileString("MACROS",temps);
					pMacroArray->Add(pMac);
				}
			}
			else
			{
				m_aTriggers.Serialize(ar);
				m_aMacros.Serialize(ar);
			}
			if(wVer < 3)
			{
				CAliasArray *pAliases = new CAliasArray;
				pAliases->SetName("Default");
				m_aAliases.Add(pAliases);
			}
			else
			{
				m_aAliases.Serialize(ar);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMudDoc diagnostics

#ifdef _DEBUG
void CMudDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMudDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMudDoc commands

BOOL CMudDoc::CanCloseFrame(CFrameWnd* /* pFrame */) 
{
	return DoSave(m_strPathName);	
}

BOOL CMudDoc::SaveModified() 
{
	return DoSave(m_strPathName);	
}

int CMudDoc::EditWorldList()
{
	CConnectDialog dlg;
	dlg.m_bShowConnect=FALSE;
	dlg.m_paWorlds = &m_aWorlds;
	dlg.m_pTriggers=&m_aTriggers;
	dlg.m_pMacros = &m_aMacros;
	dlg.m_pAliases= &m_aAliases;
	SetModifiedFlag();
	return dlg.DoModal();
}

int CMudDoc::EditTriggerList(CWorld *pWorld)
{
	if(pWorld)
	{
		CTriggerDialog dlg;
		CSelArray *pArray = (CSelArray *)m_aTriggers.Get(pWorld->m_sTriggerList);
		if(!pArray)
		{
			AfxMessageBox("Cannot find trigger list for this world, switching to default trigger list.");
			pArray = (CSelArray *)m_aTriggers.Get("default");
		}
		ASSERT_VALID(pArray);
		dlg.SetTriggers(pArray);
		SetModifiedFlag();
		return dlg.DoModal();
	}
	return FALSE;
}

CWorld *CMudDoc::GetWorld(CString sName,WORD wPort)
{
	for(int loop=0;loop<m_aWorlds.GetSize();loop++)
	{
		CWorld *pWorld = (CWorld *)m_aWorlds[loop];
		if(wPort)
		{
			if(pWorld->m_sHostName.CompareNoCase(sName)==0 && pWorld->m_wPort == wPort)
				return pWorld;
		}
		else
		{
			if(pWorld->m_sName.CompareNoCase(sName)==0)
				return pWorld;
		}
	}
	return NULL;
}

CMudDoc::AddWorld(CWorld *pWorld)
{
	return m_aWorlds.Add(pWorld);
}

int CMudDoc::EditAliasList(CWorld * pWorld)
{
	if(pWorld)
	{
		CAliasDialog dlg;
		CSelArray *pArray = (CSelArray *)m_aAliases.Get(pWorld->m_sAliasList);
		if(!pArray)
		{
			AfxMessageBox("Cannot find alias list for this world, switching to default trigger list.");
			pArray = (CSelArray *)m_aTriggers.Get("default");
		}
		ASSERT_VALID(pArray);
		dlg.SetAliases((CAliasArray *)pArray);
		SetModifiedFlag();
		return dlg.DoModal();
	}
	return FALSE;

}
