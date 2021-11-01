// Arquivo cmd_verify_ip.cpp
// Criado em 30/030/2018 as 10:05 por Acrisio
// Implementa��o da classe CmdVerifyIP

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_verify_ip.hpp"

using namespace stdA;

CmdVerifyIP::CmdVerifyIP(bool _waiter) : pangya_db(_waiter), m_uid(0), m_ip() {
}

CmdVerifyIP::CmdVerifyIP(uint32_t _uid, std::string _ip, bool _waiter) : pangya_db(_waiter), m_uid(_uid), m_ip(_ip) {
}

CmdVerifyIP::~CmdVerifyIP() {
}

void CmdVerifyIP::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(1, (uint32_t)_result->cols);

	uint32_t uid_req = IFNULL(atoi, _result->data[0]);

	if (uid_req != m_uid)
		throw exception("[CmdVerifyIP::lineResult][Error] o uid recuperado para verificar o ip access do player e diferente. UID_req: " + std::to_string(m_uid) + " != " + std::to_string(uid_req), STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 3, 0));

	m_last_verify = true;
}

response* CmdVerifyIP::prepareConsulta(database& _db) {

	m_last_verify = false;

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + _db.makeText(m_ip));

	checkResponse(r, "nao conseguiu verificar o ip de accesso do player: " + std::to_string(m_uid));

	return r;
}

uint32_t CmdVerifyIP::getUID() {
	return m_uid;
}

void CmdVerifyIP::setUID(uint32_t _uid) {
	m_uid = _uid;
}

std::string& CmdVerifyIP::getIP() {
	return m_ip;
}

void CmdVerifyIP::setIP(std::string& _ip) {
	m_ip = _ip;
}

bool CmdVerifyIP::getLastVerify() {
	return m_last_verify;
}
