// Arquivo cmd_logon_check.cpp
// Criado em 30/03/2018 as 18:29 por Acrisio
// Implementa��o da classe CmdLogonCheck

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_logon_check.hpp"

using namespace stdA;

CmdLogonCheck::CmdLogonCheck(bool _waiter) : pangya_db(_waiter), m_uid(0), m_server_uid(0), m_check(false) {
}

CmdLogonCheck::CmdLogonCheck(uint32_t _uid, bool _waiter) : pangya_db(_waiter), m_uid(_uid), m_server_uid(0), m_check(false) {
}

CmdLogonCheck::~CmdLogonCheck() {
}

void CmdLogonCheck::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(2, (uint32_t)_result->cols);

	m_check = (IFNULL(atoi, _result->data[0]) == 1) ? true : false;
	m_server_uid = IFNULL(atoi, _result->data[1]);
}

response* CmdLogonCheck::prepareConsulta(database& _db) {

	m_check = false;
	m_server_uid = 0;

	auto r = consulta(_db, m_szConsulta + std::to_string(m_uid));

	checkResponse(r, "nao conseguiu verificar o logon do player: " + std::to_string(m_uid));

	return r;
}

uint32_t CmdLogonCheck::getUID() {
	return m_uid;
}

void CmdLogonCheck::setUID(uint32_t _uid) {
	m_uid = _uid;
}

bool CmdLogonCheck::getLastCheck() {
	return m_check;
}

uint32_t CmdLogonCheck::getServerUID() {
	return m_server_uid;
}
