// Arquivo player.hpp
// Criado em 15/06/2020 as 14:54 por Acrisio
// Defini��o da classe player

#pragma once
#ifndef _STDA_PLAYER_HPP
#define _STDA_PLAYER_HPP

#include "../../Projeto IOCP/SOCKET/session.h"
#include "../../Projeto IOCP/THREAD POOL/threadpool_base.hpp"

#include "../TYPE/player_info.hpp"

namespace stdA {
	class player : public session {
		public:
			player(threadpool_base& _threadpool);
			virtual ~player();

			virtual bool clear() override;

			virtual unsigned char getStateLogged() override;

			virtual uint32_t getUID() override;
			virtual uint32_t getCapability() override;
			virtual char* getNickname() override;
			virtual char* getID() override;

		public:
			PlayerInfo m_pi;
	};
}
#endif // !_STDA_PLAYER_HPP
