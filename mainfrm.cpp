/////////////////////////////////////////////////////////////////////////////
//
// mainfrm.cpp : implementation of the CMainFrame class
//
// CMainFrame
//
#include "stdafx.h"
#include "gmud32.h"
#include "mainfrm.h"
#include "outwnd.h"
#include "gmud32vw.h"
#include "gmud32doc.h"
#include "buttoned.h"
#include "macroarray.h"
#include "macro.h"
#include "world.h"
#include "aliasarray.h"
#include "disconnect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define DOCKINGMODE DT_SMART
//#define DOCKINGMODE DT_STANDARD
//#define DOCKINGMODE DT_IMMEDIATE

extern CMenu *pMainMenu;

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_MACROBTN_EDIT, OnButtonEdit)
	ON_BN_CLICKED(IDC_MACROBTN10,OnButton10)
	ON_BN_CLICKED(IDC_MACROBTN2, OnButton2)
	ON_BN_CLICKED(IDC_MACROBTN3, OnButton3)
	ON_BN_CLICKED(IDC_MACROBTN4, OnButton4)
	ON_BN_CLICKED(IDC_MACROBTN5, OnButton5)
	ON_BN_CLICKED(IDC_MACROBTN6, OnButton6)
	ON_BN_CLICKED(IDC_MACROBTN7, OnButton7)
	ON_BN_CLICKED(IDC_MACROBTN8, OnButton8)
	ON_BN_CLICKED(IDC_MACROBTN9, OnButton9)
	ON_MESSAGE(WM_SELECT_VIEW,OnViewSelected)
	ON_MESSAGE(WM_UPDATE_WORLDS,OnUpdateWorlds)
	ON_COMMAND(ID_EDIT_ALIASLIST, OnEditAliaslist)
	ON_BN_CLICKED(IDC_MACROBTN1, OnButton1)
	ON_WM_TIMER()
    ON_COMMAND(ID_WINDOW_MANAGER, &CMainFrame::OnWindowManager)
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MACROWINDOW,&CMainFrame::OnUpdateControlBarMenu)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WINDOWBAR,&CMainFrame::OnUpdateControlBarMenu)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WORLDWINDOW,&CMainFrame::OnUpdateControlBarMenu)
    ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLBAR_MENU, &CMainFrame::OnUpdateControlBarMenu)
    ON_COMMAND(ID_VIEW_WINDOWBAR, &CMainFrame::OnViewWindowbar)
    ON_COMMAND(ID_VIEW_MACROWINDOW, &CMainFrame::OnViewMacrowindow)
    ON_COMMAND(ID_VIEW_WORLDWINDOW, &CMainFrame::OnViewWorldwindow)
    ON_COMMAND(ID_VIEW_TOOLBAR_MENU, &CMainFrame::OnViewToolbarMenu)
    ON_WM_INITMENUPOPUP()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// arrays of IDs used to initialize control bars
	
// toolbar buttons - IDs are command buttons
static UINT BASED_CODE buttons[] =
{
	// same order as in the bitmap 'toolbar.bmp'
	ID_FILE_CONNECT,
	ID_FILE_DISCONNECT,
		ID_SEPARATOR,
	ID_WINDOW_PAUSE,
		ID_SEPARATOR,
	ID_WINDOW_NEW_OPEN_DOC,
		ID_SEPARATOR,
	ID_EDIT_WORLDLIST,
		ID_SEPARATOR,
	ID_OPTIONS_WRAP,
	ID_OPTIONS_FONT,
	ID_OPTIONS_COLOR,
		ID_SEPARATOR,
	ID_OPTIONS_SWITCHES,
	ID_OPTIONS_SYSTEMMESSAGECOLOR,
	ID_OPTIONS_DEFAULTCOLORS,
		ID_SEPARATOR,
	ID_EDIT_ALIASLIST,
	ID_EDIT_TRIGGERLIST,
		ID_SEPARATOR,
	ID_OPTIONS_SOUND,	
	ID_FILE_TOGGLELOGGING,
		ID_SEPARATOR,
	ID_VIEW_STATUS_BAR,
	ID_VIEW_MACROWINDOW,
	ID_VIEW_WORLDWINDOW,
		ID_SEPARATOR,
	ID_VIEW_BROWSELOG,
		ID_SEPARATOR,
	ID_HELP_ABOUT,
};

