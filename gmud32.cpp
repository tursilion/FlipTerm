/////////////////////////////////////////////////////////////////////////////
//
// Mud32.cpp : Defines the class behaviors for the application.
//
// CMudApp

#include "stdafx.h"
#include <process.h>
#include <shlobj.h>
#include "gmud32.h"

#include "afxwinappex.h"
#include "afxdialogex.h"
#include "outwnd.h"
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
#include "utils.h"
#include "inwnd.h"
#include "switches.h"
#include "colorset.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

void StartupThread(void *pDat);

BEGIN_MESSAGE_MAP(CMudApp, CWinAppEx)
	//{{AFX_MSG_MAP(CMudApp)
	ON_COMMAND(ID_HELP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_URL_1, OnURL1)
	ON_COMMAND(ID_URL_2, OnURL2)
	ON_COMMAND(ID_URL_3, OnURL3)
	ON_COMMAND(ID_URL_4, OnURL4)
	ON_COMMAND(ID_URL_5, OnURL5)
	ON_COMMAND(ID_URL_6, OnURL6)
	ON_COMMAND(ID_URL_7, OnURL7)
	ON_COMMAND(ID_URL_8, OnURL8)
	ON_COMMAND(ID_URL_9, OnURL9)
	ON_COMMAND(ID_URL_10, OnURL10)
	ON_COMMAND(ID_WINDOW_NEW_OPEN_DOC, OnWindowNewOpenDoc)
	ON_COMMAND(ID_EDIT_WORLDLIST, OnFileEditworldlist)
	ON_COMMAND(ID_OPTIONS_FONT, OnViewFont)
	ON_COMMAND(ID_OPTIONS_COLOR, OnWindowColor)
	ON_COMMAND(ID_OPTIONS_SYSTEMMESSAGECOLOR, OnSystemColor)
	ON_COMMAND(ID_OPTIONS_DEFAULTCOLORS, OnDefaultColor)
	ON_COMMAND(ID_OPTIONS_SOUND, OnSoundsSettings)
	ON_COMMAND(ID_OPTIONS_WRAP, OnViewWrap)
	ON_COMMAND(ID_OPTIONS_DEFAULTLOGFOLDER, OnOptionsLog)
	ON_COMMAND(ID_OPTIONS_SWITCHES, OnOptionsSwitches)
	ON_UPDATE_COMMAND_UI(ID_FILE_EXIT, OnUpdateExit)
//	ON_UPDATE_COMMAND_UI(ID_EDIT_ALIASLIST, OnUpdateAliasList)
	ON_COMMAND(ID_FILE_EXIT, OnQuit)
	ON_COMMAND(ID_FILE_SAVESETTINGS, OnSave)
	ON_COMMAND(ID_FILE_EXPORTSETTINGS, OnExport)
	ON_COMMAND(IDC_HISTORYNEXT, OnHistorynext)
	ON_COMMAND(IDC_HISTORYPREV, OnHistoryprev)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
END_MESSAGE_MAP()

extern unsigned long colormap[];
extern unsigned long defcolormap[];

bool g_ClosingApp=false;
CMainFrame* pMainFrame = NULL;
CMenu *pMainMenu = NULL;

/////////////////////////////////////////////////////////////////////////////
// Our CMudApp object!

CMudApp theApp;

CMudApp *GetApp() { 
	return &theApp; 
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
	CButton	m_btn2;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnURLBtn2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CenterWindow();	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_BUTTON2, m_btn2);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_BN_CLICKED(IDC_BUTTON2, OnURLBtn2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CAboutDlg::OnURLBtn2() 
{
	char url[512];

	m_btn2.GetWindowText(url, 512);

	ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWDEFAULT);
}

/////////////////////////////////////////////////////////////////////////////
// CMudApp construction

CMudApp::CMudApp()
{
	m_bHiColorIcons = FALSE;

	// application ID string below is unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("harmlesslion.FlipTerm.NoVersion"));

	char buff[512];
	GetCurrentDirectory(512,buff);
	m_sDir=buff;
	m_sLogFolder=m_sDir;

	memcpy(colormap, defcolormap, sizeof(unsigned long)*16);
}

