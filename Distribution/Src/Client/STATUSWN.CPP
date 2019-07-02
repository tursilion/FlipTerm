// statuswn.cpp : implementation file
//
// This whole file is kind of academic - the status window is just a divider nowadays
// But, we'll use it for faking the sizing for now.

#include "stdafx.h"
#include "gmud32.h"
#include "statuswn.h"
#include "outwnd.h"
#include "gmud32vw.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStatusWnd

CStatusWnd::CStatusWnd()
{
}

CStatusWnd::~CStatusWnd()
{
}

BEGIN_MESSAGE_MAP(CStatusWnd, CWnd)
	//{{AFX_MSG_MAP(CStatusWnd)
	ON_WM_NCLBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_NCMOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStatusWnd message handlers

// We use NC, as there is no visible client area. After the capture, though,
// all messages are considered client messages
void CStatusWnd::OnNcLButtonDown(UINT nHitTest, CPoint point) 
{
	if (!GetApp()->m_bMultiline) return;

	SetCapture();

	CWnd::OnNcLButtonDown(nHitTest, point);
}

void CStatusWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	POINT myPoint;
	int y;

	if (!GetApp()->m_bMultiline) return;

	SetCursor(LoadCursor(NULL, IDC_SIZENS));

	GetCursorPos(&myPoint);
	GetParent()->ScreenToClient(&myPoint);
	y=myPoint.y;
	
	((CMudView*)GetParent())->MoveDivider(y);
	
	CWnd::OnMouseMove(nFlags, point);
}

void CStatusWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	POINT myPoint;

	if (!GetApp()->m_bMultiline) return;

	GetCursorPos(&myPoint);
	ReleaseCapture();
	
	GetParent()->ScreenToClient(&myPoint);
	((CMudView*)GetParent())->ResizeInWnd(myPoint.y);

	CWnd::OnLButtonUp(nFlags, point);
}

void CStatusWnd::OnNcMouseMove(UINT nHitTest, CPoint point) 
{
	if (!GetApp()->m_bMultiline) return;

	// set the pointer
	SetCursor(LoadCursor(NULL, IDC_SIZENS));
	CWnd::OnNcMouseMove(nHitTest, point);
}
