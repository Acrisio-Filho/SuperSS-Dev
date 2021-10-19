// Arquivo cmd_add_daily_quest.hpp
// Criado em 24/03/2018 as 21:15 por Acrisio
// Defini��o da classe CmdAddDailyQuest

#pragma once
#ifndef _STDA_CMD_ADD_DAILY_QUEST_HPP
#define _STDA_CMD_ADD_DAILY_QUEST_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdAddDailyQuest : public pangya_db {
		public:
			CmdAddDailyQuest(bool _waiter = false);
			CmdAddDailyQuest(uint32_t _uid, bool _waiter = false);
			CmdAddDailyQuest(uint32_t _uid, AddDailyQuestUser& _adqu, bool _waiter = false);
			~CmdAddDailyQuest();

			int32_t getID();
			
			uint32_t getUID();
			void setUID(uint32_t _uid);

			AddDailyQuestUser& getAddDailyQuest();
			void setAddDailyQuest(AddDailyQuestUser& _adq);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdAddDailyQuest"; };
			virtual std::wstring _wgetName() override { return L"CmdAddDailyQuest"; };

		private:
			uint32_t m_uid;
			AddDailyQuestUser m_adqu;
			int32_t m_id;

			const char* m_szConsulta = "pangya.ProcInsertNewAchievement";
	};
}

#endif // !_STDA_CMD_ADD_DAILY_QUEST_HPP
