// Arquivo cmd_update_login_reward_player.cpp
// Criado em 27/10/2020 as 20:21 por Acrisio
// Implementa��o da classe CmdUpdateLoginRewardPlayer

#if defined(_WIN32)
#pragma pack(1)
#endif
#include "cmd_update_login_reward_player.hpp"
#include "../../Projeto IOCP/UTIL/util_time.h"

using namespace stdA;

CmdUpdateLoginRewardPlayer::CmdUpdateLoginRewardPlayer(stPlayerState& _ps, bool _waiter) : pangya_db(_waiter), m_ps(_ps) {
}

CmdUpdateLoginRewardPlayer::CmdUpdateLoginRewardPlayer(bool _waiter) : pangya_db(_waiter), m_ps{ 0u } {
}

CmdUpdateLoginRewardPlayer::~CmdUpdateLoginRewardPlayer() {
}

void CmdUpdateLoginRewardPlayer::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdUpdateLoginRewardPlayer::prepareConsulta(database& _db) {
	
	if (m_ps.id == 0ull)
		throw exception("[CmdUpdateLoginRewardPlayer::prepareConsulta][Error] m_ps.id is invalid(" + std::to_string(m_ps.id) 
				+ ")", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_ps.uid == 0u)
		throw exception("[CmdUpdateLoginRewardPlayer::prepareConsulta][Error] m_ps.uid is invalid(" + std::to_string(m_ps.uid)
				+ ")", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_ps.id) + ", " + std::to_string(m_ps.uid) + ", " + std::to_string(m_ps.count_days)
		+ ", " + std::to_string(m_ps.count_seq) + ", " + std::string(m_ps.is_clear ? "1" : "0") + ", " + _db.makeText(_formatDate(m_ps.update_date))
	);

	checkResponse(r, "nao conseguiu atualizar o Player[" + m_ps.toString() + "]");

	return r;
}

stPlayerState& CmdUpdateLoginRewardPlayer::getPlayerState() {
	return m_ps;
}

void CmdUpdateLoginRewardPlayer::setPlayerState(stPlayerState& _ps) {
	m_ps = _ps;
}
