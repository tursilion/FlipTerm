/////////////////////////////////////////////////////////////////////////////
//
// Mud32vw.cpp : implementation of the CMudView class
//
// CMudView
//

#include "stdafx.h"
#include "gmud32.h"

#include "outwnd.h"

#include "gmud32doc.h"
#include "gmud32vw.h"
#include "world.h"
#include "connectd.h"

#include "inwnd.h"
#include "csmartsocket.h"
#include "lowlev.h"
#include "statuswn.h"
#include "wrapopti.h"
#include "trigger.h"
#include "mainfrm.h"
#include "textfrmt.h"
#include "io.h"
#include "replaced.h"
#include "AliasArray.h"
#include "disconnect.h"
#include "resource.h"

#include <io.h>
#include <afxmt.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern unsigned long colormap[];
extern CCriticalSection textLock;

IMPLEMENT_DYNCREATE(CMudView, CView)
BEGIN_MESSAGE_MAP(CMudView, CView)
//{{AFX_MSG_MAP(CMudView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_FILE_CONNECT, OnServerConnect)
	ON_UPDATE_COMMAND_UI(ID_FILE_CONNECT, OnUpdateServerConnect)
	ON_COMMAND(ID_FILE_DISCONNECT, OnServerDisconnect)
	ON_UPDATE_COMMAND_UI(ID_FILE_DISCONNECT, OnUpdateServerDisconnect)
	ON_MESSAGE(WM_SOCKET_DISCONNECTED,OnSocketDisconnected)
	ON_MESSAGE(WM_SOCKET_CONNECTED,OnSocketConnected)
	ON_MESSAGE(WM_SOCKET_STRING_RECIEVED,OnStringReceived)
	ON_MESSAGE(WM_ASYNCH_GETHOST_COMPLETE,OnAsynchGetHostCompleted)
	ON_MESSAGE(WM_ENTER_PRESSED,OnUserInput)
	ON_MESSAGE(WM_FONTCHANGED,OnFontChanged)
	ON_MESSAGE(WM_COLORCHANGED,OnColorChanged)
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_VIEW_BROWSELOG, OnViewLog)
	ON_WM_CTLCOLOR()
	ON_COMMAND(ID_EDIT_TRIGGERLIST, OnFileEdittriggerlist)
	ON_COMMAND(ID_WINDOW_PAUSE, OnWindowPause)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_PAUSE, OnUpdateWindowPause)
	ON_COMMAND(ID_FILE_TOGGLELOGGING, OnFileTogglelogging)
	ON_UPDATE_COMMAND_UI(ID_FILE_TOGGLELOGGING, OnUpdateFileTogglelogging)
	ON_COMMAND(ID_FILE_PASTE, OnPasteFile)
	ON_WM_VSCROLL()
	ON_WM_ERASEBKGND()
	ON_COMMAND(IDC_WINDOW1, OnWindow1)
	ON_COMMAND(IDC_WINDOW10, OnWindow10)
	ON_COMMAND(IDC_WINDOW2, OnWindow2)
	ON_COMMAND(IDC_WINDOW3, OnWindow3)
	ON_COMMAND(IDC_WINDOW4, OnWindow4)
	ON_COMMAND(IDC_WINDOW5, OnWindow5)
	ON_COMMAND(IDC_WINDOW6, OnWindow6)
	ON_COMMAND(IDC_WINDOW7, OnWindow7)
	ON_COMMAND(IDC_WINDOW8, OnWindow8)
	ON_COMMAND(IDC_WINDOW9, OnWindow9)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CString GetFileExceptionString( int cause);
/////////////////////////////////////////////////////////////////////////////
// CMudView construction/destruction
CMudView::CMudView()
{
	m_pInWnd=NULL;
	m_pOutWnd=NULL;
	m_pStatusWnd=NULL;
	m_pSocket=NULL;
	m_IdleTimer=0;		// we don't set the idle timer unless the user has at least sent one line of text
	m_IdleTimeout=0;
	
	m_pHostBuf= new char[MAXGETHOSTSTRUCT];
	m_pWorld=0;
	m_pBGBrush=0;
	m_bUnSeen=-1;
	m_bActive=FALSE;
	m_bIsLogging=FALSE;
	m_fLogFile=NULL;
	m_bConnectionPending=FALSE;

	m_Time=(unsigned int)time(NULL);

	FlashCount=0;			// Countdown for title bar flash (needed for 95/NT)
	FlashTimer=0;			// Not started yet
}

CMudView::~CMudView()
{
	if(m_bIsLogging)
	{
		CTime t = CTime::GetCurrentTime();
		CString csStr = t.Format("\n%%%%%% Log closed - %#c\n");
		m_fLogFile->WriteString(csStr);
		m_fLogFile->Flush();
		m_fLogFile->Close();
		delete m_fLogFile;
		m_fLogFile=0;
		m_bIsLogging=FALSE;
	}

	if(m_pInWnd) {
		delete m_pInWnd;
		m_pInWnd=NULL;
	}

	if(m_pOutWnd) {
		delete m_pOutWnd;
		m_pOutWnd=NULL;
	}

	if(m_pStatusWnd) {
		delete m_pStatusWnd;
		m_pStatusWnd=NULL;
	}

	if(m_pSocket) {
		delete m_pSocket;
		m_pSocket=NULL;
	}
	
	if (m_pHostBuf) {
		delete m_pHostBuf;
		m_pHostBuf=NULL;
	}

	if(m_pBGBrush) {
		delete m_pBGBrush;
		m_pBGBrush=NULL;
	}

	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	pFrame->PostMessage(WM_UPDATE_WORLDS);

}

/////////////////////////////////////////////////////////////////////////////
// CMudView drawing
void CMudView::OnDraw(CDC* /* pDC */)
{
	CMudDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
}

/////////////////////////////////////////////////////////////////////////////
// CMudView diagnostics
#ifdef _DEBUG
void CMudView::AssertValid() const
{
	CView::AssertValid();
}
void CMudView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
CMudDoc* CMudView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMudDoc)));
	return (CMudDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMudView message handlers

int CMudView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	int ret;
	int col;

	CMudApp *pApp = GetApp();

	col=pApp->m_colorBackGround;
	if (m_pWorld) {
		if (m_pWorld->m_colorBackGround != -1) {
			col=m_pWorld->m_colorBackGround;
		}
	}
	m_pBGBrush= new CBrush(colormap[col]);
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	m_pOutWnd = new COutWnd();
	if(!m_pOutWnd->Create(0,0,WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,2113))
	{
		AfxMessageBox("Error creating Output window");
		return -1;
	}
	m_pStatusWnd = new CStatusWnd();
	if(!m_pStatusWnd->Create(0,0,WS_CHILD|WS_VISIBLE|WS_DLGFRAME,CRect(0,0,0,0),this,2114))
	{
		AfxMessageBox("Error creating status line window");
		return -1;
	}
	m_pSocket = new CSmartSocket();
	m_pSocket->SetParentWnd(this);
	m_pInWnd = new CInWnd;

	if (pApp->m_bMultiline) {
		ret=m_pInWnd->Create(WS_CHILD|WS_VISIBLE|ES_AUTOVSCROLL|WS_VSCROLL|ES_MULTILINE|ES_WANTRETURN,CRect(0,0,0,0),this,2112);
	} else {
		ret=m_pInWnd->Create(WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL|ES_WANTRETURN,CRect(0,0,0,0),this,2112);
	}
	if(!ret)
	{
		AfxMessageBox("Error creating input window");
		return -1;
	}
	
	LOGFONT *pLogFont;
	pLogFont = &(GetApp())->m_LogFont;

	if (m_pWorld) {
		if (stricmp(m_pWorld->m_LogFont.lfFaceName, "Default")) {
			pLogFont = &m_pWorld->m_LogFont;
		}
	}

	m_pOutWnd->NewFont(pLogFont);
	m_pInWnd->NewFont(pLogFont);
	return 0;
}

