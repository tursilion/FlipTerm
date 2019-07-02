/////////////////////////////////////////////////////////////////////////////
//
// Mud32vw.cpp : implementation of the CMudView class
//
// CMudView
//

#include "stdafx.h"
#include "gmud32.h"

#include "gmud32doc.h"
#include "gmud32vw.h"
#include "world.h"
#include "connectd.h"

#include "outwnd.h"
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

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CMudView, CView)
BEGIN_MESSAGE_MAP(CMudView, CView)
	//{{AFX_MSG_MAP(CMudView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_SERVER_CONNECT, OnServerConnect)
	ON_UPDATE_COMMAND_UI(ID_SERVER_CONNECT, OnUpdateServerConnect)
	ON_COMMAND(ID_SERVER_DISCONNECT, OnServerDisconnect)
	ON_UPDATE_COMMAND_UI(ID_SERVER_DISCONNECT, OnUpdateServerDisconnect)
	ON_MESSAGE(WM_SOCKET_DISCONNECTED,OnSocketDisconnected)
	ON_MESSAGE(WM_SOCKET_CONNECTED,OnSocketConnected)
	ON_MESSAGE(WM_SOCKET_STRING_RECIEVED,OnStringReceived)
	ON_MESSAGE(WM_ASYNCH_GETHOST_COMPLETE,OnAsynchGetHostCompleted)
	ON_MESSAGE(WM_ENTER_PRESSED,OnUserInput)
	ON_MESSAGE(WM_FONTCHANGED,OnFontChanged)
	ON_MESSAGE(WM_COLORCHANGED,OnColorChanged)
	ON_WM_SETFOCUS()
	ON_WM_CTLCOLOR()
	ON_COMMAND(ID_FILE_EDITTRIGGERLIST, OnFileEdittriggerlist)
	ON_UPDATE_COMMAND_UI(ID_FILE_EDITTRIGGERLIST, OnUpdateFileEdittriggerlist)
	ON_COMMAND(ID_WINDOW_PAUSE, OnWindowPause)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_PAUSE, OnUpdateWindowPause)
	ON_COMMAND(ID_FILE_TOGGLELOGGING, OnFileTogglelogging)
	ON_UPDATE_COMMAND_UI(ID_FILE_TOGGLELOGGING, OnUpdateFileTogglelogging)
	ON_COMMAND(ID_PASTE_FILE, OnPasteFile)
	ON_WM_VSCROLL()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define new DEBUG_NEW

CString GetFileExceptionString( int cause);
/////////////////////////////////////////////////////////////////////////////
// CMudView construction/destruction
CMudView::CMudView()
{
	m_pInWnd=0;
	m_pOutWnd=0;
	m_pStatusWnd=0;
	m_pSocket=0;

	m_pHostBuf= new char[MAXGETHOSTSTRUCT];
	m_pWorld=0;
	m_pBGBrush=0;
	m_bUnSeen=-1;
	m_bActive=FALSE;
	m_bIsLogging=FALSE;
	m_fLogFile=NULL;
	m_bConnectionPending=FALSE;
}

CMudView::~CMudView()
{
	if(m_pInWnd)
		delete m_pInWnd;
	if(m_pOutWnd)
		delete m_pOutWnd;
	if(m_pStatusWnd)
		delete m_pStatusWnd;
	if(m_pSocket)
		delete m_pSocket;

	delete m_pHostBuf;
	if(m_pBGBrush)
		delete m_pBGBrush;
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	pFrame->PostMessage(WM_UPDATE_WORLDS);

	if(m_bIsLogging)
	{
		m_fLogFile->Flush();
		m_fLogFile->Close();
		delete m_fLogFile;
		m_fLogFile=0;
		m_bIsLogging=FALSE;
	}

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
	CMudApp *pApp = (CMudApp *)AfxGetApp();
	m_pBGBrush= new CBrush(pApp->m_colorBackGround);
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	m_pOutWnd = new COutWnd();
	if(!m_pOutWnd->Create(0,0,WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,2113))
	{
		AfxMessageBox("Error creating Output window");
		return -1;
	}
	m_pStatusWnd = new CStatusWnd();
	if(!m_pStatusWnd->Create(0,0,WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,2114))
	{
		AfxMessageBox("Error creating status line window");
		return -1;
	}
	m_pSocket = new CSmartSocket();
	m_pSocket->SetParentWnd(this);
	m_pInWnd = new CInWnd();
	if(!m_pInWnd->Create(WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL|ES_WANTRETURN,CRect(0,0,0,0),this,2112))
	{
		AfxMessageBox("Error creating input window");
		return -1;
	}
	LOGFONT *pLogFont = &((CMudApp *)AfxGetApp())->m_LogFont;
	m_pOutWnd->NewFont(pLogFont);
	m_pStatusWnd->NewFont(pLogFont);
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	return 0;
}

