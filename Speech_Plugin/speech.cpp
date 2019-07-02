// Speech Plugin - based on the Microsoft Talkback sample
// Just to speak phrases received

// WARNING! This doesn't fully work. It hangs on exit, and I'm not sure
// why. I don't wish to fool around with it any more, so you're on your
// own if YOU want to.


#include <windows.h>
#include <stdio.h>
#include "sphelper.h"
#include "speech.h"

const char PACKAGENAME[] = "Text-to-Speech";	// Tursisoft.com's Speech package
const int lowVERSION = 10;						// Version 1.0 (times 10)
const int highVERSION = 10;						// same

const int specVERSION = 10;		// based on version 1.0 of the spec, you won't normally change this

char pszRegKey[1024];

CComPtr<ISpVoice> cpVoice;

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
			break;

		case DLL_PROCESS_DETACH:
			// This hangs on the stop if active, and after completion if the voice was idle... 
			// how the heck do we clean up speech so we can close? Is it because we're exitting
			// the DLL?
			if (cpVoice.p) {
				OutputDebugString("Stopping...\n");
				cpVoice->Speak(NULL, SPF_PURGEBEFORESPEAK, 0);
				OutputDebugString("Waiting...\n");
				cpVoice->WaitUntilDone(5000);
				OutputDebugString("Uninitializing...\n");
				CoUninitialize();
				OutputDebugString("All done!\n");
			}
			break;
    }
    return TRUE;
}

// Perform initialization and reset the DLL to inactive
int MCPPlugin_Initialize(char *inRegKey) {
	HRESULT hr = E_FAIL;

	for (int idx=0; idx<MAXSESSIONS; idx++) {
		Sessions[idx].fActive=false;
		Sessions[idx].wnd=NULL;
	}

	strncpy(pszRegKey, inRegKey, 1024);
	if (strlen(pszRegKey)<1010) {
		strcat(pszRegKey, "\\speech");		// Set our local config key string
	}

    if (SUCCEEDED(hr = ::CoInitialize(NULL))) {
		cpVoice.CoCreateInstance(CLSID_SpVoice);

		return specVERSION;	// We support version 1.0 (times 10)
	} else {
		return 0;
	}
}

// Inform the client how many packages we host
int MCPPlugin_GetNumberPackages() {
	return 1;			// just 1
}

// Return the name of the packages we support
const char *MCPPlugin_GetPackageName(int n) {
	return PACKAGENAME;
}

// Return the low version of the supported package
int MCPPlugin_GetPackageLowVersion(int n) {
	return 0;			// Don't negotiate, no MCP
}

// Return the high version of the supported package
int MCPPlugin_GetPackageHighVersion(int n) {
	return 0;
}

// We don't want MCP strings
bool MCPPlugin_ProcessString(char *pStr, HWND hWnd, char **pOut) {
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
	unsigned short buf[4096];
	char buf2[4096];
	char RemStr[]="^*#()-_|\\][{}'\";:><`~\t\r\n";
	char *ptmp;
	int idx, tmp;

	if (NULL == pStr) return NULL;
	if (strlen(pStr) < 1) return NULL;

	if (false != fActive) {
		if (cpVoice) {
			// Try to avoid speaking graphics or excessive punctuation
			strncpy(buf2, pStr, 4096);
			buf2[4095]='\0';

			// Strip control codes and ANSI codes
			idx=0;
			while (buf2[idx]) {
				tmp=MyAnsiNext(&buf2[idx]);
				if (tmp == 1) {		// not ANSI
					if ((buf2[idx]<' ')||(buf2[idx]>126)) {
						memmove(&buf2[idx], &buf2[idx+1], 4095-idx);
						continue;
					}
				} else {
					if (tmp>1) {	// is ANSI
						memmove(&buf2[idx], &buf2[idx+tmp], 4096-idx-tmp);
						continue;
					}
				}
				idx++;
			}

			// Strip characters from RemStr
			idx=0;
			while (RemStr[idx]) {
				ptmp=strchr(buf2, RemStr[idx]);
				if (ptmp) {
					memmove(ptmp, ptmp+1, 4095-(ptmp-buf2));
				} else {
					idx++;
				}
			}
			while (ptmp=strstr(buf2, "  ")) {
				memmove(ptmp, ptmp+1, 4095-(ptmp-buf2));
			}
			while ((buf2[0]!='\0')&&(buf2[0] <= ' ')) {
				memmove(buf2, buf2+1, 4095);
			}
			idx=strlen(buf2)-1;
			while ((buf2[idx]!='\0')&&(buf2[idx]<= ' ')) {
				buf2[idx]='\0';
				idx--;
				if (idx<0) break;
			}
			if (strlen(buf2)<1) return NULL;

			// check for 4 of the same char in a row
			idx=0;
			while (idx<(signed)(strlen(buf2)-3)) {
				if ((buf2[idx]==buf2[idx+1])&&(buf2[idx]==buf2[idx+2])&&(buf2[idx]==buf2[idx+3])) {
					// skip this line 
					return NULL;
				}
				idx++;
			}

			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, buf2, -1, buf, 4095);
			cpVoice->Speak(buf, SPF_ASYNC, NULL );
		}
	}

	// We don't want to change the string
	return NULL;
}

// Display about information to the user
void MCPPlugin_About() {
	char szMsg[4096];

	sprintf(szMsg, "Speech (out) Plugin for FlipTerm by Tursi\nhttp://tursi.yiffco.com\nVersion 1.0\n");
	MessageBox(NULL, szMsg, "Speech Plugin", MB_OK);
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