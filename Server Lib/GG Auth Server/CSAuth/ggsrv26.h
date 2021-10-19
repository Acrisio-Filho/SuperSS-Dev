#ifndef _GG_AUTH_SERVER_H_
#define _GG_AUTH_SERVER_H_

// Pack padr�o
#pragma pack(push, ggsrv)
#pragma pack( )

#if !defined(_WIN32) && !defined(_WIN64)
    #include <stdint.h>

	// 64��Ʈ ���������� ����� ������ Ÿ�� ����  - Coded By HS_Soul [2007.12.28]
	typedef int32_t			INT32, *PINT32;
	typedef uint32_t		UINT32, *PUINT32;
	typedef int64_t			INT64, *PINT64;
	typedef uint64_t		UINT64, *PUINT64;

	typedef intptr_t		INT_PTR;
	typedef uintptr_t		UINT_PTR;
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

#ifndef ERROR_SUCCESS
	#define ERROR_SUCCESS								0		//	�Ϲ����� ����
#endif
#define NPGG_INFO_SUCCESS								0		//	GetInfo() func success.

#define NPGG_INFO_ERROR_NOTENOUGHFMEMORY			1		//	Need more memory
#define ERROR_GGAUTH_FAIL_MEM_ALLOC					1		//	CS���� ��� �ε��� �޸� ����
#define ERROR_GGAUTH_FAIL_LOAD_DLL					2		//	CS���� ���(*.dll, *.so) �ε� ����
#define ERROR_GGAUTH_FAIL_GET_PROC					3		//	CS���� ����� export �Լ� ���� ����
#define ERROR_GGAUTH_FAIL_BEFORE_INIT				4		//	������ ���� <- critical error
#define ERROR_GGAUTH_FAIL_LOAD_CFG					5		//	�������� ��� ���� ���� �ε� ����.
#define ERROR_GGAUTH_INVALID_PARAM					10		// �Լ� ȣ��� �߸��� ���ڰ��� �Ѱ��� ���

#define ERROR_GGAUTH_NO_REPLY							11		// �������. GetAuthQuery() �� CheckAuthAnswer()�ϱ����� �ι� �ҷ����� �������� �߻�
#define ERROR_GGAUTH_INVALID_PROTOCOL_VERSION	12		// CS���� ��� �������� ��ȣ(ggauth##.dll) Ʋ��
#define ERROR_GGAUTH_INVALID_REPLY					13		// �˰����� ���� ���䰪 Ʋ��.
#define ERROR_GGAUTH_INVALID_GAMEGUARD_VER		14		//	���Ӱ��� �����˻�. ���̺귯�� 2.5���� �߰���(cs������� 50�� �̻�)

// CS26 �߰� 15 ~ 19
#define ERROR_GGAUTH_SAME_CLIENT_DETECTED			15		// ���� ���Ӱ��� ����� ����
#define ERROR_GGAUTH_CLIENT_STOPPED					16		// ���Ӱ��尡 �������� ����
#define ERROR_GGAUTH_CLIENT_AUTH_ERROR				17		// ���Ӱ��� ���� ������ ������ (���� ���Ӱ��� or ���� ����)
#define ERROR_GGAUTH_INVALID_PACKET					18		// ����� �ùٸ��� ����
#define ERROR_GGAUTH_INVALID_CRC1					19		// ���� ��Ŷ�� �ùٸ��� �ʰų� ������
#define ERROR_GGAUTH_INVALID_CRC2					20		// ���� ��Ŷ�� �ùٸ��� �ʰų� ������
#define ERROR_GGAUTH_CLIENT_HACK_DETECTED			21		// ���Ӱ��忡�� Hack �� �߰��Ͽ� �������� ���� ������


#define ERROR_GGAUTH_SETSTATE_ERROR					50		// SetCSAuthState() ȣ�� �� m_CSAuthState�� NULL�̰ų�..
																		// m_PrtcTemp�� NULL �� ��� (pProtocolHead �� ã�� �� ���� ���...)

#define ERROR_GGAUTH_INVALID_GAMEMON_VER			101	//
#define ERROR_GGAUTH_INVALID_GAMEMON_VER_CODE	102	//

#define ERROR_GGAUTH_RETRY_QUERY						200

#define NPGG_CHECKUPDATED_VERIFIED			0 //csa.CheckUpdated() �Լ� ���ϰ�. ���� ���� �����
#define NPGG_CHECKUPDATED_NOTREADY			1 //csa.CheckUpdated() �Լ� ���ϰ�. �����˻縦 �ϱ� ��. ������ ���� ������ ����.
#define NPGG_CHECKUPDATED_HIGH				2 //csa.CheckUpdated() �Լ� ���ϰ�. ���� �������� ���� �����
#define NPGG_CHECKUPDATED_LOW					3 //csa.CheckUpdated() �Լ� ���ϰ�. ���� �������� ���� �����

