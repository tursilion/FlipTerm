// 3DPlugin.cpp 

#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <windows.h>
#include <commdlg.h>
#include "worldview.h"
#include <math.h>
#include <stdio.h>
#include <malloc.h>
#include "3DPlugin.h"
#include "resource.h"
#include "httpclient.h"

struct _session Sessions[MAXSESSIONS];		// max sessions. You're free to use a dynamic system to permit more

const char PACKAGENAME[] = "dns-com-tursisoft-3D";		// Tursisoft.com's 3D package
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
		MessageBox(NULL, "The 3D plugin was unable to initialize Winsock. Please try to restart.", "Winsock Error", MB_OK);
		return 0;
	}

	for (int idx=0; idx<MAXSESSIONS; idx++) {
		Sessions[idx].fActive=false;
		Sessions[idx].wnd=NULL;
		Sessions[idx].pView=NULL;
	}
	
	strncpy(pszRegKey, inRegKey, 1024);
	if (strlen(pszRegKey)<1010) {
		strcat(pszRegKey, "\\3DView");		// Set our local config key string
		// Current Registry key settings:
		// Cache - folder that contains cached images
		// Size  - Window size (1 or 2)

	}
	
	strcpy(pszCachePath, "");
	GetCurrentDirectory(1024, pszCachePath);	// set default
	nWindowSize=1;

	bufsize=1024;
	flag=false;
	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, pszRegKey, 0, KEY_READ, &hKey)) {
		if (ERROR_SUCCESS == RegQueryValueEx(hKey, "Cache", NULL, NULL, (unsigned char*)buf, &bufsize)) {
			flag=true;
			strcpy(pszCachePath, buf);
		}
		bufsize=1024;
		if (ERROR_SUCCESS == RegQueryValueEx(hKey, "Size", NULL, NULL, (unsigned char*)buf, &bufsize)) {
			nWindowSize=*((DWORD*)buf);
		} else {
			nWindowSize=1;
		}
		if (nWindowSize!=2) nWindowSize=1;

		RegCloseKey(hKey);
	}

	if (strlen(pszCachePath)<1023) {
		if (pszCachePath[strlen(pszCachePath)-1]!='\\') {
		strcat(pszCachePath, "\\");
		}
	} else {
		MessageBox(NULL, "Warning: 3D Plugin Image Cache Path is too long - select a shorter path.", "FlipTerm 3D Plugin", MB_ICONSTOP|MB_OK);
	}

	if (false == flag) {
		MessageBox(NULL, "No configuration for 3D plugin was detected. Before you will see images, you\nmust configure the 3D plugin and set the path to the image cache folder. This\nmust be the folder that currently contains the NULL.jpg file,\nusually under Plugins\\3dGraphics", "No Configuration found", MB_OK);
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
		Sessions[nSession].pView=NULL;
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
	
			l1=l2=h1=h2=0;
			sscanf(GetArg(pStr, "min-version"), "%d.%d max-version: %d.%d", &l1, &l2, &h1, &h2);	// floats just don't work well ;)
			low=l1*10+l2;
			high=h1*10+h2;

			if (high == 0) high=low;	// low, at least, must be valid

			if ((lowVERSION >= low)&&(highVERSION <= high)) {
				// We're good!
				Sessions[nSession].fActive=true;
				// We need to create our window now
				Sessions[nSession].pView=new CWorldView(hWnd, nWindowSize);
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

	if (strnicmp(&pStr[3], "mcp-internal", idx-3) == 0) {
		// Internal data passing
		char *ptype = GetArg(pStr, "type");
		if (NULL != ptype) {
			if (strnicmp(ptype, "focus", 5) == 0) {
				// Focus our child window, if available
				nSession=FindSession(hWnd);
				if (-1 != nSession) {
					if ((Sessions[nSession].pView)&&(Sessions[nSession].pView->hWnd)) {
						::SetWindowPos(Sessions[nSession].pView->hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOSIZE);
					}
				}
			}
		}
		return true;		// Other windows still get the code
	}

	if (strnicmp(&pStr[3], "*", idx-3) == 0) {
		// It's multiline data
		// At this point, your code must determine whether it's
		// multiline data intended for this plugin.

		// This sample does not use multiline
		return false;
	}

	// The same checks can be done if your plugin uses cords, by
	// checking against mcp-cord-open, mcp-cord, and mcp-cord-closed
	// We skip those steps here, as repeating the data is academic

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
	
		while ((*ptr)&&(isgraph(*ptr))&&(idx<128)) {
			szMsg[idx++]=*(ptr++);
		}
		szMsg[idx]='\0';

		// szMsg now contains just the message name. 

		// Messages to implement for 3D View:
		// :NewImage:	(args)
		// -FileName- Name of the image file to load
		// -Tag-    Name of the world it's in (update Window title: "3D View - Muck (World)" )
		// -URL-      Base URL for the image download
		// -Timestamp-Last update of this image
		// A null Filename should blank the image (ie: no image). In that case the other options are not required

		if (stricmp(szMsg, "NewImage")==0) {
			char szName[128];
			char szTag[128];
			char szURL[1024];
			char szTime[128];
			char *pTmp;
			int idx;

			pTmp=GetArg(pStr, "Filename");
			if (pTmp==NULL) {
				strcpy(szName, "");
				OutputDebugString("3DPlugin: Received command with no Filename\n");
			} else {
				idx=0;
				while ((*pTmp)&&(isgraph(*pTmp))&&(idx<128)) {
					szName[idx++]=*(pTmp++);
				}
				szName[idx]='\0';
			}

			pTmp=GetArg(pStr, "Tag");
			if (pTmp==NULL) {
				strcpy(szTag, "");
			} else {
				idx=0;
				while ((*pTmp)&&(isgraph(*pTmp))&&(idx<128)) {
					szTag[idx++]=*(pTmp++);
				}
				szTag[idx]='\0';
			}
			Sessions[nSession].pView->SetRawTag(szTag);

			// This way a muck can use a custom tag for no image
			if (strlen(szName)<1) {
				if (strlen(szTag)<1) {
					Sessions[nSession].pView->SetRawTag("No image available");
				}
			}

			pTmp=GetArg(pStr, "NoWrap");
			Sessions[nSession].pView->fWrap=true;
			if (pTmp) {
				if (*pTmp == '1') {
					Sessions[nSession].pView->fWrap=false;
				}
			}

			pTmp=GetArg(pStr, "URL");
			if (pTmp==NULL) {
				strcpy(szURL, "");
			} else {
				idx=0;
				while ((*pTmp)&&(isgraph(*pTmp))&&(idx<1024)) {
					szURL[idx++]=*(pTmp++);
				}
				szURL[idx]='\0';
				if (strlen(szURL)+strlen(szName)<1024) {
					strcat(szURL, szName);
					pTmp=szURL;
					while (*pTmp) {
						if (*pTmp == '\\') *pTmp='/';
						pTmp++;
					}
				} else {
					strcpy(szURL, "");
				}
			}

			pTmp=GetArg(pStr, "TimeStamp");
			if (pTmp==NULL) {
				strcpy(szTime, "0");
			} else {
				idx=0;
				while ((*pTmp)&&(isgraph(*pTmp))&&(idx<128)) {
					szTime[idx++]=*(pTmp++);
				}
				szTime[idx]='\0';
			}

			Sessions[nSession].pView->RequestImageLoad(szName, szURL, szTime);
		}

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
		if (Sessions[nSession].pView) {
			delete Sessions[nSession].pView;
			Sessions[nSession].pView=NULL;
		}
		Sessions[nSession].wnd=NULL;
	}
}

