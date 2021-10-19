// Arquivo cmd_update_achievement_user.cpp
// Criado em 14/04/2018 as 17:13 por Acrisio
// Implementa��o da classe CmdUpdateAchievementUser

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_achievement_user.hpp"

using namespace stdA;

CmdUpdateAchievementUser::CmdUpdateAchievementUser(bool _waiter) : pangya_db(_waiter), m_uid(0), m_ai{} {
}

CmdUpdateAchievementUser::CmdUpdateAchievementUser(uint32_t _uid, AchievementInfoEx& _ai, bool _waiter)
		: pangya_db(_waiter), m_uid(_uid), m_ai(_ai) {
}

CmdUpdateAchievementUser::~CmdUpdateAchievementUser() {
}

void CmdUpdateAchievementUser::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdUpdateAchievementUser::prepareConsulta(database& _db) {

	if (m_ai.id <= 0 || m_ai._typeid == 0)
		throw exception("[CmdUpdateAchievementUser::prepareConsulta][Error] AchievementInfoEx m_ai is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	auto r = _update(_db, m_szConsulta[0] + std::to_string(m_ai.status) + m_szConsulta[1] + std::to_string(m_uid) + m_szConsulta[2] + std::to_string(m_ai.id));

	checkResponse(r, "nao conseguiu atualizar achievement[ID=" + std::to_string(m_ai.id) + "] do player: " + std::to_string(m_uid));

	return r;
}

uint32_t CmdUpdateAchievementUser::getUID() {
	return m_uid;
}

void CmdUpdateAchievementUser::setUID(uint32_t _uid) {
	m_uid = _uid;
}

AchievementInfoEx& CmdUpdateAchievementUser::getInfo() {
	return m_ai;
}

void CmdUpdateAchievementUser::setInfo(AchievementInfoEx& _ai) {
	m_ai = _ai;
}
