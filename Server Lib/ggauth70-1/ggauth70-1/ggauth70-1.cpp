// ggauth70-1.cpp : Define as funções exportadas para a DLL.
//

#if defined(_WIN32)
#include <Windows.h>
#endif

#include "ggauth70-1.h"
#include "Protocol70.hpp"

UINT32 PrtcCheckAuthAnswer(UINT32 *_pUserFlag, GG_VERSION *_pGGVer, GG_AUTH_DATA *_pAuthAnswer, GG_AUTH_DATA *_pAuthChecked, UINT32 _nSequenceNum, UINT32 *_pLastValue4) {
	return Protocol70::checkAuthAnswer(_pUserFlag, _pGGVer, _pAuthAnswer, _pAuthChecked, _nSequenceNum, _pLastValue4);
}

UINT32 PrtcCheckValidAnswer(GG_AUTH_DATA *_pAuthAnswer, UINT32 *_pReturnFlag) {
	return Protocol70::checkValidAnswer(_pAuthAnswer, _pReturnFlag);
}

void PrtcDecryptAnswer(GG_AUTH_DATA *_pAuthAnswer) {
	Protocol70::decryptAnswer(_pAuthAnswer);
}

UINT32 PrtcEncryptQuery(GG_AUTH_DATA *_pAuthQuery) {
	return Protocol70::encryptQuery(_pAuthQuery);
}

UINT32 PrtcGetAuthQuery(UINT32 *_pUserFlag, GG_VERSION *_pGGVer, CCSAuth2 *_pCCSAuth2, DWORD _nSequenceNum, DWORD _dwServerKey) {
	return Protocol70::getAuthQuery(_pUserFlag, _pGGVer, _pCCSAuth2, _nSequenceNum, _dwServerKey);
}

UINT32 PrtcGetInformation(UINT32 _type, GG_AUTH_DATA *_pAuthAnswer){
	return Protocol70::getInformation(_type, _pAuthAnswer);
}

UINT32 PrtcGetVersion(BOOL _bUseTimer) {

	if (_bUseTimer == FALSE && Protocol70::setTimer() == 0)
		return 0;

#if defined(_WIN32)
	Protocol70::UpdateTimer(NULL, 0, 0, 0);
#elif defined(__linux__)
	Protocol70::UpdateTimer(nullptr, nullptr);
#endif
	Protocol70::InitializeVersionDll();

	return Protocol70::getVersion();
}

UINT32 PrtcLoopAuth(char *_pByLastLoop1, DWORD *_pdwLoop1AuthArray, char *_pByLastLoop2, DWORD *_pdwLoop2AuthArray, GG_AUTH_DATA *_pAuthAnswer, UINT32 _nSequenceNum) {
	return Protocol70::loopAuth((unsigned char*)_pByLastLoop1, _pdwLoop1AuthArray, (unsigned char*)_pByLastLoop2, _pdwLoop2AuthArray, _pAuthAnswer, _nSequenceNum);
}

UINT32 PrtcUpdateTimer(void) {
#if defined(_WIN32)
	Protocol70::UpdateTimer(NULL, 0, 0, 0);
#elif defined(__linux__)
	Protocol70::UpdateTimer(nullptr, nullptr);
#endif
	return 0;
}
