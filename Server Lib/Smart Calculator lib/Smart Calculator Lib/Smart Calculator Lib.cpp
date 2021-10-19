// Smart Calculator Lib.cpp : Define as funções exportadas para a DLL.
//

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include "pch.h"
#include "framework.h"
#endif

#include "Smart Calculator Lib.h"

#include "../TYPE/smart_calculator_player.hpp"
#include "../TYPE/stadium_calculator_player.hpp"

#include "../PLAYER/player_manager.hpp"
#include "../SOCKET/pool.hpp"

using namespace stdA;

SMARTCALCULATORLIB_API stContext* makePlayerContext(uint32_t _uid, eTYPE_CALCULATOR_CMD _type) {
	return sPlayerManager::getInstance().makePlayerCtx(_uid, _type);
}

SMARTCALCULATORLIB_API stContext* getPlayerContext(uint32_t _uid, eTYPE_CALCULATOR_CMD _type) {
	return sPlayerManager::getInstance().getPlayerCtx(_uid, _type);
}

SMARTCALCULATORLIB_API void removeAllPlayerContext(uint32_t _uid) {
	sPlayerManager::getInstance().removeAllPlayerCtx(_uid);
}

SMARTCALCULATORLIB_API int initSmartCalculatorLib(const uint32_t _server_id) {
    return sPool::getInstance().initialize(scLog, responseCallBack, _server_id);
}

SMARTCALCULATORLIB_API void sendCommandServer(const char* _cmd) {

	if (_cmd != nullptr)
		sPool::getInstance().sendServerCmd(_cmd);
}

SMARTCALCULATORLIB_API void scLog(const char* _log, const stdA::eTYPE_LOG _type) {
	UNREFERENCED_PARAMETER(_log);
	UNREFERENCED_PARAMETER(_type);
}

SMARTCALCULATORLIB_API void responseCallBack(const uint32_t _id, const eTYPE_CALCULATOR_CMD _type, const char* _response, const eTYPE_RESPONSE _server) {
	UNREFERENCED_PARAMETER(_id);
	UNREFERENCED_PARAMETER(_type);
	UNREFERENCED_PARAMETER(_response);
	UNREFERENCED_PARAMETER(_server); // é o server == true, player == false
}
