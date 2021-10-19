// Arquivo cmd_first_set_check.cpp
// Criado em 30/03/2018 as 20:35 por Acrisio
// Implementa��o da classe CmdFirstSetCheck

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_first_set_check.hpp"

using namespace stdA;

CmdFirstSetCheck::CmdFirstSetCheck(bool _waiter) : pangya_db(_waiter), m_uid(0), m_check(false) {
}

CmdFirstSetCheck::CmdFirstSetCheck(uint32_t _uid, bool _waiter) : pangya_db(_waiter), m_uid(_uid), m_check(false) {
}

CmdFirstSetCheck::~CmdFirstSetCheck() {
}

void CmdFirstSetCheck::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(1, (uint32_t)_result->cols);

	m_check = (IFNULL(atoi, _result->data[0]) == 1 ? true : false);
}

response* CmdFirstSetCheck::prepareConsulta(database& _db) {

	m_check = false;

	auto r = consulta(_db, m_szConsulta + std::to_string(m_uid));

	checkResponse(r, "nao conseguiu verificar o first set do player: " + std::to_string(m_uid));

	return r;
}

uint32_t CmdFirstSetCheck::getUID() {
	return m_uid;
}

void CmdFirstSetCheck::setUID(uint32_t _uid) {
	m_uid = _uid;
}

bool CmdFirstSetCheck::getLastCheck() {
	return m_check;
}
