// Arquivo player_manager.hpp
// Criado em 02/02/2021 as 17:23 por Acrisio
// Definição da classe player_manager

#pragma once
#ifndef _STDA_PLAYER_MANAGER_HPP
#define _STDA_PLAYER_MANAGER_HPP

#include <vector>
#include "player.hpp"
#include "../../Projeto IOCP/SOCKET/session_manager.hpp"

namespace stdA {

	class player_manager : public session_manager {
		public:
			player_manager(threadpool& _threadpool, unsigned long _max_session);
			virtual ~player_manager();

			//virtual void clear() override;

			virtual std::vector< player* > getAllPlayer();

			virtual player *findPlayer(unsigned long _uid, bool _oid = false);
			virtual std::vector< player* > findPlayerByType(unsigned long _type);
			virtual std::vector< player* > findPlayerByTypeExcludeUID(unsigned long _type, unsigned long _uid);
	};
}

#endif // !_STDA_PLAYER_MANAGER_HPP