void CMudView::OnSize(UINT nType, int cx, int cy)
{
	if (nType) {
		CView::OnSize(nType, cx, cy);
	}
	
	int linesize=m_pOutWnd->GetLineHeight()+5;
	int barsize=GetSystemMetrics(SM_CYFIXEDFRAME)*2; 

	if (GetApp()->m_bMultiline) {
		linesize*=GetApp()->m_NumLines;
	}

	m_pOutWnd->MoveWindow(0,0,cx,cy-linesize-barsize);
	m_pStatusWnd->MoveWindow(0,cy-linesize-barsize,cx,barsize);
	m_pInWnd->MoveWindow(0,cy-linesize,cx,linesize);
}

void CMudView::MoveDivider(int y) {
	// y is the y co-ordinate in client coordinates
	RECT myRect;
	int linesize=m_pOutWnd->GetLineHeight()+5;

	GetClientRect(&myRect);
	y=(y/linesize)*linesize;
	if (y<linesize) y=linesize;
	if (y>myRect.bottom-linesize) y=myRect.bottom-linesize;

	if (m_pStatusWnd) {
		m_pStatusWnd->SetWindowPos(&wndTop, 0, y, 0, 0, SWP_NOSIZE);
	}
}

// Resize the input window to the specified y coordinate 
// (or as close as possible)
void CMudView::ResizeInWnd(int y) {
	RECT myRect;
	int linesize=m_pOutWnd->GetLineHeight()+5;

	GetClientRect(&myRect);
	y=(y/linesize)*linesize;
	if (y<linesize) y=linesize;
	if (y>myRect.bottom-linesize) y=myRect.bottom-linesize;

	GetApp()->m_NumLines=(myRect.bottom - y)/linesize;

	GetClientRect(&myRect);
	OnSize(0, myRect.right-myRect.left, myRect.bottom-myRect.top);
}

// user requests to connect to a server.
void CMudView::OnServerConnect()
{
	KillTimer(1);					// In case it's running

	CConnectDialog dlg(this);
	dlg.m_paWorlds = &((CMudDoc *)GetDocument())->m_aWorlds;
	dlg.m_pTriggers = &((CMudDoc *)GetDocument())->m_aTriggers;
	dlg.m_pMacros = &((CMudDoc *)GetDocument())->m_aMacros;
	dlg.m_pAliases = &((CMudDoc *)GetDocument())->m_aAliases;
	if(dlg.DoModal()==IDOK)
	{
		GetDocument()->Save();
		Connect(dlg.m_pWorld);
		if (dlg.m_pWorld) {
			SetWindowText(dlg.m_pWorld->m_sName);
		}
	}
}

int CMudView::Connect(CWorld *pWorld)
{
	m_pSocket->HardClose();
	m_pWorld = pWorld;
	m_pSocket->Create();
	m_sAddress=m_pWorld->m_sHostName;
	m_wPort=m_pWorld->m_wPort;
	m_sName=m_pWorld->m_sName;

	OnFontChanged(0,0);
	OnColorChanged(0,0);

	if(atoi(m_pWorld->m_sHostName))
	{
		Printf("%%%%%% Connecting to %s on port %d.\n",(LPCSTR)m_pWorld->m_sHostName,(int)m_pWorld->m_wPort);
		m_pSocket->Connect((LPCSTR)m_pWorld->m_sHostName,m_pWorld->m_wPort);
	}
	else
	{
		WSAAsyncGetHostByName(m_hWnd,WM_ASYNCH_GETHOST_COMPLETE,m_pWorld->m_sHostName,m_pHostBuf,MAXGETHOSTSTRUCT);
		Printf("%%%%%% Performing Asynch DNS lookup on %s\n",(LPCSTR)m_pWorld->m_sHostName);
	}
	m_bConnectionPending=TRUE;	
	m_bAllowReconnect=true;

	if (m_pWorld->m_sAutologFile != "") {
		switch(m_pWorld->m_nAutologMode)
		{
		case 0:		// Append
			DoLogAppend(m_pWorld->m_sAutologFile);
			break;
		case 1:		// New
			DoLogNew();
			break;
		case 2:	{	// Daily 
			// If the autolog has not been written today, then we'll do a new. Otherwise we do append.
			int today;
			CTime t = CTime::GetCurrentTime();
			// This isn't accurate. It doesn't matter, as long as the number is different for every day
			today=t.GetMonth()*31 + t.GetDay();

			if (m_pWorld->m_nLastDayLogOpened != today) {
				m_pWorld->m_nLastDayLogOpened=today;
				DoLogNew();
			} else {
				DoLogAppend(m_pWorld->m_sLastAutologFilename);
			}
				}
			break;
		}
	}

	return TRUE;
}

// dns lookup has finished
afx_msg LONG CMudView::OnAsynchGetHostCompleted(UINT,LONG lParam)
{
	if (NULL == m_pWorld) return FALSE;

	WORD nError = WSAGETASYNCERROR(lParam);
	if(nError)
	{
		Printf("%%%%%% Error %d - %s -in DNS lookup of %s.\n",(int)
			nError,m_pSocket->SockerrToString(nError),m_pWorld->m_sHostName.GetString());
		//		WORD nBufSize = WSAGETASYNCBUFLEN(lParam);
		m_pSocket->HardClose();
		return FALSE;
	}
	hostent * pHostEnt = (hostent *)m_pHostBuf;
	char temps[30];
	LPSTR pHost=pHostEnt->h_addr;
	wsprintf(temps,"%d.%d.%d.%d",(int)(unsigned char)pHost[0],(int)(unsigned char)pHost[1],(int)(unsigned char)pHost[2],(int)(unsigned char)pHost[3]);
	m_sAddress = temps;
	Printf("%%%%%% Connecting to %s on port %d\n",temps,(int)m_pWorld->m_wPort);
	m_pSocket->Connect(temps,m_pWorld->m_wPort);
	SetUnSeen();
	m_bConnectionPending=TRUE;	
	return TRUE;
}

afx_msg LONG CMudView::OnSocketConnected(UINT  nErrCode ,LONG )
{
	ASSERT(m_pWorld);
	CFrameWnd *pWnd = (CFrameWnd *)GetParent();
	pWnd->OnUpdateFrameTitle(GetFocus()==m_pInWnd);
	SetUnSeen();
	if(nErrCode)
	{
		m_pSocket->HardClose();
		Printf("%%%%%% Error Connecting: %s\n",m_pSocket->SockerrToString(nErrCode));
		Printf("%%%%%% Waiting 5 seconds to try again...\n");
		SetTimer(1, 5000, NULL);
		return TRUE;
	}
	CMudApp *pApp = GetApp();
	if(pApp->m_bConnectBeep)
		MessageBeep(0);
	Printf("%%%%%% Connected to %s on port %d.\n",(LPCSTR)m_pWorld->m_sHostName,(int)m_pWorld->m_wPort);
	if (m_pWorld->m_bIsIRC) {
		Printf("%%%%%% Remember to set your nick with /NICK <nickname>, and join a channel with /JOIN <channel>\n");
	}
	ResetTimer();
	
	if(m_pWorld->m_sConnectString.GetLength())
	{
		m_pSocket->Printf("%s\n",m_pWorld->m_sConnectString.GetString());
		if(GetApp()->m_bEcho) {
			CString csTmp=m_pWorld->m_sConnectString;
			m_pOutWnd->ColorString(&csTmp, GetApp()->m_colorInput);
			Printf("%s\n", csTmp.GetString());
		}
	}
	m_bConnectionPending=FALSE;
	return TRUE;
}

