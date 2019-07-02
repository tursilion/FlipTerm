
#ifndef SMART_SOCKET_DEFINED
#define SMART_SOCKET_DEFINED


class CSmartSocket : public CAsyncSocket
{
	public:
		CSmartSocket();
		~CSmartSocket();
			BOOL __cdecl Printf(LPSTR format, ...);
		int SetParentWnd(CWnd *pParent);

		BOOL IsConnected();

		// over-rides
		void OnReceive(int nErrorCode);
		void OnConnect(int nErrorCode);
		void OnSend(int nErrorCode);
		void OnClose(int nErrorCode);
		
		//
		int Pause( BOOL bPause);
		BOOL HardClose();
		LPSTR SockerrToString( UINT serr );
		CWnd *m_pParent;
		CStringArray m_asOutBuffer;
		LPSTR m_pRecieveBuf;
		LPSTR m_pCombineBuf;
		LPSTR m_pFragBuf;
		BOOL m_bConnected;
		BOOL m_bPaused;

		CString m_LastNegotiated;
};

#define WM_SOCKET_BASE WM_USER+1234

#define WM_SOCKET_DISCONNECTED		WM_SOCKET_BASE
#define WM_SOCKET_CONNECTED			WM_SOCKET_BASE+1
#define WM_SOCKET_STRING_RECIEVED	WM_SOCKET_BASE+2
#define WM_ASYNCH_GETHOST_COMPLETE	WM_SOCKET_BASE+3



#endif // SMART_SOCKET_DEFINED
