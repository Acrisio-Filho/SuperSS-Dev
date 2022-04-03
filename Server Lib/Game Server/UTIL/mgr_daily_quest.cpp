// Arquivo mgr_daily_quest.cpp
// Criado em 13/04/2018 as 15:16 por Acrisio
// Implementa��o da classe MgrDailyQuest

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "mgr_daily_quest.hpp"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"

#include "../PANGYA_DB/cmd_daily_quest_info_user.hpp"
#include "../PANGYA_DB/cmd_old_daily_quest_info.hpp"
#include "../PANGYA_DB/cmd_delete_daily_quest.hpp"
#include "../PANGYA_DB/cmd_update_daily_quest.hpp"
#include "../PANGYA_DB/cmd_delete_counter_item.hpp"
#include "../PANGYA_DB/cmd_add_counter_item.hpp"
#include "../PANGYA_DB/cmd_update_quest_user.hpp"
#include "../PANGYA_DB/cmd_update_achievement_user.hpp"
#include "../PANGYA_DB/cmd_update_daily_quest_user.hpp"

#include "mgr_achievement.hpp"

#include <algorithm>

#include "../../Projeto IOCP/UTIL/iff.h"

#include "../../Projeto IOCP/DATABASE/normal_manager_db.hpp"

#include "../PACKET/packet_func_sv.h"

#include "../Game Server/game_server.h"

#include "sys_achievement.hpp"

#include "../GAME/item_manager.h"

#include "../../Projeto IOCP/UTIL/random_gen.hpp"

using namespace stdA;

#define CHECK_SESSION(_func_name, __session) { \
	if (!(__session).isConnected() && !(__session).getState()) \
		throw exception("[MgrDailyQuest::" + std::string((_func_name)) + "][Error] session not connected.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MGR_DAILY_QUEST, 1, 0)); \
} \

#define BEGIN_REQUEST(_method) CHECK_SESSION((_method), _session); \
							   if (_packet == nullptr) \
									throw exception("[MgrDailyQuest::request" + std::string((_method)) +"][Error] _packet is nullptr", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MGR_DAILY_QUEST, 2, 0)); \

