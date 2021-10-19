// Arquivo cmd_update_treasure_hunter_course_point.cpp
// Criado em 22/09/2018 as 12:46 por Acrisio
// Implementa��o da classe CmdTreasureHunterCoursePoint

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_treasure_hunter_course_point.hpp"

using namespace stdA;

CmdUpdateTreasureHunterCoursePoint::CmdUpdateTreasureHunterCoursePoint(bool _waiter)
	: pangya_db(_waiter), m_thi{0} {
}

CmdUpdateTreasureHunterCoursePoint::CmdUpdateTreasureHunterCoursePoint(TreasureHunterInfo& _thi, bool _waiter)
	: pangya_db(_waiter), m_thi(_thi) {
}

CmdUpdateTreasureHunterCoursePoint::~CmdUpdateTreasureHunterCoursePoint() {
}

void CmdUpdateTreasureHunterCoursePoint::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdUpdateTreasureHunterCoursePoint::prepareConsulta(database& _db) {

	auto r = consulta(_db, m_szConsulta[0] + std::to_string(m_thi.point) + m_szConsulta[1] + std::to_string((unsigned short)(m_thi.course & 0x7F)));

	checkResponse(r, "nao conseguiu atulizar o Treasure Hunter Info[COURSE=" + std::to_string((unsigned short)(m_thi.course & 0x7F)) + ", POINT=" + std::to_string(m_thi.point) + "]");

	return r;
}

TreasureHunterInfo& CmdUpdateTreasureHunterCoursePoint::getInfo() {
	return m_thi;
}

void CmdUpdateTreasureHunterCoursePoint::setInfo(TreasureHunterInfo& _thi) {
	m_thi = _thi;
}
