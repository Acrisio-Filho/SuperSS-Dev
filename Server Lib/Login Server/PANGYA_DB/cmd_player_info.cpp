// Arquivo cmd_player_info.cpp
// Criado em 17/03/2018 as 18:39 por Acrisio
// Implementação da classe CmdPlayerInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_player_info.hpp"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"

using namespace stdA;

CmdPlayerInfo::CmdPlayerInfo(bool _waiter) : pangya_db(_waiter), m_uid(0), m_pi{} {
};

CmdPlayerInfo::CmdPlayerInfo(uint32_t _uid, bool _waiter) : pangya_db(_waiter), m_uid(_uid), m_pi{} {
};

CmdPlayerInfo::~CmdPlayerInfo() {
};

void CmdPlayerInfo::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {
    
    checkColumnNumber(8, (uint32_t)_result->cols);

    // Aqui faz as coisas
    m_pi.uid = IFNULL(atoi, _result->data[0]);
    if (_result->data[1] != nullptr)
#if defined(_WIN32)
        memcpy_s(m_pi.id, sizeof(m_pi.id), _result->data[1], sizeof(m_pi.id));
#elif defined(__linux__)
        memcpy(m_pi.id, _result->data[1], sizeof(m_pi.id));
#endif
    if (_result->data[2] != nullptr)
#if defined(_WIN32)
        memcpy_s(m_pi.nickname, sizeof(m_pi.nickname), _result->data[2], sizeof(m_pi.nickname));
#elif defined(__linux__)
        memcpy(m_pi.nickname, _result->data[2], sizeof(m_pi.nickname));
#endif
    if (_result->data[3] != nullptr)
#if defined(_WIN32)
        memcpy_s(m_pi.pass, sizeof(m_pi.pass), _result->data[3], sizeof(m_pi.pass));
#elif defined(__linux__)
        memcpy(m_pi.pass, _result->data[3], sizeof(m_pi.pass));
#endif
    m_pi.m_cap = IFNULL(atoi, _result->data[4]);
    m_pi.level = (unsigned short)IFNULL(atoi, _result->data[5]);
	m_pi.block_flag.setIDState((uint64_t)IFNULL(atoll, _result->data[6]));
	m_pi.block_flag.m_id_state.block_time = IFNULL(atoi, _result->data[7]);
    // Fim

    if (m_pi.uid != m_uid)
        throw exception("[CmdPlayerInfo::lineResult][Error] UID do player info nao e igual ao requisitado. UID Req: " + std::to_string(m_uid) + " != " + std::to_string(m_pi.uid), STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 3, 0));
};

response* CmdPlayerInfo::prepareConsulta(database& _db) {

    m_pi.clear();
    
    auto r = procedure(_db, m_szConsulta, std::to_string(m_uid));

    checkResponse(r, "nao conseguiu pegar o info do player: " + std::to_string(m_uid));

    return r;
};

uint32_t CmdPlayerInfo::getUID() {
	return m_uid;
};

void CmdPlayerInfo::setUID(uint32_t _uid) {
	m_uid = _uid;
};

player_info& CmdPlayerInfo::getInfo() {
	return m_pi;
};

void CmdPlayerInfo::updateInfo(player_info& _pi) {
	m_pi = _pi;
};


