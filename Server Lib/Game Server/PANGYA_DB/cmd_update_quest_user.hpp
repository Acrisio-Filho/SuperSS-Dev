// Arquivo cmd_update_quest_user.hpp
// Criado em 14/04/2018 as 15:57 por Acrisio
// Defini��o da classe CmdUpdateQuestUser

#pragma once
#ifndef _STDA_CMD_UPDATE_QUEST_USER_HPP
#define _STDA_CMD_UPDATE_QUEST_USER_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdUpdateQuestUser : public pangya_db {
		public:
			explicit CmdUpdateQuestUser(bool _waiter = false);
			CmdUpdateQuestUser(uint32_t _uid, QuestStuffInfo& _qsi, bool _waiter = false);
			virtual ~CmdUpdateQuestUser();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			QuestStuffInfo& getInfo();
			void setInfo(QuestStuffInfo& _qsi);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdUpdateQuestUser"; };
			virtual std::wstring _wgetName() override { return L"CmdUpdateQuestUser"; };

		private:
			uint32_t m_uid;
			QuestStuffInfo m_qsi;

			const char* m_szConsulta = "pangya.ProcUpdateQuestUser";
	};
}

#endif // !_STDA_CMD_UPDATE_QUEST_USER_HPP
