// Arquivo cmd_auth_key_login.cpp
// Criado em 18/03/2018 as 10:52 por Acrisio
// Implementação da classe CmdAuthKeyLogin

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_auth_key_login.hpp"

using namespace stdA;

CmdAuthKeyLogin::CmdAuthKeyLogin(bool _waiter) : pangya_db(_waiter), m_uid(0), m_auth_key_login("") {
};

CmdAuthKeyLogin::CmdAuthKeyLogin(uint32_t _uid, bool _waiter) : pangya_db(_waiter), m_uid(_uid), m_auth_key_login("") {
};

CmdAuthKeyLogin::~CmdAuthKeyLogin() {
};

void CmdAuthKeyLogin::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

    checkColumnNumber(1, (uint32_t)_result->cols);

    if (is_valid_c_string(_result->data[0]))
        m_auth_key_login = std::string(_result->data[0]);

    if (m_auth_key_login.empty())
        throw exception("[CmdAuthKey::lineResult][Error] retornou nulo na consulta auth key do player: " + std::to_string(m_uid), STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 3, 0));
};

response* CmdAuthKeyLogin::prepareConsulta(database& _db) {

    m_auth_key_login = "";

    auto r = procedure(_db, m_szConsulta, std::to_string(m_uid));

    checkResponse(r, "nao conseguiu pegar a auth key do login server do player: " + std::to_string(m_uid));

	return r; 
};

std::string& CmdAuthKeyLogin::getAuthKey() {
    return m_auth_key_login;
};

void CmdAuthKeyLogin::setAuthKey(std::string _auth_key) {
    m_auth_key_login = _auth_key;
};

uint32_t CmdAuthKeyLogin::getUID() {
    return m_uid;
};

void CmdAuthKeyLogin::setUID(uint32_t _uid) {
    m_uid = _uid;
};
