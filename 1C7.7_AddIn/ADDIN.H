// AddIn.h: Definition of the CAddIn class
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ADDIN_H__FC223DE1_F108_11D0_A459_004095E1DAEA__INCLUDED_)
#define AFX_ADDIN_H__FC223DE1_F108_11D0_A459_004095E1DAEA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "resource.h"       // main symbols
#include "AddInTlb.h"
#include "atlctl.h"
void SayError(char*);
void Nu(char*);
void SendIn(char *strData);
BOOL DisconnectServer(void);
int WINAPI StartServer(const unsigned short * node,const unsigned short * port,
					   const unsigned short * name);

extern HINSTANCE dllInstance;

extern HANDLE ThreadHandle;
extern int IsConnectid;
extern IAsyncEvent *pAsyncEvent;
extern IErrorLog *pErrorLog;
extern char Orientation;



typedef unsigned char *POINTER;
typedef unsigned short int UINT2;
typedef unsigned long int UINT4;

typedef struct {
  UINT4 state[4];                                   /* state (ABCD) */
  UINT4 count[2];        /* number of bits, modulo 2^64 (lsb first) */
  unsigned char buffer[64];                         /* input buffer */
} MD5_CTX;


void MD5Init (MD5_CTX *);
void MD5Update (MD5_CTX *, unsigned char *, unsigned int);
void MD5Final (unsigned char *, MD5_CTX *);



enum AddInErrors {
                  ADDIN_E_NONE = 1000,
                  ADDIN_E_ORDINARY = 1001,
                  ADDIN_E_ATTENTION = 1002,
                  ADDIN_E_IMPORTANT = 1003,
                  ADDIN_E_VERY_IMPORTANT = 1004,
                  ADDIN_E_INFO = 1005,
                  ADDIN_E_FAIL = 1006,
                  ADDIN_E_MSGBOX_ATTENTION = 1007,
                  ADDIN_E_MSGBOX_INFO = 1008,
                  ADDIN_E_MSGBOX_FAIL = 1009
                 };

class CAddIn;

/////////////////////////////////////////////////////////////////////////////
// CAddIn

