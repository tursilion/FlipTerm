/////////////////////////////////////////////////////////////////////////////
// COutWnd
// outwnd.cpp : implementation file
//

#include "stdafx.h"
#include "outwnd.h"
#include "textfrmt.h"
#include <stdarg.h>
#include <stdio.h>
#include <afxmt.h>
#include "gmud32.h"
#include "gmud32vw.h"
#include "world.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define NUMSCROLL 10

#define DEFAULTFOREGROUNDCOLOR 7
#define DEFAULTBACKGROUNDCOLOR 0

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

#define DIM_RGB_VALUE 0x40		// only for 'bright black'
#define MED_RGB_VALUE 0x80
#define HI_RGB_VALUE  0xFE

CCriticalSection textLock;

unsigned long defcolormap[16]=	// default colors
{
	RGB(0,0,0),												// black
	RGB(MED_RGB_VALUE,0,0),									// red
	RGB(0,MED_RGB_VALUE,0),									// green
	RGB(MED_RGB_VALUE,MED_RGB_VALUE,0),						// yellow
	RGB(0,0,MED_RGB_VALUE),									// blue
	RGB(MED_RGB_VALUE,0,MED_RGB_VALUE),						// magenta
	RGB(0,MED_RGB_VALUE,MED_RGB_VALUE), 					// cyan
	RGB(MED_RGB_VALUE,MED_RGB_VALUE,MED_RGB_VALUE),			// dim white
															// bright colors
	RGB(DIM_RGB_VALUE,DIM_RGB_VALUE,DIM_RGB_VALUE),			// bright black
	RGB(HI_RGB_VALUE,0,0),									// bright red
	RGB(0,HI_RGB_VALUE,0),									// bright green
	RGB(HI_RGB_VALUE,HI_RGB_VALUE,0),						// bright yellow
	RGB(0,0,HI_RGB_VALUE),									// bright blue
	RGB(HI_RGB_VALUE,0,HI_RGB_VALUE),						// bright magenta
	RGB(0,HI_RGB_VALUE,HI_RGB_VALUE), 						// bright cyan
	RGB(HI_RGB_VALUE,HI_RGB_VALUE,HI_RGB_VALUE)				// bright white
};
unsigned long colormap[16];

// Local functions for handling color
int SkipColor(unsigned char *ptr) {
	if (((*ptr)&0xff) == 0xff) { 
		return 2;
	} else {
		return 1;
	}
}

void AddChar(CString *str, int col1, int col2) { 
	if ((col1==0)&&(col2==0)) { 
		*str+=(unsigned char)0xff; 
		*str+=(unsigned char)0x01; 
	} else {
		if ((col1==0xf)&&(col2==0xf)) { 
			*str+=(unsigned char)0xff; 
			*str+=(unsigned char)0x02;
		} else {
			*str+=(unsigned char)((col1<<4)|(col2)); 
		}
	}
}

void GetCols(COutWnd *pWnd, unsigned char *pStr, int *col1, int *col2) {
	if (*(pStr+1) != 0xff) {
		*col1=((*(pStr+1))>>4)&0xf;
		*col2=(*(pStr+1))&0xf;
	} else {
		if (*(pStr+2) == 1) {
			*col1=0;
			*col2=0;
		}
		if (*(pStr+2) == 2) {
			*col1=0xf;
			*col2=0xf;
		}
	}

	int col;
	CMudView *pView;
	pView=(CMudView*)(pWnd->GetParent());

	// If we select the default background color and it's not black, map to black
	if ((*col1 == GetApp()->m_colorBackGround) && (GetApp()->m_colorBackGround != 0)) {
		*col1=0;
	}
	// Remap dim white to the default foreground color
	if (*col1 == 7) {
		col=GetApp()->m_colorForeGround;
		if ((pView)&&(pView->m_pWorld)) {
			if (pView->m_pWorld->m_colorForeGround != -1) {
				col=pView->m_pWorld->m_colorForeGround;
			}
		}
		*col1=col;
	} else {
		// Special case - bold white when the default color is bold white must be handled here,
		// because dim white otherwise gets remapped
		col=GetApp()->m_colorForeGround;
		if ((pView)&&(pView->m_pWorld)) {
			if (pView->m_pWorld->m_colorForeGround != -1) {
				col=pView->m_pWorld->m_colorForeGround;
			}
		}
		if ((*col1 == 15) && (col == 15)) {
			*col1 = 7;
		}
	}
	// Remap black to the default background color
	if (*col2 == 0) {
		col=GetApp()->m_colorBackGround;
		if ((pView)&&(pView->m_pWorld)) {
			if (pView->m_pWorld->m_colorBackGround != -1) {
				col=pView->m_pWorld->m_colorBackGround;
			}
		}
		*col2=col;
	}
}

