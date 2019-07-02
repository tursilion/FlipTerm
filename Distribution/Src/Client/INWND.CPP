// inwnd.cpp : implementation file
//

#include "stdafx.h"
#include "gmud32.h"
#include "outwnd.h"
#include "gmud32vw.h"
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
	bControlDown=false;
	bSentMapkey=false;
	m_lastline=-1;
	myFont=NULL;
}

CInWnd::~CInWnd()
{
	if (NULL != myFont) {
		delete myFont;
	}
}

BEGIN_MESSAGE_MAP(CInWnd, CEdit)
	//{{AFX_MSG_MAP(CInWnd)
	ON_WM_CREATE()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_KEYUP()
	ON_WM_SYSCHAR()
	ON_WM_MOUSEWHEEL()
	ON_WM_KILLFOCUS()
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

int CInWnd::NewFont(LPLOGFONT lf)
{
	memcpy(&m_LogFont,lf,sizeof(LOGFONT));
	if (NULL != myFont) {
		delete myFont;
	}
	myFont=new CFont;
	myFont->CreateFontIndirect(&m_LogFont);
	SetFont(myFont,TRUE);
	Invalidate();
	return TRUE;
}

void CInWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CMudApp *pApp=GetApp();

	// Handle 'map keys' if enabled
	if (pApp->m_bMapKeys) {
		bSentMapkey=true;	// set to false in the default case
		
		// these guys directly send their commands and do not affect
		// the input buffer!
		switch (nChar) {
			case VK_NUMPAD0:
				if (GetKeyState(VK_CAPITAL)&0x01) {
					GetParent()->SendMessage(WM_ENTER_PRESSED,0,(LONG)(LPCSTR)"UP");
				} else {
					GetParent()->SendMessage(WM_ENTER_PRESSED,0,(LONG)(LPCSTR)"up");
				}
				break;

			case VK_NUMPAD1:
				if (GetKeyState(VK_CAPITAL)&0x01) {
					GetParent()->SendMessage(WM_ENTER_PRESSED,0,(LONG)(LPCSTR)"SOUTHWEST");
				} else {
					GetParent()->SendMessage(WM_ENTER_PRESSED,0,(LONG)(LPCSTR)"southwest");
				}
				break;

			case VK_NUMPAD2:
				if (GetKeyState(VK_CAPITAL)&0x01) {
					GetParent()->SendMessage(WM_ENTER_PRESSED,0,(LONG)(LPCSTR)"SOUTH");
				} else {
					GetParent()->SendMessage(WM_ENTER_PRESSED,0,(LONG)(LPCSTR)"south");
				}
				break;

			case VK_NUMPAD3:
				if (GetKeyState(VK_CAPITAL)&0x01) {
					GetParent()->SendMessage(WM_ENTER_PRESSED,0,(LONG)(LPCSTR)"SOUTHEAST");
				} else {
					GetParent()->SendMessage(WM_ENTER_PRESSED,0,(LONG)(LPCSTR)"southeast");
				}
				break;

			case VK_NUMPAD4:
				if (GetKeyState(VK_CAPITAL)&0x01) {
					GetParent()->SendMessage(WM_ENTER_PRESSED,0,(LONG)(LPCSTR)"WEST");
				} else {
					GetParent()->SendMessage(WM_ENTER_PRESSED,0,(LONG)(LPCSTR)"west");
				}
				break;

			case VK_NUMPAD5:
				if (GetKeyState(VK_CAPITAL)&0x01) {
					GetParent()->SendMessage(WM_ENTER_PRESSED,0,(LONG)(LPCSTR)"DOWN");
				} else {
					GetParent()->SendMessage(WM_ENTER_PRESSED,0,(LONG)(LPCSTR)"down");
				}
				break;

			case VK_NUMPAD6:
				if (GetKeyState(VK_CAPITAL)&0x01) {
					GetParent()->SendMessage(WM_ENTER_PRESSED,0,(LONG)(LPCSTR)"EAST");
				} else {
					GetParent()->SendMessage(WM_ENTER_PRESSED,0,(LONG)(LPCSTR)"east");
				}
				break;

			case VK_NUMPAD7:
				if (GetKeyState(VK_CAPITAL)&0x01) {
					GetParent()->SendMessage(WM_ENTER_PRESSED,0,(LONG)(LPCSTR)"NORTHWEST");
				} else {
					GetParent()->SendMessage(WM_ENTER_PRESSED,0,(LONG)(LPCSTR)"northwest");
				}
				break;

			case VK_NUMPAD8:
				if (GetKeyState(VK_CAPITAL)&0x01) {
					GetParent()->SendMessage(WM_ENTER_PRESSED,0,(LONG)(LPCSTR)"NORTH");
				} else {
					GetParent()->SendMessage(WM_ENTER_PRESSED,0,(LONG)(LPCSTR)"north");
				}
				break;

			case VK_NUMPAD9:
				if (GetKeyState(VK_CAPITAL)&0x01) {
					GetParent()->SendMessage(WM_ENTER_PRESSED,0,(LONG)(LPCSTR)"NORTHEAST");
				} else {
					GetParent()->SendMessage(WM_ENTER_PRESSED,0,(LONG)(LPCSTR)"northeast");
				}
				break;

			default:
				bSentMapkey=false;
				break;
		}
		
		if (bSentMapkey) return;
		// otherwise, fall through and handle normally 
	}

	switch(nChar)
	{
		case VK_UP:
			if ((pApp->m_bMultiline)&&(false == bControlDown)) {
				CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
				break;
			}
			// else fall through to UP
		case 'P':
			if ((false == bControlDown)&&(nChar!=VK_UP)) {
				break;
			}
			HistoryPrev();
			break;

		case VK_DOWN:
			if ((pApp->m_bMultiline)&&(false == bControlDown)) {
				CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
				break;
			}
			// else fall through to DOWN
		case 'N':
			if ((false == bControlDown)&&(nChar!=VK_DOWN)) {
				break;
			}
			HistoryNext();
			break;
		case VK_PRIOR:
			GetParent()->PostMessage(WM_VSCROLL,SB_PAGEUP ,SB_VERT);
			break;
		case VK_NEXT:
			GetParent()->PostMessage(WM_VSCROLL,SB_PAGEDOWN,SB_VERT);
			break;
		case VK_CONTROL:
			bControlDown=true;
			break;
		case 'U':
			if (bControlDown) {
				// clear the input window
				SetWindowText("");
			}
			break;

		default:
			m_lastline=-1;
			bControlDown=false;
			CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
			break;
	}
}

