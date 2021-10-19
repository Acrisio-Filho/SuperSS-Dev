// Arquivo cmd_update_player_location.cpp
// Criado em 11/05/2019 as 17:38 por Acrisio
// Implementação da classe CmdUpdatePlayerLocation

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_player_location.hpp"

using namespace stdA;

CmdUpdatePlayerLocation::CmdUpdatePlayerLocation(stPlayerLocationDB& _pl, bool _waiter) : pangya_db(_waiter), m_uid(0u), m_pl(_pl) {
}

CmdUpdatePlayerLocation::CmdUpdatePlayerLocation(uint32_t _uid, stPlayerLocationDB& _pl, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_pl(_pl) {
}

CmdUpdatePlayerLocation::~CmdUpdatePlayerLocation() {
}

void CmdUpdatePlayerLocation::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {
	
	// Não usa por que é um UPDATE
	return;
}

response* CmdUpdatePlayerLocation::prepareConsulta(database& _db) {

	if (m_uid == 0u)
		throw exception("[CmdUpdatePlayerLocation::prepareConsulta][Error] Player[UID=" + std::to_string(m_uid) + "] is invalid.", 
				STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string((short)m_pl.channel)
			+ ", " + std::to_string((short)m_pl.lobby) + ", " + std::to_string(m_pl.room) + ", " + std::to_string((unsigned short)m_pl.place)
	);

	checkResponse(r, "nao conseguiu atualizar Player[UID=" + std::to_string(m_uid) + "] Location[CHANNEL=" + std::to_string((short)m_pl.channel)
		+ ", LOBBY=" + std::to_string((short)m_pl.lobby) + ", ROOM=" + std::to_string(m_pl.room) + ", PLACE=" + std::to_string((unsigned short)m_pl.place) + "]");

	return r;
}

uint32_t CmdUpdatePlayerLocation::getUID() {
	return m_uid;
}

void CmdUpdatePlayerLocation::setUID(uint32_t _uid) {
	m_uid = _uid;
}

stPlayerLocationDB& CmdUpdatePlayerLocation::getInfo() {
	return m_pl;
}
