// Arquivo cmd_register_logon_server.hpp
// Criado em 03/12/2018 as 20:03 por Acrisio
// Defini��o da classe CmdRegisterLogonServer

#pragma once
#ifndef _STDA_CMD_REGISTER_LOGON_SERVER_HPP
#define _STDA_CMD_REGISTER_LOGON_SERVER_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

namespace stdA {
	class CmdRegisterLogonServer : public pangya_db {
		public:
			explicit CmdRegisterLogonServer(bool _waiter = false);
			CmdRegisterLogonServer(uint32_t _uid, std::string _server_id, bool _waiter = false);
			virtual ~CmdRegisterLogonServer();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			std::string& getServerId();
			void setServerId(std::string& _server_id);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			std::string _getName() override { return "CmdRegisterLogonServer"; };
			std::wstring _wgetName() override { return L"CmdRegisterLogonServer"; };

		private:
			uint32_t m_uid;
			std::string m_server_id;

			const char* m_szConsulta = "pangya.ProcRegisterLogonServer";
	};
}

#endif // !_STDA_CMD_REGISTER_LOGON_SERVER_HPP
