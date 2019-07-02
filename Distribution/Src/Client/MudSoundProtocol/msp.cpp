// MSP Plugin.cpp 
// Supports version 0.3 as described at http://www.zuggsoft.com/zmud/msp.htm
// A bit hacky and may not be 100%.
// Also supports MCP media play requests (including movies)

#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <windows.h>
#include <commdlg.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "msp.h"
#include "resource.h"
#include "httpclient.h"

struct _session Sessions[MAXSESSIONS];		// max sessions. You're free to use a dynamic system to permit more

const char PACKAGENAME[] = "dns-com-harmlesslion-Media";
const int lowVERSION = 10;								// Version 1.0 (times 10)
const int highVERSION = 10;								// same

const int specVERSION = 10;		// based on version 1.0 of the spec, you won't normally change this

char pszCachePath[1024];
int nWindowSize;

char pszRegKey[1024];
void *(*MCP_InfoRequest)(DWORD nID);

HANDLE hInstance;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			hInstance=hModule;
			break;

		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;

		case DLL_PROCESS_DETACH:
			WSACleanup();
			break;
    }
    return TRUE;
}

// Perform initialization and reset the DLL to inactive
int MCPPlugin_Initialize(char *inRegKey) {
	
	HKEY hKey;
	char buf[1024];
	unsigned long bufsize;
	bool flag;
	WSADATA myWSAData;

	if (WSAStartup(MAKEWORD(1,1), &myWSAData)) {
		MessageBox(NULL, "The Media plugin was unable to initialize Winsock. Please try to restart.", "Winsock Error", MB_OK);
		return 0;
	}

	for (int idx=0; idx<MAXSESSIONS; idx++) {
		Sessions[idx].fActive=false;
		Sessions[idx].wnd=NULL;
	}
	
	strncpy(pszRegKey, inRegKey, 1024);
	if (strlen(pszRegKey)<1010) {
		strcat(pszRegKey, "\\MediaPlay");		// Set our local config key string
	}
	
	strcpy(pszCachePath, "");
	GetCurrentDirectory(1024, pszCachePath);	// set default

	bufsize=1024;
	flag=false;
	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, pszRegKey, 0, KEY_READ, &hKey)) {
		if (ERROR_SUCCESS == RegQueryValueEx(hKey, "Cache", NULL, NULL, (unsigned char*)buf, &bufsize)) {
			flag=true;
			strcpy(pszCachePath, buf);
		}
		RegCloseKey(hKey);
	}

	if (strlen(pszCachePath)<1023) {
		if (pszCachePath[strlen(pszCachePath)-1]!='\\') {
		strcat(pszCachePath, "\\");
		}
	} else {
		MessageBox(NULL, "Warning: Media Plugin Cache Path is too long - select a shorter path.", "FlipTerm MediaPlayback Plugin", MB_ICONSTOP|MB_OK);
	}

	if (false == flag) {
		MessageBox(NULL, "No configuration for Media plugin was detected. Before you can playback media, you\nmust configure the Media Plugin and set the path to a cache folder", "No Configuration found", MB_OK);
		MCPPlugin_About();
	}

	return specVERSION;	// We support version 1.0 (times 10)
}

// Inform the client how many packages we host
int MCPPlugin_GetNumberPackages() {
	return 1;			// just 1
}

// Return the name of the packages we support
const char *MCPPlugin_GetPackageName(int n) {
	if (n!=0) {		// out of range
		return NULL;
	}

	return PACKAGENAME;
}

// Return the low version of the supported package
int MCPPlugin_GetPackageLowVersion(int n) {
	if (n!=0) {		// out of range
		return 0;
	}

	return lowVERSION;		// 1.0 times 10
}

// Return the high version of the supported package
int MCPPlugin_GetPackageHighVersion(int n) {
	if (n!=0) {		// out of range
		return 0;
	}

	return highVERSION;
}

