// GGSrvLib26-1.cpp : Define as funções da biblioteca estática.
//

#if defined(_WIN32)
#include "pch.h"
#include "framework.h"
#elif defined(__linux__)
#include <memory.h>
#include <unistd.h>
#endif

#include "GGSrvLib26-1.h"
#include "blowfish.h"
#include "gg_srv_lib_type.hpp"

UINT32 LoadAuthProtocol(char *_szProtocolNameModule, BOOL _bHeadProtocol) {

	_GG_AUTH_PROTOCOL *pProtocol = (_GG_AUTH_PROTOCOL*)malloc(sizeof(_GG_AUTH_PROTOCOL));

	if (pProtocol == nullptr)
		return ERROR_GGAUTH_FAIL_MEM_ALLOC;

	memset(pProtocol, 0, sizeof(_GG_AUTH_PROTOCOL));

	char path_full[260];

#if defined(_WIN32)
	sprintf_s(path_full, sizeof(path_full), "%s", _szProtocolNameModule);
#elif defined(__linux__)
	sprintf(path_full, "%s/%s", getcwd(nullptr, 0), _szProtocolNameModule);
#endif

#if defined(_WIN32)
	pProtocol->m_hModule = LoadLibraryA(path_full);
#elif defined(__linux__)
	pProtocol->m_hModule = dlopen(path_full, RTLD_LAZY);
#endif

	if (pProtocol->m_hModule == NULL) {

		if ((gGGAuthCtx.m_UseLog & 2) != 0) {
#if defined(_WIN32)
			sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib][err:%d] LoadAuthProtocol() - LoadLibrary failed: %s", ERROR_GGAUTH_FAIL_LOAD_DLL, path_full);
#elif defined(__linux__)
			sprintf(gGGAuthCtx.m_MsgBuff, "[S lib][err:%d] LoadAuthProtocol() - deopen failed: %s, %s", ERROR_GGAUTH_FAIL_LOAD_DLL, path_full, dlerror());
#endif
			NpLog(2, gGGAuthCtx.m_MsgBuff);
		}

		free(pProtocol);

		return ERROR_GGAUTH_FAIL_LOAD_DLL;
	}

#if defined(_WIN32)
	FNPrtcGetVersion pGetVersion = (FNPrtcGetVersion)GetProcAddress(pProtocol->m_hModule, "PrtcGetVersion");
	pProtocol->PrtcGetAuthQuery = (FNPrtcGetAuthQuery)GetProcAddress(pProtocol->m_hModule, "PrtcGetAuthQuery");
	pProtocol->PrtcCheckAuthAnswer = (FNPrtcCheckAuthAnswer)GetProcAddress(pProtocol->m_hModule, "PrtcCheckAuthAnswer");
	pProtocol->PrtcEncryptQuery = (FNPrtcEncryptQuery)GetProcAddress(pProtocol->m_hModule, "PrtcEncryptQuery");
	pProtocol->PrtcDecryptAnswer = (FNPrtcDecryptAnswer)GetProcAddress(pProtocol->m_hModule, "PrtcDecryptAnswer");
	pProtocol->PrtcUpdateTimer = (FNPrtcUpdateTimer)GetProcAddress(pProtocol->m_hModule, "PrtcUpdateTimer");
	pProtocol->PrtcLoopAuth = (FNPrtcLoopAuth)GetProcAddress(pProtocol->m_hModule, "PrtcLoopAuth");
	pProtocol->PrtcGetInformation = (FNPrtcGetInformation)GetProcAddress(pProtocol->m_hModule, "PrtcGetInformation");
	pProtocol->PrtcCheckValidAnswer = (FNPrtcCheckValidAnswer)GetProcAddress(pProtocol->m_hModule, "PrtcCheckValidAnswer");
#elif defined(__linux__)
	FNPrtcGetVersion pGetVersion = (FNPrtcGetVersion)dlsym(pProtocol->m_hModule, "PrtcGetVersion");
	pProtocol->PrtcGetAuthQuery = (FNPrtcGetAuthQuery)dlsym(pProtocol->m_hModule, "PrtcGetAuthQuery");
	pProtocol->PrtcCheckAuthAnswer = (FNPrtcCheckAuthAnswer)dlsym(pProtocol->m_hModule, "PrtcCheckAuthAnswer");
	pProtocol->PrtcEncryptQuery = (FNPrtcEncryptQuery)dlsym(pProtocol->m_hModule, "PrtcEncryptQuery");
	pProtocol->PrtcDecryptAnswer = (FNPrtcDecryptAnswer)dlsym(pProtocol->m_hModule, "PrtcDecryptAnswer");
	pProtocol->PrtcUpdateTimer = (FNPrtcUpdateTimer)dlsym(pProtocol->m_hModule, "PrtcUpdateTimer");
	pProtocol->PrtcLoopAuth = (FNPrtcLoopAuth)dlsym(pProtocol->m_hModule, "PrtcLoopAuth");
	pProtocol->PrtcGetInformation = (FNPrtcGetInformation)dlsym(pProtocol->m_hModule, "PrtcGetInformation");
	pProtocol->PrtcCheckValidAnswer = (FNPrtcCheckValidAnswer)dlsym(pProtocol->m_hModule, "PrtcCheckValidAnswer");
#endif

	if (pGetVersion == nullptr || pProtocol->PrtcGetAuthQuery == nullptr || pProtocol->PrtcCheckAuthAnswer == nullptr || pProtocol->PrtcEncryptQuery == nullptr
			|| pProtocol->PrtcDecryptAnswer == nullptr || pProtocol->PrtcUpdateTimer == nullptr
			|| pProtocol->PrtcLoopAuth == nullptr || pProtocol->PrtcGetInformation == nullptr || pProtocol->PrtcCheckValidAnswer == nullptr) {

		if ((gGGAuthCtx.m_UseLog & 2) != 0) {
#if defined(_WIN32)
			sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib][err:%d] LoadAuthProtocol() - GetProcAddress() failed: %s %p %p %p %p %p %p %p %p %p", 
					ERROR_GGAUTH_FAIL_GET_PROC, path_full, pGetVersion, pProtocol->PrtcGetAuthQuery, pProtocol->PrtcCheckAuthAnswer,
					pProtocol->PrtcEncryptQuery, pProtocol->PrtcDecryptAnswer, pProtocol->PrtcUpdateTimer, pProtocol->PrtcLoopAuth, 
					pProtocol->PrtcGetInformation, pProtocol->PrtcCheckValidAnswer);
#elif defined(__linux__)
			sprintf(gGGAuthCtx.m_MsgBuff, "[S lib][err:%d] LoadAuthProtocol() - GetProcAddress() failed: %s %p %p %p %p %p %p %p %p %p", 
					ERROR_GGAUTH_FAIL_GET_PROC, path_full, pGetVersion, pProtocol->PrtcGetAuthQuery, pProtocol->PrtcCheckAuthAnswer,
					pProtocol->PrtcEncryptQuery, pProtocol->PrtcDecryptAnswer, pProtocol->PrtcUpdateTimer, pProtocol->PrtcLoopAuth, 
					pProtocol->PrtcGetInformation, pProtocol->PrtcCheckValidAnswer);
#endif
			NpLog(2, gGGAuthCtx.m_MsgBuff);
		}

#if defined(_WIN32)
		FreeLibrary(pProtocol->m_hModule);
#elif defined(__linux__)
		dlclose(pProtocol->m_hModule);
#endif
		free(pProtocol);

		return ERROR_GGAUTH_FAIL_GET_PROC;
	}

	pProtocol->m_dwGGVer = pGetVersion(gGGAuthCtx.m_bUseTimer);
	pProtocol->RefCount = 0;
	pProtocol->m_flag = 0;
	pProtocol->reg_datetime = std::time(nullptr);
	pProtocol->unknow3 = 0;
	pProtocol->m_bDisuse = FALSE;

	if (_bHeadProtocol == TRUE) {

		pProtocol->m_bDisuse = FALSE;
		pProtocol->m_flag |= 3;
		pProtocol->m_pNext = nullptr;
		pProtocol->m_pPrev = nullptr;

		gGGAuthCtx.m_EntryListProtocol = pProtocol;

		if ((gGGAuthCtx.m_UseLog & 1) != 0) {
#if defined(_WIN32)
			sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] LoadAuthProtocol() - ggGlobal.pProtocolHead: %d", pProtocol->m_dwGGVer);
#elif defined(__linux__)
			sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] LoadAuthProtocol() - ggGlobal.pProtocolHead: %d", pProtocol->m_dwGGVer);
#endif
			NpLog(1, gGGAuthCtx.m_MsgBuff);
		}

	}else {

		if (gGGAuthCtx.m_EntryListProtocol != nullptr) {

			_GG_AUTH_PROTOCOL *pLast = gGGAuthCtx.m_EntryListProtocol;

			while (pLast != nullptr && pLast->m_pNext != nullptr)
				pLast = pLast->m_pNext;

			if (pLast != nullptr) {
				pLast->m_pNext = pProtocol;
				pProtocol->m_pPrev = pLast;
			}

			if ((gGGAuthCtx.m_UseLog & 1) != 0) {
#if defined(_WIN32)
				sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] LoadAuthProtocol() - Add a new protocol: ver - %d, %p, pN %p, pB %p",
						pProtocol->m_dwGGVer, pProtocol, pProtocol->m_pNext, pProtocol->m_pPrev);
#elif defined(__linux__)
				sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] LoadAuthProtocol() - Add a new protocol: ver - %d, %p, pN %p, pB %p",
						pProtocol->m_dwGGVer, pProtocol, pProtocol->m_pNext, pProtocol->m_pPrev);
#endif
				NpLog(1, gGGAuthCtx.m_MsgBuff);
			}
		}
	}

#if defined(_WIN32)
	EnterCriticalSection(&gGGAuthCtx.m_csProtocol);
#elif defined(__linux__)
	pthread_mutex_lock(&gGGAuthCtx.m_csProtocol);
#endif
	
	gGGAuthCtx.m_dwProtocolNum++;
	
#if defined(_WIN32)
	LeaveCriticalSection(&gGGAuthCtx.m_csProtocol);
#elif defined(__linux__)
	pthread_mutex_unlock(&gGGAuthCtx.m_csProtocol);
#endif

	if ((gGGAuthCtx.m_UseLog & 1) != 0) {

#if defined(_WIN32)
		sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] ----------PROTOCOL STATE----------");
#elif defined(__linux__)
		sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] ----------PROTOCOL STATE----------");
#endif
		NpLog(1, gGGAuthCtx.m_MsgBuff);

#if defined(_WIN32)
		sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] ## LoadAuthProtocol PATH: %s", path_full);
#elif defined(__linux__)
		sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] ## LoadAuthProtocol PATH: %s", path_full);
#endif
		NpLog(1, gGGAuthCtx.m_MsgBuff);

		if (gGGAuthCtx.m_EntryListProtocol != nullptr) {
#if defined(_WIN32)
			sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] ## [ver %d][ptr 0x%p]: pN[0x%p], pB[0x%p]",
					gGGAuthCtx.m_EntryListProtocol->m_dwGGVer, gGGAuthCtx.m_EntryListProtocol, gGGAuthCtx.m_EntryListProtocol->m_pNext, gGGAuthCtx.m_EntryListProtocol->m_pPrev);
#elif defined(__linux__)
			sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] ## [ver %d][ptr 0x%p]: pN[0x%p], pB[0x%p]",
					gGGAuthCtx.m_EntryListProtocol->m_dwGGVer, gGGAuthCtx.m_EntryListProtocol, gGGAuthCtx.m_EntryListProtocol->m_pNext, gGGAuthCtx.m_EntryListProtocol->m_pPrev);
#endif
			NpLog(1, gGGAuthCtx.m_MsgBuff);
		}
	}

	_GG_AUTH_PROTOCOL *pNext = gGGAuthCtx.m_EntryListProtocol;

	while (pNext != nullptr && (pNext = pNext->m_pNext) != nullptr) {

		if ((gGGAuthCtx.m_UseLog & 1) != 0) {

#if defined(_WIN32)
			sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] ==>");
#elif defined(__linux__)
			sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] ==>");
#endif
			NpLog(1, gGGAuthCtx.m_MsgBuff);

#if defined(_WIN32)
			sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] ## [ver %d][ptr 0x%p]: pN[0x%p], pB[0x%p]",
					pNext->m_dwGGVer, pNext, pNext->m_pNext, pNext->m_pPrev);
#elif defined(__linux__)
			sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] ## [ver %d][ptr 0x%p]: pN[0x%p], pB[0x%p]",
					pNext->m_dwGGVer, pNext, pNext->m_pNext, pNext->m_pPrev);
#endif
			NpLog(1, gGGAuthCtx.m_MsgBuff);
		}
	}

	if ((gGGAuthCtx.m_UseLog & 1) != 0) {

#if defined(_WIN32)
		sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] ## TOTAL LOADED PROTOCOL NUM: %d", gGGAuthCtx.m_dwProtocolNum);
#elif defined(__linux__)
		sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] ## TOTAL LOADED PROTOCOL NUM: %d", gGGAuthCtx.m_dwProtocolNum);
#endif
		NpLog(1, gGGAuthCtx.m_MsgBuff);

#if defined(_WIN32)
		sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] ---------------------------------");
#elif defined(__linux__)
		sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] ---------------------------------");
#endif
		NpLog(1, gGGAuthCtx.m_MsgBuff);
	}

	return ERROR_SUCCESS;
};

UINT32 UnloadAuthProtocol(_GG_AUTH_PROTOCOL *_pProtocol) {

	if (_pProtocol == nullptr)
		return ERROR_GGAUTH_INVALID_PARAM;

	if (gGGAuthCtx.m_dwProtocolNum > 1) {

		_GG_AUTH_PROTOCOL *pHead = gGGAuthCtx.m_EntryListProtocol, *pNext = gGGAuthCtx.m_EntryListProtocol->m_pNext, *pCurr = nullptr;

		while ((pCurr = pNext) != nullptr) {

			if (pCurr == _pProtocol) {

#if defined(_WIN32)
				EnterCriticalSection(&gGGAuthCtx.m_csProtocol);
#elif defined(__linux__)
				pthread_mutex_lock(&gGGAuthCtx.m_csProtocol);
#endif

				pHead->m_pNext = _pProtocol->m_pNext;

				if (_pProtocol->m_pNext != nullptr)
					_pProtocol->m_pNext->m_pPrev = pHead;

				if (_pProtocol->m_hModule != NULL)
#if defined(_WIN32)
					FreeLibrary(_pProtocol->m_hModule);
#elif defined(__linux__)
					dlclose(_pProtocol->m_hModule);
#endif

				free(_pProtocol);

				gGGAuthCtx.m_dwProtocolNum--;

#if defined(_WIN32)
				LeaveCriticalSection(&gGGAuthCtx.m_csProtocol);
#elif defined(__linux__)
				pthread_mutex_unlock(&gGGAuthCtx.m_csProtocol);
#endif

				if ((gGGAuthCtx.m_UseLog & 1) != 0) {
#if defined(_WIN32)
					sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] UnloadAuthProtocol (0x%p)", _pProtocol);
#elif defined(__linux__)
					sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] UnloadAuthProtocol (0x%p)", _pProtocol);
#endif
					NpLog(1, gGGAuthCtx.m_MsgBuff);
				}

				return ERROR_SUCCESS;
			}

			pHead = pCurr;
			pNext = pCurr->m_pNext;
		}
	}

	return ERROR_GGAUTH_INVALID_PARAM;
};

