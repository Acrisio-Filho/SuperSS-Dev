// Arquivo cmd_register_server.cpp
// Criado em 25/03/2018 as 21:27 por Acrisio
// Implementação da classe CmdRegisterServer

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_register_server.hpp"

using namespace stdA;

CmdRegisterServer::CmdRegisterServer(bool _waiter) : pangya_db(_waiter), m_si{} {
}

CmdRegisterServer::CmdRegisterServer(ServerInfoEx& _si, bool _waiter) : pangya_db(_waiter), m_si(_si) {
}

CmdRegisterServer::~CmdRegisterServer() {
}

void CmdRegisterServer::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

    // Não usa aqui por que é um UPDATE
    return;
}

response* CmdRegisterServer::prepareConsulta(database& _db) {

    auto r = procedure(_db, m_szConsulta, std::to_string(m_si.uid) + ", " + _db.makeText(m_si.nome) + ", " + _db.makeText(m_si.ip)
                + ", " + std::to_string(m_si.port) + ", " + std::to_string(m_si.tipo) + ", " + std::to_string(m_si.max_user)
                + ", " + std::to_string(m_si.curr_user) + ", " + std::to_string(m_si.rate.pang) + ", " + _db.makeText(m_si.version)
                + ", " + _db.makeText(m_si.version_client) + ", " + std::to_string(m_si.propriedade.ulProperty) + ", " + std::to_string(m_si.angelic_wings_num)
                + ", " + std::to_string(m_si.event_flag.usEventFlag) + ", " + std::to_string(m_si.rate.exp) + ", " + std::to_string(m_si.img_no)
                + ", " + std::to_string(m_si.rate.scratchy) + ", " + std::to_string(m_si.rate.club_mastery) + ", " + std::to_string(m_si.rate.treasure)
                + ", " + std::to_string(m_si.rate.papel_shop_rare_item) + ", " + std::to_string(m_si.rate.papel_shop_cookie_item) + ", " + std::to_string(m_si.rate.chuva)
    );

    checkResponse(r, "nao conseguiu registrar o server[GUID=" + std::to_string(m_si.uid) + ", PORT=" + std::to_string(m_si.port) + ", NOME=" + std::string(m_si.nome) + "] no banco de dados");

    return r;
}

ServerInfoEx& CmdRegisterServer::getInfo() {
    return m_si;
}

void CmdRegisterServer::setInfo(ServerInfoEx& _si) {
    m_si = _si;
}
