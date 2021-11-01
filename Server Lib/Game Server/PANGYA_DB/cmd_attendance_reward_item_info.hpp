// Arquivo cmd_attendance_reward_item_info.hpp
// Criado em 14/11/2018 as 21:57 por Acrisio
// Defini��o da classe CmdAttendanceRewardItemInfo

#pragma once
#ifndef _STDA_CMD_ATTENDANCE_REWARD_ITEM_INFO_HPP
#define _STDA_CMD_ATTENDANCE_REWARD_ITEM_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

#include <vector>

namespace stdA {
	class CmdAttendanceRewardItemInfo : public pangya_db {
		public:
			explicit CmdAttendanceRewardItemInfo(bool _waiter = false);
			virtual ~CmdAttendanceRewardItemInfo();

			std::vector< AttendanceRewardItemCtx >& getInfo();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			std::string _getName() override { return "CmdAttendanceRewardItemInfo"; };
			std::wstring _wgetName() override { return L"CmdAttendanceRewardItemInfo"; };

		private:
			std::vector< AttendanceRewardItemCtx > v_item;

			const char* m_szConsulta = "SELECT typeid, quantidade, tipo FROM pangya.pangya_attendance_table_item_reward";
	};
}

#endif // !_STDA_CMD_ATTENDANCE_REWARD_ITEM_INFO_HPP
