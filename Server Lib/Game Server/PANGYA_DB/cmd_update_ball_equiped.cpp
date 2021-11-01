// Arquivo cmd_update_ball_equiped.cpp
// Criado em 25/03/2018 as 10:59 por Acrisio
// Implementa��o da classe CmdUpdateBallEquiped

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_ball_equiped.hpp"

using namespace stdA;

CmdUpdateBallEquiped::CmdUpdateBallEquiped(bool _waiter) : pangya_db(_waiter), m_uid(0), m_ball_typeid(0) {
}

CmdUpdateBallEquiped::CmdUpdateBallEquiped(uint32_t _uid, uint32_t _ball_typeid, bool _waiter)
		: pangya_db(_waiter), m_uid(_uid), m_ball_typeid(_ball_typeid) {
}

CmdUpdateBallEquiped::~CmdUpdateBallEquiped() {
}

void CmdUpdateBallEquiped::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um update
	return;
}

response* CmdUpdateBallEquiped::prepareConsulta(database& _db) {

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_ball_typeid));

	checkResponse(r, "nao conseguiu atualizar a bola[TYPEID=" + std::to_string(m_ball_typeid) + "] equipada do player: " + std::to_string(m_uid));

	return r;
}

uint32_t CmdUpdateBallEquiped::getUID() {
	return m_uid;
}

void CmdUpdateBallEquiped::setUID(uint32_t _uid) {
	m_uid = _uid;
}

uint32_t CmdUpdateBallEquiped::getBallTypeid() {
	return m_ball_typeid;
}

void CmdUpdateBallEquiped::setBallTypeid(uint32_t _ball_typeid) {
	m_ball_typeid = _ball_typeid;
}