class CAddIn : 
    public IInitDone,
    public ILanguageExtender,
    public IPropertyLink,
    public CComObjectRoot,
    public CComCoClass<CAddIn,&CLSID_AddIn>,
    public IPropertyPageImpl<CAddIn>,
    public CDialogImpl<CAddIn>
{
    enum 
    {
        propIsEnabled = 0,
		propStatusServer,
		propHook,
		propProgress,
        LastProp      // Always last
    };

    enum 
    {
        methEnable = 0,
        methDisable,
        methShowInStatusLine,
        methStartTimer,
        methStopTimer,
		methStartServer,
		methSendOut,
		methStopServer,
		methFixDbf,
		methMD5,
		methExitProcess,
		methPass,
		methGetPass,
		methSetPass,
		LastMethod      // Always last
    };

public:
	CAddIn() { m_dwTitleID = IDS_PROPPAGE_CAPTION; }

// Attributes
    IErrorLog *m_iErrorLog;
    IPropertyProfile *m_iProfile;
    IStatusLine *m_iStatusLine;
    IAsyncEvent *m_iAsyncEvent;

    UINT m_uiTimer;        // Timer reference
    BOOL m_boolEnabled;    // This property enables or disables component
    CButton m_boxEnabled;  // Corresponded check box

// Methods

    BOOL LoadProperties();
    void SaveProperties();
      // This method is very useful in ILanguageExtender implementation
    CString TermString(UINT uiResID,long nAlias);
      // These two methods is convenient way to access function 
      // parameters from SAFEARRAY vector of variants
    VARIANT GetNParam(SAFEARRAY *pArray,long lIndex);
    void PutNParam(SAFEARRAY *pArray,long lIndex,VARIANT vt);

// Interfaces
BEGIN_COM_MAP(CAddIn)
    COM_INTERFACE_ENTRY(IInitDone)
    COM_INTERFACE_ENTRY(ILanguageExtender)
    COM_INTERFACE_ENTRY(IPropertyLink)
    COM_INTERFACE_ENTRY_IMPL(IPropertyPage)
END_COM_MAP()
//DECLARE_NOT_AGGREGATABLE(CAddIn) 
// Remove the comment from the line above if you don't want your object to 
// support aggregation. 

enum {IDD = IDD_ADDIN_PROP_PAGE};

LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
LRESULT OnCommand(WORD wNotify, WORD wID, HWND hWnd, BOOL& bHandled);

BEGIN_MSG_MAP(CAddIn)
    MESSAGE_HANDLER(WM_INITDIALOG,OnInitDialog)
      // Sample WM_COMMAND handler
    COMMAND_CODE_HANDLER(BN_CLICKED,OnCommand)
    CHAIN_MSG_MAP(IPropertyPageImpl<CAddIn>)
    COMMAND_HANDLER(IDC_ADDIN_STATUS, BN_CLICKED, OnClickedAddin_status)
END_MSG_MAP()

DECLARE_REGISTRY_RESOURCEID(IDR_AddIn)

// IInitDone
public:
    STDMETHOD(Init)(IDispatch *pConnection);
    STDMETHOD(Done)();
    STDMETHOD(GetInfo)(SAFEARRAY **pInfo);
// ILanguageExtender
public:

    STDMETHOD(RegisterExtensionAs)(BSTR *bstrExtensionName);

    STDMETHOD(GetNProps)(long *plProps);
    STDMETHOD(FindProp)(BSTR bstrPropName,long *plPropNum);
    STDMETHOD(GetPropName)(long lPropNum,long lPropAlias,BSTR *pbstrPropName);
    STDMETHOD(GetPropVal)(long lPropNum,VARIANT *pvarPropVal);
    STDMETHOD(SetPropVal)(long lPropNum,VARIANT *pvarPropVal);
    STDMETHOD(IsPropReadable)(long lPropNum,BOOL *pboolPropRead);
    STDMETHOD(IsPropWritable)(long lPropNum,BOOL *pboolPropWrite);

    STDMETHOD(GetNMethods)(long *plMethods);
    STDMETHOD(FindMethod)(BSTR bstrMethodName,long *plMethodNum);
    STDMETHOD(GetMethodName)(long lMethodNum,long lMethodAlias,BSTR *pbstrMethodName);
    STDMETHOD(GetNParams)(long lMethodNum,long *plParams);
    STDMETHOD(GetParamDefValue)(long lMethodNum,long lParamNum,VARIANT *pvarParamDefValue);
    STDMETHOD(HasRetVal)(long lMethodNum,BOOL *pboolRetValue);
    STDMETHOD(CallAsProc)(long lMethodNum,SAFEARRAY **paParams);
    STDMETHOD(CallAsFunc)(long lMethodNum,VARIANT *pvarRetValue,SAFEARRAY **paParams);

/* This interface implements connection between property page and object itself.
   Remember: object and its property page are implemented in ONE class but
   they work in different instances. So we should specify additional link between them
   and override SetObjects.
*/
// IPropertyLink
public:
    STDMETHOD(get_Enabled)(BOOL *boolEnabled);
    STDMETHOD(put_Enabled)(BOOL boolEnabled);
    
// IPropertyPage
public: 
      /* Unnecessary functions are simply commented instead of calling 
         parent class implementation 
      */
    //STDMETHOD(SetPageSite)(IPropertyPageSite *pPageSite);
    //STDMETHOD(Activate)(HWND hWndParent,LPCRECT prc,BOOL bModal);
    STDMETHOD(Deactivate)();
    //STDMETHOD(GetPageInfo)(PROPPAGEINFO *pPageInfo);
    //STDMETHOD(SetObjects)(ULONG cObjects,IUnknown **ppUnk);
    //STDMETHOD(Show)(UINT nCmdShow);
    //STDMETHOD(Move)(LPCRECT prc);
    //STDMETHOD(IsPageDirty)();
    STDMETHOD(Apply)();
    //STDMETHOD(Help)(LPCOLESTR pszHelpDir);
    //STDMETHOD(TranslateAccelerator)(LPMSG pMsg);
	LRESULT OnClickedAddin_status(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{

		// TODO : Add Code for control notification handler.
		return 0;
	}
};

#endif // !defined(AFX_ADDIN_H__FC223DE1_F108_11D0_A459_004095E1DAEA__INCLUDED_)