bool UpdateUniqValHashTable(bool _bModify, UINT32 _value_1, UINT32 _value_2, UINT32 _server_key) {

	if (_value_1 == 0 || _value_1 == 0xFFFFFFFF)
		return false;

	UINT32 low = _value_2 % 200, high = (_value_1 % 100) * 200, inc = low, index;

	if (_bModify == true) {

		UINT32 idx = 0xFFFFFFFF;

		do {

			index = high + inc;

			if (gGGAuthCtx.m_UniqValHashTable[index].m_value_1 == _value_1 && gGGAuthCtx.m_UniqValHashTable[index].m_value_2 == _value_2)
				return gGGAuthCtx.m_UniqValHashTable[index].m_value_3 == _server_key;

			if (gGGAuthCtx.m_UniqValHashTable[index].m_value_1 == 0) {

				if (idx != 0xFFFFFFFF)
					inc = idx;

				index = high + inc;

				gGGAuthCtx.m_UniqValHashTable[index].m_value_1 = _value_1;
				gGGAuthCtx.m_UniqValHashTable[index].m_value_2 = _value_2;
				gGGAuthCtx.m_UniqValHashTable[index].m_value_3 = _server_key;

				return true;
			}

			if (gGGAuthCtx.m_UniqValHashTable[index].m_value_1 == 0xFFFFFFFF && idx == 0xFFFFFFFF)
				idx = inc;

			inc++;

			if (inc == 200)
				inc = 0;

		} while (inc != low);

		if (idx != 0xFFFFFFFF) {

			index = high + idx;

			gGGAuthCtx.m_UniqValHashTable[index].m_value_1 = _value_1;
			gGGAuthCtx.m_UniqValHashTable[index].m_value_2 = _value_2;
			gGGAuthCtx.m_UniqValHashTable[index].m_value_3 = _server_key;

			return true;
		}

	}else {

		do {

			index = high + inc;

			if (gGGAuthCtx.m_UniqValHashTable[index].m_value_1 == _value_1
					&& gGGAuthCtx.m_UniqValHashTable[index].m_value_2 == _value_2
					&& gGGAuthCtx.m_UniqValHashTable[index].m_value_3 == _server_key) {

				gGGAuthCtx.m_UniqValHashTable[index].m_value_1 = 0xFFFFFFFF;

				return true;
			}

			if (gGGAuthCtx.m_UniqValHashTable[index].m_value_1 == 0)
				return true;

			inc++;

			if (inc == 200)
				inc = 0;

		} while (inc != low);
	}

	return true;
};

DWORD GetRandomDword() {

	DWORD random = 0;

	for (UINT32 i = 0; i < 32; i += 4)
		random |= ((UCHAR)rand() << i);

	return random;
}

bool IsValidGGVer(_GG_VERSION *_GGVer) {
	return _GGVer != nullptr && _GGVer->dwGGVer > 0 && _GGVer->wYear > 2002 && _GGVer->wYear < 2101
		&& _GGVer->wMonth > 0 && _GGVer->wMonth < 13 && _GGVer->wDay > 0 && _GGVer->wDay < 32 && _GGVer->wNum > 0 && _GGVer->wNum < 100;
}

bool IsValidGGVer2(UINT32 _dwGGVer) {
	
	if (_dwGGVer <= 0)
		return false;
	
	DWORD tmp = (_dwGGVer / 1000000);
		
	if (tmp <= 2002 || tmp >= 2101)
		return false;

	tmp = (_dwGGVer / 10000) % 100;

	if (tmp <= 0 || tmp >= 13)
		return false;

	tmp = (_dwGGVer / 100) % 100;

	return tmp > 0 && tmp < 32;
}

void GGEncryptPacket1(_GG_AUTH_DATA *_query_data) {

	if (_query_data == nullptr)
		return;

	_query_data->dwIndex ^= 0xDA93AB6F;
	_query_data->dwValue1 ^= 0xED3AE7DB;

	Blowfish_Encrypt(&gGGAuthCtx.m_BlowfishCtx, (uint32_t*)&_query_data->dwIndex, (uint32_t*)&_query_data->dwValue2);

	_query_data->dwValue2 ^= 0xE2F3D164;
	_query_data->dwValue3 ^= 0x6FD4EE8E;

	Blowfish_Encrypt(&gGGAuthCtx.m_BlowfishCtx, (uint32_t*)&_query_data->dwValue1, (uint32_t*)&_query_data->dwValue3);
}

void GGDecryptPacket2(_GG_AUTH_DATA *_query_data) {

	if (_query_data == nullptr)
		return;

	Blowfish_Decrypt(&gGGAuthCtx.m_BlowfishCtx, (uint32_t*)&_query_data->dwValue1, (uint32_t*)&_query_data->dwValue3);

	_query_data->dwValue2 ^= 0xCC0C559F;
	_query_data->dwValue3 ^= 0x1AC6186C;

	Blowfish_Decrypt(&gGGAuthCtx.m_BlowfishCtx, (uint32_t*)&_query_data->dwIndex, (uint32_t*)&_query_data->dwValue2);

	_query_data->dwIndex ^= 0xD253FC2E;
	_query_data->dwValue1 ^= 0x630FC331;
}

UINT32 UpdateGGVersion(UINT32 _dwGGVer) {

	if (_dwGGVer == 0 || _dwGGVer < CURRENT_VERSION || _dwGGVer > VERSION_LIMIT_MAX) {

		if ((gGGAuthCtx.m_UseLog & 2) == 0)
			return ERROR_SUCCESS;

#if defined(_WIN32)
		sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] UpdateGGVersion() - Invalid ggver %d", _dwGGVer);
#elif defined(__linux__)
		sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] UpdateGGVersion() - Invalid ggver %d", _dwGGVer);
#endif
		NpLog(2, gGGAuthCtx.m_MsgBuff);

		return ERROR_SUCCESS;
	}

	if (_dwGGVer > gGGAuthCtx.m_dwVersionLimitMax) {

		if ((gGGAuthCtx.m_UseLog & 2) == 0)
			return ERROR_SUCCESS;

#if defined(_WIN32)
		sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] UpdateGGVersion() - Limited ggver %d > %d", _dwGGVer, gGGAuthCtx.m_dwVersionLimitMax);
#elif defined(__linux__)
		sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] UpdateGGVersion() - Limited ggver %d > %d", _dwGGVer, gGGAuthCtx.m_dwVersionLimitMax);
#endif
		NpLog(2, gGGAuthCtx.m_MsgBuff);

		return ERROR_SUCCESS;
	}

	if (_dwGGVer < gGGAuthCtx.m_dwCurrentVersion) {

		if ((gGGAuthCtx.m_UseLog & 2) != 0) {
#if defined(_WIN32)
			sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] UpdateGGVersion() error #1026 [%d]", _dwGGVer);
#elif defined(__linux__)
			sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] UpdateGGVersion() error #1026 [%d]", _dwGGVer);
#endif
			NpLog(2, gGGAuthCtx.m_MsgBuff);
		}

		return ERROR_GGAUTH_INVALID_PROTOCOL_VERSION;
	}

	if (_dwGGVer > gGGAuthCtx.m_dwCurrentVersion && gGGAuthCtx.m_GGVerUpdateCtx.m_bCountStart == false) {

		gGGAuthCtx.m_GGVerUpdateCtx.m_bCountStart = true;

		gGGAuthCtx.m_GGVerUpdateCtx.m_CountStart = (DWORD)std::time(nullptr);

		if ((gGGAuthCtx.m_UseLog & 1) != 0) {
#if defined(_WIN32)
			sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] UpdateGGVersion() - New GG version Count start[%d][%d]", _dwGGVer, gGGAuthCtx.m_dwCurrentVersion);
#elif defined(__linux__)
			sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] UpdateGGVersion() - New GG version Count start[%d][%d]", _dwGGVer, gGGAuthCtx.m_dwCurrentVersion);
#endif
			NpLog(1, gGGAuthCtx.m_MsgBuff);
		}
	}

	time_t now = std::time(nullptr);
	INT32 diff_sec = 0;
	UINT32 j, i, sum_val1 = 0;

	for (i = 0; i < (sizeof(gGGAuthCtx.m_GGVerUpdateCtx.m_Table) / sizeof(_GG_VERSION_UPDATE)); i++) {

		if (gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwGGVer != 0 && gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwGGVer == _dwGGVer) {

			diff_sec = (INT32)difftime(gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwTime_1, now);

			if (diff_sec < 3600) {

				gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwCount++;
				gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwTime_1 = (UINT32)now;

				if (gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwCount < gGGAuthCtx.m_dwNumActiveSession || (INT32)(now - gGGAuthCtx.m_GGVerUpdateCtx.m_CountStart) <= (gGGAuthCtx.m_UpdateTimeLimit * 60)
						|| gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwGGVer <= gGGAuthCtx.m_dwCurrentVersion) {

					if ((gGGAuthCtx.m_UseLog & 1) != 0) {
#if defined(_WIN32)
						sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] UpdateGGVersion() - GGVer < new Version[%d] NEW VERSION count up [%d]:[%d][%d][%d][%d]", 
								_dwGGVer, i, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwGGVer, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwCount, (UINT32)now, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwTIme_2);
#elif defined(__linux__)
						sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] UpdateGGVersion() - GGVer < new Version[%d] NEW VERSION count up [%d]:[%d][%d][%d][%d]", 
								_dwGGVer, i, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwGGVer, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwCount, (UINT32)now, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwTIme_2);
#endif
						NpLog(1, gGGAuthCtx.m_MsgBuff);
					}
				
				}else {

					for (j = 0; j < (sizeof(gGGAuthCtx.m_GGVerUpdateCtx.m_Table) / sizeof(_GG_VERSION_UPDATE)); j++)
						sum_val1 += gGGAuthCtx.m_GGVerUpdateCtx.m_Table[j].m_dwCount;

					_dwGGVer = 0xFFFFFFFF;

					for (j = 0; j < (sizeof(gGGAuthCtx.m_GGVerUpdateCtx.m_Table) / sizeof(_GG_VERSION_UPDATE)); j++) {

						if (gGGAuthCtx.m_dwCurrentVersion < gGGAuthCtx.m_GGVerUpdateCtx.m_Table[j].m_dwGGVer && gGGAuthCtx.m_GGVerUpdateCtx.m_Table[j].m_dwCount > ((gGGAuthCtx.m_UpdateCondition * sum_val1) / 100)) {

							if ((gGGAuthCtx.m_UseLog & 1) != 0) {
#if defined(_WIN32)
								sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] UpdateGGVersion() - New GGVer[%d] find that it accepted condition[%d/%d] [%d%%] > [%d%%]", 
										gGGAuthCtx.m_GGVerUpdateCtx.m_Table[j].m_dwGGVer, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[j].m_dwCount, sum_val1, 
										(gGGAuthCtx.m_GGVerUpdateCtx.m_Table[j].m_dwCount * 100) / sum_val1, gGGAuthCtx.m_UpdateCondition);
#elif defined(__linux__)
								sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] UpdateGGVersion() - New GGVer[%d] find that it accepted condition[%d/%d] [%d%%] > [%d%%]", 
										gGGAuthCtx.m_GGVerUpdateCtx.m_Table[j].m_dwGGVer, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[j].m_dwCount, sum_val1, 
										(gGGAuthCtx.m_GGVerUpdateCtx.m_Table[j].m_dwCount * 100) / sum_val1, gGGAuthCtx.m_UpdateCondition);
#endif
								NpLog(1, gGGAuthCtx.m_MsgBuff);
							}

							if (_dwGGVer == 0xFFFFFFFF || gGGAuthCtx.m_GGVerUpdateCtx.m_Table[j].m_dwGGVer > gGGAuthCtx.m_GGVerUpdateCtx.m_Table[_dwGGVer].m_dwGGVer)
								_dwGGVer = j;
						}
					}

					if (_dwGGVer == 0xFFFFFFFF) {

						if ((gGGAuthCtx.m_UseLog & 1) != 0) {
#if defined(_WIN32)
							sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] UpdateGGVersion() - New GGVer can not find at list that it accepted condition. Reset all data.");
#elif defined(__linux__)
							sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] UpdateGGVersion() - New GGVer can not find at list that it accepted condition. Reset all data.");
#endif
							NpLog(1, gGGAuthCtx.m_MsgBuff);
						}

					}else {

						if ((gGGAuthCtx.m_UseLog & 2) != 0) {
#if defined(_WIN32)
							sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] UpdateGGVersion() - GG Ver Update RESULT-----");
#elif defined(__linux__)
							sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] UpdateGGVersion() - GG Ver Update RESULT-----");
#endif
							NpLog(2, gGGAuthCtx.m_MsgBuff);
						}

						for (j = 0; j < (sizeof(gGGAuthCtx.m_GGVerUpdateCtx.m_Table) / sizeof(_GG_VERSION_UPDATE)); j++) {

							if ((gGGAuthCtx.m_UseLog & 2) != 0) {
#if defined(_WIN32)
								sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] [%d]:[%d][%d][%d][%d]: %d%%, ",
										j, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[j].m_dwGGVer, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[j].m_dwCount, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[j].m_dwTime_1, 
										gGGAuthCtx.m_GGVerUpdateCtx.m_Table[j].m_dwTIme_2, (gGGAuthCtx.m_GGVerUpdateCtx.m_Table[j].m_dwCount * 100) / sum_val1);
#elif defined(__linux__)
								sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] [%d]:[%d][%d][%d][%d]: %d%%, ",
										j, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[j].m_dwGGVer, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[j].m_dwCount, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[j].m_dwTime_1, 
										gGGAuthCtx.m_GGVerUpdateCtx.m_Table[j].m_dwTIme_2, (gGGAuthCtx.m_GGVerUpdateCtx.m_Table[j].m_dwCount * 100) / sum_val1);
#endif
								NpLog(2, gGGAuthCtx.m_MsgBuff);
							}
						}

						if ((gGGAuthCtx.m_UseLog & 2) != 0) {

#if defined(_WIN32)
							sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] CURRENT VER: %d", gGGAuthCtx.m_dwCurrentVersion);
#elif defined(__linux__)
							sprintf(gGGAuthCtx.m_MsgBuff,"[S lib] CURRENT VER: %d", gGGAuthCtx.m_dwCurrentVersion);
#endif
							NpLog(2, gGGAuthCtx.m_MsgBuff);

#if defined(_WIN32)
							sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] UpdateGGVersion() --------------------------");
#elif defined(__linux__)
							sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] UpdateGGVersion() --------------------------");
#endif
							NpLog(2, gGGAuthCtx.m_MsgBuff);
						}

						_GG_UPREPORT report;
						
						report.dwBefore = gGGAuthCtx.m_dwCurrentVersion;
						report.dwNext = gGGAuthCtx.m_GGVerUpdateCtx.m_Table[_dwGGVer].m_dwGGVer;
						report.nType = 1;

						gGGAuthCtx.m_dwCurrentVersion = gGGAuthCtx.m_GGVerUpdateCtx.m_Table[_dwGGVer].m_dwGGVer;

						if ((gGGAuthCtx.m_UseLog & 2) != 0) {
#if defined(_WIN32)
							sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] UpdateGGVersion() - New GG version accept[%d] !!!!!!!!!!!", gGGAuthCtx.m_dwCurrentVersion);
#elif defined(__linux__)
							sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] UpdateGGVersion() - New GG version accept[%d] !!!!!!!!!!!", gGGAuthCtx.m_dwCurrentVersion);
#endif
							NpLog(2, gGGAuthCtx.m_MsgBuff);
						}

						GGAuthUpdateCallback(&report);
					}

					memset(gGGAuthCtx.m_GGVerUpdateCtx.m_Table, 0, sizeof(gGGAuthCtx.m_GGVerUpdateCtx.m_Table));

					gGGAuthCtx.m_GGVerUpdateCtx.m_bCountStart = false;
				}

				if ((gGGAuthCtx.m_UseLog & 1) != 0) {
#if defined(_WIN32)
					sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] UpdateGGVersion() ----GG Ver Update RESULT-----");
#elif defined(__linux__)
					sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] UpdateGGVersion() ----GG Ver Update RESULT-----");
#endif
					NpLog(1, gGGAuthCtx.m_MsgBuff);
				}

				for (j = 0; j < (sizeof(gGGAuthCtx.m_GGVerUpdateCtx.m_Table) / sizeof(_GG_VERSION_UPDATE)); j++) {

					if (gGGAuthCtx.m_GGVerUpdateCtx.m_Table[j].m_dwCount != 0 && (gGGAuthCtx.m_UseLog & 1) != 0) {
#if defined(_WIN32)
						sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] [%d]:[%d][%d][%d][%d], ",
								j, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[j].m_dwGGVer, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[j].m_dwCount, 
								gGGAuthCtx.m_GGVerUpdateCtx.m_Table[j].m_dwTime_1, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[j].m_dwTIme_2);
#elif defined(__linux__)
						sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] [%d]:[%d][%d][%d][%d], ",
								j, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[j].m_dwGGVer, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[j].m_dwCount, 
								gGGAuthCtx.m_GGVerUpdateCtx.m_Table[j].m_dwTime_1, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[j].m_dwTIme_2);
