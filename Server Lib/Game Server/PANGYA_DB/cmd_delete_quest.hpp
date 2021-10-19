// Arquivo cmd_delete_quest.hpp
// Criado em 11/11/2018 as 13:17 por Acrisio
// Defini��o da classe CmdDeleteQuest

#pragma once
#ifndef _STDA_CMD_DELETE_QUEST_HPP
#define _STDA_CMD_DELETE_QUEST_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

#include <vector>

namespace stdA {
	class CmdDeleteQuest : public pangya_db {
		public:
			explicit CmdDeleteQuest(bool _waiter = false);
			CmdDeleteQuest(uint32_t _uid, int32_t _id, bool _waiter = false);
			CmdDeleteQuest(uint32_t _uid, std::vector< QuestStuffInfo >& _v_id, bool _waiter = false);
			virtual ~CmdDeleteQuest();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			int32_t getId();
			std::vector< int32_t >& getIds();
			void setId(int32_t _id);
			void setId(std::vector< QuestStuffInfo >& _v_id);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			std::string _getName() override { return "CmdDeleteQuest"; };
			std::wstring _wgetName() override { return L"CmdDeleteQuest"; };

		protected:
			uint32_t m_uid;
			std::vector< int32_t > v_id;

			const char* m_szConsulta[3] = { "DELETE FROM pangya.pangya_quest WHERE UID = ", " AND id IN(", ")" };
	};
}

#endif // !_STDA_CMD_DELETE_QUEST_HPP
