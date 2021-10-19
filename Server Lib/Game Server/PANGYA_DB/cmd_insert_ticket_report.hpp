// Arquivo cmd_insert_ticket_report.hpp
// Criado em 22/09/2018 as 16:40 por Acrisio
// Defini��o da classe CmdInsetTicketReport

#pragma once
#ifndef _STDA_CMD_INSERT_TICKET_REPORT_HPP
#define _STDA_CMD_INSERT_TICKET_REPORT_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

namespace stdA {
	class CmdInsertTicketReport : public pangya_db {
		public:
			explicit CmdInsertTicketReport(bool _waiter = false);
			CmdInsertTicketReport(uint32_t _trofel, unsigned char _type, bool _waiter = false);
			virtual ~CmdInsertTicketReport();

			uint32_t getTrofel();
			void setTrofel(uint32_t _trofel);

			unsigned char getType();
			void setType(unsigned char _type);

			int32_t getId();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			std::string _getName() override { return "CmdInsertTicketReport"; };
			std::wstring _wgetName() override { return L"CmdInsertTicketReport"; };

		private:
			int32_t m_id;
			uint32_t m_trofel;
			unsigned char m_type;

			const char* m_szConsulta = "pangya.ProcInsertNewTicketReport";
	};
}

#endif // !_STDA_CMD_INSERT_TICKET_REPORT_HPP
