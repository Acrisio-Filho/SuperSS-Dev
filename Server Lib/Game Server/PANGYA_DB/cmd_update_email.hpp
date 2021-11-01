// Arquivo cmd_update_email.hpp
// Criado em 14/01/2021 as 08:59 por Acrisio
// Defini��o da classe CmdUpdateEmail

#pragma once
#ifndef _STDA_CMD_UPDATE_EMAIL_HPP
#define _STDA_CMD_UPDATE_EMAIL_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {

	class CmdUpdateEmail : public pangya_db {
		public:
			CmdUpdateEmail(uint32_t _uid, EmailInfoEx& _ei, bool _waiter = false);
			CmdUpdateEmail(bool _waiter = false);
			virtual ~CmdUpdateEmail();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			EmailInfoEx& getEmail();
			void setEmail(EmailInfoEx& _ei);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			std::string _getName() override { return "CmdUpdateEmail"; };
			std::wstring _wgetName() override { return L"CmdUpdateEmail"; };

		private:
			EmailInfoEx m_ei;
			uint32_t m_uid;

			const char* m_szConsulta[4] = {
				"UPDATE pangya.pangya_gift_table SET Lida_YN = ",
				", Contador_Vista = ",
				" WHERE UID = ",
				" AND Msg_ID = "
			};
	};
}

#endif // !_STDA_CMD_UPDATE_EMAIL_HPP