void CInWnd::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch (nChar) {
	case 'P':
	case 'N':
	case 'U':
	case VK_UP:
	case VK_DOWN:
		if (bControlDown) {
			break;
		}
		// else fall through
	default:
		bControlDown=false;
		CEdit::OnKeyUp(nChar, nRepCnt, nFlags);
	}
}

void CInWnd::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (GetApp()->m_bMapKeys) {
		if ((nChar>='0')&&(nChar<='9')&&(bSentMapkey)) {
			bSentMapkey=false;	// mind you, it's already set each time, but just to be safe
			return;
		}
	}

	switch(toupper(nChar))
	{
		case VK_ESCAPE:
			m_lastline=-1;
			SetWindowText("");
			bControlDown=false;
			break;
		case VK_RETURN:
			{
				while (m_aLines.GetSize()>=NUMLINES) {
					m_aLines.RemoveAt(m_aLines.GetSize()-1);
				}
				CString wintext;
				CString buffer;
				int idx, cnt;
				GetWindowText(buffer);
				wintext=buffer.Left(BUFFSIZE/3);
				buffer="";
				m_aLines.InsertAt(0,wintext);
				// Check for Command Stacking and Speed Walking >sigh< ;)
				// Speed walk first:
				if (GetApp()->m_bUseCLI) {
					if ((wintext.Left(2).CompareNoCase("/w") == 0) && (wintext.Left(7).CompareNoCase("/window"))) {
						// Gonna send a lot of messages now - this SHOULD work, if there are no tie-ups ;)
						for (idx=2; idx<wintext.GetLength(); idx++) {
							cnt=1;
							if ((wintext[idx]>'0')&&(wintext[idx]<='9')) {
								cnt=wintext[idx]-'0';
								idx++;
							}
							switch (toupper(wintext[idx])) {
							case 'N':
								buffer="north"; break;
							case 'S':
								buffer="south"; break;
							case 'E':
								buffer="east"; break;
							case 'W':
								buffer="west"; break;
							case 'U':
								buffer="up"; break;
							case 'D':
								buffer="down"; break;
							default:
								CMDIChildWnd * pChild =	((CMDIFrameWnd*)(GetApp()->m_pMainWnd))->MDIGetActive();
								CMudView *pView=(CMudView*)pChild->GetActiveView();
								pView->Printf("\n");
								pView->Printf("%%%%%% Bad speedwalking character. See /help.\n\n");
								goto cleanup;
							}
							if (wintext.Left(2).Compare("/W") == 0) {
								buffer.MakeUpper();
							}
							while (cnt > 0) {
								GetParent()->SendMessage(WM_ENTER_PRESSED, 0, (LONG)(LPCSTR)buffer);
								cnt--;
							}
						}
						goto cleanup;
					}
				}
				GetParent()->SendMessage(WM_ENTER_PRESSED,0,(LONG)(LPCSTR)wintext);
cleanup:
				SetWindowText("");
				m_lastline=-1;
			}
			bControlDown=false;
			break;

		case 14:		// Ctrl+N
		case 16:		// Ctrl+P
		case 21:		// Ctrl+U
			break;

		default:
			bControlDown=false;
			if ((nChar>=' ')&&(nChar<=126)) {
				m_lastline=-1;
			}
			CEdit::OnChar(nChar, nRepCnt, nFlags);
			break;
	}
}

void CInWnd::OnSysChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch (toupper(nChar)) {
	case 'S':
		OnChar(VK_RETURN, 0, 0);
		return;
	}

	CEdit::OnSysChar(nChar, nRepCnt, nFlags);
}

#define STEP 20
BOOL CInWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	int tPos;
	CMudView *pView;

	pView=(CMudView*)(GetParent());
	if (pView) {
		tPos=pView->m_pOutWnd->GetScrollPos(SB_VERT);
		tPos-=(zDelta/STEP);
		pView->m_pOutWnd->ScrollTo(tPos);
	}

	return CEdit::OnMouseWheel(nFlags, zDelta, pt);
}

void CInWnd::HistoryPrev() {
	m_lastline++;
	m_lastline=min(m_lastline,m_aLines.GetSize()-1);
	if(m_lastline>-1) {
		if (GetApp()->m_bReviewEnd) {
			SetWindowText("");
			ReplaceSel(m_aLines[m_lastline], false);
		} else {
			SetWindowText(m_aLines[m_lastline]);
		}
	}
	return;
}

void CInWnd::HistoryNext() {
	m_lastline--;
	if(m_lastline<0)
	{
		m_lastline=-1;
		SetWindowText("");
	} else {
		if (GetApp()->m_bReviewEnd) {
			SetWindowText("");
			ReplaceSel(m_aLines[m_lastline], false);
		} else {
			SetWindowText(m_aLines[m_lastline]);
		}
	}
	return;
}

void CInWnd::OnKillFocus(CWnd* pNewWnd) 
{
	bControlDown=false;
	
	CEdit::OnKillFocus(pNewWnd);
}

