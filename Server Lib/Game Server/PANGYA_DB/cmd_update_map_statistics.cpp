// Arquivo cmd_update_map_statistics.cpp
// Criado em 08/09/2018 as 22:02 por Acrisio
// Implementa��o da classe CmdUpdateMapStatistics

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_map_statistics.hpp"

using namespace stdA;

CmdUpdateMapStatistics::CmdUpdateMapStatistics(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_assist(0u), m_ms{0} {
}

CmdUpdateMapStatistics::CmdUpdateMapStatistics(uint32_t _uid, MapStatisticsEx& _ms, unsigned char _assist, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_assist(_assist), m_ms(_ms) {
}

CmdUpdateMapStatistics::~CmdUpdateMapStatistics() {
}

void CmdUpdateMapStatistics::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdUpdateMapStatistics::prepareConsulta(database& _db) {

	if (m_uid == 0)
		throw exception("[CmdUpdateMapStatistics::prepareConsulta][Error] m_uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string((unsigned short)m_ms.tipo) + ", " + std::to_string((unsigned short)(m_ms.course & 0x7F))
				+ ", " + std::to_string(m_ms.tacada) + ", " + std::to_string(m_ms.putt) + ", " + std::to_string(m_ms.hole) + ", " + std::to_string(m_ms.fairway)
				+ ", " + std::to_string(m_ms.hole_in) + ", " + std::to_string(m_ms.putt_in) + ", " + std::to_string(m_ms.total_score) + ", " + std::to_string((short)m_ms.best_score)
				+ ", " + std::to_string(m_ms.best_pang) + ", " + std::to_string(m_ms.character_typeid) + ", " + std::to_string((unsigned short)m_ms.event_score) 
				+ ", " + std::to_string((unsigned short)m_assist)
	);

	checkResponse(r, "nao conseguiu atualizar o record(MapStatistics) dados[COURSE=" + std::to_string((unsigned short)(m_ms.course & 0x7F)) + ", TIPO=" 
			+ std::to_string((unsigned short)m_ms.tipo) + ", ASSIST=" + std::to_string((unsigned short)m_assist) + "] do player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdUpdateMapStatistics::getUID() {
	return m_uid;
}

void CmdUpdateMapStatistics::setUID(uint32_t _uid) {
	m_uid = _uid;
}

unsigned char CmdUpdateMapStatistics::getAssist() {
	return m_assist;
}

void CmdUpdateMapStatistics::setAssist(unsigned char _assist) {
	m_assist = _assist;
}

MapStatisticsEx& CmdUpdateMapStatistics::getInfo() {
	return m_ms;
}

void CmdUpdateMapStatistics::setInfo(MapStatisticsEx& _ms) {
	m_ms = _ms;
}
