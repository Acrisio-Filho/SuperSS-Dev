// Arquivo cmd_old_daily_quest_info.cpp
// Criado em 24/03/2018 as 20:56 por Acrisio
// Implementa��o da classe CmdOldDailyQuestInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_old_daily_quest_info.hpp"

using namespace stdA;

CmdOldDailyQuestInfo::CmdOldDailyQuestInfo(bool _waiter) : pangya_db(_waiter), m_uid(0), v_rdqu() {
}

CmdOldDailyQuestInfo::CmdOldDailyQuestInfo(uint32_t _uid, bool _waiter) : pangya_db(_waiter), m_uid(_uid), v_rdqu() {
}

CmdOldDailyQuestInfo::~CmdOldDailyQuestInfo() {
}

void CmdOldDailyQuestInfo::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(2, (uint32_t)_result->cols);

	v_rdqu.push_back({ IFNULL(atoi, _result->data[0]), (uint32_t)IFNULL(atoi, _result->data[1]) });
}

response* CmdOldDailyQuestInfo::prepareConsulta(database& _db) {

	v_rdqu.clear();
	v_rdqu.shrink_to_fit();

	auto r = consulta(_db, m_szConsulta + std::to_string(m_uid));

	checkResponse(r, "nao conseguiu pegar o(s) old daily quest do player: " + std::to_string(m_uid));

	return r;
}

std::vector< RemoveDailyQuestUser >& CmdOldDailyQuestInfo::getInfo() {
	return v_rdqu;
}

uint32_t CmdOldDailyQuestInfo::getUID() {
	return m_uid;
}

void CmdOldDailyQuestInfo::setUID(uint32_t _uid) {
	m_uid = _uid;
}
