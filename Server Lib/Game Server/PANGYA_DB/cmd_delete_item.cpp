// Arquivo cmd_delete_item.cpp
// Criado em 31/05/2018 as 18:35 por Acrisio
// Implementa��o da classe CmdDeleteItem

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_delete_item.hpp"

using namespace stdA;

CmdDeleteItem::CmdDeleteItem(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_id(-1) {
}

CmdDeleteItem::CmdDeleteItem(uint32_t _uid, int32_t _id, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_id(_id) {
}

CmdDeleteItem::~CmdDeleteItem() {
}

void CmdDeleteItem::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdDeleteItem::prepareConsulta(database& _db) {

	if (m_id <= 0)
		throw exception("[CmdDeleteItem::prepareConsulta][Error] Item id[value=" + std::to_string(m_id) + "] is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = _update(_db, m_szConsulta[0] + std::to_string(m_uid) + m_szConsulta[1] + std::to_string(m_id));

	checkResponse(r, "nao conseguiu deletar o item[ID=" + std::to_string(m_id) + "] do player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdDeleteItem::getUID() {
	return m_uid;
}

void CmdDeleteItem::setUID(uint32_t _uid) {
	m_uid = _uid;
}

int32_t CmdDeleteItem::getID() {
	return m_id;
}

void CmdDeleteItem::setID(int32_t _id) {
	m_id = _id;
}
