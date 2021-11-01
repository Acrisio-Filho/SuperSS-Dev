// Arquivo cmd_insert_ticker.hpp
// Criado em 04/12/2018 as 01:38 por Acrisio
// Defini��o da classe CmdInsertTicker

#pragma once
#ifndef _STDA_CMD_INSERT_TICKER_HPP
#define _STDA_CMD_INSERT_TICKER_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

namespace stdA {
	class CmdInsertTicker : public pangya_db {
		public:
			explicit CmdInsertTicker(bool _waiter = false);
			CmdInsertTicker(uint32_t _uid, uint32_t _server_uid, std::string& _msg, bool _waiter = false);
			virtual ~CmdInsertTicker();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			uint32_t getServerUID();
			void setServerUID(uint32_t _server_uid);

			std::string& getMessage();
			void setMessage(std::string& _msg);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			std::string _getName() override { return "CmdInsertTicker"; };
			std::wstring _wgetName() override { return L"CmdInsertTicker"; };

		private:
			uint32_t m_uid;
			uint32_t m_server_uid;
			std::string m_msg;

			const char* m_szConsulta = "pangya.ProcRegisterTicker";
	};
}

#endif // !_STDA_CMD_INSERT_TICKER_HPP
