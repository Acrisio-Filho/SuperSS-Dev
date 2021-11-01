// Arquivo cmd_firt_login_check.hpp
// Criado em 30/03/2018 as 20:14 por Acrisio
// Defini��o da classe CmdFirstLoginCheck

#pragma once
#ifndef _STDA_CMD_FIRST_LOGIN_CHECK_HPP
#define _STDA_CMD_FIRTS_LOGIN_CHECK_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

namespace stdA {
	class CmdFirstLoginCheck : public pangya_db {
		public:
			CmdFirstLoginCheck(bool _waiter = false);
			CmdFirstLoginCheck(uint32_t _uid, bool _waiter = false);
			~CmdFirstLoginCheck();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			bool getLastCheck();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			virtual std::string _getName() override { return "CmdFirstLoginCheck"; };
			virtual std::wstring _wgetName() override { return L"CmdFirstLoginCheck"; };

		private:
			uint32_t m_uid;
			bool m_check;

			const char* m_szConsulta = "SELECT FIRST_LOGIN FROM pangya.account WHERE uid = ";
	};
}

#endif // !_STDA_CMD_FIRST_LOGIN_CHECK_HPP