void MgrDailyQuest::requestCheckAndSendDailyQuest(player& _session, packet *_packet) {
	BEGIN_REQUEST("CheckAndSandDailyQuest");

	try {

		/*if (sgs::gs == nullptr)
			throw exception("[MgrDailyQuest::checkAndSendDailyQuest][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] pediu o Daily Quest, mas a variavel static global do server is invalid(nullptr). Bug", 
				STDA_MAKE_ERROR(STDA_ERROR_TYPE::MGR_DAILY_QUEST, 5000, 0));*/

		if (MgrDailyQuest::checkCurrentQuestUser(sgs::gs::getInstance().getDailyQuestInfo(), _session)) {
			// Get Old Quest do player
			auto old_quest = MgrDailyQuest::getOldQuestUser(_session);

			for (auto& el : old_quest) {
				
				_session.m_pi.mgr_achievement.removeAchievement(el.id);

				_smp::message_pool::getInstance().push(new message("[MgrDailyQuest::requetCheckAndSendDailyQuest][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] deletou Old Daily Quest[ID=" 
						+ std::to_string(el.id) + "]", CL_FILE_LOG_AND_CONSOLE));
			}

			// Add nova quest para o player
			auto v_ai = MgrDailyQuest::addQuestUser(sgs::gs::getInstance().getDailyQuestInfo(), _session);

			packet p((unsigned short)0x216);

			p.addInt32((const int)GetSystemTimeAsUnix());

			// Achievement
			if (!v_ai.empty()) {

				p.addInt32((uint32_t)v_ai.size());

				for (auto& el : v_ai) {
					p.addUint8(2);
					p.addInt32(el._typeid);
					p.addInt32(el.id);
					p.addUint32(0);		// flag
					p.addInt32(0);		// Qntd antes
					p.addInt32(1);		// Qntd depois
					p.addInt32(1);		// add value
					p.addZeroByte(25);
				}

			}else
				p.addUint32(0u);

			packet_func::session_send(p, &_session, 1);

			packet_func::pacote225(p, &_session, _session.m_pi.dqiu, old_quest);

			packet_func::session_send(p, &_session, 1);

		}else {
			packet p((unsigned short)0x216);

			p.addInt32((const int)GetSystemTimeAsUnix());
			p.addInt32(0);

			packet_func::session_send(p, &_session, 1);

			packet_func::pacote225(p, &_session, _session.m_pi.dqiu, std::vector< RemoveDailyQuestUser >());

			packet_func::session_send(p, &_session, 1);
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[MgrDailyQuest::requestCheckAndSendDailyQuest][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
}

void MgrDailyQuest::requestLeaveQuest(player& _session, packet *_packet) {
	BEGIN_REQUEST("LeaveQuest");

	packet p;

	int32_t *quest_id = nullptr;

	try {

		uint32_t num_quest = _packet->readUint32();

		if (num_quest <= 0u)
			throw exception("[MgrDailyQuest::requestLeaveQuest][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou desistir da quest, mas o numero de quest para desistir e 0. Hacker ou Bug", 
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::MGR_DAILY_QUEST, 5010, 0));

		quest_id = new int32_t[num_quest];

		_packet->readBuffer(quest_id, sizeof(int) * num_quest);

		auto v_quest = MgrDailyQuest::leaveQuestUser(_session, quest_id, num_quest);

		std::map< int32_t, CounterItemInfo > map_cii;

		for (auto& el : v_quest)
			map_cii.insert(el.map_counter_item.begin(), el.map_counter_item.end());

		p.init_plain((unsigned short)0x216);

		p.addInt32((const int)GetSystemTimeAsUnix());

		if (!map_cii.empty()) {

			p.addUint32((uint32_t)map_cii.size());

			for (auto& el : map_cii) {

				p.addUint8(2);
				p.addInt32(el.second._typeid);
				p.addInt32(el.second.id);
				p.addUint32(0);	// flag
				p.addInt32(el.second.value);	// Qntd antes
				p.addInt32(0);	// Qntd depois
				p.addInt32(el.second.value	 * -1);	// add quantos, tipo de add tinha 0(antes) + 3(qntd) = 3(depois)
				p.addZeroByte(25);
			}

		}else
			p.addInt32(0);

		packet_func::session_send(p, &_session, 1);

		packet_func::pacote228(p, &_session, v_quest);

		packet_func::session_send(p, &_session, 1);

		if (quest_id == nullptr)
			delete[] quest_id;

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[MgrDailyQuest::requestLeaveQuest][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		auto v_ai = std::vector< AchievementInfoEx >();

		packet_func::pacote228(p, &_session, v_ai, 1);

		packet_func::session_send(p, &_session, 0);

		if (quest_id != nullptr)
			delete[] quest_id;
	}
}

void MgrDailyQuest::requestAcceptQuest(player& _session, packet *_packet) {
	BEGIN_REQUEST("AcceptQuest");

	packet p;

	int32_t *quest_id = nullptr;

	try {

		uint32_t num_quest = _packet->readUint32();

		if (num_quest <= 0u)
			throw exception("[MgrDailyQuest::requestAcceptQuest][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou aceitar o Daily Quest, mas o numero de quest para aceitar is invalid(zero). Hacker ou Bug",
					STDA_MAKE_ERROR(STDA_ERROR_TYPE::MGR_DAILY_QUEST, 5000, 0));

		quest_id = new int32_t[num_quest];

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[MgrDailyQuest::requestAcceptQuest][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] pediu para aceitar " 
				+ std::to_string(num_quest) + " Quest.", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

		_packet->readBuffer(quest_id, sizeof(int) * num_quest);

		auto v_quest = MgrDailyQuest::acceptQuestUser(_session, quest_id, num_quest);

		std::map< int32_t, CounterItemInfo > map_cii;

		for (auto& el : v_quest)
			map_cii.insert(el.map_counter_item.begin(), el.map_counter_item.end());

		packet p((unsigned short)0x216);

		p.addInt32((const int)GetSystemTimeAsUnix());

		if (!map_cii.empty()) {
			p.addInt32((uint32_t)map_cii.size());

			for (auto& el : map_cii) {
				p.addUint8(2);
				p.addInt32(el.second._typeid);
				p.addInt32(el.second.id);
				p.addUint32(0);	// flag
				p.addInt32(0);	// Qntd antes
				p.addInt32(0);	// Qntd depois
				p.addInt32(0);	// add quantos, tipo de add tinha 0(antes) + 3(qntd) = 3(depois)
				p.addZeroByte(25);
			}
		}else
			p.addInt32(0);

		packet_func::session_send(p, &_session, 1);

		packet_func::pacote226(p, &_session, v_quest);
		packet_func::session_send(p, &_session, 1);

		if (quest_id != nullptr)
			delete[] quest_id;

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[MgrDailyQuest::requestAcceptQuest][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		auto v_ai = std::vector< AchievementInfoEx >();

		packet_func::pacote226(p, &_session, v_ai, 1);
		packet_func::session_send(p, &_session, 1);

		if (quest_id != nullptr)
			delete[] quest_id;
	}
}

void MgrDailyQuest::requestTakeRewardQuest(player& _session, packet *_packet) {
	BEGIN_REQUEST("TakeRewardQuest");

	packet p;

	int32_t *quest_id = nullptr;

	try {

		uint32_t num_quest = _packet->readInt32();

		if (num_quest <= 0u)
			throw exception("[MgrDailyQuest::requestTakeRewardQuest][Error] numero de quest para pegar recompensa e 0", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MGR_DAILY_QUEST, 5005, 0));

		quest_id = new int32_t[num_quest];

		_packet->readBuffer(quest_id, sizeof(int) * num_quest);

		auto v_quest = leaveQuestUser(_session, quest_id, num_quest); /*MgrDailyQuest::getRewardQuestUser(_session, quest_id, num_quest);*/

		IFF::QuestItem *qi = nullptr;

		std::vector< stItem > v_item;
		stItem item{ 0 };

		// UPADATE Achievement ON SERVER, DB and GAME
		SysAchievement sys_achievement;

		// Add Reward Item do player
		for (auto& el : v_quest) {

			// Item Reward, d� para o player
			if ((qi = sIff::getInstance().findQuestItem(el._typeid)) != nullptr) {

				for (auto i = 0u; i < (sizeof(qi->reward._typeid) / sizeof(qi->reward._typeid[0])); ++i) {
					
					if (qi->reward._typeid[i] != 0) {
						
						item.clear();

						item.type = 2;
						item.id = -1;
						item._typeid = qi->reward._typeid[i];
						item.qntd = qi->reward.qntd[i];
						item.STDA_C_ITEM_QNTD = (short)item.qntd;
						item.STDA_C_ITEM_TIME = (short)qi->reward.time[i];
						
						// Add Item no db e no player
						auto rt = item_manager::RetAddItem::T_INIT_VALUE;

						if ((rt = item_manager::addItem(item, _session, 0, 0)) < 0)
							throw exception("[MgrDailyQuest::requestTakeRewardQuest][Error] player[UID=" + std::to_string(_session.m_pi.uid) + "] tentou pegar a recompensa da Quest[TYPEID=" 
									+ std::to_string(el._typeid) + ", ID=" + std::to_string(el.id) + "], mas nao conseguiu adicionar o Item[TYPEID=" + std::to_string(item._typeid) + "]", 
									STDA_MAKE_ERROR(STDA_ERROR_TYPE::MGR_DAILY_QUEST, 1500, 0));

						if (rt != item_manager::RetAddItem::T_SUCCESS_PANG_AND_EXP_AND_CP_POUCH)
							v_item.push_back(item);

						_smp::message_pool::getInstance().push(new message("[MgrDailyQuest::requestTakeRewardQuest][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] pegou recompensa{ITEM=[TYPEID=" 
								+ std::to_string(item._typeid) + ", ID=" + std::to_string(item.id) + ", QNTD=" + std::to_string(item.qntd) + "]} da Quest[TYPEID=" 
								+ std::to_string(el._typeid) + ", ID=" + std::to_string(el.id) + "].", CL_FILE_LOG_AND_CONSOLE));
					}
				}

				// S� add o contador de clear quest pega recompensa nas quest normais, a box de 10 clear quest n�o add ao contador
				if (el._typeid != CLEAR_10_DAILY_QUEST_TYPEID)
					sys_achievement.incrementCounter(0x6C40009F/*Pega Recompessa de clear quest*/);
			}
		}

		// Add os Counter Item Excluido do player
		for (auto& el : v_quest) {

			if (!el.map_counter_item.empty()) {

				for (auto& el2 : el.map_counter_item) {
					
					item.clear();

					item.type = 2;
					item.id = el2.second.id;
					item._typeid = el2.second._typeid;
					item.qntd = el2.second.value * -1;
					item.STDA_C_ITEM_QNTD = (short)item.qntd;
					item.stat.qntd_ant = el2.second.value;
					item.stat.qntd_dep = item.stat.qntd_ant + item.qntd;

					v_item.push_back(item);
				}
			}
		}

		// Log
		_smp::message_pool::getInstance().push(new message("[MgrDailyQuest::requestTakeRewardQuest][Log] player[UID=" + std::to_string(_session.m_pi.uid) + "] Pegou recompensa da Daily Quest com sucesso.", CL_FILE_LOG_AND_CONSOLE));

		// UPDATE ON GAME
		p.init_plain((unsigned short)0x216);

		p.addInt32((const uint32_t)GetSystemTimeAsUnix());
				
		p.addUint32((uint32_t)v_item.size());	// Att 2 Item

		for (auto& el : v_item) {
			p.addUint8(el.type);
			p.addUint32(el._typeid);
			p.addInt32(el.id);	// id do item no banco de dados
			p.addUint32(el.flag_time);	// flag
			p.addBuffer(&el.stat, sizeof(el.stat));
			p.addInt32((el.STDA_C_ITEM_TIME > 0) ? el.STDA_C_ITEM_TIME : el.STDA_C_ITEM_QNTD);
			p.addZeroByte(25);
		}

		packet_func::session_send(p, &_session, 1);

		packet_func::pacote227(p, &_session, v_quest);

		packet_func::session_send(p, &_session, 1);

		// UPADATE Achievement ON SERVER, DB and GAME
		sys_achievement.finish_and_update(_session);

		if (quest_id != nullptr)
			delete[] quest_id;

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[MgrDailyQuest::requestTakeRewardQuest][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		auto v_ai = std::vector< AchievementInfoEx >();

		packet_func::pacote227(p, &_session, v_ai, (STDA_SOURCE_ERROR_DECODE(e.getCodeError()) == STDA_ERROR_TYPE::CHANNEL) ? STDA_SYSTEM_ERROR_DECODE(e.getCodeError()) : 500050);

		packet_func::session_send(p, &_session, 1);

		if (quest_id != nullptr)
			delete[] quest_id;
	}
}

bool MgrDailyQuest::checkCurrentQuestUser(DailyQuestInfo& _dqi, player& _session) {

	CHECK_SESSION("checkCurrentQuestUser", _session);
	
	auto& pi = _session.m_pi;

	if (pi.dqiu.current_date != 0) {
		
		SYSTEMTIME st2{ 0 };

		translateDateLocal(pi.dqiu.current_date, &st2);	// Current Date Daily Quest Player

		if (_dqi.date.wYear > st2.wYear || _dqi.date.wMonth > st2.wMonth || _dqi.date.wDay > st2.wDay)
			return true;

	}else
		return true;

	return false;
}

bool MgrDailyQuest::checkCurrentQuest(DailyQuestInfo& _dqi) {

	SYSTEMTIME st2{ 0 };
	
	translateDateLocal(0, &st2);	// Local System Now Date

	return (_dqi.date.wYear < st2.wYear || _dqi.date.wMonth < st2.wMonth || _dqi.date.wDay < st2.wDay);
}

std::vector< RemoveDailyQuestUser > MgrDailyQuest::getOldQuestUser(player& _session) {

	CHECK_SESSION("getOldQuestUser", _session);

	std::vector< RemoveDailyQuestUser > v_old_quest;
	auto& map_ai = _session.m_pi.mgr_achievement.getAchievementInfo();

	std::for_each(map_ai.begin(), map_ai.end(), [&](auto& el) { if (el.second.status == AchievementInfo::PENDENTING/*PENDENTING*/) v_old_quest.push_back({ el.second.id, el.second._typeid }); });

	if (v_old_quest.empty()) {	// Se n�o achou no que estava no server procura no banco de dados

		CmdOldDailyQuestInfo cmd_odqi(_session.m_pi.uid, true);

		snmdb::NormalManagerDB::getInstance().add(0, &cmd_odqi, nullptr, nullptr);

		cmd_odqi.waitEvent();

		if (cmd_odqi.getException().getCodeError() == 0)
			v_old_quest = cmd_odqi.getInfo();
	}

	return v_old_quest;
}

std::vector< AchievementInfoEx > MgrDailyQuest::addQuestUser(DailyQuestInfo& _dqi, player& _session) {

	CHECK_SESSION("addQuestUser", _session);

	std::vector< AchievementInfoEx > v_ADQU;

	IFF::QuestItem *qi = nullptr;

	AchievementInfoEx ai{};
	QuestStuffInfo qsi{};
	CounterItemInfo cii{ 0 };

	for (auto i = 0u; i < (sizeof(_dqi._typeid) / sizeof(_dqi._typeid[0])); ++i) {
		
		// Add New Quest to player
		_session.m_pi.dqiu._typeid[i] = _dqi._typeid[i];

		// Clear(Limpa) Estruturas, temporarias
		ai.clear();
		qsi.clear();
		cii.clear();

		if ((qi = sIff::getInstance().findQuestItem(_dqi._typeid[i])) != nullptr && qi->quest.qntd > 0) {

			ai = MgrAchievement::createAchievement(_session.m_pi.uid, *qi, AchievementInfo::PENDENTING);

			auto it_ai = _session.m_pi.mgr_achievement.addAchievement(ai);

			_smp::message_pool::getInstance().push(new message("[MgrDailyQuest::addQuestUser][Log] Adicionou Quest[TYPEID=" + std::to_string(ai._typeid) + ", ID=" 
					+ std::to_string(ai.id) + "] para o player: " + std::to_string(_session.m_pi.uid), CL_FILE_LOG_AND_CONSOLE));

			v_ADQU.push_back(it_ai->second);
		}
	}

	// Seta no banco de dados a data que o player add a nova quest
	_session.m_pi.dqiu.current_date = (uint32_t)GetLocalTimeAsUnix();

	snmdb::NormalManagerDB::getInstance().add(0, new CmdUpdateDailyQuestUser(_session.m_pi.uid, _session.m_pi.dqiu), MgrDailyQuest::SQLDBResponse, nullptr);

	return v_ADQU;
}

void MgrDailyQuest::removeQuestUser(player& _session, std::vector< RemoveDailyQuestUser >& _v_el) {

	CHECK_SESSION("removeQuestUser", _session);

	if (!_v_el.empty())
		snmdb::NormalManagerDB::getInstance().add(0, new CmdDeleteDailyQuest(_session.m_pi.uid, _v_el), MgrDailyQuest::SQLDBResponse, nullptr);
}

std::vector< AchievementInfoEx > MgrDailyQuest::leaveQuestUser(player& _session, int32_t* _quest_id, uint32_t _count) {

	CHECK_SESSION("leaveQuestUser", _session);

	if (_quest_id == nullptr)
		throw exception("[MgrDailyQuest::leaveQuestUser][Error] _quest_id is nullptr", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MGR_DAILY_QUEST, 2, 0));

	if (_count == 0)
		throw exception("[MgrDailyQuest::leaveQuestUser][Error] _count is zero", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MGR_DAILY_QUEST, 3, 0));

	std::vector< AchievementInfoEx > v_ai;
	std::map< uint32_t, AchievementInfoEx >::iterator it;

	int32_t id = -1;
	uint32_t _typeid = 0u;

	for (auto i = 0u; i < _count; ++i) {
		if ((it = _session.m_pi.mgr_achievement.findAchievementById(_quest_id[i])) != _session.m_pi.mgr_achievement.getAchievementInfo().end()) {
			
			v_ai.push_back(it->second);

			id = it->second.id;
			_typeid = it->second._typeid;

			// Verifica se � o daily quest 10 Clear, se for Atualizar ela(Resta para os valores iniciais)
			if (_typeid == CLEAR_10_DAILY_QUEST_TYPEID) {

				// Reseta Achievement
				_session.m_pi.mgr_achievement.resetAchievement(it);

				_smp::message_pool::getInstance().push(new message("[MgrDailyQuest::leaveQuestUser][Log] Player[UID=" + std::to_string(_session.m_pi.uid) + "] Resetou a Quest 10 Clear[TYPEID=" 
						+ std::to_string(it->second._typeid) + ", ID=" + std::to_string(it->second.id) + "]", CL_FILE_LOG_AND_CONSOLE));

			}else {

				// Delete from achievement
				_session.m_pi.mgr_achievement.removeAchievement(it);

				_smp::message_pool::getInstance().push(new message("[MgrDailyQuest::leaveQuestUser][Log] Player[UID=" + std::to_string(_session.m_pi.uid) + "] Deletou da Quest[TYPEID=" 
						+ std::to_string(_typeid) + ", ID=" + std::to_string(id) + "]", CL_FILE_LOG_AND_CONSOLE));
			}
		}
	}

	return v_ai;
}

std::vector< AchievementInfoEx > MgrDailyQuest::acceptQuestUser(player& _session, int32_t* _quest_id, uint32_t _count) {
	
	CHECK_SESSION("acceptQuestUser", _session);

	if (_quest_id == nullptr)
		throw exception("[MgrDailyQuest::acceptQuestUser][Error] _quest_id is nullptr", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MGR_DAILY_QUEST, 2, 0));

	if (_count == 0)
		throw exception("[MgrDailyQuest::acceptQuestUser][Error] _count is zero", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MGR_DAILY_QUEST, 3, 0));

	std::vector< AchievementInfoEx > v_ai;
	std::map< uint32_t, AchievementInfoEx >::iterator it;
	IFF::QuestStuff *qs = nullptr;
	CounterItemInfo cii{ 0 };

	auto& map_ai = _session.m_pi.mgr_achievement.getAchievementInfo();

	for (auto i = 0u; i < _count; ++i) {
		if ((it = _session.m_pi.mgr_achievement.findAchievementById(_quest_id[i])) != map_ai.end()) {

			// Add Counter Item
			for (auto& el : it->second.v_qsi) {

				cii.clear();
				cii.active = 1;
				
				if ((qs = sIff::getInstance().findQuestStuff(el._typeid)) != nullptr)
					cii._typeid = qs->counter_item._typeid[0];
				else
					throw exception("[MgrDailyQuest::acceptQuestUser][Error] nao encontrou o quest stuff[typeid=" + std::to_string(el._typeid) + "] no IFF QuestStuff, para o player: " + std::to_string(_session.m_pi.uid),
										STDA_MAKE_ERROR(STDA_ERROR_TYPE::MGR_DAILY_QUEST, 6, 0));

				if ((el.counter_item_id = addCounterItemUser(_session, cii)) == -1)
					throw exception("[MgrDailyQuest::acceptQuestUser][Error] nao conseguiu adicionar o counter item[TYPEID=" + std::to_string(cii._typeid) + "] no banco de dados para o player: " + std::to_string(_session.m_pi.uid), 
										STDA_MAKE_ERROR(STDA_ERROR_TYPE::MGR_DAILY_QUEST, 7, 0));

				// Add Counter To Counter Item Map of Achievement
				it->second.map_counter_item.insert(std::make_pair(cii.id, cii));

				// Atualiza o counter id da quest no banco de dados
				snmdb::NormalManagerDB::getInstance().add(0, new CmdUpdateQuestUser(_session.m_pi.uid, el), MgrDailyQuest::SQLDBResponse, nullptr);
			}
			
			// Update Achievement, Status Active == 3
			it->second.status = 3;

			snmdb::NormalManagerDB::getInstance().add(0, new CmdUpdateAchievementUser(_session.m_pi.uid, it->second), MgrDailyQuest::SQLDBResponse, nullptr);

			_session.m_pi.dqiu.accept_date = (uint32_t)GetLocalTimeAsUnix();

			// Update Last Quest Accept Player
			snmdb::NormalManagerDB::getInstance().add(0, new CmdUpdateDailyQuestUser(_session.m_pi.uid, _session.m_pi.dqiu), MgrDailyQuest::SQLDBResponse, nullptr);

			v_ai.push_back(it->second);

			_smp::message_pool::getInstance().push(new message("[MgrDailyQuest::acceptQuestUser][Log] Player[UID=" + std::to_string(_session.m_pi.uid) + "] Aceitou a quest[ID=" + std::to_string(it->second.id) + "]", CL_FILE_LOG_AND_CONSOLE));
		}
	}

	return v_ai;
}

