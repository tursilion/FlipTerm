/////////////////////////////////////////////////////////////////////////////
//
// mainfrm.cpp : implementation of the CMainFrame class
//
// CMainFrame
//
#include "stdafx.h"
#include "gmud32.h"
#include "mainfrm.h"
#include "gmud32vw.h"
#include "gmud32doc.h"
#include "buttoned.h"
#include "macroarray.h"
#include "macro.h"
#include "world.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_LBN_SELCHANGE(IDC_CONNECT_LIST, OnSelchangeConnectList)
	ON_LBN_SETFOCUS(IDC_CONNECT_LIST, OnSetfocusConnectList)
	ON_LBN_DBLCLK(IDC_CONNECT_LIST, OnDblclkConnectList)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_EDIT, OnButtonEdit)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON10, OnButton10)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	ON_BN_CLICKED(IDC_BUTTON3, OnButton3)
	ON_BN_CLICKED(IDC_BUTTON4, OnButton4)
	ON_BN_CLICKED(IDC_BUTTON5, OnButton5)
	ON_BN_CLICKED(IDC_BUTTON6, OnButton6)
	ON_BN_CLICKED(IDC_BUTTON7, OnButton7)
	ON_BN_CLICKED(IDC_BUTTON8, OnButton8)
	ON_BN_CLICKED(IDC_BUTTON9, OnButton9)
	ON_MESSAGE(WM_SELECT_VIEW,OnViewSelected)
	ON_MESSAGE(WM_UPDATE_WORLDS,OnUpdateWorlds)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND_EX(ID_VIEW_MACROWINDOW, OnBarCheck)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MACROWINDOW,OnUpdateControlBarMenu)
	ON_COMMAND_EX(ID_VIEW_WORLDWINDOW, OnBarCheck)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WORLDWINDOW,OnUpdateControlBarMenu)
	ON_COMMAND(ID_EDIT_ALIASLIST, OnEditAliaslist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// arrays of IDs used to initialize control bars
	
// toolbar buttons - IDs are command buttons
static UINT BASED_CODE buttons[] =
{
	// same order as in the bitmap 'toolbar.bmp'
	ID_SERVER_CONNECT,
	ID_SERVER_DISCONNECT,
		ID_SEPARATOR,
	ID_WINDOW_PAUSE,
		ID_SEPARATOR,
	ID_FILE_EDITWORLDLIST,
		ID_SEPARATOR,
	ID_VIEW_WRAP,
	ID_VIEW_FONT,
		ID_SEPARATOR,
	ID_WINDOW_NEW_OPEN_DOC,
		ID_SEPARATOR,
	ID_WINDOW_COLOR,
		ID_SEPARATOR,
 	ID_SOUNDS_SETTINGS,	
		ID_SEPARATOR,
	ID_FILE_EDITTRIGGERLIST,
		ID_SEPARATOR,
	ID_FILE_TOGGLELOGGING,
		ID_SEPARATOR,
	ID_APP_ABOUT,
		ID_SEPARATOR,
  ID_EDIT_ALIASLIST
};

static UINT BASED_CODE indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_bMacrosVisible=FALSE;	
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.Create(this) ||
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
	
	if(!m_wndMacroBar.Create(this,IDD_MACRO_BAR,CBRS_LEFT,ID_VIEW_MACROWINDOW))
	{
		TRACE("Failed to create macrobar\n");
		return -1;
	}
	m_bMacrosVisible=TRUE;

	if(!m_wndConnected.Create(this,IDD_CONNECTED_WORLD_BAR,CBRS_LEFT,ID_VIEW_WORLDWINDOW))
	{
		TRACE("Failed to create macrobar\n");
		return -1;
	}
	m_bConnectedVisible=TRUE;
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndMacroBar.EnableDocking(CBRS_ALIGN_LEFT|CBRS_ALIGN_RIGHT);
	m_wndConnected.EnableDocking(CBRS_ALIGN_LEFT|CBRS_ALIGN_RIGHT);

	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	DockControlBar(&m_wndConnected,AFX_IDW_DOCKBAR_LEFT);
	DockControlBar(&m_wndMacroBar,AFX_IDW_DOCKBAR_LEFT);

	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndMacroBar.SetBarStyle(CBRS_FLOAT_MULTI|CBRS_TOOLTIPS|CBRS_ALIGN_LEFT|CBRS_ALIGN_RIGHT|CBRS_FLYBY);
	m_wndConnected.SetBarStyle(CBRS_FLOAT_MULTI|CBRS_TOOLTIPS|CBRS_ALIGN_LEFT|CBRS_ALIGN_RIGHT|CBRS_FLYBY);
	LoadBarState(AfxGetAppName());

	UpdateButtons(); // show macro buttons
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

BOOL CMainFrame::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	dwStyle &= ( ~ FWS_ADDTOTITLE);	 // clear out FWS_ADDTOTITLE
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

void CMainFrame::OnClose() 
{
	SaveBarState(AfxGetAppName());	
	CMDIFrameWnd::OnClose();
}

void CMainFrame::OnButtonEdit() 
{
	CMDIChildWnd * pChild= MDIGetActive();
	CMudView *pView=0;
	if(pChild)
		pView=(CMudView *)pChild->GetActiveView();
	else
		return ;
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
				return ;
		}
		else
			return ;
	}
	else
		return ;

	CButtonEdit dlg(this);
	dlg.SetList(pArray);
	if(dlg.DoModal()==IDOK)
		UpdateButtons();	
}

