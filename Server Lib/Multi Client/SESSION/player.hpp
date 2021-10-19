// Arquivo player.hpp
// Criado em 22/10/2017 por Acrisio
// Definição da classe player

#pragma once
#ifndef _STDA_CLIENTE_HPP
#define _STDA_CLIENTE_HPP

#include "../TYPE/client_info.h"
#include "../../Projeto IOCP/SOCKET/session.h"
#include "../../Projeto IOCP/THREAD POOL/threadpool_base.hpp"

namespace stdA {
	class player : public session {
		public:
			player(threadpool_base& _threapool);
			virtual ~player();

			virtual unsigned char getStateLogged() override;

			virtual unsigned long getUID() override;
			virtual unsigned long getCapability() override;
			virtual char* getNickname() override;
			virtual char* getID() override;

		public:
			ClientInfo m_ci;
	};
}

#endif // !_STDA_CLIENTE_HPP
