// Arquivo cmd_insert_ticker.cpp
// Criado em 04/12/2018 as 01:52 por Acrisio
// Implementa��o da classe CmdInsertTicker

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_insert_ticker.hpp"

using namespace stdA;

CmdInsertTicker::CmdInsertTicker(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_server_uid(0u), m_msg("") {
}

CmdInsertTicker::CmdInsertTicker(uint32_t _uid, uint32_t _server_uid, std::string& _msg, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_server_uid(_server_uid), m_msg(_msg) {
}

CmdInsertTicker::~CmdInsertTicker() {
}

void CmdInsertTicker::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdInsertTicker::prepareConsulta(database& _db) {

	if (m_uid == 0u)
		throw exception("[CmdInsertTicker::prepareConsulta][Error] m_uid is invalid(zero).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_server_uid == 0u)
		throw exception("[CmdInsertTicker::prepareConsulta][Error] m_server_uid is invalid(zero).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_server_uid) + ", " + _db.makeText(m_msg));

	checkResponse(r, "nao conseguiu adicionar um Ticker[MESSAGE=" + m_msg + "] para o player[UID=" + std::to_string(m_uid) + "] no Server[UID=" + std::to_string(m_server_uid) + "]");

	return r;
}

uint32_t CmdInsertTicker::getUID() {
	return m_uid;
}

void CmdInsertTicker::setUID(uint32_t _uid) {
	m_uid = _uid;
}

uint32_t CmdInsertTicker::getServerUID() {
	return m_server_uid;
}

void CmdInsertTicker::setServerUID(uint32_t _server_uid) {
	m_server_uid = _server_uid;
}

std::string& CmdInsertTicker::getMessage() {
	return m_msg;
}

void CmdInsertTicker::setMessage(std::string& _msg) {
	m_msg = _msg;
}
