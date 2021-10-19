// Arquivo cmd_personal_shop_log.cpp
// Criado em 10/06/2018 as 09:23 por Acrisio
// Implementa��o da classe CmdPersonalShopLog

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_personal_shop_log.hpp"

using namespace stdA;

CmdPersonalShopLog::CmdPersonalShopLog(bool _waiter) : pangya_db(_waiter), m_uid_sell(0u), m_uid_buy(0u), m_item_id_buy(0l), m_psi{0} {
}

CmdPersonalShopLog::CmdPersonalShopLog(uint32_t _sell_uid, uint32_t _buy_uid, PersonalShopItem& _psi, int32_t _item_id_buy, bool _waiter)
	: pangya_db(_waiter), m_uid_sell(_sell_uid), m_uid_buy(_buy_uid), m_item_id_buy(_item_id_buy), m_psi(_psi) {
}

CmdPersonalShopLog::~CmdPersonalShopLog() {
}

void CmdPersonalShopLog::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o vai usar por que � um INSERT
}

response* CmdPersonalShopLog::prepareConsulta(database& _db) {

	if (m_uid_sell == 0 || m_uid_buy == 0)
		throw exception("[CmdPersonalShopLog::prepareConsulta][Error] m_uid_sell[value=" + std::to_string(m_uid_sell) + "] or m_uid_buy[value=" + std::to_string(m_uid_buy) + "] is invalid", 
				STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid_sell) + ", " + std::to_string(m_uid_buy) + ", " + std::to_string(m_psi.item._typeid)
			+ ", " + std::to_string(m_psi.item.id) + ", " + std::to_string(m_item_id_buy) + ", " + std::to_string(m_psi.item.qntd) + ", " 
			+ std::to_string(m_psi.item.pang) + ", " + std::to_string(m_psi.item.qntd * m_psi.item.pang)
	);

	checkResponse(r, "nao conseguiu inserir log so personal shop[UID_SELL=" + std::to_string(m_uid_sell) + ", UID_BUY=" + std::to_string(m_uid_buy) + ", ITEM_TYPEID=" 
			+ std::to_string(m_psi.item._typeid) + ", ITEM_ID_SELL=" + std::to_string(m_psi.item.id) + ", ITEM_ID_BUY=" + std::to_string(m_item_id_buy) + ", ITEM_QNTD=" 
			+ std::to_string(m_psi.item.qntd) + ", ITEM_PANG=" + std::to_string(m_psi.item.pang) + ", TOTAL_PANG=" + std::to_string(m_psi.item.qntd * m_psi.item.pang) + "]");

	return r;
}

uint32_t CmdPersonalShopLog::getUIDSell() {
	return m_uid_sell;
}

void CmdPersonalShopLog::setUIDSell(uint32_t _sell_uid) {
	m_uid_sell = _sell_uid;
}

uint32_t CmdPersonalShopLog::getUIDBuy() {
	return m_uid_buy;
}

void CmdPersonalShopLog::setUIDBuy(uint32_t _buy_uid) {
	m_uid_buy = _buy_uid;
}

int32_t CmdPersonalShopLog::getItemIDBuy() {
	return m_item_id_buy;
}

void CmdPersonalShopLog::setItemIDBuy(int32_t _item_id_buy) {
	m_item_id_buy = _item_id_buy;
}

PersonalShopItem& CmdPersonalShopLog::getItemSell() {
	return m_psi;
}

void CmdPersonalShopLog::setItemSell(PersonalShopItem& _psi) {
	m_psi = _psi;
}
