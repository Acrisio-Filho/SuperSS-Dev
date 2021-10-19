// Arquivo cmd_register_logon.cpp
// Criado em 01/04/2018 as 21:45 por Acrisio
// Implementação da classe CmdRegisterLogon

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_register_logon.hpp"

using namespace stdA;

CmdRegisterLogon::CmdRegisterLogon(bool _waiter) : pangya_db(_waiter), m_uid(0), m_option(0) {
}

CmdRegisterLogon::CmdRegisterLogon(uint32_t _uid, int _option, bool _waiter) : pangya_db(_waiter), m_uid(_uid), m_option(_option) {
}

CmdRegisterLogon::~CmdRegisterLogon() {
}

void CmdRegisterLogon::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

    // Não usa por que é um UPDATE
    return;
}

response* CmdRegisterLogon::prepareConsulta(database& _db) {

    auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_option));

    checkResponse(r, "nao conseguiu registrar o logon do player: " + std::to_string(m_uid) + ", na option: " + std::to_string(m_option));

    return r;
}

uint32_t CmdRegisterLogon::getUID() {
    return m_uid;
}

void CmdRegisterLogon::setUID(uint32_t _uid) {
    m_uid = _uid;
}

int CmdRegisterLogon::getOption() {
    return m_option;
}

void CmdRegisterLogon::setOption(int _option) {
    m_option = _option;
}
