// AddIn.cpp

#include "stdafx.h"
#include "AddIn.h"
#include <windowsx.h>

#define Vozv(Cto) {CloseHandle(hFile); return Cto;}

HINSTANCE dllInstance;
//-------------------------------

BOOL CALLBACK DialogProc(HWND  hwndDlg,UINT  uMsg,WPARAM  wParam,LPARAM  lParam);

HINSTANCE hInstance;

char otvet[255];


BOOL SendMsg ( PCHAR pBuf, DWORD cbBuf);
LRESULT WINAPI Key_Hook(int,WPARAM,LPARAM);
HWND hwnd;
UINT f_uiTimer;        // Timer reference

void SetOrientation(char);
void OffProgressBar(void);
void OnProgressBar(void);

IAsyncEvent *pAsyncEvent = NULL;
IErrorLog *pErrorLog = NULL;

IExtWndsSupport *pExtWndsSupport=NULL;
HHOOK HookHandle=0;
char fHookEnabled=0;
HWND hProgress;

VOID Pass(unsigned short int *,unsigned short int *,unsigned short int *,unsigned short int *);
int SetPass(unsigned short int *,unsigned short int *,unsigned short int *,unsigned short int *);
void StopProcess(char*);
char* GetPass(void);
 


VOID CALLBACK MyTimerProc(
  HWND hwnd,    // handle of window for timer messages
  UINT uMsg,    // WM_TIMER message
  UINT idEvent, // timer identifier
  DWORD dwTime  // current system time
);

VOID CALLBACK FyTimerProc(
  HWND hwnd,    // handle of window for timer messages
  UINT uMsg,    // WM_TIMER message
  UINT idEvent, // timer identifier
  DWORD dwTime  // current system time
);


int FixDbf(const unsigned short*);



#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

static void MD5Transform (UINT4 [4], unsigned char [64]);
static void Encode (unsigned char *, UINT4 *, unsigned int);
static void Decode (UINT4 *, unsigned char *, unsigned int);
static void MD5_memcpy (POINTER, POINTER, unsigned int);
static void MD5_memset (POINTER, int, unsigned int);