/////////////////////////////////////////////////////////////////////////////
// COutWnd message handlers

COutWnd::COutWnd()
{
	m_iOffset=0;
	m_iFontHeight=0;
	AnsiReset();
	m_nRow=0;
	m_nCol=0;
	m_nSavedCol=0;
	m_nSavedRow=0;
	m_bNextNew=TRUE;
	m_ptUp=m_ptDown=CPoint(0,0);
	m_bUnPauseWhenSelectionDone=FALSE;
	m_iMaxLines=10000;
	m_bInitialized=false;
	m_AnsiColorForeGround=DEFAULTFOREGROUNDCOLOR;
	m_AnsiColorBackGround=DEFAULTBACKGROUNDCOLOR;
	m_fMouseWasDown=false;
	m_bLastEOL=true;
}

COutWnd::~COutWnd()
{															   
}

int COutWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	SetScrollRange(SB_VERT,0,m_Lines.GetCount());
	SetScrollPos(SB_VERT,m_Lines.GetCount()-m_iOffset,TRUE);
	return 0;
}

int COutWnd::GetLineHeight()
{
	ASSERT(m_iFontHeight);
	return m_iFontHeight;
}

int COutWnd::CalcFontHeight()
{
	CDC dc;
	CFont f;
	RECT myRect;
	int height;

	dc.CreateCompatibleDC(NULL);
	f.CreateFontIndirect(&m_LogFont);
	CFont *oldfont = dc.SelectObject(&f);
	CSize size=dc.GetTextExtent("ABCDEFGHIJKLMNOPQRSTUVWXyZ",26);
	height=size.cy;
	m_iCharWidth=size.cx/26;
	GetClientRect(&myRect);
	m_nCols = (myRect.right-myRect.left)/m_iCharWidth;
	dc.SelectObject(oldfont);

	m_iFontHeight=height;
	return height;
}

int COutWnd::ScrollUp(int numlines /* = 1 */)
{   
	CRect r;
	GetClientRect(&r);
	ScrollWindow(0,0-GetLineHeight()*numlines,&r,&r);
	return TRUE;
}

int COutWnd::NewFont(LPLOGFONT lf)
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
	
	textLock.Lock();
	int numlines=m_Lines.GetCount();
	textLock.Unlock();

	int numvislines=clientrect.Height()/GetLineHeight();
	int numnonvislines=numlines-numvislines;
	
	if(numnonvislines<1) {
		return;
	}

	int pos = 	(int)((int)m_Lines.GetCount()-(int)nPos);
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
	SetScrollPos(SB_VERT,m_Lines.GetCount()-m_iOffset,TRUE);
}

// The only annoyance in here is when the MDI window is maximized, the other windows
// do not get all the appropriate messages for sizing.
// We handle this in the output function - if the active form is maximized, we just
// use it's count instead, as it should be correct.
void COutWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	if (nType == SIZE_MINIMIZED) return;

	SetScrollRange(SB_VERT,0,m_Lines.GetCount(),FALSE);
	SetScrollPos(SB_VERT,m_Lines.GetCount()-m_iOffset,TRUE);
	if ((cx > 0)&&(cy > 0)) {
		CalcFontHeight();
	}
}

BOOL COutWnd::OnEraseBkgnd(CDC* pDC) 
{
	int col;
	CMudView *pView;
	pView=(CMudView*)GetParent();
	col=GetApp()->m_colorBackGround;
	if ((pView)&&(pView->m_pWorld)) {
		if (pView->m_pWorld->m_colorBackGround != -1) {
			col=pView->m_pWorld->m_colorBackGround;
		}
	}
	CBrush wbr(colormap[col]);
	CRect r;
	pDC->GetClipBox(&r);
	pDC->FillRect(&r,&wbr);
	return TRUE;
}

