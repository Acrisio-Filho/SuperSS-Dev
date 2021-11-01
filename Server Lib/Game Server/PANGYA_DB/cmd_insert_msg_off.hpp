// Arquivo cmd_insert_msg_off.hpp
// Criado em 11/08/2018 as 13:35 por Acrisio
// Defini��o da classe CmdInsertMsgOff

#pragma once
#ifndef _STDA_CMD_INSERT_MSG_OFF_HPP
#define _STDA_CMD_INSERT_MSG_OFF_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include <string>

namespace stdA {
	class CmdInsertMsgOff : public pangya_db {
		public:
			explicit CmdInsertMsgOff(bool _waiter = false);
			CmdInsertMsgOff(uint32_t _uid, uint32_t _to_uid, std::string& _msg, bool _waiter = false);
			virtual ~CmdInsertMsgOff();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			uint32_t getToUID();
			void setToUID(uint32_t _to_uid);

			std::string& getMessage();
			void setMessage(std::string& _msg);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;


			virtual std::string _getName() override { return "CmdInsertMsgOff"; };
			virtual std::wstring _wgetName() override { return L"CmdInsertMsgOff"; };

		private:
			uint32_t m_uid;
			uint32_t m_to_uid;
			std::string m_msg;

			const char* m_szConsulta = "pangya.ProcAddMsgOff";
	};
}

#endif // !_STDA_CMD_INSERT_MSG_OFF_HPP
