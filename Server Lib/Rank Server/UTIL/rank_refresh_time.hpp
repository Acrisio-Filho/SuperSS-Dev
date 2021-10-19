// Arquivo rank_refresh_time.hpp
// Criado em 18/06/2020 as 22:04 por Acrisio
// Defini��o da classe rank_refresh_time

#pragma once
#ifndef _STDA_RANK_REFRESH_TIME_HPP
#define _STDA_RANK_REFRESH_TIME_HPP

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#endif

#include <string>

namespace stdA {
	class rank_refresh_time {
		public:
			rank_refresh_time();
			rank_refresh_time(uint32_t _interval, std::string _date);
			rank_refresh_time(uint32_t _interval, SYSTEMTIME& _st);
			rank_refresh_time(uint32_t _interval, FILETIME& _ft);
			virtual ~rank_refresh_time();

			void clear();

			// Passou da data, venceu, pode atualize os registro do Rank
			bool isOutDated();

			std::string toString();

			// Get
			uint32_t& getIntervalRefresh();

			SYSTEMTIME& getLastRefreshDateSystemTime();
			FILETIME& getLastRefreshDateFileTime();

			// Set
			void setIntervalRefresh(uint32_t _interval);

			void setLastRefreshDate(std::string _date);
			void setLastRefreshDate(SYSTEMTIME& _st);
			void setLastRefreshDate(FILETIME& _ft);

		protected:
			uint32_t m_interval_refresh;			// Intervalo em horas

			SYSTEMTIME m_st_last_date;					// System Time - Ultima data que atualizou o rank registros
			FILETIME m_ft_last_date;					// File Time - Ultima data que atualizou o rank registros

	};
}

#endif // !_STDA_RANK_REFRESH_TIME_HPP
