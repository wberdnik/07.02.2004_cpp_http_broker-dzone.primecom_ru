// TunnelThread.cpp: implementation of the CTunnelThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TunnelThread.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTunnelThread::CTunnelThread()
{
//	hThread = CreateThread(NULL,0,(DWORD)&Execute,(LPVOID)this,CREATE_SUSPENDED,&ThreadID);
}

DWORD APIENTRY CTunnelThread::Execute(void * Tunnel)
{
	CTunnel * tTunnel = (CTunnel *)Tunnel;
	switch (Orientation)
	{
	case toServer:
		tTunnel->Size = recv(tTunnel->ClientSocket,(char *)tTunnel->Buffer,sizeof(tTunnel->Buffer),0);
		send(tTunnel->ServerSocket,(char *)tTunnel->Buffer,sizeof(tTunnel->Buffer),0);
		break;
	case toClient:
		tTunnel->Size = recv(tTunnel->ServerSocket,(char *)tTunnel->Buffer,sizeof(tTunnel->Buffer),0);
		send(tTunnel->ClientSocket,(char *)tTunnel->Buffer,sizeof(tTunnel->Buffer),0);
		break;
	}
	delete this;
	return 0;
}
