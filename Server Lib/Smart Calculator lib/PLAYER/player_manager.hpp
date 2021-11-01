// Arquivo player_manager.hpp
// Criado em 21/11/2020 as 13:31 por Acrisio
// Defini��o da classe PlayerManager

#pragma once
#ifndef _STDA_PLAYER_MANAGER_HPP
#define _STDA_PLAYER_MANAGER_HPP

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#include <pthread.h>
#endif

#include "../../Projeto IOCP/TYPE/smart_calculator_type.hpp"
#include "../../Projeto IOCP/TYPE/singleton.h"

#include <map>

namespace stdA {

	class PlayerManager {
		public:
			PlayerManager();
			virtual ~PlayerManager();

			stContext* makePlayerCtx(const uint32_t _uid, eTYPE_CALCULATOR_CMD _type);
			stContext* getPlayerCtx(const uint32_t _uid, eTYPE_CALCULATOR_CMD _type);
			void removeAllPlayerCtx(const uint32_t _uid);

		protected:
			void clearPlayersContext();

		protected:
			std::map< stKeyPlayer, stContext > m_player_ctx;

#if defined(_WIN32)
			CRITICAL_SECTION m_cs;
#elif defined(__linux__)
			pthread_mutex_t m_cs;
#endif
	};

	typedef Singleton< PlayerManager > sPlayerManager;
}

#endif // !_STDA_PLAYER_MANAGER_HPP
