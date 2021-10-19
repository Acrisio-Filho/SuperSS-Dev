// Arquivo cmd_ticket_report_dados_info.hpp
// Criado em 13/10/218 as 13:14 por Acrisio
// Defini��o da classe CmdTicketReportDadosInfo

#pragma once
#ifndef _STDA_CMD_TICKET_REPORT_DADOS_INFO_HPP
#define _STDA_CMD_TICKET_REPORT_DADOS_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdTicketReportDadosInfo : public pangya_db {
		public:
			explicit CmdTicketReportDadosInfo(bool _waiter = false);
			CmdTicketReportDadosInfo(int32_t _ticket_report_id, bool _waiter = false);
			virtual ~CmdTicketReportDadosInfo();

			int32_t getTicketReportId();
			void setTicketReportId(int32_t _ticket_report_id);

			TicketReportScrollInfo& getInfo();
			void setInfo(TicketReportScrollInfo& _trsi);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			std::string _getName() override { return "CmdTicketReportDadosInfo"; };
			std::wstring _wgetName() override { return L"CmdTicketReportDadosInfo"; };

		private:
			int32_t m_ticket_report_id;
			TicketReportScrollInfo m_trsi;

			const char* m_szConsulta = "pangya.ProcGetTicketReportDados";
	};
}

#endif // !_STDA_CMD_TICKET_REPORT_DADOS_INFO_HPP
