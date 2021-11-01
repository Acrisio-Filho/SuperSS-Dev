// Arquivo cmd_check_achievement.cpp
// Criado em 07/04/2018 as 19:08 por Acrisio
// Implementa��o da classe CmdCheckAchievement

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_check_achievement.hpp"

using namespace stdA;

CmdCheckAchievement::CmdCheckAchievement(bool _waiter) : pangya_db(_waiter), m_uid(0), m_check(false) {
}

CmdCheckAchievement::CmdCheckAchievement(uint32_t _uid, bool _waiter) : pangya_db(_waiter), m_uid(_uid), m_check(false) {
}

CmdCheckAchievement::~CmdCheckAchievement() {
}

void CmdCheckAchievement::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(1, (uint32_t)_result->cols);

	m_check = (IFNULL(atoi, _result->data[0]) > 0 ? true : false);
}

response* CmdCheckAchievement::prepareConsulta(database& _db) {

	m_check = false;

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid));

	checkResponse(r, "nao conseguiu verificar o achievement do player: " + std::to_string(m_uid));

	return r;
}

uint32_t CmdCheckAchievement::getUID() {
	return m_uid;
}

void CmdCheckAchievement::setUID(uint32_t _uid) {
	m_uid = _uid;
}

bool CmdCheckAchievement::getLastState() {
	return m_check;
}
