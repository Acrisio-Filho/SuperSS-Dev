// Arquivo Protocol70.cpp
// Criado em 11/10/2021 as 14:52 por Acrisio
// Implementação da classe Protocol70

#include "Protocol70.hpp"
#include <ctime>
#include <memory.h>

#if defined(__linux__)
#include "../../Projeto IOCP/UTIL/message_pool.h"
#endif

#if defined(_WIN32)
UINT_PTR Protocol70::m_TimerIdEvent = 0;
#elif defined(__linux__)
stdA::timer_manager Protocol70::m_ManagerTimer;
stdA::job_pool Protocol70::m_JobPool;
stdA::timer* Protocol70::m_pTimer = nullptr;
stdA::thread* Protocol70::m_pThread = nullptr;
#endif

USHORT Protocol70::m_TimeValue = 0;
DWORD Protocol70::m_VarShuffle = 0;

UINT32 Protocol70::getVersion() {
	return 70;
}

void Protocol70::decryptAnswer(GG_AUTH_DATA *_pAuthAnswer) {

	if (_pAuthAnswer == nullptr)
		return;

	DWORD temp;

	temp = _pAuthAnswer->dwValue1;
	_pAuthAnswer->dwValue1 = _pAuthAnswer->dwIndex.dwIndex;
	_pAuthAnswer->dwIndex.dwIndex = _pAuthAnswer->dwIndex.dwIndex ^ temp;
	temp = _pAuthAnswer->dwValue2.dwIndex;
	_pAuthAnswer->dwValue2.dwIndex = _pAuthAnswer->dwValue3.dwIndex ^ _pAuthAnswer->dwValue1;
	_pAuthAnswer->dwValue3.dwIndex = _pAuthAnswer->dwIndex.dwIndex ^ temp;
}

UINT32 Protocol70::encryptQuery(GG_AUTH_DATA *_pAuthQuery) {

	if (_pAuthQuery == nullptr)
		return 0x0B; // invalid query

	DWORD temp;

	temp = _pAuthQuery->dwValue1;

	_pAuthQuery->dwValue1 = _pAuthQuery->dwValue3.dwIndex ^ _pAuthQuery->dwIndex.dwIndex;
	_pAuthQuery->dwIndex.dwIndex = temp;
	temp = _pAuthQuery->dwValue3.dwIndex;
	_pAuthQuery->dwValue3.dwIndex = _pAuthQuery->dwValue2.dwIndex;
	_pAuthQuery->dwValue2.dwIndex = temp;

	encryptRound8((BYTE*)_pAuthQuery);

	return 0;
}

UINT32 Protocol70::getInformation(UINT32 _type, GG_AUTH_DATA *_pAuthAnswer) {

	if (_pAuthAnswer == nullptr)
		return 0;

	UINT32 ret = 0;

	switch (_type) {
	case 0:
		ret = _pAuthAnswer->dwIndex.ucIndex[1];
		break;
	case 1:
		ret = _pAuthAnswer->dwIndex.ucIndex[2];
		break;
	case 2:
	case 3:
		ret = _pAuthAnswer->dwIndex.ucIndex[3];
		break;
	case 4:
		ret = _pAuthAnswer->dwValue1;
		break;
	case 5:
		ret = _pAuthAnswer->dwValue2.ucIndex[0];
		break;
	case 6:
		ret = _pAuthAnswer->dwValue2.ucIndex[1];
		break;
	case 7:
		ret = _pAuthAnswer->dwValue2.usIndex[1];
		break;
	case 8:
		ret = _pAuthAnswer->dwValue2.dwIndex;
		break;
	case 9:
		ret = _pAuthAnswer->dwValue3.usIndex[0];
		break;
	case 10:
		ret = _pAuthAnswer->dwValue3.usIndex[1];
		break;
	}

	return ret;
}

UINT32 Protocol70::checkValidAnswer(GG_AUTH_DATA *_pAuthAnswer, UINT32 *_pReturnFlag) {

	if (_pReturnFlag == nullptr || _pAuthAnswer == nullptr)
		return 0x12; // Invalid parameter (estou usando o de invalid AuthAnswer)

	*_pReturnFlag = 0;

	if (_pAuthAnswer->dwIndex.ucIndex[0] != 0x1a)
		return 0x12;

	DWORD temp, temp2;

	temp = CRC32((BYTE*)_pAuthAnswer, 0x0E, 0x1021);
	temp2 = _pAuthAnswer->dwValue3.usIndex[1] - (WORD)temp;
	temp = CRC32((BYTE*)&_pAuthAnswer->dwValue1, 4, 0x1021);

	if (temp2 != 0) {
		if (temp2 == (_pAuthAnswer->dwIndex.ucIndex[3] - (UCHAR)temp)) {
			*_pReturnFlag = temp2;
			return 0x15;
		}
		return 0x13;
	}
	return (_pAuthAnswer->dwIndex.ucIndex[3] != (UCHAR)temp) ? 0x14 : 0;
}

UINT32 Protocol70::getAuthQuery(UINT32 *_pUserFlag, GG_VERSION *_pGGVer, CCSAuth2 *_pCCSAuth2, DWORD _nSequenceNum, DWORD _dwServerKey) {
	
	if (_pUserFlag == nullptr || _pGGVer == nullptr || _pCCSAuth2 == nullptr)
		return 0x0B; // Invalid parameter.

	if ((*_pUserFlag & 2) != 0)
		return 0x0B;

	*_pUserFlag |= 2;

	if ((*_pUserFlag & 1) == 0)
		*_pUserFlag |= 1;

	_pCCSAuth2->m_AuthQuery.dwIndex.ucIndex[0] = 0x1a;
	_pCCSAuth2->m_AuthQuery.dwIndex.ucIndex[2] = getVersion();

	if (_nSequenceNum == 0)
		_pCCSAuth2->m_AuthQuery.dwIndex.ucIndex[1] = 0;
	else {
		_pCCSAuth2->m_AuthQuery.dwIndex.ucIndex[1] = 1;
		_dwServerKey = getRandomWord();
	}

	_pCCSAuth2->m_AuthQuery.dwValue1 = _dwServerKey;
	_pCCSAuth2->m_AuthQuery.dwIndex.ucIndex[3] = (UCHAR)m_TimeTable[m_TimeValue % 0x1E];

	if ((*_pUserFlag & 0x200) != 0x200) {
		if (*_pUserFlag > 0x50000 && (rand() % 5) == 2) {
			*_pUserFlag |= 0x300;
			_pCCSAuth2->m_AuthQuery.dwIndex.ucIndex[3] = (UCHAR)m_TimeTable2[rand() % 0x0E];
		}else
			*_pUserFlag += 0x10000;
	}

	_pCCSAuth2->m_AuthQuery.dwValue2.dwIndex = m_ValueTable[_pCCSAuth2->m_AuthQuery.dwIndex.ucIndex[3]] ^ _pCCSAuth2->m_AuthQuery.dwValue1;
	_pCCSAuth2->m_AuthQuery.dwValue3.usIndex[0] = (USHORT)getRandomWord();
	_pCCSAuth2->m_AuthQuery.dwValue3.usIndex[1] = (USHORT)CRC32((BYTE*)&_pCCSAuth2->m_AuthQuery, 0x0E, 0x1021);

	return 0;
}

