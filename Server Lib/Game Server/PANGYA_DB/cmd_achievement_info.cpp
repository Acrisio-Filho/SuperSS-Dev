// Arquivo cmd_achievement_info.cpp
// Criado em 21/03/2018 as 21:56 por Acrisio
// Implementa��o da classe CmdAchievementInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "cmd_achievement_info.hpp"
#include "../../Projeto IOCP/UTIL/iff.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"

using namespace stdA;

CmdAchievementInfo::CmdAchievementInfo(bool _waiter) : pangya_db(_waiter), m_uid(0), map_ai() {
	if (!sIff::getInstance().isLoad())
		sIff::getInstance().load();
}

CmdAchievementInfo::CmdAchievementInfo(uint32_t _uid, bool _waiter) : pangya_db(_waiter), m_uid(_uid), map_ai() {
	if (!sIff::getInstance().isLoad())
		sIff::getInstance().load();
}

CmdAchievementInfo::~CmdAchievementInfo() {
}

void CmdAchievementInfo::lineResult(result_set::ctx_res* _result, uint32_t /*_index_result*/) {

	checkColumnNumber(10, (uint32_t)_result->cols);

	std::multimap< uint32_t, AchievementInfoEx >::iterator i;
	AchievementInfoEx ai{};
	QuestStuffInfo qsi{};
	CounterItemInfo cii{ 0 };

	cii.active = 1;

	ai._typeid = IFNULL(atoi, _result->data[1]);
	ai.id = IFNULL(atoi, _result->data[2]);

	IFF::Achievement *achievement = nullptr;

	qsi.id = IFNULL(atoi, _result->data[4]);
	qsi._typeid = IFNULL(atoi, _result->data[5]);
	cii._typeid = IFNULL(atoi, _result->data[6]);
	cii.id = qsi.counter_item_id = IFNULL(atoi, _result->data[7]);
	cii.value = IFNULL(atoi, _result->data[8]);
	qsi.clear_date_unix = IFNULL(atoi, _result->data[9]);

	// !@ Aqui mais para frente quando add daily quest, pode ter 1 ou mas da mesma quest
	// ou seja vai ter duplicate de typeid, aqui e da erro
	// tem que user multimap ou resolver de outro jeito
	// AJEITEI J�, s� ainda n�o suporta mais de 2 com msm ID, por que ai ele sempre vai pegar o primeiro
	if ((i = map_ai.find(ai._typeid)) == map_ai.end() || (map_ai.count(ai._typeid) == 1 && i->second.id != ai.id)) {
		ai.active = (unsigned char)IFNULL(atoi, _result->data[0]);
		ai.status = IFNULL(atoi, _result->data[3]);

		check_achievement_retorno(ai);
		check_quest_achievement(ai, qsi);

		ai.v_qsi.push_back(qsi);

		if (cii.id > 0)
			ai.map_counter_item[cii.id] = cii;

		map_ai.insert(std::make_pair(ai._typeid, ai));
	}else if (map_ai.count(ai._typeid) == 1) {

		check_quest_achievement(i->second, qsi);

		i->second.v_qsi.push_back(qsi);

		if (cii.id > 0)
			i->second.map_counter_item[cii.id] = cii;
	}else { // tem mais de 1 achievement com o mesmo typeid
		auto er = map_ai.equal_range(ai._typeid);
		auto it = er.second;

		for (it = er.first; it != er.second; ++it) {
			if (it->second.id == ai.id) {

				check_quest_achievement(it->second, qsi);

				it->second.v_qsi.push_back(qsi);

				if (cii.id > 0)
					it->second.map_counter_item[cii.id] = cii;
				break;
			}
		}

		if (it == er.second) {	// Add um novo achievement
			ai.active = (unsigned char)IFNULL(atoi, _result->data[0]);
			ai.status = IFNULL(atoi, _result->data[3]);

			check_achievement_retorno(ai);
			check_quest_achievement(ai, qsi);

			ai.v_qsi.push_back(qsi);

			if (cii.id > 0)
				ai.map_counter_item[cii.id] = cii;

			map_ai.insert(std::make_pair(ai._typeid, ai));
		}
	}
}

