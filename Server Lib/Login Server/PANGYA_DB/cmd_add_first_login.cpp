// Arquivo cmd_add_first_login.cpp
// Criado em 01/04/2018 as 16:21 por Acrisio
// Implementa��o da classe CmdAddFirstLogin

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_add_first_login.hpp"

using namespace stdA;

CmdAddFirstLogin::CmdAddFirstLogin(bool _waiter) : pangya_db(_waiter), m_uid(0), m_flag(0) {
}

CmdAddFirstLogin::CmdAddFirstLogin(uint32_t _uid, unsigned char _flag, bool _waiter) : pangya_db(_waiter), m_uid(_uid), m_flag(_flag) {
}

CmdAddFirstLogin::~CmdAddFirstLogin() {
}

void CmdAddFirstLogin::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
}

response* CmdAddFirstLogin::prepareConsulta(database& _db) {

	auto r = _update(_db, m_szConsulta[0] + std::to_string((unsigned short)m_flag) + m_szConsulta[1] + std::to_string(m_uid));

	checkResponse(r, "nao conseguiu atualizar o first login do player: " + std::to_string(m_uid));

	return r;
}

uint32_t CmdAddFirstLogin::getUID() {
	return m_uid;
}

void CmdAddFirstLogin::setUID(uint32_t _uid) {
	m_uid = _uid;
}

unsigned char CmdAddFirstLogin::getFLag() {
	return m_flag;
}

void CmdAddFirstLogin::setFlag(unsigned char _flag) {
	m_flag = _flag;
}
