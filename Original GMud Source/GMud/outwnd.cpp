/////////////////////////////////////////////////////////////////////////////
// COutWnd
// outwnd.cpp : implementation file
//

#include "stdafx.h"
#include "outwnd.h"
#include "textfrmt.h"
#include <stdarg.h>
#include <stdio.h>
#include "gmud32.h"
#include "gmud32vw.h"


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define NUMLINES 500
#define NUMSCROLL 10

BEGIN_MESSAGE_MAP(COutWnd, CWnd)
	//{{AFX_MSG_MAP(COutWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define DIM_RGB_VALUE 0x80
#define MED_RGB_VALUE 0xC0

LONG colormap[16]=
{
	RGB(0,0,0),												// black
	RGB(DIM_RGB_VALUE,0,0),									// red
	RGB(0,DIM_RGB_VALUE,0),									// green
	RGB(DIM_RGB_VALUE,DIM_RGB_VALUE,0),						// yellow
	RGB(0,0,DIM_RGB_VALUE),									// blue
	RGB(DIM_RGB_VALUE,0,DIM_RGB_VALUE),						// magenta
	RGB(0,DIM_RGB_VALUE,DIM_RGB_VALUE), 					// cyan
	RGB(MED_RGB_VALUE,MED_RGB_VALUE,MED_RGB_VALUE),			// dim white
															// bright colors
	RGB(DIM_RGB_VALUE,DIM_RGB_VALUE,DIM_RGB_VALUE),			// bright black
	RGB(255,0,0),											// bright red
	RGB(0,255,0),											// bright green
	RGB(255,255,0),											// bright yellow
	RGB(0,0,255),											// bright blue
	RGB(255,0,255),											// bright magenta
	RGB(0,255,255), 										// bright cyan
	RGB(255,255,255)										// bright white
};

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// COutWnd message handlers

COutWnd::COutWnd()
{
	m_iOffset=0;
	m_iFontHeight=0;
    m_bLastcr=TRUE;
	AnsiReset();
	m_nRow=0;
	m_nCol=0;
	m_nSavedCol=0;
	m_nSavedRow=0;
	m_bNextNew=TRUE;
	m_ptUp=m_ptDown=CPoint(0,0);
	m_bUnPauseWhenSelectionDone=FALSE;
  m_iMaxLines=10000;
}

COutWnd::~COutWnd()
{															   
}

int COutWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	SetScrollRange(SB_VERT,0,NUMLINES);
	SetScrollPos(SB_VERT,NUMLINES-m_iOffset,TRUE);

	return 0;
}


int COutWnd::GetLineHeight()
{
	ASSERT(m_iFontHeight);
	return m_iFontHeight;
}

int COutWnd::CalcFontHeight()
{
	CClientDC dc(this);
	CFont f;
	f.CreateFontIndirect(&m_LogFont);
	CFont *oldfont = dc.SelectObject(&f);
    CSize size=dc.GetTextExtent("ABCDEFGHIJKLMNOPQRSTUVWXyZ",26);
	int height=size.cy;
	m_iCharWidth=size.cx/26;
    dc.SelectObject(oldfont);
	return height;
}

COutWnd::ScrollUp(int numlines /* = 1 */)
{   
	CRect r;
	GetClientRect(&r);
	ScrollWindow(0,0-GetLineHeight()*numlines,&r,&r);
	return TRUE;
}

COutWnd::NewFont(LPLOGFONT lf)
{
	ASSERT_VALID(this);
	memcpy(&m_LogFont,lf,sizeof(LOGFONT));
	m_iFontHeight=CalcFontHeight();
	Invalidate();
	return TRUE;
}

void COutWnd::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	ASSERT_VALID(this);
	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
	
	CRect clientrect;
	GetClientRect(&clientrect);
	
	int numvislines=clientrect.Height()/GetLineHeight();
	int numlines=m_Lines.GetCount();
	int numnonvislines=numlines-numvislines;
	
	if(numnonvislines<1)
		return;

	int pos = 	(int)((int)NUMLINES-(int)nPos);
	int newoffset=m_iOffset;
	switch(nSBCode)
	{
		case SB_BOTTOM:
			newoffset=numnonvislines;
			break;
		case SB_ENDSCROLL:
			break;
		case SB_LINEDOWN:
			newoffset=m_iOffset-1;
			break;
		case SB_LINEUP:
			newoffset=m_iOffset+1;
			break;
		case SB_PAGEDOWN:
			newoffset=m_iOffset-numvislines;
			break;
		case SB_PAGEUP:
			newoffset=m_iOffset+numvislines;
			break;
		case SB_THUMBPOSITION:
			newoffset=pos;
			break;
		case SB_THUMBTRACK:
			newoffset=pos;
			break;
		case SB_TOP:
			newoffset=0;
			break;
		default:
			ASSERT(0);
	}
	if(newoffset<0)
		newoffset=0;
	if(newoffset>(int)numnonvislines)
		newoffset=numnonvislines;
		
	int numlinesmoved=m_iOffset-newoffset;
	
	if(abs(numlinesmoved)<numvislines)
		ScrollUp(numlinesmoved);
	else
		Invalidate(TRUE);
		
	m_iOffset=newoffset;
	SetScrollPos(SB_VERT,NUMLINES-m_iOffset,TRUE);
}

void COutWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	
	SetScrollRange(SB_VERT,0,NUMLINES,FALSE);
	SetScrollPos(SB_VERT,NUMLINES-m_iOffset,TRUE);
	CalcFontHeight();
	m_nCols = cx/m_iCharWidth;
}

BOOL COutWnd::OnEraseBkgnd(CDC* pDC) 
{
	CMudApp *pApp = (CMudApp *)AfxGetApp();
	CBrush wbr(pApp->m_colorBackGround);
	CRect r;
	pDC->GetClipBox(&r);
	pDC->FillRect(&r,&wbr);
	return TRUE;
}

void COutWnd::AnsiReset()
{
	m_iFGIndex=0;
	m_bBold=FALSE;
	m_AnsiColorForeGround = RGB(MED_RGB_VALUE,MED_RGB_VALUE,MED_RGB_VALUE);
	m_AnsiColorBackGround = RGB(0,0,0);
}

COutWnd::PutString(char * minstr)
{
//	return VTPutString(minstr);
	if(!*minstr){TRACE("\nBlank String in putstring.");delete minstr;return TRUE;}
    char *cstr = new char[BUFFSIZE];
	const unsigned char *instr=(const unsigned char *)minstr;
	char *str=cstr;
	int toggle=0,nCharat=0,nMaxChars=80;
	if(GetApp()->m_bScreenWrap)
		nMaxChars=m_nCols;
	CString sLeftOver;
	if(!m_bLastcr)	// the last line didn't end with a carriage return.
	{
		sLeftOver=m_Lines.GetHead();
		instr=(const unsigned char *)(const char *)sLeftOver;
		m_Lines.RemoveHead();
		InvalidateLine(0);
	}
	const unsigned char *pLastBrk=instr;
	const unsigned char *pLastBroken=instr;
	while(1)    // strip out carriage returns and convert to newlines.
	{
		int nChars = 1;
		if( !*instr || *instr == '\r' || *instr == '\n' || nCharat >= nMaxChars) // carriage return
		{
			if(GetApp()->m_bWordWrap && nCharat>=nMaxChars && pLastBrk!=pLastBroken)
			{
				str-=(instr-pLastBrk);
				instr=pLastBrk;
			}
			*str=0;
			if((*instr=='\r'&&*(instr+1)=='\n')||(*instr=='\n'&&*(instr+1)=='\r'))
				instr++;
			if(m_bLastcr)	// we're not joining a line
				toggle++;	// so scroll
			if(*instr)
			{
				instr++;
				m_bLastcr=TRUE;	// we're not at the EOL prematurely, we WANT to scroll
			}
			else if(instr<(const unsigned char *)minstr || instr>((const unsigned char *)minstr)+BUFFSIZE) // were we working on the leftover string?
			{
				instr=(const unsigned char *)minstr;	// now do the original input string
				pLastBrk=pLastBroken=instr;
				continue;
			}
			m_Lines.AddHead(cstr);						// add string to line list
      while(m_Lines.GetCount()>m_iMaxLines)
        m_Lines.RemoveTail();
			str=cstr;										// reset dest string
			if(toggle==NUMSCROLL)							// scroll up window
			{
				ScrollUp(toggle);
				UpdateWindow();
				toggle=0;
			}
			if(!*instr)
				break;
			if(nCharat>=nMaxChars ) // this line was split, and is going to be wrapped.
			{
				if(GetApp()->m_bIndent)
				{
					*str++=' ';
					*str++=' ';
				}
				if(!GetApp()->m_bWordWrap)
					instr--;
			}
			nCharat=0;										// reset char count
			pLastBrk=pLastBroken=instr;
		}
		else if(isprint(*instr)||(*instr == 27&&(!GetApp()->m_bAnsi||(nChars=MyAnsiNext((const char *)instr))==1)))
		{	// this character will be printed on screen
			if(*instr==' ' || *instr== '\t')
				pLastBrk= instr;
			*str++=*instr++;
			nCharat++;
		}
		else if(*instr=='\t')
		{
			int iNum=8-(nCharat%8);
			for(int loop=0;loop<iNum;loop++)
			{
				*str++=' ';
				nCharat++;
			}
			instr++;
		}
		else if(*instr == 27)
		{
			ASSERT(GetApp()->m_bAnsi && nChars!=1);
			unsigned const char* pAnsiCode=(instr+nChars-1);
			switch(*pAnsiCode)
			{
				case 'K':					// line erase functions
					switch(*(pAnsiCode-1))
					{	
						case '0':
						case '[':
							*str=0;
							orignull=cstr;
							break;
						case '1':{
							LPSTR pStr=cstr;
							while(pStr<str)
								*pStr++=' ';
							break;}
						case '2':
							orignull=str=cstr;
							*str=0;
							break;
					}
				default:
					instr+=nChars;
					break;
				case 'm':
					for(int loop=0;loop<nChars;loop++)
						*str++=*instr++;
					break;
			}
		}
		else	// we're not copying a character
		{
			if(*instr == 7 && GetApp()->m_bBell) // bell character
				MessageBeep(0xFFFFFFFF);
			instr++;		// skip over this character
		}
	}
	m_bLastcr=(*(instr-1)=='\r' || *(instr-1)=='\n');
	if(toggle)
	{
		ScrollUp(toggle);
		UpdateWindow();
	}
	delete cstr;
	delete minstr;
	return TRUE;
}