afx_msg LONG CMudView::OnSocketDisconnected(UINT nErrCode ,LONG)
{
	m_bConnectionPending=FALSE;
	if(!GetApp()->m_bEcho) {
		m_pOutWnd->OnEnterPressed();
	}

	if (m_bIsLogging) {
		CTime t = CTime::GetCurrentTime();
		CString csStr = t.Format("\n%%%%%% Log closed - %#c\n");
		m_fLogFile->WriteString(csStr);

		m_fLogFile->Flush();
		m_fLogFile->Close();
		delete m_fLogFile;
		m_fLogFile=0;
		m_bIsLogging=FALSE;
		Printf("%%%%%% Log file closed.\n");
	}
	Printf("%%%%%% Disconnected from server.\n");
	CFrameWnd *pWnd = (CFrameWnd *)GetParent();
	pWnd->OnUpdateFrameTitle(GetFocus()==m_pInWnd);

	SetUnSeen();
	((CMainFrame *)(AfxGetMainWnd()))->UpdateWorlds();

	if ((nErrCode)&&(m_bAllowReconnect)&&(m_pWorld->m_bReconnect)) {
		Connect(m_pWorld);
	} else {
		m_pWorld=NULL;
	}

	return TRUE;
}

void CMudView::OnServerDisconnect()
{
	KillTimer(1);	// In case it's running, too, though it shouldn't be
	if (GetApp()->GetProfileInt("Helpers", "ConfirmDisconnect", 1)) {
		CDisconnect dlg;
		
		dlg.m_Text.Format("Are you sure you want to disconnect from %s?", (LPCSTR)m_pWorld->m_sName);
		int ret = dlg.DoModal();
		if (dlg.m_Check) {
			GetApp()->WriteProfileInt("Helpers", "ConfirmDisconnect", 0);
		}

		if (IDCANCEL == ret) {
			return;
		}
	}

	m_bAllowReconnect=false;		// don't reconnect
	m_bConnectionPending=FALSE;
	m_pSocket->HardClose();
	CFrameWnd *pWnd = (CFrameWnd *)GetParent();
	pWnd->OnUpdateFrameTitle(GetFocus()==m_pInWnd);
	SetUnSeen();
}

void CMudView::OnUpdateServerConnect(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_pSocket->IsConnected() && !m_bConnectionPending);
}

void CMudView::OnUpdateServerDisconnect(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pSocket->IsConnected() || m_bConnectionPending);
}

