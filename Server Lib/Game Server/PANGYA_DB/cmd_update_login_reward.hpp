// Arquivo cmd_update_login_reward.hpp
// Criado em 27/10/2020 as 19:59 por Acrisio
// Defini��o da classe CmdUpdateLoginReward

#pragma once
#ifndef _STDA_CMD_UPDATE_LOGIN_REWARD_HPP
#define _STDA_CMD_UPDATE_LOGIN_REWARD_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

namespace stdA {

	class CmdUpdateLoginReward : public pangya_db {

		public:
			CmdUpdateLoginReward(uint64_t _id, bool _is_end, bool _waiter = false);
			CmdUpdateLoginReward(bool _waiter = false);
			virtual ~CmdUpdateLoginReward();

			uint64_t getId();
			void setId(uint64_t _id);

			bool getIsEnd();
			void setIsEnd(bool _is_end);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			virtual std::string _getName() override { return "CmdUpdateLoginReward"; };
			virtual std::wstring _wgetName() override { return L"CmdUpdateLoginReward"; };

		private:
			uint64_t m_id;

			bool m_is_end;

			const char* m_szConsulta[2] = {
				"UPDATE pangya.pangya_login_reward SET is_end = ",
				" WHERE index = "
			};
	};
}

#endif // !_STDA_CMD_UPDATE_LOGIN_REWARD_HPP
