// inwnd.cpp : implementation file
//

#include "stdafx.h"
#include "gmud32.h"
#include "inwnd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define NUMLINES 100

/////////////////////////////////////////////////////////////////////////////
// CInWnd

CInWnd::CInWnd()
{
	m_lastline=-1;
}

CInWnd::~CInWnd()
{
}

BEGIN_MESSAGE_MAP(CInWnd, CEdit)
	//{{AFX_MSG_MAP(CInWnd)
	ON_WM_CREATE()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInWnd message handlers

int CInWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;
	m_lastline=0;
	return 0;
}

CInWnd::NewFont(LPLOGFONT lf)
{
	memcpy(&m_LogFont,lf,sizeof(LOGFONT));
	CFont f;
	f.CreateFontIndirect(&m_LogFont);
	SetFont(&f,TRUE);
	Invalidate();
	return TRUE;
}

void CInWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch(nChar)
	{
		case VK_UP:
			m_lastline++;
			m_lastline=min(m_lastline,m_aLines.GetSize()-1);
			if(m_lastline>-1)
				SetWindowText(m_aLines[m_lastline]);
			break;
		case VK_DOWN:
			m_lastline--;
			if(m_lastline<0)
			{
				m_lastline=-1;
				SetWindowText("");
			}
			else
				SetWindowText(m_aLines[m_lastline]);
			break;
		case VK_PRIOR:
			GetParent()->PostMessage(WM_VSCROLL,SB_PAGEUP ,SB_VERT);
			break;
		case VK_NEXT:
			GetParent()->PostMessage(WM_VSCROLL,SB_PAGEDOWN,SB_VERT);
			break;

		default:
			m_lastline=-1;
			CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
			break;
	}
}


void CInWnd::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch(nChar)
	{
	
		case VK_ESCAPE:
			m_lastline=-1;
			SetWindowText("");
			break;
		case VK_RETURN:
			{
				if(m_aLines.GetSize()>=NUMLINES)
					m_aLines.RemoveAt(m_aLines.GetSize()-1);
				CString wintext;
				GetWindowText(wintext);
				m_aLines.InsertAt(0,wintext);
				GetParent()->SendMessage(WM_ENTER_PRESSED,0,(LONG)(LPCSTR)wintext);
				SetWindowText("");
				m_lastline=-1;
			}
			break;
		default:
			m_lastline=-1;
			CEdit::OnChar(nChar, nRepCnt, nFlags);
			break;
	}
}

