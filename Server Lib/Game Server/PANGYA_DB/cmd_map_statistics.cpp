// Arquivo cmd_map_statistics.cpp
// Criado em 18/03/2018 as 18:32 por Acrisio
// Implementa��o da classe CmdMapStatistics

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_map_statistics.hpp"

using namespace stdA;

CmdMapStatistics::CmdMapStatistics(bool _waiter)
		: pangya_db(_waiter), m_uid(0), m_type(NORMAL), m_season(CURRENT), m_modo(M_NORMAL), v_map_statistics() {
}

CmdMapStatistics::CmdMapStatistics(uint32_t _uid, TYPE_SEASON _season, TYPE _type, TYPE_MODO _modo, bool _waiter)
		: pangya_db(_waiter), m_uid(_uid), m_type(_type), m_season(_season), m_modo(_modo), v_map_statistics() {
}

CmdMapStatistics::~CmdMapStatistics() {
}

void CmdMapStatistics::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(14, (uint32_t)_result->cols);

	MapStatistics ms{ 0 };

	//ms.tipo = (unsigned char)IFNULL(atoi, _result->data[1]);
	ms.course = (unsigned char)IFNULL(atoi, _result->data[2]);
	ms.tacada = IFNULL(atoi, _result->data[3]);
	ms.putt = IFNULL(atoi, _result->data[4]);
	ms.hole = IFNULL(atoi, _result->data[5]);
	ms.fairway = IFNULL(atoi, _result->data[6]);
	ms.hole_in = IFNULL(atoi, _result->data[7]);
	ms.putt_in = IFNULL(atoi, _result->data[8]);
	ms.total_score = IFNULL(atoi, _result->data[9]);
	ms.best_score = (unsigned char)IFNULL(atoi, _result->data[10]);
	ms.best_pang = IFNULL(atoll, _result->data[11]);
	ms.character_typeid = IFNULL(atoi, _result->data[12]);
	ms.event_score = (unsigned char)IFNULL(atoi, _result->data[13]);

	v_map_statistics.push_back(ms);

}

response* CmdMapStatistics::prepareConsulta(database& _db) {
	
	v_map_statistics.clear();
	v_map_statistics.shrink_to_fit();

	uint32_t season = (m_season == ALL) ? 9 + m_modo : (m_modo == M_NORMAL ? m_season : m_season * 10 + m_modo);

	auto r = procedure(_db, (m_type == NORMAL) ? m_szConsulta[0] : m_szConsulta[1], std::to_string(m_uid) + ", " + std::to_string(season));

	checkResponse(r, "nao conseguiu pegar o map statistics " + ((m_type == NORMAL) ? std::string("NORMAL") : std::string("ASSIST")) +  "season [" + std::to_string(m_season) + "]  do player: " + std::to_string(m_uid));

	return r;
}

std::vector< MapStatistics >& stdA::CmdMapStatistics::getMapStatistics() {
	return v_map_statistics;
}

uint32_t CmdMapStatistics::getUID() {
	return m_uid;
}

void CmdMapStatistics::setUID(uint32_t _uid) {
	m_uid = _uid;
}

CmdMapStatistics::TYPE CmdMapStatistics::getType() {
	return m_type;
}

void CmdMapStatistics::setType(TYPE _type) {
	m_type = _type;
}

CmdMapStatistics::TYPE_SEASON CmdMapStatistics::getSeason() {
	return m_season;
}

void CmdMapStatistics::setSeason(TYPE_SEASON _season) {
	m_season = _season;
}

CmdMapStatistics::TYPE_MODO CmdMapStatistics::getModo() {
	return m_modo;
}

void CmdMapStatistics::setModo(TYPE_MODO _modo) {
	m_modo = _modo;
}