void CMudView::OnSize(UINT nType, int cx, int cy)
{
	if(nType)
		CView::OnSize(nType, cx, cy);
	
	int linesize=m_pOutWnd->GetLineHeight()+5;
	m_pOutWnd->MoveWindow(0,0,cx,cy-linesize*2);
	m_pStatusWnd->MoveWindow(0,cy-linesize*2,cx,linesize);
	m_pInWnd->MoveWindow(0,cy-linesize,cx,linesize);
}

// user requests to connect to a server.
void CMudView::OnServerConnect()
{
	CConnectDialog dlg(this);
	dlg.m_paWorlds = &((CMudDoc *)GetDocument())->m_aWorlds;
	dlg.m_pTriggers = &((CMudDoc *)GetDocument())->m_aTriggers;
	dlg.m_pMacros = &((CMudDoc *)GetDocument())->m_aMacros;
	dlg.m_pAliases = &((CMudDoc *)GetDocument())->m_aAliases;
	GetDocument()->SetModifiedFlag();
	if(dlg.DoModal()==IDOK)
	{
		Connect(dlg.m_pWorld);
	}
}

CMudView::Connect(CWorld *pWorld)
{
	m_pSocket->HardClose();
	m_pWorld = pWorld;
	m_pSocket->Create();
	m_sAddress=m_pWorld->m_sHostName;
	m_wPort=m_pWorld->m_wPort;
	m_sName=m_pWorld->m_sName;
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
	return TRUE;
}

