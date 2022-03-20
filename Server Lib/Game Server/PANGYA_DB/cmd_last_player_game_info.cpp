// Arquivo cmd_last_player_game_info.cpp
// Criado em 24/03/2018 as 19:06 por Acrisio
// Implementa��o da classe CmdLastPlayerGameInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_last_player_game_info.hpp"

using namespace stdA;

CmdLastPlayerGameInfo::CmdLastPlayerGameInfo(bool _waiter) : pangya_db(_waiter), m_uid(0), m_l5pg{0} {
}

CmdLastPlayerGameInfo::CmdLastPlayerGameInfo(uint32_t _uid, bool _waiter) : pangya_db(_waiter), m_uid(_uid), m_l5pg{0} {
}

CmdLastPlayerGameInfo::~CmdLastPlayerGameInfo() {
}

void CmdLastPlayerGameInfo::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(20, (uint32_t)_result->cols);

	for (auto i = 0; i < 5; i++) {
		m_l5pg.players[i].sex = IFNULL(atoi, _result->data[i * 4]);
		
		if (is_valid_c_string(_result->data[i * 4 + 1]))
			STRCPY_TO_MEMORY_FIXED_SIZE(m_l5pg.players[i].nick, sizeof(m_l5pg.players[i].nick), _result->data[i * 4 + 1]);
		
		if (is_valid_c_string(_result->data[i * 4 + 2]))
			STRCPY_TO_MEMORY_FIXED_SIZE(m_l5pg.players[i].id, sizeof(m_l5pg.players[i].id), _result->data[i * 4 + 2]);
		
		m_l5pg.players[i].uid = IFNULL(atoi, _result->data[i * 4 + 3]);
	}
}

response* CmdLastPlayerGameInfo::prepareConsulta(database& _db) {

	m_l5pg.clear();

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid));

	checkResponse(r, "nao conseguiu pegar os ultimos players game info do player: " + std::to_string(m_uid));

	return r;
}

Last5PlayersGame& CmdLastPlayerGameInfo::getInfo() {
	return m_l5pg;
}

void CmdLastPlayerGameInfo::setInfo(Last5PlayersGame& _l5pg) {
	m_l5pg = _l5pg;
}

uint32_t CmdLastPlayerGameInfo::getUID() {
	return m_uid;
}

void CmdLastPlayerGameInfo::setUID(uint32_t _uid) {
	m_uid = _uid;
}
