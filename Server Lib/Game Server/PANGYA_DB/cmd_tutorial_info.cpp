// Arquivo cmd_tutorial_info.cpp
// Criado em 18/03/2018 as 22:23 por Acrisio
// Implementa��o da classe CmdTutorialInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_tutorial_info.hpp"

using namespace stdA;

CmdTutorialInfo::CmdTutorialInfo(bool _waiter) : pangya_db(_waiter), m_uid(0), m_ti{0} {
}

CmdTutorialInfo::CmdTutorialInfo(uint32_t _uid, bool _waiter) : pangya_db(_waiter), m_uid(_uid), m_ti{0} {
}

CmdTutorialInfo::~CmdTutorialInfo() {
}

void CmdTutorialInfo::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(3, (uint32_t)_result->cols);

	m_ti.rookie = IFNULL(atoi, _result->data[0]);
	m_ti.beginner = IFNULL(atoi, _result->data[1]);
	m_ti.advancer = IFNULL(atoi, _result->data[2]);
}

response* CmdTutorialInfo::prepareConsulta(database& _db) {
	
	m_ti.clear();

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid));

	checkResponse(r, "nao conseguiu pegar o tutorial info do player: " + std::to_string(m_uid));

	return r;
}

TutorialInfo& CmdTutorialInfo::getInfo() {
	return m_ti;
}

void CmdTutorialInfo::setInfo(TutorialInfo& _ti) {
	m_ti = _ti;
}

uint32_t CmdTutorialInfo::getUID() {
	return m_uid;
}

void CmdTutorialInfo::setUID(uint32_t _uid) {
	m_uid = _uid;
}
