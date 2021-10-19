// Arquivo cmd_mail_box_info.hpp
// Criado em 24/03/2018 as 14:42 por Acrisio
// Defini��o da classe CmdMailBoxInfo

#pragma once
#ifndef _STDA_CMD_MAIL_BOX_HPP
#define _STDA_CMD_MAIL_BOX_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"
#include <vector>

namespace stdA {
	class CmdMailBoxInfo : public pangya_db {
		public:
			enum TYPE : unsigned char {
				NORMAL,
				NAO_LIDO,
			};

		public:
			explicit CmdMailBoxInfo(bool _waiter = false);
			CmdMailBoxInfo(uint32_t _uid, TYPE _type, uint32_t _page = 1u, bool _waiter = false);
			virtual ~CmdMailBoxInfo();

			std::vector< MailBox >& getInfo();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			TYPE getType();
			void setType(TYPE _type);

			uint32_t getPage();
			void setPage(uint32_t _page);

			uint32_t getTotalPage();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdMailBoxInfo"; };
			virtual std::wstring _wgetName() override { return L"CmdMailBoxInfo"; };

		private:
			uint32_t m_uid;
			uint32_t m_page;
			uint32_t m_total_page;

			TYPE m_type;

			std::vector< MailBox > v_mb;

			const char* m_szConsulta[2] = { "pangya.ProcGetEmailFromMailBox", "pangya.ProcGetEmailNaoLidaFromMailBox" };
	};
}

#endif // !_STDA_CMD_MAIL_BOX_HPP