response* CmdAchievementInfo::prepareConsulta(database& _db) {

	map_ai.clear();

#if defined(_WIN32)
	LARGE_INTEGER li{ 0 };
	LARGE_INTEGER begin{ 0 }, end{ 0 };

	QueryPerformanceFrequency(&li);
	QueryPerformanceCounter(&begin);
#elif defined(__linux__)
	timespec li { 0 };
	timespec begin{ 0 }, end{ 0 };

	clock_getres(CLOCK_MONOTONIC_RAW, &li);
	clock_gettime(CLOCK_MONOTONIC_RAW, &begin);
#endif

	auto r = procedure(_db, m_szConsulta, std::to_string(m_uid));

#if defined(_WIN32)
	QueryPerformanceCounter(&end);
#elif defined(__linux__)
	clock_gettime(CLOCK_MONOTONIC_RAW, &end);
#endif

#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[CmdAchievementInfo][Log] Executou a Query do achievement Info do player[UID=" + std::to_string(m_uid) + "] em "
			+ std::to_string(
#if defined(_WIN32)
				(end.QuadPart - begin.QuadPart) * 1000 / li.QuadPart
#elif defined(__linux__)
				DIFF_TICK(end, begin, li)
#endif
			) + "ms", CL_FILE_LOG_AND_CONSOLE));
#else
	_smp::message_pool::getInstance().push(new message("[CmdAchievementInfo][Log] Executou a Query do achievement Info do player[UID=" + std::to_string(m_uid) + "] em "
			+ std::to_string(
#if defined(_WIN32)
				(end.QuadPart - begin.QuadPart) * 1000 / li.QuadPart
#elif defined(__linux__)
				DIFF_TICK(end, begin, li)
#endif
			) + "ms", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

	checkResponse(r, "nao conseguiu pegar o achievement info do player: " + std::to_string(m_uid));

	return r;
}

inline void CmdAchievementInfo::check_achievement_retorno(AchievementInfoEx& _ai) {

	IFF::Achievement *achievement = nullptr;

	if (sIff::getInstance().getItemGroupIdentify(_ai._typeid) != iff::QUEST_ITEM && (achievement = sIff::getInstance().findAchievement(_ai._typeid)) != nullptr) \
		_ai.quest_base_typeid = achievement->typeid_quest_index; \
	else if (sIff::getInstance().getItemGroupIdentify(_ai._typeid) == iff::ACHIEVEMENT) \
		_smp::message_pool::getInstance().push(new message("[CmdAchievementInfo::lineResult][WARNING] achievement[TypeID=" + std::to_string(_ai._typeid) + "] retornado pelo banco de dados nao existe no arquivo .iff. do player: " + std::to_string(m_uid), CL_FILE_LOG_AND_CONSOLE));

}

inline void CmdAchievementInfo::check_quest_achievement(AchievementInfoEx& _ai, QuestStuffInfo& _qsi) {

	if (_ai.status == 3/*ativo*/ && (_ai.quest_base_typeid == 0 || _qsi._typeid == _ai.quest_base_typeid) && _qsi.counter_item_id <= 0) \
		_smp::message_pool::getInstance().push(new message("[CmdAchievementInfo::lineResult][WARNING] achievement quest[TypeID=" + std::to_string(_qsi._typeid) + "] nao foi criado o counter item para ele. do player: " + std::to_string(m_uid), CL_FILE_LOG_AND_CONSOLE));

}

std::multimap< uint32_t, AchievementInfoEx >& CmdAchievementInfo::getInfo() {
	return map_ai;
}

uint32_t CmdAchievementInfo::getUID() {
	return m_uid;
}

void CmdAchievementInfo::setUID(uint32_t _uid) {
	m_uid = _uid;
}