#endif
						NpLog(1, gGGAuthCtx.m_MsgBuff);
					}
				}

				if ((gGGAuthCtx.m_UseLog & 1) != 0) {

#if defined(_WIN32)
					sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] CURRENT VER: %d", gGGAuthCtx.m_dwCurrentVersion);
#elif defined(__linux__)
					sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] CURRENT VER: %d", gGGAuthCtx.m_dwCurrentVersion);
#endif
					NpLog(1, gGGAuthCtx.m_MsgBuff);

#if defined(_WIN32)
					sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] UpdateGGVersion() ---------------------------");
#elif defined(__linux__)
					sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] UpdateGGVersion() ---------------------------");
#endif
					NpLog(1, gGGAuthCtx.m_MsgBuff);
				}

				return ERROR_SUCCESS;
			}
		}
	}

	for (i = 0; i < (sizeof(gGGAuthCtx.m_GGVerUpdateCtx.m_Table) / sizeof(_GG_VERSION_UPDATE)); i++) {

		if (gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwGGVer == 0) {

			gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwGGVer = _dwGGVer;
			gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwCount = 1;
			gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwTime_1 = (UINT32)now;
			gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwTIme_2 = (UINT32)now;

			break;
		}
	}

	if (i == 5) {

		i = 0xFFFFFFFF;

		for (j = 0; j < (sizeof(gGGAuthCtx.m_GGVerUpdateCtx.m_Table) / sizeof(_GG_VERSION_UPDATE)); j++) {

			diff_sec = (INT32)difftime(gGGAuthCtx.m_GGVerUpdateCtx.m_Table[j].m_dwTime_1, now);

			if (diff_sec > 3600 && gGGAuthCtx.m_GGVerUpdateCtx.m_Table[j].m_dwCount < gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i != 0xFFFFFFFF ? i : 0].m_dwCount)
				i = j;
		}

		if (i == 0xFFFFFFFF) {

			for (j = 0; j < (sizeof(gGGAuthCtx.m_GGVerUpdateCtx.m_Table) / sizeof(_GG_VERSION_UPDATE)); j++) {

				if (gGGAuthCtx.m_GGVerUpdateCtx.m_Table[j].m_dwTime_1 < gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i != 0xFFFFFFFF ? i : 0].m_dwTime_1)
					i = j;
			}

			if (i == 0xFFFFFFFF)
				i = 0;

			if ((gGGAuthCtx.m_UseLog & 1) != 0) {
#if defined(_WIN32)
				sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] UpdateGGVersion() - add a new GG version to the GG version list[%d]. OLDEST VERSION [%d]:[%d][%d][%d][%d], ",
						_dwGGVer, i, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwGGVer, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwCount, 
						gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwTime_1, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwTIme_2);
#elif defined(__linux__)
				sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] UpdateGGVersion() - add a new GG version to the GG version list[%d]. OLDEST VERSION [%d]:[%d][%d][%d][%d], ",
						_dwGGVer, i, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwGGVer, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwCount, 
						gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwTime_1, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwTIme_2);
#endif
				NpLog(1, gGGAuthCtx.m_MsgBuff);
			}

		}else if ((gGGAuthCtx.m_UseLog & 1) != 0) {

#if defined(_WIN32)
			sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] UpdateGGVersion() - ad new GG version to the GG version list[%d]. 1 hour time out and lower users [%d]:[%d][%d][%d][%d]",
					_dwGGVer, i, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwGGVer, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwCount, 
					gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwTime_1, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwTIme_2);
#elif defined(__linux__)
			sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] UpdateGGVersion() - ad new GG version to the GG version list[%d]. 1 hour time out and lower users [%d]:[%d][%d][%d][%d]",
					_dwGGVer, i, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwGGVer, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwCount, 
					gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwTime_1, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwTIme_2);
#endif
			NpLog(1, gGGAuthCtx.m_MsgBuff);
		}

		gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwGGVer = _dwGGVer;
		gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwCount = 1;
		gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwTime_1 = (UINT32)now;
		gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwTIme_2 = (UINT32)now;

		if ((gGGAuthCtx.m_UseLog & 1) != 0) {
#if defined(_WIN32)
			sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib]                          -> [%d]:[%d][%d][%d][%d], ",
					i, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwGGVer, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwCount, 
					gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwTime_1, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwTIme_2);
#elif defined(__linux__)
			sprintf(gGGAuthCtx.m_MsgBuff, "[S lib]                          -> [%d]:[%d][%d][%d][%d], ",
					i, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwGGVer, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwCount, 
					gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwTime_1, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwTIme_2);
#endif
			NpLog(1, gGGAuthCtx.m_MsgBuff);
		}

	}else {

		if ((gGGAuthCtx.m_UseLog & 1) != 0) {
#if defined(_WIN32)
			sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] UpdateGGVersion(): insert new GG version to new GG version list[%d]:[%d][%d][%d][%d], ",
					i, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwGGVer, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwCount, 
					gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwTime_1, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwTIme_2);
#elif defined(__linux__)
			sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] UpdateGGVersion(): insert new GG version to new GG version list[%d]:[%d][%d][%d][%d], ",
					i, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwGGVer, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwCount, 
					gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwTime_1, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[i].m_dwTIme_2);
#endif
			NpLog(1, gGGAuthCtx.m_MsgBuff);
		}
	}

	if ((gGGAuthCtx.m_UseLog & 1) != 0) {
#if defined(_WIN32)
		sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] UpdateGGVersion() ----GG Ver Update RESULT-----");
#elif defined(__linux__)
		sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] UpdateGGVersion() ----GG Ver Update RESULT-----");
#endif
		NpLog(1, gGGAuthCtx.m_MsgBuff);
	}

	for (j = 0; j < (sizeof(gGGAuthCtx.m_GGVerUpdateCtx.m_Table) / sizeof(_GG_VERSION_UPDATE)); j++) {

		if (gGGAuthCtx.m_GGVerUpdateCtx.m_Table[j].m_dwCount != 0 && (gGGAuthCtx.m_UseLog & 1) != 0) {
#if defined(_WIN32)
			sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] [%d]:[%d][%d][%d][%d], ",
					j, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[j].m_dwGGVer, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[j].m_dwCount, 
					gGGAuthCtx.m_GGVerUpdateCtx.m_Table[j].m_dwTime_1, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[j].m_dwTIme_2);
#elif defined(__linux__)
			sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] [%d]:[%d][%d][%d][%d], ",
					j, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[j].m_dwGGVer, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[j].m_dwCount, 
					gGGAuthCtx.m_GGVerUpdateCtx.m_Table[j].m_dwTime_1, gGGAuthCtx.m_GGVerUpdateCtx.m_Table[j].m_dwTIme_2);
#endif
			NpLog(1, gGGAuthCtx.m_MsgBuff);
		}
	}

	if ((gGGAuthCtx.m_UseLog & 1) != 0) {

#if defined(_WIN32)
		sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] CURRENT VER: %d", gGGAuthCtx.m_dwCurrentVersion);
#elif defined(__linux__)
		sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] CURRENT VER: %d", gGGAuthCtx.m_dwCurrentVersion);
#endif
		NpLog(1, gGGAuthCtx.m_MsgBuff);

#if defined(_WIN32)
		sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] UpdateGGVersion() ---------------------------");
#elif defined(__linux__)
		sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] UpdateGGVersion() ---------------------------");
#endif
		NpLog(1, gGGAuthCtx.m_MsgBuff);
	}

	return ERROR_SUCCESS;
}

UINT32 UpdateProtocol(_GG_AUTH_PROTOCOL *_pProtocol) {

	if (_pProtocol == nullptr) {

		if ((gGGAuthCtx.m_UseLog & 2) == 0)
			return ERROR_SUCCESS;

#if defined(_WIN32)
		sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] UpdateProtocol() - Invalid protocol instance [%p]", _pProtocol);
#elif defined(__linux__)
		sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] UpdateProtocol() - Invalid protocol instance [%p]", _pProtocol);
#endif
		NpLog(2, gGGAuthCtx.m_MsgBuff);

		return ERROR_SUCCESS;
	}

	if (_pProtocol->m_dwGGVer > 0x1004F && _pProtocol->m_dwGGVer < 0x20001) {

		if (gGGAuthCtx.m_EntryListProtocol == nullptr || _pProtocol->m_dwGGVer < gGGAuthCtx.m_EntryListProtocol->m_dwGGVer) {

			if ((gGGAuthCtx.m_UseLog & 2) != 0) {
#if defined(_WIN32)
				sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] UpdateProtocol() error #1036 [%x]", _pProtocol->m_dwGGVer);
#elif defined(__linux__)
				sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] UpdateProtocol() error #1036 [%x]", _pProtocol->m_dwGGVer);
#endif
				NpLog(2, gGGAuthCtx.m_MsgBuff);
			}

			return ERROR_GGAUTH_INVALID_PROTOCOL_VERSION;
		}

		if (_pProtocol->m_dwGGVer > gGGAuthCtx.m_EntryListProtocol->m_dwGGVer && gGGAuthCtx.m_ProtocolVerUpdateCtx.m_bCountStart == false) {

			gGGAuthCtx.m_ProtocolVerUpdateCtx.m_bCountStart = true;
			gGGAuthCtx.m_ProtocolVerUpdateCtx.m_CountStart = (DWORD)std::time(nullptr);

			if ((gGGAuthCtx.m_UseLog & 1) != 0) {
#if defined(_WIN32)
				sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] UpdateProtocol(): New Protocol version Count Start [%x]:[%d]", _pProtocol->m_dwGGVer, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_CountStart);
#elif defined(__linux__)
				sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] UpdateProtocol(): New Protocol version Count Start [%x]:[%d]", _pProtocol->m_dwGGVer, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_CountStart);
#endif
				NpLog(1, gGGAuthCtx.m_MsgBuff);
			}
		}

		time_t now = std::time(nullptr);
		INT32 diff_sec = 0;
		UINT32 j, i, dwTemp, sum_val1 = 0;

		for (i = 0; i < (sizeof(gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table) / sizeof(_GG_VERSION_UPDATE)); i++) {

			if (gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwGGVer == 0 || gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwGGVer != _pProtocol->m_dwGGVer)
				continue;

			diff_sec = (INT32)difftime(gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwTime_1, now);

			if (diff_sec >= 3600)
				continue;

			gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwCount++;
			gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwTime_1 = (UINT32)now;

			if (gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwCount < gGGAuthCtx.m_dwNumActiveSession || (INT32)(now - gGGAuthCtx.m_ProtocolVerUpdateCtx.m_CountStart) <= (gGGAuthCtx.m_UpdateTimeLimit * 60)
					|| gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwGGVer <= gGGAuthCtx.m_EntryListProtocol->m_dwGGVer) {

				if ((gGGAuthCtx.m_UseLog & 1) != 0) {
#if defined(_WIN32)
					sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] UpdateProtocol(): PrtcVer < new Version[%x] NEW VERSION count up [%d]:[%x][%d][%d][%d]", 
							_pProtocol->m_dwGGVer, i, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwGGVer, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwCount, 
							(UINT32)now, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwTIme_2);
#elif defined(__linux__)
					sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] UpdateProtocol(): PrtcVer < new Version[%x] NEW VERSION count up [%d]:[%x][%d][%d][%d]", 
							_pProtocol->m_dwGGVer, i, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwGGVer, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwCount, 
							(UINT32)now, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwTIme_2);
#endif
					NpLog(1, gGGAuthCtx.m_MsgBuff);
				}

				if ((gGGAuthCtx.m_UseLog & 1) != 0) {
#if defined(_WIN32)
					sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] UpdateProtocol() ----Protocol Ver Update RESULT----");
#elif defined(__linux__)
					sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] UpdateProtocol() ----Protocol Ver Update RESULT----");
#endif
					NpLog(1, gGGAuthCtx.m_MsgBuff);
				}

				for (j = 0; j < (sizeof(gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table) / sizeof(_GG_VERSION_UPDATE)); j++) {

					if (gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwCount > 0 && (gGGAuthCtx.m_UseLog & 1) != 0) {
#if defined(_WIN32)
						sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] [%d]:[%x][%d][%d][%d], ",
								j, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwGGVer, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwCount, 
								gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwTime_1, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwTIme_2);
#elif defined(__linux__)
						sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] [%d]:[%x][%d][%d][%d], ",
								j, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwGGVer, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwCount, 
								gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwTime_1, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwTIme_2);
#endif
						NpLog(1, gGGAuthCtx.m_MsgBuff);
					}
				}

				if ((gGGAuthCtx.m_UseLog & 1) != 0) {

#if defined(_WIN32)
					sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] CURRENT VER: %x", gGGAuthCtx.m_EntryListProtocol->m_dwGGVer);
#elif defined(__linux__)
					sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] CURRENT VER: %x", gGGAuthCtx.m_EntryListProtocol->m_dwGGVer);
#endif
					NpLog(1, gGGAuthCtx.m_MsgBuff);

#if defined(_WIN32)
					sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] UpdateProtocol() ---------------------------------");
#elif defined(__linux__)
					sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] UpdateProtocol() ---------------------------------");
#endif
					NpLog(1, gGGAuthCtx.m_MsgBuff);
				}

				return ERROR_SUCCESS;
			}

			for (j = 0; j < (sizeof(gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table) / sizeof(_GG_VERSION_UPDATE)); j++)
				sum_val1 += gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwCount;

			dwTemp = 0xFFFFFFFF;

			for (j = 0; j < (sizeof(gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table) / sizeof(_GG_VERSION_UPDATE)); j++) {

				if (gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwGGVer > gGGAuthCtx.m_EntryListProtocol->m_dwGGVer && gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwCount > ((gGGAuthCtx.m_UpdateCondition * sum_val1) / 100)) {

					if ((gGGAuthCtx.m_UseLog & 1) != 0) {
#if defined(_WIN32)
						sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] UpdateProtocol(): New PrtcNew[%x] find that it accepted condition[%d/%d] and [%d%%] > [%d%%]",
								gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwGGVer, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwCount, sum_val1, 
								(gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwCount * 100) / sum_val1, gGGAuthCtx.m_UpdateCondition);
#elif defined(__linux__)
						sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] UpdateProtocol(): New PrtcNew[%x] find that it accepted condition[%d/%d] and [%d%%] > [%d%%]",
								gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwGGVer, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwCount, sum_val1, 
								(gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwCount * 100) / sum_val1, gGGAuthCtx.m_UpdateCondition);
#endif
						NpLog(1, gGGAuthCtx.m_MsgBuff);
					}

					if (dwTemp == 0xFFFFFFFF || gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwGGVer > gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[dwTemp].m_dwGGVer)
						dwTemp = j;
				}
			}

			if (dwTemp == 0xFFFFFFFF) {

				if ((gGGAuthCtx.m_UseLog & 1) != 0) {
#if defined(_WIN32)
					sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] UpdateProtocol(): New Protocol version can not find at list that it accepted condition. Reset all of data.");
#elif defined(__linux__)
					sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] UpdateProtocol(): New Protocol version can not find at list that it accepted condition. Reset all of data.");
#endif
					NpLog(1, gGGAuthCtx.m_MsgBuff);
				}

			}else {

				if ((gGGAuthCtx.m_UseLog & 2) != 0) {
#if defined(_WIN32)
					sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] UpdateProtocol() -----GG Ver Update RESULT-----");
#elif defined(__linux__)
					sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] UpdateProtocol() -----GG Ver Update RESULT-----");
#endif
					NpLog(2, gGGAuthCtx.m_MsgBuff);
				}

				for (j = 0; j < (sizeof(gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table) / sizeof(_GG_VERSION_UPDATE)); j++) {


					if ((gGGAuthCtx.m_UseLog & 2) != 0) {
#if defined(_WIN32)
						sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] [%d]:[%x][%d][%d][%d]: %d%%, ",
								j, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwGGVer, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwCount, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwTime_1, 
								gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwTIme_2, (gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwCount * 100) / sum_val1);
#elif defined(__linux__)
						sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] [%d]:[%x][%d][%d][%d]: %d%%, ",
								j, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwGGVer, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwCount, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwTime_1, 
								gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwTIme_2, (gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwCount * 100) / sum_val1);
