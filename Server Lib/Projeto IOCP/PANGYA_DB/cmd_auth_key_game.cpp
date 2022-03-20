// Arquivo cmd_auth_key_game.cpp
// Criado em 18/03/2018 as 10:52 por Acrisio
// Implementação da classe CmdAuthKeyGame

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_auth_key_game.hpp"

using namespace stdA;

CmdAuthKeyGame::CmdAuthKeyGame(bool _waiter) : pangya_db(_waiter), m_uid(0), m_server_uid(0), m_auth_key_game("") {
};

CmdAuthKeyGame::CmdAuthKeyGame(uint32_t _uid, uint32_t _server_uid, bool _waiter)
        : pangya_db(_waiter), m_uid(_uid), m_server_uid(_server_uid), m_auth_key_game("") {
};

CmdAuthKeyGame::~CmdAuthKeyGame() {
};

void CmdAuthKeyGame::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

    checkColumnNumber(1, (uint32_t)_result->cols);

    if (is_valid_c_string(_result->data[0]))
        m_auth_key_game = std::string(_result->data[0]);

    if (m_auth_key_game.empty())
        throw exception("[CmdAuthKey::lineResult][Error] retornou nulo na consulta auth key do player: " + std::to_string(m_uid), STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 3, 0));
};

response* CmdAuthKeyGame::prepareConsulta(database& _db) {

    m_auth_key_game = "";

    auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_server_uid));

    checkResponse(r, "nao conseguiu pegar a auth key do game server do player: " + std::to_string(m_uid));

	return r; 
};

std::string& CmdAuthKeyGame::getAuthKey() {
    return m_auth_key_game;
};

void CmdAuthKeyGame::setAuthKey(std::string _auth_key) {
    m_auth_key_game = _auth_key;
};

uint32_t CmdAuthKeyGame::getUID() {
    return m_uid;
};

void CmdAuthKeyGame::setUID(uint32_t _uid) {
    m_uid = _uid;
};

uint32_t CmdAuthKeyGame::getServerUID() {
    return m_server_uid;
}

void CmdAuthKeyGame::setServerUID(uint32_t _server_uid) {
    m_server_uid = _server_uid;
}
