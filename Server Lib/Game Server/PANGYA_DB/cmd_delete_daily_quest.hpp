// Arquivo cmd_delete_daily_quest.hpp
// Criado em 24/03/2018 as 21:58 por Acrisio
// Defini��o da classe CmdDeleteDailyQuest

#pragma once
#ifndef _STDA_CMD_DELETE_DAILY_QUEST_HPP
#define _STDA_CMD_DELETE_DAILY_QUEST_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"
#include <vector>

namespace stdA {
	class CmdDeleteDailyQuest : public pangya_db {
		public:
			explicit CmdDeleteDailyQuest(bool _waiter = false);
			CmdDeleteDailyQuest(uint32_t _uid, std::vector< RemoveDailyQuestUser >& _rdqu, bool _waiter = false);
			virtual ~CmdDeleteDailyQuest();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			std::vector< RemoveDailyQuestUser >& getDeleteDailyQuest();
			void setDeleteDailyQuest(std::vector< RemoveDailyQuestUser >& _rdqu);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdDeleteDailyQuest"; };
			virtual std::wstring _wgetName() override { return L"CmdDeleteDailyQuest"; };

		private:
			uint32_t m_uid;
			std::vector< RemoveDailyQuestUser > v_rdqu;

			const char* m_szConsulta[5] = { "DELETE FROM pangya.pangya_achievement WHERE uid = ", " AND ID_ACHIEVEMENT IN(", ");DELETE FROM pangya.pangya_quest WHERE uid = ", " AND achievement_id IN(", ")" };
	};
}

#endif // !_STDA_CMD_DELETE_DAILY_QUEST_HPP
