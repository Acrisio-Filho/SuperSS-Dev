#pragma once
#ifndef _GG_AUTH_SERVER_H_
#define _GG_AUTH_SERVER_H_

#if defined(_WIN32)
// Pack padrão
#pragma pack(push, ggsrv)
#pragma pack( )
#endif

#if !defined(_WIN32) && !defined(_WIN64)
#include <stdint.h>
#include <memory.h>

// 64비트 리눅스에서 사용할 데이터 타입 선언  - Coded By HS_Soul [2007.12.28]
typedef int32_t			INT32, *PINT32;
typedef uint32_t		UINT32, *PUINT32;
typedef int64_t			INT64, *PINT64;
typedef uint64_t		UINT64, *PUINT64;

typedef intptr_t		INT_PTR;
typedef uintptr_t		UINT_PTR;

typedef UINT32			DWORD;
typedef unsigned char	BYTE;
typedef BYTE			UCHAR;

#ifndef BOOL
typedef UINT32			BOOL;
#endif

#ifndef LPVOID
typedef void*			LPVOID;
#endif

#define FALSE			0
#define TRUE			1

#else
#include <Windows.h>
#endif

#if defined(_WIN32)

#ifdef _EXPORT_DLL
#define GGAUTHS_API    extern "C" __declspec(dllexport)
#define GGAUTHS_EXPORT __declspec(dllexport)
#define __CDECL        __cdecl
#else
#define GGAUTHS_API //extern "C"
//#define GGAUTHS_API  <== ggsrv_cpp_c.h 에서 활성화. extern "C" 는 c 컴파일러가 인식못함.
#define GGAUTHS_EXPORT 
#define __CDECL __cdecl
#endif

#elif defined(__linux__)

#ifdef _EXPORT_DLL
#define GGAUTHS_API    extern "C" __attribute__((visibility("default")))
#define GGAUTHS_EXPORT __attribute__((visibility("default")))

#if defined(__aarch64__) || defined(_M_ARM64)
#define __CDECL 
#else
#define __CDECL        __attribute__((cdecl))
#endif

#else
#define GGAUTHS_API //extern "C"
//#define GGAUTHS_API  <== ggsrv_cpp_c.h 에서 활성화. extern "C" 는 c 컴파일러가 인식못함.
#define GGAUTHS_EXPORT 

#if defined(__aarch64__) || defined(_M_ARM64)
#define __CDECL
#else
#define __CDECL __attribute__((cdecl))
#endif

#endif

#endif

#ifndef ERROR_SUCCESS
#define ERROR_SUCCESS								0		//	일반적인 성공
#endif
#define NPGG_INFO_SUCCESS							0		//	GetInfo() func success.

#define NPGG_INFO_ERROR_NOTENOUGHFMEMORY			1		//	Need more memory
#define ERROR_GGAUTH_FAIL_MEM_ALLOC					1		//	CS인증 모듈 로딩시 메모리 부족
#define ERROR_GGAUTH_FAIL_LOAD_DLL					2		//	CS인증 모듈(*.dll, *.so) 로딩 실패
#define ERROR_GGAUTH_FAIL_GET_PROC					3		//	CS인증 모듈의 export 함수 설정 실패
#define ERROR_GGAUTH_FAIL_BEFORE_INIT				4		//	비정상 동작 <- critical error
#define ERROR_GGAUTH_FAIL_LOAD_CFG					5		//	서버인증 모듈 설정 파일 로딩 실패.
#define ERROR_GGAUTH_INVALID_PARAM					10		// 함수 호출시 잘못된 인자값을 넘겼을 경우

#define ERROR_GGAUTH_NO_REPLY						11		// 응답없음. GetAuthQuery() 가 CheckAuthAnswer()하기전에 두번 불려지면 같은에러 발생
#define ERROR_GGAUTH_INVALID_PROTOCOL_VERSION		12		// CS인증 모듈 프로토콜 번호(ggauth##.dll) 틀림
#define ERROR_GGAUTH_INVALID_REPLY					13		// 알고리즘에 의한 응답값 틀림.
#define ERROR_GGAUTH_INVALID_GAMEGUARD_VER			14		//	게임가드 버젼검사. 라이브러리 2.5에서 추가됨(cs인증모듈 50번 이상)