// line 0 is bottom of screen
COutWnd::InvalidateLine(int nLine)
{
	CRect r;
	GetClientRect(&r);
	int nBottom=r.bottom-(GetLineHeight()*nLine);
	CRect wr(r.left,nBottom-GetLineHeight(),r.right,nBottom);//now erase the last line on the screen.
	InvalidateRect(&wr);
	return TRUE;
}

void COutWnd::OnEnterPressed()
{
	m_bLastcr=TRUE;
	if(m_nRow==0)
		m_bNextNew=TRUE;
}

void COutWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	int lineheight=GetLineHeight();
	CRect r;
	GetClientRect(&r);

	CFont f;
	f.CreateFontIndirect(&m_LogFont);
	CFont *oldfont = dc.SelectObject(&f);
	int numvislines=r.Height()/GetLineHeight();

	CMudApp *pApp = (CMudApp *)AfxGetApp();

	dc.SetTextColor(m_AnsiColorForeGround);
	dc.SetBkColor(m_AnsiColorBackGround);
  dc.SetTextAlign(TA_BOTTOM);
	BOOL bInSelection=FALSE;
  int iTopLine=min(m_Lines.GetCount()-1,m_iOffset+numvislines);
  POSITION pos =NULL;
  if(m_Lines.GetCount()>0)
    pos= m_Lines.FindIndex(iTopLine);
	for(int loop=iTopLine;loop>=m_iOffset && pos !=NULL;loop--)
	{
    
		int textbottom=r.Height()-lineheight*(loop-m_iOffset);
		CRect wr(r.left,textbottom-lineheight,r.right,textbottom);
		CString sLine=m_Lines.GetPrev(pos);
    if(dc.RectVisible(&wr))
		{	
      LPCSTR pOrigStr;
			LPCSTR pStr = pOrigStr = sLine;
      
			CString outstr;
			int leftoffset=r.left;

			int x=0;
			bInSelection=IsInSelection(x,loop);
			while(*pStr)
			{
				outstr="";
				int num=1;	// 1 if not ansi character, >1 if ansi character sequence
				while(*pStr && ((num = MyAnsiNext(pStr)) ==1) && IsInSelection(x,loop)== bInSelection)
				{
					outstr+=*pStr;
					pStr++;
					x++;
				}
				if(outstr.GetLength() && dc.RectVisible(&wr))
				{
				  if(bInSelection)	// the text we just went over was in the selection.
				  {
					  dc.SetBkColor(pApp->m_colorForeGround);
					  dc.SetTextColor(pApp->m_colorBackGround);
				  }
				  else
				  {
					  dc.SetTextColor(m_AnsiColorForeGround); 
					  dc.SetBkColor(m_AnsiColorBackGround);
				  }
					dc.TextOut(leftoffset,textbottom,outstr);
					leftoffset+=outstr.GetLength()*m_iCharWidth;
				}
				bInSelection=IsInSelection(x,loop);
				if(*pStr && num!=1)
				{
					char ansicode = *(pStr+(num-1));
					switch(ansicode)
					{
						case 'm':
							{
								pStr+=2;
								num-=2;
								while(num)
								{
									int value = atoi(pStr);
									switch(value)
									{
										case 0:			// reset
											AnsiReset();
											dc.SetTextColor(m_AnsiColorForeGround);
											dc.SetBkColor(m_AnsiColorBackGround);
											break;
										case 1:			// BOLD
										{
											m_bBold=TRUE;
											int cindex = m_iFGIndex+(m_bBold?8:0);
											m_AnsiColorForeGround=colormap[cindex];
											dc.SetTextColor(m_AnsiColorForeGround);
											num--;
											pStr++;
											break;
										}
										case 4:			// underline
										case 5:			// blink
										case 7:			// reverse
										case 8:			// invisible
											num--;
											pStr++;
											break;
										case 30:case 31:case 32:case 33:case 34:case 35:case 36:case 37:
										{
											num-=2;
											// set the foreground color
											m_iFGIndex=value-30;
											int cindex = m_iFGIndex+(m_bBold?8:0);
											m_AnsiColorForeGround=colormap[cindex];
											dc.SetTextColor(m_AnsiColorForeGround);
											pStr+=2;
											break;
										}
										case 40:case 41:case 42:case 43:case 44:case 45:case 46:case 47:
											num-=2;
											// set the Background color
											m_AnsiColorBackGround=colormap[value-40];
											dc.SetBkColor(m_AnsiColorBackGround);
											pStr+=2;
											break;
										default:
											TRACE("\nError: Unknown ansi 'm' numeric comand.");
											num--;
											pStr++;
											break;
									}
									num--;
									pStr++;
								}
							}
							break;
						default:	// unknown
							TRACE("\nUnknown ansi escape, hiding it.");
						case 0: // truncated sequence
						case 'A':case'B':case 'C':case 'D':case'R':case 'n':case'H':case'f':case's':case 'u':case'J':case'k':
							// not yet supported
							pStr+=num;
							break;
					}
				}
			}
		}
	}
	dc.SelectObject(oldfont);
}

