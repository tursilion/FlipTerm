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
#include "utils.h"
#include "disconnect.h"

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

CMudDoc::CMudDoc()
{
	Load();
}

CMudDoc::~CMudDoc()
{
	Save();

	while (m_aWorlds.GetSize()) {			
		CWorld *pWorld = (CWorld*)m_aWorlds.GetAt(0);
		delete pWorld;				// Necessary to clean up the data
		m_aWorlds.RemoveAt(0);
	}
}

BOOL CMudDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument()) {
		return FALSE;
	}

	return TRUE;	// The load functionality will handle defaults not
}

/////////////////////////////////////////////////////////////////////////////
// CMudDoc serialization

void CMudDoc::Save()
{
	int idx, cnt;

	// We need to iterate through the objects on each list, and record them
	cnt=m_aWorlds.GetSize();
	GetApp()->WriteProfileInt("", "WorldCount", cnt);
	for (idx=0; idx<cnt; idx++) {
		CWorld *pWorld;
		pWorld=(CWorld*)m_aWorlds.GetAt(idx);
		pWorld->Save(idx);
	}

	cnt=m_aTriggers.GetSize();
	GetApp()->WriteProfileInt("", "TriggerCount", cnt);
	for (idx=0; idx<cnt; idx++) {
		CTriggerArray *pArray;
		pArray=(CTriggerArray*)m_aTriggers.Get(idx);
		pArray->Save(idx);
	}

	cnt=m_aMacros.GetSize();
	GetApp()->WriteProfileInt("", "MacroCount", cnt);
	for (idx=0; idx<cnt; idx++) {
		CMacroArray *pArray;
		pArray=(CMacroArray*)m_aMacros.Get(idx);
		pArray->Save(idx);
	}
	
	cnt=m_aAliases.GetSize();
	GetApp()->WriteProfileInt("", "AliasCount", cnt);
	for (idx=0; idx<cnt; idx++) {
		CAliasArray *pArray;
		pArray=(CAliasArray*)m_aAliases.Get(idx);
		pArray->Save(idx);
	}

	// Save the window position
	WINDOWPLACEMENT wInfo;

	GetApp()->GetMainWnd()->GetWindowPlacement(&wInfo);
	GetApp()->WriteProfileInt("Window", "Top", wInfo.rcNormalPosition.top);
	GetApp()->WriteProfileInt("Window", "Left", wInfo.rcNormalPosition.left);
	GetApp()->WriteProfileInt("Window", "Right", wInfo.rcNormalPosition.right);
	GetApp()->WriteProfileInt("Window", "Bottom", wInfo.rcNormalPosition.bottom);
	if (wInfo.showCmd == SW_SHOWMAXIMIZED) {
		GetApp()->WriteProfileInt("Window", "Maximized", 1);
	} else {
		GetApp()->WriteProfileInt("Window", "Maximized", 0);
	}
}

