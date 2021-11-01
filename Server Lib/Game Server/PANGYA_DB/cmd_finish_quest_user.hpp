// Arquivo cmd_finish_quest_user.hpp
// Criado em 15/04/2018 as 23:36 por Acrisio
// Defini��o da classe CmdFinishQuestUser

#pragma once
#ifndef _STDA_CMD_FINISH_QUEST_USER_HPP
#define _STDA_CMD_FINISH_QUEST_USER_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdFinishQuestUser : public pangya_db {
		public:
			explicit CmdFinishQuestUser(bool _waiter = false);
			CmdFinishQuestUser(uint32_t _uid, bool _waiter = false);
			CmdFinishQuestUser(uint32_t _uid, QuestStuffInfo& _qsi, bool _waiter = false);
			virtual ~CmdFinishQuestUser();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			QuestStuffInfo& getInfo();
			void setInfo(QuestStuffInfo& _qsi);

			uint32_t getFinishDateSecond();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdFinishQuestUser"; };
			virtual std::wstring _wgetName() override { return L"CmdFinishQuestUser"; };

		private:
			uint32_t m_uid;
			uint32_t m_finish_date_second;
			QuestStuffInfo m_qsi;

			const char* m_szConsulta = "pangya.ProcFinishQuestUser";
	};
}

#endif // !_STDA_CMD_FINISH_QUEST_USER_HPP
