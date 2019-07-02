// conbar.cpp : implementation file
//

#include "stdafx.h"
#include "gmud32.h"
#include "conbar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


BEGIN_MESSAGE_MAP(CConnectedBar, CDialogBar)
	//{{AFX_MSG_MAP(CConnectedBar)
	ON_LBN_SELCHANGE(IDC_CONNECT_LIST, OnSelchangeConnectList)
	ON_LBN_SETFOCUS(IDC_CONNECT_LIST, OnSetfocusConnectList)
	ON_LBN_DBLCLK(IDC_CONNECT_LIST, OnDblclkConnectList)
	ON_WM_SETFOCUS()
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
	CDialogBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConnectedBar)
//	DDX_LBString(pDX, IDC_CONNECT_LIST, m_sWorldName);
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

	if(pList) while(pList->DeleteString(0)>0)
					;;
}
void CConnectedBar::OnSelchangeConnectList() 
{
	GetParent()->SendMessage(WM_SELECT_VIEW,0,(LONG)&m_sWorldName);
	TRACE("\nConnect list got clicked.");
}

void CConnectedBar::OnSetfocusConnectList() 
{
	// TODO: Add your control notification handler code here
	TRACE("\nConnect list got focus.");
}

void CConnectedBar::OnDblclkConnectList() 
{
	OnSelchangeConnectList() ;
	TRACE("\nConnect list got double clicked.");
}

BOOL CConnectedBar::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialogBar::OnChildNotify(message, wParam, lParam, pLResult);
}

void CConnectedBar::OnSetFocus(CWnd* pOldWnd) 
{
	CDialogBar::OnSetFocus(pOldWnd);
	
	// TODO: Add your message handler code here
	
}

int CConnectedBar::GetCurSel()
{
	CListBox *pList = (CListBox *)GetDlgItem(IDC_CONNECT_LIST);
	ASSERT_VALID(pList);
	return pList->GetCurSel();
}