afx_msg LONG CMudView::OnStringReceived(UINT ,LONG lparam)
{
	LPSTR pStr = (LPSTR)lparam;
	LPSTR pStripped;
	char *buffer;
	CMudDoc *pDoc = (CMudDoc *)GetDocument();
	int idx, fCR;
	bool ret=false;		// set to true if consumed
	char *ptr;
	CString csOut;

	pStripped=(char*)malloc(BUFFSIZE);

	// work the string one line at a time
	// fix up end of lines, ensure they are always \n and nothing else
	// Check the special case of 0D01 (telnet CR only) and ignore it
	idx=0; ptr=pStr;
	while (*ptr) {
		fCR=false;
		while ((*(pStr+idx)!='\0')&&(*(pStr+idx)!='\r')&&(*(pStr+idx)!='\n')) idx++;
		if (*(pStr+idx)) {
			if (*(pStr+idx+1)==1) {
				// Ignore this combination
				*(pStr+idx)=1;
				idx+=2;
				continue;
			} else {
				*(pStr+idx)='\0';
				fCR=true;
				idx++;
			}
		}
		if (fCR) {
			csOut.Format("%s\n", ptr);
			if (csOut.GetLength()==1) csOut=" \n";
		} else {
			csOut.Format("%s", ptr);
			if (csOut.GetLength()==0) csOut=" ";
		}

		// Set up a work copies of the string
		Ansimemcpy(pStripped,csOut,0,BUFFSIZE);

        // handle IRC intercepts - will set ret to true if processed
		if (m_pWorld->m_bIsIRC) {
			CString csPrefix;	// a place to save the prefix here

			// this is bad since it assumes whole strings, not guaranteed!
			// check for IRC PING, and strip the IRC prefix if it exists
			// check for prefix (first character is ':')
			if (pStripped[0] == ':') {
				// find to first space and delete (use original string)
				int i=csOut.Find(' ');
				if (-1 != i) {
					// save the prefix first...
					csPrefix=csOut.Left(i);
					csOut=csOut.Mid(i+1);
					Ansimemcpy(pStripped,csOut,0,BUFFSIZE);
				}
			}

			// first char should be a command now, we only care about a couple
			if (strnicmp(pStripped, "PING", 4) == 0) {
				// we need to send back a PONG with the right key
				int i=csOut.Find(':');
				if (-1 != i) {
					CString csTmp="PONG ";
					csTmp+=csOut.Mid(i);
					OutputDebugString(csOut);
					OutputDebugString(csTmp);
					ProcessOneLine(true, csTmp);
					ret=true;
				}
			} else if (strnicmp(pStripped, "PRIVMSG", 7) == 0) {
				// this is either a user message or a channel message.
				// for user messages, we keep it all. For channel messages,
				// we want to strip the header. We can tell by looking
				// at the target and comparing to the known channel.
				// PRIVMSG #welcome :hey booger

				CString csTmp=&pStripped[8];
				int i=csTmp.Find(' ');
				if (-1 != i) {
					csTmp=csTmp.Left(i);
				}
				CString csTmp2;
				bool bPriv=false;

				if (m_pWorld->m_csIRCChannel.CompareNoCase(csTmp)) {
					// Not a channel message
					csTmp2="PRIVMSG ";
					bPriv=true;
				}

				// but we need to know who sent it! That's in the prefix:
				// :Tursi75!Miranda@192.168.128.103 
				i=csPrefix.Find('!');
				csTmp2+=csPrefix.Mid(1, i-1);
				
				// get just the message
				i=csOut.Find(':');
				csOut=csOut.Mid(i+1);

				// check if it's an encoded ACTION
				if (0 != strnicmp(csOut, "\001ACTION", 7)) {
					// no, it's not, insert a colon
					// format according to whether it's a private message
					if (bPriv) {
						csTmp2+=" :";
					} else {
						csTmp2+=": ";	
						// I know, subtle difference, but it's about style ;)
					}
				} else {
					// yes it is, strip the command and escape bits
					csOut=csOut.Mid(7);	// keep the space
					csOut.TrimRight('\001');
				}
				csTmp2+=csOut;
				csOut=csTmp2;
				Ansimemcpy(pStripped,csOut,0,BUFFSIZE);
			} else if ((strnicmp(pStripped, "NOTICE", 6) == 0) || (strnicmp(pStripped, "ERROR", 5) == 0)) {
				// this is something from the server?, just preserve it all!
				// do nothing here
			} else if ((strnicmp(pStripped, "QUIT", 4) == 0) || (strnicmp(pStripped, "PART", 4) == 0) || (strnicmp(pStripped, "JOIN", 4) == 0)) {
				// for a channel command, output the whole string so we know who it was
				csOut=csPrefix + " " + csOut;
			} else {
				// strip the command and just output the rest of the string
				// far from perfect, but good enough for now
				int i=csOut.Find(':');
				if (-1 != i) {
					csOut=csOut.Mid(i+1);
					Ansimemcpy(pStripped,csOut,0,BUFFSIZE);
				} else {
					// server only? Output debug and drop it
					OutputDebugString("Dropping IRC-> ");
					OutputDebugString(csOut);
					ret=true;
				}
			}
		}

		// If ret was true, then the IRC code processed the string!
		if (!ret) {
			// Look for URLs for the URL dropdown
			{
				int x=-1;
				CString csTmp;
				csTmp=csOut;
				csTmp.MakeUpper();
				// Yeah, this is the cheesy way ;)
				if (x == -1) x=csTmp.Find("HTTP:");
				if (x == -1) x=csTmp.Find("FTP:");
				if (x == -1) x=csTmp.Find("HTTPS:");
				if (x == -1) x=csTmp.Find("WWW.");
				if (x != -1) {
					// We have a (probable) URL. Get a substring up to the first
					// space (we're out of luck on URLs with spaces)
					int y=csOut.Find(' ', x);
					if (y == -1) {
						csTmp=csOut.Mid(x);
					} else {
						csTmp=csOut.Mid(x, y-x);
					}
					// strip some likely bad characters
					char tmpstr[33];
					int idx2;
					for (idx2=0; idx2<31; idx2++) {
						tmpstr[idx2]=(char)(idx2+1);
					}
					tmpstr[idx2]='\0';
					y=csTmp.FindOneOf(tmpstr);
					if (y != -1) {
						csTmp=csTmp.Left(y);
					}
					csTmp.Remove('\'');
					csTmp.Remove('\"');
					csTmp.Remove(',');
					csTmp.Remove(';');
					if (csTmp.Right(1) == '.') csTmp.TrimRight('.');
					if (csTmp.Left(3).CompareNoCase("www") == 0) {
						csTmp="http://" + csTmp;
					}

					// Okay, now we have a URL, we need to add it to the menu
					CMenu *pMenu=GetApp()->m_pMainWnd->GetMenu();
					if (pMenu) {
						pMenu=pMenu->GetSubMenu(URL_MENU_INDEX);		// fixed location (URLs)
						if (pMenu) {
							CString csTmp2;

							// Check whether we already have this one in the menu
							int nCnt=pMenu->GetMenuItemCount();
							bool fFlag=true;
							for (int menuidx=0; menuidx<nCnt; menuidx++) {
								pMenu->GetMenuString(menuidx, csTmp2, MF_BYPOSITION);
								if (!csTmp.Compare(csTmp2)) {
									fFlag=false;
									break;
								}
							}
							// and don't add empty strings
							if (csTmp=="") fFlag=false;

							// fFlag is true if we want to add it
							if (fFlag) {
								DWORD id=nCnt+ID_URL_1;
								// If there are more than 10 URLs, scroll the top one off
								if (nCnt>9) {
									id=pMenu->GetMenuItemID(0);
									if (id == 0) id=ID_URL_1;		// handle the separator
									pMenu->RemoveMenu(0, MF_BYPOSITION);
								}
								// Now add the new one to the bottom
								pMenu->AppendMenu(MF_ENABLED, id, csTmp);
							}
						}
					}
				}
			}

			if (m_bIsLogging) {
				try {
					m_fLogFile->WriteString(pStripped);
				}
				catch( CFileException* e )
				{
					// Handle the file exceptions here.
					AfxMessageBox("Error writing to log file:\n"+GetFileExceptionString(e->m_cause));
					delete e;
				}
			}

			// Check triggers
			CSelArray *paTriggers= (CSelArray *)pDoc->m_aTriggers.Get(m_pWorld->m_sTriggerList);
			if (paTriggers) {
				bool fFoundAnIdle=false;
				for(int loop=0;loop<paTriggers->GetSize();loop++)
				{
					// Check for an idle trigger so we can update our local variables
					// This means it only updates when we receive text - but that will cover
					// about 98% of the cases :)
					CTrigger *thisTrig=(CTrigger*)(paTriggers->Get(loop));
					CString csName=thisTrig->GetName();
					int x=csName.Find(ANTIIDLETRIGGERTEXT);
					if (x > -1) {
						fFoundAnIdle=true;
						int nNewTime=atoi(csName.Mid(x+ANTIIDLETRIGGERLENGTH));
						if (nNewTime != m_IdleTimeout) {	// only if it's changed!
							// This one sets the idle timeout, we currently only allow one
							if (m_IdleTimer) {
								KillTimer(m_IdleTimer);
								m_IdleTimer=0;
							}
							m_IdleTimeout=nNewTime;
							if (m_IdleTimeout > 0) {
								m_IdleString=thisTrig->m_sMudOutput;
								m_IdleTimer=SetTimer(2, m_IdleTimeout*1000*60, NULL);
								CString csTmp;
								csTmp.Format("Set timer - got ID %d and HWND 0x%p\n", m_IdleTimer, m_hWnd);
								OutputDebugString(csTmp);
							} else {
								m_IdleString="";
							}
						}
					} else {
						if (2 ==thisTrig->CheckActivation(pStripped,this)) {
							m_pOutWnd->ColorString(&csOut, thisTrig->m_fgColor-1);
						}
					}
				}

				if ((!fFoundAnIdle)&&(m_IdleTimer)) {
					KillTimer(m_IdleTimer);
					m_IdleTimer=0;
					m_IdleTimeout=0;
				}
			}

			buffer=(char*)malloc(BUFFSIZE);
			int tmpLen=csOut.GetLength()>BUFFSIZE-1 ? BUFFSIZE-1 : csOut.GetLength();
			memcpy(buffer, (LPCSTR)csOut, tmpLen);
			buffer[tmpLen]='\0';
			m_pOutWnd->PutString(buffer, m_pSocket->m_bPaused, m_pWorld->m_bIs7Bit);	// this function frees the buffer
		}

		// Skip end of line - no more than 1 CRLF pair
		if ((*(pStr+idx)=='\r') || (*(pStr+idx)=='\n')) {
			idx++;
			if (((*(pStr+idx)=='\r') || (*(pStr+idx)=='\n')) && (*(pStr+idx) != *(pStr+idx-1))) {
				idx++;
			}
		}
		ptr=(pStr+idx);
	}
	
	// only mark activity if it didn't get autoprocessed
	if (!ret) {
		SetUnSeen();
	}

	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	pFrame->UpdateWorlds();
	free(pStr);
	free(pStripped);
	return TRUE;
}

void CMudView::SetUnSeen( BOOL bUnSeen /* = TRUE */ )
{
	if(	!m_bUnSeen && !m_bActive)
	{
		m_bUnSeen=bUnSeen;
		if (bUnSeen) {
			CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
			pFrame->UpdateWorlds();
			if (GetApp()->m_bShowActivity) {
				CMDIChildWnd * pChild =	((CMDIFrameWndEx*)(GetApp()->m_pMainWnd))->MDIGetActive();
				CMudView *pView=(CMudView*)pChild->GetActiveView();
				if (pView != this) {
					pView->Printf("\n\r");	// The coloring code needs the %%% to be at the beginning of the string
					pView->Printf("%%%%%% Activity in world '%s'\n\n", m_pWorld->m_sName.GetString());
				}

				if ((GetActiveWindow() != AfxGetMainWnd()) || (AfxGetMainWnd()->IsIconic())) {		// only flash if another app has focus or we are minimized
					FlashCount=10;
					if (0 == FlashTimer) {
						FlashTimer=SetTimer(99, 300, NULL);
					}
				}
			}
		}
	}
}

