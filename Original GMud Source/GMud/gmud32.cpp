/////////////////////////////////////////////////////////////////////////////
//
// Mud32.cpp : Defines the class behaviors for the application.
//
// CMudApp

#include "stdafx.h"
#define DEFINE_STORAGE_HERE
#include "gmud32.h"

#include "mainfrm.h"
#include "childfrm.h"
#include "gmud32doc.h"
#include "gmud32vw.h"
#include "stdlib.h"
#include "textclr.h"
#include "soundsdi.h"
#include "wrapopti.h"
#include "io.h"
#include "world.h"
#include "lowlev.h"
#include "ctype.h"


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

CString GetFileName();

BEGIN_MESSAGE_MAP(CMudApp, CWinApp)
	//{{AFX_MSG_MAP(CMudApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_WINDOW_NEW_OPEN_DOC, OnWindowNewOpenDoc)
	ON_COMMAND(ID_FILE_EDITWORLDLIST, OnFileEditworldlist)
	ON_COMMAND(ID_VIEW_FONT, OnViewFont)
	ON_COMMAND(ID_WINDOW_COLOR, OnWindowColor)
	ON_COMMAND(ID_SOUNDS_SETTINGS, OnSoundsSettings)
	ON_COMMAND(ID_VIEW_WRAP, OnViewWrap)
	ON_COMMAND(ID_FILE_TOGGLELOCALECHOING, OnFileTogglelocalechoing)
	ON_UPDATE_COMMAND_UI(ID_FILE_TOGGLELOCALECHOING, OnUpdateFileTogglelocalechoing)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMudApp construction

#define GMUD_PRESENCE_IDENTIFIER "GMud is in Memory"

CMudApp::CMudApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	char buff[270];
	GetCurrentDirectory(270,buff);
	m_sDir=buff;

//	WNDCLASS wc;
//	memset(wc,0,sizeof(WNDCLASS));
//	wc.lpszClassName=GMUD_PRESENCE_IDENTIFIER;
//	RegisterClass(&wc);
}

CMudApp::~CMudApp()
{
	WriteProfileString("font","Facename",m_LogFont.lfFaceName);
	WriteProfileInt("Font","Size",m_LogFont.lfHeight);

	char temps[22];
	wsprintf(temps,"%ld",m_colorBackGround);
	WriteProfileString("colors","Background",temps);

	wsprintf(temps,"%ld",m_colorForeGround);
	WriteProfileString("colors","Foreground",temps);

	WriteProfileInt("Sounds","Bell",m_bBell);
	WriteProfileInt("Sounds","Trigger",m_bTriggerSound);
	WriteProfileInt("Sounds","ConnectBeep",m_bConnectBeep);

	WriteProfileInt("Wrap","Screen",m_bScreenWrap);
	WriteProfileInt("Wrap","Word",m_bWordWrap);
	WriteProfileInt("Wrap","Indent",m_bIndent);
	WriteProfileInt("Display","Ansi",m_bAnsi);
	WriteProfileInt("Display","LocalEcho",m_bEcho);
}

CString OldGetFileName()
{
	CString filename="WorldLis.mud";
	CString dir;
	dir=getenv("HOMEPATH");
	if(dir=="")
	{
		//"//users/username/"
		char temps[_MAX_PATH];
		GetWindowsDirectory(temps,_MAX_PATH);
		dir=temps;
	}
	if(dir[dir.GetLength()-1]!='\\')
		dir+="\\";
	return dir+filename;
}