// Indicate whether we want to see all strings or not
bool MCPPlugin_WantPlainText() {
	return false;
}

// Process non-MCP strings. This function MUST be exported, even if unused.
// Note that the received string may contain ANSI and control codes!
// These should normally be preserved when modifying the string.
char *MCPPlugin_ProcessTextString(char *pStr, bool fActive) {
	return NULL;
}

// Procedure for the dialog box
INT_PTR CALLBACK dlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	HWND tmpWnd;

	switch (uMsg) {
	case WM_INITDIALOG:
		tmpWnd=GetDlgItem(hDlg, IDC_EDIT1);
		if (tmpWnd) SetWindowText(tmpWnd, pszCachePath);
		if (nWindowSize==1) {
			tmpWnd=GetDlgItem(hDlg, IDC_chk1x);
			if (tmpWnd) PostMessage(tmpWnd, BM_SETCHECK, BST_CHECKED, 0);
			tmpWnd=GetDlgItem(hDlg, IDC_chk2x);
			if (tmpWnd) PostMessage(tmpWnd, BM_SETCHECK, BST_UNCHECKED, 0);
		} else {
			tmpWnd=GetDlgItem(hDlg, IDC_chk1x);
			if (tmpWnd) PostMessage(tmpWnd, BM_SETCHECK, BST_UNCHECKED, 0);
			tmpWnd=GetDlgItem(hDlg, IDC_chk2x);
			if (tmpWnd) PostMessage(tmpWnd, BM_SETCHECK, BST_CHECKED, 0);
		}
		SetWindowPos(hDlg, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
		break;

	case WM_COMMAND:
		if (wParam == IDCANCEL) {
			EndDialog(hDlg, IDCANCEL);
		}
		
		if (wParam == IDOK) {
			tmpWnd=GetDlgItem(hDlg, IDC_chk1x);
			if (tmpWnd) {
				if (BST_CHECKED==SendMessage(tmpWnd, BM_GETCHECK, 0, 0)) {
					nWindowSize=1;
				}
			}
			tmpWnd=GetDlgItem(hDlg, IDC_chk2x);
			if (tmpWnd) {
				if (BST_CHECKED==SendMessage(tmpWnd, BM_GETCHECK, 0, 0)) {
					nWindowSize=2;
				}
			}
			tmpWnd=GetDlgItem(hDlg, IDC_EDIT1);
			if (tmpWnd) {
				SendMessage(tmpWnd, WM_GETTEXT, (WPARAM)1024, (LPARAM)pszCachePath);
				if (strlen(pszCachePath)<1023) {
					if (pszCachePath[strlen(pszCachePath)-1]!='\\') {
					strcat(pszCachePath, "\\");
					}
					EndDialog(hDlg, IDOK);
				} else {
					MessageBox(NULL, "Warning: 3D Plugin Image Cache Path is too long - select a shorter path.", "FlipTerm 3D Plugin", MB_ICONSTOP|MB_OK);
				}
			}
		}
		
		if (wParam == IDC_REOPEN) {
			int nSession=0;
			while (nSession < MAXSESSIONS) {
				if (Sessions[nSession].wnd != NULL) {
					if (Sessions[nSession].pView != NULL) {
						if (Sessions[nSession].pView->hWnd == NULL) {
							HWND oldParent=Sessions[nSession].pView->hParent;
							bool fOldWrap=Sessions[nSession].pView->fWrap;
							char oldImg[256], oldURL[256], oldTime[16];
							strcpy(oldImg, Sessions[nSession].pView->FirstImg.szRawFileName);
							strcpy(oldURL, Sessions[nSession].pView->FirstImg.szURL);
							strcpy(oldTime, Sessions[nSession].pView->FirstImg.szTime);
							delete Sessions[nSession].pView;
							Sessions[nSession].pView=new CWorldView(oldParent, nWindowSize);
							Sessions[nSession].pView->fWrap=fOldWrap;
							Sessions[nSession].pView->RequestImageLoad(oldImg, oldURL, oldTime);
						}
					}
				}
				nSession++;
			}
		}

		if (wParam == IDC_RESET) {
			int nSession=0;
			while (nSession < MAXSESSIONS) {
				if (Sessions[nSession].wnd != NULL) {
					if (Sessions[nSession].pView != NULL) {
						HWND oldParent=Sessions[nSession].pView->hParent;
						char oldImg[256], oldURL[256], oldTime[16];
						char pstr[128];
						HKEY hKey;
						bool fOldWrap=Sessions[nSession].pView->fWrap;

						strcpy(oldImg, Sessions[nSession].pView->FirstImg.szRawFileName);
						strcpy(oldURL, Sessions[nSession].pView->FirstImg.szURL);
						strcpy(oldTime, Sessions[nSession].pView->FirstImg.szTime);

						::GetWindowText(oldParent, pstr, 128);
						pstr[125]='\0';

						delete Sessions[nSession].pView;

						// Delete the registry setting
						if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, pszRegKey, 0, KEY_ALL_ACCESS, &hKey)) {
							strcat(pstr, "X");
							RegDeleteValue(hKey, pstr);
							pstr[strlen(pstr)-1]='Y';
							RegDeleteValue(hKey, pstr);
							RegCloseKey(hKey);
						}
						Sessions[nSession].pView=new CWorldView(oldParent, nWindowSize);
						strcpy(Sessions[nSession].pView->FirstImg.szFileName, oldImg);
						Sessions[nSession].pView->fWrap=fOldWrap;
						Sessions[nSession].pView->RequestImageLoad(oldImg, oldURL, oldTime);
					}
				}
				nSession++;
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
			fn.lpstrTitle="Select 3D Graphics Folder Containing NULL.jpg";
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

// Display about information to the user
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
			RegSetValueEx(hKey, "Size", NULL, REG_DWORD, (unsigned char*)&nWindowSize, sizeof(int));
			RegCloseKey(hKey);
		}
		
		if (nOldWindowSize != nWindowSize) {
			int nSession=0;
			while (nSession < MAXSESSIONS) {
				if (Sessions[nSession].wnd != NULL) {
					if (Sessions[nSession].pView != NULL) {
						Sessions[nSession].pView->SetWindowSize(nWindowSize, true);
					}
				}
				nSession++;
			}
		}
	}
}

// Helper functions, not exported

// Extracts a pointer to the beginning of the named argument in the string
// Note that it's a pointer in the original string, not a new string!
// Also note how poor this code is - it can misinterpret keywords and stuff
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
	if (*pTmp==':') pTmp++;
	while (*pTmp==' ') pTmp++;

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
