// Arquivo cmd_update_clubset_equiped.cpp
// Criado em 25/03/2018 as 11:14 por Acrisio
// Implementa��o da classe CmdUpdateClubsetEquiped

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_clubset_equiped.hpp"

using namespace stdA;

CmdUpdateClubsetEquiped::CmdUpdateClubsetEquiped(bool _waiter) : pangya_db(_waiter), m_uid(0), m_clubset_id(0) {
}

CmdUpdateClubsetEquiped::CmdUpdateClubsetEquiped(uint32_t _uid, int32_t _clubset_id, bool _waiter)
		: pangya_db(_waiter), m_uid(_uid), m_clubset_id(_clubset_id) {
}

CmdUpdateClubsetEquiped::~CmdUpdateClubsetEquiped() {
}

void CmdUpdateClubsetEquiped::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdUpdateClubsetEquiped::prepareConsulta(database& _db) {

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_clubset_id));

	checkResponse(r, "nao conseguiu atualizar o clubset[ID=" + std::to_string(m_clubset_id) + "] equipado do player: " + std::to_string(m_uid));

	return r;
}

uint32_t CmdUpdateClubsetEquiped::getUID() {
	return m_uid;
}

void CmdUpdateClubsetEquiped::setUID(uint32_t _uid) {
	m_uid = _uid;
}

int32_t CmdUpdateClubsetEquiped::getClubsetID() {
	return m_clubset_id;
}

void CmdUpdateClubsetEquiped::setClubsetID(int32_t _clubset_id) {
	m_clubset_id = _clubset_id;
}
