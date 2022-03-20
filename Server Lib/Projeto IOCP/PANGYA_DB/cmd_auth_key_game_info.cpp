// Arquivo cmd_auth_key_game_info.cpp
// Criado em 01/04/2018 as 20:15 por Acrisio
// Implementação da classe CmdAuthKeyGameInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_auth_key_game_info.hpp"
#include "../UTIL/exception.h"
#include "../TYPE/stda_error.h"

using namespace stdA;

CmdAuthKeyGameInfo::CmdAuthKeyGameInfo(bool _waiter) : pangya_db(_waiter), m_uid(0), m_server_uid(0), m_akgi{} {
}

CmdAuthKeyGameInfo::CmdAuthKeyGameInfo(uint32_t _uid, uint32_t _server_uid, bool _waiter)
        : pangya_db(_waiter), m_uid(_uid), m_server_uid(_server_uid), m_akgi{} {
}

CmdAuthKeyGameInfo::~CmdAuthKeyGameInfo() {
}

void CmdAuthKeyGameInfo::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

    checkColumnNumber(3, (uint32_t)_result->cols);

    if (is_valid_c_string(_result->data[0]))
        STRCPY_TO_MEMORY_FIXED_SIZE(m_akgi.key, sizeof(m_akgi.key), _result->data[0]);
    m_akgi.server_uid = IFNULL(atoi, _result->data[1]);
    m_akgi.valid = (unsigned char)IFNULL(atoi, _result->data[2]);

    if (m_akgi.key[0] == '\0')
        throw exception("[CmdAuthKeyGameInfo::lineResult][Error] a consulta retornou um auth key game invalido", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 3, 0));

    if (m_akgi.server_uid != m_server_uid)
        throw exception("[CmdAuthKeyGameInfo::lineResult][Error] o server uid retornado na consulta nao eh igual ao requisitado. server uid req: " 
                            + std::to_string(m_server_uid) + " != " + std::to_string(m_akgi.server_uid), 
                STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 3, 1));
}

response* CmdAuthKeyGameInfo::prepareConsulta(database& _db) {

    m_akgi.clear();

    auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_server_uid));

    checkResponse(r, "nao conseguiu pegar o auth key game do player: " + std::to_string(m_uid) + ", do server uid: " + std::to_string(m_server_uid));

    return r;
}

uint32_t CmdAuthKeyGameInfo::getUID() {
    return m_uid;
}

void CmdAuthKeyGameInfo::setUID(uint32_t _uid) {
    m_uid = _uid;
}

uint32_t CmdAuthKeyGameInfo::getServerUID() {
    return m_server_uid;
}

void CmdAuthKeyGameInfo::setServerUID(uint32_t _server_uid) {
    m_server_uid = _server_uid;
}

AuthKeyGameInfo& CmdAuthKeyGameInfo::getInfo() {
    return m_akgi;
}

void CmdAuthKeyGameInfo::setInfo(AuthKeyGameInfo& _akgi) {
    m_akgi = _akgi;
}
