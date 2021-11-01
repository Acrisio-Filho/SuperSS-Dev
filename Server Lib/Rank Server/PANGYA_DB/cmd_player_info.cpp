// Arquivo cmd_player_info.cpp
// Criado em 15/06/2020 as 16:41 por Acrisio
// Implementa��o da classe CmdPlayerInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_player_info.hpp"

using namespace stdA;

CmdPlayerInfo::CmdPlayerInfo(bool _waiter) : pangya_db(_waiter), m_uid(0u) {
}

CmdPlayerInfo::CmdPlayerInfo(uint32_t _uid, bool _waiter) : pangya_db(_waiter), m_uid(_uid) {
}

CmdPlayerInfo::~CmdPlayerInfo() {
}

void CmdPlayerInfo::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(8, (uint32_t)_result->cols);

	m_pi.uid = IFNULL(atoi, _result->data[0]);

	size_t len = 0u;
	
	if (_result->data[1] != nullptr) {
		
		len = strlen(_result->data[1]);

#if defined(_WIN32)
		memcpy_s(m_pi.id, sizeof(m_pi.id), _result->data[1], (len > sizeof(m_pi.id) ? sizeof(m_pi.id) : len));
#elif defined(__linux__)
		memcpy(m_pi.id, _result->data[1], (len > sizeof(m_pi.id) ? sizeof(m_pi.id) : len));
#endif
	}

	if (_result->data[2] != nullptr) {

		len = strlen(_result->data[2]);

#if defined(_WIN32)
		memcpy_s(m_pi.nickname, sizeof(m_pi.nickname), _result->data[2], (len > sizeof(m_pi.nickname) ? sizeof(m_pi.nickname) : len));
#elif defined(__linux__)
		memcpy(m_pi.nickname, _result->data[2], (len > sizeof(m_pi.nickname) ? sizeof(m_pi.nickname) : len));
#endif
	}
	
	m_pi.m_cap = IFNULL(atoi, _result->data[3]);
	m_pi.server_uid = IFNULL(atoi, _result->data[4]);
	m_pi.level = (unsigned short)IFNULL(atoi, _result->data[5]);
	m_pi.block_flag.setIDState((uint64_t)IFNULL(atoll, _result->data[6]));
	m_pi.block_flag.m_id_state.block_time = IFNULL(atoi, _result->data[7]);

	if (m_pi.uid != m_uid)
		throw exception("[CmdPlayerInfo::lineResult][Error] Player UID_REQUEST=" + std::to_string(m_uid) 
				+ " not match from UID_RETURNED=" + std::to_string(m_pi.uid), STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 3, 0));
}

response* CmdPlayerInfo::prepareConsulta(database& _db) {
	
	if (m_uid == 0u)
		throw exception("[CmdPlayerInfo::prepareConsulta][Error] m_uid(" + std::to_string(m_uid) + ") is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	m_pi.clear();

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid));

	checkResponse(r, "Nao conseguiu pegar o info do player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdPlayerInfo::getUID() {
	return m_uid;
}

void CmdPlayerInfo::setUID(uint32_t _uid) {
	m_uid = _uid;
}

player_info& CmdPlayerInfo::getInfo() {
	return m_pi;
}
