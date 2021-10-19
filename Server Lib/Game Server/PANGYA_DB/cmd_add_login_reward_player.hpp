// Arquivo cmd_add_login_reward_player.hpp
// Criado em 27/10/2020 as 19:24 por Acrisio
// Defini��o da classe CmdAddLoginRewardPlayer

#pragma once
#ifndef _STDA_CMD_ADD_LOGIN_REWARD_PLAYER_HPP
#define _STDA_CMD_ADD_LOGIN_REWARD_PLAYER_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/login_reward_type.hpp"

namespace stdA {

	class CmdAddLoginRewardPlayer : public pangya_db {

		public:
			CmdAddLoginRewardPlayer(uint64_t _id, stPlayerState& _ps, bool _waiter = false);
			CmdAddLoginRewardPlayer(bool _waiter = false);
			virtual ~CmdAddLoginRewardPlayer();

			uint64_t getId();
			void setId(uint64_t _id);

			stPlayerState& getPlayerState();
			void setPlayerState(stPlayerState& _ps);

			bool isGood();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			virtual std::string _getName() override { return "CmdAddLoginRewardPlayer"; };
			virtual std::wstring _wgetName() override { return L"CmdAddLoginRewardPlayer"; };

		private:
			uint64_t m_id;
			stPlayerState m_ps;

			const char* m_szConsulta = "pangya.ProcInsertLoginRewardPlayer";
	};
}

#endif // !_STDA_CMD_ADD_LOGIN_REWARD_PLAYER_HPP
