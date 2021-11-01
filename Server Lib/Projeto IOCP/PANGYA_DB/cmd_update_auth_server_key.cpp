// Arquivo cmd_update_auth_server_key.cpp
// Criado em 15/12/2018 as 18:53 por Acrisio
// Implementação da classe CmdUpdateAuthServerKey

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_auth_server_key.hpp"

using namespace stdA;

CmdUpdateAuthServerKey::CmdUpdateAuthServerKey(bool _waiter) : pangya_db(_waiter), m_ask{ 0 } {
}

CmdUpdateAuthServerKey::CmdUpdateAuthServerKey(AuthServerKey& _ask, bool _waiter)
    : pangya_db(_waiter), m_ask(_ask) {
}

CmdUpdateAuthServerKey::~CmdUpdateAuthServerKey() {
}

void CmdUpdateAuthServerKey::lineResult(result_set::ctx_res* /* _result */, uint32_t /* _index_result */) {

    // Não usa por que é um UPDATE
    return;
}

response* CmdUpdateAuthServerKey::prepareConsulta(database& _db) {

    if (m_ask.server_uid == 0u)
        throw exception("[CmdUpdateAuthServerKey::prepareConsulta][Error] AuthServerKey m_ask.server_uid is invalid(zero).", 
            STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

    std::string key = "null";

    if (!std::empty(m_ask.key))
        key = _db.makeText(m_ask.key);

    auto r = procedure(_db, m_szConsulta, std::to_string(m_ask.server_uid) + ", " + key + ", " + std::to_string((unsigned short)m_ask.valid));

    checkResponse(r, "nao conseguiu atualizar Auth Server Key[SERVER_UID=" + std::to_string(m_ask.server_uid) 
            + ", KEY=" + key + ", VALID=" + std::to_string((unsigned short)m_ask.valid) + "]");

    return r;
}

AuthServerKey& CmdUpdateAuthServerKey::getInfo() {
    return m_ask;
}

void CmdUpdateAuthServerKey::setInto(AuthServerKey& _ask) {
    m_ask = _ask;
}
