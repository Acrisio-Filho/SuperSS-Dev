// Arquivo cmd_rank_config_info.hpp
// Criado em 18/06/2020 as 21:23 por Acrisio
// Defini��o da classe CmdRankConfigInfo

#pragma once
#ifndef _STDA_CMD_RANK_CONFIG_INFO_HPP
#define _STDA_CMD_RANK_CONFIG_INFO_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../UTIL/rank_refresh_time.hpp"

namespace stdA {
	class CmdRankConfigInfo : public pangya_db {
		public:
			explicit CmdRankConfigInfo(bool _waiter = false);
			virtual ~CmdRankConfigInfo();

			rank_refresh_time& getInfo();

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// Class Name
			std::string _getName() override { return "CmdRankConfigInfo"; };
			std::wstring _wgetName() override { return L"CmdRankConfigInfo"; };

		private:
			rank_refresh_time m_rft;

			const char* m_szConsulta = "SELECT refresh_time_H, reg_date FROM pangya.pangya_rank_config";
	};
}

#endif // !_STDA_CMD_RANK_CONFIG_INFO_HPP
