// MapServer.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
#include "tunnel.h"
#include <winsock.h>
#include <shellapi.h>
#include <stdio.h>


#define MAX_LOADSTRING 100
#define WM_TASKBAR WM_APP+30 

#define WSA_ACCEPT		WM_USER+1
#define WSA_NETEVENT	WM_USER+2


int TaskBarAddIcon( HWND hWindow,UINT id, HICON hIcon,UINT CallbackMessage,	PCHAR Tip);
int TaskBarDeleteIcon(HWND hWindow,UINT id);
void PopupMenu(HWND hWnd);



HINSTANCE hInst;
TCHAR szTitle[MAX_LOADSTRING];
TCHAR szWindowClass[MAX_LOADSTRING];

static HWND hListBox;
static HWND hListBox2;
HWND hWnd;
unsigned int iTaskBarCreated=0;
int StatusTray=0;
int StatusHistory=0;
unsigned int port=15015;

static int iSt=0;
char szHint[500];
UINT ICON_ID = 0;



ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
BOOL CheckClientId(HWND hListBox, char * lpszSignature);

char * lpszMapServer = "MapServer7f542399a8f2 ";
char * lpszEmptyMessage = "empty\0";
char * lpszBusyMessage = "638 Ваш запрос обрабатывается\0";
char * lpszQuitMessage = "quit\0";


int CheckBuff(char * lpszBuff, char * lpszSubString, char * lpszSignature)
{
	unsigned int tmp1;
	char * tmp = strstr(lpszBuff,lpszSubString);

	if (tmp==NULL) return -1;	//Строка не найдена

	tmp1 = strcspn(tmp,"\0x0d\0x0a");
	if (tmp1==NULL) return -2;	//Строка найдена, но нет
								//признака окончания идентификатора
	strcpy(lpszSignature, tmp + strlen(lpszSubString));
	return tmp1 - strlen(lpszSubString);
}



void CTunnel::Excenge(void){
	char *Buff;
	TCHAR tcStr[555];
		switch (Orientation){
	case toServer:
		ioctlsocket(ClientSocket, FIONREAD, &Size);
		Buff = new char[Size+1];
		Size = recv(ClientSocket,Buff,Size,0);
		Buff[Size]=0;	
		//Buff[Size+1]=0xd;
		//Buff[Size+2]=0xa;
		send(ServerSocket,Buff,Buff[Size-1]?Size+1:Size,0);

		if(StatusHistory){
			wsprintf(tcStr,"%06d На сервер %d.%d.%d.%d : %s",
				iSt++,
			ServerSin.sin_addr.S_un.S_un_b.s_b1,
			ServerSin.sin_addr.S_un.S_un_b.s_b2,
			ServerSin.sin_addr.S_un.S_un_b.s_b3,
			ServerSin.sin_addr.S_un.S_un_b.s_b4,
			Buff);
			SendMessage(hListBox2,LB_ADDSTRING,0,(LPARAM)(LPSTR)&tcStr);
		}

		delete[] Buff;
		break;
	case toClient:
		ioctlsocket(ServerSocket, FIONREAD, &Size);
		Buff = new char[Size+1];
		Size = recv(ServerSocket,Buff,Size,0);
		Buff[Size]=0;	
		//Buff[Size+1]=0xd;
		//Buff[Size+2]=0xa;


		int iSignature = CheckBuff(Buff,lpszMapServer,lpszSignature);

		if (iSignature==-1)
		{
			send(ClientSocket,Buff,Buff[Size-1]?Size+1:Size,0);
			if(StatusHistory){
			wsprintf(tcStr,"%06d На клиента %d.%d.%d.%d : %s",
				iSt++,
			ClientSin.sin_addr.S_un.S_un_b.s_b1,
			ClientSin.sin_addr.S_un.S_un_b.s_b2,
			ClientSin.sin_addr.S_un.S_un_b.s_b3,
			ClientSin.sin_addr.S_un.S_un_b.s_b4,
			Buff);
			SendMessage(hListBox2,LB_ADDSTRING,0,(LPARAM)(LPSTR)&tcStr);
			}

		}
		else if (iSignature==0)
		{
			send(ServerSocket,lpszEmptyMessage,strlen(lpszEmptyMessage)+1,0);
		}
		else if (iSignature<0)
		{
			send(ClientSocket,Buff,Size+3,0);
		}
		else if (iSignature>0)
		{
			if (CheckClientId(hListBox,lpszSignature))
			{
				strcpy(lpszClientId,lpszSignature);
			}
			else
			{
				send(ClientSocket,lpszBusyMessage,strlen(lpszBusyMessage)+1,0);
				send(ServerSocket,lpszQuitMessage,strlen(lpszQuitMessage)+1,0);
				closesocket(ClientSocket);
				SendMessage(hWnd,WSA_NETEVENT,ClientSocket,FD_CLOSE);
			}
		}

		delete[] Buff;
		break;
	}
}

