// WorldView.h : header file
//

struct Point {
	Point() {
		x=0; y=0;
	}
	Point(long z) {
		x=(signed short)(z&0xffff);
		y=z>>16;
	}

	long x,y;
};

typedef struct _IMG {
	_IMG() {
		pRGB=NULL;
		pBuf=NULL;
		memset(szFileName, 0, 256);
		memset(szRawFileName, 0, 256);
		memset(szURL, 0, 256);
		memset(szTime, 0, 16);
		nTime=0;
		nStatus=-1;
		pNext=NULL;
	}

	unsigned char *pRGB;		// Pointer to RGB image data (derived from JPG)
	unsigned char *pBuf;		// Pointer to RGB display buffer
	int nWidth;					// Image width
	int nHeight;				// Image height
	int nBufWidth;				// Buffer width
	int nBufHeight;				// Buffer height
	char szFileName[256];		// Image Filename
	char szRawFileName[256];	// Source Image Filename
	char szURL[256];			// URL
	char szTime[16];			// Timestamp
	unsigned long nTime;		// Timestamp for last update
	int nStatus;				// -1=not available, 0=pending, 1=downloading, 2=available
	_IMG *pNext;				// Pointer to next image
} IMAGE;

/////////////////////////////////////////////////////////////////////////////
// CWorldView view

class CWorldView
{
public:
	CWorldView(HWND, int);
	virtual ~CWorldView();

	BITMAPINFO myInfo;
	HWND hWnd;

	bool ForceDraw;
	bool m_Capture;

	Point oldPoint;
	
	char namebuf[128];
	char rawname[128];

	double fMaxScale;
	double m_Zoom;
	int nWindowSize;
	
	int nY;
	int nX;

	int MuckPort;
	char MuckName[256];
	char Address[256];
	IMAGE FirstImg;
	HWND hParent;

	void DrawView();
	void CopyPixel(int sX, int sY, int dX, int dY);
	void SetWindowSize(int n, bool fReset);
	void OpenWindow();
	bool LoadJPG();
	void SetRawTag(char *pszTag);
	void SetWindowTag(char *pszTag);
	void RequestImageLoad(char *szFile, char *szURL, char *szTime);

protected:
	void OnDraw(HDC hDC);
	void OnMouseMove(UINT nFlags, Point point);
	void OnLButtonDown(UINT nFlags, Point point);
	void OnLButtonUp(UINT nFlags, Point point);
	void OnRButtonUp(UINT nFlags, Point point);
	void OnTimer(UINT nIDEvent);

public:
	bool fWrap;
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

/////////////////////////////////////////////////////////////////////////////

