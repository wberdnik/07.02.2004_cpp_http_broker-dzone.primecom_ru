
#include "stdafx.h"
#include <windows.h>
//#include <winsock.h>
#include "winsock2.h"
#include "AddIn.h"


LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL InitWinsock ();
BOOL TermWinsock (); 
BOOL SendBytes (SOCKET s, PCHAR pBuf, DWORD cbBuf);
BOOL ReceiveBytes (SOCKET s, PCHAR pBuf, DWORD cbBuf, DWORD *pcbRead);
BOOL ConnectAuthSocket (LPCSTR szServer, SOCKET *s,
						const unsigned short * port,const unsigned short *name);
BOOL CloseAuthSocket (SOCKET s);
BOOL DoAuthentication (SOCKET s,const unsigned short *);
DWORD WINAPI ThreadFunc(LPVOID lpParameter);

DWORD cbRead;
DWORD ThreadID;
SOCKET s;
HANDLE ThreadHandle=NULL;
WSAEVENT evnt;
int IsConnectid=0;
char Orientation = 'C'; 
static DWORD g_cbMaxMessage = 0;
static unsigned short g_usPort = 15015;



int WINAPI StartServer(const unsigned short * node,const unsigned short * port,
					   const unsigned short * name){
	char buf[1000];

	if (!InitWinsock ())return EXIT_FAILURE;

	WideCharToMultiByte(CP_ACP, 0, node, -1, buf, 1000, NULL, NULL);
	if (!ConnectAuthSocket (buf, &s,port,name))	return EXIT_FAILURE;
	IsConnectid=1;
	ThreadHandle=CreateThread(NULL, 0, ThreadFunc, (void *)s, 0, &ThreadID);
	return EXIT_SUCCESS;

}
  

DWORD WINAPI ThreadFunc(LPVOID lpParameter)
{
  DWORD i , Flags , RecvBytes;

  WSANETWORKEVENTS NetworkEvents;
  WSABUF DataBuf;
  char *buf;
  CString csSource("Интернет"),csMessage("РазрывСоединения"),csData("");

	evnt = WSACreateEvent();
	WSAEventSelect((SOCKET )(lpParameter), evnt,FD_READ|FD_CLOSE|FD_ACCEPT);


	while(true)	
	{
		i = WSAWaitForMultipleEvents(1,&evnt,false,WSA_INFINITE,false);
		WSAEnumNetworkEvents((SOCKET )(lpParameter), evnt, &NetworkEvents);

				if (NetworkEvents.lNetworkEvents & FD_ACCEPT)Nu("Connectinon accessed.");
				if (NetworkEvents.lNetworkEvents & FD_READ)
				{
					Flags = 0;
					ioctlsocket((SOCKET )(lpParameter),FIONREAD,&i);
					buf = new char[i+2];
					DataBuf.len = i+1;
					DataBuf.buf = buf;
					if (WSARecv((SOCKET )(lpParameter), &DataBuf, 1, &RecvBytes,
									&Flags, NULL, NULL) == SOCKET_ERROR)
						if (WSAGetLastError() != WSAEWOULDBLOCK)
								SayError("Ошибка чтения");
					SendIn(buf);
					delete[] buf;
				}

		      if (NetworkEvents.lNetworkEvents & FD_CLOSE)
			  {
				  	
				/*	IsConnectid=0;
					//	WSACloseEvent(evnt);   
					CloseAuthSocket (s);
					TermWinsock ();
*/

				  if (pAsyncEvent)
					pAsyncEvent->ExternalEvent(csSource.AllocSysString(),
                    csMessage.AllocSysString(),
                    csData.AllocSysString());

					IsConnectid=0;	
//				    ThreadHandle=NULL;
					return NULL; //TerminateThread
			  }
		::WSAResetEvent(evnt);
	}

    
	return NULL;
}



