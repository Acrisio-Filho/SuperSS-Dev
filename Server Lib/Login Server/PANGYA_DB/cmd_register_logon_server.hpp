// Arquivo cmd_register_logon_server.hpp
// Criado em 01/04/2018 as 20:48 por Acrisio
// Defini��o da classe CmdRegisterLogonServer

#pragma once
#ifndef _STDA_CMD_REGISTER_LOGON_SERVER_HPP
#define _STDA_CMD_REGISTER_LOGON_SERVER_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

namespace stdA {
	class CmdRegisterLogonServer : public pangya_db {
		public:
			CmdRegisterLogonServer(bool _waiter = false);
			CmdRegisterLogonServer(uint32_t _uid, uint32_t _server_uid, bool _waiter = false);
			~CmdRegisterLogonServer();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			uint32_t getServerUID();
			void setServerUID(uint32_t _server_uid);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			virtual std::string _getName() override { return "CmdRegisterLogonServer"; };
			virtual std::wstring _wgetName() override { return L"CmdRegisterLogonServer"; };

		private:
			uint32_t m_uid;
			uint32_t m_server_uid;

			const char* m_szConsulta = "pangya.ProcRegisterLogonServer";
	};
}

#endif // !_STDA_CMD_REGISTER_LOGON_SERVER_HPP
