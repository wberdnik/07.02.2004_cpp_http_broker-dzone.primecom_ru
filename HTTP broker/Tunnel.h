// Tunnel.h: interface for the CTunnel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TUNNEL_H__479A4191_071C_11D8_BC29_444553540000__INCLUDED_)
#define AFX_TUNNEL_H__479A4191_071C_11D8_BC29_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <winsock.h>

#define BUFFER_SIZE 4096

enum tagORIENTATION
{
	toServer = 0,
	toClient = 1
};

class CTunnel  
{private:
char * userclient;
public:
	char * userserver;
	char lpszClientId[30];
	char lpszSignature[30];
	CTunnel ::CTunnel(){userserver=NULL;userclient=NULL;ClientSocket = NULL;InitState = FALSE;}
	CTunnel ::~CTunnel(){if(userserver)delete[] userserver;if(userclient)delete[] userclient;}
	void CTunnel::SetUserServer(char *szUser,int len){userserver=new char[len+1];strncpy(userserver,szUser,len);}
	void CTunnel::SetUserClient(char *szUser,int len){userclient=new char[len+1];strncpy(userclient,szUser,len);}

	void CTunnel::Excenge(void);

	void CTunnel::SayConnected(void);
	void CTunnel::SayServer(int);
	void CTunnel::SayServerClient(int);

	SOCKET ServerSocket;
	sockaddr_in ServerSin;
	int SSLen;
	SOCKET ClientSocket;
	sockaddr_in ClientSin;
	int CSLen;
	BOOL InitState;
	DWORD Size;


	DWORD ThreadID;
	HANDLE hThread;
	tagORIENTATION Orientation;
};


#endif // !defined(AFX_TUNNEL_H__479A4191_071C_11D8_BC29_444553540000__INCLUDED_)
