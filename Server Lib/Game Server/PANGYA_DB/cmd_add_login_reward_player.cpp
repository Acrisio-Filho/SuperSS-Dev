// Arquivo cmd_add_login_reward_player.cpp
// Criado em 27/10/2020 as 19:37 por Acrisio
// Implementa��o da classe CmdAddLoginRewardPlayer

#if defined(_WIN32)
#pragma pack(1)
#endif
#include "cmd_add_login_reward_player.hpp"
#include "../../Projeto IOCP/UTIL/util_time.h"

using namespace stdA;

CmdAddLoginRewardPlayer::CmdAddLoginRewardPlayer(uint64_t _id, stPlayerState& _ps, bool _waiter)
	: pangya_db(_waiter), m_id(_id), m_ps(_ps) {
}

CmdAddLoginRewardPlayer::CmdAddLoginRewardPlayer(bool _waiter) : pangya_db(_waiter), m_id(0ull), m_ps{ 0u } {
}

CmdAddLoginRewardPlayer::~CmdAddLoginRewardPlayer() {
}

void CmdAddLoginRewardPlayer::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(1, (uint32_t)_result->cols);

	m_ps.id = (uint64_t)IFNULL(atoll, _result->data[0]);

	if (m_ps.id == 0ull)
		throw exception("[CmdAddLoginRewardPlayer::lineResult][Error] nao conseguiu adicionar player no Login Reward[ID=" + std::to_string(m_id) 
				+ "] por que m_ps.id retornado is invalid(" + std::to_string(m_ps.id) + ").", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 3, 0));
}

response* CmdAddLoginRewardPlayer::prepareConsulta(database& _db) {
	
	if (m_id == 0u)
		throw exception("[CmdAddLoginRewardPlayer::prepareConsulta][Error] m_id is invalid(" + std::to_string(m_id) 
				+ ")", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_ps.uid == 0u)
		throw exception("[CmdAddLoginRewardPlayer::prepareConsulta][Error] m_ps.uid is invalid(" + std::to_string(m_ps.uid)
				+ ")", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	m_ps.id = 0ull;

	auto r = procedure(_db, m_szConsulta, std::to_string(m_id) + ", " + std::to_string(m_ps.uid) + ", " + std::to_string(m_ps.count_days)
		+ ", " + std::to_string(m_ps.count_seq) + ", " + (m_ps.is_clear ? "1" : "0") + ", " + _db.makeText(_formatDate(m_ps.update_date))
	);

	checkResponse(r, "nao conseguiu adicionar o Player[" + m_ps.toString() + "] do Login Reward[ID=" + std::to_string(m_id) + "]");

	return r;
}

uint64_t CmdAddLoginRewardPlayer::getId() {
	return m_id;
}

void CmdAddLoginRewardPlayer::setId(uint64_t _id) {
	m_id = _id;
}

stPlayerState& CmdAddLoginRewardPlayer::getPlayerState() {
	return m_ps;
}

void CmdAddLoginRewardPlayer::setPlayerState(stPlayerState& _ps) {
	m_ps = _ps;
}

bool CmdAddLoginRewardPlayer::isGood() {
	return m_ps.id != 0ull;
}
