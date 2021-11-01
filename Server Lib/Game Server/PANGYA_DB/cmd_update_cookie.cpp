// Arquivo cmd_update_cookie.cpp
// Criado em 13/05/2018 as 11:23 por Acrisio
// Implementa��o da classe CmdUpdateCookie

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_cookie.hpp"

using namespace stdA;

CmdUpdateCookie::CmdUpdateCookie(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_cookie(0ull), m_type_update(INCREASE) {
}

CmdUpdateCookie::CmdUpdateCookie(uint32_t _uid, uint64_t _cookie, T_UPDATE_COOKIE _type_update, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_cookie(_cookie), m_type_update(_type_update) {
}

CmdUpdateCookie::~CmdUpdateCookie() {
}

void CmdUpdateCookie::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdUpdateCookie::prepareConsulta(database& _db) {

	auto r = _update(_db, m_szConsulta[0] + std::string(m_type_update == INCREASE ? " + " : " - ") + std::to_string(m_cookie) + m_szConsulta[1] + std::to_string(m_uid));

	checkResponse(r, "nao conseguiu atualizar o cookie[value=" + std::string(m_type_update == INCREASE ? " + " : " - ") + std::to_string(m_cookie) + "] do player: " + std::to_string(m_uid));

	return r;
}

uint32_t CmdUpdateCookie::getUID() {
	return m_uid;
}

void CmdUpdateCookie::setUID(uint32_t _uid) {
	m_uid = _uid;
}

uint64_t CmdUpdateCookie::getCookie() {
	return m_cookie;
}

void CmdUpdateCookie::setCookie(uint64_t _cookie) {
	m_cookie = _cookie;
}

CmdUpdateCookie::T_UPDATE_COOKIE CmdUpdateCookie::getTypeUpdate() {
	return m_type_update;
}

void CmdUpdateCookie::setTypeUpdate(T_UPDATE_COOKIE _type_update) {
	m_type_update = _type_update;
}
