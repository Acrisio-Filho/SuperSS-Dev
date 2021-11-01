// Arquivo cmd_email_info2.hpp
// Criado em 14/01/2021 as 06:22 por Acrisio
// Defini��o da classe CmdEmailInfo2

#pragma once
#ifndef _STDA_CMD_EMAIL_INFO2_HPP
#define _STDA_CMD_EMAIL_INFO2_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {

	class CmdEmailInfo2 : public pangya_db {
		public:
			CmdEmailInfo2(uint32_t _uid, int32_t _email_id, bool _waiter = false);
			CmdEmailInfo2(bool _waiter = false);
			virtual ~CmdEmailInfo2();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			int32_t getEmailId();
			void setEmailId(int32_t _email_id);

			EmailInfoEx& getInfo();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			std::string _getName() override { return "CmdEmailInfo2"; };
			std::wstring _wgetName() override { return L"CmdEmailInfo2"; };

		private:
			uint32_t m_uid;
			int32_t m_email_id;

			EmailInfoEx m_ei;

			const char* m_szConsulta = "pangya.ProcGetInformationEmail2";
	};
}

#endif // !_STDA_CMD_EMAIL_INFO2_HPP
