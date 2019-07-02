// Simple HTTP 1.0 GET client

#include <winsock2.h>
#include <windows.h>
#include <process.h>
#include <stdio.h>
#include "httpclient.h"

static int fKillThread=2;		// used to stop a running thread (0 if running, 1 = stop, 2 = not running)

datastruct::datastruct() { 
	szLocalPath=NULL; 
	szRemotePath=NULL;
	szParentWnd=NULL;
	pFunc=NULL; 
}

datastruct::~datastruct() { 
	if (szLocalPath) free(szLocalPath);
	if (szRemotePath) free(szRemotePath);
	if (szParentWnd) free(szParentWnd);
	szLocalPath=NULL; 
	szRemotePath=NULL; 
	pFunc=NULL; 
}

char *mystrdup(char *pszIn) {
	char *tmp;

	tmp=(char*)malloc(strlen(pszIn)+16);
	strcpy(tmp, pszIn);
	return tmp;
}

// Creates a new thread to handle the download
// When the thread exits, it will call the completion function with the raw filename and HTTP result code
bool StartFileDownload(char *szLocalPath, char *szRemotePath, void (*pFunc)(char *szFilename, int nResult, char *szParent), char *szParent) {
	datastruct *pDS;

	pDS=new datastruct;
	pDS->szLocalPath=mystrdup(szLocalPath);
	pDS->szRemotePath=mystrdup(szRemotePath);
	pDS->szParentWnd=mystrdup(szParent);
	pDS->pFunc=pFunc;

	if (-1 == _beginthread(GetHTTPFile, 0, pDS)) {
		delete pDS;
		pFunc(szLocalPath, 408, szParent);
		return false;
	}
	
	return true;
}

int IsHTTPRunning() {
	return !fKillThread;
}

