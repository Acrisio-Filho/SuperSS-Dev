// Arquivo cmd_auth_server_key.cpp
// Criado em 15/12/2018 as 15:25 por Acrisio
// Implementação da classe CmdAuthServerKey

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_auth_server_key.hpp"

using namespace stdA;

CmdAuthServerKey::CmdAuthServerKey(bool _waiter) : pangya_db(_waiter), m_server_uid(0u), m_ask{ 0 } {
}

CmdAuthServerKey::CmdAuthServerKey(uint32_t _server_uid, bool _waiter)
    : pangya_db(_waiter), m_server_uid(_server_uid), m_ask{ 0 }  {
}

CmdAuthServerKey::~CmdAuthServerKey() {
}

void CmdAuthServerKey::lineResult(result_set::ctx_res* _result, uint32_t /* _index_result */) {

    checkColumnNumber(3, (uint32_t)_result->cols);

    m_ask.server_uid = (uint32_t)IFNULL(atoi, _result->data[0]);

    if (is_valid_c_string(_result->data[1]))
        STRCPY_TO_MEMORY_FIXED_SIZE(m_ask.key, sizeof(m_ask.key), _result->data[1]);

    m_ask.valid = (unsigned char)IFNULL(atoi, _result->data[2]);

    if (m_ask.server_uid != m_server_uid)
        throw exception("[CmdAuthServerKey::lineResult][Error] m_ask.server_uid = " + std::to_string(m_ask.server_uid) 
            + " not match with m_server_uid = " + std::to_string(m_server_uid), STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 3, 0));
}

response* CmdAuthServerKey::prepareConsulta(database& _db) {

    if (m_server_uid == 0u)
        throw exception("[CmdAuthServerKey::prepareConsulta][Error] m_server_uid is invalid(zero).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

    // Clear
    m_ask.clear();

    auto r = consulta(_db, m_szConsulta + std::to_string(m_server_uid));

    checkResponse(r, "nao conseguiu pegar o Auth Server Key do Server[UID=" + std::to_string(m_server_uid) + "]");

    return r;
}

uint32_t CmdAuthServerKey::getServerUID() {
    return m_server_uid;
}

void CmdAuthServerKey::setServerUID(uint32_t _server_uid) {
    m_server_uid = _server_uid;
}

AuthServerKey& CmdAuthServerKey::getInfo() {
    return m_ask;
}