CMainFrame::UpdateButtons()
{
	if(!SetButtons())
		ClearButtons();
	return TRUE;
}

CMainFrame::ClearButtons()
{
	m_wndMacroBar.GetDlgItem(IDC_BUTTON1)->SetWindowText("");
	m_wndMacroBar.GetDlgItem(IDC_BUTTON2)->SetWindowText("");
	m_wndMacroBar.GetDlgItem(IDC_BUTTON3)->SetWindowText("");
	m_wndMacroBar.GetDlgItem(IDC_BUTTON4)->SetWindowText("");
	m_wndMacroBar.GetDlgItem(IDC_BUTTON5)->SetWindowText("");
	m_wndMacroBar.GetDlgItem(IDC_BUTTON6)->SetWindowText("");
	m_wndMacroBar.GetDlgItem(IDC_BUTTON7)->SetWindowText("");
	m_wndMacroBar.GetDlgItem(IDC_BUTTON8)->SetWindowText("");
	m_wndMacroBar.GetDlgItem(IDC_BUTTON9)->SetWindowText("");
	m_wndMacroBar.GetDlgItem(IDC_BUTTON10)->SetWindowText("");
	return TRUE;
}


CMainFrame::SetButtons()
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

	pButton=m_wndMacroBar.GetDlgItem(IDC_BUTTON1);
	ASSERT_VALID(pButton);
	CMacro *pMacro = (CMacro *)pArray->Get("F1");
	ASSERT_VALID(pMacro);
	pButton->SetWindowText(pMacro->m_sText.Left(12));
	pButton=m_wndMacroBar.GetDlgItem(IDC_BUTTON2);
	pButton->SetWindowText(((CMacro *)(pArray->Get("F2")))->m_sText.Left(12));
	pButton=m_wndMacroBar.GetDlgItem(IDC_BUTTON3);
	pButton->SetWindowText(((CMacro *)(pArray->Get("F3")))->m_sText.Left(12));
	pButton=m_wndMacroBar.GetDlgItem(IDC_BUTTON4);
	pButton->SetWindowText(((CMacro *)(pArray->Get("F4")))->m_sText.Left(12));
	pButton=m_wndMacroBar.GetDlgItem(IDC_BUTTON5);
	pButton->SetWindowText(((CMacro *)(pArray->Get("F5")))->m_sText.Left(12));
	pButton=m_wndMacroBar.GetDlgItem(IDC_BUTTON6);
	pButton->SetWindowText(((CMacro *)(pArray->Get("F6")))->m_sText.Left(12));
	pButton=m_wndMacroBar.GetDlgItem(IDC_BUTTON7);
	pButton->SetWindowText(((CMacro *)(pArray->Get("F7")))->m_sText.Left(12));
	pButton=m_wndMacroBar.GetDlgItem(IDC_BUTTON8);
	pButton->SetWindowText(((CMacro *)(pArray->Get("F8")))->m_sText.Left(12));
	pButton=m_wndMacroBar.GetDlgItem(IDC_BUTTON9);
	pButton->SetWindowText(((CMacro *)(pArray->Get("F9")))->m_sText.Left(12));
	pButton=m_wndMacroBar.GetDlgItem(IDC_BUTTON10);
	pButton->SetWindowText(((CMacro *)(pArray->Get("F10")))->m_sText.Left(12));
 	return TRUE;
}

