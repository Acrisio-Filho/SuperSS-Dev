// Arquivo cmd_gera_web_key.cpp
// Criado em 25/03/2018 as 17:23 por Acrisio
// Implementa��o da classe CmdGeraWebKey

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_gera_web_key.hpp"

using namespace stdA;

CmdGeraWebKey::CmdGeraWebKey(bool _waiter) : pangya_db(_waiter), m_uid(0), m_web_key() {
}

CmdGeraWebKey::CmdGeraWebKey(uint32_t _uid, bool _waiter) : pangya_db(_waiter), m_uid(_uid), m_web_key() {
}

CmdGeraWebKey::~CmdGeraWebKey() {
}

void CmdGeraWebKey::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(1, (uint32_t)_result->cols);

	if (is_valid_c_string(_result->data[0]))
		m_web_key = _result->data[0];
}

response* CmdGeraWebKey::prepareConsulta(database& _db) {

	m_web_key = "";

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid));

	checkResponse(r, "nao conseguiu pegar weblink key do player: " + std::to_string(m_uid));

	return r;
}

std::string& CmdGeraWebKey::getKey() {
	return m_web_key;
}

uint32_t CmdGeraWebKey::getUID() {
	return m_uid;
}

void CmdGeraWebKey::setUID(uint32_t _uid) {
	m_uid = _uid;
}
