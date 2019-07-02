// WorldView.cpp : implementation file
//
// The test images seem to work fairly well, and are 1120x420 (2.66:1)

#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "WorldView.h"
#include "3dplugin.h"
#include "..\..\djpeg\inc\jpegdecoder.h"
#include "httpclient.h"

#define WorldTimer 1

extern HANDLE hInstance;
extern struct _session Sessions[];		// max sessions. You're free to use a dynamic system to permit more
extern char pszRegKey[1024];

// Find a specified view 
CWorldView *FindView(HWND hWnd) {
	int nSession=0;
	while (nSession < 10) {		// MAXSESSIONS
		if ((Sessions[nSession].pView)&&(Sessions[nSession].pView->hWnd  == hWnd)) {
			return Sessions[nSession].pView;
		}
		nSession++;
	}
	
	return NULL;
}

// Find a view by parent window name. If result is -1, szFilename contains a tag for
// status update. If not 200, it's a failure code. 200 means OK
void DownloadStatusWrapper(char *szFilename, int nResult, char *szParent) {
	CWorldView *pView=NULL;
	int nSession=0;

	// first, find the view
	while (nSession < 10) {		// MAXSESSIONS
		if ((Sessions[nSession].pView)&&(Sessions[nSession].pView->hParent)) {
			char buf[128];
			::GetWindowText(Sessions[nSession].pView->hParent, buf, 128);
			if (strcmp(szParent, buf)==0) {
				pView=Sessions[nSession].pView;
				break;
			}
		}
		nSession++;
	}
	if (NULL == pView) return;

	// Now check what it is
	if (pView->FirstImg.pRGB != NULL) {
		// well, we aren't waiting for this, so just ignore it
		return;
	}

	if (nResult == 200) {
		strcpy(pView->FirstImg.szFileName, szFilename);
		if (false == pView->LoadJPG()) {
			InvalidateRect(pView->hWnd, NULL, true);
		} else {
			pView->SetWindowTag(pView->rawname);
		}
	} else {
		pView->SetWindowTag(szFilename);
		if (nResult != -1) {
			InvalidateRect(pView->hWnd, NULL, true);
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CWorldView

CWorldView::CWorldView(HWND hInParent=0, int inWindow=1)
{
	WNDCLASS myClass;

	hWnd=NULL;
	hParent=hInParent;

	m_Capture=false;
	m_Zoom=1.0;
	nWindowSize=inWindow;  // 1 or 2
	fWrap=true;
	nX=0;
	nY=0;
	oldPoint.x=-1;
	oldPoint.y=-1;
	fMaxScale=0.12;
	myInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	myInfo.bmiHeader.biWidth=0;
	myInfo.bmiHeader.biHeight=0;
	myInfo.bmiHeader.biPlanes=1;
	myInfo.bmiHeader.biBitCount=24;
	myInfo.bmiHeader.biCompression=BI_RGB;
	myInfo.bmiHeader.biSizeImage=0;
	myInfo.bmiHeader.biXPelsPerMeter=100;
	myInfo.bmiHeader.biYPelsPerMeter=100;
	myInfo.bmiHeader.biClrUsed=0;
	myInfo.bmiHeader.biClrImportant=0;

	MuckPort=0;
	MuckName[0]='\0';
	Address[0]='\0';

	FirstImg.pRGB=NULL;
	FirstImg.nWidth=0;
	FirstImg.nHeight=0;
	FirstImg.pBuf=NULL;
	FirstImg.nBufWidth=0;
	FirstImg.nBufHeight=0;
	FirstImg.szFileName[0]='\0';
	FirstImg.nTime=0;
	FirstImg.nStatus=-1;
	FirstImg.pNext=NULL;
	
	myClass.style=CS_HREDRAW | CS_OWNDC | CS_SAVEBITS | CS_VREDRAW;
	myClass.lpfnWndProc=CWorldView::WindowProc;
	myClass.cbClsExtra=0;
	myClass.cbWndExtra=0;
	myClass.hInstance=(HINSTANCE)hInstance;
	myClass.hIcon=NULL;
	myClass.hCursor=NULL;
	myClass.hbrBackground=NULL;
	myClass.lpszMenuName=NULL;
	myClass.lpszClassName="FlipTerm3DWindow1.0";

	RegisterClass(&myClass);

	SetRawTag("");

	OpenWindow();
}

CWorldView::~CWorldView()
{
	if (hWnd) ::SendMessage(hWnd, WM_CLOSE, 0, 0);
}

void CWorldView::OpenWindow() {
	RECT myRect;

	myRect.left=0;
	myRect.right=0;
	myRect.right=400;
	myRect.bottom=120;

	if (hParent) {
		char pstr[128];
		HKEY hKey;
		
		// default position
		::GetWindowRect(hParent, &myRect);
		myRect.left=myRect.right-400*nWindowSize;
		myRect.bottom=myRect.top+120*nWindowSize;
		// GetSystemMetrics returns 0 if it fails, so this is safe
		myRect.left-=GetSystemMetrics(SM_CXVSCROLL)+GetSystemMetrics(SM_CXBORDER)*2;
		myRect.top+=GetSystemMetrics(SM_CYBORDER);

		// attempt to read 'real' position
		::GetWindowText(hParent, pstr, 128);
		pstr[125]='\0';

		if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_CURRENT_USER, pszRegKey, 0, "", REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL)) {
			int buf;
			unsigned long bufsize;
			strcat(pstr, "X");
			bufsize=4;
			if (ERROR_SUCCESS == RegQueryValueEx(hKey, pstr, NULL, NULL, (unsigned char*)&buf, &bufsize)) {
				myRect.left=buf;
			}
			pstr[strlen(pstr)-1]='Y';
			bufsize=4;
			if (ERROR_SUCCESS == RegQueryValueEx(hKey, pstr, NULL, NULL, (unsigned char*)&buf, &bufsize)) {
				myRect.top=buf;
			}
			RegCloseKey(hKey);
		}
	}

	hWnd=CreateWindow("FlipTerm3DWindow1.0", namebuf, WS_POPUPWINDOW | WS_CAPTION | WS_VISIBLE | WS_CHILD, 
		myRect.left, myRect.top, 400*nWindowSize, 120*nWindowSize, hParent, NULL, (HINSTANCE)hInstance, NULL);

	if (hParent) {
		::SetFocus(hParent);
	}
}

LRESULT CALLBACK CWorldView::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	// Need to figure out which object this message is for
	CWorldView *pView=FindView(hwnd);

	if (pView) {
		switch (uMsg) {
		case WM_MOUSEMOVE:
			pView->OnMouseMove(wParam, lParam);
			return 0;
			break;

		case WM_LBUTTONDOWN:
			pView->OnLButtonDown(wParam, lParam);
			return 0;
			break;

		case WM_LBUTTONUP:
			pView->OnLButtonUp(wParam, lParam);
			return 0;
			break;

		case WM_RBUTTONUP:
			pView->OnRButtonUp(wParam, lParam);
			return 0;
			break;

		case WM_TIMER:
			pView->OnTimer(wParam);
			return 0;
			break;

		case WM_CLOSE:
			// Save the window position
			if (pView->hParent) {
				char pstr[128];
				HKEY hKey;
				RECT myRect;
				
				::GetWindowRect(pView->hWnd, &myRect);
				::GetWindowText(pView->hParent, pstr, 128);
				pstr[125]='\0';

				if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_CURRENT_USER, pszRegKey, 0, "", REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL)) {
					strcat(pstr, "X");
					RegSetValueEx(hKey, pstr, NULL, REG_DWORD, (unsigned char*)&myRect.left, sizeof(int));
					pstr[strlen(pstr)-1]='Y';
					RegSetValueEx(hKey, pstr, NULL, REG_DWORD, (unsigned char*)&myRect.top, sizeof(int));
					RegCloseKey(hKey);
				}
			}
			pView->hWnd=NULL;
			break;	// still do the default thing

		case WM_PAINT:
			pView->OnDraw(0);
			break;
		}
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

