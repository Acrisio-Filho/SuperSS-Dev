// Arquivo cmd_insert_ticket_report_data.hpp
// Criado em 22/09/2018 as 17:14 por Acrisio
// Defini��o da classe CmdInsertTicketReportData

#pragma once
#ifndef _STDA_CMD_INSERT_TICKET_REPORT_DATA_HPP
#define _STDA_CMD_INSERT_TICKET_REPORT_DATA_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/game_type.hpp"

namespace stdA {
	class CmdInsertTicketReportData : public pangya_db {
		public:
			explicit CmdInsertTicketReportData(bool _waiter = false);
			CmdInsertTicketReportData(int32_t _id, TicketReportInfo::stTicketReportDados& _trd, bool _waiter = false);
			virtual ~CmdInsertTicketReportData();

			int32_t getId();
			void setId(int32_t _id);

			TicketReportInfo::stTicketReportDados& getInfo();
			void setInfo(TicketReportInfo::stTicketReportDados& _trd);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			std::string _getName() override { return "CmdInsertTicketReportData"; };
			std::wstring _wgetName() override { return L"CmdInsertTicketReportData"; };

		private:
			int32_t m_id;
			TicketReportInfo::stTicketReportDados m_trd;

			const char* m_szConsulta = "pangya.ProcInsertTicketReportDados";
	};
}

#endif // !_STDA_CMD_INSERT_TICKET_REPORT_DATA_HPP
