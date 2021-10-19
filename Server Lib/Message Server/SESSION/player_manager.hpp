// Arquivo player_manager.hpp
// Criado em 29/07/2018 as 13:44 por Acrisio
// Defini��o da classe player_manager

#pragma once
#ifndef _STDA_PLAYER_MANAGER_HPP
#define _STDA_PLAYER_MANAGER_HPP

#include <vector>
#include "player.hpp"
#include "../../Projeto IOCP/SOCKET/session_manager.hpp"
#include "../TYPE/pangya_message_st.hpp"

namespace stdA {
	class player_manager : public session_manager {
		public:
			player_manager(threadpool& _threadpool, uint32_t _max_session);
			virtual ~player_manager();

			//virtual void clear();

			virtual player *findPlayer(uint32_t _uid, bool _oid = false);
			
			virtual std::vector< player* > findAllGM();

			virtual std::map< uint32_t, player* > findAllFriend(std::vector< FriendInfoEx* > _v_friend);

			// Procura todos players online pelo Guild UID
			virtual std::map< uint32_t, player* > findAllGuildMember(uint32_t _guild_uid);
	};
}

#endif // !_STDA_PLAYER_MANAGER_HPP