#endif
						NpLog(2, gGGAuthCtx.m_MsgBuff);
					}
				}

				if ((gGGAuthCtx.m_UseLog & 2) != 0) {

#if defined(_WIN32)
					sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] CURRENT PRTC: %x", gGGAuthCtx.m_EntryListProtocol->m_dwGGVer);
#elif defined(__linux__)
					sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] CURRENT PRTC: %x", gGGAuthCtx.m_EntryListProtocol->m_dwGGVer);
#endif
					NpLog(2, gGGAuthCtx.m_MsgBuff);

#if defined(_WIN32)
					sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] UpdateProtocol() -----------------------------");
#elif defined(__linux__)
					sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] UpdateProtocol() -----------------------------");
#endif
					NpLog(2, gGGAuthCtx.m_MsgBuff);
				}

				auto pNext = _pProtocol;

				if (gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[dwTemp].m_dwGGVer != _pProtocol->m_dwGGVer) {

					pNext = gGGAuthCtx.m_EntryListProtocol;

					do {

						if (pNext == nullptr) {

							if ((gGGAuthCtx.m_UseLog & 2) != 0) {
#if defined(_WIN32)
								sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] UpdateProtocol() error #1035 [%d]:[%x]", dwTemp, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[dwTemp].m_dwGGVer);
#elif defined(__linux__)
								sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] UpdateProtocol() error #1035 [%d]:[%x]", dwTemp, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[dwTemp].m_dwGGVer);
#endif
								NpLog(2, gGGAuthCtx.m_MsgBuff);
							}

							return ERROR_GGAUTH_INVALID_PROTOCOL_VERSION;
						}

						if (pNext->m_dwGGVer == gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[dwTemp].m_dwGGVer)
							break;

						pNext = pNext->m_pNext;

					} while (true);
				}

				_GG_UPREPORT report;

				report.dwBefore = gGGAuthCtx.m_EntryListProtocol->m_dwGGVer;
				report.dwNext = pNext->m_dwGGVer;
				report.nType = 2;

				if (pNext->m_pNext == nullptr) {

					if (pNext->m_pPrev != nullptr)
						pNext->m_pPrev->m_pNext = nullptr;
				
				}else {

					pNext->m_pNext->m_pPrev = pNext->m_pPrev;

					if (pNext->m_pPrev != nullptr)
						pNext->m_pPrev->m_pNext = pNext->m_pNext;
				}

				gGGAuthCtx.m_EntryListProtocol->m_pPrev = pNext;
				gGGAuthCtx.m_EntryListProtocol->m_bDisuse = TRUE;

				pNext->m_pPrev = nullptr;
				pNext->m_pNext = gGGAuthCtx.m_EntryListProtocol;

				gGGAuthCtx.m_EntryListProtocol = pNext;

				GGAuthUpdateCallback(&report);

				if ((gGGAuthCtx.m_UseLog & 1) != 0) {
#if defined(_WIN32)
					sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] UpdateProtocol(): NEW PROTOCOL ACTIVE!!!!!! [0x%x]:[%d][%d], ",
							pNext->m_dwGGVer, pNext->RefCount, (UINT32)pNext->reg_datetime);
#elif defined(__linux__)
					sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] UpdateProtocol(): NEW PROTOCOL ACTIVE!!!!!! [0x%x]:[%d][%d], ",
							pNext->m_dwGGVer, pNext->RefCount, (UINT32)pNext->reg_datetime);
#endif
					NpLog(1, gGGAuthCtx.m_MsgBuff);
				}
			}

			memset(gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table, 0, sizeof(gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table));

			gGGAuthCtx.m_ProtocolVerUpdateCtx.m_bCountStart = false;

			if ((gGGAuthCtx.m_UseLog & 1) != 0) {
#if defined(_WIN32)
				sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] UpdateProtocol() ----Protocol Ver Update RESULT----");
#elif defined(__linux__)
				sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] UpdateProtocol() ----Protocol Ver Update RESULT----");
#endif
				NpLog(1, gGGAuthCtx.m_MsgBuff);
			}

			for (j = 0; j < (sizeof(gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table) / sizeof(_GG_VERSION_UPDATE)); j++) {

				if (gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwCount > 0 && (gGGAuthCtx.m_UseLog & 1) != 0) {
#if defined(_WIN32)
					sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] [%d]:[%x][%d][%d][%d], ",
							j, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwGGVer, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwCount, 
							gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwTime_1, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwTIme_2);
#elif defined(__linux__)
					sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] [%d]:[%x][%d][%d][%d], ",
							j, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwGGVer, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwCount, 
							gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwTime_1, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwTIme_2);
#endif
					NpLog(1, gGGAuthCtx.m_MsgBuff);
				}
			}

			if ((gGGAuthCtx.m_UseLog & 1) != 0) {

#if defined(_WIN32)
				sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] CURRENT VER: %x", gGGAuthCtx.m_EntryListProtocol->m_dwGGVer);
#elif defined(__linux__)
				sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] CURRENT VER: %x", gGGAuthCtx.m_EntryListProtocol->m_dwGGVer);
#endif
				NpLog(1, gGGAuthCtx.m_MsgBuff);

#if defined(_WIN32)
				sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] UpdateProtocol() ---------------------------------");
#elif defined(__linux__)
				sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] UpdateProtocol() ---------------------------------");
#endif
				NpLog(1, gGGAuthCtx.m_MsgBuff);
			}

			return ERROR_SUCCESS;
		}

		for (i = 0; i < (sizeof(gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table) / sizeof(_GG_VERSION_UPDATE)); i++) {

			if (gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwGGVer == 0) {

				gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwGGVer = _pProtocol->m_dwGGVer;
				gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwCount = 1;
				gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwTime_1 = (UINT32)now;
				gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwTIme_2 = (UINT32)now;

				break;
			}
		}

		if (i == 5) {

			i = 0xFFFFFFFF;

			for (j = 0; j < (sizeof(gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table) / sizeof(_GG_VERSION_UPDATE)); j++) {

				diff_sec = (INT32)difftime(gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwTime_1, now);

				if (diff_sec > 3600 && gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwCount < gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i != 0xFFFFFFFF ? i : 0].m_dwCount)
					i = j;
			}

			if (i == 0xFFFFFFFF) {

				for (j = 0; j < (sizeof(gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table) / sizeof(_GG_VERSION_UPDATE)); j++) {

					if (gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwTime_1 < gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i != 0xFFFFFFFF ? i : 0].m_dwTime_1)
						i = j;
				}

				if (i == 0xFFFFFFFF)
					i = 0;

				if ((gGGAuthCtx.m_UseLog & 1) != 0) {
#if defined(_WIN32)
					sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] UpdateProtocol(): add a new Protocol version to the Protocol version list[%x]. OLDEST VERSION [%d]:[%x][%d][%d][%d], ",
							_pProtocol->m_dwGGVer, i, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwGGVer, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwCount, 
							gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwTime_1, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwTIme_2);
#elif defined(__linux__)
					sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] UpdateProtocol(): add a new Protocol version to the Protocol version list[%x]. OLDEST VERSION [%d]:[%x][%d][%d][%d], ",
							_pProtocol->m_dwGGVer, i, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwGGVer, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwCount, 
							gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwTime_1, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwTIme_2);
#endif
					NpLog(1, gGGAuthCtx.m_MsgBuff);
				}

			}else if ((gGGAuthCtx.m_UseLog & 1) != 0) {
#if defined(_WIN32)
				sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] UpdateProtocol(): add a new Protocol version to the Protocol version list[%x]. 1 hour time out and lower users [%d]:[%x][%d][%d][%d]",
						_pProtocol->m_dwGGVer, i, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwGGVer, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwCount, 
						gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwTime_1, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwTIme_2);
#elif defined(__linux__)
				sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] UpdateProtocol(): add a new Protocol version to the Protocol version list[%x]. 1 hour time out and lower users [%d]:[%x][%d][%d][%d]",
						_pProtocol->m_dwGGVer, i, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwGGVer, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwCount, 
						gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwTime_1, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwTIme_2);
#endif
				NpLog(1, gGGAuthCtx.m_MsgBuff);
			}

			gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwGGVer = _pProtocol->m_dwGGVer;
			gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwCount = 1;
			gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwTime_1 = (UINT32)now;
			gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwTIme_2 = (UINT32)now;

			if ((gGGAuthCtx.m_UseLog & 1) != 0) {
#if defined(_WIN32)
				sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib]                  -> [%d]:[%x][%d][%d][%d], ",
						i, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwGGVer, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwCount, 
						gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwTime_1, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwTIme_2);
#elif defined(__linux__)
				sprintf(gGGAuthCtx.m_MsgBuff, "[S lib]                  -> [%d]:[%x][%d][%d][%d], ",
						i, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwGGVer, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwCount, 
						gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwTime_1, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwTIme_2);
#endif
				NpLog(1, gGGAuthCtx.m_MsgBuff);
			}

		}else {

			if ((gGGAuthCtx.m_UseLog & 1) != 0) {
#if defined(_WIN32)
				sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] UpdateProtocol(): insert a new Protocol version to the Protocol version list [%d]:[%x][%d][%d][%d], ",
						i, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwGGVer, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwCount, 
						gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwTime_1, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwTIme_2);
#elif defined(__linux__)
				sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] UpdateProtocol(): insert a new Protocol version to the Protocol version list [%d]:[%x][%d][%d][%d], ",
						i, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwGGVer, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwCount, 
						gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwTime_1, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[i].m_dwTIme_2);
#endif
				NpLog(1, gGGAuthCtx.m_MsgBuff);
			}
		}

		if ((gGGAuthCtx.m_UseLog & 1) != 0) {
#if defined(_WIN32)
			sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] UpdateProtocol() -----Protocol Ver Update RESULT-----");
#elif defined(__linux__)
			sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] UpdateProtocol() -----Protocol Ver Update RESULT-----");
#endif
			NpLog(1, gGGAuthCtx.m_MsgBuff);
		}

		for (j = 0; j < (sizeof(gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table) / sizeof(_GG_VERSION_UPDATE)); j++) {

			if (gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwCount > 0 && (gGGAuthCtx.m_UseLog & 1) != 0) {
#if defined(_WIN32)
				sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] [%d]:[%x][%d][%d][%d], ",
						j, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwGGVer, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwCount, 
						gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwTime_1, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwTIme_2);
#elif defined(__linux__)
				sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] [%d]:[%x][%d][%d][%d], ",
						j, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwGGVer, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwCount, 
						gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwTime_1, gGGAuthCtx.m_ProtocolVerUpdateCtx.m_Table[j].m_dwTIme_2);
#endif
				NpLog(1, gGGAuthCtx.m_MsgBuff);
			}
		}

		if ((gGGAuthCtx.m_UseLog & 1) != 0) {

#if defined(_WIN32)
			sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] CURRENT VER: %x", gGGAuthCtx.m_EntryListProtocol->m_dwGGVer);
#elif defined(__linux__)
			sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] CURRENT VER: %x", gGGAuthCtx.m_EntryListProtocol->m_dwGGVer);
#endif
			NpLog(1, gGGAuthCtx.m_MsgBuff);

#if defined(_WIN32)
			sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] UpdateProtocol() -----------------------------------");
#elif defined(__linux__)
			sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] UpdateProtocol() -----------------------------------");
#endif
			NpLog(1, gGGAuthCtx.m_MsgBuff);
		}

		return ERROR_SUCCESS;
	}

	if ((gGGAuthCtx.m_UseLog & 2) != 0) {
#if defined(_WIN32)
		sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] UpdateProtocol() - Invalid protocol version [%x]", _pProtocol->m_dwGGVer);
#elif defined(__linux__)
		sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] UpdateProtocol() - Invalid protocol version [%x]", _pProtocol->m_dwGGVer);
#endif
		NpLog(2, gGGAuthCtx.m_MsgBuff);
	}

	return ERROR_SUCCESS;
}

// export static
UINT32 InitGameguardAuth(char* sGGPath, UINT32 dwNumActive, int useTimer, int useLog) {

	memset(&gGGAuthCtx, 0, sizeof(_GG_AUTH_CTX));

	gGGAuthCtx.m_dwNumActiveSession = dwNumActive;
	gGGAuthCtx.m_StrPathTmp[0] = '\0';
	gGGAuthCtx.m_dwCurrentVersion = CURRENT_VERSION;
	gGGAuthCtx.m_dwVersionLimitMax = VERSION_LIMIT_MAX;
	gGGAuthCtx.m_GGVerUpdateCtx.m_bCountStart = false;
	gGGAuthCtx.m_ProtocolVerUpdateCtx.m_bCountStart = false;
	gGGAuthCtx.m_UpdateCondition = 0x32;
	gGGAuthCtx.m_UpdateTimeLimit = 0x1E;
	
	gGGAuthCtx.m_bAuth = true;
	gGGAuthCtx.m_bUseTimer = useTimer;
	gGGAuthCtx.m_UseLog = useLog;

#if defined(_WIN32)
	InitializeCriticalSection(&gGGAuthCtx.m_csGGVer);
	InitializeCriticalSection(&gGGAuthCtx.m_csProtocol);
	InitializeCriticalSection(&gGGAuthCtx.m_csUniqVal);
#elif defined(__linux__)
	pthread_mutexattr_t __mattr;
	pthread_mutexattr_init(&__mattr);
	pthread_mutexattr_settype(&__mattr, PTHREAD_MUTEX_RECURSIVE);

	pthread_mutex_init(&gGGAuthCtx.m_csGGVer, &__mattr);
	pthread_mutex_init(&gGGAuthCtx.m_csProtocol, &__mattr);
	pthread_mutex_init(&gGGAuthCtx.m_csUniqVal, &__mattr);

	pthread_mutexattr_destroy(&__mattr);
#endif

	if (sGGPath != nullptr && sGGPath[0] != '\0') {

		auto len = strlen(sGGPath);

#if defined(_WIN32)
		if (sGGPath[len - 1] == gSeparator)
			strcpy_s(gGGAuthCtx.m_StrPathTmp, sizeof(gGGAuthCtx.m_StrPathTmp), sGGPath);
		else
			sprintf_s(gGGAuthCtx.m_StrPathTmp, sizeof(gGGAuthCtx.m_StrPathTmp), "%s%c", sGGPath, gSeparator);
#elif defined(__linux__)
		if (sGGPath[len - 1] == gSeparator)
			strcpy(gGGAuthCtx.m_StrPathTmp, sGGPath);
		else
			sprintf(gGGAuthCtx.m_StrPathTmp, "%s%c", sGGPath, gSeparator);
#endif
	}

	gGGAuthCtx.m_bInitState = true;

	char full_path[512];

#if defined(_WIN32)
	sprintf_s(full_path, sizeof(full_path), "%s%s", gGGAuthCtx.m_StrPathTmp, gConf_file_name);
#elif defined(__linux__)
	sprintf(full_path, "%s%s", gGGAuthCtx.m_StrPathTmp, gConf_file_name);
#endif

	FILE *inConf;
	
#if defined(_WIN32)
	fopen_s(&inConf, full_path, "r");
#elif defined(__linux__)
	inConf = fopen(full_path, "r");
#endif

	if (inConf == nullptr) {
		CleanupGameguardAuth();

		if ((gGGAuthCtx.m_UseLog & 2) != 0) {
#if defined(_WIN32)
			sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib][err:%d] InitGameguardAuth() - Failed to open csauth2.cfg : %s", ERROR_GGAUTH_FAIL_LOAD_CFG, full_path);
#elif defined(__linux__)
			sprintf(gGGAuthCtx.m_MsgBuff, "[S lib][err:%d] InitGameguardAuth() - Failed to open csauth2.cfg : %s", ERROR_GGAUTH_FAIL_LOAD_CFG, full_path);
#endif
			NpLog(2, gGGAuthCtx.m_MsgBuff);
		}
		return ERROR_GGAUTH_FAIL_LOAD_CFG;
	}

	if ((gGGAuthCtx.m_UseLog & 1) != 0) {
#if defined(_WIN32)
		sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] InitGameguardAuth() - Success to open csauth2.cfg : %s", full_path);
#elif defined(__linux__)
		sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] InitGameguardAuth() - Success to open csauth2.cfg : %s", full_path);
