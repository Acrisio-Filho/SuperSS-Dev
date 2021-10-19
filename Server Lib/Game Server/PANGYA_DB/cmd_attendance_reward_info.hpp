// Arquivo cmd_attendance_reward_info.hpp
// Criado em 24/03/2018 as 18:19 por Acrisio
// Defini��o da classe CmdAttendanceRewardInfo

#pragma once
#ifndef _STDA_CMD_ATTENDANCE_REWARD_INFO_HPP
#define _STDA_CMD_ATTENDANCE_REWARD_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdAttendanceRewardInfo : public pangya_db {
		public:
			explicit CmdAttendanceRewardInfo(bool _waiter = false);
			CmdAttendanceRewardInfo(uint32_t _uid, bool _waiter = false);
			virtual ~CmdAttendanceRewardInfo();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			AttendanceRewardInfoEx& getInfo();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdAttendanceRewardInfo"; };
			virtual std::wstring _wgetName() override { return L"CmdAttendanceRewardInfo"; };

		private:
			uint32_t m_uid;
			AttendanceRewardInfoEx m_ari;

			const char* m_szConsulta = "pangya.ProcGetAttendanceReward";
	};
}

#endif // !_STDA_CMD_ATTENDANCE_REWARD_INFO_HPP