/////////////////////////////////////////////////////////////////////////////
// CWorldView drawing

void CWorldView::OnDraw(HDC /*hDC*/)
{
	RECT myRect;
	HDC dc;
	HPEN pen, oldpen;
	
	dc=::GetDC(hWnd);
	::GetClientRect(hWnd, &myRect);

	if (NULL == FirstImg.pBuf) {
		::FillRect(dc, &myRect, GetSysColorBrush(COLOR_WINDOWTEXT));
		pen=CreatePen(PS_SOLID, 1, RGB(255,255,255));
		oldpen=(HPEN)::SelectObject(dc, pen);
		for (int idx=0; idx<myRect.right; idx+=myRect.right/10) {
			::MoveToEx(dc, idx, 0, NULL);
			::LineTo(dc, idx, myRect.bottom);
		}
		for (int idx=0; idx<myRect.bottom; idx+=myRect.bottom/5) {
			::MoveToEx(dc, 0, idx, NULL);
			::LineTo(dc, myRect.right, idx);
		}
		if (IsHTTPRunning()) {
			::DrawText(dc, "Requesting image from server...", 28, &myRect, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		} else {
			::DrawText(dc, "No Image is currently loaded", 28, &myRect, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		}
		::SelectObject(dc, oldpen);
		return;
	}

	::StretchDIBits(dc, 0, 0, myRect.right-myRect.left, myRect.bottom-myRect.top,
		0, 0, FirstImg.nBufWidth, FirstImg.nBufHeight, FirstImg.pBuf,
		&myInfo, DIB_RGB_COLORS, SRCCOPY);

	::ReleaseDC(hWnd, dc);
}

/////////////////////////////////////////////////////////////////////////////
// CWorldView message handlers

void CWorldView::OnMouseMove(UINT nFlags, Point point) 
{
	if (oldPoint.x == -1) {
		oldPoint=point;
		return;
	}

	if (m_Capture) {
		int xc, yc;
	
		xc=FirstImg.nWidth;
		yc=FirstImg.nHeight;

		nX+=(point.x-oldPoint.x);
		nY+=(point.y-oldPoint.y);

		// X only - Y is taken care of in the draw routine as it doesn't wrap
		if (fWrap) {
			if (nX > xc) nX-=xc;
			if (nX<0) nX+=xc;
		}
	} else {
		if (nFlags & MK_CONTROL) {
			m_Zoom+=0.01;
			if (m_Zoom > 1.0) m_Zoom=1.0;
			ForceDraw=true;
		}
		if (nFlags & MK_SHIFT) {
			m_Zoom-=0.01;
			if (m_Zoom < 0.5) m_Zoom=0.5;
			ForceDraw=true;
		}
	}

	oldPoint=point;
}

void CWorldView::OnLButtonDown(UINT nFlags, Point point) 
{
	RECT myRect;
	POINT tp;

	m_Capture=true;

	SetCursor(LoadCursor(NULL, IDC_SIZEALL));

	::SetCapture(hWnd);

	::GetClientRect(hWnd, &myRect);
	tp.x=0;
	tp.y=0;
	ClientToScreen(hWnd, &tp);
	myRect.bottom+=tp.y;
	myRect.top+=tp.y;
	myRect.left+=tp.x;
	myRect.right+=tp.x;
}

void CWorldView::OnLButtonUp(UINT nFlags, Point point) 
{
	m_Capture=false;
	
	::ReleaseCapture();

	oldPoint.x=-1;

	SetCursor(LoadCursor(NULL, IDC_ARROW));

	if (hParent) {
		::SetFocus(hParent);
	}
}

void CWorldView::OnRButtonUp(UINT nFlags, Point point) 
{
// Only do this behaviour in debug builds
#ifdef _DEBUG
	OPENFILENAME fn;
	char szBuf[512]="\0";

	// Get a new JPG image loaded
	memset(&fn, 0, sizeof(fn));
	fn.lStructSize=sizeof(fn);
	fn.hwndOwner=hWnd;
	fn.hInstance=NULL;
	fn.lpstrFilter="JPG Files\0*.jpg\0\0";
	fn.lpstrCustomFilter=NULL;
	fn.nMaxCustFilter=0;
	fn.nFilterIndex=1;
	fn.lpstrFile=szBuf;
	fn.nMaxFile=511;
	fn.lpstrInitialDir=NULL;
	fn.lpstrTitle="Select JPG image to open.";
	fn.Flags=OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	fn.lpstrDefExt="jpg";
	
	if (FirstImg.pRGB) {
		free(FirstImg.pRGB);
		FirstImg.pRGB=NULL;
	}
	if (FirstImg.pBuf) {
		free(FirstImg.pBuf);
		FirstImg.pBuf=NULL;
	}

	if (GetOpenFileName(&fn)) {
		strncpy(FirstImg.szFileName, szBuf, 256);
		FirstImg.szFileName[255]='\0';

		if (!LoadJPG()) {
			MessageBox(hWnd, "Failed to load image - try selecting another.", "Load failed", MB_OK);
			InvalidateRect(hWnd, NULL, true);
		}

	} else {
		sprintf(szBuf, "GetOpenFileName failed error code %d", CommDlgExtendedError());
		OutputDebugString(szBuf);
	}

	::SetTimer(hWnd, WorldTimer, 50, NULL);		// this is how often we refresh the view buffer
#endif
}

void CWorldView::OnTimer(UINT nIDEvent) 
{
	switch (nIDEvent) {
	case WorldTimer:
		// Redraw the buffer
		DrawView();
		break;
	}
}

void CWorldView::CopyPixel(int sX, int sY, int dX, int dY)
{
	int sW, sH, dW, dH;
	unsigned char *src, *dest;

	// Get the sizes
	sW=FirstImg.nWidth;
	sH=FirstImg.nHeight;
	dW=FirstImg.nBufWidth;
	dH=FirstImg.nBufHeight;

	// wrap around the pixels (one level only!)
	if (dY >= dH) dY-=dH;
	if (sX >= sW) sX-=sW;
	if (sY >= sH) sY-=sH;
	if (dX >= dW) {
		if (!fWrap) {
			dest=FirstImg.pBuf+3*(dY * dW + dX);
			*(dest+2)=0;		// Red
			*(dest+1)=0;		// Green
			*dest=0;			// Blue (no point incrementing, we're done)
			return;
		}
		dX-=dW;
	}

	src=FirstImg.pRGB+3*(sY * sW + sX);
	dest=FirstImg.pBuf+3*(dY * dW + dX);

	// copy 3 bytes
	*(dest+2)=*(src++);		// Red
	*(dest+1)=*(src++);		// Green
	*dest=*src;				// Blue (no point incrementing, we're done)
}

// You see the problem in here, though?
// The *edges* are 1:1, and the center of the
// image is compressed. The opposite effect is
// desired - or does it matter? When you zoom in it will affect the scale..
void CWorldView::DrawView()
{
	int x,y, xc, yc, zc;
	double sourcex, sourcey, sc;	// sourcex, sourcey, scale
	double st, cnt;					// step, count
	static int oldX=-1, oldY=-1;

	if ((NULL == FirstImg.pBuf) || (NULL == FirstImg.pRGB)) {
		return;
	}

	if ((!ForceDraw)&&(oldX!=-1)) {
		if ((oldX==nX) && (oldY==nY)) return;
	}

	// get buffer size
	xc=FirstImg.nBufWidth;
	yc=FirstImg.nBufHeight;
	zc=FirstImg.nHeight;

	// Fix Y address to avoid wraparound
	if (nY+yc > (zc*98)/100) {		// 98% of image to prevent wraparound
		nY=(zc*98)/100-yc;
	}
	if (nY<0) {
		nY=0;
	}

	if (!fWrap) {
		if (nX<0) nX=0;
		if (nX > ((FirstImg.nWidth - xc)*98)/100) {
			nX=((FirstImg.nWidth - xc)*98)/100 - 1;
		}
	}

	// Draw with a curve - ported from the VB code
	x=0;
	sc=1.0;
	sourcex=nX;
	sourcey=nY;
	cnt=0;
	st=3.14152 / xc;

	while (x<xc) {
		for (y=0; y<yc; y++) {
			CopyPixel((int)sourcex, (int)sourcey, x, y);
			sourcey+=sc;
		}
		sourcex+=sc;
		sourcey=nY;
		cnt+=st;
		sc=(1+(sin(cnt) * fMaxScale))*m_Zoom;
		x++;
	}

	// Set flags
	oldX=nX;
	oldY=nY;
	ForceDraw=false;

	// Force the window to redraw
	::InvalidateRect(hWnd, NULL, false);
}

void CWorldView::SetWindowSize(int n, bool fReset=false) {
	// 1 or 2x only
	HKEY hKey;
	char pstr[128];

	if (this == NULL) return;
	if (hWnd == NULL) return;

	if (n!=2) {
		nWindowSize=1;
	} else {
		nWindowSize=2;
	}

	SendMessage(hWnd, WM_CLOSE, 0, 0);

	if (fReset) {
		::GetWindowText(hParent, pstr, 128);
		pstr[125]='\0';
		// Delete the registry setting
		if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, pszRegKey, 0, KEY_ALL_ACCESS, &hKey)) {
			strcat(pstr, "X");
			RegDeleteValue(hKey, pstr);
			pstr[strlen(pstr)-1]='Y';
			RegDeleteValue(hKey, pstr);
			RegCloseKey(hKey);
		}
	}

	hWnd=NULL;

	OpenWindow();

	if (!LoadJPG()) {
		InvalidateRect(hWnd, NULL, true);
	}
}

