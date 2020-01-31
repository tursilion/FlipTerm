#include "stdafx.h"
#include "csmartsocket.h"
#include "gmud32.h"


CSmartSocket::CSmartSocket()
{
	m_pParent=0;
	m_pRecieveBuf = new char[BUFFSIZE];
	m_pCombineBuf= new char[BUFFSIZE];

	m_pFragBuf = new char[BUFFSIZE];
	*m_pFragBuf=*m_pRecieveBuf=*m_pCombineBuf=0;
	m_bConnected=FALSE;
	m_bPaused=FALSE;
	m_LastNegotiated="";

    // SSL settings
    m_bUseSSL = false;
    ctx = NULL;
    ssl = NULL;
    scert = NULL;
    meth = NULL;
    m_encryption = "";
    ERR_load_crypto_strings();
    SSL_load_error_strings(); // just once
}

CSmartSocket::~CSmartSocket()
{
	HardClose();
	delete m_pRecieveBuf;
	delete m_pCombineBuf;
	delete m_pFragBuf;
}

int CSmartSocket::SetParentWnd(CWnd *pParent)
{
	m_pParent=pParent;
	return TRUE;
}

// goes to the window, if possible
void CSmartSocket::Notify(LPSTR format, ...) {

    LPSTR pBuf = (LPSTR)malloc(BUFFSIZE);
    if (NULL == pBuf) return;

    va_list marker;
	va_start(marker,format);
	vsprintf(pBuf,format,marker);
	va_end(marker);

    OutputDebugString(pBuf);

    if (m_pParent) {
		m_pParent->SendMessage(WM_PRINTF_MSG,0,(long)pBuf);
	} else {
        free(pBuf);
    }
}

// goes out on the socket
BOOL __cdecl CSmartSocket::Printf(LPSTR format, ...)
{
	ASSERT_VALID(this);
	
	if(!m_bConnected) {
		return FALSE;
    }
	// do the sprintf stuff
	LPSTR buffer = (LPSTR)malloc(BUFFSIZE);

	va_list marker;
	va_start(marker,format);
	vsprintf(buffer,format,marker);
	va_end(marker);

	// add it to our output buffer	
	m_asOutBuffer.Add(buffer);
	free(buffer);
	AsyncSelect(FD_READ|FD_WRITE|FD_CLOSE);
	return TRUE;
}

