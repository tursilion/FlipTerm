// buttonba.cpp : implementation file
//

#include "stdafx.h"
#include "gmud32.h"
#include "buttonba.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CButtonBar

CButtonBar::CButtonBar()
{
}

CButtonBar::~CButtonBar()
{
}


BEGIN_MESSAGE_MAP(CButtonBar, CPaneDialog)
	//{{AFX_MSG_MAP(CButtonBar)
	ON_WM_SETFOCUS()
	ON_COMMAND(IDC_BUTTON1, OnButton1)
	ON_WM_ACTIVATE()
	ON_WM_CHILDACTIVATE()
	ON_WM_MENUCHAR()
	ON_WM_PARENTNOTIFY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CButtonBar message handlers


void CButtonBar::OnSetFocus(CWnd* pOldWnd)
{
//	CPaneDialog::OnSetFocus(pOldWnd);	
	pOldWnd->SetFocus();
}

BOOL CButtonBar::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult) 
{
	return CPaneDialog::OnChildNotify(message, wParam, lParam, pLResult);
}

void CButtonBar::OnButton1() 
{
	GetParent()->SetFocus();	
}

void CButtonBar::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CPaneDialog::OnActivate(nState, pWndOther, bMinimized);
}

void CButtonBar::OnChildActivate() 
{
	CPaneDialog::OnChildActivate();
}

LRESULT CButtonBar::OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu) 
{
	return CPaneDialog::OnMenuChar(nChar, nFlags, pMenu);
}

void CButtonBar::OnParentNotify(UINT message, LPARAM lParam) 
{
	CPaneDialog::OnParentNotify(message, lParam);
}