static UINT BASED_CODE numbers[] =
{
	IDC_WINDOW1,
	IDC_WINDOW2,
	IDC_WINDOW3,
	IDC_WINDOW4,
	IDC_WINDOW5,
	IDC_WINDOW6,
	IDC_WINDOW7,
	IDC_WINDOW8,
	IDC_WINDOW9,
};

static UINT BASED_CODE indicators[] =
{
	ID_SEPARATOR,			// Status indicator - 0 (MFC)
	ID_SEPARATOR,			// Number of lines  - 1
	ID_SEPARATOR,           // Connected timer  - 2
	ID_SEPARATOR,			// 'More' indicator - 3
    ID_SEPARATOR,           // 'SSL' indicator  - 4
	ID_INDICATOR_CAPS,      // Caps lock        - 5 (MFC)
	ID_INDICATOR_NUM,       // Num lock         - 6 (MFC)
	ID_INDICATOR_SCRL,      // Scroll lock      - 7 (MFC)
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_bMacrosVisible=FALSE;	
	m_bConnectedVisible=FALSE;
	nTimer=0;
}

CMainFrame::~CMainFrame()
{
	// Can't kill timers or otherwise touch the window here!
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;
	
#if 0
    // I'm not sold on tabs, and they seem to kill the ability to resize windows?
    // BUT, this is all it takes to make it work.
	CMDITabInfo mdiTabParams;
	mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D_ONENOTE; // other styles available...
	mdiTabParams.m_bActiveTabCloseButton = TRUE;      // set to FALSE to place close button at right of tab area
	mdiTabParams.m_bTabIcons = FALSE;    // set to TRUE to enable document icons on MDI taba
	mdiTabParams.m_bAutoColor = TRUE;    // set to FALSE to disable auto-coloring of MDI tabs
	mdiTabParams.m_bDocumentMenu = TRUE; // enable the document menu at the right edge of the tab area
	EnableMDITabbedGroups(TRUE, mdiTabParams);
#endif

    pMainMenu = new CMenu();
    pMainMenu->LoadMenu(IDR_MAINFRAME);
    SetMenu(NULL);
    ::DestroyMenu(m_hMenuDefault);
    SetMenu(pMainMenu);
    m_hMenuDefault = pMainMenu->GetSafeHmenu();

	if (!m_wndNumbers.Create(this, WS_CHILD | WS_VISIBLE | CBRS_TOP, ID_VIEW_WINDOWBAR) ||
		!m_wndNumbers.LoadBitmap(IDB_NUMBERS) ||
		!m_wndNumbers.SetButtons(numbers,
		  sizeof(numbers)/sizeof(UINT)))
	{
		TRACE0("Failed to create number toolbar\n");
		return -1;      // fail to create
	}

    if (!m_wndToolBar.Create(this, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER, ID_VIEW_TOOLBAR) ||
		!m_wndToolBar.LoadBitmap(IDR_MAINFRAME) ||
		!m_wndToolBar.SetButtons(buttons,
		  sizeof(buttons)/sizeof(UINT)))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	// The IDs used are meaningless in this case, except for #0 (since MFC updates that)
    {
        UINT Id, Style;
        int Width;
        m_wndStatusBar.GetPaneInfo(0, Id, Style, Width);
	    m_wndStatusBar.SetPaneInfo(0, Id, SBPS_STRETCH, 200);
    }
    m_wndStatusBar.SetPaneInfo(1, IDOK, SBPS_NORMAL, 100);      // lines
	m_wndStatusBar.SetPaneInfo(2, IDCANCEL, SBPS_NORMAL, 70);   // connect timer
	m_wndStatusBar.SetPaneInfo(3, IDRETRY, SBPS_NORMAL, 80);    // MORE prompt
	m_wndStatusBar.SetPaneInfo(4, IDIGNORE, SBPS_NORMAL, 40);   // SSL indicator
	nTimer=SetTimer(1, 1000, NULL);
	
    DWORD myDockStyle = AFX_CBRS_FLOAT | AFX_CBRS_CLOSE;     // no tabs, no auto-hide, no resize, no rollup
	if(!m_wndMacroBar.Create("Macros", this, TRUE, MAKEINTRESOURCE(IDD_MACRO_BAR), CBRS_LEFT, ID_VIEW_MACROWINDOW, AFX_CBRS_REGULAR_TABS, myDockStyle))
	{
		TRACE("Failed to create macrobar\n");
		return -1;
	}
	m_bMacrosVisible=TRUE;

	if(!m_wndConnected.Create("Connected", this, TRUE, MAKEINTRESOURCE(IDD_CONNECTED_WORLD_BAR), CBRS_LEFT, ID_VIEW_WORLDWINDOW, AFX_CBRS_REGULAR_TABS, myDockStyle))
	{
		TRACE("Failed to create world window\n");
		return -1;
	}
	m_bConnectedVisible=TRUE;

    // enable docking on the window anywhere
	EnableDocking(CBRS_ALIGN_ANY);
	// enable docking window behavior
	CDockingManager::SetDockingMode(DOCKINGMODE);

    // Set up the docking - force some defaults then allow any
//  m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);  // if it's not dockable, it acts like a normal menubar...
//	DockPane(&m_wndMenuBar);    // if it's not dockable, it acts like a normal menubar...

    m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndMacroBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndNumbers.EnableDocking(CBRS_ALIGN_LEFT);
	m_wndConnected.EnableDocking(CBRS_ALIGN_LEFT);

    DockPane(&m_wndNumbers);
	m_dockManager.DockPaneLeftOf(&m_wndToolBar, &m_wndNumbers);
    DockPane(&m_wndConnected);
    DockPane(&m_wndMacroBar);
	
    m_wndNumbers.EnableDocking(CBRS_ALIGN_ANY);
	m_wndConnected.EnableDocking(CBRS_ALIGN_ANY);

    // enable quick (Alt+drag) toolbar customization
	CMFCToolBar::EnableQuickCustomization();

	LoadBarState(AfxGetAppName());

    UpdateButtons(); // show macro buttons

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWndEx::PreCreateWindow(cs) )
		return FALSE;

	// Modify the Window class or styles here by modifying the CREATESTRUCT cs
	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		 | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU | WS_SIZEBOX;

	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWndEx::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnWindowManager()
{
	ShowWindowsDialog();
}

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* scan menus */);
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CMDIFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext)
{
	// base class does the real work

	if (!CMDIFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CMainFrame::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	dwStyle &= ( ~ FWS_ADDTOTITLE);	 // clear out FWS_ADDTOTITLE

	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

void CMainFrame::OnClose() 
{
	// confirmation
	if (GetApp()->GetProfileInt("Helpers", "ConfirmClose", 1)) {
		CDisconnect dlg;
		
		dlg.m_Text = "Are you sure you want to exit FlipTerm?";
		int ret = dlg.DoModal();
		if (dlg.m_Check) {
			GetApp()->WriteProfileInt("Helpers", "ConfirmClose", 0);
		}

		if (IDCANCEL == ret) {
			return;
		}
	}

	g_ClosingApp=true;	// blocks later questions

	if (nTimer) KillTimer(nTimer);
	SaveBarState(AfxGetAppName());
	
	// Write out the URL history
    {
		CString csTmp, csTmp2;

		// save each URL string
		for (int idx=0; idx<10; idx++) {
			csTmp2.Format("URL%d", idx+1);
			GetApp()->WriteProfileString("URLs", csTmp2, menuItems[idx]);
		}
	}

	CMDIFrameWndEx::OnClose();
}

void CMainFrame::OnButtonEdit() 
{
	CMDIChildWnd * pChild= MDIGetActive();
	CMudView *pView=0;
	
	if(pChild) {
		pView=(CMudView *)pChild->GetActiveView();
	} else {
		return;
	}

	CMacroArray* pArray=0;
	if(pView)
	{
		ASSERT_VALID(pView);
		CWorld *pWorld = pView->m_pWorld;
		if(pWorld)
		{
			ASSERT(pWorld);
			CMudDoc *pDoc = pView->GetDocument();
			ASSERT_VALID(pDoc);
			pArray = (CMacroArray *)(pDoc->m_aMacros.Get(pWorld->m_sMacroList));
			ASSERT_VALID(pArray);
			if(!pArray) {
				AfxMessageBox("Could not load macro set!", MB_ICONSTOP|MB_OK);
				return;
			}
		} else {
			AfxMessageBox("To edit Macro sets offline - Select Edit->Worlds, select any world, and click 'Change Macro Set'. This will allow you to modify existing sets and create new ones as needed.", MB_ICONINFORMATION|MB_OK);
			return;
		}
	} else {
		AfxMessageBox("To edit Macro sets offline - Select Edit->Worlds, select any world, and click 'Change Macro Set'. This will allow you to modify existing sets and create new ones as needed.", MB_ICONINFORMATION|MB_OK);
		return;
	}

	CButtonEdit dlg(this);
	dlg.SetList(pArray);
	if(dlg.DoModal()==IDOK)
		UpdateButtons();	
}

int CMainFrame::UpdateButtons()
{
	if(!SetButtons())
		ClearButtons();
	return TRUE;
}

int CMainFrame::ClearButtons()
{
	m_wndMacroBar.GetDlgItem(IDC_MACROBTN1)->SetWindowText("");
	m_wndMacroBar.GetDlgItem(IDC_MACROBTN2)->SetWindowText("");
	m_wndMacroBar.GetDlgItem(IDC_MACROBTN3)->SetWindowText("");
	m_wndMacroBar.GetDlgItem(IDC_MACROBTN4)->SetWindowText("");
	m_wndMacroBar.GetDlgItem(IDC_MACROBTN5)->SetWindowText("");
	m_wndMacroBar.GetDlgItem(IDC_MACROBTN6)->SetWindowText("");
	m_wndMacroBar.GetDlgItem(IDC_MACROBTN7)->SetWindowText("");
	m_wndMacroBar.GetDlgItem(IDC_MACROBTN8)->SetWindowText("");
	m_wndMacroBar.GetDlgItem(IDC_MACROBTN9)->SetWindowText("");
	m_wndMacroBar.GetDlgItem(IDC_MACROBTN10)->SetWindowText("");
	return TRUE;
}


int CMainFrame::SetButtons()
{
	CWnd *pButton;
	CString str;

	CMDIChildWnd * pChild= MDIGetActive();
	CMudView *pView=0;
	if(pChild)
		pView=(CMudView *)pChild->GetActiveView();
	else
		return FALSE;
	CMacroArray* pArray=0;
	if(pView)
	{
		ASSERT_VALID(pView);
		CWorld *pWorld = pView->m_pWorld;
		if(pWorld)
		{
			ASSERT(pWorld);
			CMudDoc *pDoc = pView->GetDocument();
			ASSERT_VALID(pDoc);
			pArray = (CMacroArray *)(pDoc->m_aMacros.Get(pWorld->m_sMacroList));
			ASSERT_VALID(pArray);
			if(!pArray)
				return FALSE;
		}
		else
			return FALSE;
	}
	else
		return FALSE;

	pButton=m_wndMacroBar.GetDlgItem(IDC_MACROBTN1);
	ASSERT_VALID(pButton);
	CMacro *pMacro = (CMacro *)pArray->Get("F1");
	ASSERT_VALID(pMacro);
	pButton->SetWindowText(pMacro->m_sText.Left(12));
	pButton=m_wndMacroBar.GetDlgItem(IDC_MACROBTN2);
	pButton->SetWindowText(((CMacro *)(pArray->Get("F2")))->m_sText.Left(12));
	pButton=m_wndMacroBar.GetDlgItem(IDC_MACROBTN3);
	pButton->SetWindowText(((CMacro *)(pArray->Get("F3")))->m_sText.Left(12));
	pButton=m_wndMacroBar.GetDlgItem(IDC_MACROBTN4);
	pButton->SetWindowText(((CMacro *)(pArray->Get("F4")))->m_sText.Left(12));
	pButton=m_wndMacroBar.GetDlgItem(IDC_MACROBTN5);
	pButton->SetWindowText(((CMacro *)(pArray->Get("F5")))->m_sText.Left(12));
	pButton=m_wndMacroBar.GetDlgItem(IDC_MACROBTN6);
	pButton->SetWindowText(((CMacro *)(pArray->Get("F6")))->m_sText.Left(12));
	pButton=m_wndMacroBar.GetDlgItem(IDC_MACROBTN7);
	pButton->SetWindowText(((CMacro *)(pArray->Get("F7")))->m_sText.Left(12));
	pButton=m_wndMacroBar.GetDlgItem(IDC_MACROBTN8);
	pButton->SetWindowText(((CMacro *)(pArray->Get("F8")))->m_sText.Left(12));
	pButton=m_wndMacroBar.GetDlgItem(IDC_MACROBTN9);
	pButton->SetWindowText(((CMacro *)(pArray->Get("F9")))->m_sText.Left(12));
	pButton=m_wndMacroBar.GetDlgItem(IDC_MACROBTN10);
	pButton->SetWindowText(((CMacro *)(pArray->Get("F10")))->m_sText.Left(12));
 	return TRUE;
}

int CMainFrame::OutputMacro(CString str)
{

	CMDIChildWnd * pChild= MDIGetActive();
	CMudView *pView=0;
	if(pChild)
		pView=(CMudView *)pChild->GetActiveView();
	if(pView)
	{
		ASSERT_VALID(pView);
		CWorld *pWorld = pView->m_pWorld;
		if(pWorld)
		{
			ASSERT(pWorld);
			CMudDoc *pDoc = pView->GetDocument();
			ASSERT_VALID(pDoc);
			CMacroArray *pArray = (CMacroArray *)pDoc->m_aMacros.Get(pWorld->m_sMacroList);
			ASSERT_VALID(pArray);
      CMacro *pMacro=(CMacro *)pArray->Get(str);
			ASSERT_VALID(pMacro);
      
			pMacro->OnActivate(*pView,*pWorld);
			pView->SetFocus();
		}
	}
	return TRUE;
}

void CMainFrame::OnButton1() 
{
	OutputMacro("F1");
}

void CMainFrame::OnButton2() 
{
	OutputMacro("F2");
}

void CMainFrame::OnButton3() 
{
	OutputMacro("F3");
}

void CMainFrame::OnButton4() 
{
	OutputMacro("F4");
}

void CMainFrame::OnButton5() 
{
	OutputMacro("F5");
}

void CMainFrame::OnButton6() 
{
	OutputMacro("F6");
}

void CMainFrame::OnButton7() 
{
	OutputMacro("F7");
}

void CMainFrame::OnButton8() 
{
	OutputMacro("F8");
}

void CMainFrame::OnButton9() 
{
	OutputMacro("F9");
}

void CMainFrame::OnButton10() 
{
	OutputMacro("F10");
}

void CMainFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	if ((GetStyle() & FWS_ADDTOTITLE) == 0)
		return;     // leave it alone!

	CMDIChildWnd* pActiveChild;
	CDocument* pDocument = GetActiveDocument();
	CMudView* pView = (CMudView*)GetActiveView();
	if (bAddToTitle && pView &&
	  (pActiveChild = MDIGetActive()) != NULL &&
	  (pActiveChild->GetStyle() & WS_MAXIMIZE) == 0 &&
	  (pDocument != NULL || (pDocument = pActiveChild->GetActiveDocument()) != NULL))
		UpdateFrameTitleForDocument(pView->GetTitle());
	else
		UpdateFrameTitleForDocument(NULL);

}

int CMainFrame::UpdateWorlds()
{
	int wnd;

	m_wndConnected.EmptyList();
	CMudView* pView = 0;
	CMudDoc* pDocument =NULL;
	CMDIChildWnd * pChild =	((CMDIFrameWndEx*)(GetApp()->m_pMainWnd))->MDIGetActive();
	if(pChild )
		pDocument = (CMudDoc *)pChild->GetActiveDocument();
	if(pDocument)
	{
		POSITION pos = pDocument->GetFirstViewPosition();
		wnd=0;
		while (pos != NULL)
		{
			pView = (CMudView*)pDocument->GetNextView(pos);
			ASSERT_VALID(pView);
			m_wndConnected.AddName(pView->m_bUnSeen,pView->GetTitle());
			if (wnd<9) {
				m_wndNumbers.SetButtonInfo(wnd, IDC_WINDOW1+wnd, TBBS_BUTTON, wnd+(pView->m_bUnSeen?10:0));
				wnd++;
			}
		}
		// Turn off the unused ones
		while (wnd<9) {
			m_wndNumbers.SetButtonInfo(wnd, IDC_WINDOW1+wnd, TBBS_BUTTON, wnd);
			wnd++;
		}
	}
	UpdateButtons();
	return TRUE;
}

afx_msg LONG CMainFrame::OnViewSelected(UINT,LONG /*lParam*/)
{
	UpdateWorlds();
	return TRUE;
}

afx_msg LONG CMainFrame::OnUpdateWorlds(UINT,LONG)
{
	UpdateWorlds();
	return TRUE;
}

void CMainFrame::OnEditAliaslist() 
{
	CMDIChildWnd * pChild= MDIGetActive();
	CMudView *pView=0;
	if(pChild) {
		pView=(CMudView *)pChild->GetActiveView();
	} else {
		AfxMessageBox("To edit Alias sets offline - Select Edit->Worlds, select any world, and click 'Change Alias Set'. This will allow you to modify existing sets and create new ones as needed.", MB_ICONINFORMATION|MB_OK);
		return;
	}

	if(pView && pView->m_pWorld)
	{
		CMudDoc *pDoc = pView->GetDocument();
		pDoc->EditAliasList(pView->m_pWorld); 
	} else {
		AfxMessageBox("To edit Alias sets offline - Select Edit->Worlds, select any world, and click 'Change Alias Set'. This will allow you to modify existing sets and create new ones as needed.", MB_ICONINFORMATION|MB_OK);
		return;
	}
}

void CMainFrame::OnTimer(UINT nIDEvent) 
{
	int nTime;
	CString tmp;
	CMudView *pView=NULL;
	CMDIChildWnd *pChild = MDIGetActive();

	if (nIDEvent == 1) {
		if(pChild) {
			pView=(CMudView *)pChild->GetActiveView();
			if (pView) {
				if (pView->m_pOutWnd) {
					tmp.Format("%d lines", pView->m_pOutWnd->m_Lines.GetCount());
					m_wndStatusBar.SetPaneText(1, tmp, true);
					if (pView->m_pOutWnd->GetOffset()) {
						m_wndStatusBar.SetPaneText(3, ">MORE<", true);
					} else {
						m_wndStatusBar.SetPaneText(3, " ", true);
					}
				}
				
				if (pView->m_pWorld) {
					nTime=(int)time(NULL) - pView->m_Time;
					tmp.Format("%d:%02d:%02d", nTime/3600, (nTime%3600)/60, nTime%60);
					m_wndStatusBar.SetPaneText(2, tmp, true);

                    // we redo the SSL status too in case the world changed
                    if (pView->m_pWorld->m_useSSL) {
                        m_wndStatusBar.SetPaneText(4, "SSL", true);
                    } else {
                        m_wndStatusBar.SetPaneText(4, "", true);
                    }
				}


			}
		}
	}

	CMDIFrameWndEx::OnTimer(nIDEvent);
}

// The View->XXX toggles

// The menu checkbox handler is shared, and we don't do status as it's automatic
void CMainFrame::OnUpdateControlBarMenu(CCmdUI* pCmdUI)
{
    // display the check mark in the menu if needed
    switch (pCmdUI->m_nID) {
        case ID_VIEW_MACROWINDOW:
            pCmdUI->SetCheck(m_wndMacroBar.IsWindowVisible());
            break;

        case ID_VIEW_WINDOWBAR:
            pCmdUI->SetCheck(m_wndNumbers.IsWindowVisible());
            break;

        case ID_VIEW_WORLDWINDOW:
            pCmdUI->SetCheck(m_wndConnected.IsWindowVisible());
            break;

        case ID_VIEW_TOOLBAR_MENU:
            pCmdUI->SetCheck(m_wndToolBar.IsWindowVisible());
            break;

        default:
            OutputDebugString("Got unknown view command");
    }
}

// The actual window toggles are distinct
void CMainFrame::OnViewToolbarMenu() {
    m_wndToolBar.ShowPane(!m_wndToolBar.IsWindowVisible(), FALSE, FALSE);
}
void CMainFrame::OnViewWindowbar() {
    m_wndNumbers.ShowPane(!m_wndNumbers.IsWindowVisible(), FALSE, FALSE);
}
void CMainFrame::OnViewMacrowindow() {
    m_wndMacroBar.ShowPane(!m_wndMacroBar.IsWindowVisible(), FALSE, FALSE);
}
void CMainFrame::OnViewWorldwindow() {
    m_wndConnected.ShowPane(!m_wndConnected.IsWindowVisible(), FALSE, FALSE);
}

// populates the URL menu during popup
afx_msg void CMainFrame::OnInitMenuPopup(CMenu *pMenu, UINT menuIdx, BOOL bSysMenu) {
    if (!bSysMenu) {
        // see whether the index is for the URLs menu
        CMenu *mainMenu = GetMenu();
        CString cs;
        if ((mainMenu->GetMenuString(menuIdx, cs, MF_BYPOSITION)) && (cs == "&URLs")) {
            for (int idx=0; idx<10; ++idx) {
                if (menuItems[idx].IsEmpty()) {
                    if (pMenu->GetMenuItemCount() < idx+1) break;
                    pMenu->RemoveMenu(idx, MF_BYPOSITION);
                } else {
                    if (pMenu->GetMenuItemCount() < idx+1) {
                        pMenu->AppendMenu(MF_ENABLED, ID_URL_1+idx, menuItems[idx]);
                    } else {
                        pMenu->ModifyMenu(idx, MF_BYPOSITION|MF_ENABLED, ID_URL_1+idx, menuItems[idx]);
                    }
                }
            }
        }
    }
}