void CTunnel::SayConnected(void){
	TCHAR tcStr[255];
	wsprintf(tcStr,"Connected %s : %d.%d.%d.%d",
	userserver,
	ServerSin.sin_addr.S_un.S_un_b.s_b1,
	ServerSin.sin_addr.S_un.S_un_b.s_b2,
	ServerSin.sin_addr.S_un.S_un_b.s_b3,
	ServerSin.sin_addr.S_un.S_un_b.s_b4);
	int iIndex = SendMessage(hListBox,LB_ADDSTRING,0,(LPARAM)(LPSTR)&tcStr);
	SendMessage(hListBox,LB_SETITEMDATA,iIndex,(DWORD)this);
}
void CTunnel::SayServer(int iIndex){
	TCHAR tcStr[255];
	wsprintf(tcStr,"Server %s : %d.%d.%d.%d",
	userserver,
	ServerSin.sin_addr.S_un.S_un_b.s_b1,
	ServerSin.sin_addr.S_un.S_un_b.s_b2,
	ServerSin.sin_addr.S_un.S_un_b.s_b3,
	ServerSin.sin_addr.S_un.S_un_b.s_b4);
	SendMessage(hListBox,LB_DELETESTRING,iIndex,0L);
	SendMessage(hListBox,LB_INSERTSTRING,iIndex,(LPARAM)tcStr);
	SendMessage(hListBox,LB_SETITEMDATA,iIndex,(LPARAM)this);
}
void CTunnel::SayServerClient(int iIndex){
	TCHAR tcStr[255];
	wsprintf(tcStr,"Server %s : %d.%d.%d.%d <- Client %s : %d.%d.%d.%d",
	userserver,
	ServerSin.sin_addr.S_un.S_un_b.s_b1,
	ServerSin.sin_addr.S_un.S_un_b.s_b2,
	ServerSin.sin_addr.S_un.S_un_b.s_b3,
	ServerSin.sin_addr.S_un.S_un_b.s_b4,
	userclient,
	ClientSin.sin_addr.S_un.S_un_b.s_b1,
	ClientSin.sin_addr.S_un.S_un_b.s_b2,
	ClientSin.sin_addr.S_un.S_un_b.s_b3,
	ClientSin.sin_addr.S_un.S_un_b.s_b4);
	SendMessage(hListBox,LB_DELETESTRING,iIndex,0L);
	SendMessage(hListBox,LB_INSERTSTRING,iIndex,(LPARAM)tcStr);
	SendMessage(hListBox,LB_SETITEMDATA,iIndex,(LPARAM)this);
}


DWORD WINAPI Execute(void * Tunnel)
{
	((CTunnel *)Tunnel)->Excenge ();
	return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	MSG msg;
	HACCEL hAccelTable;
	char ForBase[100]="";
	char lStatus='N';

	if (strlen(lpCmdLine)>2){
	sscanf(lpCmdLine,"%c %d %s",&lStatus,&port,ForBase);
	//::MessageBox(NULL,lpCmdLine,"",MB_OK);
	if(lStatus=='T')StatusTray=1;
	else StatusTray=0;
				}

	sprintf(szHint,"MapServer %d %s",port,ForBase);

	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MAPSERVER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	iTaskBarCreated = RegisterWindowMessage("TaskbarCreated");
	hInst=hInstance;
	if(StatusTray){
		ShowWindow(hWnd, SW_HIDE);
		TaskBarAddIcon(hWnd,ICON_ID, LoadIcon(hInst, (LPCTSTR)IDI_SMALL), 
									WM_TASKBAR, szHint);
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_MAPSERVER);

	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	 // Убираем за собой значок
	if (StatusTray)
		TaskBarDeleteIcon(hWnd, ICON_ID);

	return msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_MAPSERVER);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_MAPSERVER;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   
   hInst = hInstance;

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 520, 407, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