afx_msg LONG CMudView::OnUserInput(UINT,LONG lParam)
{
	LPCSTR pStr = (LPSTR)lParam;
	if(!GetApp()->m_bEcho) {
		m_pOutWnd->OnEnterPressed();
	}
	
	CString sStr=pStr;

	// Reset the idle timer
	if (m_IdleTimer) {
		KillTimer(m_IdleTimer);
	}
	if (m_IdleTimeout) {
		// Timer is set in minutes
		m_IdleTimer=SetTimer(2, m_IdleTimeout*1000*60, NULL);
		CString csTmp;
		csTmp.Format("Set timer - got ID %d and HWND 0x%p\n", m_IdleTimer, m_hWnd);
		OutputDebugString(csTmp);
	}
	
	// check aliases
	if (m_pWorld)
	{
		CAliasArray *pAliases= (CAliasArray *)GetDocument()->m_aAliases.Get(m_pWorld->m_sAliasList);
		if (pAliases) {
			sStr = pAliases->CheckAliases(sStr);
		}
	}
	
	int loop;
	// command stacking
	if (GetApp()->m_bCommandStacking) {
		for(loop=0;loop<sStr.GetLength();loop++) {
			if(sStr[loop]==GetApp()->m_CommandChar) {
				sStr.SetAt(loop,'\n');
			}
		}
	}
		
	for(loop=0;loop<sStr.GetLength();loop++)
	{
		if(sStr[loop]=='\n')
		{
			CString sLine=sStr.Left(loop);
			sStr=sStr.Mid(loop+1);
			loop=0;
			ProcessOneLine(false, sLine);
		}
	}
	ProcessOneLine(false, sStr);
	return TRUE;
}

long CMudView::ProcessOneLine(bool bSystem, CString sStr)
{
	// check user variables %var%
	if(!sStr.IsEmpty() && GetUserVariables())
	{
		CMapStringToString *pUserVariables = GetUserVariables();
		
		//  int iPos=-1;
		for(int start=0;start<sStr.GetLength();start++)
		{
			if(sStr[start]=='%')
			{
				for(int end=start+1;end<sStr.GetLength();end++)
				{
					if(sStr[end]=='%')
					{
						CString sSuspect = sStr.Mid(start+1,end-start-1);
						CString sValue;
						if(pUserVariables->Lookup(sSuspect,sValue))
						{
							sStr=sStr.Left(start)+sValue+sStr.Mid(end+1);
						}
					}
				}
			}
		}
	}
	
	// echo to user
	if (!bSystem) {
		if(GetApp()->m_bEcho) {
			CString csTmp=sStr;
			m_pOutWnd->ColorString(&csTmp, GetApp()->m_colorInput);
			Printf("%s\n\r",(LPCSTR)csTmp);
		}
	}
	
	// check for '/' command
	if(!sStr.IsEmpty()) {
		if (sStr[0]=='/') {
			if(ProcessUserCommandLine(sStr)) {
				return TRUE; // else it's not a command
			}
		
			if (m_pWorld) {
				// Check join messages, and strip the slash if it's an IRC command
				if (m_pWorld->m_bIsIRC) {
					// just make the command part uppercase
					int i=sStr.Find(' ');
					if (-1 != i) {
						CString csTmp=sStr.Left(i);
						csTmp.MakeUpper();
						sStr=sStr.Mid(i);
						sStr=csTmp+sStr;
					}

					if (sStr.Left(6).CompareNoCase("/join ") == 0) {
						m_pWorld->m_csIRCChannel = sStr.Mid(6);
						OutputDebugString("Got channel ");
						OutputDebugString(m_pWorld->m_csIRCChannel);
						OutputDebugString("\n");
					} else if (sStr.Left(6).CompareNoCase("/nick ") == 0) {
						// make the NICK/USER thing easier, you can redo it
						// later if you really want
						CString csTmp=sStr.Mid(6);
						m_pWorld->m_csNick=csTmp;
						CString csTmp2;
						csTmp2.Format("USER %s 0 * :%s", (LPCSTR)csTmp, (LPCSTR)csTmp);
						sStr+="\r\n";
						sStr+=csTmp2;
					} else if (sStr.Left(4).CompareNoCase("/me ") == 0) {
						// escape for an action command
						sStr="/PRIVMSG " + m_pWorld->m_csIRCChannel + " :\001ACTION " + sStr.Mid(4) + "\001";
					}
					sStr=sStr.Mid(1);
				}
			}
		} else {
			if (m_pWorld) {
				// not empty, format for IRC if appropriate
				if ((m_pWorld->m_bIsIRC) && (!m_pWorld->m_csIRCChannel.IsEmpty())) {
					if (!bSystem) {
						CString csTmp;
						csTmp.Format("PRIVMSG %s :%s", m_pWorld->m_csIRCChannel.GetString(), sStr.GetString());
						sStr=csTmp;
					}
				}
			}
		}
	}
	
	//put string
	m_pSocket->Printf("%s\n",(LPCSTR)sStr);

	if (m_pWorld) {
		if ((m_pWorld->m_bIsIRC) && (!bSystem) && (GetApp()->m_bEcho)) {
			// IRC won't echo your formatted command back, so just pretend it did by running it through the parser
			char *pTmpStr=(char*)malloc(sStr.GetLength() + m_pWorld->m_csNick.GetLength() + 6);
			strcpy(pTmpStr, ":");
			strcat(pTmpStr, m_pWorld->m_csNick);
			strcat(pTmpStr, "! ");
			strcat(pTmpStr, sStr);
			strcat(pTmpStr, "\r\n");
			OnStringReceived(0, (long)pTmpStr);
			// OnStringReceived frees the buffer
		}
	}

	return TRUE;
}

afx_msg LONG CMudView::OnFontChanged(UINT,LONG)
{
	LOGFONT *pLogFont;
	pLogFont = &(GetApp())->m_LogFont;

	if (m_pWorld) {
		if (stricmp(m_pWorld->m_LogFont.lfFaceName, "Default")) {
			pLogFont = &m_pWorld->m_LogFont;
		}
	}

	m_pOutWnd->NewFont(pLogFont);
	m_pInWnd->NewFont(pLogFont);
	return TRUE;
}

void CMudView::OnSetFocus(CWnd* pOldWnd) 
{
	CView::OnSetFocus(pOldWnd);
	SetUnSeen(FALSE);
	if(m_pInWnd) {
		m_pInWnd->SetFocus();	
    }
}

CString CMudView::GetTitle()
{
	if(m_pSocket->IsConnected())
	{
		return m_pWorld->m_sName;
	}
	else
	{
		return CString("Not Connected");
	}
}

HBRUSH CMudView::OnCtlColor(CDC* pDC, CWnd* /*pWnd*/, UINT nCtlColor)
{
	ASSERT_VALID(m_pBGBrush);
	CMudApp *pApp = GetApp();
	
	if ( nCtlColor == CTLCOLOR_EDIT ) 
	{
		// Set the text background color.
		int col;
		col=pApp->m_colorBackGround;
		if (m_pWorld) {
			if (m_pWorld->m_colorBackGround != -1) {
				col=m_pWorld->m_colorBackGround;
			}
		}
		pDC->SetBkColor(colormap[col]);

		// Set the text foreground color.
		col=pApp->m_colorForeGround;
		if (m_pWorld) {
			if (m_pWorld->m_colorForeGround != -1) {
				col=m_pWorld->m_colorForeGround;
			}
		}
		pDC->SetTextColor(colormap[col]);
	}

	// Return the control background brush.
	return 	(HBRUSH)m_pBGBrush->m_hObject;
}

