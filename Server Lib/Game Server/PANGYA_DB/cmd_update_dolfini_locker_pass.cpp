// Arquivo cmd_update_dolfini_locker_pass.cpp
// Criado em 02/06/2018 as 15:24 por Acrisio
// Implementa��o da classe CmdUpdateDolfiniLockerPass

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_dolfini_locker_pass.hpp"

using namespace stdA;

CmdUpdateDolfiniLockerPass::CmdUpdateDolfiniLockerPass(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_pass() {
}

CmdUpdateDolfiniLockerPass::CmdUpdateDolfiniLockerPass(uint32_t _uid, std::string& _pass, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_pass(_pass) {
}

CmdUpdateDolfiniLockerPass::~CmdUpdateDolfiniLockerPass() {
}

void CmdUpdateDolfiniLockerPass::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// n�o usa por que � um UDPATE
	return;
}

response* CmdUpdateDolfiniLockerPass::prepareConsulta(database& _db) {

	if (m_pass.empty())
		throw exception("[CmdUpdateDolfiniLockerPass::prepareConsulta][Error] pass is empty", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_pass.size() > 7)
		throw exception("[CmdUpdateDolfiniLockerPass::prepareConsulta][Error] pass is hight of permited", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + _db.makeText(m_pass));

	checkResponse(r, "nao conseguiu atualizar a senha[value=" + m_pass + "] do dolfini locker do player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdUpdateDolfiniLockerPass::getUID() {
	return m_uid;
}

void CmdUpdateDolfiniLockerPass::setUID(uint32_t _uid) {
	m_uid = _uid;
}

std::string& CmdUpdateDolfiniLockerPass::getPass() {
	return m_pass;
}

void CmdUpdateDolfiniLockerPass::setPass(std::string& _pass) {
	m_pass = _pass;
}
