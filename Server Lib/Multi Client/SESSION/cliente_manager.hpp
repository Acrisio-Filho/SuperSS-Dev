// Arquivo cliente_manager.hpp
// Criado em 01/04/2018 as 23:36 por Acrisio
// Definição da classe ClienteManager

#pragma once
#ifndef _STDA_CLIENTE_MANAGER_HPP
#define _STDA_CLIENTE_MANAGER_HPP

#include <Windows.h>
#include "../../Projeto IOCP/SOCKET/session_manager.hpp"
#include "../SESSION/player.hpp"
#include <vector>

namespace stdA {
	class ClienteManager : public session_manager {
		public:
			ClienteManager(threadpool& _threadpool, unsigned long _max_session, unsigned long _TTL);
			~ClienteManager();

			virtual void clear();

			virtual player *findCliente(unsigned long _uid, bool _oid = false);

			virtual std::vector< player* > getChannelClientes(unsigned char _channel = ~0, unsigned char _lobby = ~0);
	};
}

#endif // !_STDA_CLIENTE_MANAGER_HPP