int32_t MgrDailyQuest::addCounterItemUser(player& _session, CounterItemInfo& _cii) {

	CHECK_SESSION("addCounterItemUser", _session);

	if (_cii.isValid())
		throw exception("[MgrDailyQuest::addCounterItemUser][Error] _counter_item_typeid is invalid", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MGR_DAILY_QUEST, 4, 0));

	// Add Counter Item
	CmdAddCounterItem cmd_aci(_session.m_pi.uid, _cii._typeid, _cii.value/*Value inicial do counter item*/, true);	// waitable

	snmdb::NormalManagerDB::getInstance().add(0, &cmd_aci, nullptr, nullptr);

	cmd_aci.waitEvent();

	if (cmd_aci.getException().getCodeError() != 0 || (_cii.id = cmd_aci.getId()) == -1)
		throw exception("[MgrDailyQuest::addCounterItemUser][Error] nao conseguiu adicionar o Counter Item[Typeid=" + std::to_string(_cii._typeid) + "] para o player: " + std::to_string(_session.m_pi.uid),
			STDA_MAKE_ERROR(STDA_ERROR_TYPE::MGR_DAILY_QUEST, 5, 0));

	return _cii.id;
}

void MgrDailyQuest::updateDailyQuest(DailyQuestInfo& _dqi) {

	if (!sIff::getInstance().isLoad())
		sIff::getInstance().load();

	auto map_qi = sIff::getInstance().getQuestItem();	// Daily Quest

	std::vector< uint32_t > a, b, c;

	std::for_each(map_qi.begin(), map_qi.end(), [&](auto& el) {
		if (((el.second._typeid & 0x00FFFFFF) >> 16) < 0x40) {	// 0x40 e 0x80, 0x78400001 n�o � daily quest, � a quest de completar daily quest
			switch (el.second.type) {
			case 1:	// F�cil
			default:
				a.push_back(el.second._typeid);
				break;
			case 2:	// M�dio
				b.push_back(el.second._typeid);
				break;
			case 3:	// Dif�cil
				c.push_back(el.second._typeid);
				break;
			}
		}
	});

	for (auto i = 0u; i < 3u; ++i) {

		switch (i + 1) {
		case 1:	// F�cil
		default:
			_dqi._typeid[i] = a[(sRandomGen::getInstance().rIbeMt19937_64_chrono() % a.size())];
			break;
		case 2:	// M�dio
			_dqi._typeid[i] = b[(sRandomGen::getInstance().rIbeMt19937_64_chrono() % b.size())];
			break;
		case 3:	// Dif�cil
			_dqi._typeid[i] = c[(sRandomGen::getInstance().rIbeMt19937_64_chrono() % c.size())];
			break;
		}
	}

	// Update Time Update Daily Quest
	GetLocalTime(&_dqi.date);

	CmdUpdateDailyQuest cmd_udq(_dqi, true);	// Waiter

	snmdb::NormalManagerDB::getInstance().add(0, &cmd_udq, nullptr, nullptr);

	cmd_udq.waitEvent();

	if (cmd_udq.getException().getCodeError() != 0) {
		
		_smp::message_pool::getInstance().push(new message("[MgrDailyQuest::updateDailyQuest][ErrorSystem] " + cmd_udq.getException().getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		return;		// Error sai da fun��o
	}

	// Update Aqui por que outro sistema conseguiu atualizar primeiro no banco de dados
	if (cmd_udq.isUpdated())
		_smp::message_pool::getInstance().push(new message("[MgrDailyQuest::updateDailyQuest][Log] Daily Quest[" + cmd_udq.getInfo().toString() 
				+ "] Atualizada com sucesso.", CL_FILE_LOG_AND_CONSOLE));
	else {

		_dqi = cmd_udq.getInfo();

		_smp::message_pool::getInstance().push(new message("[MgrDailyQuest::updateDailyQuest][Log] Daily Quest[" + cmd_udq.getInfo().toString()
				+ "] Atualizada por outro sistema, pega a atulizacao do outro sistema.", CL_FILE_LOG_AND_CONSOLE));
	}

;}

void MgrDailyQuest::SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg) {

#ifdef _DEBUG
	// Classe estatica n�o pode passar o ponteiro dela, por ser est�tica, ent�o passa nullptr
	if (_arg == nullptr)
		// Static class
		_smp::message_pool::getInstance().push(new message("[MgrDailyQuest::SQLDBResponse][WARNING] _arg is nullptr", CL_FILE_LOG_AND_CONSOLE));
#endif // _DEBUG

	// Por Hora s� sai, depois fa�o outro tipo de tratamento se precisar
	if (_pangya_db.getException().getCodeError() != 0) {
		_smp::message_pool::getInstance().push(new message("[MgrDailyQuest::SQLDBResponse][Error] " + _pangya_db.getException().getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
		return;
	}

	switch (_msg_id) {
	case 1:	// Update Daily Quest
	{
		auto cmd_dqi = reinterpret_cast< CmdUpdateDailyQuest* >(&_pangya_db);

		if (cmd_dqi->isUpdated())
			_smp::message_pool::getInstance().push(new message("[MgrDailyQuest][Log] Daily Quest[" + cmd_dqi->getInfo().toString() + "] Atualizada com sucesso.", CL_FILE_LOG_AND_CONSOLE));
		else {

			// N�o conseguiu atualizar primeiro que outro sistema, ent�o pega a atualiza��o do outro sistema
			//if (sgs::gs != nullptr)
				sgs::gs::getInstance().updateDailyQuest(cmd_dqi->getInfo());

			_smp::message_pool::getInstance().push(new message("[MgrDailyQuest][Log] Daily Quest[" + cmd_dqi->getInfo().toString() + "] Atualizada por outro sistema, pega a atulizacao do outro sistema.", CL_FILE_LOG_AND_CONSOLE));
		}

		break;
	}
	case 0:
	default:
		break;
	}
}
