// Arquivo cmd_verify_pass.cpp
// Criado em 17/03/2018 as 20:53 por Acrisio
// Implementação da classe CmdVerifyPass

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_verify_pass.hpp"

using namespace stdA;

CmdVerifyPass::CmdVerifyPass(bool _waiter) : pangya_db(_waiter), m_pass(""), m_uid(0), m_lastVerify(false) {
};

CmdVerifyPass::CmdVerifyPass(uint32_t _uid, std::string _pass, bool _waiter) : pangya_db(_waiter), m_pass(_pass), m_uid(_uid), m_lastVerify(false) {
};

CmdVerifyPass::~CmdVerifyPass() {
};

void CmdVerifyPass::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {
    
    checkColumnNumber(1, (uint32_t)_result->cols);

    // Aqui faz as coisas
    uint32_t uid_req = IFNULL(atoi, _result->data[0]);
    m_lastVerify = (m_uid == uid_req ? true : false);
    // Fim

    if (!m_lastVerify)
        throw exception("[CmdVerifyPass::lineResult][Error] UID do player info nao e igual ao requisitado. UID Req: " + std::to_string(uid_req) + " != " + std::to_string(m_uid), STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 3, 0));
};

response* CmdVerifyPass::prepareConsulta(database& _db) {

    m_lastVerify = false;

    auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + _db.makeText(m_pass));

    checkResponse(r, "nao conseguiu pegar a uid do player pela senha: " + m_pass);

    return r;
};

std::string& CmdVerifyPass::getPass() {
    return m_pass;
};

void CmdVerifyPass::setPass(std::string _pass) {
    m_pass = _pass;
};

uint32_t CmdVerifyPass::getUID() {
    return m_uid;
};

void CmdVerifyPass::setUID(uint32_t _uid) {
    m_uid = _uid;
};

bool CmdVerifyPass::getLastVerify() {
    return m_lastVerify;
};
