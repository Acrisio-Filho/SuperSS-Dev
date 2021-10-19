// Arquivo cmd_premium_ticket_info.hpp
// Criado em 24/03/2018 as 19:20 por Acrisio
// Defini��o da classe CmdPremiumTicketInfo

#pragma once
#ifndef _STDA_CMD_PREMIUM_TICKET_INFO_HPP
#define _STDA_CMD_PREMIUM_TICKET_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdPremiumTicketInfo : public pangya_db {
		public:
			explicit CmdPremiumTicketInfo(bool _waiter = false);
			CmdPremiumTicketInfo(uint32_t _uid, bool _waiter = false);
			virtual ~CmdPremiumTicketInfo();

			PremiumTicket& getInfo();
			void setInfo(PremiumTicket& _pt);

			uint32_t getUID();
			void setUID(uint32_t _uid);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdPremiumTicketInfo"; };
			virtual std::wstring _wgetName() override { return L"CmdPremiumTicketInfo"; };

		private:
			uint32_t m_uid;
			PremiumTicket m_pt;

			const char* m_szConsulta = "pangya.ProcGetPremiumTicket";
	};
}

#endif // !_STDA_CMD_PREMIUM_TICKET_INFO_HPP
