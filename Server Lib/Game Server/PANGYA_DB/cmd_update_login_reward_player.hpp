// Arquivo cmd_update_login_reward_player.hpp
// Criado em 27/10/2020 as 20:11 por Acrisio
// Defini��o da classe CmdUpdateLoginRewardPlayer

#pragma once
#ifndef _STDA_CMD_UPDATE_LOGIN_REWARD_PLAYER_HPP
#define _STDA_CMD_UPDATE_LOGIN_REWARD_PLAYER_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/login_reward_type.hpp"

namespace stdA {

	class CmdUpdateLoginRewardPlayer : public pangya_db {

		public:
			CmdUpdateLoginRewardPlayer(stPlayerState& _ps, bool _waiter = false);
			explicit CmdUpdateLoginRewardPlayer(bool _waiter = false);
			virtual ~CmdUpdateLoginRewardPlayer();

			stPlayerState& getPlayerState();
			void setPlayerState(stPlayerState& _ps);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			virtual std::string _getName() override { return "CmdUpdateLoginRewardPlayer"; };
			virtual std::wstring _wgetName() override { return L"CmdUpdateLoginRewardPlayer"; };

		private:
			stPlayerState m_ps;

			const char* m_szConsulta = "pangya.procUpdateLoginRewardPlayer";
	};
}

#endif // !_STDA_CMD_UPDATE_LOGIN_REWARD_PLAYER_HPP