void CMudDoc::Load()
{
	int idx, cnt;

	cnt=GetApp()->GetProfileInt("", "WorldCount", 0);
	for (idx=0; idx<cnt; idx++) {
		CWorld *pWorld = new CWorld;
		pWorld->Load(idx);
		m_aWorlds.Add(pWorld);
	}

	cnt=GetApp()->GetProfileInt("", "TriggerCount", 0);
	if (cnt<1) {
		CTriggerArray *pArray = new CTriggerArray;
		pArray->SetName("Default");
		m_aTriggers.Add(pArray);
	} else {
		for (idx=0; idx<cnt; idx++) {
			CTriggerArray *pArray = new CTriggerArray;
			pArray->Load(idx);
			m_aTriggers.Add(pArray);
		}
	}

	cnt=GetApp()->GetProfileInt("", "MacroCount", 0);
	if (cnt<1) {
		CMacroArray *pMacroArray = new CMacroArray;
		pMacroArray->SetName("Default");
		m_aMacros.Add(pMacroArray);
		for(idx=1;idx<11;idx++)
		{
			char temps[4];
			wsprintf(temps,"F%d",idx);
			CMacro *pMac = new CMacro;
			pMac->SetName(temps);
			pMac->m_sText="";
			pMacroArray->Add(pMac);
		}
	} else {
		for (idx=0; idx<cnt; idx++) {
			CMacroArray *pArray = new CMacroArray;
			pArray->Load(idx);
			m_aMacros.Add(pArray);
		}
	}
	
	cnt=GetApp()->GetProfileInt("", "AliasCount", 0);
	if (cnt<1) {
		CAliasArray *pAliases = new CAliasArray;
		pAliases->SetName("Default");
		m_aAliases.Add(pAliases);
	} else {
		for (idx=0; idx<cnt; idx++) {
			CAliasArray *pArray = new CAliasArray;
			pArray->Load(idx);
			m_aAliases.Add(pArray);
		}
	}

	// Load the window position
	RECT myRect;
	RECT myRect2;
	int nMaximized;

	GetApp()->GetMainWnd()->GetWindowRect(&myRect2);
	myRect.top=GetApp()->GetProfileInt("Window", "Top", myRect2.top);
	myRect.left=GetApp()->GetProfileInt("Window", "Left", myRect2.left);
	myRect.right=GetApp()->GetProfileInt("Window", "Right", myRect2.right);
	myRect.bottom=GetApp()->GetProfileInt("Window", "Bottom", myRect2.bottom);
	nMaximized=GetApp()->GetProfileInt("Window", "Maximized", 0);

	if (!GetApp()->GetMainWnd()->IsZoomed()) {
		GetApp()->GetMainWnd()->MoveWindow(&myRect);	// If already maximized, leave it be
	}

	if (nMaximized) {
		if (GetApp()->m_nCmdShow == SW_SHOWNORMAL) {	// allow CmdShow to override this
			GetApp()->m_nCmdShow = SW_SHOWMAXIMIZED;	// Change it only if it was Normal
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

int CMudDoc::EditWorldList()
{
	int ret;

	CConnectDialog dlg;
	dlg.m_bShowConnect=FALSE;
	dlg.m_paWorlds = &m_aWorlds;
	dlg.m_pTriggers=&m_aTriggers;
	dlg.m_pMacros = &m_aMacros;
	dlg.m_pAliases= &m_aAliases;
	ret=dlg.DoModal();
	if (IDOK==ret) Save();
	return ret;
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
		int ret=dlg.DoModal();
		if (IDOK == ret) Save();
		return ret;
	} else {
		AfxMessageBox("To edit Trigger sets offline - Select Edit->Worlds, select any world, and click 'Change Trigger Set'. This will allow you to modify existing sets and create new ones as needed.", MB_ICONINFORMATION|MB_OK);
		return FALSE;
	}

//  Not reached
//	return FALSE;
}

int CMudDoc::AddWorld(CWorld *pWorld)
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
			AfxMessageBox("Cannot find alias list for this world, switching to default alias list.");
			pArray = (CSelArray *)m_aAliases.Get("default");
		}
		ASSERT_VALID(pArray);
		dlg.SetAliases((CAliasArray *)pArray);
		int ret=dlg.DoModal();
		if (IDOK==ret) Save();
		return ret;
	}
	return FALSE;

}

void CMudDoc::OnCloseDocument() {

	if (!g_ClosingApp) {
		// confirmation
		if (GetApp()->GetProfileInt("Helpers", "ConfirmClose", 1)) {
			CDisconnect dlg;
			
			dlg.m_Text = "Are you sure you want to close this window?";
			int ret = dlg.DoModal();
			if (dlg.m_Check) {
				GetApp()->WriteProfileInt("Helpers", "ConfirmClose", 0);
			}

			if (IDCANCEL == ret) {
				return;
			}
		}
	}

	CDocument::OnCloseDocument();
}
