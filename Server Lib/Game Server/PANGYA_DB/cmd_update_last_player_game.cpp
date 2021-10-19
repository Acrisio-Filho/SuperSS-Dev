// Arquivo cmd_update_last_player_game.cpp
// Criado em 28/10/2018 as 14:24 por Acrisio
// Implementa��o da classe CmdUpdateLastPlayerGame

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_last_player_game.hpp"

using namespace stdA;

CmdUpdateLastPlayerGame::CmdUpdateLastPlayerGame(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_l5pg{0} {
}

CmdUpdateLastPlayerGame::CmdUpdateLastPlayerGame(uint32_t _uid, Last5PlayersGame& _l5pg, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_l5pg(_l5pg) {
}

CmdUpdateLastPlayerGame::~CmdUpdateLastPlayerGame() {
}

void CmdUpdateLastPlayerGame::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdUpdateLastPlayerGame::prepareConsulta(database& _db) {

	if (m_uid == 0u)
		throw exception("[CmdUpdateLastPlayerGame::prepareConsulta][Error] uid is invalid(0)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	std::string param = "";

	for (auto i = 0u; i < (sizeof(m_l5pg.players) / sizeof(m_l5pg.players[0])); ++i) {

		if (m_l5pg.players[i].uid == 0u)	// n�o tem Player nesse passa null pra o DB
			param += ", null, null, null, null";
		else {
			param += ", " + std::to_string(m_l5pg.players[i].uid) + ", " + std::to_string(m_l5pg.players[i].sex);
			param += (std::empty(m_l5pg.players[i].id) ? ", null" : ", " + _db.makeText(m_l5pg.players[i].id));
			param += (std::empty(m_l5pg.players[i].nick) ? ", null" : ", " + _db.makeText(m_l5pg.players[i].nick));
		}
	}

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + param);

	checkResponse(r, "nao conseguiu atualizar o Last 5 Player Game do player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdUpdateLastPlayerGame::getUID() {
	return m_uid;
}

void CmdUpdateLastPlayerGame::setUID(uint32_t _uid) {
	m_uid = _uid;
}

Last5PlayersGame& CmdUpdateLastPlayerGame::getInfo() {
	return m_l5pg;
}

void CmdUpdateLastPlayerGame::setInfo(Last5PlayersGame& _l5pg) {
	m_l5pg = _l5pg;
}
