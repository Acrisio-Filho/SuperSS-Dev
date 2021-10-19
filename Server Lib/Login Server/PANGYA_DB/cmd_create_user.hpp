// Arquivo cmd_create_user.hpp
// Criado em 31/03/2018 as 15:54 por Acrisio
// Defini��o da classe CmdCreateUser.cpp

#pragma once
#ifndef _STDA_CMD_CREATE_USER_HPP
#define _STDA_CMD_CREATE_USER_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include <string>

namespace stdA {
	class CmdCreateUser : public pangya_db {
		public:
			CmdCreateUser(bool _waiter = false);
			CmdCreateUser(std::string& _id, std::string& _pass, std::string& _ip, uint32_t _server_uid, bool _waiter = false);
			~CmdCreateUser();

			std::string& getID();
			void setID(std::string& _id);

			std::string& getPASS();
			void setPass(std::string& _pass);

			std::string& getIP();
			void setIP(std::string& _ip);

			uint32_t getServerUID();
			void setServerUID(uint32_t _server_uid);

			uint32_t getUID();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			virtual std::string _getName() override { return "CmdCreateUser"; };
			virtual std::wstring _wgetName() override { return L"CmdCreateUser"; };

		private:
			std::string m_id;
			std::string m_pass;
			std::string m_ip;
			uint32_t m_server_uid;

			uint32_t m_uid;

			const char* m_szConsulta = "pangya.ProcNewUser";
	};
}

#endif // !_STDA_CMD_CREATE_USER_HPP
