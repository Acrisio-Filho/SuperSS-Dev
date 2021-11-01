// Arquivo player.hpp
// Criado em 02/02/2021 as 17:15 por Acrisio
// Defini��o da classe player

#pragma once
#ifndef _STDA_PLAYER_HPP
#define _STDA_PLAYER_HPP

#include "../../Projeto IOCP/SOCKET/session.h"
#include "../TYPE/player_info.hpp"
#include "../../Projeto IOCP/THREAD POOL/threadpool_base.hpp"

#include "../TYPE/game_guard_type.hpp"

#include <map>

namespace stdA {

	class player : public session {
		public:
			player(threadpool_base& _threadpool);
			virtual ~player();

			virtual bool clear() override;

			virtual unsigned char getStateLogged() override;

			virtual uint32_t getUID() override;
			virtual uint32_t getCapability() override;	// Tipo
			virtual char* getNickname() override;
			virtual char* getID() override;

			// Game Guard
			void addPlayerToGameGuard(unsigned long _uid);
			void removePlayerToGameGuard(unsigned long _uid);

			PlayerGameGuard* getPlayerGameGuard(unsigned long _uid);

		public:
			PlayerInfo m_pi;

			// Game Guard of players
			std::map< unsigned long/*UID*/, PlayerGameGuard* > m_gg;

	};
}

#endif // !_STDA_PLAYER_HPP