// dns lookup has finished
afx_msg LONG CMudView::OnAsynchGetHostCompleted(UINT,LONG lParam)
{
	ASSERT(m_pWorld);
	WORD nError = WSAGETASYNCERROR(lParam);
	if(nError)
	{
		Printf("%%%%%% Error %d - %s -in DNS lookup of %s.\n",(int)nError,m_pSocket->SockerrToString(nError),m_pWorld->m_sHostName);
		WORD nBufSize = WSAGETASYNCBUFLEN(lParam);
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
		m_pSocket->Create();
		Printf("%%%%%% Error Connecting: %s\n",m_pSocket->SockerrToString(nErrCode));
		Printf("%%%%%% Retrying connection to %s on port %d.\n",(LPCSTR)m_sAddress,(int)m_wPort);
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
	CMudApp *pApp = (CMudApp *)AfxGetApp();
	if(pApp->m_bConnectBeep)
		MessageBeep(0);
	Printf("%%%%%% Connected to %s on port %d.\n",(LPCSTR)m_pWorld->m_sHostName,(int)m_pWorld->m_wPort);
	
	if(m_pWorld->m_sConnectString.GetLength())
	{
		m_pSocket->Printf("%s\n",m_pWorld->m_sConnectString);
		if(GetApp()->m_bEcho)
			Printf("%s\n",m_pWorld->m_sConnectString);
	}
	m_bConnectionPending=FALSE;
	return TRUE;
}

afx_msg LONG CMudView::OnSocketDisconnected(UINT  nErrCode ,LONG)
{
	m_bConnectionPending=FALSE;
	switch(nErrCode)
	{
		case 0:
			break;
		default:
			TRACE("\nError code in onsocketdisconnected: %d",nErrCode);
			break;
	}
	if(!GetApp()->m_bEcho)
		m_pOutWnd->OnEnterPressed();
	Printf("%%%%%% Disconnected from server.\n");
	CFrameWnd *pWnd = (CFrameWnd *)GetParent();
	pWnd->OnUpdateFrameTitle(GetFocus()==m_pInWnd);
	m_pWorld=0;
	SetUnSeen();
	((CMainFrame *)(AfxGetMainWnd()))->UpdateWorlds();
	return TRUE;
}

void CMudView::OnServerDisconnect()
{
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
	LPSTR pStripped = new char[BUFFSIZE];
	CMudDoc *pDoc = (CMudDoc *)GetDocument();
	Ansimemcpy(pStripped,pStr,0,BUFFSIZE);
	CSelArray *paTriggers= (CSelArray *)pDoc->m_aTriggers.Get(m_pWorld->m_sTriggerList);
	m_pOutWnd->PutString(pStr);
	if(m_bIsLogging)
	{
		try
		{
			m_fLogFile->WriteString(pStripped);
		}
		catch( CFileException* e )
		{
		    // Handle the file exceptions here.
			AfxMessageBox("Error writing to log file:\n"+GetFileExceptionString(e->m_cause));
			delete e;
		}
	}
	for(int loop=0;loop<paTriggers->GetSize();loop++)
	{
		((CTrigger *)(paTriggers->Get(loop)))->CheckActivation(pStripped,this);
	}
	SetUnSeen();
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	pFrame->UpdateWorlds();
	delete pStripped;
	return TRUE;
}

void CMudView::SetUnSeen( BOOL bUnSeen /* = TRUE */ )
{
	if(	!m_bUnSeen && !m_bActive)
	{
		m_bUnSeen=bUnSeen;
		CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
		pFrame->UpdateWorlds();
	}
}

afx_msg LONG CMudView::OnUserInput(UINT,LONG lParam)
{
	LPCSTR pStr = (LPSTR)lParam;
	if(!GetApp()->m_bEcho)
		m_pOutWnd->OnEnterPressed();

  CString sStr=pStr;

  // check aliases
  if(m_pWorld)
  {
	  CAliasArray *pAliases= (CAliasArray *)GetDocument()->m_aAliases.Get(m_pWorld->m_sAliasList);
	  sStr = pAliases->CheckAliases(sStr);
  }

  // command stacking
  for(int loop=0;loop<sStr.GetLength();loop++)
    if(sStr[loop]=='|')
      sStr.SetAt(loop,'\n');

  for(loop=0;loop<sStr.GetLength();loop++)
  {
    if(sStr[loop]=='\n')
    {
      CString sLine=sStr.Left(loop);
      sStr=sStr.Mid(loop+1);
      loop=0;
      ProcessOneLine(sLine);
    }
  }
  ProcessOneLine(sStr);
  return TRUE;
}

long CMudView::ProcessOneLine(CString sStr)
{

  // check user variables
  if(!sStr.IsEmpty() && GetUserVariables())
  {
    CMapStringToString *pUserVariables = GetUserVariables();

    int iPos=-1;
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
  if(GetApp()->m_bEcho)
		Printf("%s\n\r",(LPCSTR)sStr);

  // check for '/' command
  if(!sStr.IsEmpty() && sStr[0]=='/')
  {
    if(ProcessUserCommandLine(sStr))
      return TRUE; // else it's not a command
  }

  //put string
	m_pSocket->Printf("%s\n",(LPCSTR)sStr);
	return TRUE;
}

afx_msg LONG CMudView::OnFontChanged(UINT,LONG)
{
	LOGFONT *pLogFont = &((CMudApp *)AfxGetApp())->m_LogFont;
	m_pOutWnd->NewFont(pLogFont);
	m_pStatusWnd->NewFont(pLogFont);
	return TRUE;
}


void CMudView::OnSetFocus(CWnd* pOldWnd) 
{
	CView::OnSetFocus(pOldWnd);
	SetUnSeen(FALSE);
	if(m_pInWnd)
		m_pInWnd->SetFocus();	
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

HBRUSH CMudView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	ASSERT_VALID(m_pBGBrush);
	HBRUSH hbr = CView::OnCtlColor(pDC, pWnd, nCtlColor);
	CMudApp *pApp = (CMudApp *)AfxGetApp();

	if ( nCtlColor== CTLCOLOR_EDIT ) 
	{
		// Set the text background color.
		pDC->SetBkColor(pApp->m_colorBackGround);
		// Set the text foreground color.
		pDC->SetTextColor(pApp->m_colorForeGround);
		// Return the control background brush.
	}
	return 	(HBRUSH)m_pBGBrush->m_hObject;
}

afx_msg LONG CMudView::OnColorChanged(UINT,LONG)
{
	delete m_pBGBrush;
	CMudApp *pApp = (CMudApp *)AfxGetApp();
	m_pBGBrush= new CBrush(pApp->m_colorBackGround);
	m_pInWnd->Invalidate();
	m_pOutWnd->Invalidate();
	m_pStatusWnd->Invalidate();
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

void CMudView::OnUpdateFileEdittriggerlist(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pWorld?1:0);
}

void CMudView::OnWindowPause() 
{
	if(m_pSocket)
		m_pSocket->Pause(!m_pSocket->m_bPaused);
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
	if(m_bIsLogging)
	{
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
		CFileDialog dlg(FALSE,"LOG","*.log",OFN_HIDEREADONLY,"Log Files (*.log) | *.log | All Files (*.*) | *.* ||",this);
		if(dlg.DoModal()==IDOK)
		{
			try
			{
        int iMode=CFile::modeCreate|CFile::modeWrite;
				if(_access("log.txt",0)==0)
				{
					CReplaceDialog rdlg(this);
					rdlg.m_sFileName = dlg.GetPathName();
					switch(rdlg.DoModal())
					{
						case IDOK:
							break;
						case IDOVERWRITE:
							iMode=CFile::modeWrite;
							break;
						default:
						case IDCANCEL:
							AfxMessageBox("File logging operation Canceled.");
							return;
					}
				}
				m_fLogFile= new CStdioFile(dlg.GetPathName(),iMode);
				if(iMode == CFile::modeWrite)
					m_fLogFile->SeekToEnd();
					
			}
			catch( CFileException* e )
			{
			    // Handle the file exceptions here.
				AfxMessageBox("Error opening Log file for writing:\n"+GetFileExceptionString(e->m_cause));
				e->Delete();
				return;
			}
			m_bIsLogging=TRUE;
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
		case CFileException::generic:
		default:
		case CFileException::none:
			return "unknown error";
	}
	return "Error showing error!";
}

BOOL __cdecl CMudView::Printf(LPSTR format, ...)
{
	LPSTR buffer = new char[BUFFSIZE];
	
	va_list marker;
	va_start(marker,format);
	vsprintf(buffer,format,marker);
	va_end(marker);

	// actually print it
	if(m_bIsLogging)
	{
		try
		{
			m_fLogFile->WriteString(buffer);
		}
		catch( CFileException* e )
		{
		    // Handle the file exceptions here.
			AfxMessageBox("Error writing to log file:\n"+GetFileExceptionString(e->m_cause));
			delete e;
		}
	}

	m_pOutWnd->PutString(buffer);

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
  //ASSERT(sInput[0]=='/');

  CMapStringToString * pVariables = GetUserVariables();
  if(!pVariables)
    return FALSE;

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
  }
  else if(strcmpi(sCommand,"/reconnect")==0)
  {
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
  else
    return FALSE; // wasnt command

  return TRUE; // was command
}

BOOL CMudView::OnEraseBkgnd(CDC* /* pDC*/ ) 
{
	return TRUE;
}
