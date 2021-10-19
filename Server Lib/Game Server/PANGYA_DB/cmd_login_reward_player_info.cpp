// Arquivo cmd_login_reward_player_info.cpp
// Criado em 27/10/2020 as 19:11 por Acrisio
// Implementa��o da classe CmdLoginRewardPlayerInfo

#if defined(_WIN32)
#pragma pack(1)
#endif
#include "cmd_login_reward_player_info.hpp"

using namespace stdA;

CmdLoginRewardPlayerInfo::CmdLoginRewardPlayerInfo(uint64_t _id, uint32_t _uid, bool _waiter) 
	: pangya_db(_waiter), m_id(_id), m_uid(_uid), m_player{0u} {
}

CmdLoginRewardPlayerInfo::CmdLoginRewardPlayerInfo(uint32_t _uid, bool _waiter)
	: pangya_db(_waiter), m_id(0ull), m_uid(_uid), m_player{0u} {
}

CmdLoginRewardPlayerInfo::CmdLoginRewardPlayerInfo(bool _waiter) 
	: pangya_db(_waiter), m_id(0ull), m_uid(0u), m_player{0u} {
}

CmdLoginRewardPlayerInfo::~CmdLoginRewardPlayerInfo() {
}

void CmdLoginRewardPlayerInfo::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(6, (uint32_t)_result->cols);

	SYSTEMTIME upt_date{ 0u };

	if (_result->data[5] != nullptr)
		_translateDate(_result->data[5], &upt_date);

	m_player.id = (uint64_t)IFNULL(atoll, _result->data[0]);
	m_player.uid = (uint32_t)IFNULL(atoi, _result->data[1]);
	m_player.count_days = (uint32_t)IFNULL(atoi, _result->data[2]);
	m_player.count_seq = (uint32_t)IFNULL(atoi, _result->data[3]);
	m_player.update_date = upt_date;
	m_player.is_clear = (IFNULL(atoi, _result->data[4]) ? true : false);
}

response* CmdLoginRewardPlayerInfo::prepareConsulta(database& _db) {
	
	if (m_id == 0ull)
		throw exception("[CmdLoginRewardPlayerInfo::prepareConsulta][Error] m_id is invalid(" + std::to_string(m_id) 
				+ ")", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_uid == 0u)
		throw exception("[CmdLoginRewardPlayerInfo::prepareConsulta][Error] m_uid is invalid(" + std::to_string(m_id)
				+ ")", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	m_player.clear();

	auto r = consulta(_db, m_szConsulta[0] + std::to_string(m_id) + m_szConsulta[1] + std::to_string(m_uid));

	checkResponse(r, "nao conseguiu pegar o Player[UID=" + std::to_string(m_uid) + "] do Login Reward[ID=" + std::to_string(m_id) + "]");

	return r;
}

uint64_t CmdLoginRewardPlayerInfo::getId() {
	return m_id;
}

void CmdLoginRewardPlayerInfo::setId(uint64_t _id) {
	m_id = _id;
}

uint32_t CmdLoginRewardPlayerInfo::getPlayerUID() {
	return m_uid;
}

void CmdLoginRewardPlayerInfo::setPlayerUID(uint32_t _uid) {
	m_uid = _uid;
}

stPlayerState& CmdLoginRewardPlayerInfo::getInfo() {
	return m_player;
}
