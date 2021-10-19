// Arquivo cmd_item_left_from_email.hpp
// Criado em 24/03/2018 as 17:23 por Acrisio
// Defini��o da classe CmdItemLeftFromEmail

#pragma once
#ifndef _STDA_CMD_ITEM_LEFT_FROM_EMAIL
#define _STDA_CMD_ITEM_LEFT_FROM_EMAIL

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

namespace stdA {
	class CmdItemLeftFromEmail : public pangya_db {
		public:
			explicit CmdItemLeftFromEmail(bool _waiter = false);
			explicit CmdItemLeftFromEmail(int32_t _email_id, bool _waiter = false);
			virtual ~CmdItemLeftFromEmail();

			uint32_t getEmailID();
			void setEmailID(uint32_t _email_id);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdItemLeftFromEmail"; };
			virtual std::wstring _wgetName() override { return L"CmdItemLeftFromEmail"; };

		private:
			uint32_t m_email_id;

			const char* m_szConsulta = "UPDATE pangya.pangya_item_mail SET valid = 0 WHERE Msg_ID = ";
	};
}

#endif // !_STDA_CMD_ITEM_LEFT_FROM_EMAIL
