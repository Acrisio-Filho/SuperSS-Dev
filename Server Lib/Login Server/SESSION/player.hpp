// Arquivo player.hpp
// Criado em 11/03/2018 as 14:00 por Acrisio
// Definição da classe player

#pragma once
#ifndef _STDA_PLAYER_HPP
#define _STDA_PLAYER_HPP

#include "../../Projeto IOCP/SOCKET/session.h"
#include "../TYPE/player_info.hpp"
#include "../../Projeto IOCP/THREAD POOL/threadpool_base.hpp"

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

#endif