// Arquivo cmd_ticker_info.hpp
// Criado em 03/12/2018 as 14:55 por Acrisio
// Defini��o da classe CmdTickerInfo

#pragma once
#ifndef _STDA_CMD_TICKER_INFO_HPP
#define _STDA_CMD_TICKER_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_auth_st.h"

namespace stdA {
	class CmdTickerInfo : public pangya_db {
		public:
			explicit CmdTickerInfo(bool _waiter = false);
			CmdTickerInfo(uint32_t _id, bool _waiter = false);
			virtual ~CmdTickerInfo();

			uint32_t getId();
			void setId(uint32_t _id);

			TickerInfo& getInfo();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			std::string _getName() override { return "CmdTickerInfo"; };
			std::wstring _wgetName() override { return L"CmdTickerInfo"; };

		private:
			uint32_t m_id;
			TickerInfo m_ti;

			const char* m_szConsulta = "pangya.ProcGetTicker";
	};
}

#endif // !_STDA_CMD_TICKER_INFO_HPP
