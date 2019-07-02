#include "stdafx.h"
#include "csmartsocket.h"
#include "gmud32.h"


#define new DEBUG_NEW

CSmartSocket::CSmartSocket()
{
	m_pParent=0;
	m_pRecieveBuf = new char[BUFFSIZE];
	m_pCombineBuf= new char[BUFFSIZE];

	m_pFragBuf = new char[BUFFSIZE];
	*m_pFragBuf=*m_pRecieveBuf=*m_pCombineBuf=0;
	m_bConnected=FALSE;
	m_bPaused=FALSE;
}

CSmartSocket::~CSmartSocket()
{
	delete m_pRecieveBuf;
	delete m_pCombineBuf;
	delete m_pFragBuf;
}

CSmartSocket::SetParentWnd(CWnd *pParent)
{
	m_pParent=pParent;
	return TRUE;
}

BOOL __cdecl CSmartSocket::Printf(LPSTR format, ...)
{
	ASSERT_VALID(this);
	
	if(!m_bConnected)
		return FALSE;
	// do the sprintf stuff
	LPSTR buffer = new char[BUFFSIZE];

	va_list marker;
	va_start(marker,format);
	vsprintf(buffer,format,marker);
	va_end(marker);

	// add it to our output buffer	
	m_asOutBuffer.Add(buffer);
	delete buffer;
	AsyncSelect(FD_READ|FD_WRITE|FD_CLOSE);
	return TRUE;
}

void CSmartSocket::OnConnect(int nErrorCode)
{
	if(!nErrorCode)
		m_bConnected=TRUE;
	m_pParent->SendMessage(WM_SOCKET_CONNECTED,nErrorCode);
	m_bPaused=FALSE;
}

void CSmartSocket::OnSend(int nErrorCode )
{
	if(nErrorCode)
		TRACE("\nError in send: (%d) - %s.",nErrorCode,SockerrToString(nErrorCode));

	if(m_asOutBuffer.GetSize())
	{
		if(Send(m_asOutBuffer[0],m_asOutBuffer[0].GetLength())==SOCKET_ERROR)
			TRACE("\nError in send function.");
		else
			m_asOutBuffer.RemoveAt(0);
	}
}

void CSmartSocket::OnClose(int nErrorCode)
{
	m_bConnected=FALSE;
	Close();
	m_pParent->PostMessage(WM_SOCKET_DISCONNECTED,nErrorCode);
	TRACE("\n\nOnClose Called.");
	Pause(FALSE);
}

BOOL CSmartSocket::HardClose()
{
	TRACE("\nSocket thinks it's connected? (%d)",m_bConnected);
	m_bConnected=FALSE;
    LINGER linger;
    if( m_hSocket == INVALID_SOCKET )
    {
        //  Ignore invalid sockets.
		TRACE("\nHard Close Aborted, not a valid socket");
        return FALSE;
    }
    linger.l_onoff  = TRUE;
    linger.l_linger = 0;
    SetSockOpt(SO_LINGER,(CHAR FAR *)&linger,sizeof(linger) );

    //  Close the socket.
	TRACE("\nHard Close Performed.");
    Close();
	m_hSocket = INVALID_SOCKET;
	m_pParent->SendMessage(WM_SOCKET_DISCONNECTED);
	return TRUE;
}

BOOL CSmartSocket::IsConnected()
{
	return m_bConnected;
}

// over-ridden functions
void CSmartSocket::OnReceive(int nErrorCode )
{
	if(m_bPaused)
		return;
	static int recursion = -1;
	recursion ++;
	if(recursion)
	{
		TRACE("\nRecursion: %d",recursion);
	}
	ASSERT_VALID(this);
	if(nErrorCode)
		TRACE("\nError in receive: (%d) - %s.",nErrorCode,SockerrToString(nErrorCode));

	LPSTR pBuf = new char [BUFFSIZE];
	LPSTR pAdjBuf = pBuf;				// adjusted buffer for frag merging.
	if(*m_pFragBuf)	// special routine to avoid sending out incomplete ansi sequences.
	{
		strcpy(pBuf,m_pFragBuf);	// first copy the frag
		*m_pFragBuf=0;
		pAdjBuf+=strlen(pBuf);
	}
	int retcode=1;
	retcode = Receive(pAdjBuf,BUFFSIZE-50);
	if(retcode>0 && retcode !=SOCKET_ERROR)
	{
		pAdjBuf[retcode]=0;	// null terminate it.
		if(GetApp()->m_bAnsi)	// special routine to avoid sending out incomplete ansi sequences.
		{
			// check the combined string for a trailing fragment
			LPSTR pCheck=pAdjBuf+(retcode-1);
			// pCheck should be pointing at the last char...
			while(pCheck>pBuf)
			{

				if(isdigit(*pCheck) && pCheck>pBuf)
					pCheck--;
				if(isdigit(*pCheck)&& pCheck>pBuf)
					pCheck--;
				if(*pCheck==';'&& pCheck>pBuf)
					pCheck--;
				else if(*pCheck=='['&& pCheck>pBuf)
					pCheck--;
				else
					break;
			}
			if(*pCheck==27) // we have a fragment
			{
				strcpy(m_pFragBuf,pCheck);	// copy it for storage
				*pCheck=0;					// truncate the string
			}
			// okay we're now ready to send the ansi-complete string.
			AfxCheckMemory();
			m_pParent->PostMessage(WM_SOCKET_STRING_RECIEVED,0,(long)pBuf);
		}
		else
			m_pParent->PostMessage(WM_SOCKET_STRING_RECIEVED,0,(long)pBuf);
	}
	else
	{
		delete pBuf;
	}

	recursion--;
}

