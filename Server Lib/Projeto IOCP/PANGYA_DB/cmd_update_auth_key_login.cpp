// Arquivo cmd_update_auth_key_login.cpp
// Criado em 07/04/2018 as 18:15 por Acrisio
// Implementação da classe CmdUpdateAuthKeyLogin

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_auth_key_login.hpp"

using namespace stdA;

CmdUpdateAuthKeyLogin::CmdUpdateAuthKeyLogin(bool _waiter) : pangya_db(_waiter), m_uid(0), m_valid(0) {
}

CmdUpdateAuthKeyLogin::CmdUpdateAuthKeyLogin(uint32_t _uid, unsigned char _valid, bool _waiter)
        : pangya_db(_waiter), m_uid(_uid), m_valid(_valid) {
}

CmdUpdateAuthKeyLogin::~CmdUpdateAuthKeyLogin() {
}

void CmdUpdateAuthKeyLogin::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

    // Não usa por que é um UPDATE
}

response* CmdUpdateAuthKeyLogin::prepareConsulta(database& _db) {

    auto r = _update(_db, m_szConsulta[0] + std::to_string((unsigned short)m_valid) + m_szConsulta[1] + std::to_string(m_uid));

    checkResponse(r, "nao conseguiu atualizar a auth key login do player: " + std::to_string(m_uid));

    return r;
}

uint32_t CmdUpdateAuthKeyLogin::getUID() {
    return m_uid;
}

void CmdUpdateAuthKeyLogin::setUID(uint32_t _uid) {
    m_uid = _uid;
}

unsigned char CmdUpdateAuthKeyLogin::getValid() {
    return m_valid;
}

void CmdUpdateAuthKeyLogin::setValid(unsigned char _valid) {
    m_valid = _valid;
}
