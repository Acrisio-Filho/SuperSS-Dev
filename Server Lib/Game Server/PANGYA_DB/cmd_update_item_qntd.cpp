// Arquivo cmd_update_item_qntd.cpp
// Criado em 31/05/2018 as 10:47 por Acrisio
// Implementa��o da classe CmdUpdateItemQntd

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_item_qntd.hpp"

using namespace stdA;

CmdUpdateItemQntd::CmdUpdateItemQntd(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_id(-1), m_qntd(0u) {
}

CmdUpdateItemQntd::CmdUpdateItemQntd(uint32_t _uid, int32_t _id, uint32_t _qntd, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_id(_id), m_qntd(_qntd) {
}

CmdUpdateItemQntd::~CmdUpdateItemQntd() {
}

void CmdUpdateItemQntd::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
}

response* CmdUpdateItemQntd::prepareConsulta(database& _db) {

	if (m_id <= 0)
		throw exception("[CmdUpdateItemQntd::prepareConsulta][Error] Item id[value=" + std::to_string(m_id) + "] is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = _update(_db, m_szConsulta[0] + std::to_string(m_qntd) + m_szConsulta[1] + std::to_string(m_uid) + m_szConsulta[2] + std::to_string(m_id));

	checkResponse(r, "nao consiguiu atualizar quantidade[value=" + std::to_string(m_qntd) + "] do Item[ID=" + std::to_string(m_id) + "] do player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdUpdateItemQntd::getUID() {
	return m_uid;
}

void CmdUpdateItemQntd::setUID(uint32_t _uid) {
	m_uid = _uid;
}

int32_t CmdUpdateItemQntd::getID() {
	return m_id;
}

void CmdUpdateItemQntd::setID(int32_t _id) {
	m_id = _id;
}

uint32_t CmdUpdateItemQntd::getQntd() {
	return m_qntd;
}

void CmdUpdateItemQntd::setQntd(uint32_t _qntd) {
	m_qntd = _qntd;
}
