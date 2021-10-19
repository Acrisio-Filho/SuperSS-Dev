// Arquivo cmd_add_card.cpp
// Criado em 19/05/2018 as 15:30 por Acrisio
// Implementa��o da classe CmdAddCard

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_add_card.hpp"

using namespace stdA;

CmdAddCard::CmdAddCard(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_gift_flag(0), m_purchase(0), m_ci{0} {
}

CmdAddCard::CmdAddCard(uint32_t _uid, CardInfo& _ci, unsigned char _purchase, unsigned char _gift_flag, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_purchase(_purchase), m_gift_flag(_gift_flag), m_ci(_ci) {
}

CmdAddCard::~CmdAddCard() {
}

void CmdAddCard::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(1, (uint32_t)_result->cols);

	m_ci.id = IFNULL(atoi, _result->data[0]);

}

response* CmdAddCard::prepareConsulta(database& _db) {

	if (m_ci._typeid == 0)
		throw exception("[CmdAddCard::prepareConsulta][Error] Card Info is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	// Ignora as flags purchase e gift por hora, para usar a tabela antiga que eu fiz de card
	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_ci._typeid)
				+ ", " + std::to_string(m_ci.qntd) + ", " + std::to_string((unsigned short)m_ci.type)
	);

	checkResponse(r, "nao conseguiu adicionar o card[TYPEID=" + std::to_string(m_ci._typeid) + "] para o player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdAddCard::getUID() {
	return m_uid;
}

void CmdAddCard::setUID(uint32_t _uid) {
	m_uid = _uid;
}

CardInfo& CmdAddCard::getInfo() {
	return m_ci;
}

void CmdAddCard::setInfo(CardInfo& _ci) {
	m_ci = _ci;
}

unsigned char CmdAddCard::getGiftFlag() {
	return m_gift_flag;
}

void CmdAddCard::setGiftFlag(unsigned char _gift_flag) {
	m_gift_flag = _gift_flag;
}

unsigned char CmdAddCard::getPurchase() {
	return m_purchase;
}

void CmdAddCard::setPurchase(unsigned char _purchase) {
	m_purchase = _purchase;
}
