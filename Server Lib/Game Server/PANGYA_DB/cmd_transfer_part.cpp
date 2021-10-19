// Arquivo cmd_transfer_part.cpp
// Criado em 10/06/2018 as 11:44 por Acrisio
// Implementa��o da classe CmdTransferPart

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_transfer_part.hpp"

using namespace stdA;

CmdTransferPart::CmdTransferPart(bool _waiter) : pangya_db(_waiter), m_uid_sell(0u), m_uid_buy(0u), m_item_id(0l), m_type_iff(0u) {
}

CmdTransferPart::CmdTransferPart(uint32_t _uid_sell, uint32_t _uid_buy, int32_t _item_id, unsigned char _type_iff, bool _waiter)
	: pangya_db(_waiter), m_uid_sell(_uid_sell), m_uid_buy(_uid_buy), m_item_id(_item_id), m_type_iff(_type_iff) {
}

CmdTransferPart::~CmdTransferPart() {
}

void CmdTransferPart::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa aqui por que � um UPDATE
	return;
}

response* CmdTransferPart::prepareConsulta(database& _db) {

	if (m_uid_sell == 0 || m_uid_buy == 0)
		throw exception("[CmdTransferPart::prepareConsulta][Error] player_s[UID=" + std::to_string(m_uid_sell) + "] or player_r[UID=" + std::to_string(m_uid_buy) + "] is invalid", 
				STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));
	
	if (m_item_id <= 0)
		throw exception("[CmdTransferPart::prepareConsulta][Error] item[ID=" + std::to_string(m_item_id) + "] is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid_sell) + ", " + std::to_string(m_uid_buy) + ", " + std::to_string(m_item_id) + ", " + std::to_string(m_type_iff));

	checkResponse(r, "nao conseguiu transferir o item[ID=" + std::to_string(m_item_id) + "] do player[UID=" + std::to_string(m_uid_sell) + "] para o player[UID=" + std::to_string(m_uid_buy) + "]");

	return r;
}

uint32_t CmdTransferPart::getUIDSell() {
	return m_uid_sell;
}

void CmdTransferPart::setUIDSell(uint32_t _uid_sell) {
	m_uid_sell = _uid_sell;
}

uint32_t CmdTransferPart::getUIDBuy() {
	return m_uid_buy;
}

void CmdTransferPart::setUIDBuy(uint32_t _uid_buy) {
	m_uid_buy = _uid_buy;
}

int32_t CmdTransferPart::getItemID() {
	return m_item_id;
}

void CmdTransferPart::setItemID(int32_t _item_id) {
	m_item_id = _item_id;
}

unsigned char CmdTransferPart::getTypeIFF() {
	return m_type_iff;
}

void CmdTransferPart::setTypeIFF(unsigned char _type_iff) {
	m_type_iff = _type_iff;
}