void COutWnd::AnsiReset()
{
	m_iFGIndex=DEFAULTFOREGROUNDCOLOR&0x7;	// not including the BOLD bit
	m_bBold=FALSE;
	m_bInverse=FALSE;

	int ansi;
	ansi=GetApp()->m_bAnsi;
	
	if (NULL != m_hWnd) {
		CMudView *pView;
		pView=(CMudView*)GetParent();
		if ((pView)&&(pView->m_pWorld)) {
			if (pView->m_pWorld->m_bAnsi != -2) {
				ansi=pView->m_pWorld->m_bAnsi;
			}
		}
	}

	if (ansi) {
		m_AnsiColorForeGround = DEFAULTFOREGROUNDCOLOR;
		m_AnsiColorBackGround = DEFAULTBACKGROUNDCOLOR;
	}
}

void COutWnd::ColorString(CString *pStr, int nCol) {
	CString OutBuf;
	int nOrig, fBold;
	char buf1[3]="0m", buf2[3]="0m";

	if ((nCol<0)||(nCol>15)) nCol=11;		// bright yellow default
	buf1[0]=(char)((nCol&0x7)+'0');

	fBold=0;
	nOrig=m_AnsiColorForeGround;
	if (nOrig>7) {
		nOrig-=8;
		fBold=1;
	}

	buf2[0]=(char)(nOrig+'0');
	
	OutBuf="\033[";
	if (nCol > 7) {
		OutBuf+="1;";
	} else {
		OutBuf+="0;";
	}
	OutBuf+="3";
	OutBuf+=buf1;
	OutBuf+=*pStr;
	OutBuf+="\033[";
	if (fBold) {
		OutBuf+="1;";
	} else {
		OutBuf+="0;";
	}
	OutBuf+="3";
	OutBuf+=buf2;

	*pStr=OutBuf;
}

