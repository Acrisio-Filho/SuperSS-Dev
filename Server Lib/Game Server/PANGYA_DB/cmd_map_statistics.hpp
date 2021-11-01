// Arquivo cmd_map_statistics.hpp
// Criado em 18/03/2018 as 18:22 por Acrisio
// Defini��o da classe CmdMapStatistics

#pragma once
#ifndef _STDA_CMD_MAP_STATISTICS_HPP
#define _STDA_CMD_MAP_STATISTICS_HPP

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"
#include "../TYPE/pangya_game_st.h"
#include <vector>

namespace stdA {
	class CmdMapStatistics : public pangya_db {
		public:
			enum TYPE : unsigned char {
				NORMAL,
				ASSIST,
			};

			enum TYPE_MODO : unsigned char {
				M_NORMAL,
				M_NATURAL,
				M_GRAND_PRIX
			};

			enum TYPE_SEASON : unsigned char {
				ALL,			// Todas Seasons
				ONE,			// 1
				TWO,			// 2
				THREE,			// 3
				FOUR,			// 4
				CURRENT,		// Atual
			};

		public:
			explicit CmdMapStatistics(bool _waiter = false);
			CmdMapStatistics(uint32_t _uid, TYPE_SEASON _season, TYPE _type, TYPE_MODO _modo, bool _waiter = false);
			virtual ~CmdMapStatistics();

			std::vector< MapStatistics >& getMapStatistics();
			
			uint32_t getUID();
			void setUID(uint32_t _uid);

			TYPE getType();
			void setType(TYPE _type);

			TYPE_SEASON getSeason();
			void setSeason(TYPE_SEASON _season);

			TYPE_MODO getModo();
			void setModo(TYPE_MODO _modo);

		protected:
			void lineResult(result_set::ctx_res* _result, uint32_t _index_result) override;
			response* prepareConsulta(database& _db) override;

			// get Class name
			virtual std::string _getName() override { return "CmdMapStatistics"; };
			virtual std::wstring _wgetName() override { return L"CmdMapStatistics"; };

		private:
			uint32_t m_uid;
			TYPE m_type;
			TYPE_SEASON m_season;
			TYPE_MODO m_modo;
			std::vector< MapStatistics > v_map_statistics;

			const char* m_szConsulta[2] = { "pangya.ProcGetMapStatistics", "pangya.ProcGetMapStatisticsAssist" };
	};
}

#endif // !_STDA_CMD_MAP_STATISTICS_HPP
