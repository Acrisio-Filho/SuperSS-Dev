// Arquivo cmd_treasure_hunter_info.hpp
// Criado em 19/03/2018 as 22:57 por Acrisio
// Defini��o da classe CmdTreasureHunterInfo

#pragma once
#ifndef _STDA_CMD_TREASURE_HUNTER_INFO_HPP
#define _STDA_CMD_TREASURE_HUNTER_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"
#include <vector>

namespace stdA {
	class CmdTreasureHunterInfo : public pangya_db {
		public:
			explicit CmdTreasureHunterInfo(bool _waiter = false);
			virtual ~CmdTreasureHunterInfo();

			std::vector< TreasureHunterInfo >& getInfo();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdTreasureHunterInfo"; };
			virtual std::wstring _wgetName() override { return L"CmdTreasureHunterInfo"; };

		private:
			std::vector< TreasureHunterInfo > v_thi;			// Treasure Hunter Info = THI

			const char* m_szConsulta = "pangya.ProcGetCourseReward";
	};
}

#endif // !_STDA_CMD_TREASURE_HUNTER_INFO_HPP
