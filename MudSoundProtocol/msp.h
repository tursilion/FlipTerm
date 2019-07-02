// Structure to track sessions
#define MAXSESSIONS 10

struct _session {
	HWND wnd;
	bool fActive;
	// Our own data is nicely stored here, too
};

// Global variables
extern char pszCachePath[1024];

// Function definitions for the default functions
int MCPPlugin_Initialize(void *pInfo);
int MCPPlugIn_GetNumberPackages();
const char *MCPPlugin_GetPackageName(int n);
int MCPPlugin_GetPackageLowVersion(int n);
int MCPPlugin_GetPackageHighVersion(int n);
bool MCPPlugin_ProcessString(char *pStr, HWND hWnd, char **pOut);
void MCPPlugin_CloseSession(HWND hWnd);
bool MCPPlugin_WantPlainText();
char *MCPPlugin_ProcessTextString(char *pStr, bool fActive);
void MCPPlugin_About();

// Non-exported helper functions
char *GetArg(char *pStr, char *pName);
char *MakeKey();
void *ProgMalloc(size_t nSize);
int FindSession(HWND hWnd);
int MyAnsiNext(LPCSTR pStr);
