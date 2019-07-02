// Header for the HTTP functions

class datastruct {
public:
	datastruct();
	~datastruct();

	char *szLocalPath;
	char *szRemotePath;
	char *szParentWnd;
	void (*pFunc)(char *szFilename, int nResult, char *szParent);
};

bool StartFileDownload(char *szLocalPath, char *szRemotePath, void (*pFunc)(char *szFilename, int nResult, char *szParent), char *szParent);
void GetHTTPFile(void *pData);
int IsHTTPRunning();

