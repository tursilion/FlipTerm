// Mud32.h : main header file for the Mud32 application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#ifndef MUD_APP_DEFINED
#define MUD_APP_DEFINED

#include "resource.h"       // main symbols

#define BUFFSIZE 32767

/////////////////////////////////////////////////////////////////////////////
// CMudApp:
// See Mud32.cpp for the implementation of this class
//

class CMudApp : public CWinApp
{
public:
	CMudApp();
	~CMudApp();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMudApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
		// Server object for document creation

	//{{AFX_MSG(CMudApp)
	afx_msg void OnAppAbout();
	afx_msg void OnWindowNewOpenDoc();
	afx_msg void OnFileEditworldlist();
	afx_msg void OnViewFont();
	afx_msg void OnWindowColor();
	afx_msg void OnSoundsSettings();
	afx_msg void OnViewWrap();
	afx_msg void OnFileTogglelocalechoing();
	afx_msg void OnUpdateFileTogglelocalechoing(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	LOGFONT m_LogFont;
	COLORREF m_colorBackGround;
	COLORREF m_colorForeGround;

	BOOL m_bBell;
	BOOL m_bTriggerSound;
	BOOL m_bConnectBeep;

	BOOL m_bScreenWrap;
	BOOL m_bWordWrap;
	BOOL m_bIndent;
	BOOL m_bAnsi;
	BOOL m_bEcho;
	CString m_sDir;

};

#ifndef DEFINE_STORAGE_HERE
extern CMudApp theApp;
#else
CMudApp theApp;
#endif

#define GetApp() (&theApp)

// various windows messages
#define WM_FONTCHANGED		12345
#define WM_COLORCHANGED 	12346
#define WM_SELECT_VIEW		12347
#define WM_UPDATE_WORLDS	12348

/////////////////////////////////////////////////////////////////////////////
#endif // MUD_APP_DEFINED