afx_msg LONG CMudView::OnColorChanged(UINT,LONG)
{
	delete m_pBGBrush;
	CMudApp *pApp = GetApp();
	int col;
	col=pApp->m_colorBackGround;
	if (m_pWorld) {
		if (m_pWorld->m_colorBackGround != -1) {
			col=m_pWorld->m_colorBackGround;
		}
	}
	m_pBGBrush= new CBrush(colormap[col]);
	m_pInWnd->Invalidate();
	m_pOutWnd->Invalidate();
	return TRUE;
}

void CMudView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{	
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
	m_bActive=bActivate;
	
	m_bUnSeen=FALSE;
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	pFrame->UpdateWorlds();
}

void CMudView::OnFileEdittriggerlist() 
{
	GetDocument()->EditTriggerList(m_pWorld);
}

void CMudView::OnWindowPause() 
{
	if(m_pSocket) {
		m_pSocket->Pause(!m_pSocket->m_bPaused);
		if (m_pSocket->m_bPaused) {
			// we need to get hacky here....
			m_pSocket->m_bPaused=false;
			Printf("%%%%%% Output paused...\n");
			m_pSocket->m_bPaused=true;
		} else {
			Printf("%%%%%% Output resumed...\n");
		}
	}
}

void CMudView::OnUpdateWindowPause(CCmdUI* pCmdUI) 
{
	if(m_pSocket)
		pCmdUI->SetCheck(m_pSocket->m_bPaused);
	else
		pCmdUI->SetCheck(FALSE);
}

BOOL CMudView::Pause(BOOL bPause)
{
	BOOL bWasPaused = m_pSocket->m_bPaused;
	if(m_pSocket)
		m_pSocket->Pause(bPause);
	return bWasPaused;
}


void CMudView::OnFileTogglelogging() 
{
	if (NULL == m_pWorld) return;

	if(m_bIsLogging)
	{
		CTime t = CTime::GetCurrentTime();
		CString csStr = t.Format("\n%%%%%% Log closed - %#c\n");
		m_fLogFile->WriteString(csStr);

		m_fLogFile->Flush();
		m_fLogFile->Close();
		delete m_fLogFile;
		m_fLogFile=0;
		m_bIsLogging=FALSE;
		AfxMessageBox("Log file closed.");
	}
	else
	{
		ASSERT(m_fLogFile==0);

		CString fn;
		fn.Format("%s\\%s.log", GetApp()->m_sLogFolder.GetString(), m_pWorld->m_sName.GetString());

		CFileDialog dlg(false, "log", fn, OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, "Log Files|*.log;*.txt||", this);
		if (dlg.DoModal()==IDOK)
		{
			try
			{
				int iMode=CFile::modeCreate|CFile::modeWrite;	// Defaults to Overwrite
				if(_access(dlg.GetPathName(),0)==0)
				{
					CReplaceDialog rdlg(this);
					rdlg.m_sFileName = dlg.GetPathName();
					switch(rdlg.DoModal())
					{
					case IDOK:				// Overwrite
						break;
					case IDC_APPEND:		// Apprend
						iMode=CFile::modeWrite;
						break;
					default:
					case IDCANCEL:			// Cancel
						AfxMessageBox("File logging operation Canceled.");
						return;
					}
				}
				m_fLogFile= new CStdioFile(dlg.GetPathName(),iMode);
				if(iMode == CFile::modeWrite) {
					m_fLogFile->SeekToEnd();
				}

				CTime t = CTime::GetCurrentTime();
				CString csStr = t.Format("\n%%%%%% Log opened - %#c\n");
				m_fLogFile->WriteString(csStr);

				Printf("%%%%%% Opened log file %s.\n", dlg.GetPathName().GetString());
				
				textLock.Lock();
				if (m_pOutWnd->m_Lines.GetCount()) {
					if (IDYES == AfxMessageBox("Would you like to begin the log at the beginning of the buffer?", MB_ICONQUESTION | MB_YESNO)) {
						POSITION pos = m_pOutWnd->m_Lines.GetTailPosition();
						CString sTmp, sOut;

						while (pos) {
							sTmp=m_pOutWnd->m_Lines.GetPrev(pos);
							if (sTmp.GetAt(0) != 0x01) {
								// We need to filter out the color codes
								sOut="";
								for (int i=0; i<sTmp.GetLength(); i+=2) {
									sOut+=sTmp.GetAt(i);
								}
								sOut+='\n';		// need end of line, too
								m_fLogFile->WriteString(sOut);
							}
						}
						Printf("%%%%%% Logged from beginning of buffer.\n");
					}
				}
				textLock.Unlock();

				m_bIsLogging=TRUE;
			}
			catch( CFileException* e )
			{
				// Handle the file exceptions here.
				AfxMessageBox("Error opening Log file for writing:\n"+GetFileExceptionString(e->m_cause));
				e->Delete();
				return;
			}
		}
	}
}

void CMudView::OnUpdateFileTogglelogging(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bIsLogging);
}

CString GetFileExceptionString( int cause)
{
	switch(cause)
	{
	case CFileException::endOfFile:
		return "The end of file was reached.";
	case CFileException::diskFull:
		return "The disk is full.";
	case CFileException::lockViolation:
		return "There was an attempt to lock a region that was already locked";
	case CFileException::sharingViolation:
		return "SHARE.EXE was not loaded, or a shared section was locked.";
	case CFileException::hardIO:
		return "There was a hardware error.";
	case CFileException::badSeek:
		return "There was a DISK ERROR trying to set the file pointer.";
	case CFileException::directoryFull:
		return "The current directory is full: there are no more directory entries.";
	case CFileException::removeCurrentDir:
		return "The current working directory cannot be removed.";
	case CFileException::invalidFile:
		return "There was an attempt to use an invalid file handle.";
	case CFileException::accessDenied:
		return "The File could not be accessed.";
	case CFileException::tooManyOpenFiles:
		return "The permitted number of open files was exceeded.";
	case CFileException::badPath:
		return "all or part of the path is invalid";
	case CFileException::fileNotFound:
		return "File not found.";
	case CFileException::genericException:
	default:
	case CFileException::none:
		// Fall out...
		;
	}
	return "unknown error";
}

BOOL __cdecl CMudView::Printf(LPSTR format, ...)
{
	LPSTR buffer;
	CString StrOut;
	
	buffer=(char*)malloc(BUFFSIZE);
	ZeroMemory(buffer, BUFFSIZE);

	va_list marker;
	va_start(marker,format);
	vsprintf(buffer,format,marker);
	va_end(marker);

	StrOut=buffer;
	if (StrOut.Left(3) == "%%%") {			// System message - color it
		m_pOutWnd->ColorString(&StrOut, GetApp()->m_colorSystem);
		StrOut.Insert(0, (char)0x01);
	}
	if (StrOut.GetLength() < BUFFSIZE) {
		strcpy(buffer, (LPCSTR)StrOut);		// Copy it back if it'll fit
	}

	m_pOutWnd->PutString(buffer, m_pSocket->m_bPaused);
	
	return TRUE;
}

void CMudView::OnPasteFile() 
{
	CFileDialog dlg(TRUE,"LOG","*.*",0,"All Files (*.*) | *.* ||",this);
	if(dlg.DoModal()==IDOK)
	{
		try
		{
			CStdioFile fFile(dlg.GetPathName(),CFile::modeRead);
			CString str;
			LPSTR pStr = str.GetBufferSetLength(2049);
			while((pStr=fFile.ReadString(pStr,2048))!=0)
			{
				m_pSocket->Printf("%s",pStr);
				Printf("%s",pStr);
			}
			Printf("\n");
			str.ReleaseBuffer();
		}
		catch( CFileException* e )
		{
			// Handle the file exceptions here.
			AfxMessageBox("Error opening file for Pasting:\n"+GetFileExceptionString(e->m_cause));
			e->Delete();
			return;
		}
	}
}

