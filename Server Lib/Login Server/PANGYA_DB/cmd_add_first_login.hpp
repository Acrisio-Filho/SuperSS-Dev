// Arquivo cmd_add_first_login.hpp
// Criado em 01/04/2018 as 16:15 por Acrisio
// Defini��o da classe CmdAddFirstLogin

#pragma once
#ifndef _STDA_CMD_ADD_FIRST_LOGIN_HPP
#define _STDA_CMD_ADD_FIRST_LOGIN_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

namespace stdA {
	class CmdAddFirstLogin : public pangya_db {
		public:
			CmdAddFirstLogin(bool _waiter = false);
			CmdAddFirstLogin(uint32_t _uid, unsigned char _flag, bool _waiter = false);
			~CmdAddFirstLogin();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			unsigned char getFLag();
			void setFlag(unsigned char _flag);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			virtual std::string _getName() override { return "CmdAddFirstLogin"; };
			virtual std::wstring _wgetName() override { return L"CmdAddFirstLogin"; };

		private:
			uint32_t m_uid;
			unsigned char m_flag;

			const char* m_szConsulta[2] = { "UPDATE pangya.account SET FIRST_LOGIN = ", " WHERE UID = " };
	};
}

#endif // !_STDA_CMD_ADD_FIRST_LOGIN_HPP
