// Arquivo cmd_update_dolfini_locker_pang.cpp
// Criado em 02/06/2018 as 21:45 por Acrisio
// Implementa��o da classe CmdUpdateDolfiniLockerPang

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_dolfini_locker_pang.hpp"

using namespace stdA;

CmdUpdateDolfiniLockerPang::CmdUpdateDolfiniLockerPang(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_pang(0ull) {
}

CmdUpdateDolfiniLockerPang::CmdUpdateDolfiniLockerPang(uint32_t _uid, uint64_t _pang, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_pang(_pang) {
}

CmdUpdateDolfiniLockerPang::~CmdUpdateDolfiniLockerPang() {
}

void CmdUpdateDolfiniLockerPang::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
}

response* CmdUpdateDolfiniLockerPang::prepareConsulta(database& _db) {

	auto r = _update(_db, m_szConsulta[0] + std::to_string(m_pang) + m_szConsulta[1] + std::to_string(m_uid));

	checkResponse(r, "nao conseguiu atualizar o pang[value=" + std::to_string(m_pang) + "] do Dolfini Locker do player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdUpdateDolfiniLockerPang::getUID() {
	return m_uid;
}

void CmdUpdateDolfiniLockerPang::setUID(uint32_t _uid) {
	m_uid = _uid;
}

uint64_t CmdUpdateDolfiniLockerPang::getPang() {
	return m_pang;
}

void CmdUpdateDolfiniLockerPang::setPang(uint64_t _pang) {
	m_pang = _pang;
}
