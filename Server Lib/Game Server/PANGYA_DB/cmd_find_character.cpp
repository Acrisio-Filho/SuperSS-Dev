// Arquivo cmd_find_character.cpp
// Criado em 20/05/2018 as 23:22 por Acrisio
// Implementa��o da classe CmdFindCharacter

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_find_character.hpp"

#include "../../Projeto IOCP/UTIL/iff.h"

using namespace stdA;

CmdFindCharacter::CmdFindCharacter(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_typeid(0u), m_ci{0} {
}

CmdFindCharacter::CmdFindCharacter(uint32_t _uid, uint32_t _typeid, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_typeid(_typeid), m_ci{0} {
}

CmdFindCharacter::~CmdFindCharacter() {
}

void CmdFindCharacter::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(81, (uint32_t)_result->cols);

	m_ci.id = IFNULL(atoi, _result->data[0]);

	if (m_ci.id > 0) { // found
		auto i = 0;

		m_ci._typeid = IFNULL(atoi, _result->data[1]);
		for (i = 0; i < 24; i++)
			m_ci.parts_id[i] = IFNULL(atoi, _result->data[2 + i]);		// 2 + 24
		for (i = 0; i < 24; i++)
			m_ci.parts_typeid[i] = IFNULL(atoi, _result->data[26 + i]);	// 26 + 24
		m_ci.default_hair = (unsigned char)IFNULL(atoi, _result->data[50]);
		m_ci.default_shirts = (unsigned char)IFNULL(atoi, _result->data[51]);
		m_ci.gift_flag = (unsigned char)IFNULL(atoi, _result->data[52]);
		for (i = 0; i < 5; i++)
			m_ci.pcl[i] = (unsigned char)IFNULL(atoi, _result->data[53 + i]);	// 53 + 5
		m_ci.purchase = (unsigned char)IFNULL(atoi, _result->data[58]);
		for (i = 0; i < 5; i++)
			m_ci.auxparts[i] = IFNULL(atoi, _result->data[59 + i]);				// 59 + 5
		for (i = 0; i < 4; i++)
			m_ci.cut_in[i] = IFNULL(atoi, _result->data[64 + i]);					// 64 + 4 Cut-in deveria guarda no db os outros 3 se for msm os 4 que penso q seja, � sim no JP USA os 4
		m_ci.mastery = IFNULL(atoi, _result->data[68]);
		for (i = 0; i < 4; i++)
			m_ci.card_character[i] = IFNULL(atoi, _result->data[69 + i]);		// 69 + 4
		for (i = 0; i < 4; i++)
			m_ci.card_caddie[i] = IFNULL(atoi, _result->data[73 + i]);			// 73 + 4
		for (i = 0; i < 4; i++)
			m_ci.card_NPC[i] = IFNULL(atoi, _result->data[77 + i]);				// 77 + 4
	}
}

response* CmdFindCharacter::prepareConsulta(database& _db) {

	if (m_typeid == 0 && sIff::getInstance().getItemGroupIdentify(m_typeid) != iff::CHARACTER)
		throw exception("[CmdFindCharacter::prepareConsulta][Error] typeid character invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	m_ci.clear();
	m_ci.id = -1;

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_typeid));

	checkResponse(r, "erro ao encontrar o character[UID=" + std::to_string(m_typeid) + "] do player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdFindCharacter::getUID() {
	return m_uid;
}

void CmdFindCharacter::setUID(uint32_t _uid) {
	m_uid = _uid;
}

uint32_t CmdFindCharacter::getTypeid() {
	return m_typeid;
}

void CmdFindCharacter::setTypeid(uint32_t _typeid) {
	m_typeid = _typeid;
}

CharacterInfo& CmdFindCharacter::getInfo() {
	return m_ci;
}

bool CmdFindCharacter::hasFound() {
	return m_ci.id > 0;
}
