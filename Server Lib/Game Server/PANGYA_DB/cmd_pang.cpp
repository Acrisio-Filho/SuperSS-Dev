// Arquivo cmd_pang.cpp
// Criado em 03/10/2019 as 18:15 por Acrisio
// Implementa��o da classe CmdPang

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_pang.hpp"

using namespace stdA;

CmdPang::CmdPang(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_pang(0ull) {
}

CmdPang::CmdPang(uint32_t _uid, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_pang(0ull) {
}

CmdPang::~CmdPang() {
}

void CmdPang::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(2, (uint32_t)_result->cols);

	uint32_t uid_req = (uint32_t)IFNULL(atoi, _result->data[0]);
	m_pang = (uint64_t)IFNULL(atoll, _result->data[1]);

	if (uid_req != m_uid)
		throw exception("[CmdPang::lineResult][Error] retornou outro uid do que foi requisitado. uid_req " + std::to_string(uid_req) + " != " + std::to_string(m_uid) , STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 3, 0));
}

response* CmdPang::prepareConsulta(database& _db) {

	if (m_uid == 0u)
		throw exception("[CmdPang::prepareConsulta][Error] m_uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	m_pang = 0ull;

	auto r = consulta(_db, m_szConsulta + std::to_string(m_uid));

	checkResponse(r, "nao conseguiu pegar o pang do player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdPang::getUID() {
	return m_uid;
}

void CmdPang::setUID(uint32_t _uid) {
	m_uid = _uid;
}

uint64_t CmdPang::getPang() {
	return m_pang;
}

void CmdPang::setPang(uint64_t _pang) {
	m_pang = _pang;
}
