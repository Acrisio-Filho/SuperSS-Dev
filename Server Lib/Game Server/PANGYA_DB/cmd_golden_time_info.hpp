// Arquivo cmd_golden_time_info.hpp
// Criado em 24/10/2020 as 02:00 por Acrisio
// Defini��o da classe CmdGoldenTimeInfo

#pragma once
#ifndef _STDA_GOLDEN_TIME_INFO_HPP
#define _STDA_GOLDEN_TIME_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/golden_time_type.hpp"

namespace stdA {

	class CmdGoldenTimeInfo : public pangya_db {

		public:
			CmdGoldenTimeInfo(bool _waiter = false);
			virtual ~CmdGoldenTimeInfo();

			std::vector< stGoldenTime >& getInfo();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			virtual std::string _getName() override { return "CmdGoldenTimeInfo"; };
			virtual std::wstring _wgetName() override { return L"CmdGoldenTimeInfo"; };

		private:
			std::vector< stGoldenTime > m_gt;

			const char* m_szConsulta = "pangya.ProcGetGoldenTimeInfo";
	};
}

#endif // !_STDA_GOLDEN_TIME_INFO_HPP
