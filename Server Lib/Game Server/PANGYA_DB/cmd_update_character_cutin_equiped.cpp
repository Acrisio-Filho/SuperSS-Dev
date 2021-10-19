// Arquivo cmd_update_character_cutin_equiped.cpp
// Criado em 25/03/2018 as 12:28 por Acrisio
// Implementa��o da classe CmdUpdateCharacterCutinEquiped

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_character_cutin_equiped.hpp"

using namespace stdA;

CmdUpdateCharacterCutinEquiped::CmdUpdateCharacterCutinEquiped(bool _waiter) : pangya_db(_waiter), m_uid(0), m_ci{0} {
}

CmdUpdateCharacterCutinEquiped::CmdUpdateCharacterCutinEquiped(uint32_t _uid, CharacterInfo& _ci, bool _waiter)
		: pangya_db(_waiter), m_uid(_uid), m_ci(_ci) {
}

CmdUpdateCharacterCutinEquiped::~CmdUpdateCharacterCutinEquiped() {
}

void CmdUpdateCharacterCutinEquiped::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdUpdateCharacterCutinEquiped::prepareConsulta(database& _db) {

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_ci.id)
					+ ", " + std::to_string(m_ci.cut_in[0]) + ", " + std::to_string(m_ci.cut_in[1])
					+ ", " + std::to_string(m_ci.cut_in[2]) + ", " + std::to_string(m_ci.cut_in[3]));

	checkResponse(r, "nao conseguiu atualizar o character[ID=" + std::to_string(m_ci.id) + "] cutin equipado do player: " + std::to_string(m_uid));

	return r;
}

uint32_t CmdUpdateCharacterCutinEquiped::getUID() {
	return m_uid;
}

void CmdUpdateCharacterCutinEquiped::setUID(uint32_t _uid) {
	m_uid = _uid;
}

CharacterInfo& CmdUpdateCharacterCutinEquiped::getInfo() {
	return m_ci;
}

void CmdUpdateCharacterCutinEquiped::setInfo(CharacterInfo& _ci) {
	m_ci = _ci;
}
