// Arquivo cmd_update_treasure_hunter_course_point.hpp
// Criado em 22/09/2018 as 12:34 por Acrisio
// Defini��o da classe CmdUpdateTreasureHunterCoursePoint

#pragma once
#ifndef _STDA_CMD_UPDATE_TREASURE_HUNTER_COURSE_POINT_HPP
#define _STDA_CMD_UPDATE_TREASURE_HUNTER_COURSE_POINT_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdUpdateTreasureHunterCoursePoint : public pangya_db {
		public:
			explicit CmdUpdateTreasureHunterCoursePoint(bool _waiter = false);
			CmdUpdateTreasureHunterCoursePoint(TreasureHunterInfo& _thi, bool _waiter = false);
			virtual ~CmdUpdateTreasureHunterCoursePoint();

			TreasureHunterInfo& getInfo();
			void setInfo(TreasureHunterInfo& _thi);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			std::string _getName() override { return "CmdUpdateTreasureHunterCoursePoint"; };
			std::wstring _wgetName() override { return L"CmdUpdateTreasureHunterCoursePoint"; };

		private:
			TreasureHunterInfo m_thi;

			const char* m_szConsulta[2] = { "UPDATE pangya.pangya_course_reward_treasure SET PANGREWARD = ", " WHERE COURSE = " };
	};
}

#endif // !_STDA_CMD_UPDATE_TREASURE_HUNTER_COURSE_POINT_HPP
