// Arquivo cmd_delete_achievement.hpp
// Criado em 11/11/2018 as 14:26 por Acrisio
// Defini��o da classe CmdDeleteAchievement

#pragma once
#ifndef _STDA_CMD_DELETE_ACHIEVEMENT_HPP
#define _STDA_CMD_DELETE_ACHIEVEMENT_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdDeleteAchievement : public pangya_db {
		public:
			explicit CmdDeleteAchievement(bool _waiter = false);
			CmdDeleteAchievement(uint32_t _uid, int32_t _id, bool _waiter = false);
			virtual ~CmdDeleteAchievement();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			int32_t getId();
			void setId(int32_t _id);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			std::string _getName() override { return "CmdDeleteAchievement"; };
			std::wstring _wgetName() override { return L"CmdDeleteAchievement"; };

		private:
			uint32_t m_uid;
			int32_t m_id;

			const char* m_szConsulta[2] = { "DELETE FROM pangya.pangya_achievement WHERE UID = ", " AND ID_ACHIEVEMENT = " };
	};
}

#endif // !_STDA_CMD_DELETE_ACHIEVEMENT_HPP