void CSmartSocket::OnConnect(int nErrorCode)
{
	if(!nErrorCode) {
        // experimental SSL support
        // http://languageshelp.blogspot.com/2008/01/ssl-openssl-with-visual-c.html
        if (m_bUseSSL) {

            // SSL setup - not sure if I need all this every time
            meth = TLS_client_method();
            ctx = SSL_CTX_new (meth); 
            if (NULL == ctx) {
                Notify("%%%%%% Failed to get SSL context - failing.\n");
                OnClose(WSANOTINITIALISED);
                return;
            }

            // Start the SSL connection
            ssl = SSL_new(ctx);
            if (NULL == ssl) {
                Notify("%%%%%% Failed to create new SSL - failing.\n");
                OnClose(WSAECONNABORTED);
                return;
            }

            SSL_set_fd(ssl, m_hSocket);
            // non-blocking socket may return WANT_READ or WANT_WRITE, we just delay here...
            // https://stackoverflow.com/questions/3952104/how-to-handle-openssl-ssl-error-want-read-want-write-on-non-blocking-sockets
            int timeout = 100;  // roughly 2 seconds total
            int delay = 20;
            for (;;) {
                if (--timeout <= 0) {
                    Notify("%%%%%% Timeout on SSL connection - aborting\n");
                    
                    // TODO: should get proper errors into here...
                    OnClose(WSAEOPNOTSUPP);
                    return;
                }

                int conval = SSL_connect(ssl);
                if (conval < 0) {
                    int x = SSL_get_error(ssl, conval);
                    if (x == SSL_ERROR_WANT_READ) {
                        OutputDebugString("SSL Socket connect WANT_READ\n");
                        Sleep(delay);
                        continue;
                    }
                    if (x == SSL_ERROR_WANT_WRITE) {
                        OutputDebugString("SSL Socket connect WANT_WRITE\n");
                        Sleep(delay);
                        continue;
                    }
                
                    Notify("%%%%%% SSL Socket error %d\n", x);

                    // Not convinced this is working right... seem to get zero here...
                    char msg[1024];
                    int err = ERR_get_error();
                    ERR_error_string_n(err, msg, sizeof(msg));
                    Notify("%%%%%% %s %s %s %s\n", msg, ERR_lib_error_string(err), ERR_func_error_string(err), ERR_reason_error_string(err));

                    // TODO: should get proper errors into here...
                    OnClose(WSAEOPNOTSUPP);
                    return;
                } else {
                    break;
                }
            }

            m_encryption = SSL_get_cipher(ssl);
            Notify("%%%%%% Successful SSL connection using %s\n", m_encryption.GetString());

            // for now, we display the certificate information.
            strcpy(fingerprint, "");
            scert = SSL_get_peer_certificate(ssl);
            if (NULL == scert) {
                // TODO: is this an error??
                Notify("%%%%%% No certificate gathered, continuing...\n");
            } else {
                // TODO: But what do you check? The whole cert? Is the name enough?
                char *txt = X509_NAME_oneline(X509_get_subject_name(scert), 0, 0);
                if (NULL == txt) {
                    Notify("%%%%%% Cert subject: (unknown)\n");
                } else {
                    Notify("%%%%%% Cert subject: %s\n", txt);
                }
                OPENSSL_free(txt);

                txt = X509_NAME_oneline(X509_get_issuer_name(scert), 0, 0);
                if (NULL == txt) {
                    Notify("%%%%%% Cert issuer: (unknown)\n");
                } else {
                    Notify("%%%%%% Cert issuer: %s\n", txt);
                }
                OPENSSL_free(txt);

                const EVP_MD *digest = EVP_get_digestbyname("sha1");
                unsigned char md_value[EVP_MAX_MD_SIZE];
                unsigned int n = sizeof(md_value);
                X509_digest(scert, digest, md_value, &n);
                for (unsigned int i=0; i<n; ++i) {
                    sprintf(fingerprint, "%s:%02X", fingerprint, md_value[i]);
                }
                Notify("%%%%%% Fingerprint: %s\n", fingerprint);

                X509_free(scert);
                scert = NULL;
            }

            // check the certificate fingerprint
            if (!m_pParent->SendMessage(WM_VERIFY_CERT,0,(long)fingerprint)) {
                // the user has rejected the key, so disconnect
                Notify("%%%%%% Key fingerprint rejected\n");
                // TODO: should get proper errors into here...
                OnClose(WSANO_DATA);
                return;
            }

            // at this point SSL is up, but we need to use the SSL functions
            // for everything instead of the default ones...
        }

        // set this last
        m_bConnected=TRUE;
    }

	m_pParent->SendMessage(WM_SOCKET_CONNECTED,nErrorCode);
	m_bPaused=FALSE;
}

void CSmartSocket::OnSend(int /*nErrorCode*/ )
{
	while(m_asOutBuffer.GetSize())
	{
        if ((m_bUseSSL)&&(NULL != ssl)) {
            // encrypted
            if (-1 == SSL_write(ssl, m_asOutBuffer[0],m_asOutBuffer[0].GetLength())) {
                break;
            } else {
			    m_asOutBuffer.RemoveAt(0);
		    }
        } else {
            // unencrypted
		    if(Send(m_asOutBuffer[0],m_asOutBuffer[0].GetLength())==SOCKET_ERROR) {
			    break;
		    } else {
			    m_asOutBuffer.RemoveAt(0);
		    }
        }
	}
}

void CSmartSocket::OnClose(int nErrorCode)
{
	m_bConnected=FALSE;

    wrapClose();

	m_pParent->PostMessage(WM_SOCKET_DISCONNECTED,nErrorCode);
	Pause(FALSE);
}

void CSmartSocket::wrapClose() {
    // just wraps the Async close method so we can
    // always do the SSL steps too
    if ((m_bUseSSL)&&(NULL != ssl)) {
        SSL_shutdown(ssl);
    }

	Close();

    if (m_bUseSSL) {
        SSL_free(ssl);
        ssl = NULL;
        SSL_CTX_free(ctx);
        ctx = NULL;
        // TODO: what about meth?
    }
}

BOOL CSmartSocket::HardClose()
{
	m_bConnected=FALSE;
    LINGER linger;
    if( m_hSocket == INVALID_SOCKET )
    {
        //  Ignore invalid sockets.
        return FALSE;
    }
    linger.l_onoff  = TRUE;
    linger.l_linger = 0;
    SetSockOpt(SO_LINGER,(CHAR FAR *)&linger,sizeof(linger) );

    //  Close the socket.
	ShutDown(2);	// both send and receive

    wrapClose();

	m_hSocket = INVALID_SOCKET;
	if (m_pParent->m_hWnd) {
		m_pParent->SendMessage(WM_SOCKET_DISCONNECTED);
	}
	return TRUE;
}

BOOL CSmartSocket::IsConnected()
{
	return m_bConnected;
}

