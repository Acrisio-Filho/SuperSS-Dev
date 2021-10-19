// Arquivo player_manager.cpp
// Criado em 21/11/2020 as 13:33 Por Acrisio
// Implementa��o da classe PlayerManager

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "player_manager.hpp"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../UTIL/logger.hpp"

#include "../TYPE/smart_calculator_player.hpp"
#include "../TYPE/stadium_calculator_player.hpp"

#include <algorithm>

using namespace stdA;

PlayerManager::PlayerManager() : m_player_ctx() {

#if defined(_WIN32)
	InitializeCriticalSection(&m_cs);
#elif defined(__linux__)
	INIT_PTHREAD_MUTEXATTR_RECURSIVE;
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif
}

PlayerManager::~PlayerManager() {

	try {
		clearPlayersContext();
	}catch (exception& e) {
		// Garantir para n�o lan�ar exception no destructor
		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}

#if defined(_WIN32)
	DeleteCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_destroy(&m_cs);
#endif
}

void PlayerManager::clearPlayersContext() {

	try {

#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		if (!m_player_ctx.empty()) {

			for (auto& el : m_player_ctx)
				if (el.second.getPlayer() != nullptr)
					delete el.second.getPlayer();

			m_player_ctx.clear();
		}

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

	}catch (exception& e) {

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}

stContext* PlayerManager::makePlayerCtx(const uint32_t _uid, eTYPE_CALCULATOR_CMD _type) {
	
	stContext* ctx = nullptr;

	try {

#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		auto it = m_player_ctx.find(stKeyPlayer{
			_uid,
			_type
		});

		if (it == m_player_ctx.end()) {

			auto it_r = m_player_ctx.insert({
				stKeyPlayer{
					_uid,
					_type
				},
				stContext(
					(_type == eTYPE_CALCULATOR_CMD::SMART_CALCULATOR)
						? reinterpret_cast< ICalculatorPlayer* >(new SmartCalculatorPlayer(_uid))
						: reinterpret_cast< ICalculatorPlayer* >(new StadiumCalculatorPlayer(_uid))
				)
			});

			if (!it_r.second && it_r.first == m_player_ctx.end())
				return ctx;

			it = it_r.first;
		}

		ctx = &it->second;

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif


	}catch (exception& e) {

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}

	return ctx;
}

stContext* PlayerManager::getPlayerCtx(const uint32_t _uid, eTYPE_CALCULATOR_CMD _type) {
	
	stContext* ctx = nullptr;

	try {

#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		auto it = m_player_ctx.find(stKeyPlayer{
			_uid,
			_type
		});

		if (it != m_player_ctx.end())
			ctx = &it->second;

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

	}catch (exception& e) {

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
	
	return ctx;
}

void PlayerManager::removeAllPlayerCtx(const uint32_t _uid) {

	try {

#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		auto it = m_player_ctx.end();

		do {

			it = std::find_if(m_player_ctx.begin(), m_player_ctx.end(), [_uid](auto& _el) {
				return _el.second.getPlayer()->m_id == _uid;
			});

			if (it != m_player_ctx.end())
				m_player_ctx.erase(it);

		} while (it != m_player_ctx.end());

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

	}catch (exception& e) {

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		sLogger::getInstance().Log("[" + std::string(__FUNCTION__) + "][ErrorSystem] " + e.getFullMessageError(), eTYPE_LOG::TL_ERROR);
	}
}
