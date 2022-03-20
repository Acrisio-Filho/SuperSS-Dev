// Arquivo cmd_player_info.cpp
// Criado em 18/03/2018 as 11:53 por Acrisio
// Implementação da classe CmdPlayerInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_player_info.hpp"

using namespace stdA;

CmdPlayerInfo::CmdPlayerInfo(bool _waiter) : pangya_db(_waiter), m_uid(0), m_pi{} {
};

CmdPlayerInfo::CmdPlayerInfo(uint32_t _uid, bool _waiter) : pangya_db(_waiter), m_uid(_uid), m_pi{} {
};

CmdPlayerInfo::~CmdPlayerInfo() {
};

void CmdPlayerInfo::lineResult(result_set::ctx_res* _result, uint32_t /*_index_resultt*/) {

    checkColumnNumber(8, (uint32_t)_result->cols);

    m_pi.uid = IFNULL(atoi, _result->data[0]);
	if (is_valid_c_string(_result->data[1]))
		STRCPY_TO_MEMORY_FIXED_SIZE(m_pi.id, sizeof(m_pi.id), _result->data[1]);
	if (is_valid_c_string(_result->data[2]))
		STRCPY_TO_MEMORY_FIXED_SIZE(m_pi.nickname, sizeof(m_pi.nickname), _result->data[2]);
	if (is_valid_c_string(_result->data[3]))
		STRCPY_TO_MEMORY_FIXED_SIZE(m_pi.pass, sizeof(m_pi.pass), _result->data[3]);
	//pi.cap = IFNULL(atoi, _result->data[4]);
	m_pi.level = (unsigned short)IFNULL(atoi, _result->data[5]);
	m_pi.block_flag.setIDState((uint64_t)IFNULL(atoll, _result->data[6]));
	m_pi.block_flag.m_id_state.block_time = IFNULL(atoi, _result->data[7]);

    if (m_pi.uid != m_uid)
        throw exception("[CmdPlayerInfo::lineResult][Error] UID do player info nao e igual ao requisitado. UID Req: " + std::to_string(m_uid) + " != " + std::to_string(m_pi.uid), STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 3, 0));
};

response* CmdPlayerInfo::prepareConsulta(database& _db) {
    
    m_pi.clear();

	// UID caso ele não consigar achar no DB
	m_pi.uid = (uint32_t)~0u;

    auto r = procedure(_db, m_szConsulta, std::to_string(m_uid));

    checkResponse(r, "nao conseguiu pegar o info do player: " + std::to_string(m_uid));

	return r;
};

player_info& CmdPlayerInfo::getInfo() {
    return m_pi;
};

void CmdPlayerInfo::setInfo(player_info& _pi) {
    m_pi = _pi;
};

uint32_t CmdPlayerInfo::getUID() {
    return m_uid;
};

void CmdPlayerInfo::setUID(uint32_t _uid) {
    m_uid = _uid;
};