// Process the received string, verifying that it's ours
bool MCPPlugin_ProcessString(char *pStr, HWND hWnd, char **pOut) {
	// Process the received string to determine if it's ours
	int idx;
	int nSession;

	if (NULL == pStr) return false;

	// Find this session
	nSession=FindSession(hWnd);
	if (-1 == nSession) {
		// Try to create a new one
		nSession=0;
		while (nSession < MAXSESSIONS) {
			if (Sessions[nSession].wnd == NULL) {
				break;
			}
			nSession++;
		}
		if (nSession >= MAXSESSIONS) {
			// No room for a new one
			OutputDebugString("Failed to create a new session - aborting.\n");
			return false;
		}

		Sessions[nSession].wnd=hWnd;
		Sessions[nSession].fActive=false;
	}

	idx=0;
	while ((pStr[idx])&&(pStr[idx]!=' ')) idx++;
	if (pStr[idx] != ' ') return false;		// no space? Invalid string
	if (idx<4) return false;				// three for the #$#, and at least 1 for another character
	
	// We know the string starts with #$#. The keyword we want ends at
	// the first space. So we compare the strings to see what we have.

	if (strnicmp(&pStr[3], "mcp-negotiate-can", idx-3) == 0) {
		if (Sessions[nSession].fActive) {
			// We're already active, so ignore it
			return false;
		}

		// Is the package us?
		char *pPackage = GetArg(pStr, "package");
		if (NULL == pPackage) return false;

		if (strnicmp(pPackage, PACKAGENAME, strlen(PACKAGENAME)) == 0) {
			// Cool! Is it a valid version?
			int l1, l2, h1, h2;
			int low, high;

			sscanf(GetArg(pStr, "min-version"), "%d.%d max-version: %d.%d", &l1, &l2, &h1, &h2);	// floats just don't work well ;)
			low=l1*10+l2;
			high=h1*10+h2;

			if (high == 0) high=low;	// low, at least, must be valid

			if ((lowVERSION >= low)&&(highVERSION <= high)) {
				// We're good!
				Sessions[nSession].fActive=true;
			} else {
				OutputDebugString("Server does not support our version.\n");
			}

			// either way, we processed the string
			return true;
		}

		// not us
		return false;
	}

	if (false == Sessions[nSession].fActive) {
		// We mustn't honour any other strings if we are not active
		return false;
	}

	if (strnicmp(&pStr[3], "*", idx-3) == 0) {
		// It's multiline data
		// At this point, your code must determine whether it's
		// multiline data intended for this plugin.

		// This sample does not use multiline
		return false;
	}

	// Is it a simple message for us?
	if (strnicmp(&pStr[3], PACKAGENAME, strlen(PACKAGENAME)) == 0) {	// not idx-3, there may be a message name attached
		// Why yes, yes it is. Get the actual message name out of the package and stuff
		// You can skip all this if your package doesn't have multiple messages
		// This is just for example
		char szMsg[128];		
		char *ptr;
		int idx;

		ptr=&pStr[3+strlen(PACKAGENAME)];
		if (*ptr =='-') ptr++;
		
		strcpy(szMsg, "");
		idx=0;
	 
		while ((*ptr)&&(*ptr != ' ')&&(idx<128)) {
			szMsg[idx++]=*(ptr++);
		}

		// szMsg now contains just the message name. 
		// This particular plugin doesn't care, though

		// At this point you proceed to process the message.
		// We just print some debug and return a string to send to the muck
		OutputDebugString("Got a message!\n");
		

		return true;
	}

	// If all has failed, then this just isn't a message for us
	return false;
}

// A session has closed and may be removed from the list
void MCPPlugin_CloseSession(HWND hWnd) {
	int nSession;

	nSession=FindSession(hWnd);
	if (-1 != nSession) {
		Sessions[nSession].wnd=NULL;
	}
}

// Indicate whether we want to see all strings or not
bool MCPPlugin_WantPlainText() {
	return true;
}

