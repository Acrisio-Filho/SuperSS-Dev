// Arquivo cmd_update_premium_ticket_time.hpp
// Criado em 07/12/2019 as 13:01 por Acrisio
// Defini��o da classe CmdUpdatePremiumTicketTime

#ifndef _STDA_CMD_UPDATE_PREMIUM_TICKET_TIME_HPP
#define _STDA_CMD_UPDATE_PREMIUM_TICKET_TIME_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdUpdatePremiumTicketTime : public pangya_db {
		public:
			explicit CmdUpdatePremiumTicketTime(bool _waiter = false);
			CmdUpdatePremiumTicketTime(uint32_t _uid, WarehouseItemEx& _wi, bool _waiter = false);
			virtual ~CmdUpdatePremiumTicketTime();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			WarehouseItemEx& getPremiumTicket();
			void setPremiumTicket(WarehouseItemEx& _wi);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			std::string _getName() override { return "CmdUpdatePremiumTicketTime"; };
			std::wstring _wgetName() override { return L"CmdUpdatePremiumTicketTime"; };

		private:
			uint32_t m_uid;
			WarehouseItemEx m_wi;

			const char* m_szConsulta = "pangya.ProcUpdatePremiumTicketTime";
	};
}

#endif // !_STDA_CMD_UPDATE_PREMIUM_TICKET_TIME_HPP
