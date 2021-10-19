// Arquivo cmd_bot_gm_event_info.hpp
// Criado em 03/11/2020 as 20:52 por Acrisio
// Defini��o da classe CmdBotGMEventInfo

#pragma once
#ifndef _STDA_CMD_BOT_GM_EVENT_INFO_HPP
#define _STDA_CMD_BOT_GM_EVENT_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/bot_gm_event_type.hpp"

#include <vector>

namespace stdA {

	class CmdBotGMEventInfo : public pangya_db {

		public:
			CmdBotGMEventInfo(bool _waiter = false);
			virtual ~CmdBotGMEventInfo();

			std::vector< stReward >& getRewardInfo();
			std::vector< stRangeTime >& getTimeInfo();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			virtual std::string _getName() override { return "CmdBotGMEventInfo"; };
			virtual std::wstring _wgetName() override { return L"CmdBotGMEventInfo"; };

		private:
			std::vector< stReward > m_reward;
			std::vector< stRangeTime > m_time;

			const char* m_szConsulta = "SELECT inicio_time, fim_time, channel_id FROM pangya.pangya_bot_gm_event_time WHERE valid = 1; SELECT typeid, qntd, qntd_time, rate FROM pangya.pangya_bot_gm_event_reward WHERE valid = 1";
	};
}

#endif // !_STDA_CMD_BOT_GM_EVENT_INFO_HPP