BOOL COutWnd::IsInSelection(int x,int y)
{

	if(m_ptUp==m_ptDown)
		return FALSE;

	CPoint minpt;
	CPoint maxpt;

	if(m_ptDown.y<m_ptUp.y)
	{
		minpt=m_ptDown;
		maxpt=m_ptUp;
	}
	else if(m_ptDown.y>m_ptUp.y)
	{
		minpt=m_ptUp;
		maxpt=m_ptDown;
	}
	else if(m_ptDown.x<m_ptUp.x)
	{
		minpt=m_ptDown;
		maxpt=m_ptUp;
	}
	else
	{
		minpt=m_ptUp;
		maxpt=m_ptDown;
	}

	if(y > minpt.y && y < maxpt.y)
		return TRUE;
	if(y == minpt.y)
	{
		if(y == maxpt.y)
		{
			if(x >=minpt.x && x < maxpt.x)
				return TRUE;
			else
				return FALSE;
		}
		if(x< minpt.x)
			return TRUE;
	}
	if(y == maxpt.y && x>=maxpt.x)
		return TRUE;
	return FALSE;
}

CPoint COutWnd::PointToChar( CPoint pt)
{
	CRect r;
	GetClientRect(&r);

	pt.x= pt.x/m_iCharWidth;
	pt.y= (r.bottom-pt.y)/GetLineHeight();
	pt.y+=m_iOffset;
	return pt;
}

void COutWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CWnd::OnLButtonDown(nFlags, point);

	m_bUnPauseWhenSelectionDone = !((CMudView *) GetParent())->Pause(TRUE);

	m_ptUp=m_ptDown = PointToChar(point);
	TRACE("\nDown: (%4d,%4d)",m_ptDown.x,m_ptDown.y);
	SetCapture();
	SetTimer(1,10,0);
}

void COutWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	KillTimer(1);
	CWnd::OnLButtonUp(nFlags, point);
	if(GetCapture()!=this)
		return;
	ReleaseCapture();
	m_ptUp = PointToChar(point);

	TRACE("\n  Up: (%4d,%4d)",m_ptUp.x,m_ptUp.y);
	// prepare clipboard
	if(!OpenClipboard())
	{
		AfxMessageBox("Unable to open clipboard");
		Invalidate();
		m_ptUp=m_ptDown=CPoint(0,0);
		return;
	}
	EmptyClipboard();

	int iSize=0;
	// count size of text;

	m_ptUp.x=max(0,m_ptUp.x);
	m_ptDown.x=max(0,m_ptDown.x);

	for(int loop=max(0,min(m_ptUp.y,m_ptDown.y));loop<=max(m_ptUp.y,m_ptDown.y) && loop<m_Lines.GetCount();loop++)
		iSize+=m_Lines.GetAt(m_Lines.FindIndex(loop)).GetLength()+3;

	// Allocate a global memory object for the text.
    HGLOBAL hglbCopy = GlobalAlloc(GMEM_DDESHARE,iSize);
    if (hglbCopy == NULL) {
		AfxMessageBox("Couldn't allocate memory to copy to clipboard");
        CloseClipboard();
		Invalidate();
		m_ptUp=m_ptDown=CPoint(0,0);
        return;
    }

    // Lock the handle and copy the text to the buffer.
	LPSTR pBuf;
    LPSTR pCopy = pBuf=(LPSTR)GlobalLock(hglbCopy);
	for(loop=min(m_Lines.GetCount()-1,max(m_ptUp.y,m_ptDown.y));loop>=max(0,min(m_ptUp.y,m_ptDown.y));loop--)
	{
    CString s=m_Lines.GetAt(m_Lines.FindIndex(loop));
		if(loop==max(m_ptUp.y,m_ptDown.y))	// first line, so defer to first char.
		{
			int firstchar;
			int lastchar=s.GetLength();
			if(m_ptUp.y > m_ptDown.y)
				firstchar=m_ptUp.x;
			else
				firstchar=m_ptDown.x;

			if(m_ptUp.y==m_ptDown.y)
			{
				firstchar=min(m_ptDown.x,m_ptUp.x);
				lastchar=max(m_ptDown.x,m_ptUp.x);
				lastchar=min(lastchar,s.GetLength());
			}
			int length=lastchar-firstchar;
			pCopy+=Ansimemcpy(pCopy,(LPCSTR)s,firstchar,length);
			if(m_ptUp.y!=m_ptDown.y)
			{
				*pCopy='\n';
				pCopy++;
			}
		}
		else if(loop==min(m_ptUp.y,m_ptDown.y))	// last line, so copy only up to last char.
		{
			int lastchar;
			if(m_ptUp.y < m_ptDown.y)
				lastchar=m_ptUp.x;
			else
				lastchar=m_ptDown.x;
			lastchar=min(lastchar,s.GetLength());
			pCopy+=Ansimemcpy(pCopy,s,0,lastchar);
		}
		else
		{
			pCopy+=Ansimemcpy(pCopy,s,0,s.GetLength());
			*pCopy='\n';
			pCopy++;
		}
	}

    *pCopy=0;    // null character
    GlobalUnlock(hglbCopy);

    // Place the handle on the clipboard.
    SetClipboardData(CF_TEXT, hglbCopy);

	
	if(!CloseClipboard())
	{
		AfxMessageBox("Unable to close clipboard");
	}
	m_ptUp=m_ptDown=CPoint(0,0);
	Invalidate();

	if(m_bUnPauseWhenSelectionDone)
		((CMudView *) GetParent())->Pause(FALSE);
	m_bUnPauseWhenSelectionDone=FALSE;
}

void COutWnd::OnMouseMove(UINT nFlags, CPoint point) 
{	
	CWnd::OnMouseMove(nFlags, point);
	if(GetCapture()==this)
	{
		CPoint ptOldUp=m_ptUp;
		m_ptUp=PointToChar(point);

		for(int loop=min(m_ptUp.y,ptOldUp.y);loop<=max(m_ptUp.y,ptOldUp.y);loop++)
		{
			InvalidateLine(loop-m_iOffset);
		}
	}
}



void COutWnd::OnTimer(UINT nIDEvent) 
{
	CWnd::OnTimer(nIDEvent);

	if(m_ptUp.y < m_iOffset)
		OnVScroll(SB_LINEDOWN,0,0);

	CRect r;
	GetClientRect(&r);
	int numvislines=r.Height()/GetLineHeight();

	if(m_ptUp.y >m_iOffset+numvislines)
		OnVScroll(SB_LINEUP,0,0);
}