int COutWnd::PutString(char * minstr, int InPaused, int Is7Bit)
{
	CString CStrOut;
	unsigned char *cstr = new unsigned char[BUFFSIZE];
	unsigned char *instr, *pLastBreak, *pLastBroken;
	unsigned char *str;
	int colored, nCharat, nMaxChars;
	bool fEndOfLine;

	static int nLineCount=0;	// to reduce hogging the CPU, we sleep briefly every 20 lines scrolled (any 20)

	if (!*minstr) {
		delete minstr;
		return TRUE;
	}

	nMaxChars=80;
	if(GetApp()->m_bScreenWrap) {
		// If the active window is maximized, we always use it's wrap value,
		// regardless whether it's us or not
		nMaxChars=m_nCols;
		
		int fMax;
		CMDIChildWnd *pChild=((CMDIFrameWndEx*)(GetApp()->m_pMainWnd))->MDIGetActive(&fMax);
		if ((pChild)&&(fMax)) {
			CMudView *pView=(CMudView*)pChild->GetActiveView();
			if (pView) {
				nMaxChars=(pView->GetWidth())/m_iCharWidth;
			}
		}
		nMaxChars--;
		if (nMaxChars < 1) nMaxChars=80;	// give up
	}

	CStrOut=minstr;
	colored=0;

	textLock.Lock();

	if(!m_bLastEOL)						// the last line didn't end with a carriage return.
	{
		CStrOut=m_Lines.GetHead();		// Get the line so we can append to it
		colored=CStrOut.GetLength();	// Note how much was already processed
		CStrOut+=minstr;				// Add the new string to it
		m_Lines.RemoveHead();			// Remove it from the buffer
		InvalidateLine(0);				// Erase it from the screen
	}

	ParseColor(&CStrOut, colored);		// Fix up the coloring and special characters
	instr=(unsigned char*)(const char*)CStrOut;	// Pointer to the string
	pLastBreak=NULL;					// Last word-wrap position noted
	pLastBroken=NULL;					// Last place we actually DID break
	str=cstr;

	nCharat=0;
	fEndOfLine=false;

	while (1) {
		// Process the string - every character includes the color code!
		
		// Check for end of line
		if ((*instr=='\0')|(*instr=='\n')|(nCharat >= nMaxChars)) {
			if (*instr != '\0') {
				fEndOfLine=true;
			}
			// Check for wordwrap
			if (((GetApp()->m_bWordWrap)&(nCharat >= nMaxChars)&(pLastBreak!=NULL)) && (pLastBreak > pLastBroken)) {
				str-=(instr - pLastBreak);
				instr=pLastBreak;
				while (*instr == ' ') {
					instr++;	// skip the space when doing wordwrap
					instr+=SkipColor((unsigned char*)instr);
				}
				pLastBroken=instr;
			}

			*(str++)='\0';
			*(str++)='\0';					// Insurance ;)

			if (*instr=='\n') {				// if a carriage return, skip it
				instr++;
				instr+=SkipColor((unsigned char*)instr);
			}

			m_Lines.AddHead((LPCTSTR)cstr);
			while (m_Lines.GetCount() > m_iMaxLines) {
				m_Lines.RemoveTail();
			}

			if (InPaused) {
				m_iOffset++;				// Adjust offset - don't scroll
				SetScrollPos(SB_VERT,m_Lines.GetCount()-m_iOffset,TRUE);	// fix scroll bar
			} else {
				if (m_bLastEOL) {
					ScrollUp(1);
				} else {
					InvalidateLine(0);
				}
			}
			m_bLastEOL=fEndOfLine;

			UpdateWindow();
			nLineCount++;
			if (nLineCount >= 20) {
				Sleep(1);				// Gives windows a chance to run
				nLineCount=0;
			}

			str=cstr;					// new output string
			nCharat=0;
			fEndOfLine=false;

			if (*instr) {
				if (GetApp()->m_bIndent) {
					CString newtmp("  ");
					ParseColor(&newtmp, 0);
					strcpy((char*)str, newtmp);
					str+=strlen((char*)str);
					nCharat=2;
				}
			} else {
				break;
			}
		} else {
			if ((*instr == 7)&(GetApp()->m_bBell)) {
				MessageBeep(0xffffffff);		// play windows beep
				instr++;
				instr+=SkipColor(instr);
			} else if (*instr == 1) {
				// We just save the character - don't increment charat (nonvisible)
				*(str++)=*(instr++);
				if (((*instr)&0xff) == 0xff) {
					*(str++)=*(instr++);
				}
				*(str++)=*(instr++);
			} else {
				if ((*instr >= ' ')&((!Is7Bit)|(*instr<0x80))) {
					if (strchr("-/:;,.", *instr)) {
						if ((*(instr+1))&&(*(instr+2))) {
							pLastBreak = instr+2;
						}
					}
					if (*instr == ' ') {
						pLastBreak=instr;
					}
					*(str++)=*(instr++);
					if (((*instr)&0xff) == 0xff) {
						*(str++)=*(instr++);
					}
					*(str++)=*(instr++);
					nCharat++;
				} else {
					instr++;
					instr+=SkipColor(instr);
				}
			}
		}
	}

	SetScrollRange(SB_VERT,0,m_Lines.GetCount(),FALSE);
	SetScrollPos(SB_VERT,m_Lines.GetCount()-m_iOffset,TRUE);

	textLock.Unlock();

	delete cstr;
	free(minstr);
	return true;
}

// line 0 is bottom of screen
int COutWnd::InvalidateLine(int nLine)
{
	CRect r;
	GetClientRect(&r);
	int nBottom=r.bottom-(GetLineHeight()*nLine);
	CRect wr(r.left,nBottom-GetLineHeight(),r.right,nBottom);  //now erase the last line on the screen.
	InvalidateRect(&wr);
	return TRUE;
}

void COutWnd::OnEnterPressed()
{
	if(m_nRow==0)
		m_bNextNew=TRUE;
}

void COutWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	int cTextCol;
	int cBkCol;
	int lineheight=GetLineHeight();

	CRect r;
	GetClientRect(&r);
	
	CFont f;
	f.CreateFontIndirect(&m_LogFont);
	CFont *oldfont = dc.SelectObject(&f);

	textLock.Lock();

	int numvislines=r.Height()/GetLineHeight();
	BOOL bInSelection=FALSE;
	int iTopLine=min(m_Lines.GetCount()-1,m_iOffset+numvislines);
	POSITION pos =NULL;
	
	dc.SetTextAlign(TA_BOTTOM);

	if(m_Lines.GetCount()>0) {
		pos=m_Lines.FindIndex(iTopLine);
	}
	
	CString sLine;
	for(int loop=iTopLine; loop>=m_iOffset && pos !=NULL; loop--)
	{
		int textbottom=r.Height()-lineheight*(loop-m_iOffset);
		CRect wr(r.left,textbottom-lineheight,r.right,textbottom);
		sLine=m_Lines.GetPrev(pos);

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
				int ansi;
				CMudView *pView;
				pView=(CMudView*)GetParent();
				ansi=GetApp()->m_bAnsi;
				if ((pView)&&(pView->m_pWorld)) {
					if (pView->m_pWorld->m_bAnsi != -2) {
						ansi=pView->m_pWorld->m_bAnsi;
					}
				}
				if (ansi) {		// only change the colors if it's on
					GetCols(this, (unsigned char*)pStr, &cTextCol, &cBkCol);
				} else {
					int col;
					col=GetApp()->m_colorForeGround;
					if ((pView)&&(pView->m_pWorld)) {
						if (pView->m_pWorld->m_colorForeGround != -1) {
							col=pView->m_pWorld->m_colorForeGround;
						}
					}
					cTextCol=col;

					col=GetApp()->m_colorBackGround;
					if ((pView)&&(pView->m_pWorld)) {
						if (pView->m_pWorld->m_colorBackGround != -1) {
							col=pView->m_pWorld->m_colorBackGround;
						}
					}
					cBkCol=col;
				}

 				while(*pStr)		// The loop continues until a color change or end of line
				{
					int fc, bc;

					if (*pStr == 0x01) {	// ignore code
						pStr++;
						pStr+=SkipColor((unsigned char *)pStr);
						continue;
					}

					if (IsInSelection(x, loop) != bInSelection) {
						break;
					}
	
					if ((pView)&&(pView->m_pWorld)) {
						if (pView->m_pWorld->m_bAnsi != -2) {
							ansi=pView->m_pWorld->m_bAnsi;
						}
					}
					if (ansi) {
						GetCols(this, (unsigned char*)pStr, &fc, &bc);
						if ((fc != cTextCol)||(bc != cBkCol)) {
							break;
						}
					}

					outstr+=*pStr;
					pStr++;
					pStr+=SkipColor((unsigned char*)pStr);
					x++;
				}
				if(outstr.GetLength() && dc.RectVisible(&wr))				// If we have anything visible...
				{
					if(bInSelection)	// the text we just went over was in the selection.
					{
						int col;
						col=GetApp()->m_colorForeGround;
						if ((pView)&&(pView->m_pWorld)) {
							if (pView->m_pWorld->m_colorForeGround != -1) {
								col=pView->m_pWorld->m_colorForeGround;
							}
						}
						dc.SetBkColor(colormap[col]);

						col=GetApp()->m_colorBackGround;
						if ((pView)&&(pView->m_pWorld)) {
							if (pView->m_pWorld->m_colorBackGround != -1) {
								col=pView->m_pWorld->m_colorBackGround;
							}
						}
						dc.SetTextColor(colormap[col]);
					}
					else
					{
						dc.SetTextColor(colormap[cTextCol]); 
						dc.SetBkColor(colormap[cBkCol]);
					}
					dc.TextOut(leftoffset,textbottom,outstr);
					leftoffset+=outstr.GetLength()*m_iCharWidth;
				}

				bInSelection=IsInSelection(x,loop);
			}
		}
	}

	textLock.Unlock();

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
	m_fMouseWasDown=true;

	m_ptUp=m_ptDown = PointToChar(point);
	SetCapture();
	SetTimer(1,10,0);
}

void COutWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	LPSTR pBuf;
    LPSTR pCopy;
    HGLOBAL hglbCopy;
	int loop, iSize;
	
	KillTimer(1);
	CWnd::OnLButtonUp(nFlags, point);
	ReleaseCapture();

	pBuf=NULL;
	pCopy=NULL;
	hglbCopy=NULL;
	loop=0;
	iSize=0;

	if (m_fMouseWasDown == false) {
		goto unpause;		// never got the down, so ignore
	}

	m_ptUp = PointToChar(point);
	if (m_ptDown == m_ptUp) {
		goto unpause;		// never mind
	}

	// prepare clipboard
	if(!OpenClipboard())
	{
		AfxMessageBox("Unable to open clipboard");
		Invalidate();
		m_ptUp=m_ptDown=CPoint(0,0);
		goto unpause;
	}
	EmptyClipboard();

	iSize=0;
	// count size of text;

	m_ptUp.x=max(0,m_ptUp.x);
	m_ptDown.x=max(0,m_ptDown.x);

	textLock.Lock();

	for(loop=max(0,min(m_ptUp.y,m_ptDown.y));loop<=max(m_ptUp.y,m_ptDown.y) && loop<m_Lines.GetCount();loop++) {
		iSize+=(m_Lines.GetAt(m_Lines.FindIndex(loop)).GetLength()/2)+5;
	}

	// Allocate a global memory object for the text.
    hglbCopy = GlobalAlloc(GMEM_DDESHARE,iSize);
    if (hglbCopy == NULL) {
		textLock.Unlock();
        CloseClipboard();
		Invalidate();
		AfxMessageBox("Couldn't allocate memory to copy to clipboard");
		m_ptUp=m_ptDown=CPoint(0,0);
        goto unpause;
    }

    // Lock the handle and copy the text to the buffer.
    pCopy = pBuf=(LPSTR)GlobalLock(hglbCopy);
	for(loop=min(m_Lines.GetCount()-1,max(m_ptUp.y,m_ptDown.y));loop>=max(0,min(m_ptUp.y,m_ptDown.y));loop--)
	{
    CString s=m_Lines.GetAt(m_Lines.FindIndex(loop));
		if(loop==max(m_ptUp.y,m_ptDown.y))	// first line, so defer to first char.
		{
			int firstchar;
			int lastchar=s.GetLength()/2;
			if(m_ptUp.y > m_ptDown.y) {
				firstchar=m_ptUp.x;
			} else {
				firstchar=m_ptDown.x;
			}

			if(m_ptUp.y==m_ptDown.y)
			{
				firstchar=min(m_ptDown.x,m_ptUp.x);
				lastchar=max(m_ptDown.x,m_ptUp.x);
				lastchar=min(lastchar,s.GetLength()/2);
			}
			firstchar*=2;
			lastchar*=2;
			int length=lastchar-firstchar;
			char *ptr = (char*)(LPCSTR)s;
			ptr+=firstchar;
			while (length>0) {
				*(pCopy++)=*(ptr++);
				ptr++;
				length-=2;
			}
			if(m_ptUp.y!=m_ptDown.y)
			{
				*(pCopy++)='\r';
				*(pCopy++)='\n';
			}
		}
		else if(loop==min(m_ptUp.y,m_ptDown.y))	// last line, so copy only up to last char.
		{
			int lastchar;
			if(m_ptUp.y < m_ptDown.y)
				lastchar=m_ptUp.x;
			else
				lastchar=m_ptDown.x;
			lastchar=min(lastchar,s.GetLength()/2);
			lastchar*=2;
			char *ptr = (char*)(LPCSTR)s;
			while (lastchar>0) {
				*(pCopy++)=*(ptr++);
				ptr++;
				lastchar-=2;
			}
		}
		else
		{
			char *ptr = (char*)(LPCSTR)s;
			int length=s.GetLength();
			while (length>0) {
				*(pCopy++)=*(ptr++);
				ptr++;
				length-=2;
			}
			*(pCopy++)='\r';
			*(pCopy++)='\n';
		}
	}

	textLock.Unlock();

    *pCopy=0;    // null character
    GlobalUnlock(hglbCopy);

	if (pBuf == pCopy) {		// ie: was there any data?
		GlobalFree(hglbCopy);
	} else {
		// Place the handle on the clipboard.
		SetClipboardData(CF_TEXT, hglbCopy);
	}
	
	if(!CloseClipboard())
	{
		AfxMessageBox("Unable to close clipboard");
	}
	m_ptUp=m_ptDown=CPoint(0,0);
	Invalidate();

	if ((pBuf != pCopy) && (GetApp()->GetProfileInt("Helpers", "ShowCopy", 1))) {
		int ret = AfxMessageBox("When you select text in FlipTerm, it is automatically copied to the clipboard,\nno additional steps required! Your text is now available on the clipboard, ready to paste.\nDo you want to see this reminder again in the future?", MB_ICONINFORMATION | MB_YESNO);
		if (IDNO == ret) {
			GetApp()->WriteProfileInt("Helpers", "ShowCopy", 0);
		}
	}

