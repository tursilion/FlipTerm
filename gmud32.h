// Mud32.h : main header file for the Mud32 application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#ifndef MUD_APP_DEFINED
#define MUD_APP_DEFINED

#include "resource.h"					// main symbols

#define BUFFSIZE 32767

const HANDLE TheHeap=GetProcessHeap();	// Our shared memory heap
#define malloc(x) HeapAlloc(TheHeap, 0, x)
#define free(x) HeapFree(TheHeap, 0, x)

#define URL_MENU_INDEX 5
extern CString menuItems[10];

extern bool g_ClosingApp;

/////////////////////////////////////////////////////////////////////////////
// CMudApp:
// See Mud32.cpp for the implementation of this class
//

class CMudApp : public CWinAppEx
{
public:
	CMudApp();
    virtual ~CMudApp();
	void DoSave();

// Overrides
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	afx_msg void OnURL1();
	afx_msg void OnURL2();
	afx_msg void OnURL3();
	afx_msg void OnURL4();
	afx_msg void OnURL5();
	afx_msg void OnURL6();
	afx_msg void OnURL7();
	afx_msg void OnURL8();
	afx_msg void OnURL9();
	afx_msg void OnURL10();
	afx_msg void OnWindowNewOpenDoc();
	afx_msg void OnFileEditworldlist();
	afx_msg void OnViewFont();
	afx_msg void OnWindowColor();
	afx_msg void OnSystemColor();
	afx_msg void OnDefaultColor();
	afx_msg void OnSoundsSettings();
	afx_msg void OnViewWrap();
	afx_msg void OnOptionsLog();
	afx_msg void OnOptionsSwitches();
	afx_msg void OnUpdateExit(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAliasList(CCmdUI* pCmdUI);
	afx_msg void OnQuit();
	afx_msg void OnSave();
	afx_msg void OnExport();
	afx_msg void OnHistorynext();
	afx_msg void OnHistoryprev();
	DECLARE_MESSAGE_MAP()

public:
	LOGFONT m_LogFont;
	int m_colorBackGround;
	int m_colorForeGround;
	int m_colorSystem;
	int m_colorInput;

	BOOL m_bBell;
	BOOL m_bTriggerSound;
	BOOL m_bConnectBeep;

	BOOL m_bScreenWrap;
	BOOL m_bWordWrap;
	BOOL m_bIndent;
	BOOL m_bAnsi;
	BOOL m_bEcho;
	BOOL m_bMapKeys;
	BOOL m_bShowActivity;
	BOOL m_bUseCLI;
	BOOL m_bMultiline;
	BOOL m_bCommandStacking;
	BOOL m_bReviewEnd;
	BOOL m_bNewWindow;
	char m_CommandChar;
	BOOL m_bANSIReset;
	CString m_sDir;
	CString m_sLogFolder;
	int m_NumLines;
	
	CMultiDocTemplate* pDocTemplate;
};

CMudApp *GetApp();
extern CMudApp theApp;

// various windows messages
#define WM_FONTCHANGED		12345
#define WM_COLORCHANGED 	12346
#define WM_SELECT_VIEW		12347
#define WM_UPDATE_WORLDS	12348

/////////////////////////////////////////////////////////////////////////////
#endif // MUD_APP_DEFINED
