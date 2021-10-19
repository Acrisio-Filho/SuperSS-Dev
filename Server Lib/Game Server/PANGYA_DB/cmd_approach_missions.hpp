// Arquivo cmd_approach_missions.hpp
// Criado em 14/06/2020 as 17:00 por Acrisio
// Defini��o da classe CmdApproachMission

#pragma once
#ifndef _STDA_CMD_APPROACH_MISSIONS_HPP
#define _STDA_CMD_APPROACH_MISSIONS_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/approach_type.hpp"

namespace stdA {
	class CmdApproachMissions : public pangya_db {
		public:
			explicit CmdApproachMissions(bool _waiter = false);
			virtual ~CmdApproachMissions();

			std::vector< mission_approach_dados >& getInfo();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// Class Name
			virtual std::string _getName() override { return "CmdApproachMissions"; };
			virtual std::wstring _wgetName() override { return L"CmdApproachMissions"; };
		
		private:
			std::vector< mission_approach_dados > m_missions;

			const char* m_szConsulta = "pangya.ProcGetApproachMissions";
	};
}

#endif // !_STDA_CMD_APPROACH_MISSIONS_HPP
