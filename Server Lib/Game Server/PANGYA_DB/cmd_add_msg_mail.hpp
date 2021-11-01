// Arquivo cmd_add_msg_mail.hpp
// Criado em 26/05/2018 as 15:06 por Acrisio
// Definie��o da classe CmdAddMsgMail

#pragma once
#ifndef _STDA_CMD_ADD_MSG_MAIL_HPP
#define _STDA_CMD_ADD_MSG_MAIL_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include <string>

namespace stdA {
	class CmdAddMsgMail : public pangya_db {
		public:
			explicit CmdAddMsgMail(bool _waiter = false);
			CmdAddMsgMail(uint32_t _uid_from, uint32_t _uid_to, std::string& _msg, bool _waiter = false);
			virtual ~CmdAddMsgMail();

			uint32_t getUIDFrom();
			void setUIDFrom(uint32_t _uid_from);

			uint32_t getUIDTo();
			void setUIDTo(uint32_t _uid_to);

			std::string& getMsg();
			void setMsg(std::string& _msg);

			int32_t getMailID();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdAddMsgMail"; };
			virtual std::wstring _wgetName() override { return L"CmdAddMsgMail"; };

		private:
			uint32_t m_uid_from;
			uint32_t m_uid_to;
			std::string m_msg;

			int32_t m_mail_id;

			const char* m_szConsulta = "pangya.ProcColocaMsgNoGiftTable";
	};
}

#endif // !_STDA_CMD_ADD_MSG_MAIL_HPP
