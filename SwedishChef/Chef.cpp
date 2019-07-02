// Chef.cpp - Swedish Chef output filter
// This is a total abuse of the plugin system. But what the heck. ;)
// When on, it will only "Encheferize" the active window :)

#include <windows.h>
#include "chef.h"
#include <stdio.h>

#define iswordchar(x) ((isalpha(x))||((x)=='\''))

const char PACKAGENAME[] = "Swedish Chef";		// Swedish Chef - Simple names are okay for non-MCP plugins
const int lowVERSION = 10;						// Version 1.0 (times 10)
const int highVERSION = 10;						// same

const int specVERSION = 10;		// based on version 1.0 of the spec, you won't normally change this

bool fOn=false;					// defaults to off

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
		strcat(pszRegKey, "\\SwedishChef");		// Set our local config key string
	}

	return specVERSION;	// We support version 1.0 (times 10)
}

// Inform the client how many packages we host
int MCPPlugin_GetNumberPackages() {
	return 1;			// we must export 1 to show up in the about list!
}

// Return the name of the packages we support
const char *MCPPlugin_GetPackageName(int n) {
	return PACKAGENAME;	// we must export our name
}

// Return the low version of the supported package
int MCPPlugin_GetPackageLowVersion(int n) {
	return 0;			// But we'll always report version 0 so we don't negotiate
}

// Return the high version of the supported package
int MCPPlugin_GetPackageHighVersion(int n) {
	return 0;			// But we'll always report version 0 so we don't negotiate
}

// Process the received string, verifying that it's ours
bool MCPPlugin_ProcessString(char *pStr, HWND hWnd, char **pOut) {
	// this plugin doesn't do MCP
	return false;
}

// A session has closed and may be removed from the list
void MCPPlugin_CloseSession(HWND hWnd) {
}

// Indicate whether we want to see all strings or not
bool MCPPlugin_WantPlainText() {
	return true;
}

// Helper for the translation
#define UpdateStr(Orig, New) {					\
	if (strncmp(pIn, Orig, strlen(Orig))==0) {	\
		inword=1;								\
		if (nLen>=(signed)strlen(Orig)) {		\
			strcat(OutBuf, New);				\
		}										\
		nLen-=strlen(New);						\
		pIn+=strlen(Orig);						\
		continue;								\
	}											\
}												\

// Process non-MCP strings. This function MUST be exported, even if unused.
// Note that the received string may contain ANSI and control codes!
// These should normally be preserved when modifying the string.
// (We don't worry about ANSI here - the Chef won't recognize the codes
// and shouldn't touch them.)
char *MCPPlugin_ProcessTextString(char *pStr, bool fActive) {
	char *OutBuf, *pIn, *pOut;
	int nLen;
	int i_seen=0, inword=0;

	if (!fOn) return NULL;

	if (NULL == pStr) return NULL;
	nLen=strlen(pStr);
	if (nLen<1) return NULL;

	nLen*=3;
	if (nLen>16383) nLen=16383;			// Max string length is 16383 chars (pre-colorization)
	OutBuf=(char*)ProgMalloc(nLen+1);	// Worst case, every char becomes 3, plus terminator
	if (NULL == OutBuf) {
		return NULL;	// Your DLLs may want to be more careful about it :)
	}
	ZeroMemory(OutBuf, nLen+1);
	pIn=pStr;							
	pOut=OutBuf;

	while ((nLen>0)&&(*pIn)) {
		// Brute force approach, based on the Encheferizer by John Hagerman

		if ((!isalpha(*pIn))&&((*pIn)!='\'')) {
			i_seen=0;
			inword=0;
		}

		// Remember that 'UpdateStr' includes a 'continue' if it succeeds

		UpdateStr("an", "un");
		UpdateStr("An", "Un");
		UpdateStr("au", "oo");
		UpdateStr("Au", "oo");

		if ((*pIn == 'a') && (iswordchar(*(pIn+1)))) {
			UpdateStr("a", "e");
		}

		if ((*pIn == 'A') && (iswordchar(*(pIn+1)))) {
			UpdateStr("A", "E");
		}

		if (strncmp(pIn, "en", 2)==0) {
			if (!iswordchar(*(pIn+2))) {
				UpdateStr("en", "ee");
			}
		}

		if (inword) {
			UpdateStr("ew", "oo");
		}

		if (inword) {
			if ((*pIn == 'e') && (!iswordchar(*(pIn+1)))) {
				UpdateStr("e", "e-a");
			}
		}

		if (!inword) {
			UpdateStr("e", "i");
			UpdateStr("E", "I");
		}

		if (inword) {
			UpdateStr("f", "ff");
			UpdateStr("ir", "ur");
			if (*pIn == 'i') {
				if (i_seen++) {
					UpdateStr("i", "i");
				} else {
					UpdateStr("i", "ee");
				}
			}
			UpdateStr("ow", "oo");
		}

		if (!inword) {
			UpdateStr("o", "oo");
			UpdateStr("O", "Oo");
		}

		if (inword) {
			UpdateStr("o", "u");
		}

		UpdateStr("the", "zee");
		UpdateStr("The", "Zee");
		
		if ((*pIn == 't') && (*(pIn+1) == 'h') && (!iswordchar(*(pIn+2)))) {
			UpdateStr("th", "t");
		}

		if (inword) {
			UpdateStr("tion", "shun");
			UpdateStr("u", "oo");
			UpdateStr("U", "Oo");
		}

		UpdateStr("v", "f");

		UpdateStr("V", "F");
		UpdateStr("w", "v");
		UpdateStr("W", "V");
		UpdateStr(".", ".");

		// No matches? Just copy it then
		// This nicely preserves all the codes for us
		pOut=OutBuf;
		while (*pOut) pOut++;
		*(pOut++)=*(pIn++);
		*pOut='\0';
		nLen--;
	}
	return OutBuf;
}

// Display about information to the user
void MCPPlugin_About() {
	char szMsg[4096];
	int ret;

	sprintf(szMsg, "Sample Plugin for FlipTerm [CHEF] by Tursi\nhttp://tursi.yiffco.com\nBased on the Encheferizer by John Hagerman\nVersion 1.1\n\nPress YES to activate the Chef or NO to turn him off.");
	ret=MessageBox(NULL, szMsg, "Swedish Chef Plugin", MB_OK|MB_YESNO);
	if (IDYES == ret) {
		fOn=true;
	} else {
		fOn=false;
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