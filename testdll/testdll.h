// testdll.h : main header file for the testdll DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CtestdllApp
// See testdll.cpp for the implementation of this class
//

class CtestdllApp : public CWinApp
{
public:
	CtestdllApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
