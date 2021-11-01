// Arquivo cmd_add_item.cpp
// Criado em 31/05/2018 as 10:22 por Acrisio
// Implementa��o da classe CmdAddItem

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_add_item.hpp"

using namespace stdA;

CmdAddItem::CmdAddItem(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_purchase(0), m_gift_flag(0), m_wi{0} {
}

CmdAddItem::CmdAddItem(uint32_t _uid, WarehouseItemEx& _wi, unsigned char _purchase, unsigned char _gift_flag, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_purchase(_purchase), m_gift_flag(_gift_flag), m_wi(_wi) {
}

CmdAddItem::~CmdAddItem() {
}

void CmdAddItem::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(1, (uint32_t)_result->cols);

	m_wi.id = IFNULL(atoi, _result->data[0]);
}

response* CmdAddItem::prepareConsulta(database& _db) {

	if (m_wi._typeid == 0)
		throw exception("[CmdAddItem::prepareConsulta][Error] Item invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string((unsigned short)m_gift_flag) + ", " + std::to_string((unsigned short)m_purchase)
			+ ", " + std::to_string(m_wi.id) + ", " + std::to_string(m_wi._typeid) + ", " + std::to_string((unsigned short)m_wi.flag)
			+ ", " + std::to_string(m_wi.STDA_C_ITEM_TIME) + ", " + std::to_string(m_wi.c[0]) + ", " + std::to_string(m_wi.c[1])
			+ ", " + std::to_string(m_wi.c[2]) + ", " + std::to_string(m_wi.c[3]) + ", " + std::to_string(m_wi.c[4])
	);

	checkResponse(r, "nao conseguiu adicionar o Item[TYPEID=" + std::to_string(m_wi._typeid) + "] para o player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdAddItem::getUID() {
	return m_uid;
}

void CmdAddItem::setUID(uint32_t _uid) {
	m_uid = _uid;
}

unsigned char CmdAddItem::getPurchase() {
	return m_purchase;
}

void CmdAddItem::setPurchase(unsigned char _purchase) {
	m_purchase = _purchase;
}

unsigned char CmdAddItem::getGiftFlag() {
	return m_gift_flag;
}

void CmdAddItem::setGiftFlag(unsigned char _gift_flag) {
	m_gift_flag = _gift_flag;
}

WarehouseItemEx& CmdAddItem::getInfo() {
	return m_wi;
}

void CmdAddItem::setInfo(WarehouseItemEx& _wi) {
	m_wi = _wi;
}
