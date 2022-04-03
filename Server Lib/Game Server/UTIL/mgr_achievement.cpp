// Arquivo mgr_achievement.cpp
// Criado em 08/04/2018 as 23:47 por Acrisio
// Implementação da classe MgrAchievement

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "mgr_achievement.hpp"
#include "../PANGYA_DB/cmd_create_achievement.hpp"
#include "../PANGYA_DB/cmd_create_quest.hpp"
#include "../../Projeto IOCP/UTIL/iff.h"

#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"

#include "../PANGYA_DB/cmd_achievement_info.hpp"
#include "../PANGYA_DB/cmd_check_achievement.hpp"
#include "../PANGYA_DB/cmd_delete_counter_item.hpp"
#include "../PANGYA_DB/cmd_delete_quest.hpp"
#include "../PANGYA_DB/cmd_delete_achievement.hpp"

#include "../PANGYA_DB/cmd_update_counter_item.hpp"
#include "../PANGYA_DB/cmd_update_quest_user.hpp"
#include "../PANGYA_DB/cmd_update_achievement_user.hpp"

#include "../PACKET/packet_func_sv.h"
#include "../../Projeto IOCP/PACKET/packet.h"

#include "../../Projeto IOCP/DATABASE/normal_manager_db.hpp"

#include "../../Projeto IOCP/UTIL/string_util.hpp"

#include <algorithm>

using namespace stdA;

