// Arquivo cmd_trophy_special.cpp
// Criado em 21/03/2018 as 23:08 por Acrisio
// Implementa��o da classe CmdTrophySpecial

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_trophy_special.hpp"

using namespace stdA;

CmdTrophySpecial::CmdTrophySpecial(bool _waiter) : pangya_db(_waiter), m_uid(0), m_type(NORMAL), m_season(CURRENT), v_tei() {
}

CmdTrophySpecial::CmdTrophySpecial(uint32_t _uid, TYPE_SEASON _season, TYPE _type, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_season(_season), m_type(_type), v_tei() {
}

CmdTrophySpecial::~CmdTrophySpecial() {
}

void CmdTrophySpecial::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(3, (uint32_t)_result->cols);

	TrofelEspecialInfo tei{ 0 };

	tei.id = IFNULL(atoi, _result->data[0]);
	tei._typeid = IFNULL(atoi, _result->data[1]);
	tei.qntd = IFNULL(atoi, _result->data[2]);

	v_tei.push_back(tei);
}

response* CmdTrophySpecial::prepareConsulta(database& _db) {

	v_tei.clear();
	v_tei.shrink_to_fit();

	auto r = procedure(_db, (m_type == NORMAL) ? m_szConsulta[0] : m_szConsulta[1], std::to_string(m_uid) + ", " + std::to_string(m_season));

	checkResponse(r, "nao conseguiu pegar o Trophy Special do player: " + std::to_string(m_uid));

	return r;
}

std::vector< TrofelEspecialInfo >& CmdTrophySpecial::getInfo() {
	return v_tei;
}

uint32_t CmdTrophySpecial::getUID() {
	return m_uid;
}

void CmdTrophySpecial::setUID(uint32_t _uid) {
	m_uid = _uid;
}

CmdTrophySpecial::TYPE_SEASON CmdTrophySpecial::getSeason() {
	return m_season;
}

void CmdTrophySpecial::setSeason(TYPE_SEASON _season) {
	m_season = _season;
}

CmdTrophySpecial::TYPE CmdTrophySpecial::getType() {
	return m_type;
}

void CmdTrophySpecial::setType(TYPE _type) {
	m_type = _type;
}