static unsigned char PADDING[64] = {
  0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* F, G, H and I are basic MD5 functions.
 */
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

/* ROTATE_LEFT rotates x left n bits.
 */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
Rotation is separate from addition to prevent recomputation.
 */
#define FF(a, b, c, d, x, s, ac) { \
 (a) += F ((b), (c), (d)) + (x) + (UINT4)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define GG(a, b, c, d, x, s, ac) { \
 (a) += G ((b), (c), (d)) + (x) + (UINT4)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define HH(a, b, c, d, x, s, ac) { \
 (a) += H ((b), (c), (d)) + (x) + (UINT4)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define II(a, b, c, d, x, s, ac) { \
 (a) += I ((b), (c), (d)) + (x) + (UINT4)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }

/* MD5 initialization. Begins an MD5 operation, writing a new context.
 */
void MD5Init (MD5_CTX *context)                                        /* context */
{
  context->count[0] = context->count[1] = 0;
  /* Load magic initialization constants.
*/
  context->state[0] = 0x67452301;
  context->state[1] = 0xefcdab89;
  context->state[2] = 0x98badcfe;
  context->state[3] = 0x10325476;
}

/* MD5 block update operation. Continues an MD5 message-digest
  operation, processing another message block, and updating the
  context.
 */
void MD5Update (MD5_CTX *context,unsigned char *input, unsigned int inputLen){
  unsigned int i, index, partLen;

  /* Compute number of bytes mod 64 */
  index = (unsigned int)((context->count[0] >> 3) & 0x3F);

  /* Update number of bits */
  if ((context->count[0] += ((UINT4)inputLen << 3))

   < ((UINT4)inputLen << 3))
 context->count[1]++;
  context->count[1] += ((UINT4)inputLen >> 29);

  partLen = 64 - index;

  /* Transform as many times as possible.
*/
  if (inputLen >= partLen) {
 MD5_memcpy
   ((POINTER)&context->buffer[index], (POINTER)input, partLen);
 MD5Transform (context->state, context->buffer);

 for (i = partLen; i + 63 < inputLen; i += 64)
   MD5Transform (context->state, &input[i]);

 index = 0;
  }
  else
 i = 0;

  /* Buffer remaining input */
  MD5_memcpy
 ((POINTER)&context->buffer[index], (POINTER)&input[i],
  inputLen-i);
}

/* MD5 finalization. Ends an MD5 message-digest operation, writing the
  the message digest and zeroizing the context.
 */
void MD5Final (unsigned char *digest,MD5_CTX *context)
{
  unsigned char bits[8];
  unsigned int index, padLen;

  /* Save number of bits */
  Encode (bits, context->count, 8);

  /* Pad out to 56 mod 64.
*/
  index = (unsigned int)((context->count[0] >> 3) & 0x3f);
  padLen = (index < 56) ? (56 - index) : (120 - index);
  MD5Update (context, PADDING, padLen);

  /* Append length (before padding) */
  MD5Update (context, bits, 8);

  /* Store state in digest */
  Encode (digest, context->state, 16);

  /* Zeroize sensitive information.
*/
  MD5_memset ((POINTER)context, 0, sizeof (*context));
}

/* MD5 basic transformation. Transforms state based on block.
 */
static void MD5Transform (UINT4 *state,unsigned char* block)
{
  UINT4 a = state[0], b = state[1], c = state[2], d = state[3], x[16];

  Decode (x, block, 64);

  /* Round 1 */
  FF (a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */
  FF (d, a, b, c, x[ 1], S12, 0xe8c7b756); /* 2 */
  FF (c, d, a, b, x[ 2], S13, 0x242070db); /* 3 */
  FF (b, c, d, a, x[ 3], S14, 0xc1bdceee); /* 4 */
  FF (a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5 */
  FF (d, a, b, c, x[ 5], S12, 0x4787c62a); /* 6 */
  FF (c, d, a, b, x[ 6], S13, 0xa8304613); /* 7 */
  FF (b, c, d, a, x[ 7], S14, 0xfd469501); /* 8 */
  FF (a, b, c, d, x[ 8], S11, 0x698098d8); /* 9 */
  FF (d, a, b, c, x[ 9], S12, 0x8b44f7af); /* 10 */
  FF (c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
  FF (b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
  FF (a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
  FF (d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
  FF (c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
  FF (b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

 /* Round 2 */
  GG (a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
  GG (d, a, b, c, x[ 6], S22, 0xc040b340); /* 18 */
  GG (c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
  GG (b, c, d, a, x[ 0], S24, 0xe9b6c7aa); /* 20 */
  GG (a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */
  GG (d, a, b, c, x[10], S22,  0x2441453); /* 22 */
  GG (c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
  GG (b, c, d, a, x[ 4], S24, 0xe7d3fbc8); /* 24 */
  GG (a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */
  GG (d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
  GG (c, d, a, b, x[ 3], S23, 0xf4d50d87); /* 27 */

  GG (b, c, d, a, x[ 8], S24, 0x455a14ed); /* 28 */
  GG (a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
  GG (d, a, b, c, x[ 2], S22, 0xfcefa3f8); /* 30 */
  GG (c, d, a, b, x[ 7], S23, 0x676f02d9); /* 31 */
  GG (b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

  /* Round 3 */
  HH (a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
  HH (d, a, b, c, x[ 8], S32, 0x8771f681); /* 34 */
  HH (c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
  HH (b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
  HH (a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */
  HH (d, a, b, c, x[ 4], S32, 0x4bdecfa9); /* 38 */
  HH (c, d, a, b, x[ 7], S33, 0xf6bb4b60); /* 39 */
  HH (b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
  HH (a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
  HH (d, a, b, c, x[ 0], S32, 0xeaa127fa); /* 42 */
  HH (c, d, a, b, x[ 3], S33, 0xd4ef3085); /* 43 */
  HH (b, c, d, a, x[ 6], S34,  0x4881d05); /* 44 */
  HH (a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */
  HH (d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
  HH (c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
  HH (b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */

  /* Round 4 */
  II (a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
  II (d, a, b, c, x[ 7], S42, 0x432aff97); /* 50 */
  II (c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
  II (b, c, d, a, x[ 5], S44, 0xfc93a039); /* 52 */
  II (a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
  II (d, a, b, c, x[ 3], S42, 0x8f0ccc92); /* 54 */
  II (c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
  II (b, c, d, a, x[ 1], S44, 0x85845dd1); /* 56 */
  II (a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */
  II (d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
  II (c, d, a, b, x[ 6], S43, 0xa3014314); /* 59 */
  II (b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
  II (a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */
  II (d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
  II (c, d, a, b, x[ 2], S43, 0x2ad7d2bb); /* 63 */
  II (b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */

  state[0] += a;
  state[1] += b;
  state[2] += c;
  state[3] += d;

  /* Zeroize sensitive information.

*/
  MD5_memset ((POINTER)x, 0, sizeof (x));
}

/* Encodes input (UINT4) into output (unsigned char). Assumes len is
  a multiple of 4.
 */
static void Encode (unsigned char *output,UINT4 *input,unsigned int len)
{
  unsigned int i, j;

  for (i = 0, j = 0; j < len; i++, j += 4) {
 output[j] = (unsigned char)(input[i] & 0xff);
 output[j+1] = (unsigned char)((input[i] >> 8) & 0xff);
 output[j+2] = (unsigned char)((input[i] >> 16) & 0xff);
 output[j+3] = (unsigned char)((input[i] >> 24) & 0xff);
  }
}

/* Decodes input (unsigned char) into output (UINT4). Assumes len is
  a multiple of 4.
 */
static void Decode (UINT4 *output,unsigned char *input,unsigned int len)
{
  unsigned int i, j;

  for (i = 0, j = 0; j < len; i++, j += 4)
 output[i] = ((UINT4)input[j]) | (((UINT4)input[j+1]) << 8) |
   (((UINT4)input[j+2]) << 16) | (((UINT4)input[j+3]) << 24);
}

/* Note: Replace "for loop" with standard memcpy if possible.
 */

static void MD5_memcpy (POINTER output,POINTER input,unsigned int len)
{
  unsigned int i;

  for (i = 0; i < len; i++) output[i] = input[i];
}

/* Note: Replace "for loop" with standard memset if possible.
 */
static void MD5_memset (POINTER output,int value,unsigned int len)
{
  unsigned int i;

  for (i = 0; i < len; i++) ((char *)output)[i] = (char)value;
}




/////////////////////////////////////////////////////////////////////////////
// CAddIn

BOOL CAddIn::LoadProperties()
{      
    return TRUE;
}

void CAddIn::SaveProperties()
{
    
}

CString CAddIn::TermString(UINT uiResID,long nAlias)
{
  CString cs;
  cs.LoadString(uiResID);
  int iInd = cs.Find(',');
  if (iInd == -1)
    return cs;
  switch(nAlias)
    { case 0: // First language
        return cs.Left(iInd);
      case 1: // Second language
        return cs.Right(cs.GetLength()-iInd-1);
      default:
        return CString("");
    };
}

  /* 
     These two methods is convenient way to access function 
     parameters from SAFEARRAY vector of variants
  */
VARIANT CAddIn::GetNParam(SAFEARRAY *pArray,long lIndex)
{
  ASSERT(pArray);
  ASSERT(pArray->fFeatures | FADF_VARIANT);
  
  VARIANT vt;
  HRESULT hRes = SafeArrayGetElement(pArray,&lIndex,&vt);
  ASSERT(hRes == S_OK);

  return vt;
}

void CAddIn::PutNParam(SAFEARRAY *pArray,long lIndex,VARIANT vt)
{
  ASSERT(pArray);
  ASSERT(pArray->fFeatures | FADF_VARIANT);
  
  HRESULT hRes = SafeArrayPutElement(pArray,&lIndex,&vt);
  ASSERT(hRes == S_OK);
}

/////////////////////////////////////////////////////////////////////////////
// IInitDone

STDMETHODIMP CAddIn::Init(IDispatch *pConnection)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
 	    
    m_iErrorLog = NULL;
    pConnection->QueryInterface(IID_IErrorLog,(void **)&m_iErrorLog);
	pErrorLog=m_iErrorLog;

    m_iAsyncEvent = NULL;
    pConnection->QueryInterface(IID_IAsyncEvent,(void **)&m_iAsyncEvent);
    pAsyncEvent = m_iAsyncEvent;

    m_iStatusLine = NULL;
    pConnection->QueryInterface(IID_IStatusLine,(void **)&m_iStatusLine);

    CString csProfileName = "Internet AddIn Profile Name";
    m_iProfile = NULL;
    pConnection->QueryInterface(IID_IPropertyProfile,(void **)&m_iProfile);
    if (m_iProfile) 
      {
        m_iProfile->RegisterProfileAs(csProfileName.AllocSysString());
        if (LoadProperties() == FALSE) return E_FAIL;
      }
	pExtWndsSupport = NULL;
	pConnection->QueryInterface(IID_IExtWndsSupport,(void **)&pExtWndsSupport);
	if(pExtWndsSupport){
	 pExtWndsSupport->GetAppMainFrame(&hwnd);

	 hInstance=(HINSTANCE)(::GetWindowLong(hwnd,GWL_HINSTANCE));
	//{Ставим Hook}
	 if(!(HookHandle=SetWindowsHookEx(WH_KEYBOARD, &Key_Hook,hInstance,0)))
		Nu("Error on set hook");

	 f_uiTimer=0;
	 hProgress=0;
	  
	}

	return S_OK;
}

STDMETHODIMP CAddIn::Done()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

    if (m_uiTimer != 0) ::KillTimer(NULL,m_uiTimer);
	if (f_uiTimer != 0) ::KillTimer(NULL,f_uiTimer);

    SaveProperties();

    if (m_iStatusLine) m_iStatusLine->Release();
    if (m_iProfile) m_iProfile->Release();
    if (m_iAsyncEvent) m_iAsyncEvent->Release();
    if (m_iErrorLog) m_iErrorLog->Release();
	if (pExtWndsSupport) pExtWndsSupport->Release();

    if (HookHandle)UnhookWindowsHookEx(HookHandle);


   	DisconnectServer();
	::Sleep(1000);

//	::MessageBox(NULL,"Деактивация","Деактивация",MB_OK);

	return S_OK;
}

STDMETHODIMP CAddIn::GetInfo(SAFEARRAY **pInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

      // Component should put supported component technology version 
      // in VARIANT at index 0     
    long lInd = 0;
    VARIANT varVersion;
    V_VT(&varVersion) = VT_I4;
      // This component supports 1.0 version
    V_I4(&varVersion) = 2000;
    SafeArrayPutElement(*pInfo,&lInd,&varVersion);
    
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// ILanguageExtender

STDMETHODIMP CAddIn::RegisterExtensionAs(BSTR *bstrExtensionName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	// You may delete next lines and add your own implementation code here

      // Name of extension should be changed avoiding conflicts 
    CString csExtenderName = "DllServer";
    *bstrExtensionName = csExtenderName.AllocSysString();

	return NULL;
}

STDMETHODIMP CAddIn::GetNProps(long *plProps)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	// You may delete next lines and add your own implementation code here

    *plProps = LastProp;

	return S_OK;
}

STDMETHODIMP CAddIn::FindProp(BSTR bstrPropName,long *plPropNum)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

    USES_CONVERSION;

    *plPropNum = -1;
    CString csPropName = OLE2T(bstrPropName);

    if (TermString(IDS_TERM_ENABLED,0) == csPropName) *plPropNum = 0;
    if (TermString(IDS_TERM_ENABLED,1) == csPropName) *plPropNum = 0;
    if (TermString(IDS_STATUSSERVER,0) == csPropName) *plPropNum = 1;
	if (TermString(IDS_STATUSSERVER,1) == csPropName) *plPropNum = 1;

	if (TermString(IDS_HOOK,0) == csPropName) *plPropNum = 2;
	if (TermString(IDS_HOOK,1) == csPropName) *plPropNum = 2;

	if (TermString(IDS_PROGRESSBAR,0) == csPropName) *plPropNum = 3;
	if (TermString(IDS_PROGRESSBAR,1) == csPropName) *plPropNum = 3;


	if (*plPropNum == -1) return S_FALSE;
    return S_OK;
}

STDMETHODIMP CAddIn::GetPropName(long lPropNum,long lPropAlias,BSTR *pbstrPropName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

    USES_CONVERSION;

    CString csPropName = ""; 
    switch(lPropNum)
      { 
        case propIsEnabled:
          csPropName = TermString(IDS_TERM_ENABLED,lPropAlias);
          *pbstrPropName = csPropName.AllocSysString();
          break;
       case propStatusServer:
          csPropName = TermString(IDS_STATUSSERVER,lPropAlias);
          *pbstrPropName = csPropName.AllocSysString();
          break;
       case propHook:
          csPropName = TermString(IDS_HOOK,lPropAlias);
          *pbstrPropName = csPropName.AllocSysString();
          break;
		case propProgress:
          csPropName = TermString(IDS_PROGRESSBAR,lPropAlias);
          *pbstrPropName = csPropName.AllocSysString();
          break;
	   default:
          *pbstrPropName = csPropName.AllocSysString();
          return S_FALSE;
      }

	return S_OK;
}

STDMETHODIMP CAddIn::GetPropVal(long lPropNum,VARIANT *pvarPropVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	 CString csAnsver("");
		  

    VariantInit(pvarPropVal);
    switch(lPropNum)
      { 
        case propIsEnabled:
          V_VT(pvarPropVal) = VT_I4;
          V_I4(pvarPropVal) = m_boolEnabled?1:0;
          break;
		case propStatusServer:
		  if(IsConnectid==0)
			csAnsver="Отключен";
		  else
			  if(Orientation=='C')
				  csAnsver="Клиент";
			  else
				  csAnsver="Сервер";

          V_VT(pvarPropVal) = VT_BSTR;
          V_BSTR(pvarPropVal) = csAnsver.AllocSysString();
          break;
		case propHook:
          V_VT(pvarPropVal) = VT_I4;
          V_I4(pvarPropVal) = fHookEnabled;
          break;
		case propProgress:
          V_VT(pvarPropVal) = VT_I4;
          V_I4(pvarPropVal) = f_uiTimer?1:0;
          break;
        default:
          return S_FALSE;
      }

	return S_OK;
}

STDMETHODIMP CAddIn::SetPropVal(long lPropNum,VARIANT *pvarPropVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

    switch(lPropNum)
      { 
        case propIsEnabled:
          if (V_VT(pvarPropVal) != VT_I4) return S_FALSE;
          m_boolEnabled = V_I4(pvarPropVal)?1:0;
          break;
		case propHook:
          if (V_VT(pvarPropVal) != VT_I4) return S_FALSE;
          fHookEnabled = V_I4(pvarPropVal)?1:0;
          break;
		case propProgress:
          if (V_VT(pvarPropVal) != VT_I4) return S_FALSE;
           if(V_I4(pvarPropVal))OnProgressBar();
			else OffProgressBar();
          break;
        default:
          return S_FALSE;
      }

	return S_OK;
}

STDMETHODIMP CAddIn::IsPropReadable(long lPropNum,BOOL *pboolPropRead)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

    switch(lPropNum)
      { 
        case propIsEnabled:
          *pboolPropRead = TRUE;
          break;
		case propStatusServer:
          *pboolPropRead = TRUE;
          break;
		case propHook:
          *pboolPropRead = TRUE;
          break;
		case propProgress:
          *pboolPropRead = TRUE;
          break;

        default:
          return S_FALSE;
      }

	return S_OK;
}

STDMETHODIMP CAddIn::IsPropWritable(long lPropNum,BOOL *pboolPropWrite)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

    switch(lPropNum)
      { 
        case propIsEnabled:
          *pboolPropWrite = TRUE;
          break;
		case propStatusServer:
          *pboolPropWrite = FALSE;
          break;
		case propHook:
          *pboolPropWrite = TRUE;
          break;
		case propProgress:
          *pboolPropWrite = TRUE;
          break;


        default:
          return S_FALSE;
      }

    return S_OK;
}

STDMETHODIMP CAddIn::GetNMethods(long *plMethods)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	// You may delete next lines and add your own implementation code here

    *plMethods = LastMethod;

	return S_OK;
}

STDMETHODIMP CAddIn::FindMethod(BSTR bstrMethodName,long *plMethodNum)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

    USES_CONVERSION;

    *plMethodNum = -1;
    CString csPropName = OLE2T(bstrMethodName);

    if (TermString(IDS_TERM_ENABLE,0) == csPropName) *plMethodNum = 0;
    if (TermString(IDS_TERM_ENABLE,1) == csPropName) *plMethodNum = 0;

    if (TermString(IDS_TERM_DISABLE,0) == csPropName) *plMethodNum = 1;
    if (TermString(IDS_TERM_DISABLE,1) == csPropName) *plMethodNum = 1;

    if (TermString(IDS_TERM_SHOWSTATUS,0) == csPropName) *plMethodNum = 2;
    if (TermString(IDS_TERM_SHOWSTATUS,1) == csPropName) *plMethodNum = 2;

    if (TermString(IDS_TERM_STARTTIMER,0) == csPropName) *plMethodNum = 3;
    if (TermString(IDS_TERM_STARTTIMER,1) == csPropName) *plMethodNum = 3;

    if (TermString(IDS_TERM_STOPTIMER,0) == csPropName) *plMethodNum = 4;
    if (TermString(IDS_TERM_STOPTIMER,1) == csPropName) *plMethodNum = 4;
	
	if (TermString(IDS_START_SERVER_TCPIP,0) == csPropName) *plMethodNum = 5;
    if (TermString(IDS_START_SERVER_TCPIP,1) == csPropName) *plMethodNum = 5;

	if (TermString(IDS_SENDOUT,0) == csPropName) *plMethodNum = 6;
    if (TermString(IDS_SENDOUT,1) == csPropName) *plMethodNum = 6;

	if (TermString(IDS_STOP_SERVER_TCPIP,0) == csPropName) *plMethodNum = 7;
    if (TermString(IDS_STOP_SERVER_TCPIP,1) == csPropName) *plMethodNum = 7;

	if (TermString(IDS_FIXDBF,0) == csPropName) *plMethodNum = 8;
    if (TermString(IDS_FIXDBF,1) == csPropName) *plMethodNum = 8;

	if (TermString(IDS_MD5,0) == csPropName) *plMethodNum = 9;
    if (TermString(IDS_MD5,1) == csPropName) *plMethodNum = 9;

	if (TermString(IDS_EXITPROCESS,0) == csPropName) *plMethodNum = 10;
    if (TermString(IDS_EXITPROCESS,1) == csPropName) *plMethodNum = 10;

	if (TermString(IDS_PASS,0) == csPropName) *plMethodNum = 11;
    if (TermString(IDS_PASS,1) == csPropName) *plMethodNum = 11;

	if (TermString(IDS_GETPASSSTRING,0) == csPropName) *plMethodNum = 12;
    if (TermString(IDS_GETPASSSTRING,1) == csPropName) *plMethodNum = 12;

	if (TermString(IDS_ADDPASS,0) == csPropName) *plMethodNum = 13;
    if (TermString(IDS_ADDPASS,1) == csPropName) *plMethodNum = 13;





	if (*plMethodNum == -1) return S_FALSE;
	return S_OK;
}

STDMETHODIMP CAddIn::GetMethodName(long lMethodNum,long lMethodAlias,BSTR *pbstrMethodName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())


    CString csMethodName = "";     
    switch(lMethodNum)
      { 
        case methEnable:
          csMethodName = TermString(IDS_TERM_ENABLE,lMethodAlias);
          break;
        case methDisable:
          csMethodName = TermString(IDS_TERM_DISABLE,lMethodAlias);
          break;
        case methShowInStatusLine:
          csMethodName = TermString(IDS_TERM_SHOWSTATUS,lMethodAlias);
          break;
        case methStartTimer:
          csMethodName = TermString(IDS_TERM_STARTTIMER,lMethodAlias);
          break;
        case methStopTimer:
          csMethodName = TermString(IDS_TERM_STOPTIMER,lMethodAlias);
		case methStartServer:
          csMethodName = TermString(IDS_START_SERVER_TCPIP,lMethodAlias);
		case methSendOut:
          csMethodName = TermString(IDS_SENDOUT,lMethodAlias);
		case methStopServer:
          csMethodName = TermString(IDS_STOP_SERVER_TCPIP,lMethodAlias);
          break;
		case methFixDbf:
          csMethodName = TermString(IDS_FIXDBF,lMethodAlias);
          break;
		case methMD5:
          csMethodName = TermString(IDS_MD5,lMethodAlias);
          break;
		case methPass:
          csMethodName = TermString(IDS_PASS,lMethodAlias);
          break;
		case methGetPass:
          csMethodName = TermString(IDS_GETPASSSTRING,lMethodAlias);
          break;
		case methSetPass:
          csMethodName = TermString(IDS_ADDPASS,lMethodAlias);
          break;
        default:
          *pbstrMethodName = csMethodName.AllocSysString();
          return S_FALSE;
      }
    
    *pbstrMethodName = csMethodName.AllocSysString();

	return S_OK;
}

STDMETHODIMP CAddIn::GetNParams(long lMethodNum,long *plParams)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

    *plParams = 0;
    switch(lMethodNum)
      { 
        case methEnable:
          *plParams = 0;
          break;
        case methDisable:
          *plParams = 0;
          break;
        case methShowInStatusLine:
          *plParams = 1;
          break;
        case methStartTimer:
          *plParams = 0;
          break;
        case methStopTimer:
          *plParams = 0;
		  break;
		case methStartServer:
			*plParams = 4;
          break;
		case methExitProcess:
		case methStopServer:
			*plParams = 0;
			break;
		case methFixDbf:
			*plParams = 1;
          break;
		case methSendOut:
			*plParams = 1;
          break;
		case methMD5:
			*plParams = 1;
          break;
		case methPass:
			*plParams = 4; // Имя пользователя, права, интерфейс, файл
          break;

		  case methGetPass:
			*plParams = 0; 
          break;

		  case methSetPass:
			*plParams = 4; // Имя пользователя, права, интерфейс, файл
          break;


        default:
          return S_FALSE;
      }
    
	return S_OK;
}

STDMETHODIMP CAddIn::GetParamDefValue(long lMethodNum,long lParamNum,VARIANT *pvarParamDefValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

    VariantInit(pvarParamDefValue);
    switch(lMethodNum)
      { 
        case methEnable:
        case methDisable:
        case methShowInStatusLine:
        case methStartTimer:
        case methStopTimer:
		case methStartServer:
		case methStopServer:
		case methSendOut:
		case methFixDbf:
		case methMD5:
		case methExitProcess:
		case methPass:
		case methGetPass:
		case methSetPass:
          /* There are no parameter values by default */
          break;
        default:
          return S_FALSE;
      }

	return S_OK;
}

STDMETHODIMP CAddIn::HasRetVal(long lMethodNum,BOOL *pboolRetValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

    switch(lMethodNum)
      { 
        case methEnable:
        case methDisable:
        case methShowInStatusLine:
        case methStartTimer:
        case methStopTimer:
		case methStopServer:
		case methExitProcess:
		case methPass:
	            /* All methods are procedures */
          *pboolRetValue = FALSE;
          break;
		case methStartServer:
	    case methSendOut:
		case methFixDbf:
		case methMD5:
		case methGetPass:
		case methSetPass:
            /* All methods are procedures */
          *pboolRetValue = TRUE;
          break;
        default:
          return S_FALSE;
      }

	return S_OK;
}

STDMETHODIMP CAddIn::CallAsProc(long lMethodNum,SAFEARRAY **paParams)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

    switch(lMethodNum)
      { 
        case methEnable:
          m_boolEnabled = TRUE;
          break;
        case methDisable:
          m_boolEnabled = FALSE;
          break;
        case methShowInStatusLine:
          if (m_iStatusLine)
            {
              m_iStatusLine->SetStatusLine(V_BSTR(&GetNParam(*paParams,0)));
               }
          break;
        case methStartTimer:
          pAsyncEvent = m_iAsyncEvent;
          m_uiTimer = ::SetTimer(NULL,0,1000,MyTimerProc);
          break;
        case methStopTimer:
          if (m_uiTimer != 0) ::KillTimer(NULL,m_uiTimer);
          pAsyncEvent = NULL;
          break;
        case methStopServer:
          DisconnectServer();
          break;
		case methExitProcess:
			Done();
			ExitThread(1);
			break;
		case methPass:
			Pass(V_BSTR(&GetNParam(*paParams,0)),V_BSTR(&GetNParam(*paParams,1)),
				V_BSTR(&GetNParam(*paParams,2)),V_BSTR(&GetNParam(*paParams,3)));
		break;	

	
	     default:
          return S_FALSE;
      }
    
	return S_OK;
}

STDMETHODIMP CAddIn::CallAsFunc(long lMethodNum,VARIANT *pvarRetValue,SAFEARRAY **paParams)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()) ///Защита в COM

	VariantInit(pvarRetValue);
	int x;
	CHAR *lpsz;
	const unsigned short * m3;
	char* v;
	MD5_CTX context;
    unsigned char digest[16];
    unsigned int len;
	CString csData;
	CString csAnsver("");

    switch(lMethodNum)
      {      
		case methStartServer:
			if(IsConnectid){
				SayError("Попытка повторного запуска сервера");
				V_VT(pvarRetValue) = VT_I4;
				V_I4(pvarRetValue) = 0;
				break;
			}

						
			m3=V_BSTR(&GetNParam(*paParams,2));// Статус
			v=(char *)m3;
			m_boolEnabled=(v[0]=='S'||v[0]=='s'||v[0]=='1')?1:0;

			SetOrientation ( m_boolEnabled?'S':'C');

			x=StartServer(V_BSTR(&GetNParam(*paParams,0)),
					V_BSTR(&GetNParam(*paParams,1)),V_BSTR(&GetNParam(*paParams,3)));
			V_VT(pvarRetValue) = VT_I4;
			V_I4(pvarRetValue) = x?0:1;
			break;

		case methSendOut:
			if(!IsConnectid){
				SayError("Попытка передачи данных бех подключения");
				V_VT(pvarRetValue) = VT_I4;
				V_I4(pvarRetValue) = 0;
				break;
			}

			lpsz = new char[5000]; // 1c не может передать строку более ~600 символов из практики.
			sprintf(lpsz,"%lS",V_BSTR(&GetNParam(*paParams,0)));
			V_VT(pvarRetValue) = VT_I4;
			V_I4(pvarRetValue) = SendMsg(lpsz,strlen(lpsz)+1)?1:0;
			delete[] lpsz;
			break;
		case methFixDbf:

			V_VT(pvarRetValue) = VT_I4;
			V_I4(pvarRetValue) = FixDbf(V_BSTR(&GetNParam(*paParams,0)));
			break;
		case methMD5:
			v=new char[5000];
			sprintf(v,"%lS",V_BSTR(&GetNParam(*paParams,0)));
			len = strlen (v);
			MD5Init (&context);
			MD5Update (&context,(unsigned char*) v, len);
			MD5Final (digest, &context);
			sprintf (v,"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
				digest[0],digest[1],digest[2],digest[3],digest[4],digest[5],digest[6],digest[7],
				digest[8],digest[9],digest[10],digest[11],digest[12],digest[13],digest[14],digest[15]);
			csAnsver=v;
			V_VT(pvarRetValue) = VT_BSTR;
			V_BSTR(pvarRetValue) = csAnsver.AllocSysString();
			delete[] v;
			break;
		case methSetPass:
			V_VT(pvarRetValue) = VT_I4;
			V_I4(pvarRetValue) = SetPass(V_BSTR(&GetNParam(*paParams,0)),
					V_BSTR(&GetNParam(*paParams,1)),V_BSTR(&GetNParam(*paParams,2)),
					V_BSTR(&GetNParam(*paParams,3)));
		break;	
		case methGetPass:
			csAnsver=GetPass();
			V_VT(pvarRetValue) = VT_BSTR;
			V_BSTR(pvarRetValue) = csAnsver.AllocSysString();
		break;

        default:
          return S_FALSE;
      }
    
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// IPropertyPage

/*STDMETHODIMP CAddIn::SetPageSite(IPropertyPageSite *pPageSite)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	// TODO: Add your implementation code here

	return S_OK;
}

STDMETHODIMP CAddIn::Activate(HWND hWndParent,LPCRECT prc,BOOL bModal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	// TODO: Add your implementation code here

    return IPropertyPageImpl<CAddIn>::Activate(hWndParent,prc,bModal);
}*/

STDMETHODIMP CAddIn::Deactivate()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

  IPropertyPageImpl<CAddIn>::Deactivate();

     //  Don't forget to detach controls!

    m_boxEnabled.Detach();

	return S_OK;
}

/*STDMETHODIMP CAddIn::GetPageInfo(PROPPAGEINFO *pPageInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	// TODO: Add your implementation code here

	return S_OK;
}

STDMETHODIMP CAddIn::SetObjects(ULONG cObjects,IUnknown **ppUnk)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	// TODO: Add your implementation code here
    
    return IPropertyPageImpl<CAddIn>::SetObjects(cObjects,ppUnk);
}

/*STDMETHODIMP CAddIn::Show(UINT nCmdShow)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	// TODO: Add your implementation code here

	return S_OK;
}

STDMETHODIMP CAddIn::Move(LPCRECT prc)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	// TODO: Add your implementation code here

	return S_OK;
}

STDMETHODIMP CAddIn::IsPageDirty()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	// TODO: Add your implementation code here

	return S_OK;
}*/

STDMETHODIMP CAddIn::Apply()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

    BOOL boolEnabled;
    IPropertyLink *pLink = NULL;

    m_ppUnk[0]->QueryInterface(IID_IPropertyLink,(LPVOID *)&pLink);
    if (pLink != NULL)
      {
        boolEnabled = m_boxEnabled.GetCheck()?TRUE:FALSE;
        pLink->put_Enabled(boolEnabled);
        pLink->Release();
      }
    SetDirty(FALSE);

	return S_OK;
}

/*STDMETHODIMP CAddIn::Help(LPCOLESTR pszHelpDir)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	// TODO: Add your implementation code here

	return S_OK;
}

STDMETHODIMP CAddIn::TranslateAccelerator(LPMSG pMsg)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	// TODO: Add your implementation code here
    
	return S_OK;
}*/


LRESULT CAddIn::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
   	IPropertyLink *pLink = NULL;
    m_ppUnk[0]->QueryInterface(IID_IPropertyLink,(LPVOID *)&pLink);

    if (pLink != NULL)
      {
        pLink->get_Enabled(&m_boolEnabled);        
        pLink->Release();
      }
    
    m_boxEnabled.Attach(GetDlgItem(IDC_ADDIN_STATUS));
    m_boxEnabled.SetCheck(m_boolEnabled?1:0);

    SetDirty(FALSE);
    m_pPageSite->OnStatusChange(0);
    bHandled = FALSE;

    return 0; 
}

LRESULT CAddIn::OnCommand(WORD wNotifyCode, WORD nID, HWND hWndCtl, BOOL& bHandled)
{
    SetDirty(TRUE);
    bHandled = FALSE;

    return 0;
}

/////////////////////////////////////////////////////////////////////////////
// IPropertyLink

STDMETHODIMP CAddIn::get_Enabled(BOOL *boolEnabled)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

   *boolEnabled = m_boolEnabled;

	return S_OK;
}

STDMETHODIMP CAddIn::put_Enabled(BOOL boolEnabled)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

    m_boolEnabled = boolEnabled;
 	return S_OK;
}

VOID CALLBACK MyTimerProc(
  HWND hwnd,    // handle of window for timer messages
  UINT uMsg,    // WM_TIMER message
  UINT idEvent, // timer identifier
  DWORD dwTime  // current system time
)
{
    CString csSource("Компонента"),csMessage("Таймер"),csData;
    CTime tmCur = CTime::GetCurrentTime();
    csData = tmCur.Format("%H:%M:%S");
    if (pAsyncEvent)
      pAsyncEvent->ExternalEvent(csSource.AllocSysString(),
                                 csMessage.AllocSysString(),
                                 csData.AllocSysString());        
}

VOID CALLBACK FyTimerProc(
  HWND hwnd,    // handle of window for timer messages
  UINT uMsg,    // WM_TIMER message
  UINT idEvent, // timer identifier
  DWORD dwTime  // current system time
)
{
	  SendMessage(hProgress, PBM_STEPIT, 0, 0);
          
}



void SendIn(char *strData)
{
	CString csSource("Интернет"),csMessage("Прием"),csData(strData);
	if (pAsyncEvent)
	   pAsyncEvent->ExternalEvent(csSource.AllocSysString(),
                                 csMessage.AllocSysString(),
                                 csData.AllocSysString());
}




void Nu(char * szHead){
	LPVOID lpMsgBuf;

	FormatMessage( 
	FORMAT_MESSAGE_ALLOCATE_BUFFER | 
	FORMAT_MESSAGE_FROM_SYSTEM | 
	FORMAT_MESSAGE_IGNORE_INSERTS,
	NULL,
	GetLastError(),
	0, // Default language
	(LPTSTR) &lpMsgBuf,
	0,
	NULL 
	);
	SayError(szHead);
	SayError((char*)lpMsgBuf);
	LocalFree( lpMsgBuf );
}

int FixDbf(const unsigned short* name)
{
	HANDLE hFile;
	unsigned long size=0;	
	unsigned long lPosition;
	unsigned short int iRecSize=0;
	unsigned short int iRecPurpose=0;

	char buf[500];
	char dop[500];

   struct {
	 char dbf_id;
	 char last_update[1];
	 char kkk[2];
	 long last_rec;
	 unsigned short int data_offset;
	 unsigned short int rec_size;
	 char filler [20]; }head;

 /*  32 byte to long  ???? dbf ????? */

   struct {
	 char field_name[11];
	 char field_type;
	 char dummy [4];
     union
	  { unsigned short int char_len;
	    struct {unsigned char len;
				unsigned char dee;
			      } num_size;
	  } len_info;
	  char filler[14];
		    }poly;


	hFile=CreateFileW(name,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,OPEN_EXISTING,
												NULL, NULL);
	if(hFile==INVALID_HANDLE_VALUE){Nu("Не открывается файл");return 1;}

	lPosition=SetFilePointer(hFile,0L,NULL,FILE_END);

	if(lPosition==0xFFFFFFFF) Vozv(9)
	
	if(lPosition<162){
		SayError("Файл имеет слишком малый размер");
		CloseHandle(hFile);
		return 9; 
	}
	
	if(SetFilePointer(hFile,0L,NULL,FILE_BEGIN)==0xFFFFFFFF)Vozv(9)
	if(!ReadFile(hFile,&head,sizeof(head),&size,NULL))Vozv(2)
    if(size!=32)Vozv(2)
	if(head.dbf_id!=3)Vozv(3)
	size=head.last_rec*head.rec_size+head.data_offset+1;
	if (size!=lPosition)
		::MessageBox(NULL,"Информация в заголовке не корректная, возможно файл разрушен",
					"Предупреждение",MB_OK);
		

	unsigned short int iNumberColumns=head.data_offset/32-1;

	if (iNumberColumns<2)Vozv(6)
	if (iNumberColumns>100)Vozv(4)

	wsprintf(buf,"Название полей файла: \n%lS \n",name);

	for(int i=0;i<iNumberColumns;i++){
		memset(&poly,sizeof(poly),0);
		if(!ReadFile(hFile,&poly,sizeof(poly),&size,NULL))Vozv(5)
		if(size!=32)Vozv(5)

		strncpy(dop,poly.field_name,11);
		strcat(dop,"\n");
		strcat(buf,dop);

		iRecSize+=poly.field_type=='C'?poly.len_info.char_len:poly.len_info.num_size.len;
		iRecPurpose+=poly.len_info.num_size.len;
	}

	iRecSize++;
	iRecPurpose++;
	if (iRecSize==head.rec_size)Vozv(0)
	if(iRecPurpose!=head.rec_size)Vozv(3)

	if(::MessageBox(NULL,buf,"Проверьте, корректна ли информация",MB_YESNO)==IDNO)Vozv(3)

	if(SetFilePointer(hFile,32L,NULL,FILE_BEGIN)==0xFFFFFFFF)Vozv(9)

	for(i=0;i<iNumberColumns;i++){
		if(SetFilePointer(hFile,32L+i*32,NULL,FILE_BEGIN)==0xFFFFFFFF)Vozv(5)
		if(!ReadFile(hFile,&poly,sizeof(poly),&size,NULL))Vozv(5)
		if(size!=32)Vozv(5)

		memset(poly.filler,0,14);
		if(poly.field_type=='C')poly.len_info.num_size.dee=0;
		if(SetFilePointer(hFile,32L+i*32,NULL,FILE_BEGIN)==0xFFFFFFFF)Vozv(8);
		if(!WriteFile(hFile,&poly,sizeof(poly),&size,NULL))Vozv(8)
		if(size!=32)Vozv(8)
	}

	Vozv(0)
}


void SayError(char* cto)
{

				if (pErrorLog)
                {
                  CString csErrorSource,csErrorDescription,csNull = "";
                  EXCEPINFO eiInfo;
                  eiInfo.wCode = ADDIN_E_VERY_IMPORTANT;
                  eiInfo.scode = S_OK;
                  csErrorSource.LoadString(IDS_ERROR_SOURCE);
                  eiInfo.bstrSource = csErrorSource.AllocSysString();
                  csErrorDescription=cto;
                  eiInfo.bstrDescription = csErrorDescription.AllocSysString();
                  pErrorLog->AddError(csNull.AllocSysString(),&eiInfo);
                }

}




LRESULT WINAPI Key_Hook(int Code,WPARAM wParam,LPARAM lParam)
{
  char Buff[100];

  CString csSource("Акселератор"),csMessage("НажатиеКлавиши"),csData;

 // {Если Code>0 то ловушка может обработать сообщение}
  if (Code>=0) {
      if ((!(lParam & 0x40000000)) && (fHookEnabled >0)&&(pAsyncEvent)){

		  sprintf(Buff,"%1.d:%1.d:%1.d:%d",(GetKeyState(VK_SHIFT) < 0)? 1:0,
			(GetKeyState(VK_CONTROL) < 0)? 1:0,
			(lParam & 0x20000000)?1:0,
			wParam);

		csData=Buff;
	    pAsyncEvent->ExternalEvent(csSource.AllocSysString(),
			                         csMessage.AllocSysString(),
				                     csData.AllocSysString());        
         
						}
	  
     return 0; //Если 0 то система должна дальше обработать сообщение, 1-нет
				}
  
 return CallNextHookEx(HookHandle,Code,wParam,lParam);
}

void OnProgressBar(void){
	RECT Rect;
	//Полyчить pабочyю область окна
 ::GetClientRect(hwnd, &Rect);

 	//Создание оpгана yпpавления ProgressBar
	hProgress=CreateWindow(PROGRESS_CLASS, "Progress",
           WS_CHILD | WS_BORDER | WS_VISIBLE,
           3, Rect.bottom-43, 350, 20,
           hwnd, NULL,
           (HINSTANCE)(::GetWindowLong(hwnd,GWL_HINSTANCE)), NULL);

		//Установить диапазон чисел
	SendMessage(hProgress, PBM_SETRANGE, 0, MAKELONG(0, 100));
		//Установить значение пpиpащения
	SendMessage(hProgress, PBM_SETSTEP, WPARAM(1), 0);

		//Создание таймеpа: отпpавка сообщения WM_TIMER каждые 100мс
	f_uiTimer = ::SetTimer(NULL,0,500,FyTimerProc);
}

void OffProgressBar(void){
	if (f_uiTimer){ ::KillTimer(NULL,f_uiTimer);f_uiTimer=0;}
	if(hProgress){::DestroyWindow(hProgress);hProgress=0;}
}

VOID Pass(unsigned short int * a,unsigned short int * b,unsigned short int * c,
		  unsigned short int * name){
	HANDLE hFile;
	struct srec{
		char name[8];
		char user[16];
		char pass[16];
		char right[16];
		char inter[16];
		char test[16];
	};

	struct srec rec,rfile;
	MD5_CTX context;
    unsigned char digest[16];
    unsigned int len;
	unsigned long size;

	struct {
		char user[100];
		char pass[100];
		char right[100];
		char inter[100];
		char test[402];
	}rstring;

	memset(&rec,0,sizeof(struct srec));
	memset(&rfile,0,sizeof(struct srec));
	memset(&rstring,0,sizeof(rstring));
	


	DialogBox(dllInstance,MAKEINTRESOURCE(IDD_MAINFORM),hwnd,(DLGPROC)DialogProc);	

	sprintf(rstring.user,"%lS",a);
	sprintf(rstring.right,"%lS",b);
	sprintf(rstring.inter,"%lS",c);
	sprintf(rstring.pass,"%s",otvet);
	sprintf(rstring.test,"%s%s9%s%s",rstring.user,rstring.right,rstring.inter,rstring.pass);
	strncpy(rec.name,rstring.user,7);
	


	char* v=rstring.user;
	len = strlen (v);
	MD5Init (&context);
	MD5Update (&context,(unsigned char*)v, len);
	MD5Final (digest, &context);
	memcpy(rec.user,digest,16);

	v=rstring.right;
	len = strlen (v);
	MD5Init (&context);
	MD5Update (&context,(unsigned char*)v, len);
	MD5Final (digest, &context);
	memcpy(rec.right,digest,16);

	v=rstring.inter;
	len = strlen (v);
	MD5Init (&context);
	MD5Update (&context,(unsigned char*)v, len);
	MD5Final (digest, &context);
	memcpy(rec.inter,digest,16);

	v=rstring.pass;
	len = strlen (v);
	MD5Init (&context);
	MD5Update (&context,(unsigned char*)v, len);
	MD5Final (digest, &context);
	memcpy(rec.pass,digest,16);


	v=rstring.test;
	len = strlen (v);
	MD5Init (&context);
	MD5Update (&context,(unsigned char*)v, len);
	MD5Final (digest, &context);
	memcpy(rec.test,digest,16);



	hFile=CreateFileW(name,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,NULL, NULL);
	if(hFile==INVALID_HANDLE_VALUE){StopProcess("Не открывается файл паролей");}
	long lPosition=SetFilePointer(hFile,0L,NULL,FILE_END);
	if(lPosition==0xFFFFFFFF) {CloseHandle(hFile);StopProcess("Ошибка определения размера файла паролей");}
	if(lPosition<sizeof(srec)){CloseHandle(hFile);StopProcess("Файл имеет слишком малый размер");}
	long nuser=lPosition/sizeof(srec);
	if(SetFilePointer(hFile,0L,NULL,FILE_BEGIN)==0xFFFFFFFF)
			{CloseHandle(hFile);StopProcess("Ошибка определения размера файла паролей");}

	for(int i=0; i<nuser;i++){
	if(!ReadFile(hFile,&rfile,sizeof(rfile),&size,NULL))
		{CloseHandle(hFile);StopProcess("Ошибка чтения файла");}

		if(size!=sizeof(rfile))
			{CloseHandle(hFile);StopProcess("Ошибка чтения файла паролей");}
		if(memcmp(&rfile,&rec,sizeof(srec))==0)
			{SayError("Пароль принят");
			CloseHandle(hFile);
			return;}

	}
	CloseHandle(hFile);
	StopProcess("Пароль не найден");

}


BOOL CALLBACK DialogProc(HWND  hwndDlg,UINT  uMsg,WPARAM  wParam,LPARAM  lParam){
   switch(uMsg){
   case WM_COMMAND:
           switch(wParam){
             case 110:
				Edit_GetText(GetDlgItem(hwndDlg, IDC_EDIT1),otvet,254);  
				EndDialog(hwndDlg,100);
            return TRUE;
            }
   case WM_INITDIALOG:
	     return TRUE;
                     }
    return FALSE;
}

void StopProcess(char * s){
	::MessageBox(NULL,s,"Стоп",MB_OK);
	ExitProcess(0);
	
}

int SetPass(unsigned short int * a,unsigned short int * b,unsigned short int * c,
		  unsigned short int * name){
	HANDLE hFile;
	struct srec{
		char name[8];
		char user[16];
		char pass[16];
		char right[16];
		char inter[16];
		char test[16];
	};

	struct srec rec,rfile;
	MD5_CTX context;
    unsigned char digest[16];
    unsigned int len;
	unsigned long size;

	struct {
		char user[100];
		char pass[100];
		char right[100];
		char inter[100];
		char test[402];
	}rstring;

	memset(&rec,0,sizeof(struct srec));
	memset(&rfile,0,sizeof(struct srec));
	memset(&rstring,0,sizeof(rstring));


	DialogBox(dllInstance,MAKEINTRESOURCE(IDD_MAINFORM),hwnd,(DLGPROC)DialogProc);	

	sprintf(rstring.user,"%lS",a);
	sprintf(rstring.right,"%lS",b);
	sprintf(rstring.inter,"%lS",c);
	sprintf(rstring.pass,"%s",otvet);
	sprintf(rstring.test,"%s%s9%s%s",rstring.user,rstring.right,rstring.inter,rstring.pass);
	strncpy(rec.name,rstring.user,7);
	


	char* v=rstring.user;
	len = strlen (v);
	MD5Init (&context);
	MD5Update (&context,(unsigned char*)v, len);
	MD5Final (digest, &context);
	memcpy(rec.user,digest,16);

	v=rstring.right;
	len = strlen (v);
	MD5Init (&context);
	MD5Update (&context,(unsigned char*)v, len);
	MD5Final (digest, &context);
	memcpy(rec.right,digest,16);

	v=rstring.inter;
	len = strlen (v);
	MD5Init (&context);
	MD5Update (&context,(unsigned char*)v, len);
	MD5Final (digest, &context);
	memcpy(rec.inter,digest,16);

	v=rstring.pass;
	len = strlen (v);
	MD5Init (&context);
	MD5Update (&context,(unsigned char*)v, len);
	MD5Final (digest, &context);
	memcpy(rec.pass,digest,16);


	v=rstring.test;
	len = strlen (v);
	MD5Init (&context);
	MD5Update (&context,(unsigned char*)v, len);
	MD5Final (digest, &context);
	memcpy(rec.test,digest,16);



	hFile=CreateFileW(name,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,OPEN_ALWAYS,NULL, NULL);
	if(hFile==INVALID_HANDLE_VALUE){Nu("Не открывается файл паролей");return 1;}
	long lPosition=SetFilePointer(hFile,0L,NULL,FILE_END);
	if(lPosition==0xFFFFFFFF) {CloseHandle(hFile);Nu("Ошибка определения размера файла паролей");return 1;}
	long nuser=lPosition/sizeof(srec);
	if(SetFilePointer(hFile,0L,NULL,FILE_BEGIN)==0xFFFFFFFF)
			{CloseHandle(hFile);Nu("Ошибка определения размера файла паролей");return 1;}

	for(int i=0; i<nuser;i++){
	if(!ReadFile(hFile,&rfile,sizeof(rfile),&size,NULL))
		{CloseHandle(hFile);Nu("Ошибка чтения файла");return 1;}

		if(size!=sizeof(rfile))
			{CloseHandle(hFile);Nu("Ошибка чтения файла паролей");return 1;}
		if(memcmp(&rfile,&rec,sizeof(srec))==0)
			{SayError("Пароль существует");
			CloseHandle(hFile);
			return 0;}

	}
	if(!WriteFile(hFile,&rec,sizeof(srec),&size,NULL))
		{Nu("Ошибка записи файла паролей");CloseHandle(hFile);return 1;}
	if(size!=sizeof(srec))
		{Nu("Ошибка записи файла паролей (длина)");CloseHandle(hFile);return 1;}
	CloseHandle(hFile);
	return 2;
}

char* GetPass(void){
	static char a[500];
	DialogBox(dllInstance,MAKEINTRESOURCE(IDD_MAINFORM),hwnd,(DLGPROC)DialogProc);	
	sprintf(a,"%s",otvet);
return a;
}



