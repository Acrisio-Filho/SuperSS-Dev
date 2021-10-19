// Arquivo cmd_finish_quest_user.cpp
// Criado em 15/04/2018 as 23:44 por Acrisio
// Implementa��o da classe CmdFinishQuestUser

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_finish_quest_user.hpp"

using namespace stdA;

CmdFinishQuestUser::CmdFinishQuestUser(bool _waiter) : pangya_db(_waiter), m_uid(0), m_finish_date_second(0), m_qsi{} {
}

CmdFinishQuestUser::CmdFinishQuestUser(uint32_t _uid, bool _waiter) : pangya_db(_waiter), m_uid(_uid), m_finish_date_second(0), m_qsi{} {
}

CmdFinishQuestUser::CmdFinishQuestUser(uint32_t _uid, QuestStuffInfo& _qsi, bool _waiter) : pangya_db(_waiter), m_uid(_uid), m_finish_date_second(0), m_qsi(_qsi) {
}

CmdFinishQuestUser::~CmdFinishQuestUser() {
}

void CmdFinishQuestUser::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(1, (uint32_t)_result->cols);

	m_finish_date_second = IFNULL(atoi, _result->data[0]);
}

response* CmdFinishQuestUser::prepareConsulta(database& _db) {

	if (m_uid == 0)
		throw exception("[CmdFinishQuestUser::prepareConsulta][Error] m_uid is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_qsi.id <= 0 || m_qsi._typeid == 0)
		throw exception("[CmdFinishQuestUser][Error] QuestStuffInfoEx _qsi is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 5, 0));

	m_finish_date_second = 0;

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_qsi.id));

	checkResponse(r, "nao conseguiu finilizar a quest[ID=" + std::to_string(m_qsi.id) + "] do player: " + std::to_string(m_uid));

	return r;
}

uint32_t CmdFinishQuestUser::getUID() {
	return m_uid;
}

void CmdFinishQuestUser::setUID(uint32_t _uid) {
	m_uid = _uid;
}

QuestStuffInfo& CmdFinishQuestUser::getInfo() {
	return m_qsi;
}

void CmdFinishQuestUser::setInfo(QuestStuffInfo& _qsi) {
	m_qsi = _qsi;
}

uint32_t CmdFinishQuestUser::getFinishDateSecond() {
	return m_finish_date_second;
}
