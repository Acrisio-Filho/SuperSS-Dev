// Arquivo cmd_update_level_and_exp.cpp
// Criado em 06/07/0218 as 20:26 por Acrisio
// Implementa��o da classe CmdUpdateLevelAndExp

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_level_and_exp.hpp"

using namespace stdA;

CmdUpdateLevelAndExp::CmdUpdateLevelAndExp(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_level(0u), m_exp(0u) {
}

CmdUpdateLevelAndExp::CmdUpdateLevelAndExp(uint32_t _uid, unsigned char _level, uint32_t _exp, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_level(_level), m_exp(_exp) {
}

CmdUpdateLevelAndExp::~CmdUpdateLevelAndExp() {
}

void CmdUpdateLevelAndExp::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdUpdateLevelAndExp::prepareConsulta(database& _db) {

	if (m_uid == 0)
		throw exception("[CmdUpdateLevelAndExp::prepareConsulta][Error] m_uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = _update(_db, m_szConsulta[0] + std::to_string((unsigned short)m_level) + m_szConsulta[1] + std::to_string(m_exp) + m_szConsulta[2] + std::to_string(m_uid));

	checkResponse(r, "nao conseguiu atualizar Level[value=" + std::to_string((unsigned short)m_level) + "] Exp[value=" + std::to_string(m_exp) + "] do player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdUpdateLevelAndExp::getUID() {
	return m_uid;
}

void CmdUpdateLevelAndExp::setUID(uint32_t _uid) {
	m_uid = _uid;
}

unsigned char CmdUpdateLevelAndExp::getLevel() {
	return m_level;
}

void CmdUpdateLevelAndExp::setLevel(unsigned char _level) {
	m_level = _level;
}

uint32_t CmdUpdateLevelAndExp::getExp() {
	return m_exp;
}

void CmdUpdateLevelAndExp::setExp(uint32_t _exp) {
	m_exp = _exp;
}
