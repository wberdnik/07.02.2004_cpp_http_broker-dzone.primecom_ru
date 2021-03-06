#define STRICT
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include "lab5.h"

// �������� ������������� �������
extern  CALLBACK _export fundll1(LPSTR);

typedef void (CALLBACK _export *pfundll)(LPSTR);
pfundll fundll;
HINSTANCE HDll;

char szAppName[]="����p���p��� p����� N 5";
HINSTANCE hInstance; //������������p �p��������

HWND MainWindow;

static int GlobalData; //����������� (�����) ������

CRITICAL_SECTION Sect1; //��� �p���������� y������
static BOOL CritSects; //������������/�� ������������ Critical Sections



//������� �y����� �p��p����
#pragma argsused
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance,
                   LPSTR lpszCmdParam, int nCmdShow)
{
  MSG Msg;
  
  //������p���� ����
  if(!hPrevInstance)
    if(!Register(hInst))
      return FALSE;

  //�������� ����
  MainWindow=Create(hInst, nCmdShow);
  if(!MainWindow)
    return FALSE;

  //���� ��p������ ���������
  while (GetMessage(&Msg, NULL, 0, 0)) //����p�� ��������� �� ���p���
  {
    TranslateMessage(&Msg); //��p������ ���������
    DispatchMessage(&Msg);  //��p����� ��������� � �y����� ����
  }

  return Msg.wParam;
}


//������p���� ����
BOOL Register(HINSTANCE hInst)
{
  WNDCLASSEX WndClassEx; //p����p����� ����� ����

  WndClassEx.cbSize=sizeof(WNDCLASSEX);
  WndClassEx.style=CS_HREDRAW | CS_VREDRAW;
  WndClassEx.lpfnWndProc=WndProc; //������� �p����yp�
  WndClassEx.cbClsExtra=0;
  WndClassEx.cbWndExtra=0;
  WndClassEx.hInstance=hInst; //����p����p �p��������
  //������� (32x32) ������p���� �p��������
  WndClassEx.hIcon=LoadIcon(NULL, IDI_APPLICATION);
  WndClassEx.hCursor=LoadCursor(NULL, IDC_ARROW); //�yp��p ����
  WndClassEx.hbrBackground=(HBRUSH)(COLOR_WINDOW+1); //��� ����
  WndClassEx.lpszMenuName="MENU_1"; //���� ����
  WndClassEx.lpszClassName=szAppName; //��� ������ ����
  //��������� (16x16) ������p���� �p��������
  WndClassEx.hIconSm=LoadIcon(NULL, IDI_APPLICATION);

  return (RegisterClassEx(&WndClassEx)!=0); //������p���� ������ ����
}


//�������� ����
HWND Create(HINSTANCE hInst, int nCmdShow)
{
  hInstance=hInst;

  //�������� ����
  HWND hwnd=CreateWindow(szAppName, szAppName,
                         WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT,
                         CW_USEDEFAULT, CW_USEDEFAULT,
                         NULL, NULL, hInst, NULL);

  if(hwnd==NULL)
    return hwnd;

  ShowWindow(hwnd, nCmdShow);
  UpdateWindow(hwnd);

  return hwnd;
}


//�y����� ����, ��p����������� ���������
LRESULT WINAPI WndProc(HWND hwnd, UINT Message,
                       WPARAM wParam, LPARAM lParam)
{
        // ��p������� ���������
        switch(Message)
        {
          HANDLE_MSG(hwnd, WM_CREATE, Lab5_OnCreate);
          HANDLE_MSG(hwnd, WM_DESTROY, Lab5_OnDestroy);
          HANDLE_MSG(hwnd, WM_COMMAND, Lab5_OnCommand);

          default:
            //������� �p����yp� ��p������ ��������� �� y��������
            return Lab5_DefProc(hwnd, Message, wParam, lParam);
        }
}


//�y����� ��p������ ��������� WM_CREATE
#pragma argsused
BOOL Lab5_OnCreate(HWND hwnd, CREATESTRUCT FAR* lpCreateStruct)
{
  //�������� 2-� ListBox'��

  CreateWindow("listbox", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
               20, 20, 200, 340, hwnd, HMENU(ID_LISTBOX1),
               hInstance, NULL);

  CreateWindow("listbox", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
               230, 20, 200, 340, hwnd, HMENU(ID_LISTBOX2),
               hInstance, NULL);

  //������������� ��p������� �p���������� y������
  InitializeCriticalSection(&Sect1); 
  CritSects=FALSE; //������� �� ������������ ����p�������� �������

  return TRUE;
}


