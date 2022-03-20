// Arquivo cmd_login_reward_info.hpp
// Criado em 27/10/2020 as 18:47 por Acrisio
// Defini��o da classe CmdLoginRewardInfo

#pragma once
#ifndef _STDA_CMD_LOGIN_REWARD_INFO_HPP
#define _STDA_CMD_LOGIN_REWARD_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/login_reward_type.hpp"
#include <vector>

namespace stdA {

	class CmdLoginRewardInfo : public pangya_db {

		public:
			CmdLoginRewardInfo(bool _waiter = false);
			virtual ~CmdLoginRewardInfo();

			std::vector< stLoginReward >& getInfo();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			virtual std::string _getName() override { return "CmdLoginRewardInfo"; };
			virtual std::wstring _wgetName() override { return L"CmdLoginRewardInfo"; };

		private:
			std::vector< stLoginReward > m_lr;

			const char* m_szConsulta = "(SELECT " DB_MAKE_ESCAPE_KEYWORD_A("index") "\
												, " DB_MAKE_ESCAPE_KEYWORD_A("name") "\
												, " DB_MAKE_ESCAPE_KEYWORD_A("type") "\
												, days_to_gift\
												, n_times_gift\
												, item_typeid\
												, item_qntd\
												, item_qntd_time\
												, is_end\
												, end_date\
										FROM pangya.pangya_login_reward WHERE is_end = 0)";
	};
}

#endif // !_STDA_CMD_LOGIN_REWARD_INFO_HPP
