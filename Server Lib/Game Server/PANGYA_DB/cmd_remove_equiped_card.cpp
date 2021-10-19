// Arquivo cmd_remove_equiped_card.cpp
// Criado em 17/06/2018 as 21:20 por Acrisio
// Implementa��o da classe CmdRemoveEquipedCard

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_remove_equiped_card.hpp"

using namespace stdA;

CmdRemoveEquipedCard::CmdRemoveEquipedCard(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_cei{0} {
}

CmdRemoveEquipedCard::CmdRemoveEquipedCard(uint32_t _uid, CardEquipInfo& _cei, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_cei(_cei) {
}

CmdRemoveEquipedCard::~CmdRemoveEquipedCard() {
}

void CmdRemoveEquipedCard::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdRemoveEquipedCard::prepareConsulta(database& _db) {

	if (m_uid == 0)
		throw exception("[CmdRemoveEquipedCard::prepareConsulta][Error] m_uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_cei._typeid == 0)
		throw exception("[CmdRemoveEquipedCard::prepareConsulta][Error] CardEquipedInfo is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_cei.parts_id) + ", " + std::to_string(m_cei.parts_typeid)
			+ ", " + std::to_string(m_cei._typeid) + ", " + std::to_string(m_cei.slot)
	);

	checkResponse(r, "nao conseguiu remover card[TYPEID=" + std::to_string(m_cei._typeid) + "] equipado no Character[TYPEID=" 
			+ std::to_string(m_cei.parts_typeid) + ", ID=" + std::to_string(m_cei.parts_id) + "] do player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdRemoveEquipedCard::getUID() {
	return m_uid;
}

void CmdRemoveEquipedCard::setUID(uint32_t _uid) {
	m_uid = _uid;
}

CardEquipInfo& CmdRemoveEquipedCard::getInfo() {
	return m_cei;
}

void CmdRemoveEquipedCard::setInfo(CardEquipInfo& _cei) {
	m_cei = _cei;
}
