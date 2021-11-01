// Arquivo cmd_update_golden_time.hpp
// Criado em 24/10/2020 as 03:25 por Acrisio
// Deifini��o da classe CmdUpdateGoldenTime

#pragma once
#ifndef _STDA_CMD_UPDATE_GOLDEN_TIME_HPP
#define _STDA_CMD_UPDATE_GOLDEN_TIME_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

namespace stdA {

	class CmdUpdateGoldenTime : public pangya_db {

		public:
			CmdUpdateGoldenTime(bool _waiter = false);
			CmdUpdateGoldenTime(uint32_t _id, bool _is_end, bool _waiter = false);
			virtual ~CmdUpdateGoldenTime();

			uint32_t getId();
			void setId(uint32_t _id);

			bool getIsEnd();
			void setIsEnd(bool _is_end);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			virtual std::string _getName() override { return "CmdUpdateGoldenTime"; };
			virtual std::wstring _wgetName() override { return L"CmdUpdateGoldenTime"; };

		private:
			uint32_t m_id;
			bool m_is_end;

			const char* m_szConsulta[2] = { 
				"UPDATE pangya.pangya_golden_time_info SET is_end = ",
				" WHERE index = "
			};
	};
}

#endif // !_STDA_CMD_UPDATE_GOLDEN_TIME_HPP
