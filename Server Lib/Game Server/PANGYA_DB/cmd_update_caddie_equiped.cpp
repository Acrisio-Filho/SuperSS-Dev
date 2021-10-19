// Arquivo cmd_update_caddie_equiped.cpp
// Criado em 25/03/2018 as 10:39 por Acrisio
// Implementa��o da classe CmdUpdateCaddieEquiped

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_caddie_equiped.hpp"

using namespace stdA;

CmdUpdateCaddieEquiped::CmdUpdateCaddieEquiped(bool _waiter) : pangya_db(_waiter), m_uid(0), m_caddie_id(-1) {
}

CmdUpdateCaddieEquiped::CmdUpdateCaddieEquiped(uint32_t _uid, int32_t _caddie_id, bool _waiter)
		: pangya_db(_waiter), m_uid(_uid), m_caddie_id(_caddie_id) {
}

CmdUpdateCaddieEquiped::~CmdUpdateCaddieEquiped() {
}

void CmdUpdateCaddieEquiped::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um update
	return;
}

response* CmdUpdateCaddieEquiped::prepareConsulta(database& _db) {

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_caddie_id));

	checkResponse(r, "nao conseguiu atualizar o caddie[ID=" + std::to_string(m_caddie_id) + "] equipado. do player: " + std::to_string(m_uid));

	return r;
}

uint32_t CmdUpdateCaddieEquiped::getUID() {
	return m_uid;
}

void CmdUpdateCaddieEquiped::setUID(uint32_t _uid) {
	m_uid = _uid;
}

int32_t CmdUpdateCaddieEquiped::getCaddieID() {
	return m_caddie_id;
}

void CmdUpdateCaddieEquiped::setCaddieID(int32_t _caddie_id) {
	m_caddie_id = _caddie_id;
}
