// Arquivo cmd_extend_rental.cpp
// Criado em 10/06/2018 as 16:57 por Acrisio
// Implementa��o da classe CmdExtendRental

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_extend_rental.hpp"

using namespace stdA;

CmdExtendRental::CmdExtendRental(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_item_id(0l), m_date() {
}

CmdExtendRental::CmdExtendRental(uint32_t _uid, int32_t _item_id, std::string& _date, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_item_id(_item_id), m_date(_date) {
}

CmdExtendRental::~CmdExtendRental() {
}

void CmdExtendRental::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdExtendRental::prepareConsulta(database& _db) {

	if (m_uid == 0)
		throw exception("[CmdExtendRental::prepareConsulta][Error] m_uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_item_id <= 0)
		throw exception("[CmdExtendRental::prepareConsulta][Error] m_item_id[value=" + std::to_string(m_item_id) + "] is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_date.empty())
		throw exception("[CmdExtendRental::prepareConsulta][Error] m_date is empty", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_item_id) + ", " + _db.makeText(m_date));

	checkResponse(r, "nao conseguiu extender o Part Rental[ID=" + std::to_string(m_item_id) + "] do player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdExtendRental::getUID() {
	return m_uid;
}

void CmdExtendRental::setUID(uint32_t _uid) {
	m_uid = _uid;
}

int32_t CmdExtendRental::getItemID() {
	return m_item_id;
}

void CmdExtendRental::setItemID(int32_t _item_id) {
	m_item_id = _item_id;
}

std::string& CmdExtendRental::getDate() {
	return m_date;
}

void CmdExtendRental::setDate(std::string& _date) {
	m_date = _date;
}
