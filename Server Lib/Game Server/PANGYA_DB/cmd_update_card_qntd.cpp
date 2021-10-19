// Arquivo cmd_update_card_qntd.cpp
// Criado em 31/05/2018 as 09:59 por Acrisio
// Implementa��o da classe CmdUpdateCardQntd

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_card_qntd.hpp"

using namespace stdA;

CmdUpdateCardQntd::CmdUpdateCardQntd(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_id(-1), m_qntd(0u) {
}

CmdUpdateCardQntd::CmdUpdateCardQntd(uint32_t _uid, int32_t _id, uint32_t _qntd, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_id(_id), m_qntd(_qntd) {
}

CmdUpdateCardQntd::~CmdUpdateCardQntd() {
}

void CmdUpdateCardQntd::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
}

response* CmdUpdateCardQntd::prepareConsulta(database& _db) {

	if (m_id <= 0)
		throw exception("[CmdUpdateCardQntd::prepareConsulta][Error] card id[value=" + std::to_string(m_id) + "] is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = _update(_db, m_szConsulta[0] + std::to_string(m_qntd) + m_szConsulta[1] + std::to_string(m_uid) + m_szConsulta[2] + std::to_string(m_id));

	checkResponse(r, "nao conseguiu atualizar quantidade[value=" + std::to_string(m_qntd) + "] do card[ID=" + std::to_string(m_id) + "] do player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdUpdateCardQntd::getUID() {
	return m_uid;
}

void CmdUpdateCardQntd::setUID(uint32_t _uid) {
	m_uid = _uid;
}

int32_t CmdUpdateCardQntd::getID() {
	return m_id;
}

void CmdUpdateCardQntd::setID(int32_t _id) {
	m_id = _id;
}

uint32_t CmdUpdateCardQntd::getQntd() {
	return m_qntd;
}

void CmdUpdateCardQntd::setQntd(uint32_t _qntd) {
	m_qntd = _qntd;
}