// Process non-MCP strings. This function MUST be exported, even if unused.
// Note that the received string may contain ANSI and control codes!
// These should normally be preserved when modifying the string.
char *MCPPlugin_ProcessTextString(char *pStr, bool fActive) {
	char /* *pOut,*/ *pTmp;
	
	// Check if this string is one we want. We won't play the sound if this isn't the
	// currently active Window
	if (!fActive) return NULL;

	if (NULL != (pTmp=strstr(pStr, "!!SOUND("))) {
		// Sound command
		char fname[1024];
		char type[512];		// The spec is unclear what this is even for
		char url[1024];
		char tmp[1024];
		char *pDat;
		int vol=100;
		int repeats=1;
		int priority=50;
		int idx;

		strcpy(fname, "");
		strcpy(type, "");
		strcpy(url, "");
		
		// Verify at start of line only
		if (pTmp != pStr) return NULL;

		// Collect the data
		// fname
		pDat=pTmp+8;
		while (*pDat == ' ') pDat++;
		idx=0;
		while ((*pDat)&&(*pDat != ' ')&&(*pDat != ')')&&(idx<1023)) {
			fname[idx++]=*(pDat++);
		}
		*pDat='\0';

		// vol (we'll just reuse type for these numbers)
		pDat=GetArg(pTmp, "V=");
		if (NULL != pDat) {
			idx=0;
			while ((*pDat)&&(*pDat != ' ')&&(*pDat != ')')&&(idx<511)) {
				type[idx++]=*(pDat++);
			}
			*pDat='\0';
			vol=atoi(type);
			if ((vol == 0)&&(type[0]!='0')) vol=100;
		}

		// repeats
		pDat=GetArg(pTmp, "L=");
		if (NULL != pDat) {
			idx=0;
			while ((*pDat)&&(*pDat != ' ')&&(*pDat != ')')&&(idx<511)) {
				type[idx++]=*(pDat++);
			}
			*pDat='\0';
			if (type[0]=='-') {
				repeats=-atoi(&type[1]);
			} else {
				repeats=atoi(type);
			}
			if (repeats == 0) repeats=1;
		}

		// priority
		pDat=GetArg(pTmp, "P=");
		if (NULL != pDat) {
			idx=0;
			while ((*pDat)&&(*pDat != ' ')&&(*pDat != ')')&&(idx<511)) {
				type[idx++]=*(pDat++);
			}
			*pDat='\0';
			priority=atoi(type);
			if ((priority == 0)&&(type[0]!='0')) priority=50;
		}

		// type for real this time ;)
		pDat=GetArg(pTmp, "T=");
		if (NULL != pDat) {
			idx=0;
			while ((*pDat)&&(*pDat != ' ')&&(*pDat != ')')&&(idx<511)) {
				type[idx++]=*(pDat++);
			}
			*pDat='\0';
		}

		// and URL
		pDat=GetArg(pTmp, "T=");
		if (NULL != pDat) {
			idx=0;
			while ((*pDat)&&(*pDat != ' ')&&(*pDat != ')')&&(idx<1023)) {
				url[idx++]=*(pDat++);
			}
			*pDat='\0';
		}

		// Verify that fname is legal - no "..", ":", and not empty
		if ((strlen(fname)==0)||(strstr(fname, ".."))||(strchr(fname, ':'))) {
			return NULL;	// illegal char
		}

		// URLs don't allow .. or | or file: to prevent file access
		strcpy(tmp, url);
		strlwr(tmp);		
		if ((strstr(url, ".."))||(strchr(url, '|'))||(strstr(tmp, "file:"))) {
			return NULL;	// illegal char
		}

		// fname needs and '/' chars converted to '\\'
		idx=0;
		while (fname[idx]) {
			if (fname[idx]=='/') fname[idx]='\\';
			idx++;
		}

		_snprintf(tmp, 1024, "File=%s, Vol=%d, Repeat=%d, Priority=%d, Type=%s, URL=%s\n", fname, vol, repeats, priority, type, url);
		tmp[1023]='\0';
		OutputDebugString(tmp);
	}

	// Invalidate the string so it's not printed
//	pOut=(char*)ProgMalloc(1);
//	*pOut='\0';
//	return pOut;
return NULL;
}

// Procedure for the dialog box
INT_PTR CALLBACK dlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	HWND tmpWnd;

	switch (uMsg) {
	case WM_INITDIALOG:
		tmpWnd=GetDlgItem(hDlg, IDC_EDIT1);
		if (tmpWnd) SetWindowText(tmpWnd, pszCachePath);
		SetWindowPos(hDlg, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
		break;

	case WM_COMMAND:
		if (wParam == IDCANCEL) {
			EndDialog(hDlg, IDCANCEL);
		}
		
		if (wParam == IDOK) {
			tmpWnd=GetDlgItem(hDlg, IDC_EDIT1);
			if (tmpWnd) {
				SendMessage(tmpWnd, WM_GETTEXT, (WPARAM)1024, (LPARAM)pszCachePath);
				if (strlen(pszCachePath)<1023) {
					if (pszCachePath[strlen(pszCachePath)-1]!='\\') {
					strcat(pszCachePath, "\\");
					}
					EndDialog(hDlg, IDOK);
				} else {
					MessageBox(NULL, "Warning: Media Plugin Cache Path is too long - select a shorter path.", "FlipTerm Media Playback Plugin", MB_ICONSTOP|MB_OK);
				}
			}
		}
		
		if (wParam == IDC_BROWSE) {
			OPENFILENAME fn;
			char buf[1024], buf2[1024];

			strcpy(buf, "(SELECT FOLDER)");
			strcpy(buf2, "");

			tmpWnd=GetDlgItem(hDlg, IDC_EDIT1);
			if (tmpWnd) {
				SendMessage(tmpWnd, WM_GETTEXT, (WPARAM)1024, (LPARAM)buf2);
			}

			fn.lStructSize=sizeof(fn);
			fn.hwndOwner=hDlg;
			fn.hInstance=NULL;
			fn.lpstrFilter=NULL;
			fn.lpstrCustomFilter=NULL;
			fn.nMaxCustFilter=0;
			fn.nFilterIndex=0;
			fn.lpstrFile=buf;
			fn.nMaxFile=1024;
			fn.lpstrFileTitle=NULL;
			fn.nMaxFileTitle=NULL;
			fn.lpstrInitialDir=buf2;
			fn.lpstrTitle="Select Cache Folder for Downloaded Media";
			fn.Flags=OFN_HIDEREADONLY;
			fn.nFileOffset=0;
			fn.nFileExtension=0;
			fn.lpstrDefExt=NULL;
			fn.lCustData=NULL;
			fn.lpfnHook=NULL;
			fn.lpTemplateName=NULL;

			if (GetOpenFileName(&fn)) {
				buf[fn.nFileOffset]='\0';
				tmpWnd=GetDlgItem(hDlg, IDC_EDIT1);
				if (tmpWnd) SetWindowText(tmpWnd, buf);
			}
		}
		break;

	case WM_CLOSE:
		EndDialog(hDlg, IDCANCEL);
		break;
	}

	return false;
}

