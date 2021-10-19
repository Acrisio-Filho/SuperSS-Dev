// Arquivo cmd_equip_card.cpp
// Criado em 17/06/2018 as 18:46 por Acrisio
// Implementa��o da classe CmdEquipCard

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_equip_card.hpp"

using namespace stdA;

CmdEquipCard::CmdEquipCard(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_tempo(0u), m_cei{0} {
}

CmdEquipCard::CmdEquipCard(uint32_t _uid, CardEquipInfoEx& _cei, uint32_t _tempo, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_tempo(_tempo), m_cei(_cei) {
}

CmdEquipCard::~CmdEquipCard() {
}

void CmdEquipCard::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(1, (uint32_t)_result->cols);

	m_cei.index = IFNULL(atoll, _result->data[0]);

	if (m_cei.index < 0)
		throw exception("[CmdEquipCard::lineResult][Error] m_cei[index=" + std::to_string(m_cei.index) + "] is invalid, nao conseguiu equipar o card[TYPEID=" 
				+ std::to_string(m_cei._typeid) + "] para o player[UID=" + std::to_string(m_uid) + "]", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 3, 0));

	return;
}

response* CmdEquipCard::prepareConsulta(database& _db) {

	if (m_uid == 0)
		throw exception("[CmdEquipCard::prepareConsulta][Error] m_uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_cei._typeid == 0)
		throw exception("[CmdEquipCard::prepareConsulta][Error] CardEquipInfo[TYPEID=" + std::to_string(m_cei._typeid) + "] is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	m_cei.index = -1;

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_cei._typeid) + ", " + std::to_string(m_cei.parts_id)
			+ ", " + std::to_string(m_cei.parts_typeid) + ", " + std::to_string(m_cei.efeito) + ", " + std::to_string(m_cei.efeito_qntd)
			+ ", " + std::to_string(m_cei.slot) + ", " + std::to_string(m_cei.tipo) + ", " + std::to_string(m_tempo)
	);

	checkResponse(r, "nao conseguiu equipar o Card[TYPEID=" + std::to_string(m_cei._typeid) + "] no Character[TYPEID=" 
			+ std::to_string(m_cei.parts_typeid) + ", ID=" + std::to_string(m_cei.parts_id) + "] para o player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdEquipCard::getUID() {
	return m_uid;
}

void CmdEquipCard::setUID(uint32_t _uid) {
	m_uid = _uid;
}

uint32_t CmdEquipCard::getTempo() {
	return m_tempo;
}

void CmdEquipCard::setTempo(uint32_t _tempo) {
	m_tempo = _tempo;
}

CardEquipInfoEx& CmdEquipCard::getInfo() {
	return m_cei;
}

void CmdEquipCard::setInfo(CardEquipInfoEx& _cei) {
	m_cei = _cei;
}