UINT32 Protocol70::loopAuth(unsigned char *_pByLastLoop1, DWORD *_pdwLoop1AuthArray, unsigned char *_pByLastLoop2, DWORD *_pdwLoop2AuthArray, GG_AUTH_DATA *_pAuthAnswer, UINT32 _nSequenceNum) {

	if (_pByLastLoop1 == nullptr || _pByLastLoop2 == nullptr || _pdwLoop1AuthArray == nullptr || _pdwLoop2AuthArray == nullptr || _pAuthAnswer == nullptr)
		return 0;

	if (_nSequenceNum == 0)
		return _nSequenceNum;

	if (_nSequenceNum == 1 && (_pAuthAnswer->dwValue2.ucIndex[0] == 0 || _pAuthAnswer->dwValue2.ucIndex[1] == 0))
		return 0;

	if (_pAuthAnswer->dwValue2.ucIndex[0] == *_pByLastLoop1)
		return 0x10;

	UINT32 lastlooptemp = _pdwLoop1AuthArray[*_pByLastLoop1];

	if (lastlooptemp == 0)
		return 0x10;

	DWORD index, value2Index = _pdwLoop1AuthArray[_pAuthAnswer->dwValue2.ucIndex[0]];

	if (value2Index == 0) {
		
		index = *_pByLastLoop1;

		while (++index <= _pAuthAnswer->dwValue2.ucIndex[0]) {

			if (_pdwLoop1AuthArray[index % 0x100] == 0)
				_pdwLoop1AuthArray[index % 0x100] = loopAuthHash1(lastlooptemp);

			lastlooptemp = _pdwLoop1AuthArray[index % 0x100];

		}

		value2Index = _pdwLoop1AuthArray[_pAuthAnswer->dwValue2.ucIndex[0]];
	}

	*_pByLastLoop1 = _pAuthAnswer->dwValue2.ucIndex[0];

	if (_pAuthAnswer->dwValue2.ucIndex[1] == *_pByLastLoop2)
		return 0x10;

	lastlooptemp = _pdwLoop2AuthArray[*_pByLastLoop2];

	if (lastlooptemp == 0)
		return 0x10;

	DWORD value2Index2 = _pdwLoop2AuthArray[_pAuthAnswer->dwValue2.ucIndex[1]];

	if (value2Index2 == 0) {

		index = *_pByLastLoop2;

		while (++index <= _pAuthAnswer->dwValue2.ucIndex[1]) {

			if (_pdwLoop2AuthArray[index % 0x100] == 0)
				_pdwLoop2AuthArray[index % 0x100] = loopAuthHash2(lastlooptemp);

			lastlooptemp = _pdwLoop2AuthArray[index % 0x100];
		}

		value2Index2 = _pdwLoop2AuthArray[_pAuthAnswer->dwValue2.ucIndex[1]];
	}

	*_pByLastLoop2 = _pAuthAnswer->dwValue2.ucIndex[1];

	return (_pAuthAnswer->dwValue2.usIndex[1] != (USHORT)(value2Index + value2Index2)) ? 0x11 : 0;
}

UINT32 Protocol70::checkAuthAnswer(UINT32 *_pUserFlag, GG_VERSION *_pGGVer, GG_AUTH_DATA *_pAuthAnswer, GG_AUTH_DATA *_pAuthChecked, UINT32 _nSequenceNum, UINT32 *_pdwLastValue4) {

	if (_pUserFlag == nullptr || _pGGVer == nullptr || _pAuthAnswer == nullptr || _pAuthChecked == nullptr || _pdwLastValue4 == nullptr)
		return 0x0C; // invalid parameter. (USANDO o do invalid version protocol)

	GG_AUTH_DATA gg_auth_temp;

	AlgorithmGenAuthData(_pAuthAnswer, &gg_auth_temp, _pGGVer);

	UINT32 value3 = gg_auth_temp.dwValue3.dwIndex ^ *_pdwLastValue4;

	if (_pAuthChecked->dwIndex.ucIndex[2] != getVersion())
		return 0x0C;

	if (_pAuthChecked->dwIndex.ucIndex[1] == 0) {

		if (_pAuthChecked->dwValue1 != _pGGVer->dwGGver)
			return 0x0E;

		value3 = (value3 + 0x23048219 + m_ValueTable2[(_pGGVer->wNum + _pGGVer->wDay + _pGGVer->wMonth + _pGGVer->wYear) % 100]);
	}

	if (_pAuthChecked->dwValue3.usIndex[0] != (USHORT)value3)
		return 0x0D;

	*_pdwLastValue4 = _pAuthChecked->dwValue1;

	if ((*_pUserFlag & 0x300) == 0x300)
		*_pUserFlag = *_pUserFlag & 0xFCFF | 0x200;

	return 0;
}

#if defined(__linux__)
void* job_exec(void* _param) {

    if (_param == nullptr)
        return (void*)0;

    stdA::job_pool *jl = reinterpret_cast<stdA::job_pool*>(_param);
    stdA::job *_job = nullptr;

    while (1) {

        _job = jl->getJob();

        if (_job != nullptr) {

            if (_job->execute_job() != 0)
                stdA::_smp::message_pool::getInstance().push(new stdA::message("[main::job_exec][Error] fail in exec job.", stdA::CL_FILE_LOG_AND_CONSOLE));

            delete _job;

            _job = nullptr;
        }
    }

    return (void*)0;
}
#endif

UINT_PTR Protocol70::setTimer() {
#if defined(_WIN32)
	m_TimerIdEvent = SetTimer(NULL, 0, 300000, Protocol70::UpdateTimer);

	return m_TimerIdEvent;
#elif defined(__linux__)

	try {

		if (m_pTimer != nullptr)
			m_ManagerTimer.deleteTimer(m_pTimer);

		if (m_pThread != nullptr)
			delete m_pThread;

		m_pThread = new stdA::thread(0, job_exec, &m_JobPool);

		m_pTimer = m_ManagerTimer.createTimer(300000, new stdA::timer::timer_param{stdA::job(Protocol70::UpdateTimer, nullptr, nullptr), m_JobPool}, stdA::timer::PERIODIC_INFINITE);

	}catch (...) {};

	return TRUE;
#endif
}