// (Runs the Configuration dialog)
void MCPPlugin_About() {
	int nRet;
	HGLOBAL hRes;
	HRSRC hResInfo;
	void *pTemplate;
	int nOldWindowSize;

	hResInfo=FindResource((HINSTANCE)hInstance, MAKEINTRESOURCE(IDD_DIALOG1), RT_DIALOG);
	hRes=LoadResource((HINSTANCE)hInstance, hResInfo);
	pTemplate=LockResource(hRes);

	nOldWindowSize=nWindowSize;

	nRet=DialogBoxIndirect((HINSTANCE)hInstance, (DLGTEMPLATE*)pTemplate, NULL, dlgProc);

	if (IDOK == nRet) {
		HKEY hKey;

		// Write the config to the registry
		if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_CURRENT_USER, pszRegKey, 0, "", REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL)) {
			RegSetValueEx(hKey, "Cache", NULL, REG_SZ, (unsigned char*)pszCachePath, strlen(pszCachePath));
			RegCloseKey(hKey);
		}
	}
}

// Helper functions, not exported

// Extracts a pointer to the beginning of the named argument in the string
// Note that it's a pointer in the original string, not a new string!
char *GetArg(char *pStr, char *pName) {
	char *pTmp;
	char *inStr;
	char *subStr;

	inStr=strdup(pStr);
	subStr=strdup(pName);
	strlwr(inStr);
	strlwr(subStr);
	pTmp=strstr(inStr, subStr);	// Need it case-insensitive

	if (NULL == pTmp) {
		free(inStr);
		free(subStr);
		return NULL;
	}

	pTmp=pStr+(pTmp-inStr);		// Readjust to the correct string

	pTmp+=strlen(subStr);
	while ((*pTmp=='=')||(*pTmp==' ')) pTmp++;

	free(inStr);				// Done with these
	free(subStr);
	
	if (*pTmp=='\0') {
		return NULL;
	} else {
		return pTmp;
	}
}

// Returns a (more or less) unique key based on this module's address and the timestamp
// Note that the static buffer is recycled. Not multithread safe, and assumes this function
// will not be generating two keys in the same 10ms-or-so timeslice.
char *MakeKey() {
	static char buf[128];
	FILETIME myTime;

	GetSystemTimeAsFileTime(&myTime);

	sprintf(buf, "%u%u", &PACKAGENAME, myTime.dwLowDateTime);
	return buf;
}

// Malloc's memory on the calling program's heap, so that program can free the data
// successfully
void *ProgMalloc(size_t nSize) {
	HANDLE theHeap;

	theHeap=GetProcessHeap();
	return HeapAlloc(theHeap, 0, nSize);
}

// Find a specified session number
int FindSession(HWND hWnd) {
	int nSession=0;
	while (nSession < MAXSESSIONS) {
		if (Sessions[nSession].wnd == hWnd) {
			break;
		}
		nSession++;
	}

	if (Sessions[nSession].wnd == hWnd) {
		return nSession;
	} else {
		return -1;
	}
}

// Return the number of characters to skip to get to the next
// Non-ANSI sequence
int MyAnsiNext(LPCSTR pStr)
{
	if(!*pStr)
	{
		return 0;
	}
	LPCSTR pAbort = pStr;
	if(*pStr!=27)
		return 1;
	pStr++;
	if(strchr("c78ABCDFGHIJKMZ=>12<",*pStr))
		return 2;
	if(*pStr=='#' && *(pStr+1)>'2' && *(pStr+1)<'9')
		return 3;
	if( (*pStr=='(' || *pStr==')')&& strchr("AB012",*(pStr+1)))
		return 3;
	if(*pStr!='[')
		goto abort;
	pStr++;
	if(strchr("KJm",*pStr))
		return 3;
	do{
		if(*pStr==';')
			pStr++; // move off of '[' or ';'

		// get first digit
		if(isdigit(*pStr))
			pStr++;
		else
			goto abort;

		// get second digit
		if(isdigit(*pStr))
			pStr++;
	}
	while(*pStr==';');
	if(strchr("rmABCDRnHfsuJkKycqg",*pStr))
		return (pStr-pAbort)+1;
abort:
	return 1;
	
}

/////////////////////////////////////////
