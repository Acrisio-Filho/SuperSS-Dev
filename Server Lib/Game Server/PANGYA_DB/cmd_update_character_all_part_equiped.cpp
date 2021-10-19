// Arquivo cmd_update_character_all_part_equiped.cpp
// Criado em 25/03/2018 as 13:17 por Acrisio
// Implementa��o da classe CmdUpdateCharacterAllPartEquiped

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_character_all_part_equiped.hpp"

using namespace stdA;

CmdUpdateCharacterAllPartEquiped::CmdUpdateCharacterAllPartEquiped(bool _waiter) : pangya_db(_waiter), m_uid(0), m_ci{0} {
}

CmdUpdateCharacterAllPartEquiped::CmdUpdateCharacterAllPartEquiped(uint32_t _uid, CharacterInfo& _ci, bool _waiter)
		: pangya_db(_waiter), m_uid(_uid), m_ci(_ci) {
}

CmdUpdateCharacterAllPartEquiped::~CmdUpdateCharacterAllPartEquiped() {
}

void CmdUpdateCharacterAllPartEquiped::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa aqui por que � um UPDATE
	return;
}

response* CmdUpdateCharacterAllPartEquiped::prepareConsulta(database& _db) {

	std::string q = ""; // "|" + std::to_string(_ci._typeid) + "|" + std::to_string(_ci.id);

	q += "|" + std::to_string((unsigned short)m_ci.default_hair) + "|" + std::to_string((unsigned short)m_ci.default_shirts);
	q += "|" + std::to_string((unsigned short)m_ci.gift_flag) + "|" + std::to_string((unsigned short)m_ci.purchase);

	auto is = 0u;

	for (is = 0u; is < (sizeof(m_ci.parts_typeid) / sizeof(m_ci.parts_typeid[0])); ++is)
		q += "|" + std::to_string(m_ci.parts_typeid[is]);

	for (is = 0u; is < (sizeof(m_ci.parts_id) / sizeof(m_ci.parts_id[0])); ++is)
		q += "|" + std::to_string(m_ci.parts_id[is]);

	for (is = 0u; is < (sizeof(m_ci.auxparts) / sizeof(m_ci.auxparts[0])); ++is)
		q += "|" + std::to_string(m_ci.auxparts[is]);

	// Depois o Cutin tem que colocar os 4, aqui s� est� 1 que era o antigo, que eu fiz
	for (is = 0u; is < (sizeof(m_ci.cut_in) / sizeof(m_ci.cut_in[0])); ++is)
		q += "|" + std::to_string(m_ci.cut_in[is]);

	for (is = 0u; is < (sizeof(m_ci.pcl) / sizeof(m_ci.pcl[0])); ++is)
		q += "|" + std::to_string((unsigned short)m_ci.pcl[is]);

	// Mastery Character
	q += "|" + std::to_string(m_ci.mastery);

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_ci.id) + ", " + _db.makeText(q));

	checkResponse(r, "nao conseguiu atualizar o character[ID=" + std::to_string(m_ci.id) + "] parts equipado do player: " + std::to_string(m_uid));

	return r;
}

uint32_t CmdUpdateCharacterAllPartEquiped::getUID() {
	return m_uid;
}

void CmdUpdateCharacterAllPartEquiped::setUID(uint32_t _uid) {
	m_uid = _uid;
}

CharacterInfo& CmdUpdateCharacterAllPartEquiped::getInfo() {
	return m_ci;
}

void CmdUpdateCharacterAllPartEquiped::setInfo(CharacterInfo& _ci) {
	m_ci = _ci;
}