#endif
		NpLog(1, gGGAuthCtx.m_MsgBuff);
	}

	fseek(inConf, 0, SEEK_SET);

	UINT32 protocolVers[100];
	UINT32 protocol_list_count = 0;

	for (protocol_list_count = 0; protocol_list_count < 100; protocol_list_count++)
#if defined(_WIN32)
		if (fscanf_s(inConf, "%d", &protocolVers[protocol_list_count]) == -1)
#elif defined(__linux__)
		if (fscanf(inConf, "%d", &protocolVers[protocol_list_count]) == -1)
#endif
			break;

	if (protocol_list_count == 0) {

		if ((gGGAuthCtx.m_UseLog & 2) != 0) {
#if defined(_WIN32)
			sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] InitGameguardAuth() - Before CleanupGameguardAuth()");
#elif defined(__linux__)
			sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] InitGameguardAuth() - Before CleanupGameguardAuth()");
#endif
			NpLog(2, gGGAuthCtx.m_MsgBuff);
		}

		CleanupGameguardAuth();
		
		if ((gGGAuthCtx.m_UseLog & 2) != 0) {
#if defined(_WIN32)
			sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib][err:%d] InitGameguardAuth() - csauth2.cfg contained nothing", ERROR_GGAUTH_FAIL_LOAD_CFG);
#elif defined(__linux__)
			sprintf(gGGAuthCtx.m_MsgBuff, "[S lib][err:%d] InitGameguardAuth() - csauth2.cfg contained nothing", ERROR_GGAUTH_FAIL_LOAD_CFG);
#endif
			NpLog(2, gGGAuthCtx.m_MsgBuff);
		}
		
		fclose(inConf);
		
		return ERROR_GGAUTH_FAIL_LOAD_CFG;
	}

	if ((gGGAuthCtx.m_UseLog & 1) != 0) {
#if defined(_WIN32)
		sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] InitGameguardAuth() - Protocol list count : %d", protocol_list_count);
#elif defined(__linux__)
		sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] InitGameguardAuth() - Protocol list count : %d", protocol_list_count);
#endif
		NpLog(1, gGGAuthCtx.m_MsgBuff);
	}

	BOOL protocol_head;
	UINT32 error;
	
	if (protocol_list_count > 0) {

		for (UINT32 i = 0; i < protocol_list_count; i++) {

#if defined(_WIN32)
			sprintf_s(full_path, sizeof(full_path), gGG_auth_dll_name, gGGAuthCtx.m_StrPathTmp, protocolVers[i]);
#elif defined(__linux__)
			sprintf(full_path, gGG_auth_dll_name, gGGAuthCtx.m_StrPathTmp, protocolVers[i]);
#endif

			if (i == 0) {

				if ((gGGAuthCtx.m_UseLog & 1) != 0) {
#if defined(_WIN32)
					sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] InitGameguardAuth() - Load Protocol: %s", full_path);
#elif defined(__linux__)
					sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] InitGameguardAuth() - Load Protocol: %s", full_path);
#endif
					NpLog(1, gGGAuthCtx.m_MsgBuff);
				}
				
				protocol_head = TRUE;

			}else {

				if ((gGGAuthCtx.m_UseLog & 1) != 0) {
#if defined(_WIN32)
					sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] InitGameguardAuth() Load additional protocol: %s", full_path);
#elif defined(__linux__)
					sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] InitGameguardAuth() Load additional protocol: %s", full_path);
#endif
					NpLog(1, gGGAuthCtx.m_MsgBuff);
				}

				protocol_head = FALSE;
			}

			if ((error = LoadAuthProtocol(full_path, protocol_head)) != ERROR_SUCCESS) {

				if ((gGGAuthCtx.m_UseLog & 2) != 0) {
#if defined(_WIN32)
					sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib][err:%d] InitGameguardAuth() - LoadAuthProtocol() failed: %s", error, full_path);
#elif defined(__linux__)
					sprintf(gGGAuthCtx.m_MsgBuff, "[S lib][err:%d] InitGameguardAuth() - LoadAuthProtocol() failed: %s", error, full_path);
#endif
					NpLog(2, gGGAuthCtx.m_MsgBuff);
				}

				CleanupGameguardAuth();
				fclose(inConf);
				return error;
			}
		}
	}

	fclose(inConf);

#if defined(_WIN32)
	sprintf_s(full_path, sizeof(full_path), "%s%s", gGGAuthCtx.m_StrPathTmp, gVer_file_name);
#elif defined(__linux__)
	sprintf(full_path, "%s%s", gGGAuthCtx.m_StrPathTmp, gVer_file_name);
#endif

#if defined(_WIN32)
	fopen_s(&inConf, full_path, "r");
#elif defined(__linux__)
	inConf = fopen(full_path, "r");
#endif

	if (inConf != nullptr) {

		if ((gGGAuthCtx.m_UseLog & 1) != 0) {
#if defined(_WIN32)
			sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] InitGameguardAuth() - Success to load csver.cfg: %s", full_path);
#elif defined(__linux__)
			sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] InitGameguardAuth() - Success to load csver.cfg: %s", full_path);
#endif
			NpLog(1, gGGAuthCtx.m_MsgBuff);
		}

		fseek(inConf, 0, SEEK_SET);

		UINT32 ver;

#if defined(_WIN32)
		if (fscanf_s(inConf, "%d", &ver) == -1) {
#elif defined(__linux__)
		if (fscanf(inConf, "%d", &ver) == -1) {
#endif

			if ((gGGAuthCtx.m_UseLog & 2) != 0) {
#if defined(_WIN32)
				sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] InitGameguardAuth() - csver.cfg contained nothing: %s", full_path);
#elif defined(__linux__)
				sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] InitGameguardAuth() - csver.cfg contained nothing: %s", full_path);
#endif
				NpLog(2, gGGAuthCtx.m_MsgBuff);
			}

		}else if (ver == 0 || ver < CURRENT_VERSION || ver > VERSION_LIMIT_MAX) {

			if ((gGGAuthCtx.m_UseLog & 2) != 0) {
#if defined(_WIN32)
				sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] InitGameguardAuth() - csver.cfg has invalid ggver limit. skip: %d", ver);
#elif defined(__linux__)
				sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] InitGameguardAuth() - csver.cfg has invalid ggver limit. skip: %d", ver);
#endif
				NpLog(2, gGGAuthCtx.m_MsgBuff);
			}

		}else
			gGGAuthCtx.m_dwVersionLimitMax = ver;

		fclose(inConf);
	}

	Blowfish_Init(&gGGAuthCtx.m_BlowfishCtx, (char*)gBlowfish_init_key, sizeof(gBlowfish_init_key) - 1/*remove null terminate character*/);

	if ((gGGAuthCtx.m_UseLog & 2) != 0) {
#if defined(_WIN32)
		sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] InitGameguardAuth() - Success (%s)", gGGAuthCtx.m_StrPathTmp);
#elif defined(__linux__)
		sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] InitGameguardAuth() - Success (%s)", gGGAuthCtx.m_StrPathTmp);
#endif
		NpLog(2, gGGAuthCtx.m_MsgBuff);
	}

	return ERROR_SUCCESS;
};

void CleanupGameguardAuth() {

	if (gGGAuthCtx.m_EntryListProtocol != nullptr) {

		_GG_AUTH_PROTOCOL *pNext = nullptr;
		
		do {
			
			pNext = gGGAuthCtx.m_EntryListProtocol;

			if ((gGGAuthCtx.m_UseLog & 1) != 0) {
#if defined(_WIN32)
				sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] CleanupGameguardAuth() - FREE col[0x%p] dwVersion[%d] pNext[0x%p] pProtocol->pBefore[0x%p]",
						gGGAuthCtx.m_EntryListProtocol, gGGAuthCtx.m_EntryListProtocol->m_dwGGVer, gGGAuthCtx.m_EntryListProtocol->m_pNext, gGGAuthCtx.m_EntryListProtocol->m_pPrev);
#elif defined(__linux__)
				sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] CleanupGameguardAuth() - FREE col[0x%p] dwVersion[%d] pNext[0x%p] pProtocol->pBefore[0x%p]",
						gGGAuthCtx.m_EntryListProtocol, gGGAuthCtx.m_EntryListProtocol->m_dwGGVer, gGGAuthCtx.m_EntryListProtocol->m_pNext, gGGAuthCtx.m_EntryListProtocol->m_pPrev);
#endif
				NpLog(2, gGGAuthCtx.m_MsgBuff);
			}

			gGGAuthCtx.m_EntryListProtocol = pNext->m_pNext;

			if (pNext->m_hModule != NULL)
#if defined(_WIN32)
				FreeLibrary(pNext->m_hModule);
#elif defined(__linux__)
				dlclose(pNext->m_hModule);
#endif

			free(pNext);

		} while (gGGAuthCtx.m_EntryListProtocol != nullptr);
	}

#if defined(_WIN32)
	DeleteCriticalSection(&gGGAuthCtx.m_csGGVer);
	DeleteCriticalSection(&gGGAuthCtx.m_csProtocol);
	DeleteCriticalSection(&gGGAuthCtx.m_csUniqVal);
#elif defined(__linux__)
	pthread_mutex_destroy(&gGGAuthCtx.m_csGGVer);
	pthread_mutex_destroy(&gGGAuthCtx.m_csProtocol);
	pthread_mutex_destroy(&gGGAuthCtx.m_csUniqVal);
#endif

	if ((gGGAuthCtx.m_UseLog & 1) != 0) {
#if defined(_WIN32)
		sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] CleanupGameguardAuth()");
#elif defined(__linux__)
		sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] CleanupGameguardAuth()");
#endif
		NpLog(2, gGGAuthCtx.m_MsgBuff);
	}
};

//useTimer 가 true 일 경우 호출.
UINT32 GGAuthUpdateTimer() {

	time_t now;
	tm local;
	
	now = std::time(nullptr);
#if defined(_WIN32)
	localtime_s(&local, &now);
#elif defined(__linux__)
	localtime_r(&now, &local);
#endif

	for (auto protocol = gGGAuthCtx.m_EntryListProtocol; protocol != nullptr; protocol = protocol->m_pNext) {

		if ((gGGAuthCtx.m_UseLog & 1) != 0) {
#if defined(_WIN32)
			sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] GGAuthUpdateTimero() - [%x] %d/%d/%d %d:%d:%d",
					protocol->m_dwGGVer, local.tm_year + 1900, local.tm_mon + 1, local.tm_mday, 
					local.tm_hour, local.tm_min, local.tm_sec);
#elif defined(__linux__)
			sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] GGAuthUpdateTimero() - [%x] %d/%d/%d %d:%d:%d",
					protocol->m_dwGGVer, local.tm_year + 1900, local.tm_mon + 1, local.tm_mday, 
					local.tm_hour, local.tm_min, local.tm_sec);
#endif
			NpLog(1, gGGAuthCtx.m_MsgBuff);
		}

		protocol->PrtcUpdateTimer();
	}

	return ERROR_SUCCESS;

};

UINT32 AddAuthProtocol(char* sDllName) {

	UINT32 error = LoadAuthProtocol(sDllName, 0);

	if (error != ERROR_SUCCESS) {

		if ((gGGAuthCtx.m_UseLog & 2) != 0) {
#if defined(_WIN32)
			sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib][err:%d] AddAuthProtocol() - Failed: %s", error, sDllName);
#elif defined(__linux__)
			sprintf(gGGAuthCtx.m_MsgBuff, "[S lib][err:%d] AddAuthProtocol() - Failed: %s", error, sDllName);
#endif
			NpLog(2, gGGAuthCtx.m_MsgBuff);
		}
	}

	if ((gGGAuthCtx.m_UseLog & 1) != 0) {
#if defined(_WIN32)
		sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] AddAuthProtocol() - Success : %s", sDllName);
#elif defined(__linux__)
		sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] AddAuthProtocol() - Success : %s", sDllName);
#endif
		NpLog(1, gGGAuthCtx.m_MsgBuff);
	}

	return ERROR_SUCCESS;
};

//버전 상한선을 제한한다.
UINT32 SetGGVerLimit(UINT32 nLimitVer) {

	if (nLimitVer == 0 || nLimitVer < CURRENT_VERSION || nLimitVer > VERSION_LIMIT_MAX) {

		if ((gGGAuthCtx.m_UseLog & 2) != 0) {
#if defined(_WIN32)
			sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib][err:%d] SetGGVerLimit() - Invalid ggver: %d", ERROR_GGAUTH_INVALID_GAMEMON_VER, nLimitVer);
#elif defined(__linux__)
			sprintf(gGGAuthCtx.m_MsgBuff, "[S lib][err:%d] SetGGVerLimit() - Invalid ggver: %d", ERROR_GGAUTH_INVALID_GAMEMON_VER, nLimitVer);
#endif
			NpLog(2, gGGAuthCtx.m_MsgBuff);
		}

		return ERROR_GGAUTH_INVALID_GAMEMON_VER;
	}

	if (nLimitVer <= CURRENT_VERSION) {

		if ((gGGAuthCtx.m_UseLog & 2) != 0) {
#if defined(_WIN32)
			sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib][err:%d] SetGGVerLimit() - Too low ggver: %d <= %d", ERROR_GGAUTH_INVALID_GAMEMON_VER, nLimitVer, CURRENT_VERSION);
#elif defined(__linux__)
			sprintf(gGGAuthCtx.m_MsgBuff, "[S lib][err:%d] SetGGVerLimit() - Too low ggver: %d <= %d", ERROR_GGAUTH_INVALID_GAMEMON_VER, nLimitVer, CURRENT_VERSION);
#endif
			NpLog(2, gGGAuthCtx.m_MsgBuff);
		}

		return ERROR_GGAUTH_INVALID_GAMEMON_VER;
	}

	char full_path[512];

#if defined(_WIN32)
	sprintf_s(full_path, sizeof(full_path), "%s%s", gGGAuthCtx.m_StrPathTmp, gVer_file_name);
#elif defined(__linux__)
	sprintf(full_path, "%s%s", gGGAuthCtx.m_StrPathTmp, gVer_file_name);
#endif

	FILE *inVer = nullptr;

#if defined(_WIN32)
	fopen_s(&inVer, full_path, "w");
#elif defined(__linux__)
	inVer = fopen(full_path, "w");
#endif

	if (inVer == nullptr) {

		if ((gGGAuthCtx.m_UseLog & 2) != 0) {
#if defined(_WIN32)
			sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib][err:%d] SetGGVerLimit() - Failed to open: %s", ERROR_GGAUTH_FAIL_LOAD_CFG, full_path);
#elif defined(__linux__)
			sprintf(gGGAuthCtx.m_MsgBuff, "[S lib][err:%d] SetGGVerLimit() - Failed to open: %s", ERROR_GGAUTH_FAIL_LOAD_CFG, full_path);
#endif
			NpLog(2, gGGAuthCtx.m_MsgBuff);
		}

		return ERROR_GGAUTH_FAIL_LOAD_CFG;
	}

#if defined(_WIN32)
	fprintf_s(inVer, "%d", nLimitVer);
#elif defined(__linux__)
	fprintf(inVer, "%d", nLimitVer);
#endif
	fclose(inVer);

	gGGAuthCtx.m_dwVersionLimitMax = nLimitVer;

	return ERROR_SUCCESS;
};

//업데이트 조건을 설정한다.
UINT32 SetUpdateCondition(int nTimeLimit, int nCondition) {

	if (nTimeLimit > 0 && nCondition > 0 && nCondition < 101) {

		gGGAuthCtx.m_UpdateCondition = nCondition;
		gGGAuthCtx.m_UpdateTimeLimit = nTimeLimit;
		
		return ERROR_SUCCESS;
	}

	return ERROR_GGAUTH_INVALID_PARAM;
};

// 전역 인증 기능 On/Off 를 위한 함수
UINT32 CheckCSAuth(bool bCheck) {
	
	if (bCheck == true) {
		gGGAuthCtx.m_bAuth = bCheck;
		return ERROR_SUCCESS;
	}

	if (bCheck == false) {
		gGGAuthCtx.m_bAuth = bCheck;
		return ERROR_SUCCESS;
	}

	return ERROR_GGAUTH_INVALID_PARAM;
};

int DecryptHackData(char* lpszUserKey, LPVOID lpData, DWORD dwLength) {

	// !@ não tem no game server, talvez tenho no .lib
	return ERROR_SUCCESS;
};

