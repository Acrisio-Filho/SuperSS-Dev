// Arquivo cmd_auth_key_login_info.cpp
// Criado em 01/04/2018 as 20:00 por Acrisio
// Implementação da classe CmdAuthKeyLoginInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_auth_key_login_info.hpp"
#include "../UTIL/exception.h"
#include "../TYPE/stda_error.h"

using namespace stdA;

CmdAuthKeyLoginInfo::CmdAuthKeyLoginInfo(bool _waiter) : pangya_db(_waiter), m_uid(0), m_akli{0} {
}

CmdAuthKeyLoginInfo::CmdAuthKeyLoginInfo(uint32_t _uid, bool _waiter) : pangya_db(_waiter), m_uid(_uid), m_akli{0} {
}

CmdAuthKeyLoginInfo::~CmdAuthKeyLoginInfo() {
}

void CmdAuthKeyLoginInfo::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

    checkColumnNumber(2, (uint32_t)_result->cols);

    if (is_valid_c_string(_result->data[0]))
        STRCPY_TO_MEMORY_FIXED_SIZE(m_akli.key, sizeof(m_akli.key), _result->data[0]);
    m_akli.valid = (unsigned char)IFNULL(atoi, _result->data[1]);

    if (m_akli.key[0] == '\0')
        throw exception("[CmdAuthKeyLoginInfo::lineResult][Error] a consulta retornou uma auth key login invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 3, 0));
}

response* CmdAuthKeyLoginInfo::prepareConsulta(database& _db) {

    m_akli.clear();

    auto r = procedure(_db, m_szConsulta, std::to_string(m_uid));

    checkResponse(r, "nao conseguiu pegar a auth key login do player: " + std::to_string(m_uid));

    return r;
}

uint32_t CmdAuthKeyLoginInfo::getUID() {
    return m_uid;
}

void CmdAuthKeyLoginInfo::setUID(uint32_t _uid) {
    m_uid = _uid;
}

AuthKeyLoginInfo& CmdAuthKeyLoginInfo::getInfo() {
    return m_akli;
}

void CmdAuthKeyLoginInfo::setInfo(AuthKeyLoginInfo& _akli) {
    m_akli = _akli;
}