CMudApp::~CMudApp() {
    if (NULL != pMainMenu) {
        delete pMainMenu;
    }
}

int CMudApp::ExitInstance() 
{
	DoSave();
	return CWinAppEx::ExitInstance();
}

void CMudApp::DoSave() 
{
	WriteProfileString("font","Facename",m_LogFont.lfFaceName);
	WriteProfileInt("Font","Size",m_LogFont.lfHeight);
	WriteProfileInt("Font", "Weight", m_LogFont.lfWeight);
	WriteProfileInt("Font", "Italic", m_LogFont.lfItalic);

	WriteProfileInt("colors","Background",m_colorBackGround);
	WriteProfileInt("colors","Foreground",m_colorForeGround);
	WriteProfileInt("colors","System",    m_colorSystem);
	WriteProfileInt("colors","Input",     m_colorInput);
	// save color palette
	for (int i=0; i<16; i++) {
		CString tmp;
		tmp.Format("%d", i);
		WriteProfileInt("colors", tmp, colormap[i]);
	}

	WriteProfileInt("Sounds","Bell",m_bBell);
	WriteProfileInt("Sounds","Trigger",m_bTriggerSound);
	WriteProfileInt("Sounds","ConnectBeep",m_bConnectBeep);

	WriteProfileInt("Wrap","Screen",m_bScreenWrap);
	WriteProfileInt("Wrap","Word",m_bWordWrap);
	WriteProfileInt("Wrap","Indent",m_bIndent);

	WriteProfileInt("Display","Ansi",m_bAnsi);
	WriteProfileInt("Display","LocalEcho",m_bEcho);
	WriteProfileInt("Display","Activity", m_bShowActivity);
	WriteProfileInt("Display","CLI", m_bUseCLI);
	WriteProfileInt("Display", "Multiline", m_bMultiline);
	WriteProfileInt("Display", "CommandStacking", m_bCommandStacking);
	WriteProfileInt("Display", "CommandChar", m_CommandChar);
	WriteProfileInt("Display", "ForceANSIReset", m_bANSIReset);
	WriteProfileInt("Display", "NumberInputLines", m_NumLines);

	WriteProfileInt("Misc", "ReviewEnd", m_bReviewEnd);
	WriteProfileInt("Misc", "NewWindowConnect", m_bNewWindow);
	WriteProfileInt("Misc", "MapKeys", m_bMapKeys);

	WriteProfileString("Log", "Directory", m_sLogFolder);
}

/////////////////////////////////////////////////////////////////////////////
// CMudApp initialization