int ModuleInfo(char* dest, unsigned int length) {

	if (length < (gGGAuthCtx.m_dwProtocolNum * 100))
		return ERROR_GGAUTH_FAIL_MEM_ALLOC;

	memset(dest, 0, sizeof(length));

	if (gGGAuthCtx.m_EntryListProtocol == nullptr)
		return ERROR_SUCCESS;

	char *pState = nullptr;
	char strTmp[128];
	UINT32 total_len = 0, curr_len = 0;

	for (auto protocol = gGGAuthCtx.m_EntryListProtocol; protocol != nullptr; protocol = protocol->m_pNext) {

		if (protocol == gGGAuthCtx.m_EntryListProtocol)
			pState = (char*)gSTATE_ACTIVE;
		else if (protocol->m_bDisuse == TRUE || protocol->m_dwGGVer < gGGAuthCtx.m_EntryListProtocol->m_dwGGVer)
			pState = (char*)gSTATE_DISUSE;
		else
			pState = (char*)gSTATE_STANDBY;

#if defined(_WIN32)
		sprintf_s(strTmp, sizeof(strTmp), "PRTC[%d]C[0x%p]G[0x%p]E[0x%p]D[0x%p]U[0x%p] : state[%s] ==>",
				protocol->m_dwGGVer, protocol->PrtcCheckAuthAnswer, protocol->PrtcGetAuthQuery, protocol->PrtcEncryptQuery,
				protocol->PrtcDecryptAnswer, protocol->PrtcUpdateTimer, pState);
#elif defined(__linux__)
		sprintf(strTmp, "PRTC[%d]C[0x%p]G[0x%p]E[0x%p]D[0x%p]U[0x%p] : state[%s] ==>",
				protocol->m_dwGGVer, protocol->PrtcCheckAuthAnswer, protocol->PrtcGetAuthQuery, protocol->PrtcEncryptQuery,
				protocol->PrtcDecryptAnswer, protocol->PrtcUpdateTimer, pState);
#endif

		curr_len = (UINT32)strlen(strTmp);

		if (length < (total_len + curr_len))
			return ERROR_GGAUTH_FAIL_MEM_ALLOC;

#if defined(_WIN32)
		memcpy_s(dest + total_len, (gGGAuthCtx.m_dwProtocolNum * 100), strTmp, curr_len);
#elif defined(__linux__)
		memcpy(dest + total_len, strTmp, curr_len);
#endif

		total_len += curr_len;
	}

	return ERROR_SUCCESS;
};

CCSAuth2::CCSAuth2() {
	Init();
};

// Destructor
CCSAuth2::~CCSAuth2() {

	if (gGGAuthCtx.m_bInitState != false) {

		if ((m_dwUserFlag & 0x20) == 0x20) {

#if defined(_WIN32)
			EnterCriticalSection(&gGGAuthCtx.m_csUniqVal);
#elif defined(__linux__)
			pthread_mutex_lock(&gGGAuthCtx.m_csUniqVal);
#endif
			
			UpdateUniqValHashTable(false, m_dwUniqValue1, m_dwUniqValue2, m_dwServerKey);
			
#if defined(_WIN32)
			LeaveCriticalSection(&gGGAuthCtx.m_csUniqVal);
#elif defined(__linux__)
			pthread_mutex_unlock(&gGGAuthCtx.m_csUniqVal);
#endif

			if (m_bPrtcRef == TRUE && (m_dwUserFlag & 8) != 0 && (gGGAuthCtx.m_UseLog & 1) != 0) {
#if defined(_WIN32)
				sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] Close() - dwReferCount does not decrement(m_dwUserFlag& PRTC_USER_NEW_PRTC): %d", m_pProtocol->RefCount);
#elif defined(__linux__)
				sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] Close() - dwReferCount does not decrement(m_dwUserFlag& PRTC_USER_NEW_PRTC): %d", m_pProtocol->RefCount);
#endif
				NpLog(1, gGGAuthCtx.m_MsgBuff);
			}

			m_bPrtcRef = FALSE;
			m_dwUserFlag &= 0xFFFFFFDF;

		}else if ((gGGAuthCtx.m_UseLog & 1) != 0) {
#if defined(_WIN32)
			sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] Close() called without Init()");
#elif defined(__linux__)
			sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] Close() called without Init()");
#endif
			NpLog(1, gGGAuthCtx.m_MsgBuff);
		}
	}
};

void CCSAuth2::Init() {

	if (gGGAuthCtx.m_bInitState != false) {

		m_pProtocol = gGGAuthCtx.m_EntryListProtocol;

		memset(&m_AuthQuery, 0, sizeof(m_AuthQuery));
		memset(&m_AuthAnswer, 0, sizeof(m_AuthAnswer));
		memset(&m_AuthQueryTmp, 0, sizeof(m_AuthQueryTmp));

		m_bAuth = true;
		m_bPrtcRef = FALSE;
		
		memset(&m_GGVer, 0, sizeof(m_GGVer));

		if ((m_dwUserFlag & 0x20) != 0) {
#if defined(_WIN32)
			EnterCriticalSection(&gGGAuthCtx.m_csUniqVal);
#elif defined(__linux__)
			pthread_mutex_lock(&gGGAuthCtx.m_csUniqVal);
#endif
			UpdateUniqValHashTable(false, m_dwUniqValue1, m_dwUniqValue2, m_dwServerKey);
#if defined(_WIN32)
			LeaveCriticalSection(&gGGAuthCtx.m_csUniqVal);
#elif defined(__linux__)
			pthread_mutex_unlock(&gGGAuthCtx.m_csUniqVal);
#endif
		}

		m_dwUserFlag = 0x20;
		m_bAllowOldVersion = false;
		m_dwServerKey = GetRandomDword();
		m_nSequenceNum = -1;
		m_dwLastValue4 = 0;
		m_byLastLoop1 = '\0';
		m_byLastLoop2 = '\0';
		m_dwUniqValue1 = 0;
		m_dwUniqValue2 = 0;

		memset(m_dwLoop1AuthArray, 0, sizeof(m_dwLoop1AuthArray));
		memset(m_dwLoop2AuthArray, 0, sizeof(m_dwLoop2AuthArray));

		m_dwLoop2AuthArray[0] = m_dwServerKey;
		m_dwLoop1AuthArray[0] = m_dwUniqValue1;
		
		m_pNext = nullptr;
	}
}

UINT32 CCSAuth2::GetAuthQuery() {

	if ((m_dwUserFlag & 0x20) != 0x20) {

		if ((gGGAuthCtx.m_UseLog & 2) == 0)
			return ERROR_GGAUTH_FAIL_BEFORE_INIT;

#if defined(_WIN32)
		sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib][err:%d] GetAuthQuery() called before Init()", ERROR_GGAUTH_FAIL_BEFORE_INIT);
#elif defined(__linux__)
		sprintf(gGGAuthCtx.m_MsgBuff, "[S lib][err:%d] GetAuthQuery() called before Init()", ERROR_GGAUTH_FAIL_BEFORE_INIT);
#endif
		NpLog(2, gGGAuthCtx.m_MsgBuff);

		return ERROR_GGAUTH_FAIL_BEFORE_INIT;
	}

	if (m_pProtocol == nullptr) {

		if ((gGGAuthCtx.m_UseLog & 2) == 0)
			return ERROR_GGAUTH_FAIL_BEFORE_INIT;

#if defined(_WIN32)
		sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib][err:%d] GetAuthQuery() - Protocol is not loaded", ERROR_GGAUTH_FAIL_BEFORE_INIT);
#elif defined(__linux__)
		sprintf(gGGAuthCtx.m_MsgBuff, "[S lib][err:%d] GetAuthQuery() - Protocol is not loaded", ERROR_GGAUTH_FAIL_BEFORE_INIT);
#endif
		NpLog(2, gGGAuthCtx.m_MsgBuff);

		return ERROR_GGAUTH_FAIL_BEFORE_INIT;
	}

	if (m_bAuth == false || gGGAuthCtx.m_bAuth == false)
		return ERROR_SUCCESS;

	if (m_bPrtcRef == FALSE) {

		m_pProtocol->RefCount++;

		if ((gGGAuthCtx.m_EntryListProtocol->m_flag & 2) == 0 && gGGAuthCtx.m_dwNumActiveSession < gGGAuthCtx.m_EntryListProtocol->RefCount)
			gGGAuthCtx.m_EntryListProtocol->m_flag |= 2;

		m_bPrtcRef = TRUE;
	}

	if ((m_dwUserFlag & 1) != 0) {

		if (!IsValidGGVer(&this->m_GGVer)) {

			if ((gGGAuthCtx.m_UseLog & 2) != 0) {
#if defined(_WIN32)
				sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib][err:%d] PrtcGetAuthQuery() - Invalid GGVer: %d", ERROR_GGAUTH_INVALID_GAMEGUARD_VER, m_GGVer.dwGGVer);
#elif defined(__linux__)
				sprintf(gGGAuthCtx.m_MsgBuff, "[S lib][err:%d] PrtcGetAuthQuery() - Invalid GGVer: %d", ERROR_GGAUTH_INVALID_GAMEGUARD_VER, m_GGVer.dwGGVer);
#endif
				NpLog(1, gGGAuthCtx.m_MsgBuff);
			}

			return ERROR_GGAUTH_INVALID_GAMEGUARD_VER;
		}
	}

	m_nSequenceNum++;

	UINT32 error = m_pProtocol->PrtcGetAuthQuery(&m_dwUserFlag, &m_GGVer, this, m_nSequenceNum, m_dwServerKey);

	if (error == ERROR_SUCCESS) {

		memcpy(&m_AuthQueryTmp, &m_AuthQuery, sizeof(_GG_AUTH_DATA));

		error = m_pProtocol->PrtcEncryptQuery(&m_AuthQuery);

		GGEncryptPacket1(&m_AuthQuery);

		if (error == ERROR_SUCCESS)
			return error;

		if (error == ERROR_GGAUTH_NO_REPLY) {

			if ((gGGAuthCtx.m_UseLog & 2) == 0)
				return ERROR_GGAUTH_NO_REPLY;

#if defined(_WIN32)
			sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib][err:%d] PrtcGetAuthQuery() called without ProcCheckAuthAnswer()", ERROR_GGAUTH_NO_REPLY);
#elif defined(__linux__)
			sprintf(gGGAuthCtx.m_MsgBuff, "[S lib][err:%d] PrtcGetAuthQuery() called without ProcCheckAuthAnswer()", ERROR_GGAUTH_NO_REPLY);
#endif
			NpLog(1, gGGAuthCtx.m_MsgBuff);
		
		}else {
#if defined(_WIN32)
			sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib][err:%d] PrtcGetAuthQuery() - ERROR_UNKNOWN", error);
#elif defined(__linux__)
			sprintf(gGGAuthCtx.m_MsgBuff, "[S lib][err:%d] PrtcGetAuthQuery() - ERROR_UNKNOWN", error);
#endif
			NpLog(1, gGGAuthCtx.m_MsgBuff);
		}

	}else {

		if (error == ERROR_GGAUTH_NO_REPLY) {

			if ((gGGAuthCtx.m_UseLog & 2) == 0)
				return ERROR_GGAUTH_NO_REPLY;

#if defined(_WIN32)
			sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib][err:%d] PrtcGetAuthQuery() called without ProcCheckAuthAnswer()", ERROR_GGAUTH_NO_REPLY);
#elif defined(__linux__)
			sprintf(gGGAuthCtx.m_MsgBuff, "[S lib][err:%d] PrtcGetAuthQuery() called without ProcCheckAuthAnswer()", ERROR_GGAUTH_NO_REPLY);
#endif
			NpLog(1, gGGAuthCtx.m_MsgBuff);
		
		}else {
#if defined(_WIN32)
			sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib][err:%d] PrtcGetAuthQuery() - ERROR_UNKNOWN", error);
#elif defined(__linux__)
			sprintf(gGGAuthCtx.m_MsgBuff, "[S lib][err:%d] PrtcGetAuthQuery() - ERROR_UNKNOWN", error);
#endif
			NpLog(1, gGGAuthCtx.m_MsgBuff);
		}
	}

	return error;
}

UINT32 CCSAuth2::CheckAuthAnswer() {

	if ((m_dwUserFlag & 0x20) != 0x20) {

		if ((gGGAuthCtx.m_UseLog & 2) == 0)
			return ERROR_GGAUTH_FAIL_BEFORE_INIT;

#if defined(_WIN32)
		sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib][err:%d] CheckAuthAnswer() called before Init()", ERROR_GGAUTH_FAIL_BEFORE_INIT);
#elif defined(__linux__)
		sprintf(gGGAuthCtx.m_MsgBuff, "[S lib][err:%d] CheckAuthAnswer() called before Init()", ERROR_GGAUTH_FAIL_BEFORE_INIT);
#endif
		NpLog(2, gGGAuthCtx.m_MsgBuff);

		return ERROR_GGAUTH_FAIL_BEFORE_INIT;
	}

	if ((m_dwUserFlag & 2) == 0)
		return ERROR_SUCCESS;

	m_dwUserFlag &= 0xFFFFFFFD;

	if (m_pProtocol == nullptr) {

		if ((gGGAuthCtx.m_UseLog & 2) == 0)
			return ERROR_GGAUTH_FAIL_BEFORE_INIT;

#if defined(_WIN32)
		sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib][err:%d] CheckAuthAnswer() - Protocol is not loaded", ERROR_GGAUTH_FAIL_BEFORE_INIT);
#elif defined(__linux__)
		sprintf(gGGAuthCtx.m_MsgBuff, "[S lib][err:%d] CheckAuthAnswer() - Protocol is not loaded", ERROR_GGAUTH_FAIL_BEFORE_INIT);
#endif
		NpLog(2, gGGAuthCtx.m_MsgBuff);

		return ERROR_GGAUTH_FAIL_BEFORE_INIT;
	}

	if (m_bAuth == false || gGGAuthCtx.m_bAuth == false)
		return ERROR_SUCCESS;

	_GG_AUTH_DATA query_tmp;

	memcpy(&query_tmp, &m_AuthAnswer, sizeof(_GG_AUTH_DATA));

	GGDecryptPacket2(&query_tmp);

	m_pProtocol->PrtcDecryptAnswer(&query_tmp);

	UINT32 dwFlagReturn = 0;

	UINT32 error = m_pProtocol->PrtcCheckValidAnswer(&query_tmp, &dwFlagReturn);

	if (error != ERROR_SUCCESS) {

		if (dwFlagReturn != 0) {

			if ((gGGAuthCtx.m_UseLog & 2) == 0)
				return error;

#if defined(_WIN32)
			sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib][err:%d] PrtcCheckAuthAnswer() - client hack detected (code: %d)", error, dwFlagReturn);
#elif defined(__linux__)
			sprintf(gGGAuthCtx.m_MsgBuff, "[S lib][err:%d] PrtcCheckAuthAnswer() - client hack detected (code: %d)", error, dwFlagReturn);
#endif
			NpLog(1, gGGAuthCtx.m_MsgBuff);

			return error;
		}

		if ((gGGAuthCtx.m_UseLog & 2) == 0)
			return error;

#if defined(_WIN32)
		sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib][err:%d] PrtcCheckAuthAnswer() - Invalid packet", error);
#elif defined(__linux__)
		sprintf(gGGAuthCtx.m_MsgBuff, "[S lib][err:%d] PrtcCheckAuthAnswer() - Invalid packet", error);
#endif
		NpLog(1, gGGAuthCtx.m_MsgBuff);

		return error;
	}

	UINT32 first_auth = m_pProtocol->PrtcGetInformation(0, &query_tmp),
		   protocol_ver = m_pProtocol->PrtcGetInformation(1, &query_tmp),
		   gg_ver = m_pProtocol->PrtcGetInformation(4, &query_tmp);

	if ((gGGAuthCtx.m_UseLog & 1) != 0) {
#if defined(_WIN32)
		sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] CheckAuthAnswer() %08x %08x %08x %08x - %d %d", 
				query_tmp.dwIndex, query_tmp.dwValue1, query_tmp.dwValue2, query_tmp.dwValue3, 
				first_auth, protocol_ver);
#elif defined(__linux__)
		sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] CheckAuthAnswer() %08x %08x %08x %08x - %d %d", 
				query_tmp.dwIndex, query_tmp.dwValue1, query_tmp.dwValue2, query_tmp.dwValue3, 
				first_auth, protocol_ver);
