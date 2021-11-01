// Arquivo cmd_update_map_statistics.hpp
// Criado em 08/09/2018 as 21:55 por Acrisio
// Defini��o da classe CmdUpdateMapStatistics

#pragma once
#ifndef _STDA_CMD_UPDATE_MAP_STATISTICS_HPP
#define _STDA_CMD_UPDATE_MAP_STATISTICS_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"

namespace stdA {
	class CmdUpdateMapStatistics : public pangya_db {
		public:
			explicit CmdUpdateMapStatistics(bool _waiter = false);
			CmdUpdateMapStatistics(uint32_t _uid, MapStatisticsEx& _ms, unsigned char _assist, bool _waiter = false);
			virtual ~CmdUpdateMapStatistics();

			uint32_t getUID();
			void setUID(uint32_t _uid);

			unsigned char getAssist();
			void setAssist(unsigned char _assist);

			MapStatisticsEx& getInfo();
			void setInfo(MapStatisticsEx& _ms);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			std::string _getName() override { return "CmdUpdateMapStatistics"; };
			std::wstring _wgetName() override { return L"CmdUpdateMapStatistics"; };

		private:
			uint32_t m_uid;
			unsigned char m_assist;
			MapStatisticsEx m_ms;

			const char* m_szConsulta = "pangya.ProcUpdateMapStatistics";
	};
}

#endif // !_STDA_CMD_UPDATE_MAP_STATISTICS_HPP
