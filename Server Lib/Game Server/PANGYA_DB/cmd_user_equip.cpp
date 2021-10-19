// Arquivo cmd_user_equip.cpp
// Criado em 18/03/2018 as 17:47 por Acrisio
// Implementa��o da classe CmdUserEquip

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_user_equip.hpp"

using namespace stdA;

CmdUserEquip::CmdUserEquip(bool _waiter) : pangya_db(_waiter), m_uid(0), m_ue{0} {
}

CmdUserEquip::CmdUserEquip(uint32_t _uid, bool _waiter) : pangya_db(_waiter), m_uid(_uid), m_ue{0} {
}

CmdUserEquip::~CmdUserEquip() {
}

void CmdUserEquip::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(29, (uint32_t)_result->cols);

	auto i = 0;

	m_ue.caddie_id = IFNULL(atoi, _result->data[0]);
	m_ue.character_id = IFNULL(atoi, _result->data[1]);
	m_ue.clubset_id = IFNULL(atoi, _result->data[2]);
	m_ue.ball_typeid = IFNULL(atoi, _result->data[3]);
	for (i = 0; i < 10; i++)
		m_ue.item_slot[i] = IFNULL(atoi, _result->data[4 + i]);		// 4 + 10
	for (i = 0; i < 6; i++)
		m_ue.skin_id[i] = IFNULL(atoi, _result->data[14 + i]);		// 14 + 6
	for (i = 0; i < 6; i++)
		m_ue.skin_typeid[i] = IFNULL(atoi, _result->data[20 + i]);	// 20 + 6
	m_ue.mascot_id = IFNULL(atoi, _result->data[26]);
	for (i = 0; i < 2; i++)
		m_ue.poster[i] = IFNULL(atoi, _result->data[27 + i]);		// 27 + 2
}

response* CmdUserEquip::prepareConsulta(database& _db) {
	
	m_ue.clear();

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid));

	checkResponse(r, "nao conseguiu pegar o UserEquip do player: " + std::to_string(m_uid));

	return r;
}

UserEquip& CmdUserEquip::getEquip() {
	return m_ue;
}

void CmdUserEquip::setEquip(UserEquip& _ue) {
	m_ue = _ue;
}

uint32_t CmdUserEquip::getUID() {
	return m_uid;
}

void CmdUserEquip::setUID(uint32_t _uid) {
	m_uid = _uid;
}
