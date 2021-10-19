// Arquivo rank_refresh_time.cpp
// Criado em 18/06/2020 as 22:06 por Acrisio
// Implementa��o da classe rank_refresh_time

#include "rank_refresh_time.hpp"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"

#include "../../Projeto IOCP/UTIL/util_time.h"

#include <memory.h>

using namespace stdA;

rank_refresh_time::rank_refresh_time() : m_interval_refresh(0u), m_st_last_date{ 0u }, m_ft_last_date{ 0u } {
}

rank_refresh_time::rank_refresh_time(uint32_t _interval, std::string _date) : m_interval_refresh(_interval),
	m_st_last_date{ 0u }, m_ft_last_date{ 0u } {

	setLastRefreshDate(_date);
}

rank_refresh_time::rank_refresh_time(uint32_t _interval, SYSTEMTIME& _st) : m_interval_refresh(_interval),
	m_st_last_date{ 0u }, m_ft_last_date{ 0u } {

	setLastRefreshDate(_st);
}

rank_refresh_time::rank_refresh_time(uint32_t _interval, FILETIME& _ft) : m_interval_refresh(_interval),
	m_st_last_date{ 0u }, m_ft_last_date{ 0u } {

	setLastRefreshDate(_ft);
}

rank_refresh_time::~rank_refresh_time() {
	clear();
}

void rank_refresh_time::clear() {

	// Mudei o jeito que limpa a classe, por que no x64 d� erro por causa da vtable
	memset(&m_interval_refresh, 0, sizeof(m_interval_refresh));
	memset(&m_st_last_date, 0, sizeof(m_st_last_date));
	memset(&m_ft_last_date, 0, sizeof(m_ft_last_date));
}

bool rank_refresh_time::isOutDated() {

#ifdef _DEBUG
	int64_t diff = 0ll;

	SYSTEMTIME st_tmp_chk{ 0u };
	FILETIME ft_tmp_chk{ 0u };

	reinterpret_cast< LARGE_INTEGER* >(&ft_tmp_chk)->QuadPart = reinterpret_cast< LARGE_INTEGER* >(&m_ft_last_date)->QuadPart + STDA_10_MICRO_PER_HOUR * m_interval_refresh;

	FileTimeToSystemTime(&ft_tmp_chk, &st_tmp_chk);

	// Verifica se j� chegou no tempo de atualizar o rank registros
	if ((diff = getLocalTimeDiff(st_tmp_chk)) > 0)
		_smp::message_pool::getInstance().push(new message("[rank_refresh_time::isOutDated][Log] Passou da hora. Diff: " + std::to_string(diff / STDA_10_MICRO_PER_SEC) + " seconds.", CL_FILE_LOG_AND_CONSOLE));

	return diff > 0;
#else
	SYSTEMTIME st_tmp_chk{ 0u };
	FILETIME ft_tmp_chk{ 0u };

	reinterpret_cast< LARGE_INTEGER* >(&ft_tmp_chk)->QuadPart = reinterpret_cast< LARGE_INTEGER* >(&m_ft_last_date)->QuadPart + STDA_10_MICRO_PER_HOUR * m_interval_refresh;

	FileTimeToSystemTime(&ft_tmp_chk, &st_tmp_chk);

	return getLocalTimeDiff(st_tmp_chk) > 0;
#endif
}

std::string rank_refresh_time::toString() {
	return std::string("Interval_Hora=") + std::to_string(m_interval_refresh) + ", Last_Refresh=" + _formatDate(m_st_last_date);
}

uint32_t& rank_refresh_time::getIntervalRefresh() {
	return m_interval_refresh;
}

SYSTEMTIME& rank_refresh_time::getLastRefreshDateSystemTime() {
	return m_st_last_date;
}

FILETIME& rank_refresh_time::getLastRefreshDateFileTime() {
	return m_ft_last_date;
}

void rank_refresh_time::setIntervalRefresh(uint32_t _interval) {
	m_interval_refresh = _interval;
}

void rank_refresh_time::setLastRefreshDate(std::string _date) {

	_translateDate(_date, &m_st_last_date);

	setLastRefreshDate(m_st_last_date);
}

void rank_refresh_time::setLastRefreshDate(SYSTEMTIME& _st) {

	m_st_last_date = _st;

	SystemTimeToFileTime(&m_st_last_date, &m_ft_last_date);
}

void rank_refresh_time::setLastRefreshDate(FILETIME& _ft) {

	m_ft_last_date = _ft;

	FileTimeToSystemTime(&m_ft_last_date, &m_st_last_date);
}
