// Arquivo cmd_delete_rental.cpp
// Criado em 10/06/2018 as 17:47 por Acrisio
// Implementa��o da classe CmdDeleteRental

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_delete_rental.hpp"

using namespace stdA;

CmdDeleteRental::CmdDeleteRental(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_item_id(0l) {
}

CmdDeleteRental::CmdDeleteRental(uint32_t _uid, int32_t _item_id, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_item_id(_item_id) {
}

CmdDeleteRental::~CmdDeleteRental() {
}

void CmdDeleteRental::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
}

response* CmdDeleteRental::prepareConsulta(database& _db) {

	if (m_uid == 0)
		throw exception("[CmdDeleteRental::prepareConsulta][Error] m_uid is invalied(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_item_id <= 0)
		throw exception("[CmdDeleteRental::prepareConsulta][Error] item_id[value=" + std::to_string(m_item_id) + "] is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = _update(_db, m_szConsulta[0] + std::to_string(m_uid) + m_szConsulta[1] + std::to_string(m_item_id));

	checkResponse(r, "nao conseguiu deletar o Rental Item[ID=" + std::to_string(m_item_id) + "] do player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdDeleteRental::getUID() {
	return m_uid;
}

void CmdDeleteRental::setUID(uint32_t _uid) {
	m_uid = _uid;
}

int32_t CmdDeleteRental::getItemID() {
	return m_item_id;
}

void CmdDeleteRental::setItemID(int32_t _item_id) {
	m_item_id = _item_id;
}
