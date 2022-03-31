// Arquivo gg_srv_lib_type.hpp
// Criado em 29/03/2022 as 13:16 por Acrisio
// Definição dos tipos usados no GameGuard Server Auth Lib

#pragma once
#ifndef _GG_SRV_LIB_TYPE_HPP
#define _GG_SRV_LIB_TYPE_HPP

#if defined(__linux__)
#include <pthread.h>
#include <dlfcn.h>
#endif

#include <cstdlib>
#include <cstdio>
#include <ctime>

constexpr auto CURRENT_VERSION = 2003010101;
constexpr auto VERSION_LIMIT_MAX = 2100010101;

constexpr char gBlowfish_init_key[] = "y].ZdaXlMls";
constexpr char gConf_file_name[] = "csauth2.cfg";
constexpr char gVer_file_name[] = "csver.cfg";

#if defined(_WIN32)
constexpr char gGG_auth_dll_name[] = "%sggauth%d.dll";
#elif defined(__linux__)
constexpr char gGG_auth_dll_name[] = "%sggauth%d.so";
#endif

#if defined(_WIN32)
constexpr char gSeparator = '\\';
#elif defined(__linux__)
constexpr char gSeparator = '/';
#endif

constexpr char gSTATE_ACTIVE[] = "ACTIVE";
constexpr char gSTATE_STANDBY[] = "STANDBY";
constexpr char gSTATE_DISUSE[] = "DISUSE";

#if defined(_WIN32)
// Pack padr�o
#pragma pack(push, ggsrv)
#pragma pack( )
#endif

typedef UINT32(*FNPrtcCheckAuthAnswer)(UINT32 *_pUserFlag, GG_VERSION *_pGGVer, GG_AUTH_DATA *_pAuthAnswer, GG_AUTH_DATA *_pAuthChecked, UINT32 _nSequenceNum, UINT32 *_pLastValue4);
typedef UINT32(*FNPrtcCheckValidAnswer)(GG_AUTH_DATA *_pAuthAnswer, UINT32 *_pReturnFlag);
typedef void(*FNPrtcDecryptAnswer)(GG_AUTH_DATA *_pAuthAnswer);
typedef UINT32(*FNPrtcEncryptQuery)(GG_AUTH_DATA *_pAuthQuery);
typedef UINT32(*FNPrtcGetAuthQuery)(UINT32 *_pUserFlag, GG_VERSION *_pGGVer, CCSAuth2 *_pCCSAuth2, DWORD _nSequenceNum, DWORD _dwServerKey);
typedef UINT32(*FNPrtcGetInformation)(UINT32 _type, GG_AUTH_DATA *_pAuthAnswer);
typedef UINT32(*FNPrtcGetVersion)(BOOL _bUseTimer);
typedef UINT32(*FNPrtcLoopAuth)(char* _pByLastLoop1, DWORD *_pdwLoop1AuthArray, char* _pByLastLoop2, DWORD *_pdwLoop2AuthArray, GG_AUTH_DATA *_pAuthAnswer, UINT32 _nSequenceNum);
typedef UINT32(*FNPrtcUpdateTimer)(void);

struct _GG_AUTH_PROTOCOL {
#if defined(_WIN32)
	HMODULE m_hModule;
#elif defined(__linux__)
	void* m_hModule;
#endif
	UINT32 m_dwGGVer;
	UINT32 RefCount;
	UINT32 m_flag;
	time_t reg_datetime;
	UINT32 unknow3;
	BOOL m_bDisuse;
	FNPrtcGetAuthQuery PrtcGetAuthQuery;
	FNPrtcCheckAuthAnswer PrtcCheckAuthAnswer;
	FNPrtcEncryptQuery PrtcEncryptQuery;
	FNPrtcDecryptAnswer PrtcDecryptAnswer;
	FNPrtcUpdateTimer PrtcUpdateTimer;
	FNPrtcLoopAuth PrtcLoopAuth;
	FNPrtcGetInformation PrtcGetInformation;
	FNPrtcCheckValidAnswer PrtcCheckValidAnswer;
	_GG_AUTH_PROTOCOL *m_pNext;
	_GG_AUTH_PROTOCOL *m_pPrev;
};

struct _UNIQ_VAL {
	DWORD m_value_1;
	DWORD m_value_2;
	DWORD m_value_3;
};

struct _GG_VERSION_UPDATE {
	UINT32 m_dwGGVer;
	UINT32 m_dwCount;
	UINT32 m_dwTime_1;
	UINT32 m_dwTIme_2;
};

struct _GG_VERSION_UPDATE_CTX {
	bool m_bCountStart;	// true j� foi inicializado
	DWORD m_CountStart;
	_GG_VERSION_UPDATE m_Table[5];
};

struct _GG_AUTH_CTX {
	bool m_bAuth;
	bool m_bInitState;
	BOOL m_bUseTimer;
	int m_UseLog;
	DWORD m_dwNumActiveSession;
	DWORD m_dwProtocolNum;
	DWORD m_dwCurrentVersion;
	DWORD m_dwVersionLimitMax;
	_GG_AUTH_PROTOCOL* m_EntryListProtocol;
	int m_UpdateCondition;
	int m_UpdateTimeLimit;
	_GG_VERSION_UPDATE_CTX m_GGVerUpdateCtx;
	_GG_VERSION_UPDATE_CTX m_ProtocolVerUpdateCtx;
	_UNIQ_VAL m_UniqValHashTable[20000];
#if defined(_WIN32)
	CRITICAL_SECTION m_csGGVer;
	CRITICAL_SECTION m_csProtocol;
	CRITICAL_SECTION m_csUniqVal;
#elif defined(__linux__)
	pthread_mutex_t m_csGGVer;
	pthread_mutex_t m_csProtocol;
	pthread_mutex_t m_csUniqVal;
#endif
	_BLOWFISH_CTX m_BlowfishCtx;
	char m_StrPathTmp[256];
	char m_MsgBuff[1024];
};

static _GG_AUTH_CTX gGGAuthCtx;

#if defined(_WIN32)
// Pop Pack padr�o
#pragma pack(pop, ggsrv)
#endif

#endif // !_GG_SRV_LIB_TYPE_HPP