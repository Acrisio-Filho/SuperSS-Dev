// Arquivo cmd_update_daily_quest.hpp
// Criado em 13/04/2018 as 18:36 por Acrisio
// Defini��o da classe CmdUpdateDailyQuest

#pragma once
#ifndef _STDA_CMD_UPDATE_DAILY_QUEST_HPP
#define _STDA_CMD_UPDATE_DAILY_QUEST_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdUpdateDailyQuest : public pangya_db {
		public:
			explicit CmdUpdateDailyQuest(bool _waiter = false);
			CmdUpdateDailyQuest(DailyQuestInfo& _dqi, bool _waiter = false);
			virtual ~CmdUpdateDailyQuest();

			DailyQuestInfo& getInfo();
			void setInfo(DailyQuestInfo& _dqi);

			bool isUpdated();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdUpdateDailyQuest"; };
			virtual std::wstring _wgetName() override { return L"CmdUpdateDailyQuest"; };

		private:
			DailyQuestInfo m_dqi;
			bool m_updated;					// true atualizou no DB, false, outro j� atualizou pega o valor do DB

			const char* m_szConsulta = "pangya.ProcUpdateDailyQuest";
	};
}

#endif // !_STDA_CMD_UPDATE_DAILY_QUEST_HPP