int GetTunnelBySocket(SOCKET Socket, HWND hListBox)
{
	CTunnel * Tunnel;
	int a = SendMessage(hListBox,LB_GETCOUNT,0,0L);
	for (int i=0;i<a;i++)
	{
		Tunnel = (CTunnel *)SendMessage(hListBox,LB_GETITEMDATA,i,0L);
		if (!Tunnel) continue;
		if ((Tunnel->ClientSocket==Socket) || (Tunnel->ServerSocket==Socket)) return i;
	}
	return -1;
}

int GetFreeServer(HWND hListBox)
{
	CTunnel * Tunnel;
	int a = SendMessage(hListBox,LB_GETCOUNT,0,0L);
	for (int i=0;i<a;i++)
	{
		Tunnel = (CTunnel *)SendMessage(hListBox,LB_GETITEMDATA,i,0L);
		if (!Tunnel) continue;
		if ((Tunnel->ClientSocket==NULL) && (Tunnel->InitState))
			return i;
	}
	return -1;
}

BOOL CheckClientId(HWND hListBox, char * lpszSignature)
{
	CTunnel * Tunnel;
	int a = SendMessage(hListBox,LB_GETCOUNT,0,0L);
	for (int i=0;i<a;i++)
	{
		Tunnel = (CTunnel *)SendMessage(hListBox,LB_GETITEMDATA,i,0L);
		if (!Tunnel) continue;
		if ((strlen(Tunnel->lpszClientId)==strlen(lpszSignature)) && (strncmp(Tunnel->lpszClientId,lpszSignature,strlen(lpszSignature))==0))
			return false;
	}
	return true;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT r;
	int iIndex;
	WSAData wsa;
	static SOCKET ss;
	static sockaddr_in sin;
	int lsin;
	CTunnel * Tunnel;
	CTunnel * Tmp;
	int a,i;

	if(iTaskBarCreated==message && StatusTray==1){
		TaskBarAddIcon(hWnd,ICON_ID, LoadIcon(hInst, (LPCTSTR)IDI_SMALL),WM_TASKBAR, szHint);
		return 0;
	}	


	switch (message) 
	{
	    case WM_CREATE:
			GetWindowRect(hWnd,&r);
			hListBox = CreateWindow("listbox",NULL,WS_CHILD|WS_VISIBLE|
				LBS_STANDARD,0,0,513,162,hWnd,NULL,hInst,NULL);

			hListBox2 = CreateWindow("listbox",NULL,WS_CHILD|WS_VISIBLE|
				LBS_STANDARD,0,167,513,162,hWnd,NULL,hInst,NULL);


			if (WSAStartup(MAKEWORD(2,0),&wsa))
			{
				MessageBox(NULL,"WSAStartup Error","Error",MB_OK);
				return 0;
			};

			ss = socket(AF_INET,SOCK_STREAM,0);
			if (ss == INVALID_SOCKET)
			{
				MessageBox(NULL,"socket Error","Error",MB_OK);
				return 0;
			}

			sin.sin_family = AF_INET;
			sin.sin_port = htons(port);
			sin.sin_addr.S_un.S_addr = INADDR_ANY;
			if(bind(ss,(const struct sockaddr *)&sin,sizeof(sin))==SOCKET_ERROR)
			{
				closesocket(ss);
				MessageBox(NULL,"bind Error","Error",MB_OK);
				return 0;
			}
			
			if (listen(ss,5) == SOCKET_ERROR)
			{
				closesocket(ss);
				MessageBox(NULL,"listen Error","Error",MB_OK);
				return 0;
			}

			if (WSAAsyncSelect(ss,hWnd,WSA_ACCEPT,FD_ACCEPT)>0)
			{
				closesocket(ss);
				MessageBox(NULL,"WSAAsyncSelect Error","Error",MB_OK);
			}
			break;
		case WSA_ACCEPT: //wParam = socket
			lsin = sizeof(sin);
			Tunnel = new CTunnel;
			
			Tunnel->SSLen = sizeof(Tunnel->ServerSin);
			Tunnel->ServerSocket = accept(ss,(LPSOCKADDR)&(Tunnel->ServerSin),(int far *)&(Tunnel->SSLen));
			if (Tunnel->ServerSocket == INVALID_SOCKET)
			{
				MessageBox(NULL,"accept Error","Error",MB_OK);
				return 0;
			}

			if (WSAAsyncSelect(Tunnel->ServerSocket,hWnd,WSA_NETEVENT,FD_READ|FD_CLOSE)>0)
			{
				closesocket(Tunnel->ServerSocket);
				MessageBox(NULL,"WSAAsyncSelect Error","Error",MB_OK);
			}
			Tunnel->SayConnected();
			send(Tunnel->ServerSocket,"#",1,0);
			break;
		case WSA_NETEVENT: //wParam = socket
			iIndex = GetTunnelBySocket(wParam,hListBox);
			if (iIndex == -1) return 0;
			Tunnel = (CTunnel *)SendMessage(hListBox,LB_GETITEMDATA,iIndex,0L);

			switch (WSAGETSELECTEVENT(lParam))
			{
			case FD_READ:
				if (!(Tunnel->InitState))
				{
					char cWho;
					unsigned long uLen;
					char * Buff;

					ioctlsocket(Tunnel->ServerSocket, FIONREAD, &uLen);
					Buff = new char[uLen+1];
					uLen = recv(Tunnel->ServerSocket,Buff,uLen,0);
					cWho=Buff[0];

					if (uLen>2)Tunnel->SetUserServer(Buff+2,uLen-1);
					else Tunnel->SetUserServer("Noname",9);

					delete[] Buff;

					switch(cWho)
					{
					case 'C':
						SendMessage(hListBox,LB_DELETESTRING,iIndex,0L);
						iIndex = GetFreeServer(hListBox);
						if (iIndex == -1)
						{	send(Tunnel->ServerSocket,"633 Нет свободных серверов.",28,0);
							closesocket(Tunnel->ServerSocket);
							return 0;
						}
						Tmp = (CTunnel *)SendMessage(hListBox,LB_GETITEMDATA,iIndex,0L);
						Tmp->ClientSin = Tunnel->ServerSin;
						Tmp->ClientSocket = Tunnel->ServerSocket;
						Tmp->CSLen = Tunnel->SSLen;
						Tmp->SetUserClient(Tunnel->userserver,strlen(Tunnel->userserver)+1);
						delete Tunnel;

						Tmp->SayServerClient(iIndex);

						uLen=strlen(Tmp->userserver)+18;
						Buff = new char[uLen+1];
						sprintf(Buff,"635 Подключен к %s",Tmp->userserver);
						send(Tmp->ClientSocket,Buff,uLen+1,0);
						delete[] Buff;

						return 0;
					case 'S':
						Tunnel->InitState = TRUE;
						Tunnel->SayServer(iIndex);
						return 0;
					default:
						closesocket(Tunnel->ServerSocket);
						delete Tunnel;
						SendMessage(hListBox,LB_DELETESTRING,iIndex,0L);
						return 0;
					}
				}

				Tunnel->Orientation =(Tunnel->ClientSocket == wParam)?toServer:toClient;
				Tunnel->hThread = CreateThread(NULL,0,Execute,(void *)Tunnel,0,&Tunnel->ThreadID);
				break;
			case FD_CLOSE:
				//Если не инициализирован
				if (!(Tunnel->InitState))
				{
					SendMessage(hListBox,LB_DELETESTRING,iIndex,0L);
					delete Tunnel;
					return 0;
				}
				//Если клиент
				if (Tunnel->ClientSocket == wParam)
				{//Отключение клиента
					Tunnel->ClientSocket = NULL;
					Tunnel->SayServer(iIndex);
					send(Tunnel->ServerSocket,"QUIT",5,0);
					return 0;
				}
				//Если сервер 
				if (Tunnel->ServerSocket == wParam)
				{
					SendMessage(hListBox,LB_DELETESTRING,iIndex,0L);
					if (Tunnel->ClientSocket == NULL)
					{
						delete Tunnel;
						return 0;
					}
					else
					{
						iIndex = GetFreeServer(hListBox);
						if (iIndex == -1)
						{
							send(Tunnel->ClientSocket,"632 Отключение сервера. Нет свободных серверов.",48,0);
							closesocket(Tunnel->ClientSocket);
							delete Tunnel;
							return 0;
						}
						else
						{// Переключаем на другой сервер
							Tmp = (CTunnel *)SendMessage(hListBox,LB_GETITEMDATA,iIndex,0L);
							Tmp->ClientSocket = Tunnel->ClientSocket;
							Tmp->ClientSin = Tunnel->ClientSin;
							Tmp->CSLen = Tunnel->CSLen;
							Tmp->SetUserClient(Tunnel->userserver,strlen(Tunnel->userserver)+1);
							delete Tunnel;

							send(Tmp->ServerSocket,"QUIT",5,0);
							send(Tmp->ClientSocket,"631 Переключение на другой сервер",34,0);

							Tmp->SayServerClient(iIndex);
							return 0;
						}
					}
				}
				break;
			}
			break;

		case WM_TASKBAR:
			if(wParam==ICON_ID)
				if(lParam ==WM_LBUTTONDOWN) {PopupMenu(hWnd);return 0;}
			break;
		

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDM_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				   break;
				case IDM_TRAY:
					
						// Создаем значок
					TaskBarAddIcon(hWnd,ICON_ID, LoadIcon(hInst, (LPCTSTR)IDI_SMALL), 
									WM_TASKBAR, szHint);
					StatusTray=1;
					// Скрываем окно
					ShowWindow(hWnd, SW_HIDE);

				   break;

				case IDM_CLEARHISTORY:
					a = SendMessage(hListBox2,LB_GETCOUNT,0,0L);
					for (i=0;i<a;i++)
						SendMessage(hListBox2,LB_DELETESTRING,i,0L);
					break;
				case IDM_SWITCHHISTORY:
					StatusHistory=!StatusHistory;
					if(StatusHistory){
						SendMessage(hListBox2,LB_ADDSTRING,0,(LPARAM)(LPSTR)"000000 Протоколирование включено");
						iSt++;}
					break;
				case IDM_RESTORE:
						// Регистрируем сообщение о перезапуске Explorer`а
					
					if(StatusTray){StatusTray=0;TaskBarDeleteIcon(hWnd, ICON_ID);}
					// Скрываем окно
					ShowWindow(hWnd, SW_SHOWNORMAL);

				   break;
				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_DESTROY:
			closesocket(ss);
			WSACleanup();

			a=SendMessage(hListBox,LB_GETCOUNT,0,0L);
			for (i=0;i<a;i++)
			if(!(Tunnel = (CTunnel *)SendMessage(hListBox,LB_GETITEMDATA,i,0L))) continue;
			else delete Tunnel;

			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}



int TaskBarAddIcon( HWND hWindow,UINT id, HICON hIcon,UINT CallbackMessage,	PCHAR Tip)
 {
  NOTIFYICONDATA nid;

  memset(&nid,0,sizeof(NOTIFYICONDATA));
  
  nid.cbSize = sizeof(NOTIFYICONDATA);
  nid.hWnd   = hWindow;
  nid.uID    = id;
  nid.uFlags = NIF_MESSAGE|NIF_ICON|NIF_TIP;
  nid.uCallbackMessage = CallbackMessage;
  nid.hIcon  = hIcon;
  strncpy(nid.szTip, Tip, sizeof(nid.szTip));
  
  return Shell_NotifyIcon(NIM_ADD, &nid);

}


int TaskBarDeleteIcon(HWND hWindow,UINT id){

	NOTIFYICONDATA nid;
	memset(&nid,0,sizeof(NOTIFYICONDATA));  

	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd   = hWindow;
	nid.uID    = id;
  return Shell_NotifyIcon(NIM_DELETE, &nid);
}


void PopupMenu(HWND hWnd){

  HMENU hMenu;
  HMENU hPopup;
  POINT P;
  
  hMenu = LoadMenu(hInst, (LPCSTR) MENU_TRAY);
  hPopup = GetSubMenu(hMenu, 0);

  GetCursorPos(&P);
  SetForegroundWindow(hWnd);
  TrackPopupMenu(hPopup, TPM_LEFTALIGN|TPM_LEFTBUTTON,P.x, P.y, 0, hWnd, NULL);
  PostMessage(hWnd, WM_NULL, 0, 0);
  DestroyMenu(hMenu);
}