//�y����� ��p������ ��������� WM_DESTROY
#pragma argsused
void Lab5_OnDestroy(HWND hwnd)
{
  //������� ��p�����y� �p��������� ������
  DeleteCriticalSection(&Sect1);

  //��p������ ��������� � ����p����� � ���p��� ��������� �p��������
  PostQuitMessage(0);
}


//�y����� ��p������ ��������� WM_COMMAND
#pragma argsused
void Lab5_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{

  switch(id)
  {
    case CM_RUNTHREADS: //������� � ���y����� ������
      RunThread(hwnd);
      break;

    case CM_CRITSECS: //���/���� ����p�������� � ������� Critical Section
    {
      CritSects=!CritSects; //���/���� ������

      //"�������" ��p�� �y����� ����
      HMENU Menu=GetMenu(hwnd);
      if(CritSects)
        CheckMenuItem(Menu, CM_CRITSECS, MF_BYCOMMAND | MF_CHECKED);
      else
        CheckMenuItem(Menu, CM_CRITSECS, MF_BYCOMMAND | MF_UNCHECKED);

      break;
    }

    case CM_DLL1:
    {
      fundll1((LPSTR)"������ �� DLL!");
      break;
    }
    case CM_DLL2:
    {
      HDll=LoadLibrary("dll2.dll");
      fundll = (pfundll)GetProcAddress((HMODULE)HDll,(LPCSTR)"fundll2");
      fundll((LPSTR)"������ �� DLL!");
      FreeLibrary((HMODULE)HDll);
      break;
    }

    default:
      break;
  }

  //��p����� ��������� �y����� ���� �� y��������
  FORWARD_WM_COMMAND(hwnd, id, hwndCtl, codeNotify, DefWindowProc);
}


//�p����yp� ������ 1
//������� �p�������� ����� 3 � p���������� ��p������� GlobalData, 
//����� �������� �� ���� ����� 3; ������� �������� GlobalData �
//ListBox1
unsigned long __stdcall ThreadFunc1(LPVOID)
{
  int i, j;
  char S[100];

  //�������� ������
  ListBox_ResetContent(GetDlgItem(MainWindow, ID_LISTBOX1));

  for(j=0; j<20; j++)
  {
    //����� � �p��������� y������
    if(CritSects) EnterCriticalSection(&Sect1); 

    Sleep(5); //�������� �p����� ��� ����������

    GlobalData+=3;
    i=GlobalData-3;
    sprintf(S, "����p�����: %d", i);

    //�������� � ������ �������� GlobalData
    ListBox_AddString(GetDlgItem(MainWindow, ID_LISTBOX1), S);
    GlobalData-=3;

    //����p���� �p�������� y������
    if(CritSects) LeaveCriticalSection(&Sect1);
  }

  return 0;
}


//�p����yp� ������ 2
//������� �������� ����� 3 �� p���������� ��p������� GlobalData, 
//����� �p�������� � ���y ����� 3; ������� �������� GlobalData �
//ListBox2
unsigned long __stdcall ThreadFunc2(LPVOID)
{
  int i, j;
  char S[100];

  //�������� ������
  ListBox_ResetContent(GetDlgItem(MainWindow, ID_LISTBOX2));

  for(j=0; j<20; j++)
  {
    //����� � �p��������� y������
    if(CritSects) EnterCriticalSection(&Sect1);

    Sleep(10); //�������� �p����� ��� ����������

    GlobalData-=3;
    i=GlobalData+3;
    sprintf(S, "����p�����: %d", i);

    //�������� � ������ �������� GlobalData
    ListBox_AddString(GetDlgItem(MainWindow, ID_LISTBOX2), S);
    GlobalData+=3;

    //����p���� �p�������� y������
    if(CritSects) LeaveCriticalSection(&Sect1);
  }

  return 0;
}


//�p����yp� �������� �������
void RunThread(HWND hwnd)
{
  DWORD ThreadID1, ThreadID2;
  HANDLE ThreadHandles[2];

  GlobalData=100; //����������p����� p��������y� ��p�����y�

  //�������� 2-� �������
  ThreadHandles[0]=CreateThread(NULL, 0, ThreadFunc1, NULL, 0, &ThreadID1);
  ThreadHandles[1]=CreateThread(NULL, 0, ThreadFunc2, NULL, 0, &ThreadID2);

  if(!ThreadHandles[0] || !ThreadHandles[1])
    MessageBox(hwnd, "������ �������� �������!", "������!",
               MB_OK | MB_ICONHAND);
}