#define	NPLOG_DEBUG	0x00000001	// ����� �޼��� 
#define	NPLOG_ERROR	0x00000002  // ���� �޼��� 

#define NPLOG_ENABLE_DEBUG 0x00000001 // ����� �޼��� ������
#define NPLOG_ENABLE_ERROR 0x00000002 // ���� �޼��� ������

// gameguard auth data
typedef struct _GG_AUTH_DATA
{
	UINT32 dwIndex;
	UINT32 dwValue1;
	UINT32 dwValue2;
	UINT32 dwValue3;
} GG_AUTH_DATA, *PGG_AUTH_DATA;

// gameguard version data
typedef struct _GG_VERSION
{
	UINT32	dwGGVer;
	unsigned short	wYear;
	unsigned short	wMonth;
	unsigned short	wDay;
	unsigned short	wNum;
} GG_VERSION, *PGG_VERSION;

// Data for Server <-> Server (�������� ����, ���Ӱ��� ���ι���, Flag)
typedef struct _GG_CSAUTH_STATE
{
	UINT32	m_PrtcVersion;
	UINT32	m_GGVersion;
	UINT32	m_UserFlag;
} GG_CSAUTH_STATE, *PGG_CSAUTH_STATE;

typedef struct _GG_AUTH_PROTOCOL *PGG_AUTH_PROTOCOL;

GGAUTHS_API UINT32 __CDECL InitGameguardAuth(char* sGGPath, UINT32 dwNumActive, int useTimer, int useLog);
GGAUTHS_API void  __CDECL CleanupGameguardAuth();
GGAUTHS_API UINT32 __CDECL GGAuthUpdateTimer(); //useTimer �� true �� ��� ȣ��.
GGAUTHS_API UINT32 __CDECL AddAuthProtocol(char* sDllName);
GGAUTHS_API UINT32 __CDECL SetGGVerLimit(UINT32 nLimitVer);	//���� ���Ѽ��� �����Ѵ�.
GGAUTHS_API UINT32 __CDECL SetUpdateCondition(int nTimeLimit, int nCondition); //������Ʈ ������ �����Ѵ�.
GGAUTHS_API UINT32 __CDECL CheckCSAuth(bool bCheck);	// ���� ���� ��� On/Off �� ���� �Լ�
GGAUTHS_API int	 __CDECL DecryptHackData(char* lpszUserKey, LPVOID lpData, DWORD dwLength);
GGAUTHS_API int ModuleInfo(char* dest, int length);
GGAUTHS_API void NpLog(int mode, char* msg); // referenced by 
//{
//	if(mode & (NPLOG_DEBUG | NPLOG_ERROR)) // for examples 
//#ifdef _WIN32
//		printf(msg);
//#else
//		printf(msg); 
//#endif
//};

typedef struct _GG_UPREPORT
{
	UINT32	dwBefore;   // Before version
	UINT32	dwNext;		// Now version
	int		nType;		// Update type 1 : gameguard version, 2 : protocol num
} GG_UPREPORT, *PGG_UPREPORT;

GGAUTHS_API void GGAuthUpdateCallback(PGG_UPREPORT report); // referenced by 
//{
//	printf("GGAuth version update [%s] : [%ld] -> [%ld] \n", // for examples
//			report->nType==1?"GameGuard Ver":"Protocol Num", 
//			report->dwBefore,
//			report->dwNext); 
//};

#define MY_CSAuth2 FALSE

#if MY_CSAuth2 != TRUE

// Class CSAuth2
class GGAUTHS_EXPORT CCSAuth2
{
public:
	// Constructor
	CCSAuth2();

	// Destructor
	~CCSAuth2();

public:	
	GG_AUTH_DATA m_AuthQuery;
	GG_AUTH_DATA m_AuthAnswer;

	DWORD m_dwUniqValue1;
	DWORD m_dwUniqValue2;
	CCSAuth2 *m_pNext;

protected:
	bool m_bAuth;
	PGG_AUTH_PROTOCOL m_pProtocol;
	UINT32 m_bPrtcRef;
	UINT32 m_dwUserFlag;
	GG_VERSION m_GGVer;				//���Ӱ��� ����
	GG_AUTH_DATA m_AuthQueryTmp;	//m_AuthQuery ���� ����صд�.
	bool m_bAllowOldVersion;		// 2009.05.14

	int m_nSequenceNum;
	DWORD m_dwServerKey;
	DWORD m_dwLastValue4;

