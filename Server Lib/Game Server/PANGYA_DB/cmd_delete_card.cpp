// Arquivo cmd_delete_card.cpp
// Criado em 31/05/2018 as 21:34 por Acrisio
// Implementa��o da classe CmdDeleteCard

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_delete_card.hpp"

using namespace stdA;

CmdDeleteCard::CmdDeleteCard(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_id(-1) {
}

CmdDeleteCard::CmdDeleteCard(uint32_t _uid, int32_t _id, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_id(_id) {
}

CmdDeleteCard::~CmdDeleteCard() {
}

void CmdDeleteCard::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {
	
	// N�o usa por que � um UPDATE
	return;
}

response* CmdDeleteCard::prepareConsulta(database& _db) {

	if (m_id <= 0)
		throw exception("[CmdDeleteCard::prepareConsulta][Error] Card id[value=" + std::to_string(m_id) + "] is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = _update(_db, m_szConsulta[0] + _db.makeText("N") + m_szConsulta[1] + std::to_string(m_uid) + m_szConsulta[2] + std::to_string(m_id));

	checkResponse(r, "nao conseguiu deletar Card[ID=" + std::to_string(m_id) + "] do player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdDeleteCard::getUID() {
	return m_uid;
}

void CmdDeleteCard::setUID(uint32_t _uid) {
	m_uid = _uid;
}

int32_t CmdDeleteCard::getID() {
	return m_id;
}

void CmdDeleteCard::setID(int32_t _id) {
	m_id = _id;
}
