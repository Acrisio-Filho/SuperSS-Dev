// Arquivo cmd_create_user.cpp
// Criado em 31/03/2018 as 16:01 por Acrisio
// implementa��o da classe CmdCreateUser

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_create_user.hpp"

using namespace stdA;

CmdCreateUser::CmdCreateUser(bool _waiter) : pangya_db(_waiter), m_id(), m_pass(), m_ip(), m_server_uid(0), m_uid(0) {
}

CmdCreateUser::CmdCreateUser(std::string& _id, std::string& _pass, std::string& _ip, uint32_t _server_uid, bool _waiter)
		: pangya_db(_waiter), m_id(_id), m_pass(_pass), m_ip(_ip), m_server_uid(_server_uid), m_uid(0) {
}

CmdCreateUser::~CmdCreateUser() {
}

void CmdCreateUser::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(1, (uint32_t)_result->cols);

	m_uid = IFNULL(atoi, _result->data[0]);
}

response* CmdCreateUser::prepareConsulta(database& _db) {

	m_uid = 0;

	if (m_id.empty() || m_pass.empty() || m_ip.empty())
		throw exception("[CmdCreateUser::prepareConsulta][Error] argumentos invalidos.[ID=" + m_id +",PASSWORD=" + m_pass + ",IP=" + m_ip + "]", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, _db.makeText(m_id) + ", " + _db.makeText(m_pass) + ", " + _db.makeText(m_ip) + ", " + std::to_string(m_server_uid));

	checkResponse(r, "nao conseguiu criar um usuario[ID=" + m_id + ",PASSWORD=" + m_pass + ",IP=" + m_ip + ",SERVER UID=" + std::to_string(m_server_uid) + "]");

	return r;
}

std::string& CmdCreateUser::getID() {
	return m_id;
}

void CmdCreateUser::setID(std::string& _id) {
	m_id = _id;
}

std::string& CmdCreateUser::getPASS() {
	return m_pass;
}

void CmdCreateUser::setPass(std::string& _pass) {
	m_pass = _pass;
}

std::string& CmdCreateUser::getIP() {
	return m_ip;
}

void CmdCreateUser::setIP(std::string& _ip) {
	m_ip = _ip;
}

uint32_t CmdCreateUser::getServerUID() {
	return m_server_uid;
}

void CmdCreateUser::setServerUID(uint32_t _server_uid) {
	m_server_uid = _server_uid;
}

uint32_t CmdCreateUser::getUID() {
	return m_uid;
}