	BYTE  m_byLastLoop1;
	BYTE  m_byLastLoop2;
	DWORD  m_dwLoop1AuthArray[0x100];
	DWORD  m_dwLoop2AuthArray[0x100];

public:
	void  Init();
	UINT32 GetAuthQuery();
	UINT32 CheckAuthAnswer();
	UINT32 CheckUserCSAuth(bool bCheck);
	inline void InitCSAuthState(PGG_CSAUTH_STATE m_CSAuthState) { memset(m_CSAuthState, 0, sizeof(GG_CSAUTH_STATE)); };
	UINT32 GetCSAuthState(PGG_CSAUTH_STATE m_CSAuthState);
	UINT32 SetCSAuthState(PGG_CSAUTH_STATE m_CSAuthState);
	UINT32 SetSecretOrder();
	void  Close();
	int	  Info(char* dest, int length); // protocol information
	int	  CheckUpdated(); //���� ������� ������Ʈ ���θ� Ȯ���Ѵ�.	

	void	AllowOldVersion();	// 2009.05.14 - �����̵� ������ ���ؼ� �������� ����Ͽ� ���� ������ ���´�.
};

#endif // MY_CSAuth2 != TRUE


// C type CSAuth2
#define NPGG_USER_AUTH_QUERY	0x00000001 //GGAuthGetUserValue() �� ���� �÷��� Query
#define NPGG_USER_AUTH_ANSWER	0x00000002 //GGAuthGetUserValue() �� ���� �÷��� Answer
#define NPGG_USER_AUTH_INDEX	0x00000010 //GGAuthGetUserValue() �� ���� �÷��� index
#define NPGG_USER_AUTH_VALUE1	0x00000020 //GGAuthGetUserValue() �� ���� �÷��� Value1
#define NPGG_USER_AUTH_VALUE2	0x00000040 //GGAuthGetUserValue() �� ���� �÷��� Value2
#define NPGG_USER_AUTH_VALUE3	0x00000080 //GGAuthGetUserValue() �� ���� �÷��� Value3

typedef void*          LPGGAUTH;

//. Do not support c type functions at CS26
#if MY_CSAuth2 == TRUE

GGAUTHS_EXPORT LPGGAUTH __CDECL GGAuthCreateUser();                                             // CCSAuth2()
GGAUTHS_EXPORT UINT32   __CDECL GGAuthDeleteUser(LPGGAUTH pGGAuth);                             // ~CCSAuth2()
GGAUTHS_EXPORT UINT32	__CDECL GGAuthInitUser(LPGGAUTH pGGAuth);											// Init()
GGAUTHS_EXPORT UINT32   __CDECL GGAuthCloseUser(LPGGAUTH pGGAuth);										// Close()
GGAUTHS_EXPORT UINT32   __CDECL GGAuthGetQuery(LPGGAUTH pGGAuth, PGG_AUTH_DATA pAuthData);		// GetAuthQuery()
GGAUTHS_EXPORT UINT32   __CDECL GGAuthCheckAnswer(LPGGAUTH pGGAuth, PGG_AUTH_DATA pAuthData);	// CheckAuthAnswer()
GGAUTHS_API int      __CDECL GGAuthCheckUpdated(LPGGAUTH pGGAuth);									// CheckAuthAnswer()
GGAUTHS_API int      __CDECL GGAuthUserInfo(LPGGAUTH pGGAuth, char* dest, int length);			// CheckAuthAnswer()
GGAUTHS_EXPORT UINT32	__CDECL GGAuthGetState(LPGGAUTH pGGAuth, PGG_CSAUTH_STATE pAuthState);	// GetCSAuthState()
GGAUTHS_EXPORT UINT32	__CDECL	GGAuthSetState(LPGGAUTH pGGAuth, PGG_CSAUTH_STATE pAuthState);	// SetCSAuthState()
GGAUTHS_EXPORT UINT32	__CDECL	GGAuthSetSecureOrder(LPGGAUTH pGGAuth);								// SetSecureOrder()
GGAUTHS_EXPORT UINT32   __CDECL GGAuthCheckUserCSAuth(LPGGAUTH pGGAuth, bool bCheck);				// CheckCSAuth()
GGAUTHS_API UINT32	__CDECL GGAuthAllowOldVersion(LPGGAUTH pGGAuth);								// AllowOldVersion()
GGAUTHS_EXPORT UINT32	__CDECL GGGetCurrentGGVer();

GGAUTHS_EXPORT UINT32	 __CDECL GGAuthGetUserValue(LPGGAUTH pGGAuth, int type);

#endif // MY_CSAuth2 == TRUE

// Pop Pack padr�o
#pragma pack(pop, ggsrv)

#endif

