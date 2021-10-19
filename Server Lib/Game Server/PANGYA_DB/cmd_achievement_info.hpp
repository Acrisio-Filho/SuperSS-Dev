// Arquivo cmd_achievement_info.hpp
// Criado em 21/03/2018 as 21:49 por Acrisio
// Defini��o da classe CmdAchievementInfo

#pragma once
#ifndef _STDA_CMD_ACHIEVEMENT_INFO_HPP
#define _STDA_CMD_ACHIEVEMENT_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"
#include <map>
#include <vector>

namespace stdA {
	class CmdAchievementInfo : public pangya_db {
		public:
			explicit CmdAchievementInfo(bool _waiter = false);
			CmdAchievementInfo(uint32_t _uid, bool _waiter = false);
			virtual ~CmdAchievementInfo();

			std::multimap< uint32_t, AchievementInfoEx >& getInfo();

			uint32_t getUID();
			void setUID(uint32_t _uid);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdAchievementInfo"; };
			virtual std::wstring _wgetName() override { return L"CmdAchievementInfo"; };

		protected:
			inline void check_achievement_retorno(AchievementInfoEx& _ai);
			inline void check_quest_achievement(AchievementInfoEx& _ai, QuestStuffInfo& _qsi);

		private:
			uint32_t m_uid;
			std::multimap< uint32_t, AchievementInfoEx > map_ai;

			const char* m_szConsulta = "pangya.ProcGetNewAchievement";	// Antigo pangya.ProcGetAchievement_New
	};
}

#endif // !_STDA_CMD_ACHIEVEMENT_INFO_HPP