// CS26 추가 15 ~ 19
#define ERROR_GGAUTH_SAME_CLIENT_DETECTED			15		// 동일 게임가드 사용자 검출
#define ERROR_GGAUTH_CLIENT_STOPPED					16		// 게임가드가 동작하지 않음
#define ERROR_GGAUTH_CLIENT_AUTH_ERROR				17		// 게임가드 동작 인증에 실패함 (동일 게임가드 or 인증 변조)
#define ERROR_GGAUTH_INVALID_PACKET					18		// 헤더가 올바르지 않음
#define ERROR_GGAUTH_INVALID_CRC1					19		// 응답 패킷이 올바르지 않거나 변조됨
#define ERROR_GGAUTH_INVALID_CRC2					20		// 응답 패킷이 올바르지 않거나 변조됨
#define ERROR_GGAUTH_CLIENT_HACK_DETECTED			21		// 게임가드에서 Hack 을 발견하여 서버인증 값이 변조됨


#define ERROR_GGAUTH_SETSTATE_ERROR					50		// SetCSAuthState() 호출 시 m_CSAuthState가 NULL이거나..
															// m_PrtcTemp가 NULL 인 경우 (pProtocolHead 를 찾을 수 없는 경우...)

#define ERROR_GGAUTH_INVALID_GAMEMON_VER			101	//
#define ERROR_GGAUTH_INVALID_GAMEMON_VER_CODE		102	//

#define ERROR_GGAUTH_RETRY_QUERY					200

#define NPGG_CHECKUPDATED_VERIFIED					0 //csa.CheckUpdated() 함수 리턴값. 현재 버전 사용자
#define NPGG_CHECKUPDATED_NOTREADY					1 //csa.CheckUpdated() 함수 리턴값. 버전검사를 하기 전. 버전에 대한 정보가 없음.
#define NPGG_CHECKUPDATED_HIGH						2 //csa.CheckUpdated() 함수 리턴값. 현재 버전보다 높은 사용자
#define NPGG_CHECKUPDATED_LOW						3 //csa.CheckUpdated() 함수 리턴값. 현재 버전보다 낮은 사용자

#define	NPLOG_DEBUG	0x00000001	// 디버깅 메세지 
#define	NPLOG_ERROR	0x00000002  // 에러 메세지 

#define NPLOG_ENABLE_DEBUG 0x00000001 // 디버깅 메세지 생성함
#define NPLOG_ENABLE_ERROR 0x00000002 // 에러 메세지 생성함

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

// Data for Server <-> Server (프로토콜 버전, 게임가드 내부버전, Flag)
typedef struct _GG_CSAUTH_STATE
{
	UINT32	m_PrtcVersion;
	UINT32	m_GGVersion;
	UINT32	m_UserFlag;
} GG_CSAUTH_STATE, *PGG_CSAUTH_STATE;

typedef struct _GG_AUTH_PROTOCOL *PGG_AUTH_PROTOCOL;

GGAUTHS_API UINT32 __CDECL InitGameguardAuth(char* sGGPath, UINT32 dwNumActive, int useTimer, int useLog);
GGAUTHS_API void  __CDECL CleanupGameguardAuth();
GGAUTHS_API UINT32 __CDECL GGAuthUpdateTimer(); //useTimer 가 true 일 경우 호출.
GGAUTHS_API UINT32 __CDECL AddAuthProtocol(char* sDllName);
GGAUTHS_API UINT32 __CDECL SetGGVerLimit(UINT32 nLimitVer);	//버전 상한선을 제한한다.
GGAUTHS_API UINT32 __CDECL SetUpdateCondition(int nTimeLimit, int nCondition); //업데이트 조건을 설정한다.
GGAUTHS_API UINT32 __CDECL CheckCSAuth(bool bCheck);	// 전역 인증 기능 On/Off 를 위한 함수
GGAUTHS_API int	 __CDECL DecryptHackData(char* lpszUserKey, LPVOID lpData, DWORD dwLength);
GGAUTHS_API int __CDECL ModuleInfo(char* dest, unsigned int length);
GGAUTHS_API void __CDECL NpLog(int mode, char* msg); // referenced by 
/*
{
	if(mode & (NPLOG_DEBUG | NPLOG_ERROR)) // for examples
#ifdef WIN32
		OutputDebugString(msg);
#else
		printf(msg);
#endif
};
*/

typedef struct _GG_UPREPORT
{
	UINT32	dwBefore;   // Before version
	UINT32	dwNext;		// Now version
	int		nType;		// Update type 1 : gameguard version, 2 : protocol num
} GG_UPREPORT, *PGG_UPREPORT;