#define CHECK_SESSION(_proc_name, _session) { \
	if (!(_session).isConnected() || !(_session).getState()) \
		throw exception("[MgrAchievement::" + std::string((_proc_name)) +"][Error] session is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MGR_ACHIEVEMENT, 1, 0)); \
} \

#define CHECK_STATE(_proc_name) { \
	if (!m_state) \
		throw exception("[MgrAchievement::" + std::string((_proc_name)) + "][Error] Manager Achievement state is invalid, please call method initAchievement first.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MGR_ACHIEVEMENT, 1000, 0)); \
} \

MgrAchievement::MgrAchievement() : m_uid(0u), map_ai(), m_state(false), m_pontos(0u) {
}

MgrAchievement::~MgrAchievement() {
	clear();
}

void MgrAchievement::clear() {

	if (!map_ai.empty())
		map_ai.clear();

	m_state = false;
	m_pontos = 0u;
}

void MgrAchievement::initAchievement(uint32_t _uid, bool _create) {

	if (_uid == 0u)
		throw exception("[MgrAchievement::initAchievement][Error] _uid is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MGR_ACHIEVEMENT, 2000, 0));

	m_uid = _uid;

	initAchievement(_create);
}

void MgrAchievement::initAchievement(uint32_t _uid, std::multimap< uint32_t, AchievementInfoEx >& _mp_achievement) {

	if (_uid == 0u)
		throw exception("[MgrAchievement::initAchievement][Error] _uid is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MGR_ACHIEVEMENT, 2000, 1));

	m_uid = _uid;

	for (auto& el : _mp_achievement)
		addAchievement(el.second);	// Add Achievement

	m_state = true;
}

void MgrAchievement::initAchievement(bool _create) {

	if (!_create) {	// Verifica se o player tem Achievement, antes de criar

		CmdCheckAchievement cmd_cAchieve(m_uid, true);	// waitable

		snmdb::NormalManagerDB::getInstance().add(0, &cmd_cAchieve, nullptr, nullptr);

		cmd_cAchieve.waitEvent();

		if (cmd_cAchieve.getException().getCodeError() != 0) {
			m_state = false;
			throw cmd_cAchieve.getException();
		}

		// Cria Achievements do player
		if (!cmd_cAchieve.getLastState()) {

			auto qi = sIff::getInstance().findQuestItem(CLEAR_10_DAILY_QUEST_TYPEID);	// Daily Quest 10 dias Clear

			AchievementInfoEx ai{};

			if (qi != nullptr)
				addAchievement((ai = MgrAchievement::createAchievement(m_uid, *qi, AchievementInfo::ACTIVED)));

			auto& achievement = sIff::getInstance().getAchievement();

			for (auto it = achievement.begin(); it != achievement.end(); ++it)
				addAchievement((ai = MgrAchievement::createAchievement(m_uid, it->second, AchievementInfo::ACTIVED)));

			_smp::message_pool::getInstance().push(new message("[MgrAchievement::initAchievement][Log] player[UID=" + std::to_string(m_uid) + "] Criou Achievement.", CL_FILE_LOG_AND_CONSOLE));

		}else {

			CmdAchievementInfo cmd_ai(m_uid, true);	// waitable

			snmdb::NormalManagerDB::getInstance().add(0, &cmd_ai, nullptr, nullptr);

			cmd_ai.waitEvent();

			if (cmd_ai.getException().getCodeError() != 0) {
				m_state = false;
				throw cmd_ai.getException();
			}

			for (auto& el : cmd_ai.getInfo())
				addAchievement(el.second);	// Add Achievement
		}

	}else {	// Create Achievement

		// Cria Achievements do player
		auto qi = sIff::getInstance().findQuestItem(CLEAR_10_DAILY_QUEST_TYPEID);	// Daily Quest 10 dias Clear

		AchievementInfoEx ai{};

		if (qi != nullptr)
			addAchievement((ai = MgrAchievement::createAchievement(m_uid, *qi, AchievementInfo::ACTIVED)));

		auto& achievement = sIff::getInstance().getAchievement();

		for (auto it = achievement.begin(); it != achievement.end(); ++it)
			addAchievement((ai = MgrAchievement::createAchievement(m_uid, it->second, AchievementInfo::ACTIVED)));

		_smp::message_pool::getInstance().push(new message("[MgrAchievement::initAchievement][Log] player[UID=" + std::to_string(m_uid) + "] Criou Achievement.", CL_FILE_LOG_AND_CONSOLE));

	}

	m_state = true;
}

std::vector< CounterItemInfo > MgrAchievement::getCounterItemInfo() {
	CHECK_STATE("getCounterItemInfo");

	std::vector< CounterItemInfo > v_cii;

	std::for_each(map_ai.begin(), map_ai.end(), [&](auto& el) {
		std::for_each(el.second.map_counter_item.begin(), el.second.map_counter_item.end(), [&](auto& el2) {
			v_cii.push_back(el2.second);
		});
	});

	return v_cii;
}

std::multimap< uint32_t, AchievementInfoEx >& MgrAchievement::getAchievementInfo() {
	CHECK_STATE("getAchievementInfo");

	return map_ai;
}

uint32_t MgrAchievement::getPontos() {
	CHECK_STATE("getPontos");

	return m_pontos;
}

void MgrAchievement::resetAchievement(int32_t _id) {
	CHECK_STATE("resetAchievement");

	if (_id <= 0)
		throw exception("[MgrAchievement::resetAchievement][Error] invalid achievement[ID=" + std::to_string(_id) + "]", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MGR_ACHIEVEMENT, 1600, 0));

	resetAchievement(findAchievementById(_id));
}

void MgrAchievement::resetAchievement(std::multimap<uint32_t, AchievementInfoEx>::iterator _it) {
	CHECK_STATE("resetAchievement");

	if (_it == map_ai.end())
		throw exception("[MgrAchievement::resetAchievement][Error] iterator achievement invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MGR_ACHIEVEMENT, 1601, 0));


	// Zera o(s) Counter Item(ns)
	if (!_it->second.map_counter_item.empty()) {
		
		for (auto& el : _it->second.map_counter_item) {
			
			el.second.value = 0l;

			snmdb::NormalManagerDB::getInstance().add(4, new CmdUpdateCounterItem(m_uid, el.second), MgrAchievement::SQLDBResponse, this);
		}
	}

	// Zera o clear date unix da(s) Quest(s)
	if (!_it->second.v_qsi.empty()) {

		for (auto& el : _it->second.v_qsi) {

			el.clear_date_unix = 0u;

			snmdb::NormalManagerDB::getInstance().add(5, new CmdUpdateQuestUser(m_uid, el), MgrAchievement::SQLDBResponse, this);
		}
	}

	// Atualiza o Achievement
	_it->second.status = AchievementInfo::ACHIEVEMENT_STATUS::ACTIVED;

	snmdb::NormalManagerDB::getInstance().add(6, new CmdUpdateAchievementUser(m_uid, _it->second), MgrAchievement::SQLDBResponse, this);

	// Log
	_smp::message_pool::getInstance().push(new message("[MgrAchievement::resetAchievement][Log] player[UID=" + std::to_string(m_uid) + "] Resetou Achievement[TYPEID=" 
			+ std::to_string(_it->second._typeid) + ", ID=" + std::to_string(_it->second.id) + "] para os valores iniciais com sucesso", CL_FILE_LOG_AND_CONSOLE));
}

void MgrAchievement::removeAchievement(int32_t _id) {
	CHECK_STATE("removeAchievement");

	if (_id <= 0)
		throw exception("[MgrAchievement::removeAchievement][Error] invalid achievement[ID=" + std::to_string(_id) + "]", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MGR_ACHIEVEMENT, 10, 0));

	removeAchievement(findAchievementById(_id));
}

void MgrAchievement::removeAchievement(std::multimap< uint32_t, AchievementInfoEx >::iterator _it) {
	CHECK_STATE("removeAchievement");

	if (_it == map_ai.end())
		throw exception("[MgrAchievement::removeAchievement][Error] iterator achievement invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MGR_ACHIEVEMENT, 12, 0));

	// Delete Counter Item
	if (!_it->second.map_counter_item.empty())
		snmdb::NormalManagerDB::getInstance().add(1, new CmdDeleteCounterItem(m_uid, _it->second.map_counter_item), MgrAchievement::SQLDBResponse, this);

	// Delete Quest
	if (!_it->second.v_qsi.empty())
		snmdb::NormalManagerDB::getInstance().add(2, new CmdDeleteQuest(m_uid, _it->second.v_qsi), MgrAchievement::SQLDBResponse, this);

	// Delete Achievement
	snmdb::NormalManagerDB::getInstance().add(3, new CmdDeleteAchievement(m_uid, _it->second.id), MgrAchievement::SQLDBResponse, this);

	auto id = _it->second.id;

	map_ai.erase(_it);

	// Log
	_smp::message_pool::getInstance().push(new message("[MgrAchievement::removeAchievement][Log] player[UID=" + std::to_string(m_uid) + "] deletou Achievement[ID=" + std::to_string(id) + "]", CL_FILE_LOG_AND_CONSOLE));
}

// Method Not Necessary CHECK_STATE because it is part of initialization of class
std::multimap< uint32_t, AchievementInfoEx >::iterator MgrAchievement::addAchievement(AchievementInfoEx& _ai) {
	
	auto it_new = map_ai.insert(std::make_pair(_ai._typeid, _ai));

	if (it_new == map_ai.end())
		throw exception("[MgrAchievement::addAchievement][Error] nao conseguiu inserir o achievement no multimap", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MGR_ACHIEVEMENT, 11, 0));

	IFF::QuestStuff *qsi = nullptr;

	// Add Pontos Achievement Quest Concluído
	if (sIff::getInstance().getItemGroupIdentify(_ai._typeid) == iff::ACHIEVEMENT && _ai.status == AchievementInfo::ACTIVED || _ai.status == AchievementInfo::CONCLUEDED) {
		
		for (auto& el : _ai.v_qsi) {

			if (el.clear_date_unix != 0 && (qsi = sIff::getInstance().findQuestStuff(el._typeid)) != nullptr) {

				for (auto i = 0u; i < (sizeof(qsi->reward_item._typeid) / sizeof(qsi->reward_item._typeid[0])); ++i)

					if (qsi->reward_item._typeid[i] != 0 && qsi->reward_item._typeid[i] == 0x6C000001/*Achievement Pontos*/)
						m_pontos += qsi->reward_item.qntd[i];
			}
		}
	}

	return it_new;
}

void MgrAchievement::sendAchievementGuiToPlayer(session& _session) {
	CHECK_STATE("sendAchievementGuiToPlayer");

	CHECK_SESSION("sendAchievementGuiToPlayer", _session);

	// Tem que passar o 22D com os dados, e 22C para mostrar o GUI

	std::multimap< uint32_t, AchievementInfoEx > v_element;

	std::for_each(map_ai.begin(), map_ai.end(), [&](auto& el) {
		if (sIff::getInstance().getItemGroupIdentify(el.second._typeid) == iff::ACHIEVEMENT)
			v_element.insert(std::make_pair(el.second._typeid, el.second));
	});

	auto elements = v_element.size();
	auto map_i = v_element.begin();

	packet p;
	CounterItemInfo *cii = nullptr;

	size_t por_packet = ((MAX_BUFFER_PACKET - 100) > (17 + 4 + (16 * 10)) ? (MAX_BUFFER_PACKET - 100) / ((17 + 4) + (16 * 10)) : 1);
	size_t i = 0, index = 0, total = elements;

	for (; map_i != v_element.end(); total -= por_packet) {
		p.init_plain((unsigned short)0x22D);

		p.addInt32(0);	// SUCCESS

		p.addInt32((uint32_t)total);
		p.addInt32((uint32_t)((total > por_packet) ? por_packet : total));

		for (i = 0; i < por_packet && map_i != v_element.end(); i++, map_i++) {
			p.addInt32(map_i->second._typeid);
			p.addInt32(map_i->second.id);
			p.addInt32((uint32_t)map_i->second.v_qsi.size());

			for (auto ii = map_i->second.v_qsi.begin(); ii != map_i->second.v_qsi.end(); ii++) {
				p.addInt32(ii->_typeid);

				if (ii->counter_item_id > 0 && (cii = map_i->second.findCounterItemById(ii->counter_item_id)) != nullptr)
					p.addInt32(cii->value);
				else // não tem o counter id e nem o typeid
					p.addZeroByte(4);

				p.addInt32(ii->clear_date_unix);
			}
		}

		packet_func::session_send(p, &_session, 1);

	}	// FIM MAKE_SPLIT_PACKET

	p.init_plain((unsigned short)0x22C);

	p.addInt32(0);	// SUCCESS

	packet_func::session_send(p, &_session, 1);
}

void MgrAchievement::sendAchievementToPlayer(session& _session) {
	CHECK_STATE("sendAchievementToPlayer");

	CHECK_SESSION("", _session);

	auto elements = map_ai.size();
	auto map_i = map_ai.begin();

	packet p;
	CounterItemInfo *cii = nullptr;

	size_t por_packet = ((MAX_BUFFER_PACKET - 100) > (17 + 4 + (16 * 10))) ? (MAX_BUFFER_PACKET - 100) / ((17 + 4) + (16 * 10)) : 1;	// por pacote
	size_t i = 0, index = 0, total = elements;

	for (; map_i != map_ai.end(); total -= por_packet) {
		p.init_plain((unsigned short)0x21E);

		p.addInt32(0);	// SUCCESS

		p.addInt32((uint32_t)total);
		p.addInt32((uint32_t)((total > por_packet) ? por_packet : total));

		for (i = 0; i < por_packet && map_i != map_ai.end(); i++, map_i++) {
			p.addInt8(map_i->second.active);
			p.addInt32(map_i->second._typeid);
			p.addInt32(map_i->second.id);
			p.addInt32(map_i->second.status);
			p.addInt32((uint32_t)map_i->second.v_qsi.size());

			for (auto ii = map_i->second.v_qsi.begin(); ii != map_i->second.v_qsi.end(); ii++) {
				p.addInt32(ii->_typeid);

				if (ii->counter_item_id > 0 && (cii = map_i->second.findCounterItemById(ii->counter_item_id)) != nullptr) {
					p.addInt32(cii->_typeid);
					p.addInt32(cii->id);
				}else // não tem o counter id e nem o typeid
					p.addZeroByte(8);

				p.addInt32(ii->clear_date_unix);
			}
		}

		packet_func::session_send(p, &_session, 0);
	}
}

void MgrAchievement::sendCounterItemToPlayer(session& _session) {
	CHECK_STATE("sendCounterItemToPlayer");

	CHECK_SESSION("", _session);

	auto v_element = getCounterItemInfo();
	auto elements = v_element.size();

	packet p;

	MAKE_BEGIN_SPLIT_PACKET(0x21D, _session, sizeof(CounterItemInfo), MAX_BUFFER_PACKET);

	p.addInt32(0);	// SUCCESS

	MAKE_MED_SPLIT_PACKET(1);

	MAKE_MID_SPLIT_PACKET_VECTOR(sizeof(CounterItemInfo));

	packet_func::session_send(p, &_session, 0);

	}	// FIM MAKE_SPLIT_PACKET --- // Fim do primeiro FOR, então estou usando o end desse MACRO por que ele colocar no VECTOR no final
}

void MgrAchievement::incrementPoint(uint32_t _increase) {
	CHECK_STATE("incrementPoint");

	m_pontos += _increase;
}

CounterItemInfo* MgrAchievement::findCounterItemById(int32_t _id) {
	CHECK_STATE("findCounterItemById");

	if (_id <= 0)
		throw exception("[MgrAchievement::findCounterItemById][Error] _id is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MGR_ACHIEVEMENT, 13, 0));

	CounterItemInfo *cii = nullptr;

	for (auto& el : map_ai) {
		if ((cii = el.second.findCounterItemById(_id)) != nullptr)
			return cii;
	}

	return cii;
}

CounterItemInfo* MgrAchievement::findCounterItemByTypeid(uint32_t _typeid) {
	CHECK_STATE("findCounterItemByTypeid");
	
	if (_typeid == 0)
		throw exception("[MgrAchievement::findCounterItemByTypeid][Error] _typeid is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MGR_ACHIEVEMENT, 13, 0));

	CounterItemInfo *cii = nullptr;

	for (auto& el : map_ai) {
		if ((cii = el.second.findCounterItemByTypeId(_typeid)) != nullptr)
			return cii;
	}
	
	return cii;
}

QuestStuffInfo* MgrAchievement::findQuestStuffById(int32_t _id) {
	CHECK_STATE("findQuestStuffById");
	
	if (_id <= 0)
		throw exception("[MgrAchievement::findQuestStuffById][Error] _id is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MGR_ACHIEVEMENT, 13, 0));

	QuestStuffInfo *qsi = nullptr;

	for (auto& el : map_ai) {
		if ((qsi = el.second.findQuestStuffById(_id)) != nullptr)
			return qsi;
	}

	return qsi;
}

QuestStuffInfo* MgrAchievement::findQuestStuffByTypeId(uint32_t _typeid) {
	CHECK_STATE("findQuestStuffByTypeid");

	if (_typeid == 0)
		throw exception("[MgrAchievement::findQuestStuffByTypeId][Error] _typeid is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MGR_ACHIEVEMENT, 13, 0));

	QuestStuffInfo *qsi = nullptr;

	for (auto& el : map_ai) {
		if ((qsi = el.second.findQuestStuffByTypeId(_typeid)) != nullptr)
			return qsi;
	}

	return qsi;
}

std::multimap< uint32_t, AchievementInfoEx >::iterator MgrAchievement::findAchievementById(int32_t _id) {
	CHECK_STATE("findAchievementById");
	
	if (_id <= 0)
		throw exception("[MgrAchievement::findAchievementById][Error] _id is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MGR_ACHIEVEMENT, 13, 0));
	
	return VECTOR_FIND_ITEM(map_ai, second.id, == , _id);
}

// Static Method
AchievementInfoEx MgrAchievement::createAchievement(uint32_t _uid, IFF::Achievement& _achievement, AchievementInfo::ACHIEVEMENT_STATUS _status) {

    if (_achievement._typeid == 0)
        throw exception("[MgrAchievement::createAchievement(IFF::Achievement)][Error] achievement invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MGR_ACHIEVEMENT, 1, 0));

	AchievementInfoEx ai{};
	QuestStuffInfo qsi{};
	CounterItemInfo cii{ 0 };

	cii.active = 1;

	ai.active = 1;
	ai.status = _status;
	ai._typeid = _achievement._typeid;
	ai.quest_base_typeid = _achievement.typeid_quest_index;

    CmdCreateQuest cmd_cq(_uid, true);			// Waitable
    CmdCreateAchievement cmd_ca(_uid, true);	// Waitable
    
    IFF::QuestStuff *qs = nullptr;
    
    if (_achievement.quest_typeid[0] > 0) {
        //CmdCreateQuest cmd_cq(pi->uid, *qi, 3/*1 pendente, 2 excluida, 3 ativa, 4 concluida*/);
		auto name = std::string(_achievement.name);

        cmd_ca.setAchievement(_achievement._typeid, name, _status);

		snmdb::NormalManagerDB::getInstance().add(0, &cmd_ca, nullptr, nullptr);

		cmd_ca.waitEvent();

        if (cmd_ca.getException().getCodeError() == 0 && (ai.id = cmd_ca.getID()) != -1) {
            
            auto i = 0u;

            cmd_cq.setAchievementID(ai.id);

            do {
                if (_achievement.quest_typeid[i] != 0 && (qs = sIff::getInstance().findQuestStuff(_achievement.quest_typeid[i])) != nullptr) {
                    
					qsi.clear();

					qsi._typeid = _achievement.quest_typeid[i];
					cii._typeid = qs->counter_item._typeid[0];

                    cmd_cq.setQuest(*qs, (_achievement.typeid_quest_index == 0 || _achievement.typeid_quest_index == _achievement.quest_typeid[i]) ? true : false);

					snmdb::NormalManagerDB::getInstance().add(0, &cmd_cq, nullptr, nullptr);

					cmd_cq.waitEvent();

					qsi.id = cmd_cq.getID();
					cii.id = cmd_cq.getCounterItemID();

					if (cmd_cq.getException().getCodeError() != 0 || qsi.id == -1 || (cmd_cq.getIncludeCounter() && cii.id == 0))
                        throw exception("[MgrAchievement::createAchievement(IFF::Achievement)][Error] nao conseguiu criar quest para o player: " + std::to_string(_uid), STDA_MAKE_ERROR(STDA_ERROR_TYPE::MGR_ACHIEVEMENT, 3, 0));

					// Add o Counter Item Id para o Quest Stuff Info, se a quest tem o counter item
					if (cmd_cq.getIncludeCounter())
						qsi.counter_item_id = cii.id;

					cii.value = 0;

					ai.v_qsi.push_back(qsi);

					if (cii.id > 0)
						ai.map_counter_item[cii.id] = cii;
                }
            } while (++i < (sizeof(_achievement.quest_typeid) / sizeof(_achievement.quest_typeid[0])));

			// Atualiza os counter item id nas quest stuff se o achievement esta com o quest base
			auto it = ai.getQuestBase();

			if (it != ai.v_qsi.end()) {
				
				for (auto& el : ai.v_qsi) {
					
					// Update
					if (el.counter_item_id == 0l)
						el.counter_item_id = it->counter_item_id;
				}
			}
				
        }else
            throw exception("[MgrAchievement::createAchievement(IFF::Achievement)][Error] nao conseguiu criar achievement para o player: " + std::to_string(_uid), STDA_MAKE_ERROR(STDA_ERROR_TYPE::MGR_ACHIEVEMENT, 2, 0));
    }else
        throw exception("[MgrAchievement::createAchievement(IFF::Achievement)][Error] not have quest on achievement, achievement invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MGR_ACHIEVEMENT, 1, 1));

	return ai;
}

// Static Method
AchievementInfoEx MgrAchievement::createAchievement(uint32_t _uid, IFF::QuestItem& _qi, AchievementInfo::ACHIEVEMENT_STATUS _status) {
    
	if (_qi._typeid == 0)
		throw exception("[MgrAchievement::createAchievement(IFF::QuestItem)][Error] QuestItem invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MGR_ACHIEVEMENT, 1, 0));

	AchievementInfoEx ai{};
	QuestStuffInfo qsi{};
	CounterItemInfo cii{ 0 };

	cii.active = 1;

	ai.active = 1;
	ai.status = _status;
	ai._typeid = _qi._typeid;

	CmdCreateQuest cmd_cq(_uid, true);			// Waitable
    CmdCreateAchievement cmd_ca(_uid, true);	// Waitable
    
    IFF::QuestStuff *qs = nullptr;

    if (_qi.quest.qntd > 0 || _qi.quest._typeid[0] > 0) {
        //CmdCreateQuest cmd_cq(pi->uid, *qi, 3/*1 pendente, 2 excluida, 3 ativa, 4 concluida*/);
		auto name = std::string(_qi.name);

		cmd_ca.setAchievement(_qi._typeid, name, _status);

		snmdb::NormalManagerDB::getInstance().add(0, &cmd_ca, nullptr, nullptr);

		cmd_ca.waitEvent();

        if (cmd_ca.getException().getCodeError() == 0 && (ai.id = cmd_ca.getID()) != -1) {
            
            auto i = 0u;

            cmd_cq.setAchievementID(ai.id);

            do {
				if (_qi.quest._typeid[i] != 0 && (qs = sIff::getInstance().findQuestStuff(_qi.quest._typeid[i])) != nullptr) {
                    
					qsi.clear();
					
					qsi._typeid = _qi.quest._typeid[i];
					cii._typeid = qs->counter_item._typeid[0];

                    cmd_cq.setQuest(*qs, (_status != AchievementInfo::PENDENTING) ? true : false);

					snmdb::NormalManagerDB::getInstance().add(0, &cmd_cq, nullptr, nullptr);

					cmd_cq.waitEvent();

					qsi.id = cmd_cq.getID();
					cii.id = cmd_cq.getCounterItemID();

					if (cmd_cq.getException().getCodeError() != 0 || qsi.id == -1 || (cmd_cq.getIncludeCounter() && cii.id == 0))
                        throw exception("[MgrAchievement::createAchievement(IFF::QuestItem)][Error] nao conseguiu criar quest para o player: " + std::to_string(_uid), STDA_MAKE_ERROR(STDA_ERROR_TYPE::MGR_ACHIEVEMENT, 3, 0));

					// Add o Counter Item Id para o Quest Stuff Info, se a quest tem o counter item
					if (cmd_cq.getIncludeCounter())
						qsi.counter_item_id = cii.id;

					cii.value = 0;

					ai.v_qsi.push_back(qsi);

					if (cii.id > 0)
						ai.map_counter_item[cii.id] = cii;
                }
            } while (++i < _qi.quest.qntd);

			// Atualiza os counter item id nas quest stuff se o achievement esta com o quest base
			auto it = ai.getQuestBase();

			if (it != ai.v_qsi.end()) {

				for (auto& el : ai.v_qsi) {

					// Update
					if (el.counter_item_id == 0l)
						el.counter_item_id = it->counter_item_id;
				}
			}

        }else
            throw exception("[MgrAchievement::createAchievement(IFF::QuestItem][Error] nao conseguiu criar achievement para o player: " + std::to_string(_uid), STDA_MAKE_ERROR(STDA_ERROR_TYPE::MGR_ACHIEVEMENT, 2, 0));
    }else
        throw exception("[MgrAchievement::createAchievement(IFF::QuestItem)][WARING] not have counter item on QuestItem. invalid QuestItem", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MGR_ACHIEVEMENT, 1, 1));

	return ai;
}

void MgrAchievement::SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg) {

	if (_arg == nullptr) {
		_smp::message_pool::getInstance().push(new message("[MgrAchievement::SQLDBResponse][Error] _arg is nullptr", CL_FILE_LOG_AND_CONSOLE));
		return;
	}

	// Por Hora só sai, depois faço outro tipo de tratamento se precisar
	if (_pangya_db.getException().getCodeError() != 0) {
		_smp::message_pool::getInstance().push(new message("[MgrAchievement::SQLDBResponse][Error] " + _pangya_db.getException().getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		return;
	}

	auto mgr_achievement = reinterpret_cast<MgrAchievement*>(_arg);

	switch (_msg_id) {
	case 1:	// Delete Counter Item
	{
		auto cmd_dci = reinterpret_cast< CmdDeleteCounterItem* >(&_pangya_db);

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[MgrAchievement::SQLDBResponse][Log] player[UID=" + std::to_string(cmd_dci->getUID()) + "] deletou Counter Item[ID={" 
				+ stdA::to_string< std::vector< int32_t >, std::string >(cmd_dci->getIds()) + "}] com sucesso.", CL_FILE_LOG_AND_CONSOLE));
#else
		_smp::message_pool::getInstance().push(new message("[MgrAchievement::SQLDBResponse][Log] player[UID=" + std::to_string(cmd_dci->getUID()) + "] deletou Counter Item[ID={"
				+ stdA::to_string< std::vector< int32_t >, std::string >(cmd_dci->getIds()) + "}] com sucesso.", CL_ONLY_FILE_LOG));
#endif // _DEBUG

		break;
	}
	case 2: // Delete Quest
	{
		auto cmd_dq = reinterpret_cast<CmdDeleteQuest*>(&_pangya_db);

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[MgrAchievement::SQLDBResponse][Log] player[UID=" + std::to_string(cmd_dq->getUID()) + "] deletou Quest[ID={" 
				+ stdA::to_string< std::vector< int32_t >, std::string >(cmd_dq->getIds()) + "}] com sucesso.", CL_FILE_LOG_AND_CONSOLE));
#else
		_smp::message_pool::getInstance().push(new message("[MgrAchievement::SQLDBResponse][Log] player[UID=" + std::to_string(cmd_dq->getUID()) + "] deletou Quest[ID={"
				+ stdA::to_string< std::vector< int32_t >, std::string >(cmd_dq->getIds()) + "}] com sucesso.", CL_ONLY_FILE_LOG));
#endif // _DEBUG

		break;
	}
	case 3: // Delete Achievement
	{
		auto cmd_da = reinterpret_cast< CmdDeleteAchievement* >(&_pangya_db);

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[MgrAchievement::SQLDBResponse][Log] player[UID=" + std::to_string(cmd_da->getUID()) + "] deletou Achievement[ID=" 
				+ std::to_string(cmd_da->getId()) + "] com sucesso.", CL_FILE_LOG_AND_CONSOLE));
#else
		_smp::message_pool::getInstance().push(new message("[MgrAchievement::SQLDBResponse][Log] player[UID=" + std::to_string(cmd_da->getUID()) + "] deletou Achievement[ID="
				+ std::to_string(cmd_da->getId()) + "] com sucesso.", CL_ONLY_FILE_LOG));
#endif // _DEBUG

		break;
	}
	case 4:	// Update Counter Item
	{
		auto cmd_uci = reinterpret_cast< CmdUpdateCounterItem* >(&_pangya_db);

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[MgrAchievement::SQLDBResponse][Log] player[UID=" + std::to_string(cmd_uci->getUID()) + "] Atualizou Counter Item[TYPEID=" 
				+ std::to_string(cmd_uci->getInfo()._typeid) + ", ID=" + std::to_string(cmd_uci->getInfo().id) + "] com sucesso.", CL_FILE_LOG_AND_CONSOLE));
#else
		_smp::message_pool::getInstance().push(new message("[MgrAchievement::SQLDBResponse][Log] player[UID=" + std::to_string(cmd_uci->getUID()) + "] Atualizou Counter Item[TYPEID="
				+ std::to_string(cmd_uci->getInfo()._typeid) + ", ID=" + std::to_string(cmd_uci->getInfo().id) + "] com sucesso.", CL_ONLY_FILE_LOG));
#endif // _DEBUG

		break;
	}
	case 5:	// Update Quest User
	{
		auto cmd_uqu = reinterpret_cast<CmdUpdateQuestUser*>(&_pangya_db);

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[MgrAchievement::SQLDBResponse][Log] player[UID=" + std::to_string(cmd_uqu->getUID()) + "] Atualizou Quest[TYPEID="
				+ std::to_string(cmd_uqu->getInfo()._typeid) + ", ID=" + std::to_string(cmd_uqu->getInfo().id) + "] com sucesso.", CL_FILE_LOG_AND_CONSOLE));
#else
		_smp::message_pool::getInstance().push(new message("[MgrAchievement::SQLDBResponse][Log] player[UID=" + std::to_string(cmd_uqu->getUID()) + "] Atualizou Quest[TYPEID="
				+ std::to_string(cmd_uqu->getInfo()._typeid) + ", ID=" + std::to_string(cmd_uqu->getInfo().id) + "] com sucesso.", CL_ONLY_FILE_LOG));
#endif // _DEBUG

		break;
	}
	case 6:	// Update Achievement User
	{
		auto cmd_uau = reinterpret_cast< CmdUpdateAchievementUser* >(&_pangya_db);

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[MgrAchievement::SQLDBResponse][Log] player[UID=" + std::to_string(cmd_uau->getUID()) + "] Atualizou Achievement[TYPEID="
				+ std::to_string(cmd_uau->getInfo()._typeid) + ", ID=" + std::to_string(cmd_uau->getInfo().id) + "] com sucesso.", CL_FILE_LOG_AND_CONSOLE));
#else
		_smp::message_pool::getInstance().push(new message("[MgrAchievement::SQLDBResponse][Log] player[UID=" + std::to_string(cmd_uau->getUID()) + "] Atualizou Achievement[TYPEID="
				+ std::to_string(cmd_uau->getInfo()._typeid) + ", ID=" + std::to_string(cmd_uau->getInfo().id) + "] com sucesso.", CL_ONLY_FILE_LOG));
#endif // _DEBUG

		break;
	}
	case 0:
	default:
		break;
	}
}