// over-ridden functions
void CSmartSocket::OnReceive(int /*nErrorCode*/ )
{
	static int recursion = -1;

	recursion ++;
	ASSERT_VALID(this);

	LPSTR pBuf;

	pBuf=(LPSTR)malloc(BUFFSIZE);

	LPSTR pAdjBuf = pBuf;				// adjusted buffer for frag merging.
	if(*m_pFragBuf)	// special routine to avoid sending out incomplete ansi sequences.
	{
		strcpy(pBuf,m_pFragBuf);	// first copy the frag
		*m_pFragBuf=0;
		pAdjBuf+=strlen(pBuf);
	}
	int retcode=1;
    if ((m_bUseSSL)&&(NULL != ssl)) {
        retcode = SSL_read(ssl, pAdjBuf, BUFFSIZE-50);
    } else {
    	retcode = Receive(pAdjBuf,BUFFSIZE-50);
    }
	if(retcode>0 && retcode !=SOCKET_ERROR)
	{
		pAdjBuf[retcode]=0;	// null terminate it.
		// Do away with 0 bytes inside the packet (replace with 1, which we'll ignore)
		for (int i=0; i<retcode; i++) {
			if (pAdjBuf[i]=='\0') {
				pAdjBuf[i]=1;
			}
		}

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

		// Check the received string for Telnet negotiation, and handle it here. We don't send anything
		// unless the server asks for it, first
		while (NULL != (pCheck=strchr(pBuf, 255))) {		// char 255 indicates telnet request
			// The next byte must be 251 (WILL), 252 (DO), 253 (WONT) or 254 (DONT) 
			// (note: it could be something else, but we don't care here. Bare minimum.)
			int cmd, option;
			int process=0;
			char buf[1024];

			// In case we don't handle the character, we need to remove it from the stream
			// so we don't see it again.
			*pCheck=(char)(unsigned char)0xfe;

			// Get the command
			cmd=*(unsigned char*)(++pCheck);
			option=0;
			// First we'll check the command. We don't do most things, so by and large we
			// check for very little. We only respond, we don't do our own requests
			
			switch (cmd) {
			case 251:		// WILL
			case 252:		// WONT
				// okay, good
				if (!(*pCheck)) break;
				option=*(unsigned char*)(++pCheck);

				process=3;
				if (m_LastNegotiated.Find((char)option) != -1) break;
				switch (option) {
				case 3:		// Suppress Go-Ahead
					Printf("%c%c%c", 255, 253, 3);		// DO
					sprintf(buf, "Received %s %d, responding DO\n", cmd==251?"WILL":"WON'T", option);
					OutputDebugString(buf);
					break;

				default:
					Printf("%c%c%c", 255, 254, option);	// DONT
					sprintf(buf, "Received %s %d, responding DON'T\n", cmd==251?"WILL":"WON'T", option);
					OutputDebugString(buf);
				}
				break;

			case 253:		// DO
			case 254:		// DONT
				// okay, good
				if (!(*pCheck)) break;
				option=*(unsigned char*)(++pCheck);

				process=3;
				if (m_LastNegotiated.Find((char)option) != -1) break;

				switch (option) {
				case 3:		// suppress Go-Ahead
					Printf("%c%c%c", 255, 251, 3);		// WILL
					sprintf(buf, "Received %s %d, responding WILL\n", cmd==253?"DO":"DON'T", option);
					OutputDebugString(buf);
					break;

				default:
					Printf("%c%c%c", 255, 252, option);	// WONT
					sprintf(buf, "Received %s %d, responding WON'T\n", cmd==253?"DO":"DON'T", option);
					OutputDebugString(buf);
				}
				break;

			// Things we don't support
			case 241:	// NOP
			case 243:	// Break
			case 244:	// IP
			case 245:	// AO
			case 246:	// AYT
			case 247:	// EC
			case 248:	// EL
			case 249:	// GA
			case 250:	// SB
			case 255:	// 0xff literal data
				sprintf(buf, "Received unexpected Telnet character/command %d (ignoring)\n", cmd);
				OutputDebugString(buf);
				process=2;	// 2 chars
				break;

			// Anything else we'll ignore completely
			}

			if (process) {
				m_LastNegotiated+=(char)option;		// track all options already negotiated
				memmove(pCheck-(process-1), pCheck+1, BUFFSIZE-(pCheck-pBuf-1));
				if (!strlen(pBuf)) {
					free(pBuf);
					pBuf=NULL;
					break;
				}
			}
		}
				
		// okay we're now ready to send the ansi-complete string.
		if (pBuf) {
			m_pParent->SendMessage(WM_SOCKET_STRING_RECIEVED,0,(long)pBuf);
		}
	}
	else
	{
		free(pBuf);
	}

	recursion--;
}

int CSmartSocket::Pause( BOOL bPause)
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
