// Arquivo cmd_golden_time_round.hpp
// Criado em 24/10/2020 as 02:31 por Acrisio
// Defini��o da classe CmdGoldenTimeRound

#pragma once
#ifndef _STDA_CMD_GOLDEN_TIME_ROUND_HPP
#define _STDA_CMD_GOLDEN_TIME_ROUND_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/golden_time_type.hpp"

namespace stdA {

	class CmdGoldenTimeRound : public pangya_db {

		public:
			CmdGoldenTimeRound(uint32_t _id, bool _waiter = false);
			explicit CmdGoldenTimeRound(bool _waiter = false);
			virtual ~CmdGoldenTimeRound();

			uint32_t getId();
			void setId(uint32_t _id);

			std::vector< stRound >& getInfo();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			virtual std::string _getName() override { return "CmdGoldenTimeRound"; };
			virtual std::wstring _wgetName() override { return L"CmdGoldenTimeRound"; };

		private:
			uint32_t m_id;

			std::vector< stRound > m_round;

			const char* m_szConsulta = "SELECT time FROM pangya.pangya_golden_time_round WHERE golden_time_id = ";
	};
}

#endif // !_STDA_CMD_GOLDEN_TIME_ROUND_HPP
