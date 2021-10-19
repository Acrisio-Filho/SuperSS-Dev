// Arquivo player_manager.hpp
// Criado em 02/12/2018 as 13:04 por Acrisio
// Defini��o da classe player_manager

#pragma once
#ifndef _STDA_PLAYER_MANAGER_HPP
#define _STDA_PLAYER_MANAGER_HPP

#include <vector>
#include "player.hpp"
#include "../../Projeto IOCP/SOCKET/session_manager.hpp"

namespace stdA {
	class player_manager : public session_manager {
		public:
			player_manager(threadpool& _threadpool, uint32_t _max_session);
			virtual ~player_manager();

			//virtual void clear() override;

			virtual std::vector< player* > getAllPlayer();

			virtual player *findPlayer(uint32_t _uid, bool _oid = false);
			virtual std::vector< player* > findPlayerByType(uint32_t _type);
			virtual std::vector< player* > findPlayerByTypeExcludeUID(uint32_t _type, uint32_t _uid);
	};
}

#endif // !_STDA_PLAYER_MANAGER_HPP
