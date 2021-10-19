// Arquivo cmd_daily_quest_info_user.hpp
// Criado em 24/03/2018 as 19:51 por Acrisio
// Defini��o da classe CmdDailyQuestInfoUser

#pragma once
#ifndef _STDA_CMD_DAILY_QUEST_INFO_USER_HPP
#define _STDA_CMD_DAILY_QUEST_INFO_USER_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdDailyQuestInfoUser : public pangya_db {
		public:
			enum TYPE : unsigned char {
				GET,
				CHECK,
			};

		public:
			explicit CmdDailyQuestInfoUser(bool _waiter = false);
			CmdDailyQuestInfoUser(uint32_t _uid, TYPE _type, bool _waiter = false);
			virtual ~CmdDailyQuestInfoUser();

			DailyQuestInfoUser& getInfo();
			void setInfo(DailyQuestInfoUser& _dqiu);

			bool check();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			TYPE getType();
			void setType(TYPE _type);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdDailyQuestInfoUser"; };
			virtual std::wstring _wgetName() override { return L"CmdDailyQuestInfoUser"; };

		private:
			uint32_t m_uid;
			TYPE m_type;
			bool m_ok;
			DailyQuestInfoUser m_dqiu;

			const char* m_szConsulta[2] = { "pangya.ProcGetDailyQuest_New", "pangya.ProcCheckPlayerDailyQuest" };
	};
}

#endif // !_STDA_CMD_DAILY_QUEST_INFO_USER_HPP