// Abort if fKillThread is set to 1. Set to 2 when stopped
// Does not call the exit function if aborted
void GetHTTPFile(void *pData) {
	datastruct *pDS=(datastruct*)pData;
	unsigned long nRemoteAddr;
	char *pszRemoteNode, *pTmp;
	struct hostent *pHost;
	PROTOENT* pProto;
	sockaddr_in sa;
	SOCKET WWWSocket;
	char OutBuf[4096];
	char szName[1024];
	int nReturn, nCnt;
	int nFoundData;
	FILE *fp;

	pszRemoteNode=NULL;
	fp=NULL;

	if (fKillThread == 0) {
		pDS->pFunc("Ending previous thread...", -1, pDS->szParentWnd);
		fKillThread=1;
		while (fKillThread == 1) {
			Sleep(50);
		}
	}
	fKillThread=0;

	pDS->pFunc("Looking up server...", -1, pDS->szParentWnd);

	// Extract just the address from the URI (which must be http://<server>/path/)
	pTmp=pDS->szRemotePath;
	while ((*pTmp)&&(*(pTmp+1))&&((*pTmp!='/')||(*(pTmp+1)!='/'))) pTmp++;
	if ((*pTmp!='/')||(*(pTmp+1)!='/')) {
		pDS->pFunc("Unable to parse URL.", 406, pDS->szParentWnd);
		goto abort;
	}

	pszRemoteNode=strdup(pTmp+2);

	// Now take till the first '/'
	pTmp=pszRemoteNode;
	while ((*pTmp)&&(*pTmp != '/')) pTmp++;
	if (*pTmp == '/') *pTmp='\0';

	// Now we have an address, attempt to parse it
	nRemoteAddr=inet_addr(pszRemoteNode);
	if (INADDR_NONE == nRemoteAddr) {
		// Attempt to look it up
		pHost=gethostbyname(pszRemoteNode);
		if (NULL == pHost) {
			pDS->pFunc("Failed to locate server", 404, pDS->szParentWnd);
			goto abort;
		}
		nRemoteAddr=*(unsigned long*)(pHost->h_addr_list[0]);
		if ((INADDR_NONE == nRemoteAddr)||(0 == nRemoteAddr)) {
			pDS->pFunc("Failed to locate server", 404, pDS->szParentWnd);
			goto abort;
		}
	}
	sa.sin_addr.S_un.S_addr=nRemoteAddr;
	
	// We should have an address
	if (fKillThread) {
		goto abort;
	}

	pDS->pFunc("Contacting server...", -1, pDS->szParentWnd);
	
	// Get the protocol number for TCP
	pProto = getprotobyname("tcp");
	if (NULL == pProto) {
		pDS->pFunc("Failed to get TCP information.", 500, pDS->szParentWnd);
		goto abort;
	}

	// Create a socket
	WWWSocket = socket(AF_INET, SOCK_STREAM, pProto->p_proto);
	if (INVALID_SOCKET == WWWSocket) {
		pDS->pFunc("Failed to create socket.", 500, pDS->szParentWnd);
		goto abort;
	}

	sa.sin_family=AF_INET;
	sa.sin_port=htons(80);

	if (connect(WWWSocket, (sockaddr*)&sa, sizeof(sa))) {
		pDS->pFunc("Failed to Connect to server", 400, pDS->szParentWnd);
		goto abort;
	}

	if (fKillThread) {
		goto abort;
	}

	// (http 1.1 - more useful as it includes result codes)
	// ->connect on port 80
	// ->GET HTTP://<uri> HTTP/1.1<crlf>
	// -><crlf>
	// <-HTTP/1.1 xxx Text of return code<crlf>
	// <-Possibly more text, may watch for Content-Encoding:<crlf>
	// <-May also watch for 'Content-Length: x' as a clue<crlf>
	// <-<crlf>
	// <-Data
	//
	// Code 200 is OK

	int maxlen, curlen;

	maxlen=-1;
	curlen=-1;

	sprintf(OutBuf, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", pDS->szRemotePath, pszRemoteNode);
	
	if (SOCKET_ERROR == send(WWWSocket, OutBuf, strlen(OutBuf), 0)) {
		pDS->pFunc("Failed to send request to server", 500, pDS->szParentWnd);
		goto abort;
	}

	if (fKillThread) {
		goto abort;
	}

	// Data should now, hopefully, be streaming back. We need to examine the first line
	if (SOCKET_ERROR == (nCnt=recv(WWWSocket, OutBuf, 4095, 0))) {
		pDS->pFunc("Failed to receive data from server", 500, pDS->szParentWnd);
		goto abort;
	}
	OutBuf[nCnt]='\0';

	if (fKillThread) {
		goto abort;
	}

	if (1 != sscanf(OutBuf, "HTTP/%*d.%*d %d ", &nReturn)) {
		pDS->pFunc("Failed to parse reply from server", 400, pDS->szParentWnd);
		goto abort;
	}

	if (nReturn != 200) {
		sprintf(OutBuf, "Server returned failure (code %d)", nReturn);
		pDS->pFunc(OutBuf, nReturn, pDS->szParentWnd);
		goto abort;
	}

	_snprintf(szName, 1024, "%s", pDS->szLocalPath);
	szName[1023]='\0';
	fp=fopen(szName, "wb");
	if (NULL == fp) {
		pDS->pFunc("Failed to open local file", 500, pDS->szParentWnd);
		goto abort;
	}

	pDS->pFunc("Receiving file...", -1, pDS->szParentWnd);

	nFoundData=0;

	for (;;) {
		if (fKillThread) {
			goto abort;
		}

		if (0 == nFoundData) {
			pTmp=strstr(OutBuf, "Content-Length:");
			if (pTmp) {
				maxlen=atoi(pTmp+15);
				curlen=0;
			}
			pTmp=strstr(OutBuf, "\r\n\r\n");
			if (pTmp) {
				pTmp+=4;
				fwrite(pTmp, nCnt-(pTmp-OutBuf), 1, fp);
				if (-1 != maxlen) {
					curlen+=nCnt-(pTmp-OutBuf);
				}
				nFoundData=1;
			}
		} else {
			fwrite(OutBuf, nCnt, 1, fp);
			if (-1 != maxlen) {
				curlen+=nCnt;
				sprintf(OutBuf, "Receiving: %d/%d bytes", curlen, maxlen);
				pDS->pFunc(OutBuf, -1, pDS->szParentWnd);
				if (curlen == maxlen) {
					closesocket(WWWSocket);
				}
			}
		}

		if (SOCKET_ERROR == (nCnt=recv(WWWSocket, OutBuf, 4095, 0))) {
			// Here we'll assume connect reset, and we have all the data
			break;
		}
		OutBuf[nCnt]='\0';
		if (0 == nCnt) break;
	}
	fclose(fp);
	fp=NULL;

	pDS->pFunc(pDS->szLocalPath, 200, pDS->szParentWnd);

abort:
	// We think we're done
	if (fp) fclose(fp);
	if (fKillThread == 1) {
		// we were aborted, delete the file
		OutputDebugString("Thread aborted... deleting output file.\n");
		DeleteFile(szName);
	}
	closesocket(WWWSocket);
	delete pDS;
	if (pszRemoteNode) free(pszRemoteNode);
	fKillThread=2;
	return;
}