#endif
		NpLog(1, gGGAuthCtx.m_MsgBuff);
	}

	if (m_pProtocol->m_dwGGVer < protocol_ver) {

		if (first_auth != 0) {

			if ((gGGAuthCtx.m_UseLog & 1) == 0)
				return ERROR_GGAUTH_INVALID_PROTOCOL_VERSION;

#if defined(_WIN32)
			sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib][err:%d] CheckAuthAnswer() : PROTOCOL CHECKER %d != %d, not first auth", ERROR_GGAUTH_INVALID_PROTOCOL_VERSION, 
					protocol_ver, m_pProtocol->m_dwGGVer);
#elif defined(__linux__)
			sprintf(gGGAuthCtx.m_MsgBuff, "[S lib][err:%d] CheckAuthAnswer() : PROTOCOL CHECKER %d != %d, not first auth", ERROR_GGAUTH_INVALID_PROTOCOL_VERSION, 
					protocol_ver, m_pProtocol->m_dwGGVer);
#endif
			NpLog(1, gGGAuthCtx.m_MsgBuff);

			return ERROR_GGAUTH_INVALID_PROTOCOL_VERSION;
		}

		m_dwUserFlag &= 0xFFFFFFF7;

		for (auto protocol = gGGAuthCtx.m_EntryListProtocol; protocol != nullptr; protocol = protocol->m_pNext) {

			if (protocol->m_bDisuse == FALSE && protocol->m_dwGGVer == protocol_ver) {

				m_pProtocol->RefCount--;
				m_pProtocol = protocol;

				m_dwUserFlag |= 8;

				if ((gGGAuthCtx.m_UseLog & 1) != 0) {
#if defined(_WIN32)
					sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] CheckAuthAnswer() : PROTOCOL CHECKER %d == %d, founded at list", protocol_ver, protocol->m_dwGGVer);
#elif defined(__linux__)
					sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] CheckAuthAnswer() : PROTOCOL CHECKER %d == %d, founded at list", protocol_ver, protocol->m_dwGGVer);
#endif
					NpLog(1, gGGAuthCtx.m_MsgBuff);
				}

				break;
			}

			if ((gGGAuthCtx.m_UseLog & 1) != 0) {
#if defined(_WIN32)
				sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] CheckAuthAnswer() : PROTOCOL CHECKER %d != %d, try next protocol: %d", 
						protocol_ver, protocol->m_dwGGVer, (protocol->m_pNext != nullptr ? protocol->m_pNext->m_dwGGVer : 0));
#elif defined(__linux__)
				sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] CheckAuthAnswer() : PROTOCOL CHECKER %d != %d, try next protocol: %d", 
						protocol_ver, protocol->m_dwGGVer, (protocol->m_pNext != nullptr ? protocol->m_pNext->m_dwGGVer : 0));
#endif
				NpLog(1, gGGAuthCtx.m_MsgBuff);
			}
		}

		if ((m_dwUserFlag & 8) != 8) {

			if ((gGGAuthCtx.m_UseLog & 1) == 0)
				return ERROR_GGAUTH_INVALID_PROTOCOL_VERSION;

#if defined(_WIN32)
			sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib][err:%d] CheckAuthAnswer() : PROTOCOL CHECKER %d != %d, cannot find at list", 
					ERROR_GGAUTH_INVALID_PROTOCOL_VERSION, protocol_ver, m_pProtocol->m_dwGGVer);
#elif defined(__linux__)
			sprintf(gGGAuthCtx.m_MsgBuff, "[S lib][err:%d] CheckAuthAnswer() : PROTOCOL CHECKER %d != %d, cannot find at list", 
					ERROR_GGAUTH_INVALID_PROTOCOL_VERSION, protocol_ver, m_pProtocol->m_dwGGVer);
#endif
			NpLog(1, gGGAuthCtx.m_MsgBuff);

			return ERROR_GGAUTH_INVALID_PROTOCOL_VERSION;
		}

		if (!IsValidGGVer2(gg_ver)) {

			if ((gGGAuthCtx.m_UseLog & 2) == 0)
				return ERROR_GGAUTH_INVALID_GAMEGUARD_VER;

#if defined(_WIN32)
			sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib][err:%d] CheckAuthAnswer() - Answered ggver is not valid: %d", ERROR_GGAUTH_INVALID_GAMEGUARD_VER, gg_ver);
#elif defined(__linux__)
			sprintf(gGGAuthCtx.m_MsgBuff, "[S lib][err:%d] CheckAuthAnswer() - Answered ggver is not valid: %d", ERROR_GGAUTH_INVALID_GAMEGUARD_VER, gg_ver);
#endif
			NpLog(1, gGGAuthCtx.m_MsgBuff);

			return ERROR_GGAUTH_INVALID_GAMEGUARD_VER;
		}

		if (m_bAllowOldVersion == false && gg_ver < gGGAuthCtx.m_dwCurrentVersion) {

			if ((gGGAuthCtx.m_UseLog & 2) == 0)
				return ERROR_GGAUTH_INVALID_GAMEGUARD_VER;

#if defined(_WIN32)
			sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib][err:%d] CheckAuthAnswer() - low ggver %d -> %d", 
					ERROR_GGAUTH_INVALID_GAMEGUARD_VER, gGGAuthCtx.m_dwCurrentVersion, gg_ver);
#elif defined(__linux__)
			sprintf(gGGAuthCtx.m_MsgBuff, "[S lib][err:%d] CheckAuthAnswer() - low ggver %d -> %d", 
					ERROR_GGAUTH_INVALID_GAMEGUARD_VER, gGGAuthCtx.m_dwCurrentVersion, gg_ver);
#endif
			NpLog(1, gGGAuthCtx.m_MsgBuff);

			return ERROR_GGAUTH_INVALID_GAMEGUARD_VER;
		}

		if (gg_ver > gGGAuthCtx.m_dwVersionLimitMax) {

			if ((gGGAuthCtx.m_UseLog & 2) == 0)
				return ERROR_GGAUTH_INVALID_GAMEGUARD_VER;

#if defined(_WIN32)
			sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib][err:%d] CheckAuthAnswer() - higher than limited ggver %d > %d", 
					ERROR_GGAUTH_INVALID_GAMEGUARD_VER, gg_ver, gGGAuthCtx.m_dwVersionLimitMax);
#elif defined(__linux__)
			sprintf(gGGAuthCtx.m_MsgBuff, "[S lib][err:%d] CheckAuthAnswer() - higher than limited ggver %d > %d", 
					ERROR_GGAUTH_INVALID_GAMEGUARD_VER, gg_ver, gGGAuthCtx.m_dwVersionLimitMax);
#endif
			NpLog(1, gGGAuthCtx.m_MsgBuff);
			
			return ERROR_GGAUTH_INVALID_GAMEGUARD_VER;
		}

		m_GGVer.dwGGVer = gg_ver;
		m_GGVer.wYear = gg_ver / 1000000;
		m_GGVer.wMonth = (gg_ver / 10000) % 100;
		m_GGVer.wDay = (gg_ver / 100) % 100;
		m_GGVer.wNum = gg_ver % 100;
	
	}else {

		if (protocol_ver < m_pProtocol->m_dwGGVer) {

			if ((gGGAuthCtx.m_UseLog & 1) == 0)
				return ERROR_GGAUTH_INVALID_PROTOCOL_VERSION;

#if defined(_WIN32)
			sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib][err:%d] CheckAuthAnswer() : PROTOCOL CHECKER %d < %d", 
					ERROR_GGAUTH_INVALID_PROTOCOL_VERSION, protocol_ver, m_pProtocol->m_dwGGVer);
#elif defined(__linux__)
			sprintf(gGGAuthCtx.m_MsgBuff, "[S lib][err:%d] CheckAuthAnswer() : PROTOCOL CHECKER %d < %d", 
					ERROR_GGAUTH_INVALID_PROTOCOL_VERSION, protocol_ver, m_pProtocol->m_dwGGVer);
#endif
			NpLog(1, gGGAuthCtx.m_MsgBuff);

			return ERROR_GGAUTH_INVALID_PROTOCOL_VERSION;
		}

		if (first_auth == 0) {

			if (!IsValidGGVer2(gg_ver)) {

				if ((gGGAuthCtx.m_UseLog & 2) == 0)
					return ERROR_GGAUTH_INVALID_GAMEGUARD_VER;

#if defined(_WIN32)
				sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib][err:%d] CheckAuthAnswer() - Answered ggver is not valid: %d", ERROR_GGAUTH_INVALID_GAMEGUARD_VER, gg_ver);
#elif defined(__linux__)
				sprintf(gGGAuthCtx.m_MsgBuff, "[S lib][err:%d] CheckAuthAnswer() - Answered ggver is not valid: %d", ERROR_GGAUTH_INVALID_GAMEGUARD_VER, gg_ver);
#endif
				NpLog(1, gGGAuthCtx.m_MsgBuff);

				return ERROR_GGAUTH_INVALID_GAMEGUARD_VER;
			}

			if (m_bAllowOldVersion == false && gg_ver < gGGAuthCtx.m_dwCurrentVersion) {

				if ((gGGAuthCtx.m_UseLog & 2) == 0)
					return ERROR_GGAUTH_INVALID_GAMEGUARD_VER;

#if defined(_WIN32)
				sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib][err:%d] CheckAuthAnswer() - low ggver %d -> %d",
						ERROR_GGAUTH_INVALID_GAMEGUARD_VER, gGGAuthCtx.m_dwCurrentVersion, gg_ver);
#elif defined(__linux__)
				sprintf(gGGAuthCtx.m_MsgBuff, "[S lib][err:%d] CheckAuthAnswer() - low ggver %d -> %d",
						ERROR_GGAUTH_INVALID_GAMEGUARD_VER, gGGAuthCtx.m_dwCurrentVersion, gg_ver);
#endif
				NpLog(1, gGGAuthCtx.m_MsgBuff);

				return ERROR_GGAUTH_INVALID_GAMEGUARD_VER;
			}

			if (gg_ver > gGGAuthCtx.m_dwVersionLimitMax) {

				if ((gGGAuthCtx.m_UseLog & 2) == 0)
					return ERROR_GGAUTH_INVALID_GAMEGUARD_VER;

#if defined(_WIN32)
				sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib][err:%d] CheckAuthAnswer() - higher than limited ggver %d > %d",
						ERROR_GGAUTH_INVALID_GAMEGUARD_VER, gg_ver, gGGAuthCtx.m_dwVersionLimitMax);
#elif defined(__linux__)
				sprintf(gGGAuthCtx.m_MsgBuff, "[S lib][err:%d] CheckAuthAnswer() - higher than limited ggver %d > %d",
						ERROR_GGAUTH_INVALID_GAMEGUARD_VER, gg_ver, gGGAuthCtx.m_dwVersionLimitMax);
#endif
				NpLog(1, gGGAuthCtx.m_MsgBuff);

				return ERROR_GGAUTH_INVALID_GAMEGUARD_VER;
			}

			m_GGVer.dwGGVer = gg_ver;
			m_GGVer.wYear = gg_ver / 1000000;
			m_GGVer.wMonth = (gg_ver / 10000) % 100;
			m_GGVer.wDay = (gg_ver / 100) % 100;
			m_GGVer.wNum = gg_ver % 100;
		}
	}

	if (!IsValidGGVer(&m_GGVer)) {

		if ((gGGAuthCtx.m_UseLog & 2) == 0)
			return ERROR_GGAUTH_INVALID_GAMEGUARD_VER;

#if defined(_WIN32)
		sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib][err:%d] CheckAuthAnswer() - Answered ggver is not valid2 : %d", ERROR_GGAUTH_INVALID_GAMEGUARD_VER, gg_ver);
#elif defined(__linux__)
		sprintf(gGGAuthCtx.m_MsgBuff, "[S lib][err:%d] CheckAuthAnswer() - Answered ggver is not valid2 : %d", ERROR_GGAUTH_INVALID_GAMEGUARD_VER, gg_ver);
#endif
		NpLog(1, gGGAuthCtx.m_MsgBuff);

		return ERROR_GGAUTH_INVALID_GAMEGUARD_VER;
	}

	error = m_pProtocol->PrtcCheckAuthAnswer(&m_dwUserFlag, &m_GGVer, &m_AuthQueryTmp, &query_tmp, m_nSequenceNum, (UINT32*)&m_dwLastValue4);

	m_dwLastValue4 = query_tmp.dwValue3;

	if (error != ERROR_SUCCESS) {

		if ((gGGAuthCtx.m_UseLog & 2) == 0)
			return error;

#if defined(_WIN32)
		sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib][err:%d] PrtcCheckAuthAnswer() - Auth fail", error);
#elif defined(__linux__)
		sprintf(gGGAuthCtx.m_MsgBuff, "[S lib][err:%d] PrtcCheckAuthAnswer() - Auth fail", error);
#endif
		NpLog(1, gGGAuthCtx.m_MsgBuff);

		return error;
	}

	if (first_auth == 1) {
		m_dwUniqValue1 = gg_ver;
		m_dwLoop1AuthArray[0] = gg_ver;
	}else if (first_auth == 2) {

		m_dwUniqValue2 = gg_ver;

		bool ret_upt;
		
#if defined(_WIN32)
		EnterCriticalSection(&gGGAuthCtx.m_csUniqVal);
#elif defined(__linux__)
		pthread_mutex_lock(&gGGAuthCtx.m_csUniqVal);
#endif
		
		ret_upt = UpdateUniqValHashTable(true, m_dwUniqValue1, m_dwUniqValue2, m_dwServerKey);

#if defined(_WIN32)
		LeaveCriticalSection(&gGGAuthCtx.m_csUniqVal);
#elif defined(__linux__)
		pthread_mutex_unlock(&gGGAuthCtx.m_csUniqVal);
#endif

		if (!ret_upt) {

#if defined(_WIN32)
			sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib][err:%d] Same client detected.", ERROR_GGAUTH_SAME_CLIENT_DETECTED);
#elif defined(__linux__)
			sprintf(gGGAuthCtx.m_MsgBuff, "[S lib][err:%d] Same client detected.", ERROR_GGAUTH_SAME_CLIENT_DETECTED);
#endif
			NpLog(1, gGGAuthCtx.m_MsgBuff);

			return ERROR_GGAUTH_SAME_CLIENT_DETECTED;
		}
	}

	char ant_byLastLoop1 = m_byLastLoop1,
		 ant_byLastLoop2 = m_byLastLoop2;

	error = m_pProtocol->PrtcLoopAuth((char*)&m_byLastLoop1, m_dwLoop1AuthArray, (char*)&m_byLastLoop2, m_dwLoop2AuthArray, &query_tmp, m_nSequenceNum);

	if (error == ERROR_SUCCESS) {

		if (m_bAllowOldVersion == true)
			return ERROR_SUCCESS;

		if (first_auth == 0) {

			if (gGGAuthCtx.m_GGVerUpdateCtx.m_bCountStart == true) {
				
				m_dwUserFlag |= 4;

				return ERROR_SUCCESS;
			}

			if (gGGAuthCtx.m_dwCurrentVersion < m_GGVer.dwGGVer) {

				if ((gGGAuthCtx.m_UseLog & 1) != 0) {
#if defined(_WIN32)
					sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] CheckAuthAnswer() : New GGVer Receive [%d]", m_GGVer.dwGGVer);
#elif defined(__linux__)
					sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] CheckAuthAnswer() : New GGVer Receive [%d]", m_GGVer.dwGGVer);
#endif
					NpLog(1, gGGAuthCtx.m_MsgBuff);
				}

				m_dwUserFlag |= 4;

				return ERROR_SUCCESS;
			}
		
		}else {

			if ((m_dwUserFlag & 4) != 0 || (m_dwUserFlag & 8) != 0) {


				if ((m_dwUserFlag & 4) != 0) {

					m_dwUserFlag &= 0xFFFFFFFB;

#if defined(_WIN32)
					EnterCriticalSection(&gGGAuthCtx.m_csGGVer);
#elif defined(__linux__)
					pthread_mutex_lock(&gGGAuthCtx.m_csGGVer);
#endif
					UpdateGGVersion(m_GGVer.dwGGVer);
#if defined(_WIN32)
					LeaveCriticalSection(&gGGAuthCtx.m_csGGVer);
#elif defined(__linux__)
					pthread_mutex_unlock(&gGGAuthCtx.m_csGGVer);
#endif
				}

				if ((m_dwUserFlag & 8) != 0) {

					m_dwUserFlag &= 0xFFFFFFF7;

#if defined(_WIN32)
					EnterCriticalSection(&gGGAuthCtx.m_csProtocol);
#elif defined(__linux__)
					pthread_mutex_lock(&gGGAuthCtx.m_csProtocol);
#endif
					UpdateProtocol(m_pProtocol);
#if defined(_WIN32)
					LeaveCriticalSection(&gGGAuthCtx.m_csProtocol);
#elif defined(__linux__)
					pthread_mutex_unlock(&gGGAuthCtx.m_csProtocol);
#endif
				}
			}
		}

		return ERROR_SUCCESS;
	}

	if (error == ERROR_GGAUTH_CLIENT_STOPPED && (gGGAuthCtx.m_UseLog & 2) != 0)