void CMudView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CView::OnVScroll(nSBCode, nPos, pScrollBar);
	m_pOutWnd->PostMessage(WM_VSCROLL,nSBCode,(long)pScrollBar);
}

CMapStringToString * CMudView::GetUserVariables()
{
	if(!m_pWorld)
		return NULL;
	return m_pWorld->GetUserVariables();
}

bool CMudView::ProcessUserCommandLine(CString sInput)
{
	if (false == GetApp()->m_bUseCLI) {
		return FALSE;
	}
	
	// split off argument
	CString sCommand,sArguments;
	for(int loop=0;loop<sInput.GetLength();loop++)
	{
		if(sInput[loop]==' ')
		{
			sCommand = sInput.Left(loop);
			sArguments = sInput.Mid(loop);
			break;
		}
	}
	if(sCommand == "")
		sCommand = sInput;
	sCommand.TrimRight();
	sArguments.TrimLeft();
	
	if(strcmpi(sCommand,"/set")==0)
	{
		CMapStringToString * pVariables = GetUserVariables();

		if(!pVariables) {
			Printf("** You must be connected to a world first.\n\r");
			return FALSE;
		}
	
		if(sArguments == "") // dump variables to console;
		{
			printf("\n");
			for(POSITION pos=pVariables->GetStartPosition();pos!=NULL;)
			{
				CString sKey,sValue;
				pVariables->GetNextAssoc(pos,sKey,sValue);
				Printf("%%%s%% = %s\n",(LPCSTR)sKey,(LPCSTR)sValue);
			}
		}
		else
		{
			// split off key and value
			CString sKey,sValue;
			for(int loop=0;loop<sArguments.GetLength();loop++)
			{
				char ch=sArguments[loop];
				if(ch =='=' || ch == ' ')
				{
					int begin=loop;
					while(loop<sArguments.GetLength() && (sArguments[loop] == '=' || sArguments[loop]==' '))
						loop++;
					
					sKey = sArguments.Left(begin);
					sValue = sArguments.Mid(loop);
					break;
				}
			}
			if(!sKey.GetLength())
				sKey=sArguments;
			sKey.TrimRight();
			sValue.TrimLeft();
			if(sValue == "")
			{
				if(pVariables->RemoveKey(sKey))
					Printf("\nKey %s removed\n",(LPCSTR)sKey);
				else
					Printf("\nError, could not remove Key:%s\n",(LPCSTR)sKey);
			}
			else
			{
				pVariables->SetAt(sKey,sValue);
				Printf("\n%%%s%% = %s\n",(LPCSTR)sKey,(LPCSTR)sValue);
			}
		}
		return true;
	}
	
	if(strcmpi(sCommand,"/reconnect")==0)
	{
		if(!m_pWorld) {
			Printf("** You must be connected to a world first.\n\r");
			return TRUE;
		}

		m_pSocket->HardClose();
		m_pSocket->Create();
		Printf("%%%%%% Reconnecting to %s on port %d.\n",(LPCSTR)m_sAddress,(int)m_wPort);
		m_bConnectionPending=TRUE;	
		CObArray *pA=&((CMudDoc *)GetDocument())->m_aWorlds;
		for(int loop=0;loop<pA->GetSize();loop++)
		{
			CWorld *pW = (CWorld *)(pA->GetAt(loop));
			if(pW->m_sName == m_sName)
				m_pWorld=pW;
		}
		ASSERT_VALID(m_pWorld);
		m_pSocket->Connect((LPCSTR)m_pWorld->m_sHostName,m_pWorld->m_wPort);
		return TRUE;
	}
	
	if (strcmpi(sCommand, "/connect")==0)
	{
		if(m_pWorld) {
			Printf("** You must disconnect from this world or open a new window first.\n\r");
			return TRUE;
		}

		CConnectDialog dlg(this);
		dlg.m_paWorlds = &((CMudDoc *)GetDocument())->m_aWorlds;
		dlg.m_pTriggers = &((CMudDoc *)GetDocument())->m_aTriggers;
		dlg.m_pMacros = &((CMudDoc *)GetDocument())->m_aMacros;
		dlg.m_pAliases = &((CMudDoc *)GetDocument())->m_aAliases;
		dlg.csWorld=sArguments;
		if (IDOK == dlg.DoModal()) {
			if (dlg.m_pWorld) {
				Connect(dlg.m_pWorld);
			} else {
				MessageBox("Could not find an entry for that world.");
			}
		}
		return TRUE;
	}

	if (strcmpi(sCommand, "/disconnect")==0)
	{
		if(!m_pWorld) {
			Printf("** You must be connected to a world first.\n\r");
			return TRUE;
		}

		m_bAllowReconnect=false;
		m_pSocket->HardClose();

		return TRUE;
	}

	if (strcmpi(sCommand, "/new")==0)
	{
		CMudApp *pApp;

		pApp=GetApp();
		pApp->OnWindowNewOpenDoc();
		return TRUE;
	}

	if (strcmpi(sCommand, "/close")==0)
	{
		CMDIChildWnd * pChild =	((CMDIFrameWndEx*)(GetApp()->m_pMainWnd))->MDIGetActive();
		
		pChild->PostMessage(WM_CLOSE);
		return TRUE;
	}

	if (strcmpi(sCommand, "/window")==0)
	{
		CWnd *mainWnd;
		int idx;

		if (sArguments) {
			idx=atoi(sArguments);
		} else {
			idx=0;
		}
		mainWnd=GetApp()->GetMainWnd();
		
		mainWnd->PostMessage(WM_COMMAND,AFX_IDM_FIRST_MDICHILD+idx-1);
		return TRUE;
	}

	if (strcmpi(sCommand, "/quit")==0)
	{
		CWnd *mainWnd;

		mainWnd=GetApp()->GetMainWnd();
		mainWnd->PostMessage(WM_CLOSE, 0, 0);
		return TRUE;
	}

	if (strcmpi(sCommand, "/help")==0)
	{
		Printf("Command Line Help\n\r");
		Printf("=================\n\r");
		Printf("/close             - Close this window (and connection)\n\r");
		Printf("/connect           - Show the connect dialog\n\r");
		Printf("/connect <world>   - Connect to requested world\n\r");
		Printf("/disconnect        - Disconnect this world\n\r");
		Printf("/help              - Show this text\n\r");
		Printf("/new               - Open a new window\n\r");
		Printf("/quit              - Close the program and all connections\n\r");
		Printf("/set               - Show user variables\n\r");
		Printf("/set <var>=<value> - Set a user variable\n\r");
		Printf("/w<string>         - Speed walk. The string is specified as n,s,e,w,u,d (directions) with an optional\n\r");
		Printf("                     number (1-9). IE: /wn2eu2w means: North, East, East, Up, West, West\n\r");
		Printf("/window <x>        - Switch to window number 'x'\n\r");
		Printf("\n\r");
		return TRUE;
	}

	return FALSE; // wasnt command
}

BOOL CMudView::OnEraseBkgnd(CDC* /* pDC*/ ) 
{
	return TRUE;
}

void CMudView::OnWindow1() 
{
	CWnd *mainWnd;

	mainWnd=GetApp()->GetMainWnd();
	mainWnd->PostMessage(WM_COMMAND,AFX_IDM_FIRST_MDICHILD+0);
}

void CMudView::OnWindow10() 
{
	CWnd *mainWnd;

	mainWnd=GetApp()->GetMainWnd();
	mainWnd->PostMessage(WM_COMMAND,AFX_IDM_FIRST_MDICHILD+9);
}

