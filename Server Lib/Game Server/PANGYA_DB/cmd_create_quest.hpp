// Arquivo cmd_create_quest.hpp
// Criado em 31/03/2018 as 22:51 por Acrisio
// Defini��o da classe CmdCreateQuest

#pragma once
#ifndef _STDA_CMD_CREATE_QUEST_HPP
#define _STDA_CMD_CREATE_QUEST_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../../Projeto IOCP/TYPE/data_iff.h"

namespace stdA {
	class CmdCreateQuest : public pangya_db {
		public:
			explicit CmdCreateQuest(bool _waiter = false);
			CmdCreateQuest(uint32_t _uid, bool _waiter = false);
			CmdCreateQuest(uint32_t _uid, uint32_t _achievement_id, IFF::QuestStuff& _quest, bool _include_counter, bool _waiter = false);
			virtual ~CmdCreateQuest();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			uint32_t getAchievementID();
			void setAchievementID(uint32_t _achievement_id);

			IFF::QuestStuff& getQuest();
			void setQuest(IFF::QuestStuff& _quest, bool _include_counter);

			bool getIncludeCounter();

			int32_t getID();
			int32_t getCounterItemID();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdCreateQuest"; };
			virtual std::wstring _wgetName() override { return L"CmdCreateQuest"; };

		private:
			uint32_t m_uid;
			uint32_t m_achievement_id;
			int32_t m_id;
			int32_t m_counter_item_id;
			bool m_include_counter;
			IFF::QuestStuff m_quest;

			const char* m_szConsulta = "pangya.ProcInsertNewQuest";
	};
}

#endif // !_STDA_CMD_CREATE_QUEST_HPP
