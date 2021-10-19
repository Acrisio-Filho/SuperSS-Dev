// Arquivo cmd_register_logon_server.cpp
// Criado em 03/12/2018 as 20:11 por Acrisio
// Implementa��o da clasee CmdRegisterLogonServer

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_register_logon_server.hpp"

using namespace stdA;

CmdRegisterLogonServer::CmdRegisterLogonServer(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_server_id("") {
}

CmdRegisterLogonServer::CmdRegisterLogonServer(uint32_t _uid, std::string _server_id, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_server_id(_server_id) {
}

CmdRegisterLogonServer::~CmdRegisterLogonServer() {
}

void CmdRegisterLogonServer::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdRegisterLogonServer::prepareConsulta(database& _db) {

	if (m_uid == 0u)
		throw exception("[CmdRegisterLogonServer::prepareConsulta][Error] m_uid is invalid(zero).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_server_id.empty())
		throw exception("[CmdRegisterLogonServer::prepareConsulta][Error] m_server_id is invalid(empty).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + _db.makeText(m_server_id));

	checkResponse(r, "nao conseguiu Registrar o Logon do Player[UID=" + std::to_string(m_uid) + "] no Server[ID=" + m_server_id + "]");

	return r;
}

uint32_t CmdRegisterLogonServer::getUID() {
	return m_uid;
}

void CmdRegisterLogonServer::setUID(uint32_t _uid) {
	m_uid = _uid;
}

std::string& CmdRegisterLogonServer::getServerId() {
	return m_server_id;
}

void CmdRegisterLogonServer::setServerId(std::string& _server_id) {
	m_server_id = _server_id;
}
