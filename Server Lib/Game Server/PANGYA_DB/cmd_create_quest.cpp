// Arquivo cmd_create_quest.cpp
// Criado em 31/03/2018 as 22:58 por Acrisio
// Implementa��o da classe CmdCreateQuest

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_create_quest.hpp"

using namespace stdA;

CmdCreateQuest::CmdCreateQuest(bool _waiter) : pangya_db(_waiter), m_uid(0), m_id(-1), m_counter_item_id(-1), m_quest{}, m_include_counter(false) {
}

CmdCreateQuest::CmdCreateQuest(uint32_t _uid, bool _waiter) : pangya_db(_waiter), m_uid(_uid), m_id(-1), m_counter_item_id(-1), m_quest{}, m_include_counter(false) {
}

CmdCreateQuest::CmdCreateQuest(uint32_t _uid, uint32_t _achievement_id, IFF::QuestStuff& _quest, bool _include_counter, bool _waiter) :
		pangya_db(_waiter), m_uid(_uid), m_achievement_id(_achievement_id), m_id(-1), m_counter_item_id(-1), m_quest(_quest), m_include_counter(_include_counter) {
}

CmdCreateQuest::~CmdCreateQuest() {
}

void CmdCreateQuest::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(2, (uint32_t)_result->cols);

	m_id = IFNULL(atoi, _result->data[0]);
	m_counter_item_id = IFNULL(atoi, _result->data[1]);
}

response* CmdCreateQuest::prepareConsulta(database& _db) {

	if (m_quest._typeid == 0)
		throw exception("[CmdCreateQuest::prepareConsulta][Error] QuestStuff invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	if (m_quest.counter_item.qntd[0] == 0 && m_quest.counter_item._typeid[0] == 0)
		throw exception("[CmdCreateQuest::prepareConsulta][Error] IFF::QuestStuff not have counter item, invalid create quest", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 1));

	m_id = -1;
	m_counter_item_id = -1;

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + _db.makeText(m_quest.name) + ", " +  std::to_string(m_achievement_id)
				+ ", " + std::to_string(m_quest._typeid) + ", " + std::to_string((m_include_counter ? m_quest.counter_item._typeid[0] : 0)));

	checkResponse(r, "nao conseguiu adicionar Quest para o player: " + std::to_string(m_uid));

	return r;
}

uint32_t CmdCreateQuest::getUID() {
	return m_uid;
}

void CmdCreateQuest::setUID(uint32_t _uid) {
	m_uid = _uid;
}

uint32_t CmdCreateQuest::getAchievementID() {
	return m_achievement_id;
}

void CmdCreateQuest::setAchievementID(uint32_t _achievement_id) {
	m_achievement_id = _achievement_id;
}

IFF::QuestStuff& CmdCreateQuest::getQuest() {
	return m_quest;
}

void CmdCreateQuest::setQuest(IFF::QuestStuff& _quest, bool _include_counter) {
	m_quest = _quest;
	m_include_counter = _include_counter;
}

bool CmdCreateQuest::getIncludeCounter() {
	return m_include_counter;
}

int32_t CmdCreateQuest::getID() {
	return m_id;
}

int32_t CmdCreateQuest::getCounterItemID() {
	return m_counter_item_id;
}
