// Plugin_Skeleton.cpp 
// This skeleton contains code and definitions for a module containing
// only a single package. But it is easily extended for more.

#include <windows.h>
#include "Plugin_Skeleton.h"
#include <stdio.h>

const char PACKAGENAME[] = "dns-com-tursisoft-dummy";	// Tursisoft.com's Dummy package
const int lowVERSION = 10;								// Version 1.0 (times 10)
const int highVERSION = 10;								// same

const int specVERSION = 10;		// based on version 1.0 of the spec, you won't normally change this

char pszRegKey[1024];
void *(*MCP_InfoRequest)(DWORD nID);

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

// Perform initialization and reset the DLL to inactive
int MCPPlugin_Initialize(char *inRegKey) {
	for (int idx=0; idx<MAXSESSIONS; idx++) {
		Sessions[idx].fActive=false;
		Sessions[idx].wnd=NULL;
	}

	strncpy(pszRegKey, inRegKey, 1024);
	if (strlen(pszRegKey)<1010) {
		strcat(pszRegKey, "\\dummy");		// Set our local config key string
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
	 
		while ((*ptr)&&(*ptr != ' ')&&(idx<128)) {
			szMsg[idx++]=*(ptr++);
		}

		// szMsg now contains just the message name. 
		// This particular plugin doesn't care, though

		// At this point you proceed to process the message.
		// We just print some debug and return a string to send to the muck
		OutputDebugString("Got a message!\n");
		
		// Reuse ptr to send a mesage back to the muck - forces a pose ;)
		ptr=(char*)ProgMalloc(1024);
		strcpy(ptr, ":jumps up and down. \"Whoo-hoo! It works!\"");
		*pOut=ptr;

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
	return false;
}

// Process non-MCP strings. This function MUST be exported, even if unused.
// Note that the received string may contain ANSI and control codes!
// These should normally be preserved when modifying the string.
char *MCPPlugin_ProcessTextString(char *pStr, bool fActive) {
	return NULL;
}

// Display about information to the user
void MCPPlugin_About() {
	char szMsg[4096];

	sprintf(szMsg, "Dummy Plugin for FlipTerm by Tursi\nhttp://tursi.yiffco.com\nVersion 1.0\n");
	MessageBox(NULL, szMsg, "Dummy MCP Plugin", MB_OK);
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

// Modify style of a window (ie: the parent! ;) )
// Adapted from the MFC source
static bool _ModifyStyle(HWND hWnd, int nStyleOffset, DWORD dwRemove, DWORD dwAdd, UINT nFlags) {
	if (NULL == hWnd) return true;

	DWORD dwStyle = GetWindowLong(hWnd, nStyleOffset);
	DWORD dwNewStyle = (dwStyle & ~dwRemove) | dwAdd;

	if (dwStyle == dwNewStyle)
		return true;	// If no change, it's not an error

	::SetWindowLong(hWnd, nStyleOffset, dwNewStyle);

	if (nFlags)
	{
		::SetWindowPos(hWnd, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | nFlags);
	}
	return true;
}

// Defined in WinUser.h
#define GWL_STYLE           (-16)
#define GWL_EXSTYLE         (-20)

bool ModifyStyle(HWND hWnd, DWORD dwRemove, DWORD dwAdd, UINT nFlags)
{
	return _ModifyStyle(hWnd, GWL_STYLE, dwRemove, dwAdd, nFlags);
}

bool ModifyStyleEx(HWND hWnd, DWORD dwRemove, DWORD dwAdd, UINT nFlags)
{
	return _ModifyStyle(hWnd, GWL_EXSTYLE, dwRemove, dwAdd, nFlags);
}

