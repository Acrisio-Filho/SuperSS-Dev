// Arquivo cmd_register_player_login.cpp
// Criado em 01/04/2018 as 18:43 por Acrisio
// Implementa��o da classe CmdRegisterPlayerLogin

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_register_player_login.hpp"

using namespace stdA;

CmdRegisterPlayerLogin::CmdRegisterPlayerLogin(bool _waiter) : pangya_db(_waiter), m_uid(0), m_ip(), m_server_uid(0) {
}

CmdRegisterPlayerLogin::CmdRegisterPlayerLogin(uint32_t _uid, std::string& _ip, uint32_t _server_uid, bool _waiter) 
		: pangya_db(_waiter), m_uid(_uid), m_ip(_ip), m_server_uid(_server_uid) {
}

CmdRegisterPlayerLogin::~CmdRegisterPlayerLogin() {
}

void CmdRegisterPlayerLogin::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdRegisterPlayerLogin::prepareConsulta(database& _db) {

	if (m_ip.empty())
		throw exception("[CmdRegisterPlayerLogin::prepareConsulta][Error] ip is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + _db.makeText(m_ip) + ", " + std::to_string(m_server_uid));

	checkResponse(r, "nao conseguiu registrar o login do player: " + std::to_string(m_uid) + ", IP: " + m_ip);

	return r;
}

uint32_t CmdRegisterPlayerLogin::getUID() {
	return m_uid;
}

void CmdRegisterPlayerLogin::setUID(uint32_t _uid) {
	m_uid = _uid;
}

std::string& CmdRegisterPlayerLogin::getIP() {
	return m_ip;
}

void CmdRegisterPlayerLogin::setIP(std::string& _ip) {
	m_ip = _ip;
}

uint32_t CmdRegisterPlayerLogin::getServerUID() {
	return m_server_uid;
}

void CmdRegisterPlayerLogin::setServerUID(uint32_t _server_uid) {
	m_server_uid = _server_uid;
}