CMainFrame::OutputMacro(CString str)
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

CMainFrame::UpdateWorlds()
{
	m_wndConnected.EmptyList();
	CMudView* pView = 0;
	CMudDoc* pDocument =NULL;
	CMDIChildWnd * pChild =	((CMDIFrameWnd*)(AfxGetApp()->m_pMainWnd))->MDIGetActive();
	if(pChild )
		pDocument = (CMudDoc *)pChild->GetActiveDocument();
	if(pDocument)
	{
		POSITION pos = pDocument->GetFirstViewPosition();
		while (pos != NULL)
		{
			pView = (CMudView*)pDocument->GetNextView(pos);
			ASSERT_VALID(pView);
			m_wndConnected.AddName(pView->m_bUnSeen,pView->GetTitle());
		}
	}
	else
		TRACE("\nCouldn't get active doc in updateworlds");
	UpdateButtons();
	return TRUE;
}

afx_msg LONG CMainFrame::OnViewSelected(UINT,LONG lParam)
{
	CString *pStr = (CString *)(void *)lParam;
	TRACE("\nView %s selected.",pStr);
	UpdateButtons();
	UpdateWorlds();
	return TRUE;
}

void CMainFrame::OnSetfocusConnectList() 
{
}

void CMainFrame::OnDblclkConnectList() 
{
	OnSelchangeConnectList() ;
}

void CMainFrame::OnSelchangeConnectList()
{
	TRACE("\nConnect list got clicked.");
	int iSelected = m_wndConnected.GetCurSel();
	int iSelAt = 0;
	CMDIChildWnd * pChild =	((CMDIFrameWnd*)(AfxGetApp()->m_pMainWnd))->MDIGetActive();
	CDocument *pDocument=0;
	CView *pView=0;
	if(pChild )
		pDocument = (CMudDoc *)pChild->GetActiveDocument();
	if(pDocument)
	{
		POSITION pos = pDocument->GetFirstViewPosition();
		while (pos != NULL)
		{
			pView = (CMudView*)pDocument->GetNextView(pos);
			ASSERT_VALID(pView);
			if(iSelAt==iSelected)
			{
				MDIActivate(pView->GetParent()); //->SetFocus();
				pView->SetFocus();
			}
			iSelAt++;
		}
	}
	else
		TRACE("\nCouldn't get active doc in onselchangeworldlsit");
	UpdateButtons();
}

afx_msg LONG CMainFrame::OnUpdateWorlds(UINT,LONG)
{
	UpdateWorlds();
	return TRUE;
}

void CMainFrame::OnEditCopy() 
{
	//CEditWnd
	
}

void CMainFrame::OnEditAliaslist() 
{
	CMDIChildWnd * pChild= MDIGetActive();
	CMudView *pView=0;
	if(pChild)
		pView=(CMudView *)pChild->GetActiveView();
	else
		return ;
	CMacroArray* pArray=0;
	if(pView && pView->m_pWorld)
	{
		CMudDoc *pDoc = pView->GetDocument();
    pDoc->EditAliasList(pView->m_pWorld); 
  }
}
