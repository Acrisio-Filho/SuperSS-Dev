// Arquivo cmd_delete_ball.cpp
// Criado em 31/05/2018 as 21:13 por Acrisio
// Implementa��o da classe CmdDeleteBall

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_delete_ball.hpp"

using namespace stdA;

CmdDeleteBall::CmdDeleteBall(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_id(-1) {
}

CmdDeleteBall::CmdDeleteBall(uint32_t _uid, int32_t _id, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_id(_id) {
}

CmdDeleteBall::~CmdDeleteBall() {
}

void CmdDeleteBall::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdDeleteBall::prepareConsulta(database& _db) {

	if (m_id <= 0)
		throw exception("[CmdDeleteBall::prepareConsulta][Error] Ball id[value=" + std::to_string(m_id) + "] is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = _update(_db, m_szConsulta[0] + std::to_string(m_uid) + m_szConsulta[1] + std::to_string(m_id));

	checkResponse(r, "nao conseguiu deletar Ball[ID=" + std::to_string(m_id) + "] do player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdDeleteBall::getUID() {
	return m_uid;
}

void CmdDeleteBall::setUID(uint32_t _uid) {
	m_uid = _uid;
}

int32_t CmdDeleteBall::getID() {
	return m_id;
}

void CmdDeleteBall::setID(int32_t _id) {
	m_id = _id;
}