BOOL InitWinsock ()
{
	int nRes;
	WSADATA wsaData;
	WORD wVerRequested = MAKEWORD( 2, 0 ); //0x0101; // ver 1.1
    char buffer[512];
	nRes = WSAStartup (wVerRequested, &wsaData);
	if (nRes)  {
		sprintf (buffer, "Couldn't init winsock: %d\n", nRes);
		SayError(buffer);
		return(FALSE);
	}

	return(TRUE);
}




BOOL ConnectAuthSocket (LPCSTR szServer, SOCKET *s,const unsigned short * port,
							const unsigned short * name)
{
	SOCKET sockServer;
	unsigned long ulAddress;
	struct hostent *pHost;
	SOCKADDR_IN sin;
    
	ulAddress = inet_addr (szServer);
	if (INADDR_NONE == ulAddress) {
		pHost = gethostbyname (szServer);
		if (NULL == pHost) {
			Nu("Ошибка определения IP адреса узла");
			return(FALSE);
		}

		memcpy((char FAR *)&ulAddress, pHost->h_addr, pHost->h_length);
	}

	// create the socket
	sockServer = socket (AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == sockServer) {
		Nu("Ошибка создания сокета");
		return(FALSE);
	}

	swscanf(port,L"%u",&g_usPort);
	

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = ulAddress;
	sin.sin_port = htons (g_usPort);



	// connect to remote endpoint
	//
	if (connect (sockServer, (LPSOCKADDR) &sin, sizeof (sin))) {
		Nu("Ошибка подключения");
		return(FALSE);
	}


	if (!DoAuthentication (sockServer,name)) {
		closesocket (sockServer);
		return(FALSE);
	}

	*s = sockServer;

	return(TRUE);
}



BOOL DoAuthentication (SOCKET s,const unsigned short * name)
{
	 char buf[4096]="";

	recv(s,buf,1,0);
	if(buf[0]!='#') return FALSE;
	sprintf(buf,"%lc %ls",Orientation,name);
	if(!send(s,buf,strlen(buf)+1,0)) return FALSE;
	return TRUE;
}



BOOL TermWinsock ()
{	return !(SOCKET_ERROR == WSACleanup ());
}

BOOL SendMsg ( PCHAR pBuf, DWORD cbBuf)
{
	if (0 == cbBuf)	return(TRUE);
	if (!SendBytes (s, pBuf, cbBuf))return(FALSE);
	return(TRUE);
}	


BOOL SendBytes (SOCKET s, PCHAR pBuf, DWORD cbBuf)
{
	PCHAR pTemp = pBuf;
	int cbSent, cbRemaining = cbBuf;

	if (0 == cbBuf)	return(TRUE);

	while (cbRemaining) {
		cbSent = send (s, pTemp, cbRemaining, 0);
		if (SOCKET_ERROR == cbSent) {
			Nu("Ошибка отправки по сети");
			return FALSE;
		}

		pTemp += cbSent;
		cbRemaining -= cbSent;
	}

	return TRUE;
}

BOOL ReceiveBytes (SOCKET s, PCHAR pBuf, DWORD cbBuf, DWORD *pcbRead)
{
	PCHAR pTemp = pBuf;
	int cbRead, cbRemaining = cbBuf;

	while (cbRemaining) {
		cbRead = recv (s, pTemp, cbRemaining, 0);
		if (0 == cbRead)
			break;

		if (SOCKET_ERROR == cbRead) {
			Nu("Ошибка при приеме");
			return FALSE;
		}
	
		cbRemaining -= cbRead;
		pTemp += cbRead;
	}

	*pcbRead = cbBuf - cbRemaining;

	return TRUE;
}


BOOL CloseAuthSocket (SOCKET s)
{
	shutdown (s, 2);
	closesocket (s);
	return(TRUE);
}	




BOOL DisconnectServer()
{
	TerminateThread(ThreadHandle,0);
	ThreadHandle=NULL;
	IsConnectid=0;
	WSACloseEvent(evnt);   
	CloseAuthSocket (s);
	TermWinsock ();
	return true;
}

void SetOrientation(char data)
{
Orientation = data;
}