CSmartSocket::Pause( BOOL bPause)
{
	if(bPause)
	{
		m_bPaused=TRUE;
	}
	else
	{
		m_bPaused=FALSE;
		OnReceive(0);
	}

	return bPause;
}

LPSTR CSmartSocket::SockerrToString( UINT serr )
{
    switch( serr )
    {
	    case WSAENAMETOOLONG :
	        return "Name too long";

	    case WSANOTINITIALISED :
	        return "Not initialized";

	    case WSASYSNOTREADY :
	        return "System not ready";

	    case WSAVERNOTSUPPORTED :
	        return "Version is not supported";

	    case WSAESHUTDOWN :
	        return "Can't send after socket shutdown";

	    case WSAEINTR :
	        return "Interrupted system call";

	    case WSAHOST_NOT_FOUND :
	        return "Host not found";

	    case WSATRY_AGAIN :
	        return "Try again";

	    case WSANO_RECOVERY :
	        return "Non-recoverable error";

	    case WSANO_DATA :
	        return "Hostname lookup failure";

	    case WSAEBADF :
	        return "Bad file number";

	    case WSAEWOULDBLOCK :
	        return "Operation would block";

	    case WSAEINPROGRESS :
	        return "Operation now in progress";

	    case WSAEALREADY :
	        return "Operation already in progress";

	    case WSAEFAULT :
	        return "Bad address";

	    case WSAEDESTADDRREQ :
	        return "Destination address required";

	    case WSAEMSGSIZE :
	        return "Message too long";

	    case WSAEPFNOSUPPORT :
	        return "Protocol family not supported";

	    case WSAENOTEMPTY :
	        return "Directory not empty";

	    case WSAEPROCLIM :
	        return "EPROCLIM returned";

	    case WSAEUSERS :
	        return "EUSERS returned";

	    case WSAEDQUOT :
	        return "Disk quota exceeded";

	    case WSAESTALE :
	        return "ESTALE returned";

	    case WSAEINVAL :
	        return "Invalid argument";

	    case WSAEMFILE :
	        return "Too many open files";

	    case WSAEACCES :
	        return "Access denied";

	    case WSAELOOP :
	        return "Too many levels of symbolic links";

	    case WSAEREMOTE :
	        return "The object is remote";

	    case WSAENOTSOCK :
	        return "Socket operation on non-socket";

	    case WSAEADDRNOTAVAIL :
	        return "Can't assign requested address";

	    case WSAEADDRINUSE :
	        return "Address already in use";

	    case WSAEAFNOSUPPORT :
	        return "Address family not supported by protocol family";

	    case WSAESOCKTNOSUPPORT :
	        return "Socket type not supported";

	    case WSAEPROTONOSUPPORT :
	        return "Protocol not supported";

	    case WSAENOBUFS :
	        return "No buffer space is supported";

	    case WSAETIMEDOUT :
	        return "Connection timed out";

	    case WSAEISCONN :
	        return "Socket is already connected";

	    case WSAENOTCONN :
	        return "Socket is not connected";

	    case WSAENOPROTOOPT :
	        return "Bad protocol option";

	    case WSAECONNRESET :
	        return "Connection reset by peer";

	    case WSAECONNABORTED :
	        return "Software caused connection abort";

	    case WSAENETDOWN :
	        return "Network is down";

	    case WSAENETRESET :
	        return "Network was reset";

	    case WSAECONNREFUSED :
	        return "Connection refused";

	    case WSAEHOSTDOWN :
	        return "Host is down";

	    case WSAEHOSTUNREACH :
	        return "Host is unreachable";

	    case WSAEPROTOTYPE :
	        return "Protocol is wrong type for socket";

	    case WSAEOPNOTSUPP :
	        return "Operation not supported on socket";

	    case WSAENETUNREACH :
	        return "ICMP network unreachable";

	    case WSAETOOMANYREFS :
	        return "Too many references";

	    default :
	        return "Unknown";
    }
}
