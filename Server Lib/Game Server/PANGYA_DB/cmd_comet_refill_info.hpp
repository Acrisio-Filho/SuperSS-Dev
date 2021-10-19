// Arquivo cmd_comet_refill_info.hpp
// Criado em 08/07/2018 as 22:42 por Acrisio
// Defini��o da classe CmdCometRefillInfo

#pragma once
#ifndef _STDA_CMD_COMET_REFILL_INFO_HPP
#define _STDA_CMD_COMET_REFILL_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/comet_refill_type.hpp"
#include <map>

namespace stdA {
	class CmdCometRefillInfo : public pangya_db {
		public:
			explicit CmdCometRefillInfo(bool _waiter = false);
			virtual ~CmdCometRefillInfo();

			std::map< uint32_t, ctx_comet_refill > getInfo();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdCometRefillInfo"; };
			virtual std::wstring _wgetName() override { return L"CmdCometRefillInfo"; };

		private:
			std::map< uint32_t, ctx_comet_refill > m_ctx_cr;

			const char* m_szConsulta = "pangya.ProcGetCometRefillInfo";
	};
}

#endif // !_STDA_CMD_COMET_REFILL_INFO_HPP
