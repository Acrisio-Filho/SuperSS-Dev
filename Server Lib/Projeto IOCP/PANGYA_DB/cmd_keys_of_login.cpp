// Arquivo cmd_keys_of_login.cpp
// Criado em 18/03/2018 as 12:42 por Acrisio
// Implementação da classe CmdKeysOfLogin

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_keys_of_login.hpp"

using namespace stdA;

CmdKeysOfLogin::CmdKeysOfLogin() : pangya_db(), m_uid(0), m_keys_of_login{0} {
};

CmdKeysOfLogin::CmdKeysOfLogin(uint32_t _uid) : pangya_db(), m_uid(_uid), m_keys_of_login{0} {
};

CmdKeysOfLogin::~CmdKeysOfLogin() {
};

void CmdKeysOfLogin::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

    checkColumnNumber(3, _result->cols);

    if (is_valid_c_string(_result->data[0]))
		strcpy_s(m_keys_of_login.keys[0], _result->data[0]);
	if (is_valid_c_string(_result->data[1]))
		strcpy_s(m_keys_of_login.keys[1], _result->data[1]);
	m_keys_of_login.valid = (unsigned char)IFNULL(atoi, _result->data[2]);

    if (m_keys_of_login.keys[0][0] == '\0' || m_keys_of_login.keys[1][0] == '\0')
        throw exception("[CmdKeysOfLogin::lineResult][Error] a consulta retornou as chaves nula do player: " + std::to_string(m_uid), STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 3, 0));
};

response* CmdKeysOfLogin::prepareConsulta() {

    m_keys_of_login.clear();

    auto r = procedure(m_szConsulta, std::to_string(m_uid));

    checkResponse(r, "nao conseguiu pegar as Keys do Login do player: " + std::to_string(m_uid));

	return r;
};

KeysOfLogin& CmdKeysOfLogin::getKeys() {
    return m_keys_of_login;
};

void CmdKeysOfLogin::setKeys(KeysOfLogin& _keys_of_login) {
    m_keys_of_login = _keys_of_login;
};

uint32_t CmdKeysOfLogin::getUID() {
    return m_uid;
};

void CmdKeysOfLogin::setUID(uint32_t _uid) {
    m_uid = _uid;
};