void CMudView::OnWindow2() 
{
	CWnd *mainWnd;

	mainWnd=GetApp()->GetMainWnd();
	mainWnd->PostMessage(WM_COMMAND,AFX_IDM_FIRST_MDICHILD+1);
}

void CMudView::OnWindow3() 
{
	CWnd *mainWnd;

	mainWnd=GetApp()->GetMainWnd();
	mainWnd->PostMessage(WM_COMMAND,AFX_IDM_FIRST_MDICHILD+2);
}

void CMudView::OnWindow4() 
{
	CWnd *mainWnd;

	mainWnd=GetApp()->GetMainWnd();
	mainWnd->PostMessage(WM_COMMAND,AFX_IDM_FIRST_MDICHILD+3);
}

void CMudView::OnWindow5() 
{
	CWnd *mainWnd;

	mainWnd=GetApp()->GetMainWnd();
	mainWnd->PostMessage(WM_COMMAND,AFX_IDM_FIRST_MDICHILD+4);
}

void CMudView::OnWindow6() 
{
	CWnd *mainWnd;

	mainWnd=GetApp()->GetMainWnd();
	mainWnd->PostMessage(WM_COMMAND,AFX_IDM_FIRST_MDICHILD+5);
}

void CMudView::OnWindow7() 
{
	CWnd *mainWnd;

	mainWnd=GetApp()->GetMainWnd();
	mainWnd->PostMessage(WM_COMMAND,AFX_IDM_FIRST_MDICHILD+6);
}

void CMudView::OnWindow8() 
{
	CWnd *mainWnd;

	mainWnd=GetApp()->GetMainWnd();
	mainWnd->PostMessage(WM_COMMAND,AFX_IDM_FIRST_MDICHILD+7);
}

void CMudView::OnWindow9() 
{
	CWnd *mainWnd;

	mainWnd=GetApp()->GetMainWnd();
	mainWnd->PostMessage(WM_COMMAND,AFX_IDM_FIRST_MDICHILD+8);
}

void CMudView::DoLogAppend(CString fn) {
	try
	{
		m_fLogFile= new CStdioFile(fn,CFile::modeWrite|CFile::modeCreate|CFile::modeNoTruncate|CFile::typeText);
		m_fLogFile->SeekToEnd();
		Printf("%%%%%% Auto-opened log file %s for append.\n", fn.GetString());
		
		CTime t = CTime::GetCurrentTime();
		CString csStr = t.Format("\n%%%%%% Log opened - %#c\n");
		m_fLogFile->WriteString(csStr);

		m_bIsLogging=TRUE;
	}
	catch( CFileException* e )
	{
		// Handle the file exceptions here.
		if (e->m_cause == CFileException::sharingViolation) {
			AfxMessageBox("Unable to auto-log this session. Auto-log may only append one file per world.");
		} else {
			AfxMessageBox("Error opening auto-log file for append: "+GetFileExceptionString(e->m_cause)+"\nThe path was: " + fn);
		}
		e->Delete();
		m_bIsLogging=FALSE;
	}
}

void CMudView::DoLogNew() {
	// Need to create a unique filename
	CString fn;
	CString tmp;
	CString tmpname;
	char parsename[1024];
	try
	{
		CString ext;
		int idx;

		idx=m_pWorld->m_nLastAutoFileNumber;
		if (idx<1) idx=1;
	
		if (strchr(m_pWorld->m_sAutologFile, '%')) {
			struct tm *now;
			time_t tt;
			time(&tt);
			now=localtime(&tt);
			strftime(parsename, 1023, m_pWorld->m_sAutologFile, now);
			for (idx=2; parsename[idx]; idx++) {
				if (parsename[idx]==':') parsename[idx]='.';
				if (strchr("\\/*?\"<>|", parsename[idx])) parsename[idx]='-';
			}
			idx=1;
		} else {
			strncpy(parsename,m_pWorld->m_sAutologFile, 1023);
			parsename[1023]='\0';
		}

		if (_access(parsename, 0) == 0) {
			tmpname=parsename;
			if (tmpname.ReverseFind('.') != -1) {
				ext=tmpname.Right(tmpname.GetLength()-tmpname.ReverseFind('.'));
				fn=tmpname.Left(tmpname.GetLength()-ext.GetLength());
			} else {
				fn=tmpname;
				ext="";
			}

			do {
				tmp.Format("%s_%d%s", fn.GetString(), idx++, ext.GetString());
			} while (_access(tmp, 0) == 0);
		} else {
			tmp=parsename;
		}
		
		m_pWorld->m_nLastAutoFileNumber=idx;
		m_pWorld->m_sLastAutologFilename=tmp;

		m_fLogFile= new CStdioFile(tmp,CFile::modeCreate|CFile::modeWrite|CFile::typeText);
		
		Printf("%%%%%% Auto-opened new log file %s.\n", tmp.GetString());
		
		CTime t = CTime::GetCurrentTime();
		CString csStr = t.Format("\n%%%%%% Log opened - %#c\n");
		m_fLogFile->WriteString(csStr);

		m_bIsLogging=TRUE;
	}
	catch( CFileException* e )
	{
		// Handle the file exceptions here.
		AfxMessageBox("Error opening auto-log file for writing: "+GetFileExceptionString(e->m_cause)+"\nThe path was: " + tmp);
		e->Delete();
		m_bIsLogging=FALSE;
	}
}

void CMudView::OnViewLog()
{
	// Open the default log folder
	if ((int)ShellExecute(NULL, "open", GetApp()->m_sLogFolder, NULL, NULL, SW_SHOWNORMAL) < 32) {
		AfxMessageBox("Unable to open the default log folder - check that it is configured correctly.", MB_ICONSTOP | MB_OK);
	}
}

void CMudView::OnTimer(UINT nIDEvent) 
{
	int loop=0;
	CObArray *pA=NULL;

	if (NULL == this) {
		return;
	}

	if (nIDEvent == m_IdleTimer) {
		if (!m_IdleString.IsEmpty()) {
			if (NULL != m_pWorld) {
				OutputDebugString("Idle Timeout\n");
				ProcessOneLine(true, m_IdleString);
			}
		}
		return;
	}

	switch (nIDEvent) {
	case 1:		// Retry connection
		KillTimer(1);
		m_pSocket->Create();
		Printf("%%%%%% Retrying connection to %s on port %d.\n",(LPCSTR)m_sAddress,(int)m_wPort);
		m_bConnectionPending=TRUE;	
		pA=&((CMudDoc *)GetDocument())->m_aWorlds;
		for(loop=0;loop<pA->GetSize();loop++)
		{
			CWorld *pW = (CWorld *)(pA->GetAt(loop));
			if(pW->m_sName == m_sName)
				m_pWorld=pW;
		}
		ASSERT_VALID(m_pWorld);
		m_pSocket->Connect((LPCSTR)m_pWorld->m_sHostName,m_pWorld->m_wPort);
		break;

	case 99:	// Window flash
		if (FlashCount > 0) {
			FlashCount--;
			if (FlashCount) {
				AfxGetMainWnd()->FlashWindow(true);
			} else {
				AfxGetMainWnd()->FlashWindow(false);
				KillTimer(FlashTimer);
				FlashTimer=0;
			}
		}
		break;
	}

	CView::OnTimer(nIDEvent);
}

void CMudView::HistoryPrev() {
	m_pInWnd->HistoryPrev();
}

void CMudView::HistoryNext() {
	m_pInWnd->HistoryNext();
}


void CMudView::OnDestroy() 
{
	if (m_IdleTimer) {
		KillTimer(m_IdleTimer);
		m_IdleTimer=0;
	}

	CView::OnDestroy();
}
