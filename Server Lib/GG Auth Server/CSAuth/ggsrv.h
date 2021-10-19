#ifndef _GG_AUTH_SERVER_H_
#define _GG_AUTH_SERVER_H_

#ifndef _WIN32
	typedef unsigned short	WORD;
	typedef unsigned long	DWORD;
	typedef void*			LPVOID;
	typedef int            BOOL;
#endif
								
#ifdef _EXPORT_DLL
	#define GGAUTHS_API    extern "C" __declspec(dllexport)
	#define GGAUTHS_EXPORT __declspec(dllexport)
	#define __CDECL        __cdecl
#else
  	#define GGAUTHS_API extern "C"
	//#define GGAUTHS_API  <== ggsrv_cpp_c.h ���� Ȱ��ȭ. extern "C" �� c �����Ϸ��� �νĸ���.
	#define GGAUTHS_EXPORT 
	#define __CDECL 
#endif

#define	NPGG_INFO_SUCCESS					0	//Get info success.
#define	NPGG_INFO_ERROR_NOTENOUGHFMEMORY	1 //Need more memory

#define NPGG_CHECKUPDATED_VERIFIED			0 //csa.CheckUpdated() �Լ� ���ϰ�. ���� ���� �����
#define NPGG_CHECKUPDATED_NOTREADY			1 //csa.CheckUpdated() �Լ� ���ϰ�. �����˻縦 �ϱ� ��. ������ ���� ������ ����.
#define NPGG_CHECKUPDATED_HIGH				2 //csa.CheckUpdated() �Լ� ���ϰ�. ���� �������� ���� �����
#define NPGG_CHECKUPDATED_LOW				3 //csa.CheckUpdated() �Լ� ���ϰ�. ���� �������� ���� �����

#define	NPLOG_DEBUG	0x00000001	// ����� �޼���
#define	NPLOG_ERROR	0x00000002  // ���� �޼���

#define NPLOG_ENABLE_DEBUG 0x00000001 // ����� �޼��� ������
#define NPLOG_ENABLE_ERROR 0x00000002 // ���� �޼��� ������

#define	NPGG_USER_AUTH_QUERY	0x00000001 //GGAuthGetUserValue() �� ���� �÷��� Query
#define	NPGG_USER_AUTH_ANSWER	0x00000002 //GGAuthGetUserValue() �� ���� �÷��� Answer
//
#define NPGG_USER_AUTH_INDEX	0x00000010 //GGAuthGetUserValue() �� ���� �÷��� index
#define NPGG_USER_AUTH_VALUE1	0x00000020 //GGAuthGetUserValue() �� ���� �÷��� Value1
#define NPGG_USER_AUTH_VALUE2	0x00000040 //GGAuthGetUserValue() �� ���� �÷��� Value2
#define NPGG_USER_AUTH_VALUE3	0x00000080 //GGAuthGetUserValue() �� ���� �÷��� Value3


// gameguard auth data
typedef struct _GG_AUTH_DATA
{
	DWORD dwIndex;
	DWORD dwValue1;
	DWORD dwValue2;
	DWORD dwValue3;
} GG_AUTH_DATA, *PGG_AUTH_DATA;

// gameguard version data
typedef struct _GG_VERSION
{
	DWORD	dwGGVer;
	WORD	wYear;
	WORD	wMonth;
	WORD	wDay;
	WORD	wNum;
} GG_VERSION, *PGG_VERSION;

typedef struct _GG_AUTH_PROTOCOL *PGG_AUTH_PROTOCOL;

// ggauth.dll�� Path
GGAUTHS_API DWORD __CDECL InitGameguardAuth(char* sGGPath, DWORD dwNumActive, BOOL useTimer, int useLog);
GGAUTHS_API void  __CDECL CleanupGameguardAuth();
GGAUTHS_API DWORD __CDECL GGAuthUpdateTimer(); //useTimer �� true �� ��� ȣ��.

