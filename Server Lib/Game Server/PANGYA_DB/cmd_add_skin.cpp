// Arquivo cmd_add_skin.cpp
// Criado em 20/05/2018 as 10:56 por Acrisio
// Implementa��o da classe CmdAddSkin

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_add_skin.hpp"

using namespace stdA;

CmdAddSkin::CmdAddSkin(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_gift_flag(0), m_purchase(0), m_wi{0} {
}

CmdAddSkin::CmdAddSkin(uint32_t _uid, WarehouseItemEx& _wi, unsigned char _purchase, unsigned char _gift_flag, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_gift_flag(_gift_flag), m_purchase(_purchase), m_wi(_wi) {
}

CmdAddSkin::~CmdAddSkin() {
}

void CmdAddSkin::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(1, (uint32_t)_result->cols);

	m_wi.id = IFNULL(atoi, _result->data[0]);
}

response* CmdAddSkin::prepareConsulta(database& _db) {

	if (m_wi._typeid == 0)
		throw exception("[CmdAddSkin::prepareConsulta][Error] Skin is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_wi._typeid)
				+ ", " + std::to_string((unsigned short)m_wi.flag) + ", " + std::to_string((unsigned short)m_gift_flag)
				+ ", " + std::to_string((unsigned short)m_purchase) + ", " + std::to_string(m_wi.STDA_C_ITEM_TIME)
	);

	checkResponse(r, "nao conseguiu adicionar Skin[TYPEID=" + std::to_string(m_wi._typeid) + "] para o player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdAddSkin::getUID() {
	return m_uid;
}

void CmdAddSkin::setUID(uint32_t _uid) {
	m_uid = _uid;
}

WarehouseItemEx& CmdAddSkin::getInfo() {
	return m_wi;
}

void CmdAddSkin::setInfo(WarehouseItemEx& _wi) {
	m_wi = _wi;
}

unsigned char CmdAddSkin::getGiftFlag() {
	return m_gift_flag;
}

void CmdAddSkin::setGiftFlag(unsigned char _gift_flag) {
	m_gift_flag = _gift_flag;
}

unsigned char CmdAddSkin::getPurchase() {
	return m_purchase;
}

void CmdAddSkin::setPurchase(unsigned char _purchase) {
	m_purchase = _purchase;
}
