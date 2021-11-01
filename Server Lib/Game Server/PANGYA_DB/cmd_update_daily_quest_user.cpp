// Arquivo cmd_update_daily_quest_user.cpp
// Criado em 11/11/2018 as 16:04 por Acrisio
// Implementa��o da classe CmdUpdateDailyQuestUser

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_daily_quest_user.hpp"

using namespace stdA;

CmdUpdateDailyQuestUser::CmdUpdateDailyQuestUser(bool _waiter) : pangya_db(_waiter), m_uid(0u), m_dqiu{0} {
}

CmdUpdateDailyQuestUser::CmdUpdateDailyQuestUser(uint32_t _uid, DailyQuestInfoUser& _dqiu, bool _waiter)
	: pangya_db(_waiter), m_uid(_uid), m_dqiu(_dqiu) {
}

CmdUpdateDailyQuestUser::~CmdUpdateDailyQuestUser() {
}

void CmdUpdateDailyQuestUser::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdUpdateDailyQuestUser::prepareConsulta(database& _db) {

	if (m_uid == 0u)
		throw exception("[CmdUpdateDailyQuestUser][Error] m_uid is invalid(zero).", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	std::string accept_dt = "null", today_dt = "null";

	if (m_dqiu.accept_date != 0)
		accept_dt = _db.makeText(formatDateLocal(m_dqiu.accept_date));

	if (m_dqiu.current_date != 0)
		today_dt = _db.makeText(formatDateLocal(m_dqiu.current_date));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + accept_dt + ", " + today_dt);

	checkResponse(r, "nao conseguiu Atualizar o DailyQuest[ACCEPT_DT=" + accept_dt + ", TODAY_DT=" + today_dt + "] do player[UID=" + std::to_string(m_uid) + "]");

	return r;
}

uint32_t CmdUpdateDailyQuestUser::getUID() {
	return m_uid;
}

void CmdUpdateDailyQuestUser::setUID(uint32_t _uid) {
	m_uid = _uid;
}

DailyQuestInfoUser& CmdUpdateDailyQuestUser::getInfo() {
	return m_dqiu;
}

void CmdUpdateDailyQuestUser::setInfo(DailyQuestInfoUser& _dqiu) {
	m_dqiu = _dqiu;
}
