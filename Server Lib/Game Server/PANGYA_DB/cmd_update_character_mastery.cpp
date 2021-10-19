// Arquivo cmd_update_character_mastery.cpp
// Criado em 17/06/2018 as 11:15 por Acrisio
// Implementa��o da classe CmdUpdateCharacterMastery

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_character_mastey.hpp"

using namespace stdA;

CmdUpdateCharacterMastery::CmdUpdateCharacterMastery(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_ci{0} {
}

CmdUpdateCharacterMastery::CmdUpdateCharacterMastery(uint32_t _uid, CharacterInfo& _ci, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_ci(_ci) {
}

CmdUpdateCharacterMastery::~CmdUpdateCharacterMastery() {
}

void CmdUpdateCharacterMastery::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdUpdateCharacterMastery::prepareConsulta(database& _db) {

	if (m_uid == 0)
		throw exception("[CmdUpdateCharacterMastery::prepareConsulta][Error] m_uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_ci.id <= 0 || m_ci._typeid == 0)
		throw exception("[CmdUpdateCharacterMastery::prepareConsulta][Error] CharacterInfo[TYPEID=" + std::to_string(m_ci._typeid) + ", ID=" + std::to_string(m_ci.id) + "] is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = _update(_db, m_szConsulta[0] + std::to_string(m_ci.mastery) + m_szConsulta[1] + std::to_string(m_uid) + m_szConsulta[2] + std::to_string(m_ci.id));

	checkResponse(r, "nao conseguiu atualizar Character[TYPEID=" + std::to_string(m_ci._typeid) + ", ID=" + std::to_string(m_ci.id) + "] Mastery[value=" + std::to_string(m_ci.mastery) + "] do player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdUpdateCharacterMastery::getUID() {
	return m_uid;
}

void CmdUpdateCharacterMastery::setUID(uint32_t _uid) {
	m_uid = _uid;
}

CharacterInfo& CmdUpdateCharacterMastery::getInfo() {
	return m_ci;
}

void CmdUpdateCharacterMastery::setInfo(CharacterInfo& _ci) {
	m_ci = _ci;
}