bool CWorldView::LoadJPG() {

	if (FirstImg.pRGB) {
		free(FirstImg.pRGB);
		FirstImg.pRGB=NULL;
	}
	if (FirstImg.pBuf) {
		free(FirstImg.pBuf);
		FirstImg.pBuf=NULL;
	}

	FirstImg.nStatus=2;				// available (not using the other modes yet)
	FirstImg.nTime=0;				// not using yet
	FirstImg.pNext=NULL;

	if (strlen(FirstImg.szFileName)<1) return true;	// no image to load

	// JpegDecoder
	int lines_decoded = 0;
	Pjpeg_decoder Pd=NULL;

	Pjpeg_decoder_file_stream Pinput_stream = new jpeg_decoder_file_stream();
	if (Pinput_stream->open(FirstImg.szFileName))	{
		delete Pinput_stream;
		SetWindowTag("Image not loaded.");
		return false;
	}
	Pd = new jpeg_decoder(Pinput_stream, false);	// false = no MMX (we don't know if it's there)
	if (Pd->get_error_code() != 0) {
		SetWindowTag("Error: Decoder failed");
		// Always be sure to delete the input stream object _after_
		// the decoder is deleted. Reason: the decoder object calls the input
		// stream's detach() method.
		delete Pd;
		delete Pinput_stream;
		return false;
	}
	if (Pd->begin()) {
		SetWindowTag("Error: Decoder failed");
		delete Pd;
		delete Pinput_stream;
		return false;
	}
	FirstImg.nWidth=Pd->get_width();
	FirstImg.nHeight=Pd->get_height();
	FirstImg.pRGB = (uchar *)malloc(Pd->get_width() * Pd->get_height() * 3);
	if (!FirstImg.pRGB) {
		SetWindowTag("Error: Out of memory!");
		delete Pd;
		delete Pinput_stream;
		return false;
	}
	if (Pd->get_num_components() == 3) {
		OutputDebugString("Reading color image...\n");							// 24 bit (read as 32bit lines)
		for ( ; ; ) {
			char *Pscan_line_ofs;
			uint scan_line_len;
			if (Pd->decode((void**)&Pscan_line_ofs, &scan_line_len)) break;
			for (uint idx=0, idx2=0; idx2<scan_line_len; idx+=3, idx2+=4) {
				*(FirstImg.pRGB+(lines_decoded*Pd->get_width()*3)+idx)=*(Pscan_line_ofs+idx2);
				*(FirstImg.pRGB+(lines_decoded*Pd->get_width()*3)+idx+1)=*(Pscan_line_ofs+idx2+1);
				*(FirstImg.pRGB+(lines_decoded*Pd->get_width()*3)+idx+2)=*(Pscan_line_ofs+idx2+2);
			}
			lines_decoded++;
		}
	} else {
		// greyscale
		OutputDebugString("Reading greyscale image...\n");						// Only 8 bit, but we extend it
		for ( ; ; ) {
			char *Pscan_line_ofs;
			uint scan_line_len;
			if (Pd->decode((void**)&Pscan_line_ofs, &scan_line_len)) break;
			for (uint idx=0; idx<scan_line_len; idx++) {
				*(FirstImg.pRGB+(lines_decoded*Pd->get_width()*3)+idx*3)=*(Pscan_line_ofs+idx);
				*(FirstImg.pRGB+(lines_decoded*Pd->get_width()*3)+idx*3+1)=*(Pscan_line_ofs+idx);
				*(FirstImg.pRGB+(lines_decoded*Pd->get_width()*3)+idx*3+2)=*(Pscan_line_ofs+idx);
			}
			lines_decoded++;
		}
	}
	delete Pd;
	delete Pinput_stream;
	// End JPG decoder

	if (NULL != FirstImg.pRGB) {
		// Now we have an image, set up the frame buffer
		// The buffer is 1/6 horizontal of the image (60 degree), and
		// 1/5 vertical (which, assuming it's 90 degrees, is 18 degrees)
		FirstImg.nBufWidth=(((FirstImg.nWidth/6)/4)+1)*4; // must be a multiple of 4
		FirstImg.nBufHeight=FirstImg.nHeight/5;
		myInfo.bmiHeader.biWidth=FirstImg.nBufWidth;
		myInfo.bmiHeader.biHeight=-FirstImg.nBufHeight;	// must be negative for top-down
		FirstImg.pBuf=(unsigned char*)malloc(FirstImg.nBufWidth*FirstImg.nBufHeight*3);
		ZeroMemory(FirstImg.pBuf, FirstImg.nBufWidth*FirstImg.nBufHeight*3);
		// And finally, set up the viewpoint to be roughly centered
		nX=(FirstImg.nWidth/2)-(FirstImg.nBufWidth/2);
		nY=(FirstImg.nHeight/2)-(FirstImg.nBufHeight/2);
		ForceDraw=true;
	} else {
		SetWindowTag("Failed to load image");
	}
	
	::SetTimer(hWnd, WorldTimer, 50, NULL);		// this is how often we refresh the view buffer
	return true;
}

