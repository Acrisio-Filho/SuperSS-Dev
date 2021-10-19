// Arquivo cmd_update_mascot_equiped.cpp
// Criado em 25/03/2018 as 11:46 por Acrisio
// Implementa��o da classe CmdUpdateMascotEquiped

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_mascot_equiped.hpp"

using namespace stdA;

CmdUpdateMascotEquiped::CmdUpdateMascotEquiped(bool _waiter) : pangya_db(_waiter), m_uid(0), m_mascot_id(-1) {
}

CmdUpdateMascotEquiped::CmdUpdateMascotEquiped(uint32_t _uid, int32_t _mascot_id, bool _waiter)
		: pangya_db(_waiter), m_uid(_uid), m_mascot_id(_mascot_id) {
}

CmdUpdateMascotEquiped::~CmdUpdateMascotEquiped() {
}

void CmdUpdateMascotEquiped::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdUpdateMascotEquiped::prepareConsulta(database& _db) {

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_mascot_id));

	checkResponse(r, "nao conseguiu atualizar o mascot[ID=" + std::to_string(m_mascot_id) + "] equipado do player: " + std::to_string(m_uid));

	return r;
}

uint32_t CmdUpdateMascotEquiped::getUID() {
	return m_uid;
}

void CmdUpdateMascotEquiped::setUID(uint32_t _uid) {
	m_uid = _uid;
}

int32_t CmdUpdateMascotEquiped::getMascotID() {
	return m_mascot_id;
}

void CmdUpdateMascotEquiped::setMascotID(int32_t _mascot_id) {
	m_mascot_id = _mascot_id;
}
