// Arquivo cmd_first_login_check.cpp
// Criado em 30/03/2018 as 20:19 por Acrisio
// Implementa��o da classe CmdFirstLoginCheck

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_first_login_check.hpp"

using namespace stdA;

CmdFirstLoginCheck::CmdFirstLoginCheck(bool _waiter) : pangya_db(_waiter), m_uid(0), m_check(false) {
}

CmdFirstLoginCheck::CmdFirstLoginCheck(uint32_t _uid, bool _waiter) : pangya_db(_waiter), m_uid(_uid), m_check(false) {
}

CmdFirstLoginCheck::~CmdFirstLoginCheck() {
}

void CmdFirstLoginCheck::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(1, (uint32_t)_result->cols);

	m_check = (IFNULL(atoi, _result->data[0]) == 1 ? true : false);
}

response* CmdFirstLoginCheck::prepareConsulta(database& _db) {

	m_check = false;

	auto r = consulta(_db, m_szConsulta + std::to_string(m_uid));

	checkResponse(r, "nao conseguiu verificar o first login do player: " + std::to_string(m_uid));

	return r;
}

uint32_t CmdFirstLoginCheck::getUID() {
	return m_uid;
}

void CmdFirstLoginCheck::setUID(uint32_t _uid) {
	m_uid = _uid;
}

bool CmdFirstLoginCheck::getLastCheck() {
	return m_check;
}
