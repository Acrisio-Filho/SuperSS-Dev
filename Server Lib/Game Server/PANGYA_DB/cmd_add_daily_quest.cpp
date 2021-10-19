// Arquivo cmd_add_daily_quest.cpp
// Criado em 24/03/2018 as 21:25 por Acrisio
// Implementa��o da classe CmdAddDailyQuest

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_add_daily_quest.hpp"
#include "../../Projeto IOCP/UTIL/iff.h"

using namespace stdA;

CmdAddDailyQuest::CmdAddDailyQuest(bool _waiter) : pangya_db(_waiter), m_uid(0), m_adqu{0}, m_id(-1) {
}

CmdAddDailyQuest::CmdAddDailyQuest(uint32_t _uid, bool _waiter) : pangya_db(_waiter), m_uid(_uid), m_adqu{0}, m_id(-1) {
}

CmdAddDailyQuest::CmdAddDailyQuest(uint32_t _uid, AddDailyQuestUser& _adqu, bool _waiter) : pangya_db(_waiter), m_uid(_uid), m_adqu(_adqu), m_id(-1) {
}

CmdAddDailyQuest::~CmdAddDailyQuest() {
}

void CmdAddDailyQuest::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(1, _result->cols);

	m_id = IFNULL(atoi, _result->data[0]);
}

response* CmdAddDailyQuest::prepareConsulta(database& _db) {

	if (m_adqu._typeid == 0)
		throw exception("[CmdAddDailyQuest::prepareConsulta][Error] quest typeis is invalid.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PANGYA_DB, 4, 0));

	IFF::QuestStuff *qs = nullptr;
	uint32_t CounterItem_typeid = 0u;

	CounterItem_typeid = 0u;

	if ((qs = sIff::getInstance().findQuestStuff(m_adqu._typeid)) != nullptr)
		CounterItem_typeid = qs->counter_item._typeid[0];
	
	// Clear ID DAILY QUEST ADD
	m_id = -1;

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid) + ", " + _db.makeText(m_adqu.name) 
				+ ", " + std::to_string(m_adqu._typeid)+ ", " + std::to_string(m_adqu.quest_typeid)
				+ ", " + std::to_string(CounterItem_typeid) + ", 1, " + std::to_string(m_adqu.status));

	checkResponse(r, "nao conseguiu adicionar Daily Quest[name=" + std::string(m_adqu.name) + ", typeid=" + std::to_string(m_adqu._typeid) 
			+ ", quest_typeid=" + std::to_string(m_adqu.quest_typeid) + ", counteritem_typeid=" + std::to_string(CounterItem_typeid) 
			+ ", option=1, status=" + std::to_string(m_adqu.status) + "] para o player: " + std::to_string(m_uid));

	return r;
}

int32_t CmdAddDailyQuest::getID() {
	return m_id;
}

uint32_t CmdAddDailyQuest::getUID() {
	return m_uid;
}

void CmdAddDailyQuest::setUID(uint32_t _uid) {
	m_uid = _uid;
}

AddDailyQuestUser& CmdAddDailyQuest::getAddDailyQuest() {
	return m_adqu;
}

void CmdAddDailyQuest::setAddDailyQuest(AddDailyQuestUser& _adqu) {
	m_adqu = _adqu;
}
