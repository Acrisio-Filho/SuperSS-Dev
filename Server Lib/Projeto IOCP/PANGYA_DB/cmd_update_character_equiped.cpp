// Arquivo cmd_update_character_equiped.cpp
// Criado em 25/03/2018 as 11:31 por Acrisio
// Implementa��o da classe CmdUpdateCharacterEquiped

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_character_equiped.hpp"

using namespace stdA;

CmdUpdateCharacterEquiped::CmdUpdateCharacterEquiped(bool _waiter) : pangya_db(_waiter), m_uid(0), m_character_id(-1) {
}

CmdUpdateCharacterEquiped::CmdUpdateCharacterEquiped(uint32_t _uid, int32_t _character_id, bool _waiter)
		: pangya_db(_waiter), m_uid(_uid), m_character_id(_character_id) {
}

CmdUpdateCharacterEquiped::~CmdUpdateCharacterEquiped() {
}

void CmdUpdateCharacterEquiped::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdUpdateCharacterEquiped::prepareConsulta(database& _db) {

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_character_id));

	checkResponse(r, "nao conseguiu atualizar o character[ID=" + std::to_string(m_character_id) + "] equipado do player: " + std::to_string(m_uid));

	return r;
}

uint32_t CmdUpdateCharacterEquiped::getUID() {
	return m_uid;
}

void CmdUpdateCharacterEquiped::setUID(uint32_t _uid) {
	m_uid = _uid;
}

int32_t CmdUpdateCharacterEquiped::getCharacterID() {
	return m_character_id;
}

void CmdUpdateCharacterEquiped::setCharacterID(int32_t _character_id) {
	m_character_id = _character_id;
}