GGAUTHS_API void __CDECL GGAuthUpdateCallback(PGG_UPREPORT report); // referenced by 
/*
{
	printf("GGAuth version update [%s] : [%ld] -> [%ld] \n", // for examples
			report->nType==1?"GameGuard Ver":"Protocol Num",
			report->dwBefore,
			report->dwNext);
};
*/

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
	GG_VERSION m_GGVer;				//게임가드 버젼
	GG_AUTH_DATA m_AuthQueryTmp;	//m_AuthQuery 값을 백업해둔다.
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
	UINT32 GetCSAuthState(PGG_CSAUTH_STATE _CSAuthState);
	UINT32 SetCSAuthState(PGG_CSAUTH_STATE _CSAuthState);
	UINT32 SetSecretOrder();
	void  Close();
	int	  Info(char* dest, int length); // protocol information
	int	  CheckUpdated(); //현재 사용자의 업데이트 여부를 확인한다.	

	void	AllowOldVersion();	// 2009.05.14 - 서버이동 유저에 한해서 구버전을 허용하여 접속 끊김을 막는다.
};



// C type CSAuth2
#define NPGG_USER_AUTH_QUERY	0x00000001 //GGAuthGetUserValue() 의 전달 플래그 Query
#define NPGG_USER_AUTH_ANSWER	0x00000002 //GGAuthGetUserValue() 의 전달 플래그 Answer
#define NPGG_USER_AUTH_INDEX	0x00000010 //GGAuthGetUserValue() 의 전달 플래그 index
#define NPGG_USER_AUTH_VALUE1	0x00000020 //GGAuthGetUserValue() 의 전달 플래그 Value1
#define NPGG_USER_AUTH_VALUE2	0x00000040 //GGAuthGetUserValue() 의 전달 플래그 Value2
#define NPGG_USER_AUTH_VALUE3	0x00000080 //GGAuthGetUserValue() 의 전달 플래그 Value3

typedef void*          LPGGAUTH;

//. Do not support c type functions at CS26

GGAUTHS_API LPGGAUTH __CDECL GGAuthCreateUser();                                             // CCSAuth2()
GGAUTHS_API UINT32   __CDECL GGAuthDeleteUser(LPGGAUTH pGGAuth);                             // ~CCSAuth2()
GGAUTHS_API UINT32	__CDECL GGAuthInitUser(LPGGAUTH pGGAuth);											// Init()
GGAUTHS_API UINT32   __CDECL GGAuthCloseUser(LPGGAUTH pGGAuth);										// Close()
GGAUTHS_API UINT32   __CDECL GGAuthGetQuery(LPGGAUTH pGGAuth, PGG_AUTH_DATA pAuthData);		// GetAuthQuery()
GGAUTHS_API UINT32   __CDECL GGAuthCheckAnswer(LPGGAUTH pGGAuth, PGG_AUTH_DATA pAuthData);	// CheckAuthAnswer()
GGAUTHS_API int      __CDECL GGAuthCheckUpdated(LPGGAUTH pGGAuth);									// CheckAuthAnswer()
GGAUTHS_API int      __CDECL GGAuthUserInfo(LPGGAUTH pGGAuth, char* dest, int length);			// CheckAuthAnswer()
GGAUTHS_API UINT32	__CDECL GGAuthGetState(LPGGAUTH pGGAuth, PGG_CSAUTH_STATE pAuthState);	// GetCSAuthState()
GGAUTHS_API UINT32	__CDECL	GGAuthSetState(LPGGAUTH pGGAuth, PGG_CSAUTH_STATE pAuthState);	// SetCSAuthState()
GGAUTHS_API UINT32	__CDECL	GGAuthSetSecureOrder(LPGGAUTH pGGAuth);								// SetSecureOrder()
GGAUTHS_API UINT32   __CDECL GGAuthCheckUserCSAuth(LPGGAUTH pGGAuth, bool bCheck);				// CheckCSAuth()
GGAUTHS_API UINT32	__CDECL GGAuthAllowOldVersion(LPGGAUTH pGGAuth);								// AllowOldVersion()
GGAUTHS_API UINT32	__CDECL GGGetCurrentGGVer();

GGAUTHS_API UINT32	 __CDECL GGAuthGetUserValue(LPGGAUTH pGGAuth, int type);

#if defined(_WIN32)
// Pop Pack padrão
#pragma pack(pop, ggsrv)
#endif

#endif