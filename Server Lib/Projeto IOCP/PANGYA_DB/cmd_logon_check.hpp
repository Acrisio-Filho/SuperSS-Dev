// Arquivo cmd_logon_check.hpp
// Criado em 30/03/2018 as 18:25 por Acrisio
// Deini��o da classe CmdLogonCheck

#pragma once
#ifndef _STDA_CMD_LOGON_CHECK_HPP
#define _STDA_CMD_LOGON_CHECK_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

namespace stdA {
	class CmdLogonCheck : public pangya_db {
		public:
			CmdLogonCheck(bool _waiter = false);
			CmdLogonCheck(uint32_t _uid, bool _waiter = false);
			~CmdLogonCheck();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			bool getLastCheck();
			uint32_t getServerUID();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			virtual std::string _getName() override { return "CmdLogonCheck"; };
			virtual std::wstring _wgetName() override { return L"CmdLogonCheck"; };

		private:
			uint32_t m_uid;
			uint32_t m_server_uid;
			bool m_check;

			const char* m_szConsulta = "SELECT LOGON, game_server_id FROM pangya.account WHERE UID = ";
	};
}

#endif // !_STDA_CMD_LOGON_CHECK_HPP
