// Arquivo cmd_old_daily_quest_info.hpp
// Criado em 24/03/2018 as 20:50 por Acrisio
// Defini��o da classe CmdOldDailyQuestInfo

#pragma once
#ifndef _STDA_CMD_OLD_DAILY_QUEST_INFO_HPP
#define _STDA_CMD_OLD_DAILY_QUEST_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"
#include <vector>

namespace stdA {
	class CmdOldDailyQuestInfo : public pangya_db {
		public:
			explicit CmdOldDailyQuestInfo(bool _waiter = false);
			CmdOldDailyQuestInfo(uint32_t _uid, bool _waiter = false);
			virtual ~CmdOldDailyQuestInfo();

			std::vector< RemoveDailyQuestUser >& getInfo();

			uint32_t getUID();
			void setUID(uint32_t _uid);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdOldDailyQuestInfo"; };
			virtual std::wstring _wgetName() override { return L"CmdOldDailyQuestInfo"; };

		private:
			uint32_t m_uid;
			std::vector< RemoveDailyQuestUser > v_rdqu;

			const char* m_szConsulta = "SELECT ID_ACHIEVEMENT, typeid FROM pangya.pangya_achievement WHERE status = 1 AND UID = ";
	};
}

#endif // !_STDA_CMD_OLD_DAILY_QUEST_INFO_HPP