CString GetFileName()
{
	CString filename="WorldLis.mud";
	return GetApp()->m_sDir+"\\"+filename;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CMudApp object


/////////////////////////////////////////////////////////////////////////////
// CMudApp initialization

BOOL CMudApp::InitInstance()
{
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	Enable3dControls();
	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	memset(&m_LogFont,0,sizeof(m_LogFont));
	strcpy(m_LogFont.lfFaceName,GetProfileString("font","facename","Courier New"));
	m_LogFont.lfHeight=GetProfileInt("font","size",-12);

	// get background color
	CString str=GetProfileString("colors","Background","none");
	if(str=="none" || (!atoi(str) && *str!='0'))
		m_colorBackGround=RGB(0,0,0);
	else
		sscanf(str,"%ld",&m_colorBackGround);

	// get foreground color
	str=GetProfileString("colors","Foreground","none");
	if(str=="none" || (!atoi(str) && *str!='0'))
		m_colorForeGround=RGB(255,255,255);
	else
		sscanf(str,"%ld",&m_colorForeGround);

	// Get Sounds Settings
	m_bBell=GetProfileInt("Sounds","Bell",1);
	m_bTriggerSound=GetProfileInt("Sounds","Trigger",1);
	m_bConnectBeep=GetProfileInt("Sounds","ConnectBeep",0);

	// get screen wrapping preferences
	m_bScreenWrap=GetProfileInt("Wrap","Screen",0);
	m_bWordWrap=GetProfileInt("Wrap","Word",0);
	m_bIndent=GetProfileInt("Wrap","Indent",0);
	m_bEcho=GetProfileInt("Display","LocalEcho",1);

	// other screen preferences
	m_bAnsi=GetProfileInt("Display","Ansi",1);

	// Register the application's document templates.  Document templates
	// Serve as the connection between documents, frame windows and views.
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_MudTYPE,
		RUNTIME_CLASS(CMudDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CMudView));
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;
	CMudDoc *pDoc;
	// create a new (empty) document
	if(access(GetFileName(),0))
	{
		if(access(OldGetFileName(),0))
		{
			AfxMessageBox("Could not find World List, Creating new world list");
			OnFileNew();
		}
		else
		{
			AfxMessageBox("Moving world list data file from "+OldGetFileName()+" to "+GetFileName());
			pDoc=(CMudDoc *)OpenDocumentFile(OldGetFileName());
		}
	}
	else
		pDoc=(CMudDoc *)OpenDocumentFile(GetFileName());
	
	CMDIChildWnd * pChild=pMainFrame->MDIGetActive();
	if(pChild)
	{
		pDoc=(CMudDoc *)pChild->GetActiveDocument();
		pChild->MDIMaximize();
	}
	if(pDoc)
		pDoc->SetPathName(GetFileName(),FALSE);
	else
	{
		AfxMessageBox("Error creating world list data file "+GetFileName()+", aborting program.");
		return FALSE;
	}
	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();


	if(!*m_lpCmdLine)
		return TRUE;

	// process command line arguments
	long port=0;
	char *progname="GMUD32";
	char mudname[255];
	CWorld *pWorld = 0;

	LPSTR pStr;	// skip any leading space
	lstrcpy(mudname,skipspace(m_lpCmdLine));
	pStr=mudname;
	while(*pStr)				// skip to end of string
		pStr++;

	while(isspace(*(pStr-1)))	// remove trailing spaces
		pStr--;
	
	*pStr=0;					// null term string
	pWorld = pDoc->GetWorld(m_lpCmdLine);	// try whole string first, in case numbers at end of world string
	if(!pWorld)		// couldn't find it that way...
	{
		while(isdigit(*(pStr-1)))	// skip back to beginning of port number
			pStr--;

		if(isdigit(*pStr))			// we've found a port number
		{
			port=atol(pStr);
			while(isspace(*(pStr-1)))	// remove spaces between address/port
				pStr--;
			*pStr=0;
		}
		pWorld = pDoc->GetWorld(mudname,(WORD)port);
	}
	if(!pWorld)	// couldn't find world or host/port
	{
		if(!port)	// weren't specifying port, so error
		{
			CString message;
			message.Format("Could not find world \'%s\'\nUsage: %s worldname\n   or: %s hostname port",m_lpCmdLine,progname,progname);
			AfxMessageBox(message);
			return TRUE;
		}
		pWorld = new CWorld;
		pWorld->m_sName = mudname;
		pWorld->m_sHostName = mudname;
		pWorld->m_wPort = (WORD)port;
		pDoc->AddWorld(pWorld);
	}

	CMudView *pView = (CMudView *)pChild->GetActiveView();
	if(pWorld)
		pView->Connect(pWorld);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CMudApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CMudApp commands // CWinApp OnWindowNew

void CMudApp::OnWindowNewOpenDoc() 
{
	if(!GetOpenDocumentCount())
		OpenDocumentFile(GetFileName());
	else
		m_pMainWnd->PostMessage(WM_COMMAND,ID_WINDOW_NEW);
}

void CMudApp::OnFileEditworldlist() 
{
	CMudDoc *pDoc=0;
	int deletedoc=FALSE;
	if(!GetOpenDocumentCount())
	{
		pDoc = new CMudDoc();
		if(!pDoc->OnOpenDocument(GetFileName()))
		{
			delete pDoc;
			pDoc=0;
		}
		else
			deletedoc=TRUE;
	}
	else
	{
		CMainFrame *pFrame = (CMainFrame *)GetMainWnd();

		if(pFrame)
		{
			CMDIChildWnd *pChild = pFrame->MDIGetActive();
			if(pChild)
				pDoc=(CMudDoc *)pChild->GetActiveDocument();
		}
	}
	if(pDoc)
	{
		pDoc->EditWorldList();
		pDoc->OnSaveDocument(GetFileName());
	}
	else
		AfxMessageBox("Error opening World List, Cannot edit.");
	if(deletedoc)
		delete pDoc;
}

void CMudApp::OnViewFont() 
{
	CFontDialog dlg(&m_LogFont,CF_ANSIONLY|CF_FIXEDPITCHONLY|CF_FORCEFONTEXIST|CF_INITTOLOGFONTSTRUCT|CF_SCREENFONTS);
	if(dlg.DoModal()==IDOK)
	{
		memcpy(&m_LogFont,dlg.m_cf.lpLogFont,sizeof(LOGFONT));
		m_pMainWnd->SendMessageToDescendants(WM_FONTCHANGED);
	}
}

void CMudApp::OnWindowColor()
{
	CTextClrDlg dlg;
	dlg.m_colorBackGround=m_colorBackGround;
	dlg.m_colorForeGround=m_colorForeGround;
	dlg.m_bAnsi = m_bAnsi;
	if(dlg.DoModal()==IDOK)
	{
		m_colorBackGround=dlg.m_colorBackGround;
		m_colorForeGround=dlg.m_colorForeGround;
		m_bAnsi=dlg.m_bAnsi;
		m_pMainWnd->SendMessageToDescendants(WM_COLORCHANGED);
	}
}

void CMudApp::OnSoundsSettings() 
{
	CSoundsDialog dlg;
	dlg.m_bBell = m_bBell;
	dlg.m_bTriggerSound=m_bTriggerSound;
	dlg.m_bConnectBeep=m_bConnectBeep;
	if(dlg.DoModal()==IDOK)
	{
		m_bBell = dlg.m_bBell;
		m_bTriggerSound=dlg.m_bTriggerSound;
		m_bConnectBeep=dlg.m_bConnectBeep;
	}
}

void CMudApp::OnViewWrap() 
{
	CWrapOptions dlg;

	dlg.m_iWrapWidth = m_bScreenWrap;
	dlg.m_iWordWrap = m_bWordWrap;
	dlg.m_bIndent = m_bIndent;
	if(dlg.DoModal()==IDOK)
	{
		m_bScreenWrap = dlg.m_iWrapWidth;
		m_bWordWrap = dlg.m_iWordWrap;
		m_bIndent=dlg.m_bIndent;
	}
}

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CenterWindow();	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMudApp::OnFileTogglelocalechoing() 
{
	if(m_bEcho)
		m_bEcho=FALSE;
	else
		m_bEcho=TRUE;
	
}

void CMudApp::OnUpdateFileTogglelocalechoing(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bEcho);
}
