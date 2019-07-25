// conbar.cpp : implementation file
//

#include "stdafx.h"
#include "gmud32.h"
#include "conbar.h"
#include "outwnd.h"
#include "gmud32vw.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CConnectedBar, CPaneDialog)
	//{{AFX_MSG_MAP(CConnectedBar)
	ON_LBN_SELCHANGE(IDC_CONNECT_LIST, OnSelchangeConnectList)
	ON_WM_SETFOCUS()
	ON_LBN_DBLCLK(IDC_CONNECT_LIST, OnDblclkConnectList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConnectedBar dialog

CConnectedBar::CConnectedBar()
{
	//{{AFX_DATA_INIT(CConnectedBar)
	m_sWorldName = _T("");
	//}}AFX_DATA_INIT
}


void CConnectedBar::DoDataExchange(CDataExchange* pDX)
{
	CPaneDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConnectedBar)
	//}}AFX_DATA_MAP
}

BOOL CConnectedBar::AddName(BOOL bUnseen,CString sTitle)
{
	CListBox *pList = (CListBox *)GetDlgItem(IDC_CONNECT_LIST);
	ASSERT_VALID(pList);

	pList->AddString(bUnseen?"*"+sTitle:" "+sTitle);
	
	return TRUE;
}

void CConnectedBar::EmptyList()
{
	CListBox *pList = (CListBox *)GetDlgItem(IDC_CONNECT_LIST);
	ASSERT_VALID(pList);

	if (pList) {
        while (pList->DeleteString(0)>0) { }
    }
}
void CConnectedBar::OnSelchangeConnectList() 
{
	CWnd *mainWnd;
	int idx;

    // get the list selection so we know which window to change to
	idx=GetCurrentSel();
    if (-1 == idx) {
        return;
    }

    // why are we redrawing the buttons here...? I think we're just sending it
    // to be updated afterwards... should this not be after the window change?
    GetParent()->SendMessage(WM_SELECT_VIEW,0,(LONG)&m_sWorldName);

    // get the main window so we can command the change
	mainWnd=GetApp()->GetMainWnd();
    if (NULL == mainWnd) {
        return;
    }

	mainWnd->PostMessage(WM_COMMAND,AFX_IDM_FIRST_MDICHILD+idx);
	mainWnd->SetFocus();
}

int CConnectedBar::GetCurrentSel()
{
	CListBox *pList = (CListBox *)GetDlgItem(IDC_CONNECT_LIST);
	ASSERT_VALID(pList);
    if (NULL == pList) {
        return -1;
    }
	return pList->GetCurSel();
}

void CConnectedBar::OnDblclkConnectList() 
{
	OnSelchangeConnectList();
}