void CWorldView::SetRawTag(char *pszTag) {
	strncpy(rawname, pszTag, 128);
	rawname[127]='\0';
	SetWindowTag(pszTag);
}

void CWorldView::SetWindowTag(char *pszTag) {
	char pstr[128];

	strcpy(namebuf, "3D - ");
	if (hParent) {
		::GetWindowText(hParent, pstr, 128);
		strncat(namebuf, pstr, 128-strlen(namebuf));
	}
	namebuf[127]='\0';
	if (strlen(namebuf)<125) strcat(namebuf, " - ");
	strncat(namebuf, pszTag, 128-strlen(namebuf));
	namebuf[127]='\0';
	if (hWnd) {
		SetWindowText(hWnd, namebuf);
	}
}

// Filename, fully qualified URL, and Unix-style timestamp (or 0)
void CWorldView::RequestImageLoad(char *szFile, char *szURL, char *szTime) {
	// Security check, disallow ".." in the path
	char *pTmp;
	FILE *fp;
	char buf[256];

	pTmp=szFile;
	while (*pTmp) {
		if ((*pTmp == '.') && (*(pTmp+1) == '.')) {
			SetWindowTag("Illegal filename attempted");
			if (FirstImg.pRGB) {
				free(FirstImg.pRGB);
				FirstImg.pRGB=NULL;
			}
			if (FirstImg.pBuf) {
				free(FirstImg.pBuf);
				FirstImg.pBuf=NULL;
			}
			InvalidateRect(hWnd, NULL, true);
			return;
		}
		if (*pTmp=='/') *pTmp='\\';
		if (*(pTmp+1)=='/') *(pTmp+1)='\\';
		if ((*pTmp=='\\')&&(*(pTmp+1)=='\\')) {
			// remove double backslashes
			memmove(pTmp, pTmp+1, strlen(pTmp));	// this will include the terminating NULL
			continue;
		}
		pTmp++;
	}

	pTmp=szURL;
	while (*pTmp) {
		if (*pTmp=='\\') *pTmp='/';
		pTmp++;
	}

	if (strlen(szFile) > 0) {
		char tmpdrv[_MAX_DRIVE];
		char tmpdir[_MAX_DIR];
		char tmptmp[_MAX_DRIVE+_MAX_DIR];

		_snprintf(FirstImg.szFileName, 256, "%s\\%s", pszCachePath, szFile);
		FirstImg.szFileName[255]='\0';

		// The filename should contain a relative folder path. We must verify that it exists
		_splitpath(FirstImg.szFileName, tmpdrv, tmpdir, NULL, NULL);
		strcpy(tmptmp, tmpdrv);
		strcat(tmptmp, tmpdir);
		if (!CreateDirectory(tmptmp, NULL)) {
			// This is probably not serious
			DWORD err=GetLastError();
			if (183 != err) {
				sprintf(tmptmp, "Failed to create directory: %d\n", err);
				OutputDebugString(tmptmp);
			}
		}

		// Now we check the database
		_snprintf(buf, 256, "%s\\database.txt", pszCachePath);
		buf[255]='\0';
		fp=fopen(buf, "r+b");
		if (NULL == fp) {
			fp=fopen(buf, "w+b");
		}
		if (fp) {
			int flag=0;

			while (!feof(fp)) {
				fread(buf, 1, 256, fp);
				if (strncmp(buf, szFile, strlen(szFile)) == 0) {
					if (strncmp(&buf[128], szTime, strlen(szTime)) < 0) {
						// remove the old file and update the timestamp
						OutputDebugString("Removing old file...\n");
						DeleteFile(FirstImg.szFileName);
						fseek(fp, -256, SEEK_CUR);
						fprintf(fp, "%-128.128s%-128.128s", szFile, szTime);
						flag=1;
					}
					break;
				}
			}
			if (flag==0) {
				// New line
				fprintf(fp, "%-128.128s%-128.128s", szFile, szTime);
			}
			fclose(fp);
		}
	} else {
		_snprintf(FirstImg.szFileName, 256, "%sNULL.jpg", pszCachePath);
		FirstImg.szFileName[255]='\0';
	}
	FirstImg.szFileName[255]='\0';
	strncpy(FirstImg.szRawFileName, szFile, 256);
	FirstImg.szRawFileName[255]='\0';
	strncpy(FirstImg.szURL, szURL, 256);
	FirstImg.szURL[255]='\0';
	strncpy(FirstImg.szTime, szTime, 16);
	FirstImg.szTime[15]='\0';

	if (NULL == hWnd) return;		// Don't actually do the load if the window is closed

	if (!LoadJPG()) {
		// Failed to load - try to download it
		char buf[128];

		InvalidateRect(hWnd, NULL, true);

		::GetWindowText(hParent, buf, 128);
		StartFileDownload(FirstImg.szFileName, szURL, DownloadStatusWrapper, buf);
	}
}
