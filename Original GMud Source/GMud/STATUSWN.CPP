// statuswn.cpp : implementation file
//

#include "stdafx.h"
#include "statuswn.h"
#include "gmud32.h"

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
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStatusWnd message handlers

CStatusWnd::SetName(LPCSTR newname)
{
	m_sName=newname;
	Invalidate();
	return TRUE;
}

CStatusWnd::SetChannel(LPCSTR newchannel)
{
	m_sChannel=newchannel;
	Invalidate();
	return TRUE;
}

CStatusWnd::SetChanMode(LPCSTR newmode)
{
	m_sMode=newmode;
	Invalidate();
	return TRUE;
}

CStatusWnd::SetOpped(BOOL bOpped)
{
	m_bOpped = bOpped;
	Invalidate();
	return TRUE;
}

void CStatusWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
/*

	
	dc.SetBkColor(RGB(0,0,0));
	char temps[1024];   
	CFont f;
	f.CreateFontIndirect(&m_LogFont);
	CFont *oldfont = dc.SelectObject(&f);

	dc.SetTextColor(RGB(255,255,255));
	sprintf(temps,"%c%s %s (+%s)",(m_bOpped)?'@':' ',(LPCSTR)m_sName,(LPCSTR)m_sChannel,(LPCSTR)m_sMode);
	dc.TextOut(0,0,temps,lstrlen(temps));
	
	dc.SelectObject(oldfont);
*/
}

BOOL CStatusWnd::OnEraseBkgnd(CDC* pDC)
{
	CRect r;
	pDC->GetClipBox(&r);
	CMudApp *pApp = (CMudApp *)AfxGetApp();
	CBrush br(pApp->m_colorForeGround);

	pDC->FillRect(&r,&br);
	return TRUE;
}

CStatusWnd::NewFont(LPLOGFONT lf)
{
	memcpy(&m_LogFont,lf,sizeof(LOGFONT));
	Invalidate();
	return TRUE;
}
