// Arquivo cmd_add_clubset.cpp
// Criado em 19/05/2018 as 14:07 por Acrisio
// Implementa��o da classe CmdAddClubSet

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_add_clubset.hpp"

using namespace stdA;

CmdAddClubSet::CmdAddClubSet(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_purchase(0), m_gift_flag(0), m_wi{0} {
}

CmdAddClubSet::CmdAddClubSet(uint32_t _uid, WarehouseItemEx& _wi, unsigned char _purchase, unsigned char _gift_flag, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_purchase(_purchase), m_gift_flag(_gift_flag), m_wi(_wi) {
}

CmdAddClubSet::~CmdAddClubSet() {
}

void CmdAddClubSet::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {


	checkColumnNumber(1, (uint32_t)_result->cols);

	m_wi.id = IFNULL(atoi, _result->data[0]);
}

response* CmdAddClubSet::prepareConsulta(database& _db) {

	if (m_wi._typeid == 0)
		throw exception("[CmdAddClubSet::prepareConsulta][Error] Clubset item is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string((unsigned short)m_gift_flag) + ", " + std::to_string((unsigned short)m_purchase)
					+ ", " + std::to_string(m_wi._typeid) + ", " + std::to_string((unsigned short)m_wi.flag) + ", " + std::to_string(m_wi.STDA_C_ITEM_TIME)
					+ ", " + std::to_string(m_wi.clubset_workshop.level)
	);

	checkResponse(r, "nao conseguiu adicionar o clubset[TYPEID=" + std::to_string(m_wi._typeid) + "] para o player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdAddClubSet::getUID() {
	return m_uid;
}

void CmdAddClubSet::setUID(uint32_t _uid) {
	m_uid = _uid;
}

WarehouseItemEx& CmdAddClubSet::getInfo() {
	return m_wi;
}

void CmdAddClubSet::setInfo(WarehouseItemEx& _wi) {
	m_wi = _wi;
}

unsigned char CmdAddClubSet::getGiftFlag() {
	return m_gift_flag;
}

void CmdAddClubSet::setGiftFlag(unsigned char _gift_flag) {
	m_gift_flag = _gift_flag;
}

unsigned char CmdAddClubSet::getPurchase() {
	return m_purchase;
}

void CmdAddClubSet::setPurchase(unsigned char _purchase) {
	m_purchase = _purchase;
}
