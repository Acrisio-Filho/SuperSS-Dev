// Arquivo cmd_new_auth_server_key.cpp
// Criado em 15/12/2018 as 15:43 por Acrisio
// Implementação da classe CmdNewAuthServerKey

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_new_auth_server_key.hpp"
#include <cstring>

using namespace stdA;

CmdNewAuthServerKey::CmdNewAuthServerKey(bool _waiter) : pangya_db(_waiter), m_server_uid(0u), m_key("") {
}

CmdNewAuthServerKey::CmdNewAuthServerKey(uint32_t _server_uid, bool _waiter)
    : pangya_db(_waiter), m_server_uid(_server_uid), m_key("") {
}

CmdNewAuthServerKey::~CmdNewAuthServerKey() {
}

void CmdNewAuthServerKey::lineResult(result_set::ctx_res* _result, uint32_t /* _index_result */) {

    checkColumnNumber(1, (uint32_t)_result->cols);

    if (is_valid_c_string(_result->data[0]))
        m_key.assign(_result->data[0], _result->data[0] + strlen(_result->data[0]));
}

response* CmdNewAuthServerKey::prepareConsulta(database& _db) {

    if (m_server_uid == 0u)
        throw exception("[CmdNewAuthServerKey::prepareConsulta][Error] m_server_uid is invalid(zero).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

    auto r = procedure(_db, m_szConsulta, std::to_string(m_server_uid));

    checkResponse(r, "Server[UID=" + std::to_string(m_server_uid) + "] nao conseguiu gerar uma nova key para o Auth Server");

    return r;
}

uint32_t CmdNewAuthServerKey::getServerUID() {
    return m_server_uid;
}

void CmdNewAuthServerKey::setServerUID(uint32_t _server_uid) {
    m_server_uid = _server_uid;
}

std::string& CmdNewAuthServerKey::getInfo() {
    return m_key;
}