BOOL CMudApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_STANDARD_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	EnableTaskbarInteraction();
	SetRegistryKey("TursiSoft");	// Save all settings in Registry (should be harmlesslion now, but that's okay)
	LoadStdProfileSettings(0);  // Load standard INI file options (including MRU)
	
	memset(&m_LogFont,0,sizeof(m_LogFont));
	strcpy(m_LogFont.lfFaceName,GetProfileString("font","facename","FixedSys"));
	m_LogFont.lfHeight=GetProfileInt("font","size",-12);
	m_LogFont.lfWeight=GetProfileInt("font", "weight", 0);
	m_LogFont.lfItalic=GetProfileInt("font", "italic", 0) ? true:false;

	// get background color
	m_colorBackGround=GetProfileInt("colors","Background",0)&0xf;

	// get foreground color
	m_colorForeGround=GetProfileInt("colors","Foreground",15)&0xf;

	// get system color
	m_colorSystem=GetProfileInt("colors", "System", 11)&0xf;

	// get input text color (default to foreground color)
	m_colorInput=GetProfileInt("colors", "Input", m_colorForeGround)&0xf;

	// get color palette
	for (int i=0; i<16; i++) {
		CString tmp;
		tmp.Format("%d", i);
		colormap[i]=GetProfileInt("colors", tmp, colormap[i]);
	}

	// Get Sounds Settings
	m_bBell=GetProfileInt("Sounds","Bell",1);
	m_bTriggerSound=GetProfileInt("Sounds","Trigger",1);
	m_bConnectBeep=GetProfileInt("Sounds","ConnectBeep",0);

	// get screen wrapping preferences
	m_bScreenWrap=GetProfileInt("Wrap","Screen",0);
	m_bWordWrap=GetProfileInt("Wrap","Word",1);
	m_bIndent=GetProfileInt("Wrap","Indent",0);
	m_bEcho=GetProfileInt("Display","LocalEcho",1);

	// other screen preferences
	m_bAnsi=GetProfileInt("Display","Ansi",1);
	m_bShowActivity=GetProfileInt("Display", "Activity", 1);
	m_bUseCLI=GetProfileInt("Display","CLI",1);
	m_bMultiline=GetProfileInt("Display", "Multiline", 1);
	m_bCommandStacking=GetProfileInt("Display", "CommandStacking", 0);
	m_CommandChar=(char)GetProfileInt("Display", "CommandChar", ';');
	m_bANSIReset=GetProfileInt("Display", "ForceANSIReset", 0);
	m_NumLines=GetProfileInt("Display", "NumberInputLines", 3);

	// Misc preferences
	m_bReviewEnd=GetProfileInt("Misc", "ReviewEnd", 1);
	m_bNewWindow=GetProfileInt("Misc", "NewWindowConnect", 1);
	m_bMapKeys=GetProfileInt("Misc", "MapKeys", 0);

	// Log preferences
	m_sLogFolder=GetProfileString("Log", "Directory", "");

	if (m_sLogFolder != "") {	// check existance
		if (_access(m_sLogFolder, 0)) {
			AfxMessageBox("Warning: Default log folder was not found!", MB_ICONEXCLAMATION | MB_OK);
			m_sLogFolder="";
		}
	}

	if (m_sLogFolder == "") {
		char myPath[MAX_PATH];

		if (S_OK == SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, myPath)) {
			int ret=AfxMessageBox("Would you like FlipTerm to create a directory in My Documents to store logs in?", MB_ICONQUESTION | MB_YESNO);
			if (IDYES == ret) {
				m_sLogFolder=myPath;
				m_sLogFolder+="\\FlipTerm Logs";
				if (_access(m_sLogFolder, 0)) {
					if (false == CreateDirectory(m_sLogFolder, NULL)) {
						AfxMessageBox("Unable to create the log directory. Please select a directory from the Options menu.", MB_ICONSTOP | MB_OK);
						m_sLogFolder=m_sDir;
					}
				}
			} else {
				AfxMessageBox("You can choose a default folder to store logs in from the Options menu.", MB_ICONINFORMATION | MB_OK);
				m_sLogFolder=m_sDir;
			}
		} else {
			// Can't find the documents folder, just default to current
			m_sLogFolder=m_sDir;
		}

		// As this is a convenient 'first run' place, check if log files are unassociated.
		// If they are, copy the association from txt files
		HKEY hKey, hKey2;
		char dataType[512], handler[512];
		DWORD dataTypeSize=sizeof(dataType);
		DWORD handlerSize=sizeof(handler);

		hKey=0;
		hKey2=0;
		while (1) {		// use breaks for an early out
			if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_CLASSES_ROOT, ".txt", 0, KEY_QUERY_VALUE, &hKey)) break;
			if (ERROR_SUCCESS != RegQueryValueEx(hKey, "", NULL, NULL, (unsigned char*)dataType, &dataTypeSize)) break;
			if (ERROR_SUCCESS != RegOpenKeyEx(hKey, "PersistentHandler", 0, KEY_QUERY_VALUE, &hKey2)) break;
			if (ERROR_SUCCESS != RegQueryValueEx(hKey2, "", NULL, NULL, (unsigned char*)handler, &handlerSize)) break;
			RegCloseKey(hKey2);
			RegCloseKey(hKey);
			hKey=0;
			hKey2=0;
			if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CLASSES_ROOT, ".log", 0, KEY_ALL_ACCESS, &hKey)) break;

			// Else, write the new settings to associate it to the same program as .txt
			if (ERROR_SUCCESS != RegCreateKeyEx(HKEY_CLASSES_ROOT, ".log", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL)) break;
			if (ERROR_SUCCESS != RegSetValueEx(hKey, "", 0, REG_SZ, (unsigned char*)dataType, strlen(dataType)+1)) break;
			if (ERROR_SUCCESS != RegCreateKeyEx(hKey, "PersistentHandler", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey2, NULL)) break;
			if (ERROR_SUCCESS != RegSetValueEx(hKey2, "", 0, REG_SZ, (unsigned char*)handler, strlen(handler)+1)) break;
			
			// Tell the system there was a change
			SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
			break;	// we don't really want to loop
		}
		if (hKey2) RegCloseKey(hKey2);
		if (hKey) RegCloseKey(hKey);
	}
	
	InitContextMenuManager();
    InitShellManager();
	InitKeyboardManager();
	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// Register the application's document templates.  Document templates
	// Serve as the connection between documents, frame windows and views.
	pDocTemplate = new CMultiDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CMudDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CMudView));
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	pMainFrame = new CMainFrame;
	if (!pMainFrame) return FALSE;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME)) {
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;
//    pMainMenu->Attach(pMainFrame->m_wndMenuBar.GetHMenu());

	CMudDoc *pDoc=NULL;
	// create a new (empty) document
	pDoc=(CMudDoc*)pDocTemplate->CreateNewDocument();
	CFrameWnd* pFrame = pDocTemplate->CreateNewFrame(pDoc, NULL);
	pDoc->OnNewDocument();
	pDocTemplate->InitialUpdateFrame(pFrame, pDoc, true);
	
	CMDIChildWnd * pChild=pMainFrame->MDIGetActive();
	if(pChild)
	{
		pDoc=(CMudDoc *)pChild->GetActiveDocument();
		pChild->MDIMaximize();
	}
	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	// URL history
	if (pMainMenu) {
		CMenu *pMenu=pMainMenu->GetSubMenu(URL_MENU_INDEX);		// fixed location (URLs)
		if (pMenu) {
			CString csTmp, csTmp2;

			// Read and add each URL string, till there are no more
			for (int idx=0; idx<10; idx++) {
				csTmp2.Format("URL%d", idx+1);
				csTmp=GetApp()->GetProfileString("URLs", csTmp2);
				if (csTmp=="") break;
				// There's a separator bar by default in the dropdown, used
				// to help the menu work when empty. If there are any valid items,
				// we remove it now to prevent problems later when we wrap
				if (idx == 0) {
					pMenu->RemoveMenu(0, MF_BYPOSITION);
				}
                // MF_STRING is 0, so it's optional to specify it...
				pMenu->AppendMenu(MF_ENABLED|MF_STRING, ID_URL_1+idx, csTmp);
			}
            pMainFrame->DrawMenuBar();
		}
	}

	CWorld *pWorld = NULL;
	bool firstWindow=true;
	// Enumerate through the loaded worlds and see which ones we need to connect
	for (int idx=0; idx < pDoc->m_aWorlds.GetSize(); idx++) {
		pWorld = (CWorld*)pDoc->m_aWorlds.GetAt(idx);

		if (pWorld->m_bAutoConnect) {
			// New Window
			if (!firstWindow) {
				// create a new document view
				pFrame = pDocTemplate->CreateNewFrame(pDoc, NULL);
				pDoc->OnNewDocument();
				pDocTemplate->InitialUpdateFrame(pFrame, pDoc, true);
				pMainFrame->GetWndConnected()->AddName(false, pWorld->m_sName);
			}
			firstWindow=false;
			
            if (pChild) {
                CMudView *pView=(CMudView*)pChild->GetActiveView();
    			pView->Connect(pWorld);
            }
		}
	}
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMudApp commands // CWinApp OnWindowNew

void CMudApp::OnWindowNewOpenDoc() 
{
	CMudDoc *pDoc;
	CFrameWnd* pFrame;
	CMDIChildWnd * pChild =	((CMDIFrameWndEx*)(m_pMainWnd))->MDIGetActive();

	if (pChild) {
		pDoc=(CMudDoc *)pChild->GetActiveDocument();
	} else {
		pDoc=(CMudDoc*)pDocTemplate->CreateNewDocument();
	}
	// create a new (empty) document
	pFrame = pDocTemplate->CreateNewFrame(pDoc, NULL);
	pDoc->OnNewDocument();
	pDocTemplate->InitialUpdateFrame(pFrame, pDoc, true);

	// Pop up connection dialog if desired
	if (m_bNewWindow) {
		PostMessage(m_pMainWnd->GetSafeHwnd(), WM_COMMAND, ID_FILE_CONNECT, 0);
	}
}

void CMudApp::OnFileEditworldlist() 
{
	CMudDoc *pDoc=0;
	int deletedoc=FALSE;
	if(!GetOpenDocumentCount())
	{
		pDoc = new CMudDoc();
		if(!pDoc->OnNewDocument())
		{
			delete pDoc;
			pDoc=0;
		}
		else {
			deletedoc=TRUE;
		}
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
	}
	else
		AfxMessageBox("Error opening World List, Cannot edit.");
	if(deletedoc)
		delete pDoc;
}

void CMudApp::OnViewFont() 
{
	CFontDialog dlg(&m_LogFont,CF_FIXEDPITCHONLY|CF_FORCEFONTEXIST|CF_INITTOLOGFONTSTRUCT|CF_SCREENFONTS);
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

void CMudApp::OnSystemColor()
{
	CTextClrDlg dlg;
	dlg.m_colorBackGround=m_colorInput;
	dlg.m_colorForeGround=m_colorSystem;
	dlg.m_bAnsi = m_bAnsi;
	dlg.m_bIsSystem=false;
	if(dlg.DoModal()==IDOK)
	{
		m_colorSystem=dlg.m_colorForeGround;
		m_colorInput=dlg.m_colorBackGround;
	}
}

void CMudApp::OnDefaultColor()
{
	CColorSet Dlg;

	if (IDOK == Dlg.DoModal()) {
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

void CMudApp::OnUpdateAliasList(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(false);
}

void CMudApp::OnQuit()
{
	GetMainWnd()->SendMessage(WM_CLOSE);
}

void CMudApp::OnUpdateExit(CCmdUI * pCmdUI)
{
	pCmdUI->Enable(true);
}

void CMudApp::OnOptionsSwitches()
{
	CSwitches dlg;

	dlg.DoModal();
}

// App command to run the dialog
void CMudApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

void CMudApp::OnURL1()
{
	CString csTmp;

	if (pMainMenu) {
		CMenu *pMenu=pMainMenu->GetSubMenu(URL_MENU_INDEX);		// fixed location (URLs)
		if (pMenu) {
			pMenu->GetMenuString(ID_URL_1, csTmp, MF_BYCOMMAND);
			ShellExecute(NULL, "open", csTmp, NULL, NULL, SW_SHOWNORMAL);
		}
	}

}
void CMudApp::OnURL2()
{
	CString csTmp;

	if (pMainMenu) {
		CMenu *pMenu=pMainMenu->GetSubMenu(URL_MENU_INDEX);		// fixed location (URLs)
		if (pMenu) {
			pMenu->GetMenuString(ID_URL_2, csTmp, MF_BYCOMMAND);
			ShellExecute(NULL, "open", csTmp, NULL, NULL, SW_SHOWNORMAL);
		}
	}

}
void CMudApp::OnURL3()
{
	CString csTmp;

	if (pMainMenu) {
		CMenu *pMenu=pMainMenu->GetSubMenu(URL_MENU_INDEX);		// fixed location (URLs)
		if (pMenu) {
			pMenu->GetMenuString(ID_URL_3, csTmp, MF_BYCOMMAND);
			ShellExecute(NULL, "open", csTmp, NULL, NULL, SW_SHOWNORMAL);
		}
	}

}
void CMudApp::OnURL4()
{
	CString csTmp;

	if (pMainMenu) {
		CMenu *pMenu=pMainMenu->GetSubMenu(URL_MENU_INDEX);		// fixed location (URLs)
		if (pMenu) {
			pMenu->GetMenuString(ID_URL_4, csTmp, MF_BYCOMMAND);
			ShellExecute(NULL, "open", csTmp, NULL, NULL, SW_SHOWNORMAL);
		}
	}

}
void CMudApp::OnURL5()
{
	CString csTmp;

	if (pMainMenu) {
		CMenu *pMenu=pMainMenu->GetSubMenu(URL_MENU_INDEX);		// fixed location (URLs)
		if (pMenu) {
			pMenu->GetMenuString(ID_URL_5, csTmp, MF_BYCOMMAND);
			ShellExecute(NULL, "open", csTmp, NULL, NULL, SW_SHOWNORMAL);
		}
	}

}
void CMudApp::OnURL6()
{
	CString csTmp;

	if (pMainMenu) {
		CMenu *pMenu=pMainMenu->GetSubMenu(URL_MENU_INDEX);		// fixed location (URLs)
		if (pMenu) {
			pMenu->GetMenuString(ID_URL_6, csTmp, MF_BYCOMMAND);
			ShellExecute(NULL, "open", csTmp, NULL, NULL, SW_SHOWNORMAL);
		}
	}

}
void CMudApp::OnURL7()
{
	CString csTmp;

	if (pMainMenu) {
		CMenu *pMenu=pMainMenu->GetSubMenu(URL_MENU_INDEX);		// fixed location (URLs)
		if (pMenu) {
			pMenu->GetMenuString(ID_URL_7, csTmp, MF_BYCOMMAND);
			ShellExecute(NULL, "open", csTmp, NULL, NULL, SW_SHOWNORMAL);
		}
	}

}
void CMudApp::OnURL8()
{
	CString csTmp;

	if (pMainMenu) {
		CMenu *pMenu=pMainMenu->GetSubMenu(URL_MENU_INDEX);		// fixed location (URLs)
		if (pMenu) {
			pMenu->GetMenuString(ID_URL_8, csTmp, MF_BYCOMMAND);
			ShellExecute(NULL, "open", csTmp, NULL, NULL, SW_SHOWNORMAL);
		}
	}

}
void CMudApp::OnURL9()
{
	CString csTmp;

	if (pMainMenu) {
		CMenu *pMenu=pMainMenu->GetSubMenu(URL_MENU_INDEX);		// fixed location (URLs)
		if (pMenu) {
			pMenu->GetMenuString(ID_URL_9, csTmp, MF_BYCOMMAND);
			ShellExecute(NULL, "open", csTmp, NULL, NULL, SW_SHOWNORMAL);
		}
	}

}
void CMudApp::OnURL10()
{
	CString csTmp;

	if (pMainMenu) {
		CMenu *pMenu=pMainMenu->GetSubMenu(URL_MENU_INDEX);		// fixed location (URLs)
		if (pMenu) {
			pMenu->GetMenuString(ID_URL_10, csTmp, MF_BYCOMMAND);
			ShellExecute(NULL, "open", csTmp, NULL, NULL, SW_SHOWNORMAL);
		}
	}

}

void CMudApp::OnSave()
{
	DoSave();		// Save settings

	// We need to find a document to save the worlds, triggers, etc
	CMDIFrameWndEx *pWnd=(CMDIFrameWndEx*)GetMainWnd()->GetActiveWindow();
	if (pWnd) {
		CMDIChildWnd * pChild= pWnd->MDIGetActive();
		CMudView *pView;
		if(pChild) {
			pView=(CMudView *)pChild->GetActiveView();
			if(pView) {
				CMudDoc *pDoc = pView->GetDocument();
				if (pDoc) {
					pDoc->Save();
					return;
				}
			}
		}
	}

	AfxMessageBox("Unable to save settings at this time.");
}

void CMudApp::OnExport()
{
	// first, make sure they are saved
	OnSave();

	// next, get a path
	CFileDialog dlg(false, "reg", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, "Registry Files|*.reg||", m_pMainWnd);

	if (IDOK == dlg.DoModal()) {
		CString csTmp;
		// Shell out to regedit to dump the code
		csTmp.Format("/e %s \"HKEY_CURRENT_USER\\SOFTWARE\\TURSISOFT\\FLIPTERM\"", dlg.GetFileName().GetString());
		if ((int)ShellExecute(NULL, "open", "regedit", csTmp, NULL, SW_SHOWMINIMIZED) > 32) {
			AfxMessageBox("Configuration export successful");
		}
	}
}

// Change the default log folder
void CMudApp::OnOptionsLog()
{
	CString fn;

	fn.Format("%s\\Select Folder.log", m_sLogFolder.GetString());
	CFileDialog dlg(false, "log", fn, OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, "Log Files|*.log;*.txt||", GetMainWnd()->GetActiveWindow());

	dlg.m_ofn.lpstrTitle="Select default folder for log files";
	if (IDOK == dlg.DoModal()) {
		fn=dlg.GetPathName().Left(dlg.GetPathName().GetLength() - dlg.GetFileName().GetLength() - 1);
		
		if (fn != m_sLogFolder) {
			if (IDYES == AfxMessageBox("Would you like to update the paths for your autologs? (if any)\nChanges will take effect the next time the log is opened.", MB_ICONQUESTION | MB_YESNO)) {
				CMDIFrameWndEx *pWnd=(CMDIFrameWndEx*)GetMainWnd()->GetActiveWindow();
				if (pWnd) {
					CMDIChildWnd * pChild= pWnd->MDIGetActive();
					CMudView *pView;
					if(pChild) {
						pView=(CMudView *)pChild->GetActiveView();
						if(pView) {
							CMudDoc *pDoc = pView->GetDocument();
							if (pDoc) {
								// Enumerate the worlds
								CWorld *pWorld = NULL;
								for (int idx=0; idx < pDoc->m_aWorlds.GetSize(); idx++) {
									pWorld = (CWorld*)pDoc->m_aWorlds.GetAt(idx);
									if (pWorld) {
										char buf[_MAX_FNAME], ext[_MAX_EXT];
										// check all log entries
										if (pWorld->m_sAutologFile.GetLength()) {
											_splitpath(pWorld->m_sAutologFile, NULL, NULL, buf, ext);
											pWorld->m_sAutologFile.Format("%s\\%s%s", fn.GetString(), buf, ext);
										}
/* is this scary indentation */			if (pWorld->m_sLastAutologFilename.GetLength()) {
/* or what? */								_splitpath(pWorld->m_sLastAutologFilename, NULL, NULL, buf, ext);
											pWorld->m_sLastAutologFilename.Format("%s\\%s%s", fn.GetString(), buf, ext);
										}
										if (pWorld->m_nLastDayLogOpened) {
											pWorld->m_nLastDayLogOpened=0;	// reset the daily log
										}
									}
								}
							}
						}
					}
				}
			}
			m_sLogFolder = fn;
		}
	}
}

void CMudApp::OnHistorynext() 
{
	CMDIChildWnd * pChild=((CMainFrame*)m_pMainWnd)->MDIGetActive();
	if (!pChild) return;
	CMudView *pView=(CMudView*)pChild->GetActiveView();
	if (!pView) return;
	pView->HistoryNext();
}

void CMudApp::OnHistoryprev() 
{
	CMDIChildWnd * pChild=((CMainFrame*)m_pMainWnd)->MDIGetActive();
	if (!pChild) return;
	CMudView *pView=(CMudView*)pChild->GetActiveView();
	if (!pView) return;
	pView->HistoryPrev();
}

void CMudApp::PreLoadState()
{
}

void CMudApp::LoadCustomState()
{
}

void CMudApp::SaveCustomState()
{
}

