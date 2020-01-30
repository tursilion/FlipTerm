
#ifndef SMART_SOCKET_DEFINED
#define SMART_SOCKET_DEFINED

// Open SSL stuff
#include <openssl/ssl.h>
#include <openssl/err.h>


class CSmartSocket : public CAsyncSocket
{
	public:
		CSmartSocket();
		~CSmartSocket();
	    void CSmartSocket::Notify(LPSTR format, ...);
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
        void wrapClose();
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

        // SSL member data
        bool m_bUseSSL;
        SSL_CTX * ctx;
        SSL * ssl;
        X509* scert;
        const SSL_METHOD *meth;

};

#define WM_SOCKET_BASE WM_USER+1234

#define WM_SOCKET_DISCONNECTED		WM_SOCKET_BASE
#define WM_SOCKET_CONNECTED			WM_SOCKET_BASE+1
#define WM_SOCKET_STRING_RECIEVED	WM_SOCKET_BASE+2
#define WM_ASYNCH_GETHOST_COMPLETE	WM_SOCKET_BASE+3



#endif // SMART_SOCKET_DEFINED
