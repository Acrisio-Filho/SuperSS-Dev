// Arquivo cmd_cookie.cpp
// Criado em 19/03/2018 as 21:37 por Acrisio
// Implementação da classe CmdCookie

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_cookie.hpp"

using namespace stdA;

CmdCookie::CmdCookie(bool _waiter) : pangya_db(_waiter), m_uid(0), m_cookie(0ull) {
}

CmdCookie::CmdCookie(uint32_t _uid, bool _waiter) : pangya_db(_waiter), m_uid(_uid), m_cookie(0ull) {
}

CmdCookie::~CmdCookie() {
}

void CmdCookie::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

    checkColumnNumber(2, (uint32_t)_result->cols);

    uint32_t uid_req = IFNULL(atoi, _result->data[0]);
    m_cookie = IFNULL(atoll, _result->data[1]);

    if (uid_req != m_uid)
        throw exception("[CmdCookie::lineResult][Error] retornou outro uid do que foi requisitado. uid_req: " + std::to_string(uid_req) + " != " + std::to_string(m_uid), STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 3, 0));
}

response* CmdCookie::prepareConsulta(database& _db) {

	if (m_uid == 0u)
		throw exception("[CmdCookie::prepareConsulta][Error] m_uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

    m_cookie = 0ull;

    auto r = consulta(_db, m_szConsulta + std::to_string(m_uid));

    checkResponse(r, "nao conseguiu pegar o cookie do player: " + std::to_string(m_uid));

    return r;
}

uint64_t CmdCookie::getCookie() {
    return m_cookie;
}

void CmdCookie::setCookie(uint64_t _cookie) {
    m_cookie = _cookie;
}

uint32_t CmdCookie::getUID() {
    return m_uid;
}

void CmdCookie::setUID(uint32_t _uid) {
    m_uid = _uid;
}