#if defined(_WIN32)
VOID Protocol70::UpdateTimer(HWND, UINT, UINT_PTR, DWORD) {
#elif defined(__linux__)
int Protocol70::UpdateTimer(void*, void*) {
#endif
	SYSTEMTIME st;
	DWORD temp;

	GetSystemTime(&st);

	if (st.wDay == 0x1f)
		temp = st.wHour + (st.wMonth * 0x708) - 0x450;
	else
		temp = (((st.wMonth * 0x4b) + st.wDay) * 0x18) + st.wHour - 0x720;
	
	m_TimeValue = temp % 0x1e;

#if defined(__linux__)
	return 0;
#endif
}

#if defined(__linux__)
void Protocol70::destroy() {

	try {

		if (m_pTimer != nullptr)
			m_ManagerTimer.deleteTimer(m_pTimer);

		if (m_pThread != nullptr)
			delete m_pThread;

	} catch(...) {};

	m_pTimer = nullptr;
	m_pThread = nullptr;
}
#endif

void Protocol70::InitializeVersionDll() {
	srand((UINT32)time(nullptr));
}

DWORD Protocol70::getRandomWord() {

	DWORD random = 0;

	for (UINT32 i = 0; i < 32; i += 4)
		random |= ((UCHAR)rand() << i);

	return random;
}

void Protocol70::encryptRound8(BYTE *_dst) {

	DWORD round, index, bits, index2;
	UCHAR hash;
	UCHAR *pOffset, *pOffset2;

	round = 8; // round
	index = 1;

	do {

		bits = round; // round
		pOffset2 = _dst;
		pOffset = _dst;

		if (round != 0) {

			do {

				pOffset2 += index;
				index2 = index;

				if (index != 0) {

					do {

						hash = *pOffset2 ^ *pOffset;

						if ((hash & 0x80) == 0)
							hash <<= 1;
						else
							hash = hash << 1 ^ 0xA9;

						*pOffset ^= hash;
						pOffset++;
						*pOffset2 ^= hash;
						pOffset2++;

					} while (--index2 > 0);
				}

				pOffset += index;

			} while (--bits > 0);
		}

		// divide and plus by 2
		round >>= 1;
		index <<= 1;

	} while (round > 0);
}

UINT32 Protocol70::CRC32(BYTE *_src, UINT32 _len, UINT32 _mask) {
	
	UINT32 crc32_hash = 0, index = 0;

	do {
		crc32_hash = CRC32_part(_src[index++], _mask, crc32_hash);
	} while (index < _len);

	return ~crc32_hash;
}

UINT32 Protocol70::CRC32_part(BYTE _src, UINT32 _mask, UINT32 _part) {
	
	UINT32 bits = 8;
	UINT32 hash = (UINT32)(_src & 0xFF) << 8;

	do {

		if (((hash ^ _part) & 0x8000) == 0)
			_part <<= 1;
		else
			_part = _part * 2 ^ _mask;

		// plus 2
		hash <<= 1;

	} while (--bits > 0);

	return _part;
}

INT32 Protocol70::loopAuthHash1(UINT32 _value) {
	
	INT32 hash = (_value % 0x1C271) * 0x33D0 + (_value / 0x1C271) * -0xC28;

	if (hash < 1)
		hash += 0x7FFFFFFF;

	if (hash == 0)
		hash = 0x7FFFFFFF;

	return hash;
}

INT32 Protocol70::loopAuthHash2(UINT32 _value) {
	
	INT32 hash = (_value % 0x1F39D) * 0x2C08 + (_value / 0x1F39D) * -0xA3B;

	return (hash < 1 && (hash += 0x7FFFFFFF) == 0) ? 0x7FFFFFFF : hash;
}

void Protocol70::AlgorithmGenAuthData(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	if (_pSrc == nullptr || _pDst == nullptr || _pGGVer == nullptr)
		return;

	memset(_pDst, 0, sizeof(GG_AUTH_DATA));

	switch (_pSrc->dwIndex.ucIndex[3]) {
	case 1:
		AlgorithmShuffle_1(_pSrc, _pDst, _pGGVer);
		break;
	case 2:
		AlgorithmShuffle_2(_pSrc, _pDst, _pGGVer);
		break;
	case 3:
		AlgorithmShuffle_3(_pSrc, _pDst, _pGGVer);
		break;
	case 4:
		AlgorithmShuffle_4(_pSrc, _pDst, _pGGVer);
		break;
	case 5:
		AlgorithmShuffle_5(_pSrc, _pDst, _pGGVer);
		break;
	case 6:
		AlgorithmShuffle_6(_pSrc, _pDst, _pGGVer);
		break;
	case 7:
		AlgorithmShuffle_7(_pSrc, _pDst, _pGGVer);
		break;
	case 8:
		AlgorithmShuffle_8(_pSrc, _pDst, _pGGVer);
		break;
	case 9:
		AlgorithmShuffle_9(_pSrc, _pDst, _pGGVer);
		break;
	case 10:
		AlgorithmShuffle_10(_pSrc, _pDst, _pGGVer);
		break;
	case 11:
		AlgorithmShuffle_11(_pSrc, _pDst, _pGGVer);
		break;
	case 12:
		AlgorithmShuffle_12(_pSrc, _pDst, _pGGVer);
		break;
	case 13:
		AlgorithmShuffle_13(_pSrc, _pDst, _pGGVer);
		break;
	case 14:
		AlgorithmShuffle_14(_pSrc, _pDst, _pGGVer);
		break;
	case 15:
		AlgorithmShuffle_15(_pSrc, _pDst, _pGGVer);
		break;
	case 16:
		AlgorithmShuffle_16(_pSrc, _pDst, _pGGVer);
		break;
	case 17:
		AlgorithmShuffle_17(_pSrc, _pDst, _pGGVer);
		break;
	case 18:
		AlgorithmShuffle_18(_pSrc, _pDst, _pGGVer);
		break;
	case 19:
		AlgorithmShuffle_19(_pSrc, _pDst, _pGGVer);
		break;
	case 20:
		AlgorithmShuffle_20(_pSrc, _pDst, _pGGVer);
		break;
	case 21:
		AlgorithmShuffle_21(_pSrc, _pDst, _pGGVer);
		break;
	case 22:
		AlgorithmShuffle_22(_pSrc, _pDst, _pGGVer);
		break;
	case 23:
		AlgorithmShuffle_23(_pSrc, _pDst, _pGGVer);
		break;
	case 24:
		AlgorithmShuffle_24(_pSrc, _pDst, _pGGVer);
		break;
	case 25:
		AlgorithmShuffle_25(_pSrc, _pDst, _pGGVer);
		break;
	case 26:
		AlgorithmShuffle_26(_pSrc, _pDst, _pGGVer);
		break;
	case 27:
		AlgorithmShuffle_27(_pSrc, _pDst, _pGGVer);
		break;
	case 28:
		AlgorithmShuffle_28(_pSrc, _pDst, _pGGVer);
		break;
	case 29:
		AlgorithmShuffle_29(_pSrc, _pDst, _pGGVer);
		break;
	case 30:
		AlgorithmShuffle_30(_pSrc, _pDst, _pGGVer);
		break;
	case 31:
		AlgorithmShuffle_31(_pSrc, _pDst, _pGGVer);
		break;
	case 32:
		AlgorithmShuffle_32(_pSrc, _pDst, _pGGVer);
		break;
	case 33:
		AlgorithmShuffle_33(_pSrc, _pDst, _pGGVer);
		break;
	case 34:
		AlgorithmShuffle_34(_pSrc, _pDst, _pGGVer);
		break;
	case 35:
		AlgorithmShuffle_35(_pSrc, _pDst, _pGGVer);
		break;
	case 36:
		AlgorithmShuffle_36(_pSrc, _pDst, _pGGVer);
		break;
	case 37:
		AlgorithmShuffle_37(_pSrc, _pDst, _pGGVer);
		break;
	case 38:
		AlgorithmShuffle_38(_pSrc, _pDst, _pGGVer);
		break;
	case 39:
		AlgorithmShuffle_39(_pSrc, _pDst, _pGGVer);
		break;
	case 40:
		AlgorithmShuffle_40(_pSrc, _pDst, _pGGVer);
		break;
	case 41:
		AlgorithmShuffle_41(_pSrc, _pDst, _pGGVer);
		break;
	case 42:
		AlgorithmShuffle_42(_pSrc, _pDst, _pGGVer);
		break;
	case 43:
		AlgorithmShuffle_43(_pSrc, _pDst, _pGGVer);
		break;
	case 44:
		AlgorithmShuffle_44(_pSrc, _pDst, _pGGVer);
		break;
	case 45:
		AlgorithmShuffle_45(_pSrc, _pDst, _pGGVer);
		break;
	case 46:
		AlgorithmShuffle_46(_pSrc, _pDst, _pGGVer);
		break;
	case 47:
		AlgorithmShuffle_47(_pSrc, _pDst, _pGGVer);
		break;
	case 48:
		AlgorithmShuffle_48(_pSrc, _pDst, _pGGVer);
		break;
	case 49:
		AlgorithmShuffle_49(_pSrc, _pDst, _pGGVer);
		break;
	case 50:
		AlgorithmShuffle_50(_pSrc, _pDst, _pGGVer);
		break;
	}
}

void Protocol70::AlgorithmShuffle_1(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	DWORD tmp, tmp2, tmp3;

	tmp = _pSrc->dwValue1 ^ m_ConstShuffle[6];
	tmp3 = (_pSrc->dwValue2.dwIndex ^ 0xFBB6615F);
	tmp2 = tmp + 0x4E7EE885;

	tmp = m_ConstShuffleArray[0x5F + ((_pGGVer->wNum * _pGGVer->wDay + 0xC) % 10 * 100)] + (m_ConstShuffle[3] ^ _pSrc->dwValue3.dwIndex ^ (tmp3 - 0x6911F949) ^ tmp);

	m_VarShuffle = (tmp3 - 0x6911F949 + 0xEAA228D2) ^ tmp;

	_pDst->dwValue3.dwIndex = (m_VarShuffle ^ tmp2) + m_ConstShuffle[13] + tmp + tmp2;
}


void Protocol70::AlgorithmShuffle_2(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	_pDst->dwValue3.dwIndex = ((m_ConstShuffle[7] + 0x4FF1EE4B) ^ m_ConstShuffle[14]) + m_ConstShuffle[10] + 0xA11F94E0 + m_ConstShuffle[5];
}

void Protocol70::AlgorithmShuffle_3(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	m_VarShuffle = m_ConstShuffleArray[0x46 + ((_pGGVer->wNum * _pGGVer->wDay + 0x57) % 10 * 100)] + (_pSrc->dwValue2.dwIndex ^ (m_ConstShuffle[0] + 0xD7FE3FD2)) + m_ConstShuffle[4];

	_pDst->dwValue3.dwIndex = (m_VarShuffle ^ 0x13B034F4) + ((_pSrc->dwValue3.dwIndex + 0x3E42DADC) ^ m_ConstShuffle[11]) + m_ConstShuffle[0] + 0xD7FE3FD2;
}

void Protocol70::AlgorithmShuffle_4(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	_pDst->dwValue3.dwIndex = ((m_ConstShuffle[7] + 0xE274FAD8) ^ m_ConstShuffle[14]) + m_ConstShuffle[10] + 0x304ECFF3 + m_ConstShuffle[5];
}

void Protocol70::AlgorithmShuffle_5(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	_pDst->dwValue3.dwIndex = ((m_ConstShuffle[7] + 0xDDDD7978) ^ m_ConstShuffle[14]) + m_ConstShuffle[10] + 0xEBFDF1A1 + m_ConstShuffle[5];
}

void Protocol70::AlgorithmShuffle_6(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	DWORD temp;

	temp = _pSrc->dwValue1 - 0x43A1A2A8;

	m_VarShuffle = m_ConstShuffle[7] + temp;

	_pDst->dwValue3.dwIndex = m_ConstShuffleArray[0x3E + ((_pGGVer->wNum * _pGGVer->wDay + 0x4F) % 10 * 100)] + (m_ConstShuffle[12] ^ (_pSrc->dwValue2.dwIndex + 0xC25DD915 + m_ConstShuffle[2])) + m_VarShuffle + 0x5D42865F + temp;
}

void Protocol70::AlgorithmShuffle_7(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	m_VarShuffle = (m_ConstShuffleArray[0x2C + ((_pGGVer->wNum * _pGGVer->wDay + 0x3D) % 10 * 100)] - _pSrc->dwValue2.dwIndex) + (m_ConstShuffle[0] ^ 0xDE696108) + m_ConstShuffle[4];

	_pDst->dwValue3.dwIndex = (m_VarShuffle ^ 0x255A87F8) + ((_pSrc->dwValue3.dwIndex + 0x3E0C4101) ^ m_ConstShuffle[11]) + (m_ConstShuffle[0] ^ 0xDE696108);
}

void Protocol70::AlgorithmShuffle_8(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	DWORD tmp, tmp2, tmp3;

	tmp3 = _pSrc->dwValue2.dwIndex ^ 0x8D032A1;
	tmp = _pSrc->dwValue1 ^ m_ConstShuffle[6];
	tmp2 = tmp + 0xBF03C27;

	tmp = m_ConstShuffleArray[40 + ((_pGGVer->wNum * _pGGVer->wDay + 0x39) % 10 * 100)] + (m_ConstShuffle[3] ^ (tmp3 + 0x2780B443 + tmp) ^ _pSrc->dwValue3.dwIndex);

	m_VarShuffle = (tmp3 + 0x2780B443 + 0x8807164F) ^ tmp;

	_pDst->dwValue3.dwIndex = (m_VarShuffle ^ tmp2) + m_ConstShuffle[13] + tmp + tmp2;
}

void Protocol70::AlgorithmShuffle_9(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	DWORD temp;

	temp = _pSrc->dwValue1 + 0x3CFCCBA7;

	m_VarShuffle = m_ConstShuffle[7] + temp;

	_pDst->dwValue3.dwIndex = m_ConstShuffleArray[0x1B + ((_pGGVer->wNum * _pGGVer->wDay + 0x2C) % 10 * 100)] + (m_ConstShuffle[12] ^ (_pSrc->dwValue2.dwIndex + 0xB9ECE2C8 + m_ConstShuffle[2])) + m_VarShuffle + 0x7EC4DFB4 + temp;
}

void Protocol70::AlgorithmShuffle_10(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	DWORD temp, temp2;

	temp = _pSrc->dwValue1 ^ m_ConstShuffle[9] ^ 0xEBFEB8A4;
	temp2 = _pSrc->dwValue3.dwIndex ^ 0x64C341C9;

	m_VarShuffle = temp + temp2 + 0x1AF440FB;

	_pDst->dwValue3.dwIndex = m_ConstShuffleArray[0x3D + ((_pGGVer->wNum * _pGGVer->wDay + 0x4E) % 10 * 100)] + (m_VarShuffle ^ 0x64C341C9) + m_ConstShuffle[16] + temp2 + 0x44E + temp;
}

void Protocol70::AlgorithmShuffle_11(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	m_VarShuffle = m_ConstShuffleArray[0x5A + ((_pGGVer->wNum * _pGGVer->wDay + 7) % 10 * 100)] + (_pSrc->dwValue2.dwIndex ^ m_ConstShuffle[0] + 0x636F53BA) + m_ConstShuffle[4];

	_pDst->dwValue3.dwIndex = (m_VarShuffle ^ 0x95E8D587) + (_pSrc->dwValue3.dwIndex ^ m_ConstShuffle[11] ^ 0x23BFFE0B) + m_ConstShuffle[0] + 0x636F53BA;
}

void Protocol70::AlgorithmShuffle_12(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	GG_AUTH_DATA lSrc{ 0 }, lDst{ 0 };
	DWORD tmp, tmp2, tmp3;

	lSrc.dwIndex.dwIndex = 0x2C;
	lSrc.dwValue1 = _pSrc->dwValue1;
	lSrc.dwValue2.dwIndex = _pSrc->dwValue2.dwIndex;
	lSrc.dwValue3.dwIndex = _pSrc->dwValue3.dwIndex;

	AlgorithmShuffle_44(&lSrc, &lDst, _pGGVer);

	tmp = (_pSrc->dwValue3.dwIndex ^ 0xE6FD42F5) + 0x36EEFDC1;
	m_VarShuffle = m_ConstShuffle[1] + tmp;
	tmp2 = m_ConstShuffle[8] + 0xCE64844C + _pSrc->dwValue2.dwIndex ^ m_ConstShuffle[15] ^ lDst.dwValue1;

	tmp3 = m_ConstShuffleArray[0x3F + ((_pGGVer->wNum * _pGGVer->wDay + 0x50) % 10 * 100)];

	_pDst->dwValue3.dwIndex = (lDst.dwValue3.dwIndex ^ tmp + tmp3) + tmp2 + (m_ConstShuffle[2] ^ tmp2);
}

void Protocol70::AlgorithmShuffle_13(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	m_VarShuffle = (m_ConstShuffleArray[0x3C + ((_pGGVer->wNum * _pGGVer->wDay + 0x4D) % 10 * 100)] - _pSrc->dwValue2.dwIndex) + m_ConstShuffle[4] + m_ConstShuffle[0] + 0xB7132D45;

	_pDst->dwValue3.dwIndex = (m_VarShuffle ^ 0x9BCBC35A) + ((_pSrc->dwValue3.dwIndex - 0x654C24BA) ^ m_ConstShuffle[11]) + m_ConstShuffle[0] + 0xB7132D45;
}

void Protocol70::AlgorithmShuffle_14(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	DWORD tmp;

	tmp = _pSrc->dwValue1 - 0x700D5EBD;

	m_VarShuffle = m_ConstShuffle[7] + tmp;

	_pDst->dwValue3.dwIndex = m_ConstShuffleArray[0x55 + ((_pGGVer->wNum * _pGGVer->wDay + 2) % 10 * 100)] 
			+ (m_ConstShuffle[12] ^ _pSrc->dwValue2.dwIndex + 0xBD5FA2D6 + m_ConstShuffle[2]) + m_VarShuffle + 0x51E81829 + tmp;
}

void Protocol70::AlgorithmShuffle_15(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	DWORD tmp, tmp2, tmp3;

	tmp = _pSrc->dwValue1 ^ m_ConstShuffle[6];
	tmp3 = (_pSrc->dwValue2.dwIndex ^ 0x7D0C4DB3) + 0x112EB835;
	tmp2 = tmp - 0x787B7DFC;

	tmp = m_ConstShuffleArray[0x3D + ((_pGGVer->wNum * _pGGVer->wDay + 0x4E) % 10 * 100)] + (m_ConstShuffle[3] ^ (tmp - tmp3) ^ _pSrc->dwValue3.dwIndex);

	m_VarShuffle = tmp ^ tmp3 ^ 0x15D20B4;

	_pDst->dwValue3.dwIndex = (m_VarShuffle ^ tmp2) + m_ConstShuffle[13] + tmp + tmp2;
}

void Protocol70::AlgorithmShuffle_16(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	DWORD tmp;

	tmp = _pSrc->dwValue1 - 0xBDC759F;

	m_VarShuffle = m_ConstShuffle[7] + tmp;

	_pDst->dwValue3.dwIndex = m_ConstShuffleArray[0x35 + ((_pGGVer->wNum * _pGGVer->wDay + 0x46) % 10 * 100)] 
			+ (m_ConstShuffle[12] ^ _pSrc->dwValue2.dwIndex + 0x2F38AB56 + m_ConstShuffle[2]) + m_VarShuffle + 0xB3639BC5 + tmp;
}

void Protocol70::AlgorithmShuffle_17(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	DWORD tmp;

	tmp = _pSrc->dwValue1 - 0x4C123474;

	m_VarShuffle = m_ConstShuffle[7] + tmp;

	_pDst->dwValue3.dwIndex = m_ConstShuffleArray[0xB + ((_pGGVer->wNum * _pGGVer->wDay + 0x1C) % 10 * 100)]
			+ (m_ConstShuffle[12] ^ _pSrc->dwValue2.dwIndex + 0x88C1DD1E + m_ConstShuffle[2]) + m_VarShuffle + 0xF2560F9C + tmp;
}

void Protocol70::AlgorithmShuffle_18(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	DWORD tmp;

	tmp = _pSrc->dwValue1 - 0xAC254F3;

	m_VarShuffle = m_ConstShuffle[7] + tmp;

	_pDst->dwValue3.dwIndex = m_ConstShuffleArray[0x4C + ((_pGGVer->wNum * _pGGVer->wDay + 0x5D) % 10 * 100)]
			+ (m_ConstShuffle[12] ^ _pSrc->dwValue2.dwIndex + 0xDF54F607 + m_ConstShuffle[2]) + m_VarShuffle + 0xEC00FE66 + tmp;
}

void Protocol70::AlgorithmShuffle_19(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	DWORD tmp, tmp2, tmp3;

	tmp2 = _pSrc->dwValue1 + m_ConstShuffle[6];
	tmp3 = _pSrc->dwValue2.dwIndex + 0x64D9CE5F;
	tmp = ((tmp2 - tmp3) ^ _pSrc->dwValue3.dwIndex) + m_ConstShuffleArray[0x8 + ((_pGGVer->wNum * _pGGVer->wDay + 0x19) % 10 * 100)] + m_ConstShuffle[3];

	m_VarShuffle = tmp + 0x4ACF92F6 + tmp3;

	_pDst->dwValue3.dwIndex = m_ConstShuffle[13] + (tmp2 - 0x7888FFF0) * 2 + m_VarShuffle + tmp;
}

void Protocol70::AlgorithmShuffle_20(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	DWORD tmp, tmp2, tmp3;

	tmp = _pSrc->dwValue1 ^ m_ConstShuffle[6];
	tmp3 = _pSrc->dwValue2.dwIndex ^ 0x772DC4B;
	tmp2 = tmp ^ 0x8987DBF7;

	tmp = m_ConstShuffleArray[0x16 + ((_pGGVer->wNum * _pGGVer->wDay + 0x27) % 10 * 100)]
			+ (m_ConstShuffle[3] ^ _pSrc->dwValue3.dwIndex ^ (tmp3 - 0x6E510557) ^ tmp);

	m_VarShuffle = (tmp3 - 0x6E510557) + 0xE930F0E4 ^ tmp;

	_pDst->dwValue3.dwIndex = (m_VarShuffle ^ tmp2) + m_ConstShuffle[13] + tmp + tmp2;
}

void Protocol70::AlgorithmShuffle_21(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	GG_AUTH_DATA lSrc{ 0 }, lDst{ 0 };
	DWORD tmp, tmp2, tmp3;

	lSrc.dwIndex.dwIndex = 5;
	lSrc.dwValue1 = _pSrc->dwValue1;
	lSrc.dwValue2.dwIndex = _pSrc->dwValue2.dwIndex;
	lSrc.dwValue3.dwIndex = _pSrc->dwValue3.dwIndex;

	AlgorithmShuffle_5(&lSrc, &lDst, _pGGVer);

	tmp3 = _pSrc->dwValue3.dwIndex - 0x32DF687E ^ 0xA81DA4C;

	m_VarShuffle = m_ConstShuffle[1] + tmp3;

	tmp2 = (_pSrc->dwValue2.dwIndex ^ 0x6CBDA5B0) + m_ConstShuffle[8] ^ m_ConstShuffle[15];

	tmp = m_ConstShuffleArray[0x39 + ((_pGGVer->wNum * _pGGVer->wDay + 0x4A) % 10 * 100)];

	tmp2 = tmp2 ^ lDst.dwValue1;

	_pDst->dwValue3.dwIndex = (lDst.dwValue3.dwIndex ^ tmp3 + tmp) + tmp2 + (m_ConstShuffle[2] ^ tmp2);
}

void Protocol70::AlgorithmShuffle_22(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	m_VarShuffle = m_ConstShuffleArray[0x1E + ((_pGGVer->wNum * _pGGVer->wDay + 0x2F) % 10 * 100)]
			+ _pSrc->dwValue2.dwIndex + m_ConstShuffle[4] + (m_ConstShuffle[0] ^ 0x6A27AB7D);

	_pDst->dwValue3.dwIndex = m_VarShuffle + (_pSrc->dwValue3.dwIndex + 0x17634783 ^ m_ConstShuffle[11]) + 0xAAC6D4B2 + (m_ConstShuffle[0] ^ 0x6A27AB7D);
}

void Protocol70::AlgorithmShuffle_23(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	DWORD tmp, tmp2, tmp3;

	tmp2 = _pSrc->dwValue1 + m_ConstShuffle[6];
	tmp3 = _pSrc->dwValue2.dwIndex - 0x4DDEDE8C;
	tmp = (_pSrc->dwValue3.dwIndex ^ tmp3 ^ tmp2) + m_ConstShuffleArray[0x5B + ((_pGGVer->wNum * _pGGVer->wDay + 8) % 10 * 100)] + m_ConstShuffle[3];

	m_VarShuffle = tmp + 0xE8E70BCB + tmp3;

	_pDst->dwValue3.dwIndex = m_ConstShuffle[13] + ((tmp2 - 0x6ECD48D1) * 2) + m_VarShuffle + tmp;
}

void Protocol70::AlgorithmShuffle_24(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	DWORD tmp, tmp2;

	tmp = _pSrc->dwValue2.dwIndex + 0x22289B2A;
	tmp2 = (_pSrc->dwValue3.dwIndex ^ tmp ^ _pSrc->dwValue1 - m_ConstShuffle[6]) + m_ConstShuffleArray[0x3B + ((_pGGVer->wNum * _pGGVer->wDay + 0x4C) % 10 * 100)] - m_ConstShuffle[3];

	m_VarShuffle = (tmp ^ 0x33C8C315) - tmp2;

	_pDst->dwValue3.dwIndex = m_ConstShuffle[13] + m_VarShuffle + tmp2;
}

void Protocol70::AlgorithmShuffle_25(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	m_VarShuffle = (m_ConstShuffleArray[0x2 + ((_pGGVer->wNum * _pGGVer->wDay + 0x13) % 10 * 100)] - _pSrc->dwValue2.dwIndex) + m_ConstShuffle[4] + (m_ConstShuffle[0] ^ 0x370D3D46);

	_pDst->dwValue3.dwIndex = m_VarShuffle + (_pSrc->dwValue3.dwIndex ^ m_ConstShuffle[11] ^ 0x2600DDE6) + 0xC2BB8C31 + (m_ConstShuffle[0] ^ 0x370D3D46);
}

void Protocol70::AlgorithmShuffle_26(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	DWORD tmp;

	tmp = _pSrc->dwValue1 ^ 0xE8F62D1F;

	m_VarShuffle = m_ConstShuffle[7] + tmp;

	_pDst->dwValue3.dwIndex = (m_VarShuffle ^ 0xF36E1273) + tmp + m_ConstShuffleArray[0x2A + ((_pGGVer->wNum * _pGGVer->wDay + 0x3B) % 10 * 100)] + (m_ConstShuffle[12] ^ (_pSrc->dwValue2.dwIndex ^ 0x3B08B74) + m_ConstShuffle[2]);
}

void Protocol70::AlgorithmShuffle_27(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	DWORD tmp, tmp2;

	tmp = (_pSrc->dwValue1 - m_ConstShuffle[9]) + 0x7A2021F;
	tmp2 = _pSrc->dwValue3.dwIndex ^ 0x8F251A;

	m_VarShuffle = tmp2 + 0xB1176F6D + tmp;

	_pDst->dwValue3.dwIndex = m_ConstShuffleArray[0x5D + ((_pGGVer->wNum * _pGGVer->wDay + 10) % 10 * 100)] + (m_VarShuffle ^ 0x8F251A) + m_ConstShuffle[16] + tmp2 + 0x44E + tmp;
}

void Protocol70::AlgorithmShuffle_28(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	m_VarShuffle = m_ConstShuffleArray[0x1D + ((_pGGVer->wNum * _pGGVer->wDay + 0x2E) % 10 * 100)] + (_pSrc->dwValue2.dwIndex ^ m_ConstShuffle[0] + 0x7CABC29E) + m_ConstShuffle[4];

	_pDst->dwValue3.dwIndex = m_VarShuffle + (_pSrc->dwValue3.dwIndex + 0x77D72A5A ^ m_ConstShuffle[11]) + 0x2A3A40E + m_ConstShuffle[0] + 0x7CABC29E;
}

void Protocol70::AlgorithmShuffle_29(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	DWORD tmp, tmp2, tmp3;

	tmp2 = _pSrc->dwValue1 + m_ConstShuffle[6];
	tmp = _pSrc->dwValue2.dwIndex + 0x3C1E8FCB;
	tmp3 = (tmp + tmp2 ^ _pSrc->dwValue3.dwIndex) + m_ConstShuffleArray[0x5E + ((_pGGVer->wNum * _pGGVer->wDay + 0xB) % 10 * 100)] + m_ConstShuffle[3];

	m_VarShuffle = (tmp ^ 0x269E8A65) + tmp3;

	_pDst->dwValue3.dwIndex = m_ConstShuffle[13] + (tmp2 - 0x65FCBAE6) * 2 + m_VarShuffle + tmp3;
}

void Protocol70::AlgorithmShuffle_30(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	DWORD tmp, tmp2;

	tmp = _pSrc->dwValue2.dwIndex - 0x26425C57;
	tmp2 = (((_pSrc->dwValue1 - m_ConstShuffle[6]) - tmp) ^ _pSrc->dwValue3.dwIndex) + (m_ConstShuffleArray[0x14 + ((_pGGVer->wNum * _pGGVer->wDay + 0x25) % 10 * 100)] - m_ConstShuffle[3]);

	m_VarShuffle = (tmp - tmp2) - 0xFAA0CC4;

	_pDst->dwValue3.dwIndex = m_ConstShuffle[13] + m_VarShuffle + tmp2;
}

void Protocol70::AlgorithmShuffle_31(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	DWORD tmp;

	tmp = (_pSrc->dwValue1 - m_ConstShuffle[9]) - 0x34352DF0;

	m_VarShuffle = _pSrc->dwValue3.dwIndex + 0x336B2402 + 0x682432E8 + tmp;

	_pDst->dwValue3.dwIndex = m_ConstShuffleArray[0x45 + ((_pGGVer->wNum * _pGGVer->wDay + 0x56) % 10 * 100)] + m_ConstShuffle[16] + m_VarShuffle + (_pSrc->dwValue3.dwIndex + 0x336B2402) + tmp + 0x336B2850;
}

void Protocol70::AlgorithmShuffle_32(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	DWORD tmp;

	tmp = _pSrc->dwValue1 + 0x4905F430;

	m_VarShuffle = m_ConstShuffle[7] + tmp;

	_pDst->dwValue3.dwIndex = m_ConstShuffleArray[0x4A + ((_pGGVer->wNum * _pGGVer->wDay + 0x5B) % 10 * 100)]
			+ (m_ConstShuffle[12] ^ _pSrc->dwValue2.dwIndex + 0xE645533 + m_ConstShuffle[2]) + m_VarShuffle + 0x5C57EEBC + tmp;
}

void Protocol70::AlgorithmShuffle_33(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	DWORD tmp, tmp2;

	tmp = _pSrc->dwValue1 ^ m_ConstShuffle[9] ^ 0xBD3482D4;
	tmp2 = _pSrc->dwValue3.dwIndex ^ 0x48DB880D;

	m_VarShuffle = (tmp - tmp2) + 0x2D3E495C;

	_pDst->dwValue3.dwIndex = m_ConstShuffleArray[0x1 + ((_pGGVer->wNum * _pGGVer->wDay + 0x12) % 10 * 100)] + (m_VarShuffle ^ 0x48DB880D) + m_ConstShuffle[16] + tmp2 + 0x44E + tmp;
}

void Protocol70::AlgorithmShuffle_34(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	GG_AUTH_DATA lSrc{ 0 }, lDst{ 0 };
	DWORD tmp, tmp2, tmp3;

	lSrc.dwIndex.dwIndex = 0x2C;
	lSrc.dwValue1 = _pSrc->dwValue1;
	lSrc.dwValue2.dwIndex = _pSrc->dwValue2.dwIndex;
	lSrc.dwValue3.dwIndex = _pSrc->dwValue3.dwIndex;

	AlgorithmShuffle_44(&lSrc, &lDst, _pGGVer);

	tmp3 = _pSrc->dwValue3.dwIndex ^ 0x98872066;

	m_VarShuffle = m_ConstShuffle[1] + tmp3;

	tmp2 = (m_ConstShuffle[8] + 0x9378A51F + _pSrc->dwValue2.dwIndex) ^ m_ConstShuffle[15];

	tmp = m_ConstShuffleArray[((_pGGVer->wNum * _pGGVer->wDay + 0x11) % 10 * 100)];

	tmp2 = tmp2 ^ lDst.dwValue1;

	_pDst->dwValue3.dwIndex = (lDst.dwValue3.dwIndex ^ tmp3 + tmp) + tmp2 + (m_ConstShuffle[2] ^ tmp2);
}

void Protocol70::AlgorithmShuffle_35(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	DWORD tmp, tmp2, tmp3;

	tmp = _pSrc->dwValue1 ^ m_ConstShuffle[6];
	tmp3 = (_pSrc->dwValue2.dwIndex ^ 0x95742B6A) + 0x22AAD4D6;
	tmp2 = tmp - 0x526DF66A;

	tmp = m_ConstShuffleArray[0x4A + ((_pGGVer->wNum * _pGGVer->wDay + 0x5B) % 10 * 100)] + (m_ConstShuffle[3] ^ tmp3 + tmp ^ _pSrc->dwValue3.dwIndex);

	m_VarShuffle = tmp ^ tmp3 ^ 0x3554D6A1;

	_pDst->dwValue3.dwIndex = (m_VarShuffle ^ tmp2) + m_ConstShuffle[13] + tmp + tmp2;
}

void Protocol70::AlgorithmShuffle_36(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	DWORD tmp;

	tmp = _pSrc->dwValue1 + 0x34FF0159;

	m_VarShuffle = m_ConstShuffle[7] + tmp;

	_pDst->dwValue3.dwIndex = m_ConstShuffleArray[0xD + ((_pGGVer->wNum * _pGGVer->wDay + 0x1E) % 10 * 100)]
			+ (m_ConstShuffle[12] ^ _pSrc->dwValue2.dwIndex + 0x11B2EEFA + m_ConstShuffle[2]) + m_VarShuffle + 0x32F8D58F + tmp;
}

void Protocol70::AlgorithmShuffle_37(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	DWORD tmp, tmp2;

	tmp = (_pSrc->dwValue1 - m_ConstShuffle[9]) - 0x7FA95B7B;
	tmp2 = _pSrc->dwValue3.dwIndex ^ 0xE9B03451;
	m_VarShuffle = (tmp - tmp2) - 0x127CF73F;

	_pDst->dwValue3.dwIndex = m_ConstShuffleArray[0x28 + ((_pGGVer->wNum * _pGGVer->wDay + 0x39) % 10 * 100)]
			+ (m_VarShuffle ^ 0xE9B03451) + m_ConstShuffle[16] + tmp2 + 0x44E + tmp;
}

void Protocol70::AlgorithmShuffle_38(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	DWORD tmp, tmp2;

	tmp = _pSrc->dwValue2.dwIndex - 0x26925F3;
	tmp2 = (tmp + (_pSrc->dwValue1 - m_ConstShuffle[6]) ^ _pSrc->dwValue3.dwIndex) + (m_ConstShuffleArray[0x3D + ((_pGGVer->wNum * _pGGVer->wDay + 0x4E) % 10 * 100)] - m_ConstShuffle[3]);

	m_VarShuffle = (tmp - tmp2) - 0x6228B8DC;

	_pDst->dwValue3.dwIndex = m_ConstShuffle[13] + m_VarShuffle + tmp2;
}

void Protocol70::AlgorithmShuffle_39(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	_pDst->dwValue3.dwIndex = (m_ConstShuffle[7] + 0x17BDBEFA ^ m_ConstShuffle[14]) + m_ConstShuffle[10] + 0x11BB52DF + m_ConstShuffle[5];
}

void Protocol70::AlgorithmShuffle_40(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	_pDst->dwValue3.dwIndex = ((m_ConstShuffle[7] + 0xD34BDE55) ^ m_ConstShuffle[14]) + m_ConstShuffle[10] + 0xA7292D81 + m_ConstShuffle[5];
}

void Protocol70::AlgorithmShuffle_41(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	DWORD tmp;

	tmp = _pSrc->dwValue1 - 0x4DF71DDE;

	m_VarShuffle = m_ConstShuffle[7] + tmp;

	_pDst->dwValue3.dwIndex = m_ConstShuffleArray[0x4 + ((_pGGVer->wNum * _pGGVer->wDay + 0x15) % 10 * 100)] 
			+ (m_ConstShuffle[12] ^ _pSrc->dwValue2.dwIndex + 0x711E9F67 + m_ConstShuffle[2]) + m_VarShuffle + 0x20FEBF6C + tmp;
}

void Protocol70::AlgorithmShuffle_42(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	m_VarShuffle = m_ConstShuffleArray[0x3F + ((_pGGVer->wNum * _pGGVer->wDay + 0x50) % 10 * 100)] + (_pSrc->dwValue2.dwIndex ^ m_ConstShuffle[0] + 0xCDC2C7B0) + m_ConstShuffle[4];

	_pDst->dwValue3.dwIndex = m_VarShuffle + (_pSrc->dwValue3.dwIndex + 0x78763BB2 ^ m_ConstShuffle[11]) + 0xC11D10D6 + m_ConstShuffle[0] + 0xCDC2C7B0;
}

void Protocol70::AlgorithmShuffle_43(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	DWORD tmp, tmp2;

	tmp = _pSrc->dwValue1 ^ m_ConstShuffle[9] ^ 0x13214BF2;
	tmp2 = _pSrc->dwValue3.dwIndex - 0xA7FCDC1;

	m_VarShuffle = (tmp - tmp2) - 0x4E75B6CF;

	_pDst->dwValue3.dwIndex = m_ConstShuffleArray[0x4C + ((_pGGVer->wNum * _pGGVer->wDay + 0x5D) % 10 * 100)] + m_ConstShuffle[16] + m_VarShuffle + tmp2 + 0xF580368D + tmp;
}

void Protocol70::AlgorithmShuffle_44(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	_pDst->dwValue3.dwIndex = (m_ConstShuffle[7] + 0x37D1701D ^ m_ConstShuffle[14]) + m_ConstShuffle[10] + 0x560B19D0 + m_ConstShuffle[5];
}

void Protocol70::AlgorithmShuffle_45(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	GG_AUTH_DATA lSrc{ 0 }, lDst{ 0 };
	DWORD tmp, tmp2, tmp3;

	lSrc.dwIndex.dwIndex = 5;
	lSrc.dwValue1 = _pSrc->dwValue1;
	lSrc.dwValue2.dwIndex = _pSrc->dwValue2.dwIndex;
	lSrc.dwValue3.dwIndex = _pSrc->dwValue3.dwIndex;

	AlgorithmShuffle_5(&lSrc, &lDst, _pGGVer);

	tmp3 = _pSrc->dwValue3.dwIndex - 0x679BA432;

	m_VarShuffle = m_ConstShuffle[1] + tmp3;

	tmp2 = m_ConstShuffle[8] + 0x8E1D5C39 + _pSrc->dwValue2.dwIndex ^ m_ConstShuffle[15];

	tmp = m_ConstShuffleArray[0x3B + ((_pGGVer->wNum * _pGGVer->wDay + 0x4C) % 10 * 100)];

	tmp2 = tmp2 ^ lDst.dwValue1;

	_pDst->dwValue3.dwIndex = (lDst.dwValue3.dwIndex ^ tmp3 + tmp) + tmp2 + (m_ConstShuffle[2] ^ tmp2);
}

void Protocol70::AlgorithmShuffle_46(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	DWORD tmp, tmp2;

	tmp2 = _pSrc->dwValue3.dwIndex ^ 0x38F682DC;
	tmp = (_pSrc->dwValue1 - m_ConstShuffle[9]) - 0x5F5CAE64;

	m_VarShuffle = (tmp2 ^ tmp) + 0x32C5ECED;

	_pDst->dwValue3.dwIndex = m_ConstShuffleArray[0x48 + ((_pGGVer->wNum * _pGGVer->wDay + 0x59) % 10 * 100)] 
			+ (m_VarShuffle ^ 0x38F682DC) + m_ConstShuffle[16] + tmp2 + 0x44E + tmp;
}

void Protocol70::AlgorithmShuffle_47(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	GG_AUTH_DATA lSrc{ 0 }, lDst{ 0 };
	DWORD tmp, tmp2, tmp3;

	lSrc.dwIndex.dwIndex = 0x27;
	lSrc.dwValue1 = _pSrc->dwValue1;
	lSrc.dwValue2.dwIndex = _pSrc->dwValue2.dwIndex;
	lSrc.dwValue3.dwIndex = _pSrc->dwValue3.dwIndex;

	AlgorithmShuffle_39(&lSrc, &lDst, _pGGVer);

	tmp3 = (_pSrc->dwValue3.dwIndex - 0xAC25AFB) ^ 0xD3EBA94B;

	m_VarShuffle = m_ConstShuffle[1] + tmp3;

	tmp2 = (_pSrc->dwValue2.dwIndex ^ 0xCD7E1E49) + m_ConstShuffle[8] ^ m_ConstShuffle[15];

	tmp = m_ConstShuffleArray[0x20 + ((_pGGVer->wNum * _pGGVer->wDay + 0x31) % 10 * 100)];

	tmp2 = tmp2 ^ lDst.dwValue1;

	_pDst->dwValue3.dwIndex = (lDst.dwValue3.dwIndex ^ tmp3 + tmp) + tmp2 + (m_ConstShuffle[2] ^ tmp2);
}

void Protocol70::AlgorithmShuffle_48(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	DWORD tmp, tmp2;

	tmp = _pSrc->dwValue2.dwIndex + 0x6D610501;
	tmp2 = ((_pSrc->dwValue1 - m_ConstShuffle[6]) - tmp ^ _pSrc->dwValue3.dwIndex) + (m_ConstShuffleArray[0x39 + ((_pGGVer->wNum * _pGGVer->wDay + 0x4A) % 10 * 100)] - m_ConstShuffle[3]);

	m_VarShuffle = (tmp - tmp2) + 0x2CCFDAF0;

	_pDst->dwValue3.dwIndex = m_ConstShuffle[13] + m_VarShuffle + tmp2;
}

void Protocol70::AlgorithmShuffle_49(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	DWORD tmp, tmp2;

	tmp = _pSrc->dwValue1 + 0xC77FB948 + m_ConstShuffle[9];
	tmp2 = _pSrc->dwValue3.dwIndex ^ 0xF795D1AA;

	m_VarShuffle = tmp2 + 0x222AFB30 + tmp;

	_pDst->dwValue3.dwIndex = m_ConstShuffleArray[0x32 + ((_pGGVer->wNum * _pGGVer->wDay + 0x43) % 10 * 100)] 
			+ (m_VarShuffle ^ 0xF795D1AA) + m_ConstShuffle[16] + tmp2 + 0x44E + tmp;
}

void Protocol70::AlgorithmShuffle_50(GG_AUTH_DATA *_pSrc, GG_AUTH_DATA *_pDst, GG_VERSION *_pGGVer) {

	m_VarShuffle = m_ConstShuffleArray[0x4F + ((_pGGVer->wNum * _pGGVer->wDay + 0x60) % 10 * 100)] + _pSrc->dwValue2.dwIndex + m_ConstShuffle[4] + (m_ConstShuffle[0] ^ 0x435BB3E7);

	_pDst->dwValue3.dwIndex = (m_VarShuffle ^ 0x852D1885) + ((_pSrc->dwValue3.dwIndex + 0x8680B83) ^ m_ConstShuffle[11]) + (m_ConstShuffle[0] ^ 0x435BB3E7);
}
