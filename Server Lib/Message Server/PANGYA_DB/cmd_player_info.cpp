// Arquivo cmd_player_info.cpp
// Criado em 29/07/2018 as 22:12 por Acrisio
// Implementa��o da classe CmdPlayerInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_player_info.hpp"

using namespace stdA;

CmdPlayerInfo::CmdPlayerInfo(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_pi{0} {
}

CmdPlayerInfo::CmdPlayerInfo(uint32_t _uid, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_pi{0} {
}

CmdPlayerInfo::~CmdPlayerInfo() {

}

void CmdPlayerInfo::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(11, (uint32_t)_result->cols);

	m_pi.uid = (uint32_t)IFNULL(atoi, _result->data[0]);
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
	m_pi.m_cap = (uint32_t)IFNULL(atoi, _result->data[3]);
	m_pi.guild_uid = (uint32_t)IFNULL(atoi, _result->data[4]);
	
	if (_result->data[5] != nullptr)
#if defined(_WIN32)
		memcpy_s(m_pi.guild_name, sizeof(m_pi.guild_name), _result->data[5], strlen(_result->data[5]));
#elif defined(__linux__)
		memcpy(m_pi.guild_name, _result->data[5], strlen(_result->data[5]));
#endif

	m_pi.sex = (unsigned char)IFNULL(atoi, _result->data[6]);
	m_pi.level = (unsigned short)IFNULL(atoi, _result->data[7]);
	m_pi.server_uid = (uint32_t)IFNULL(atoi, _result->data[8]);
	m_pi.block_flag.setIDState((uint64_t)IFNULL(atoll, _result->data[9]));
	m_pi.block_flag.m_id_state.block_time = IFNULL(atoi, _result->data[10]);

	if (m_uid != m_pi.uid)
		throw exception("[CmdPlayerInfo::lineResult][Error] player[UID_resquest=" + std::to_string(m_uid) + ", UID_return=" 
				+ std::to_string(m_pi.uid) + "] retornou um consulta diferente do esperado. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 3, 0));
}

response* CmdPlayerInfo::prepareConsulta(database& _db) {

	if (m_uid == 0)
		throw exception("[CmdPlayerInfo::prepareConsulta][Error] m_uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid));

	checkResponse(r, " nao conseguiu pegar o Info do Player[UID=" + std::to_string(m_uid) + "]");

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
