// Arquivo cmd_register_player_login.hpp
// Criado em 01/04/2018 as 18:37 por Acrisio
// Defini��o da classe CmdRegisterPlayerLogin

#pragma once
#ifndef _STDA_CMD_REGISTER_PLAYER_LOGIN_HPP
#define _STDA_CMD_REGISTER_PLAYER_LOGIN_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include <string>

namespace stdA {
	class CmdRegisterPlayerLogin : public pangya_db {
		public:
			CmdRegisterPlayerLogin(bool _waiter = false);
			CmdRegisterPlayerLogin(uint32_t _uid, std::string& _ip, uint32_t _server_uid, bool _waiter = false);
			~CmdRegisterPlayerLogin();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			std::string& getIP();
			void setIP(std::string& _ip);

			uint32_t getServerUID();
			void setServerUID(uint32_t _server_uid);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			virtual std::string _getName() override { return "CmdRegisterPlayerLogin"; };
			virtual std::wstring _wgetName() override { return L"CmdRegisterPlayerLogin"; };

		private:
			uint32_t m_uid;
			uint32_t m_server_uid;
			std::string m_ip;

			const char* m_szConsulta = "pangya.ProcRegisterLogin";
	};
}

#endif // !_STDA_CMD_REGISTER_PLAYER_LOGIN_HPP
