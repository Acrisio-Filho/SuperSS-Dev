// Arquivo cmd_my_room_config.cpp
// Criado em 22/03/2018 as 20:25 por Acrisio
// Implementa��o da classe CmdMyRoomConfig

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_my_room_config.hpp"

using namespace stdA;

CmdMyRoomConfig::CmdMyRoomConfig(bool _waiter) : pangya_db(_waiter), m_uid(0), m_mrc{0} {
}

CmdMyRoomConfig::CmdMyRoomConfig(uint32_t _uid, bool _waiter) : pangya_db(_waiter), m_uid(_uid), m_mrc{0} {
}

CmdMyRoomConfig::~CmdMyRoomConfig() {
}

void CmdMyRoomConfig::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(3, (uint32_t)_result->cols);

	if (is_valid_c_string(_result->data[0]))
		STRCPY_TO_MEMORY_FIXED_SIZE(m_mrc.pass, sizeof(m_mrc.pass), _result->data[0]);
	m_mrc.public_lock = (unsigned char)IFNULL(atoi, _result->data[1]);
	m_mrc.allow_enter = (unsigned short)IFNULL(atoi, _result->data[2]);
}

response* CmdMyRoomConfig::prepareConsulta(database& _db) {

	m_mrc.clear();

	auto r = consulta(_db, m_szConsulta + std::to_string(m_uid));

	checkResponse(r, "nao conseguiu pegar o my room config do player: " + std::to_string(m_uid));

	return r;
}

MyRoomConfig& CmdMyRoomConfig::getMyRoomConfig() {
	return m_mrc;
}

void CmdMyRoomConfig::setMyRoomConfig(MyRoomConfig& _mrc) {
	m_mrc = _mrc;
}

uint32_t CmdMyRoomConfig::getUID() {
	return m_uid;
}

void CmdMyRoomConfig::setUID(uint32_t _uid) {
	m_uid = _uid;
}
