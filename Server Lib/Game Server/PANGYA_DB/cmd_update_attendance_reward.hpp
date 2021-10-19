// Arquivo cmd_update_attendance_reward.hpp
// Criado em 14/11/2018 as 22:39 por Acrisio
// Defini��o da classe CmdUpdateAttendanceReward

#pragma once
#ifndef _STDA_CMD_UPDATE_ATTENDANCE_REWARD_HPP
#define _STDA_CMD_UPDATE_ATTENDANCE_REWARD_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdUpdateAttendanceReward : public pangya_db {
		public:
			explicit CmdUpdateAttendanceReward(bool _waiter = false);
			CmdUpdateAttendanceReward(uint32_t _uid, AttendanceRewardInfoEx& _ari, bool _waiter = false);
			virtual ~CmdUpdateAttendanceReward();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			AttendanceRewardInfoEx& getInfo();
			void setInfo(AttendanceRewardInfoEx& _ari);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			std::string _getName() override { return "CmdUpdateAttendanceReward"; };
			std::wstring _wgetName() override { return L"CmdUpdateAttendanceReward"; };

		private:
			uint32_t m_uid;
			AttendanceRewardInfoEx m_ari;

			const char* m_szConsulta = "pangya.ProcUpdateAttendanceReward";
	};
}

#endif // !_STDA_CMD_UPDATE_ATTENDANCE_REWARD_HPP
