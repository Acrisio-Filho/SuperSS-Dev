// Arquivo cmd_email_info.hpp
// Criado em 24/03/2018 as 15:10 por Acrisio
// Defini��o da classe CmdEmailInfo

#pragma once
#ifndef _STD_CMD_EMAIL_INFO_HPP
#define _STDA_CMD_EMAIL_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdEmailInfo : public pangya_db {
		public:
			explicit CmdEmailInfo(bool _waiter = false);
			CmdEmailInfo(uint32_t _uid, uint32_t _email_id, bool _waiter = false);
			virtual ~CmdEmailInfo();

			EmailInfo& getInfo();
			void setInfo(EmailInfo& _ei);

			uint32_t getUID();
			void setUID(uint32_t _uid);

			uint32_t getEmailID();
			void setEmailID(uint32_t _email_id);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdEmailInfo"; };
			virtual std::wstring _wgetName() override { return L"CmdEmailInfo"; };

		private:
			uint32_t m_uid;
			uint32_t m_email_id;
			EmailInfo m_ei;

			const char* m_szConsulta = "pangya.ProcGetInformationEmail";
	};
}

#endif // !_STD_CMD_EMAIL_INFO_HPP