// protocol dll name
GGAUTHS_API DWORD __CDECL AddAuthProtocol(char* sDllName);

//Need for modify by gameserver engineer !!!!!!!!!!!!!!!!
GGAUTHS_API void NpLog(int mode, char* msg);
/*
{
	if(mode & (NPLOG_DEBUG | NPLOG_ERROR)) //Select log mode. 
#ifdef _WIN32
		OutputDebugString(msg);
#else
		printf(msg); 
#endif
};
*/

typedef struct _GG_UPREPORT
{
	DWORD	dwBefore;   // Before version
	DWORD	dwNext;		// Now version
	int		nType;		// Update type 1 : gameguard version, 2 : protocol num
} GG_UPREPORT, *PGG_UPREPORT;

//Need for modify by gameserver engineer !!!!!!!!!!!!!!!!
GGAUTHS_API void GGAuthUpdateCallback(PGG_UPREPORT report);
/*
{
	printf("GGAuth version update [%s] : [%ld] -> [%ld] \n", 
			report->nType==1?"GameGuard Ver":"Protocol Num", 
			report->dwBefore,
			report->dwNext); 
};
*/

// protocol information
GGAUTHS_API int ModuleInfo(char* dest, int length);

// Class CSAuth2
class GGAUTHS_EXPORT CCSAuth2
{
public:
	// Constructor
	CCSAuth2();

	// Destructor
	~CCSAuth2();

protected:
	PGG_AUTH_PROTOCOL m_pProtocol;
	DWORD m_bPrtcRef;
	DWORD m_dwUserFlag;
	GG_VERSION m_GGVer;	//���Ӱ��� ����
	GG_AUTH_DATA m_AuthQueryTmp;	//m_AuthQuery ���� ����صд�.

	BOOL m_bNewProtocol; // ���ο� �������ݷ� �����ϸ� true. ���ο� ���������� �Ҽ�ȭ�Ǹ� false, ����Ʈ false;

	BOOL m_bActive; //���� ��ü�� ����ڿ��� �Ҵ�Ǿ�����. Init ȣ���� true, Close ȣ���� false

public:	
	GG_AUTH_DATA m_AuthQuery;
	GG_AUTH_DATA m_AuthAnswer;

	void  Init();
	DWORD GetAuthQuery();
	DWORD CheckAuthAnswer();	
	void  Close();
	int	  Info(char* dest, int length); // protocol information
	int	  CheckUpdated(); //���� ������� ������Ʈ ���θ� Ȯ���Ѵ�.
};

// C type CSAuth2
typedef LPVOID          LPGGAUTH;

GGAUTHS_API LPGGAUTH __CDECL GGAuthCreateUser();                                              // CCSAuth2()
GGAUTHS_API DWORD     __CDECL GGAuthDeleteUser(LPGGAUTH pGGAuth);                             // ~CCSAuth2()
GGAUTHS_API DWORD	 __CDECL GGAuthInitUser(LPGGAUTH pGGAuth);                              // Init()
GGAUTHS_API DWORD     __CDECL GGAuthCloseUser(LPGGAUTH pGGAuth);                             // Close()
GGAUTHS_API DWORD    __CDECL GGAuthGetQuery(LPGGAUTH pGGAuth, PGG_AUTH_DATA pAuthData);     // GetAuthQuery()
GGAUTHS_API DWORD    __CDECL GGAuthCheckAnswer(LPGGAUTH pGGAuth, PGG_AUTH_DATA pAuthData);  // CheckAuthAnswer()
GGAUTHS_API int      __CDECL GGAuthCheckUpdated(LPGGAUTH pGGAuth);  // CheckAuthAnswer()
GGAUTHS_API int      __CDECL GGAuthUserInfo(LPGGAUTH pGGAuth, char* dest, int length);  // CheckAuthAnswer()
GGAUTHS_API DWORD	 __CDECL GGAuthGetUserValue(LPGGAUTH pGGAuth, int type);
#endif

