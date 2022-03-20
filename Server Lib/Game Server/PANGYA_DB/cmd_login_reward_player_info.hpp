// Arquivo cmd_login_reward_player_info.hpp
// Criado em 27/10/2020 as 19:04 por Acrisio
// Defini��o da classe CmdLoginRewardPlayerInfo

#pragma once
#ifndef _STDA_CMD_LOGIN_REWARD_PLAYER_INFO_HPP
#define _STDA_CMD_LOGIN_REWARD_PLAYER_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/login_reward_type.hpp"
#include <map>

namespace stdA {

	class CmdLoginRewardPlayerInfo : public pangya_db {

		public:
			CmdLoginRewardPlayerInfo(uint64_t _id, uint32_t _uid, bool _waiter = false);
			explicit CmdLoginRewardPlayerInfo(uint32_t _uid, bool _waiter = false);
			explicit CmdLoginRewardPlayerInfo(bool _waiter = false);
			virtual ~CmdLoginRewardPlayerInfo();

			uint64_t getId();
			void setId(uint64_t _id);

			uint32_t getPlayerUID();
			void setPlayerUID(uint32_t _uid);

			stPlayerState& getInfo();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			virtual std::string _getName() override { return "CmdLoginRewardPlayerInfo"; };
			virtual std::wstring _wgetName() override { return L"CmdLoginRewardPlayerInfo"; };

		private:
			uint64_t m_id;
			uint32_t m_uid;
			
			stPlayerState m_player;

			const char* m_szConsulta[2] = { 
				"SELECT " DB_MAKE_ESCAPE_KEYWORD_A("index") "\
					,uid\
					,count_days\
					,count_seq\
					,is_clear\
					,update_date\
				FROM pangya.pangya_login_reward_player WHERE login_reward_id = ",
				" AND uid = "
			};
	};
}

#endif // !_STDA_CMD_LOGIN_REWARD_PLAYER_INFO_HPP