#if defined(_WIN32)
		sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib][err:%d] Client threads were stopped", error);
#elif defined(__linux__)
		sprintf(gGGAuthCtx.m_MsgBuff, "[S lib][err:%d] Client threads were stopped", error);
#endif
	else if (error == ERROR_GGAUTH_CLIENT_AUTH_ERROR && (gGGAuthCtx.m_UseLog & 2) != 0)
#if defined(_WIN32)
		sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib][err:%d] Client threads auth error", error);
#elif defined(__linux__)
		sprintf(gGGAuthCtx.m_MsgBuff, "[S lib][err:%d] Client threads auth error", error);
#endif
	else if ((gGGAuthCtx.m_UseLog & 2) != 0)
#if defined(_WIN32)
		sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib][err:%d] Client threads auth occured unknown error", error);
#elif defined(__linux__)
		sprintf(gGGAuthCtx.m_MsgBuff, "[S lib][err:%d] Client threads auth occured unknown error", error);
#endif

	NpLog(1, gGGAuthCtx.m_MsgBuff);

	if ((gGGAuthCtx.m_UseLog & 2) != 0) {

#if defined(_WIN32)
		sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] %d -> %d, %d -> %d, uv: %x, sk: %x, auth: %x (%d)",
				(unsigned char)ant_byLastLoop1, m_pProtocol->PrtcGetInformation(5, &query_tmp), 
				(unsigned char)ant_byLastLoop2, m_pProtocol->PrtcGetInformation(6, &query_tmp),
				m_dwUniqValue1, m_dwServerKey, m_pProtocol->PrtcGetInformation(7, &query_tmp), m_nSequenceNum);
#elif defined(__linux__)
		sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] %d -> %d, %d -> %d, uv: %x, sk: %x, auth: %x (%d)",
				(unsigned char)ant_byLastLoop1, m_pProtocol->PrtcGetInformation(5, &query_tmp), 
				(unsigned char)ant_byLastLoop2, m_pProtocol->PrtcGetInformation(6, &query_tmp),
				m_dwUniqValue1, m_dwServerKey, m_pProtocol->PrtcGetInformation(7, &query_tmp), m_nSequenceNum);
#endif

		NpLog(1, gGGAuthCtx.m_MsgBuff);
	}

	return error;
}

UINT32 CCSAuth2::CheckUserCSAuth(bool bCheck) {
	
	if (bCheck == true) {
		
		m_bAuth = true;

		return ERROR_SUCCESS;
	}

	if (bCheck == false) {

		m_bAuth = false;

		return ERROR_SUCCESS;
	}

	return ERROR_GGAUTH_INVALID_PARAM;
}

UINT32 CCSAuth2::GetCSAuthState(PGG_CSAUTH_STATE _CSAuthState) {

	if (_CSAuthState == nullptr)
		return ERROR_GGAUTH_INVALID_PARAM;
	
	memset(_CSAuthState, 0, sizeof(GG_CSAUTH_STATE));

	_CSAuthState->m_PrtcVersion = m_pProtocol->m_dwGGVer;
	_CSAuthState->m_GGVersion = m_GGVer.dwGGVer;
	_CSAuthState->m_UserFlag = m_dwUserFlag;

	return ERROR_SUCCESS;
}

UINT32 CCSAuth2::SetCSAuthState(PGG_CSAUTH_STATE _CSAuthState) {
	
	if (_CSAuthState == nullptr) {

		if ((gGGAuthCtx.m_UseLog & 2) == 0)
			return ERROR_GGAUTH_SETSTATE_ERROR;

#if defined(_WIN32)
		sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] SetCSAuthState() : '_CSAuthState' is nullptr!!!");
#elif defined(__linux__)
		sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] SetCSAuthState() : '_CSAuthState' is nullptr!!!");
#endif
		NpLog(2, gGGAuthCtx.m_MsgBuff);

		return ERROR_GGAUTH_SETSTATE_ERROR;
	}

	if (_CSAuthState->m_GGVersion != m_pProtocol->m_dwGGVer) {

		if (gGGAuthCtx.m_EntryListProtocol == nullptr) {

			if ((gGGAuthCtx.m_UseLog & 2) == 0)
				return ERROR_GGAUTH_SETSTATE_ERROR;

#if defined(_WIN32)
			sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] SetCSAuthState() : 'm_PrtcTemp' is nullptr!!!");
#elif defined(__linux__)
			sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] SetCSAuthState() : 'm_PrtcTemp' is nullptr!!!");
#endif
			NpLog(2, gGGAuthCtx.m_MsgBuff);

			return ERROR_GGAUTH_SETSTATE_ERROR;
		}

		for (auto protocol = gGGAuthCtx.m_EntryListProtocol; protocol != nullptr; protocol = protocol->m_pNext) {

			if (protocol->m_dwGGVer == _CSAuthState->m_PrtcVersion) {
				m_pProtocol = protocol;
				break;
			}
		}
	}

	m_GGVer.dwGGVer = _CSAuthState->m_GGVersion;
	m_GGVer.wYear = (_CSAuthState->m_GGVersion / 1000000);
	m_GGVer.wMonth = (_CSAuthState->m_GGVersion / 10000) % 100;
	m_GGVer.wDay = (_CSAuthState->m_GGVersion / 100) % 100;
	m_GGVer.wNum = _CSAuthState->m_GGVersion % 100;

	m_dwUserFlag = _CSAuthState->m_UserFlag;

	return ERROR_SUCCESS;
}

UINT32 CCSAuth2::SetSecretOrder() {

	// !@ Esse função não tem no game server, talvez tenho no .lib
	return ERROR_SUCCESS;
}

void CCSAuth2::Close() {

	if (gGGAuthCtx.m_bInitState == true) {

		if ((m_dwUserFlag & 0x20) == 0x20) {

#if defined(_WIN32)
			EnterCriticalSection(&gGGAuthCtx.m_csUniqVal);
#elif defined(__linux__)
			pthread_mutex_lock(&gGGAuthCtx.m_csUniqVal);
#endif
			UpdateUniqValHashTable(false, m_dwUniqValue1, m_dwUniqValue2, m_dwServerKey);
#if defined(_WIN32)
			LeaveCriticalSection(&gGGAuthCtx.m_csUniqVal);
#elif defined(__linux__)
			pthread_mutex_unlock(&gGGAuthCtx.m_csUniqVal);
#endif

			if (m_bPrtcRef == TRUE && (m_dwUserFlag & 8) != 0 && (gGGAuthCtx.m_UseLog & 1) != 0) {
#if defined(_WIN32)
				sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] Close() - dwReferCount does not decrement(m_dwUserFlag & PRTC_USER_NEW_PRTC): %d", m_pProtocol->RefCount);
#elif defined(__linux__)
				sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] Close() - dwReferCount does not decrement(m_dwUserFlag & PRTC_USER_NEW_PRTC): %d", m_pProtocol->RefCount);
#endif
				NpLog(1, gGGAuthCtx.m_MsgBuff);
			}

			m_bPrtcRef = FALSE;
			m_dwUserFlag &= 0xFFFFFFDF;

		}else if ((gGGAuthCtx.m_UseLog & 1) != 0) {
			
#if defined(_WIN32)
			sprintf_s(gGGAuthCtx.m_MsgBuff, sizeof(gGGAuthCtx.m_MsgBuff), "[S lib] Close() called without Init()");
#elif defined(__linux__)
			sprintf(gGGAuthCtx.m_MsgBuff, "[S lib] Close() called without Init()");
#endif
			NpLog(1, gGGAuthCtx.m_MsgBuff);
		}
	}
}

int CCSAuth2::Info(char * dest, int length) {

	if (dest == nullptr)
		return ERROR_GGAUTH_INVALID_PARAM;

	if (length < 100)
		return NPGG_INFO_ERROR_NOTENOUGHFMEMORY;

	memset(dest, 0, length);

	char *pState = nullptr;

	if (m_pProtocol == gGGAuthCtx.m_EntryListProtocol)
		pState = (char*)gSTATE_ACTIVE;
	else if (m_pProtocol->m_bDisuse || m_pProtocol->m_dwGGVer < gGGAuthCtx.m_EntryListProtocol->m_dwGGVer)
		pState = (char*)gSTATE_DISUSE;
	else
		pState = (char*)gSTATE_STANDBY;

#if defined(_WIN32)
	sprintf_s(dest, length, "PRTC[0x%d]C[0x%p]G[0x%p]E[0x%p]D[0x%p]U[0x%p] : state[%s] ==>", 
			m_pProtocol->m_dwGGVer, m_pProtocol->PrtcCheckAuthAnswer, m_pProtocol->PrtcGetAuthQuery,
			m_pProtocol->PrtcEncryptQuery, m_pProtocol->PrtcDecryptAnswer, m_pProtocol->PrtcUpdateTimer, pState);
#elif defined(__linux__)
	sprintf(dest, "PRTC[0x%d]C[0x%p]G[0x%p]E[0x%p]D[0x%p]U[0x%p] : state[%s] ==>", 
			m_pProtocol->m_dwGGVer, m_pProtocol->PrtcCheckAuthAnswer, m_pProtocol->PrtcGetAuthQuery,
			m_pProtocol->PrtcEncryptQuery, m_pProtocol->PrtcDecryptAnswer, m_pProtocol->PrtcUpdateTimer, pState);
#endif

	return ERROR_SUCCESS;
}

int CCSAuth2::CheckUpdated() {

	if (m_GGVer.dwGGVer == 0)
		return NPGG_CHECKUPDATED_NOTREADY;

	if (m_GGVer.dwGGVer < gGGAuthCtx.m_dwCurrentVersion)
		return NPGG_CHECKUPDATED_LOW;

	if (m_GGVer.dwGGVer > gGGAuthCtx.m_dwCurrentVersion)
		return NPGG_CHECKUPDATED_HIGH;

	if (m_pProtocol->m_dwGGVer < gGGAuthCtx.m_EntryListProtocol->m_dwGGVer)
		return NPGG_CHECKUPDATED_LOW;

	return (m_pProtocol->m_dwGGVer > gGGAuthCtx.m_EntryListProtocol->m_dwGGVer) ? NPGG_CHECKUPDATED_HIGH : 0;
}

void CCSAuth2::AllowOldVersion() {
	m_bAllowOldVersion = true;
}

//. Do not support c type functions at CS26

LPGGAUTH GGAuthCreateUser() {
	return new CCSAuth2();
}

UINT32 GGAuthDeleteUser(LPGGAUTH pGGAuth) {
	
	if (pGGAuth != nullptr)
		delete ((CCSAuth2*)pGGAuth);

	return ERROR_SUCCESS;
}

UINT32 GGAuthInitUser(LPGGAUTH pGGAuth) {
	
	if (pGGAuth == nullptr)
		return ERROR_GGAUTH_INVALID_PARAM;

	((CCSAuth2*)pGGAuth)->Init();

	return ERROR_SUCCESS;
}

UINT32 GGAuthCloseUser(LPGGAUTH pGGAuth) {
	
	if (pGGAuth == nullptr)
		return ERROR_GGAUTH_INVALID_PARAM;

	((CCSAuth2*)pGGAuth)->Close();

	return ERROR_SUCCESS;
}

UINT32 GGAuthGetQuery(LPGGAUTH pGGAuth, PGG_AUTH_DATA pAuthData) {
	
	if (pGGAuth == nullptr || pAuthData == nullptr)
		return ERROR_GGAUTH_INVALID_PARAM;

	UINT32 error = ((CCSAuth2*)pGGAuth)->GetAuthQuery();

	memcpy(pAuthData, &((CCSAuth2*)pGGAuth)->m_AuthQuery, sizeof(_GG_AUTH_DATA));

	return error;
}

UINT32 GGAuthCheckAnswer(LPGGAUTH pGGAuth, PGG_AUTH_DATA pAuthData) {
	
	if (pGGAuth == nullptr || pAuthData == nullptr)
		return ERROR_GGAUTH_INVALID_PARAM;

	memcpy(&((CCSAuth2*)pGGAuth)->m_AuthAnswer, pAuthData, sizeof(_GG_AUTH_DATA));

	return ((CCSAuth2*)pGGAuth)->CheckAuthAnswer();
}

int GGAuthCheckUpdated(LPGGAUTH pGGAuth) {
	
	if (pGGAuth == nullptr)
		return ERROR_GGAUTH_INVALID_PARAM;

	return ((CCSAuth2*)pGGAuth)->CheckAuthAnswer();
}

int GGAuthUserInfo(LPGGAUTH pGGAuth, char * dest, int length) {
	
	if (pGGAuth == nullptr || dest == nullptr)
		return ERROR_GGAUTH_INVALID_PARAM;

	return ((CCSAuth2*)pGGAuth)->Info(dest, length);
}

UINT32 GGAuthGetState(LPGGAUTH pGGAuth, PGG_CSAUTH_STATE pAuthState) {
	
	if (pGGAuth == nullptr || pAuthState == nullptr)
		return ERROR_GGAUTH_INVALID_PARAM;

	return ((CCSAuth2*)pGGAuth)->GetCSAuthState(pAuthState);
}

UINT32 GGAuthSetState(LPGGAUTH pGGAuth, PGG_CSAUTH_STATE pAuthState) {
	
	if (pGGAuth == nullptr || pAuthState == nullptr)
		return ERROR_GGAUTH_INVALID_PARAM;

	return ((CCSAuth2*)pGGAuth)->SetCSAuthState(pAuthState);
}

UINT32 GGAuthSetSecureOrder(LPGGAUTH pGGAuth) {
	
	// !@ Esse função não tem no game server, talvez tenho no .lib
	return ERROR_SUCCESS;
}

UINT32 GGAuthCheckUserCSAuth(LPGGAUTH pGGAuth, bool bCheck) {
	
	if (pGGAuth == nullptr)
		return ERROR_GGAUTH_INVALID_PARAM;

	return ((CCSAuth2*)pGGAuth)->CheckUserCSAuth(bCheck);
}

UINT32 GGAuthAllowOldVersion(LPGGAUTH pGGAuth) {
	
	if (pGGAuth == nullptr)
		return ERROR_GGAUTH_INVALID_PARAM;

	((CCSAuth2*)pGGAuth)->AllowOldVersion();

	return ERROR_SUCCESS;
}

UINT32 GGGetCurrentGGVer() {
	return gGGAuthCtx.m_dwCurrentVersion;
}

UINT32 GGAuthGetUserValue(LPGGAUTH pGGAuth, int type) {
	
	if (pGGAuth == nullptr || type <= 0)
		return 0xFFFFFFFF;

	_GG_AUTH_DATA *pQuery = nullptr;

	if ((type & 1) != 0)
		pQuery = &((CCSAuth2*)pGGAuth)->m_AuthQuery;

	if ((type & 2) != 0)
		pQuery = &((CCSAuth2*)pGGAuth)->m_AuthAnswer;

	if (pQuery == nullptr)
		return 0xFFFFFFFF;

	if ((type & 0x10) != 0)
		return pQuery->dwIndex;

	if ((type & 0x20) != 0)
		return pQuery->dwValue1;

	if ((type & 0x40) != 0)
		return pQuery->dwValue2;

	if ((type & 0x80) != 0)
		return pQuery->dwValue3;

	return 0xFFFFFFFF;
}
