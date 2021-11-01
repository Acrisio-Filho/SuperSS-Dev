// Arquivo cmd_register_logon_server.cpp
// Criado em 0/04/2018 as 20:52 por Acrisio
// Implementa��o da classe CmdRegisterLogonServer

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_register_logon_server.hpp"

using namespace stdA;

CmdRegisterLogonServer::CmdRegisterLogonServer(bool _waiter) : pangya_db(_waiter), m_uid(0), m_server_uid(0) {
}

CmdRegisterLogonServer::CmdRegisterLogonServer(uint32_t _uid, uint32_t _server_uid, bool _waiter)
		: pangya_db(_waiter), m_uid(_uid), m_server_uid(_server_uid) {
}

CmdRegisterLogonServer::~CmdRegisterLogonServer() {
}

void CmdRegisterLogonServer::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdRegisterLogonServer::prepareConsulta(database& _db) {

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_server_uid));

	checkResponse(r, "nao conseguiu registrar o logon no server[UID=" + std::to_string(m_server_uid) + "] do player: " + std::to_string(m_uid));

	return r;
}

uint32_t CmdRegisterLogonServer::getUID() {
	return m_uid;
}

void CmdRegisterLogonServer::setUID(uint32_t _uid) {
	m_uid = _uid;
}

uint32_t CmdRegisterLogonServer::getServerUID() {
	return m_server_uid;
}

void CmdRegisterLogonServer::setServerUID(uint32_t _server_uid) {
	m_server_uid = _server_uid;
}