unpause:
	if(m_bUnPauseWhenSelectionDone) {
		((CMudView *) GetParent())->Pause(FALSE);
	}
	m_bUnPauseWhenSelectionDone=FALSE;
	m_fMouseWasDown=false;
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

// This function strips out ANSI codes, placing a color code after every character instead
// It also changes all end of lines to \n (allowing \r, \n, \r\n, \n\r), and replaces
// tab characters with spaces to the nearest 8th character
// Unfortunately, the combination 00 - black on black, is illegal, as it will terminate the
// CString buffer. Because we use 0 (black) as  the background color, we need a way to permit black text.
// Therefore, 00 is replaced with FF. FF is always followed by a control byte. Right now, 1 means
// black on black, and 2 means white on white. Never write 00 into the string unless you mean it.
void COutWnd::ParseColor(CString *pInStr, int nColored)
{
	int nCount, iTmp;
	CString pOut;
	char *pStr=(char*)(const char*)*pInStr;

	nCount=0;

	pOut=pInStr->Left(nColored);

	if ((unsigned)nColored <= strlen(pStr)) {
		int tmp=0;
		nCount=nColored;

		pStr+=nColored;			// skip amount already colored
		// nCount is only used for Tabs
		while (tmp<nColored) {
			tmp++;	// skip char
			if (pInStr->GetAt(tmp) == 0xff) {
				tmp++;		// skip control
				nCount--;	// 1 fewer bytes
			}
			tmp++;	// skip color
		}
		nCount/=2;	// divide for visible char count
	} else {
		OutputDebugString("nColored is greated than the strlen of pStr!\n");
		return;
	} 

	while(*pStr)
	{
		int num=1;			// 1 if not ansi character, >1 if ansi character sequence
 		while ((*pStr) && ((num = MyAnsiNext(pStr))==1))	// single character
		{
			switch (*pStr) {
			case '\r':		// end of line?
				pOut+='\n';
				if (m_bInverse) {
					AddChar(&pOut, m_AnsiColorBackGround, m_AnsiColorForeGround);
				} else {
					AddChar(&pOut, m_AnsiColorForeGround, m_AnsiColorBackGround);
				}
				nCount++;
				pStr++;
				if (*pStr == '\n') pStr++;
				if (GetApp()->m_bANSIReset) {
					AnsiReset();
				}
				break;

			case '\n':		// end of line?
				pOut+='\n';
				if (m_bInverse) {
					AddChar(&pOut, m_AnsiColorBackGround, m_AnsiColorForeGround);
				} else {
					AddChar(&pOut, m_AnsiColorForeGround, m_AnsiColorBackGround);
				}
				nCount++;
				pStr++;
				if (*pStr == '\r') pStr++;
				if (GetApp()->m_bANSIReset) {
					AnsiReset();
				}
				break;

			case '\t':		// tab character
				iTmp=8-(nCount%8);
				while (iTmp>0) {
					pOut+=' ';
					if (m_bInverse) {
						AddChar(&pOut, m_AnsiColorBackGround, m_AnsiColorForeGround);
					} else {
						AddChar(&pOut, m_AnsiColorForeGround, m_AnsiColorBackGround);
					}
					nCount++;
					iTmp--;
				}
				pStr++;
				break;

			default:		// no special processing
				pOut+=*pStr;
				if (m_bInverse) {
					AddChar(&pOut, m_AnsiColorBackGround, m_AnsiColorForeGround);
				} else {
					AddChar(&pOut, m_AnsiColorForeGround, m_AnsiColorBackGround);
				}
				nCount++;
				pStr++;
			}
		}
		if ((*pStr) && (num>1))							// ANSI code
		{
			char ansicode = *(pStr+(num-1));
			switch(ansicode)
			{
			case 'm':
				{
					// Carefully skip the Esc and the [
					if (*pStr) pStr++;
					if (*pStr) pStr++;
					if (num>=2) num-=2; else num=0;

					if ((num == 1) && (*pStr == 'm')) {
						// No numbers? That appears to be undefined but used in some places
						// as a reset, so we'll special case it here
						AnsiReset();
						num--;
						pStr++;
						break;
					}

					while(num>0)
					{
						// End of string
						if (!*pStr) {
							num=0;
							continue;
						}

						// Traps ';' and 'm'
						if (!isdigit(*pStr)) {
							num--;
							pStr++;
							continue;
						}

						int value = atoi(pStr);
						switch(value)
						{
						case 0:			// reset
							AnsiReset();
							break;
						case 1:			// BOLD
							{
								m_bBold=TRUE;
								m_AnsiColorForeGround=(m_iFGIndex|0x8);
								int col;
								CMudView *pView;
								pView=(CMudView*)(GetParent());

								col=GetApp()->m_colorForeGround;
								if ((pView)&&(pView->m_pWorld)) {
									if (pView->m_pWorld->m_colorForeGround != -1) {
										col=pView->m_pWorld->m_colorForeGround;
									}
								}
								if (m_AnsiColorForeGround == col) {	// default IS bold? 
									col=GetApp()->m_colorBackGround;
									if ((pView)&&(pView->m_pWorld)) {
										if (pView->m_pWorld->m_colorBackGround != -1) {
											col=pView->m_pWorld->m_colorBackGround;
										}
									}
									if (m_iFGIndex != col) {								// Don't set to the same as the background
										if (m_iFGIndex != 7) {								// White is a special case
											m_AnsiColorForeGround=m_iFGIndex;				// then use non bold here
										}
									}
								}
								break;
							}
						case 7:			// reverse
								m_bInverse=TRUE;
							break;

						case 3:			// Italics
						case 4:			// underline
						case 5:			// blink
						case 8:			// invisible
							break;

						case 30:case 31:case 32:case 33:case 34:case 35:case 36:case 37:
							{
								// set the foreground color
								m_iFGIndex=value-30;
								m_AnsiColorForeGround = (m_iFGIndex|(m_bBold?8:0));
								int col;
								CMudView *pView;
								pView=(CMudView*)(GetParent());

								col=GetApp()->m_colorForeGround;
								if ((pView)&&(pView->m_pWorld)) {
									if (pView->m_pWorld->m_colorForeGround != -1) {
										col=pView->m_pWorld->m_colorForeGround;
									}
								}
								
								if ((m_bBold)&&(m_AnsiColorForeGround == col)) {// default IS bold? 
									col=GetApp()->m_colorBackGround;
									if ((pView)&&(pView->m_pWorld)) {
										if (pView->m_pWorld->m_colorBackGround != -1) {
											col=pView->m_pWorld->m_colorBackGround;
										}
									}
									if (m_iFGIndex != col) {					// Don't set to the same as the background
										if (m_iFGIndex != 7) {					// White is a special case
											m_AnsiColorForeGround=m_iFGIndex;	// then use non bold here
										}
									}
								}
								break;
							}
						case 40:case 41:case 42:case 43:case 44:case 45:case 46:case 47:
							// set the Background color
							m_AnsiColorBackGround = value-40;
							break;
						default:
							break;
						}
						while ((num) && (isdigit(*pStr))) {
							num--;
							pStr++;
						}
					}
				}
				break;
			default:	// unknown
			case 0: // truncated sequence
			case 'A':case'B':case 'C':case 'D':case'R':case 'n':case'H':case'f':case's':case 'u':case'J':case'k':
				// not yet supported
				while (num--) {
					if (*pStr) pStr++;
				}
				break;
			}
		}
	}

	*pInStr=pOut;
}
