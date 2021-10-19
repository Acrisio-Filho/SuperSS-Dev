// Arquivo cmd_mail_box_info2.hpp
// Criado em 13/01/2021 as 11:55 por Acrisio
// Defini��o da classe CmdMailBoxInfo2

#pragma once
#ifndef _STDA_CMD_MAIL_BOX_INFO2_HPP
#define _STDA_CMD_MAIL_BOX_INFO2_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"
#include <map>

namespace stdA {

	class CmdMailBoxInfo2 : public pangya_db {
		public:
			explicit CmdMailBoxInfo2(uint32_t _uid, bool _waiter = false);
			CmdMailBoxInfo2(bool _waiter = false);
			virtual ~CmdMailBoxInfo2();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			std::map< int32_t, EmailInfoEx >& getInfo();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			std::string _getName() override { return "CmdMailBoxInfo2"; };
			std::wstring _wgetName() override { return L"CmdMailBoxInfo2"; };

		private:
			uint32_t m_uid;
			std::map< int32_t, EmailInfoEx > m_emails;

			const char* m_szConsulta = "pangya.ProcGetAllEmailFromMailBox";
	};
}

#endif // !_STDA_CMD_MAIL_BOX_INFO2_HPP
