// Arquivo cmd_update_dolfini_locker_mode.cpp
// Criado em 02/06/2018 as 18:09 por Acrisio
// Implementa��o da classe CmdUpdateDolfiniLockerMode

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_dolfini_locker_mode.hpp"

using namespace stdA;

CmdUpdateDolfiniLockerMode::CmdUpdateDolfiniLockerMode(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_locker(0) {
}

CmdUpdateDolfiniLockerMode::CmdUpdateDolfiniLockerMode(uint32_t _uid, unsigned char _locker, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_locker(_locker) {
}

CmdUpdateDolfiniLockerMode::~CmdUpdateDolfiniLockerMode() {
}

void CmdUpdateDolfiniLockerMode::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdUpdateDolfiniLockerMode::prepareConsulta(database& _db) {

	auto r = _update(_db, m_szConsulta[0] + std::to_string((unsigned short)m_locker) + m_szConsulta[1] + std::to_string(m_uid));

	checkResponse(r, "nao conseguiu atualizar o modo[locker=" + std::to_string(m_locker) + "] do dolfini locker do player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdUpdateDolfiniLockerMode::getUID() {
	return m_uid;
}

void CmdUpdateDolfiniLockerMode::setUID(uint32_t _uid) {
	m_uid = _uid;
}

unsigned char CmdUpdateDolfiniLockerMode::getLocker() {
	return m_locker;
}

void CmdUpdateDolfiniLockerMode::setLocker(unsigned char _locker) {
	m_locker = _locker;
}
