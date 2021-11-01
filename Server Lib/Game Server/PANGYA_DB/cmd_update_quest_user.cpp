// Arquivo cmd_update_quest_user.cpp
// Criado em 14/04/2018 as 16:03 por Acrisio
// Implementa��o da classe CmdUpdateQuestUser

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_update_quest_user.hpp"

using namespace stdA;

CmdUpdateQuestUser::CmdUpdateQuestUser(bool _waiter) : pangya_db(_waiter), m_uid(0), m_qsi{} {
}

CmdUpdateQuestUser::CmdUpdateQuestUser(uint32_t _uid, QuestStuffInfo& _qsi, bool _waiter) : pangya_db(_waiter), m_uid(_uid), m_qsi(_qsi) {
}

CmdUpdateQuestUser::~CmdUpdateQuestUser() {
}

void CmdUpdateQuestUser::lineResult(result_set::ctx_res* /*_result*/, uint32_t /*_index_result*/) {

	// N�o usa por que � um UPDATE
	return;
}

response* CmdUpdateQuestUser::prepareConsulta(database& _db) {

	if (m_qsi.id <= 0 || m_qsi._typeid == 0 || m_qsi.counter_item_id <= 0)
		throw exception("[CmdUpdateQuestUser::prepareConsulta][Error] QuestStuffInfoEx m_qsi is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	std::string clear_dt = "null";

	if (m_qsi.clear_date_unix != 0)
		clear_dt = _db.makeText(formatDateLocal(m_qsi.clear_date_unix));

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + std::to_string(m_qsi.id) + ", " + std::to_string(m_qsi.counter_item_id) + ", " + clear_dt);

	checkResponse(r, "nao conseguiu atualizar a quest[ID=" + std::to_string(m_qsi.id) + "] do player: " + std::to_string(m_uid));

	return r;
}

uint32_t CmdUpdateQuestUser::getUID() {
	return m_uid;
}

void CmdUpdateQuestUser::setUID(uint32_t _uid) {
	m_uid = _uid;
}

QuestStuffInfo& CmdUpdateQuestUser::getInfo() {
	return m_qsi;
}

void CmdUpdateQuestUser::setInfo(QuestStuffInfo& _qsi) {
	m_qsi = _qsi;
}
