// Arquivo cmd_update_ball_qntd.cpp
// Criado em 31/05/2018 as 09:30 por Acrisio
// Implementa��o da classe CmdUpdateBallQntd

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_ball_qntd.hpp"

using namespace stdA;

CmdUpdateBallQntd::CmdUpdateBallQntd(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_id(-1), m_qntd(0u) {
}

CmdUpdateBallQntd::CmdUpdateBallQntd(uint32_t _uid, int32_t _id, uint32_t _qntd, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_id(_id), m_qntd(_qntd) {
}

CmdUpdateBallQntd::~CmdUpdateBallQntd() {
}

void CmdUpdateBallQntd::lineResult(result_set::ctx_res* /*result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
}

response* CmdUpdateBallQntd::prepareConsulta(database& _db) {

	if (m_id <= 0)
		throw exception("[CmdUpdateBallQntd][Error] Ball id[value=" + std::to_string(m_id) + "] is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = _update(_db, m_szConsulta[0] + std::to_string(m_qntd) + m_szConsulta[1] + std::to_string(m_uid) + m_szConsulta[2] + std::to_string(m_id));

	checkResponse(r, "nao conseguiu atualizar quantidade[value=" + std::to_string(m_qntd) + "] da Ball[ID=" + std::to_string(m_id) + "] do player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdUpdateBallQntd::getUID() {
	return m_uid;
}

void CmdUpdateBallQntd::setUID(uint32_t _uid) {
	m_uid = _uid;
}

int32_t CmdUpdateBallQntd::getID() {
	return m_id;
}

void CmdUpdateBallQntd::setID(int32_t _id) {
	m_id = _id;
}

uint32_t CmdUpdateBallQntd::getQntd() {
	return m_qntd;
}

void CmdUpdateBallQntd::setQntd(uint32_t _qntd) {
	m_qntd = _qntd;
}
