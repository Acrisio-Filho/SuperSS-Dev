// Arquivo player_info.cpp
// Criado em 04/03/2018 as 12:33 por Acrisio
// Implementação da PlayerInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "player_info.h"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"
#include "../../Projeto IOCP/UTIL/hex_util.h"

#include "../PANGYA_DB/cmd_update_cookie.hpp"
#include "../PANGYA_DB/cmd_update_pang.hpp"
#include "../PANGYA_DB/cmd_update_level_and_exp.hpp"
#include "../PANGYA_DB/cmd_update_user_info.hpp"
#include "../PANGYA_DB/cmd_update_normal_trofel.hpp"

#include "../PANGYA_DB/cmd_insert_grand_prix_clear.hpp"
#include "../PANGYA_DB/cmd_update_grand_prix_clear.hpp"

#include "../PANGYA_DB/cmd_grand_zodiac_pontos.hpp"

#include "../PANGYA_DB/cmd_cookie.hpp"
#include "../PANGYA_DB/cmd_pang.hpp"

#include "../PANGYA_DB/cmd_trofel_info.hpp"
#include "../PANGYA_DB/cmd_user_info.hpp"

#include "../PANGYA_DB/cmd_update_player_location.hpp"

#include "../PANGYA_DB/cmd_find_mail_box_item.hpp"

#include "../../Projeto IOCP/DATABASE/normal_manager_db.hpp"

#include "../GAME/item_manager.h"

#include "game_type.hpp"

#include <algorithm>

using namespace stdA;

#define BEGIN_CALL_BACK_TITLE_CONDITION(_method)	int ret = 0; \
													\
													try { \
														if (_arg == nullptr) \
															throw exception("[PlayerInfo::" + std::string(#_method) + "][Error] arg is nullptr.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PLAYER, 5001, 0)); \
														\
														PlayerInfo *pi = reinterpret_cast< PlayerInfo* >(_arg); \
														\

#define END_CALL_BACK_TITLE_CONDITION(_method)		}catch (exception& e) { \
														_smp::message_pool::getInstance().push(new message("[PlayerInfo::" + std::string(#_method) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE)); \
													} \
													\
													return ret; \

int PlayerInfo::better_hit_pangya_bronze(void* _arg) {
	BEGIN_CALL_BACK_TITLE_CONDITION(better_hit_pangya_bronze);

	if (pi->ui.getPangyaShotRate() >= 70.f)
		ret = 1;	// passa na condição

	END_CALL_BACK_TITLE_CONDITION(better_hit_pangya_bronze);
};

int PlayerInfo::better_fairway_bronze(void* _arg) {
	BEGIN_CALL_BACK_TITLE_CONDITION(better_fairway_bronze);

	if (pi->ui.getFairwayRate() >= 70.f)
		ret = 1;	// passa na condição

	END_CALL_BACK_TITLE_CONDITION(better_fairway_bronze);
};

int PlayerInfo::better_putt_bronze(void* _arg) {
	BEGIN_CALL_BACK_TITLE_CONDITION(better_putt_bronze);

	if (pi->ui.getPuttRate() >= 80.f)
		ret = 1;	// passa na condição

	END_CALL_BACK_TITLE_CONDITION(better_putt_bronze);
};

int PlayerInfo::better_quit_rate_bronze(void* _arg) {
	BEGIN_CALL_BACK_TITLE_CONDITION(better_quit_rate_bronze);

	if (pi->ui.getQuitRate() <= 3.f)
		ret = 1;	// passa na condição

	END_CALL_BACK_TITLE_CONDITION(better_quit_rate_bronze);
};

int PlayerInfo::better_hit_pangya_silver(void* _arg) {
	BEGIN_CALL_BACK_TITLE_CONDITION(better_hit_pangya_silver);

	if (pi->ui.getPangyaShotRate() >= 77.f)
		ret = 1;	// passa na condição

	END_CALL_BACK_TITLE_CONDITION(better_hit_pangya_silver);
};

int PlayerInfo::better_fairway_silver(void* _arg) {
	BEGIN_CALL_BACK_TITLE_CONDITION(better_fairway_silver);

	if (pi->ui.getFairwayRate() >= 72.f)
		ret = 1;	// passa na condição

	END_CALL_BACK_TITLE_CONDITION(better_fairway_silver);
};

int PlayerInfo::better_putt_silver(void* _arg) {
	BEGIN_CALL_BACK_TITLE_CONDITION(better_putt_silver);

	if (pi->ui.getPuttRate() >= 90.f)
		ret = 1;	// passa na condição

	END_CALL_BACK_TITLE_CONDITION(better_putt_silver);
};

int PlayerInfo::better_quit_rate_silver(void* _arg) {
	BEGIN_CALL_BACK_TITLE_CONDITION(better_quit_rate_silver);

	if (pi->ui.getQuitRate() <= 2.f)
		ret = 1;	// passa na condição

	END_CALL_BACK_TITLE_CONDITION(better_quit_rate_silver);
};

int PlayerInfo::better_hit_pangya_gold(void* _arg) {
	BEGIN_CALL_BACK_TITLE_CONDITION(better_hit_pangya_gold);

	if (pi->ui.getPangyaShotRate() >= 85.f)
		ret = 1;	// passa na condição

	END_CALL_BACK_TITLE_CONDITION(better_hit_pangya_gold);
};

int PlayerInfo::better_fairway_gold(void* _arg) {
	BEGIN_CALL_BACK_TITLE_CONDITION(better_fairway_gold);

	if (pi->ui.getFairwayRate() >= 90.f)
		ret = 1;	// passa na condição

	END_CALL_BACK_TITLE_CONDITION(better_fairway_gold);
};

int PlayerInfo::better_putt_gold(void* _arg) {
	BEGIN_CALL_BACK_TITLE_CONDITION(better_putt_gold);

	if (pi->ui.getPuttRate() >= 95.f)
		ret = 1;	// passa na condição

	END_CALL_BACK_TITLE_CONDITION(better_putt_gold);
};

int PlayerInfo::better_quit_rate_gold(void* _arg) {
	BEGIN_CALL_BACK_TITLE_CONDITION(better_quit_rate_gold);

	if (pi->ui.getQuitRate() <= 1.f)
		ret = 1;	// passa na condição

	END_CALL_BACK_TITLE_CONDITION(better_quit_rate_gold);
};

int PlayerInfo::atirador_de_ouro(void* _arg) {
	BEGIN_CALL_BACK_TITLE_CONDITION(atirador_de_ouro);

	if (pi->ti_current_season.getSumGold() >= 10u)
		ret = 1;	// passa na condição

	END_CALL_BACK_TITLE_CONDITION(atirador_de_ouro);
};

int PlayerInfo::atirador_de_silver(void* _arg) {
	BEGIN_CALL_BACK_TITLE_CONDITION(atirador_de_silver);

	if (pi->ti_current_season.getSumSilver() >= 10u)
		ret = 1;	// passa na condição

	END_CALL_BACK_TITLE_CONDITION(atirador_de_silver);
};

int PlayerInfo::atirador_de_bronze(void* _arg) {
	BEGIN_CALL_BACK_TITLE_CONDITION(atirador_de_bronze);

	if (pi->ti_current_season.getSumBronze() >= 10u)
		ret = 1;	// passa na condição

	END_CALL_BACK_TITLE_CONDITION(atirador_de_bronze);
};

int PlayerInfo::master_course(void* _arg) {
	BEGIN_CALL_BACK_TITLE_CONDITION(master_course);

	if (pi->isMasterCourse())
		ret = 1;	// passa na condição

	END_CALL_BACK_TITLE_CONDITION(master_course);
};

int PlayerInfo::natural_record_80(void* _arg) {
	BEGIN_CALL_BACK_TITLE_CONDITION(natural_record_80);

	// No JP ele pega o do Grand Prix
	if (pi->getSumRecordGrandPrix() <= -80)
		ret = 1;	// passa na condição

	END_CALL_BACK_TITLE_CONDITION(natural_record_80);
};

int PlayerInfo::natural_record_200(void* _arg) {
	BEGIN_CALL_BACK_TITLE_CONDITION(natural_record_200);

	// No JP ele pega o do Grand Prix
	if (pi->getSumRecordGrandPrix() <= -200)
		ret = 1;	// passa na condição

	END_CALL_BACK_TITLE_CONDITION(natural_record_200);
};

int PlayerInfo::natural_record_300(void* _arg) {
	BEGIN_CALL_BACK_TITLE_CONDITION(natural_record_300);

	// No JP ele pega o do Grand Prix
	if (pi->getSumRecordGrandPrix() <= -300)
		ret = 1;	// passa na condição

	END_CALL_BACK_TITLE_CONDITION(natural_record_300);
};

int PlayerInfo::natural_record_350(void* _arg) {
	BEGIN_CALL_BACK_TITLE_CONDITION(natural_record_350);

	// No JP ele pega o do Grand Prix
	if (pi->getSumRecordGrandPrix() <= -350)
		ret = 1;	// passa na condição

	END_CALL_BACK_TITLE_CONDITION(natural_record_350);
};

int PlayerInfo::natural_record_390(void* _arg) {
	BEGIN_CALL_BACK_TITLE_CONDITION(natural_record_390);

	// No JP ele pega o do Grand Prix
	if (pi->getSumRecordGrandPrix() <= -390)
		ret = 1;	// passa na condição

	END_CALL_BACK_TITLE_CONDITION(natural_record_390);
};

int PlayerInfo::natural_record_420(void* _arg) {
	BEGIN_CALL_BACK_TITLE_CONDITION(natural_record_420);

	// No JP ele pega o do Grand Prix
	if (pi->getSumRecordGrandPrix() <= -420)
		ret = 1;	// passa na condição

	END_CALL_BACK_TITLE_CONDITION(natural_record_420);
};

int PlayerInfo::natural_record_470(void* _arg) {
	BEGIN_CALL_BACK_TITLE_CONDITION(natural_record_470);

	// No JP ele pega o do Grand Prix
	if (pi->getSumRecordGrandPrix() <= -470)
		ret = 1;	// passa na condição

	END_CALL_BACK_TITLE_CONDITION(natural_record_470);
};

int PlayerInfo::natural_record_540(void* _arg) {
	BEGIN_CALL_BACK_TITLE_CONDITION(natural_record_540);

	// No JP ele pega o do Grand Prix
	if (pi->getSumRecordGrandPrix() <= -540)
		ret = 1;	// passa na condição

	END_CALL_BACK_TITLE_CONDITION(natural_record_540);
};

PlayerInfo::PlayerInfo() : player_info() {

#if defined(_WIN32)
	InitializeCriticalSection(&m_cs);
#elif defined(__linux__)
	INIT_PTHREAD_MUTEXATTR_RECURSIVE;
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif

	clear();

	// Inicializa o map de Title call back condition
	mp_title_callback.insert(std::make_pair(0x15, stTitleMapCallback(PlayerInfo::better_hit_pangya_bronze, this)));
	mp_title_callback.insert(std::make_pair(0x16, stTitleMapCallback(PlayerInfo::better_fairway_bronze, this)));
	mp_title_callback.insert(std::make_pair(0x17, stTitleMapCallback(PlayerInfo::better_putt_bronze, this)));
	mp_title_callback.insert(std::make_pair(0x18, stTitleMapCallback(PlayerInfo::master_course, this)));
	mp_title_callback.insert(std::make_pair(0x19, stTitleMapCallback(PlayerInfo::atirador_de_ouro, this)));
	mp_title_callback.insert(std::make_pair(0x1a, stTitleMapCallback(PlayerInfo::atirador_de_silver, this)));
	mp_title_callback.insert(std::make_pair(0x1b, stTitleMapCallback(PlayerInfo::atirador_de_bronze, this)));
	mp_title_callback.insert(std::make_pair(0x1C, stTitleMapCallback(PlayerInfo::better_quit_rate_bronze, this)));
	mp_title_callback.insert(std::make_pair(0x32, stTitleMapCallback(PlayerInfo::better_hit_pangya_silver, this)));
	mp_title_callback.insert(std::make_pair(0x33, stTitleMapCallback(PlayerInfo::better_fairway_silver, this)));
	mp_title_callback.insert(std::make_pair(0x34, stTitleMapCallback(PlayerInfo::better_putt_silver, this)));
	mp_title_callback.insert(std::make_pair(0x35, stTitleMapCallback(PlayerInfo::better_quit_rate_silver, this)));
	mp_title_callback.insert(std::make_pair(0x45, stTitleMapCallback(PlayerInfo::natural_record_420, this)));
	mp_title_callback.insert(std::make_pair(0x46, stTitleMapCallback(PlayerInfo::natural_record_390, this)));
	mp_title_callback.insert(std::make_pair(0x47, stTitleMapCallback(PlayerInfo::natural_record_350, this)));
	mp_title_callback.insert(std::make_pair(0x48, stTitleMapCallback(PlayerInfo::natural_record_300, this)));
	mp_title_callback.insert(std::make_pair(0x49, stTitleMapCallback(PlayerInfo::natural_record_200, this)));
	mp_title_callback.insert(std::make_pair(0x4a, stTitleMapCallback(PlayerInfo::natural_record_80, this)));
	mp_title_callback.insert(std::make_pair(0x7B, stTitleMapCallback(PlayerInfo::better_quit_rate_gold, this)));
	mp_title_callback.insert(std::make_pair(0x7C, stTitleMapCallback(PlayerInfo::better_putt_gold, this)));
	mp_title_callback.insert(std::make_pair(0x7D, stTitleMapCallback(PlayerInfo::better_fairway_gold, this)));
	mp_title_callback.insert(std::make_pair(0x7E, stTitleMapCallback(PlayerInfo::better_hit_pangya_gold, this)));
	mp_title_callback.insert(std::make_pair(0x17C, stTitleMapCallback(PlayerInfo::natural_record_470, this)));
	mp_title_callback.insert(std::make_pair(0x17D, stTitleMapCallback(PlayerInfo::natural_record_540, this)));
};

PlayerInfo::~PlayerInfo() {
	
	clear();

	// Limpa map do title call back
	if (!mp_title_callback.empty())
		mp_title_callback.clear();

#if defined(_WIN32)
	DeleteCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_destroy(&m_cs);
#endif
};

void PlayerInfo::clear() {
	
	player_info::clear();

	m_pl.clear();

	size_t i, j;

	for (i = 0; i < MS_NUM_MAPS; i++) {
		a_ms_normal[i].clear((unsigned char)i);
		a_ms_natural[i].clear((unsigned char)i);
		a_ms_grand_prix[i].clear((unsigned char)i);
		a_msa_normal[i].clear((unsigned char)i);
		a_msa_natural[i].clear((unsigned char)i);
		a_msa_grand_prix[i].clear((unsigned char)i);
	}

	for (j = 0; j < 9; j++)
		for (i = 0; i < MS_NUM_MAPS; i++)
			aa_ms_normal_todas_season[j][i].clear((unsigned char)i);

	mi.clear();
	ui.clear();
	pt.clear();
	ti_current_season.clear();
	ti_rest_season.clear();
	TutoInfo.clear();
	ue.clear();
	cg.clear();
	gi.clear();
	ei.clear();
	cmu.clear();
	cwlul.clear();
	cwtc.clear();
	mrc.clear();
	df.clear();
	dqiu.clear();
	l5pg.clear();
	ari.clear();

	cookie = 0ull;
	place = 0;
	location.clear();
	state = 0u;
	state_lounge = 0u;
	whisper = 1u;	// Whisper Começa ativo, que desativa ele é o pangya na hora de logar

	m_state_logged = 0;

	grand_zodiac_pontos = 0ull;

	channel = INVALID_CHANNEL;
	lobby = INVALID_CHANNEL;

	if (!mp_fi.empty())
		mp_fi.clear();

	if (!v_mri.empty()) {
		v_mri.clear();
		v_mri.shrink_to_fit();
	}

	if (!mp_ce.empty()) {
		mp_ce.clear();
		//v_ce.shrink_to_fit();
	}

	if (!mp_scl.empty())
		mp_scl.clear();

	if (!mp_ci.empty()) {
		mp_ci.clear();
		//v_ci.shrink_to_fit();
	}

	if (!mp_mi.empty()) {
		mp_mi.clear();
		//v_mi.shrink_to_fit();
	}

	if (!mp_wi.empty()) {
		mp_wi.clear();
		//v_wi.shrink_to_fit();
	}

	mgr_achievement.clear();

	if (!v_card_info.empty()) {
		v_card_info.clear();
		v_card_info.shrink_to_fit();
	}

	if (!v_cei.empty()) {
		v_cei.clear();
		v_cei.shrink_to_fit();
	}

	if (!v_ib.empty()) {
		v_ib.clear();
		v_ib.shrink_to_fit();
	}

	if (!mp_ui.empty()) {
		mp_ui.clear();
		//v_ui.shrink_to_fit();
	}

	if (!v_gpc.empty()) {
		v_gpc.clear();
		v_gpc.shrink_to_fit();
	}

	if (!v_tsi_current_season.empty()) {
		v_tsi_current_season.clear();
		v_tsi_current_season.shrink_to_fit();
	}

	if (!v_tsi_rest_season.empty()) {
		v_tsi_rest_season.clear();
		v_tsi_rest_season.shrink_to_fit();
	}

	if (!v_tgp_current_season.empty()) {
		v_tgp_current_season.clear();
		v_tgp_current_season.shrink_to_fit();
	}

	if (!v_tgp_rest_season.empty()) {
		v_tgp_rest_season.clear();
		v_tgp_rest_season.shrink_to_fit();
	}

	// Clear Mail Box
	m_mail_box.clear();
};

PlayerInfo::stTitleMapCallback* PlayerInfo::getTitleCallBack(uint32_t _id) {

	auto it = mp_title_callback.find(_id);

	return (it != mp_title_callback.end() ? &it->second : nullptr);
};

uint32_t PlayerInfo::getSizeCupGrandZodiac() {

	uint32_t size_cup = 1u;

	if (grand_zodiac_pontos < 300)
		size_cup = 9u;
	else if (grand_zodiac_pontos < 600)
		size_cup = 8u;
	else if (grand_zodiac_pontos < 1200)
		size_cup = 7u;
	else if (grand_zodiac_pontos < 1800)
		size_cup = 6u;
	else if (grand_zodiac_pontos < 4000)
		size_cup = 5u;
	else if (grand_zodiac_pontos < 5200)
		size_cup = 4u;
	else if (grand_zodiac_pontos < 7600)
		size_cup = 3u;
	else if (grand_zodiac_pontos < 10000)
		size_cup = 2u;

	return size_cup;
};

FriendInfo* PlayerInfo::findFriendInfoByUID(uint32_t _uid) {

	if (_uid == 0u) {
		_smp::message_pool::getInstance().push(new message("[PlayerInfo::findFriendInfoByUID][Error] _uid is invalid(0)", CL_FILE_LOG_AND_CONSOLE));

		return nullptr;
	}

	auto it = mp_fi.find(_uid);

	return (it != mp_fi.end()) ? &it->second : nullptr;
};

FriendInfo* PlayerInfo::findFriendInfoById(std::string _id) {
	
	if (_id.empty()) {
		_smp::message_pool::getInstance().push(new message("[PlayerInfo::findFriendInfoByID][Error] _id is invalid(empty)", CL_FILE_LOG_AND_CONSOLE));

		return nullptr;
	}

	auto it = std::find_if(mp_fi.begin(), mp_fi.end(), [&](auto& _el) {
		return _id.compare(_el.second.id) == 0;
	});

	return (it != mp_fi.end()) ? &it->second : nullptr;
};

FriendInfo* PlayerInfo::findFriendInfoByNickname(std::string _nickname) {
	
	if (_nickname.empty()) {
		_smp::message_pool::getInstance().push(new message("[PlayerInfo::findFriendInfoByNickname][Error] _nickname is invalid(empty)", CL_FILE_LOG_AND_CONSOLE));

		return nullptr;
	}

	auto it = std::find_if(mp_fi.begin(), mp_fi.end(), [&](auto& _el) {
		return _nickname.compare(_el.second.nickname) == 0;
	});

	return (it != mp_fi.end()) ? &it->second : nullptr;
};

WarehouseItemEx* PlayerInfo::findWarehouseItemById(int32_t _id) {
	
	auto it = findWarehouseItemItById(_id);

	return (it == mp_wi.end() ? nullptr : &it->second);
}

WarehouseItemEx* PlayerInfo::findWarehouseItemByTypeid(uint32_t _typeid) {
	
	auto it = findWarehouseItemItByTypeid(_typeid);

	return (it == mp_wi.end() ? nullptr : &it->second);
}

WarehouseItemEx* PlayerInfo::findWarehouseItemByTypeidAndId(uint32_t _typeid, int32_t _id) {
	
	auto it = findWarehouseItemItByTypeidAndId(_typeid, _id);

	return (it == mp_wi.end() ? nullptr : &it->second);
}

CharacterInfo* PlayerInfo::findCharacterById(int32_t _id) {
	
	auto it = findCharacterItById(_id);

	return (it == mp_ce.end() ? nullptr : &it->second);
}

CharacterInfo* PlayerInfo::findCharacterByTypeid(uint32_t _typeid) {
	
	auto it = findCharacterItByTypeid(_typeid);

	return (it == mp_ce.end() ? nullptr : &it->second);
}

CharacterInfo* PlayerInfo::findCharacterByTypeidAndId(uint32_t _typeid, int32_t _id) {
	
	auto it = findCharacterItByTypeidAndId(_typeid, _id);

	return (it == mp_ce.end() ? nullptr : &it->second);
}

CaddieInfoEx* PlayerInfo::findCaddieById(int32_t _id) {
	
	auto it = findCaddieItById(_id);

	return (it == mp_ci.end() ? nullptr : &it->second);
}

CaddieInfoEx* PlayerInfo::findCaddieByTypeid(uint32_t _typeid) {
	
	auto it = findCaddieItByTypeid(_typeid);

	return (it == mp_ci.end() ? nullptr : &it->second);
}

CaddieInfoEx* PlayerInfo::findCaddieByTypeidAndId(uint32_t _typeid, int32_t _id) {
	
	auto it = findCaddieItByTypeidAndId(_typeid, _id);

	return (it == mp_ci.end() ? nullptr : &it->second);
}

MascotInfoEx* PlayerInfo::findMascotById(int32_t _id) {
	
	auto it = findMascotItById(_id);

	return (it == mp_mi.end() ? nullptr : &it->second);
}

MascotInfoEx* PlayerInfo::findMascotByTypeid(uint32_t _typeid) {
	
	auto it = findMascotItByTypeid(_typeid);

	return (it == mp_mi.end() ? nullptr : &it->second);
}

MascotInfoEx* PlayerInfo::findMascotByTypeidAndId(uint32_t _typeid, int32_t _id) {
	
	auto it = findMascotItByTypeidAndId(_typeid, _id);

	return (it == mp_mi.end() ? nullptr : &it->second);
}

MyRoomItem* PlayerInfo::findMyRoomItemById(int32_t _id) {
	
	auto it = findMyRoomItemItById(_id);

	return (it == v_mri.end() ? nullptr : &(*it));
}

MyRoomItem* PlayerInfo::findMyRoomItemByTypeid(uint32_t _typeid) {
	
	auto it = findMyRoomItemItByTypeid(_typeid);

	return (it == v_mri.end() ? nullptr : &(*it));
}

CardInfo* PlayerInfo::findCardById(int32_t _id) {
	
	auto it = findCardItById(_id);

	return (it == v_card_info.end() ? nullptr : &(*it));
}

CardInfo* PlayerInfo::findCardByTypeid(uint32_t _typeid) {
	
	auto it = findCardItByTypeid(_typeid);

	return (it == v_card_info.end() ? nullptr : &(*it));
}

CardEquipInfoEx* PlayerInfo::findCardEquipedById(int32_t _id, uint32_t _char_typeid, uint32_t _slot) {
	
	auto it = findCardEquipedItById(_id, _char_typeid, _slot);

	return (it == v_cei.end() ? nullptr : &(*it));
}

CardEquipInfoEx* PlayerInfo::findCardEquipedByTypeid(uint32_t _typeid, uint32_t _char_typeid, uint32_t _slot, uint32_t _tipo, uint32_t _efeito) {
	
	auto it = findCardEquipedItByTypeid(_typeid, _char_typeid, _slot, _tipo, _efeito);

	return (it == v_cei.end() ? nullptr : &(*it));
}

TrofelEspecialInfo* PlayerInfo::findTrofelEspecialById(int32_t _id) {
	
	auto it = findTrofelEspecialItById(_id);

	return (it == v_tsi_current_season.end() ? nullptr : &(*it));
}

TrofelEspecialInfo* PlayerInfo::findTrofelEspecialByTypeid(uint32_t _typeid) {
	
	auto it = findTrofelEspecialItByTypeid(_typeid);

	return (it == v_tsi_current_season.end() ? nullptr : &(*it));
}

TrofelEspecialInfo* PlayerInfo::findTrofelEspecialByTypeidAndId(uint32_t _typeid, int32_t _id) {
	
	auto it = findTrofelEspecialItByTypeidAndId(_typeid, _id);

	return (it == v_tsi_current_season.end() ? nullptr : &(*it));
}

TrofelEspecialInfo* PlayerInfo::findTrofelGrandPrixById(int32_t _id) {
	
	auto it = findTrofelGrandPrixItById(_id);

	return (it == v_tgp_current_season.end() ? nullptr : &(*it));
}

TrofelEspecialInfo* PlayerInfo::findTrofelGrandPrixByTypeid(uint32_t _typeid) {
	
	auto it = findTrofelGrandPrixItByTypeid(_typeid);

	return (it == v_tgp_current_season.end() ? nullptr : &(*it));
}

TrofelEspecialInfo* PlayerInfo::findTrofelGrandPrixByTypeidAndId(uint32_t _typeid, int32_t _id) {
	
	auto it = findTrofelGrandPrixItByTypeidAndId(_typeid, _id);

	return (it == v_tgp_current_season.end() ? nullptr : &(*it));
}

std::multimap< int32_t/*ID*/, WarehouseItemEx >::iterator PlayerInfo::findWarehouseItemItById(int32_t _id) {
	
	return mp_wi.find(_id);
}

std::multimap< int32_t/*ID*/, WarehouseItemEx >::iterator PlayerInfo::findWarehouseItemItByTypeid(uint32_t _typeid) {
	
	return std::find_if(mp_wi.begin(), mp_wi.end(), [&](auto& _el) {
		return _el.second._typeid == _typeid;
	});
}

std::multimap< int32_t/*ID*/, WarehouseItemEx >::iterator PlayerInfo::findWarehouseItemItByTypeidAndId(uint32_t _typeid, int32_t _id) {
	
	auto it = mp_wi.find(_id);

	if (it != mp_wi.end() && it->second._typeid != _typeid) {
		
		if (mp_wi.count(_id) > 1) {
			auto er = mp_wi.equal_range(_id);

			it = std::find_if(er.first, er.second/*End*/, [&](auto& _el) {
				return _el.second._typeid == _typeid;
			});
		}else
			it = mp_wi.end();
	}

	return it;
}

std::multimap< int32_t/*ID*/, CharacterInfo >::iterator PlayerInfo::findCharacterItById(int32_t _id) {
	
	return mp_ce.find(_id);
}

std::multimap< int32_t/*ID*/, CharacterInfo >::iterator PlayerInfo::findCharacterItByTypeid(uint32_t _typeid) {
	
	return std::find_if(mp_ce.begin(), mp_ce.end(), [&](auto& _el) {
		return _el.second._typeid == _typeid;
	});
}

std::multimap< int32_t/*ID*/, CharacterInfo >::iterator PlayerInfo::findCharacterItByTypeidAndId(uint32_t _typeid, int32_t _id) {
	
	auto it = mp_ce.find(_id);

	if (it != mp_ce.end() && it->second._typeid != _typeid) {
		
		if (mp_ce.count(_id) > 1) {
			auto er = mp_ce.equal_range(_id);

			it = std::find_if(er.first, er.second/*End*/, [&](auto& _el) {
				return _el.second._typeid == _typeid;
			});
		}else
			it = mp_ce.end();
	}

	return it;
}

std::multimap< int32_t/*ID*/, CaddieInfoEx >::iterator PlayerInfo::findCaddieItById(int32_t _id) {

	return mp_ci.find(_id);
}

std::multimap< int32_t/*ID*/, CaddieInfoEx >::iterator PlayerInfo::findCaddieItByTypeid(uint32_t _typeid) {
	
	return std::find_if(mp_ci.begin(), mp_ci.end(), [&](auto& _el) {
		return _el.second._typeid == _typeid;
	});
}

std::multimap< int32_t/*ID*/, CaddieInfoEx >::iterator PlayerInfo::findCaddieItByTypeidAndId(uint32_t _typeid, int32_t _id) {
	
	auto it = mp_ci.find(_id);

	if (it != mp_ci.end() && it->second._typeid != _typeid) {
		
		if (mp_ci.count(_id) > 1) {
			auto er = mp_ci.equal_range(_id);

			it = std::find_if(er.first, er.second/*End*/, [&](auto& _el) {
				return _el.second._typeid == _typeid;
			});
		}else
			it = mp_ci.end();
	}

	return it;
}

std::multimap< int32_t/*ID*/, MascotInfoEx >::iterator PlayerInfo::findMascotItById(int32_t _id) {
	
	return mp_mi.find(_id);
}

std::multimap< int32_t/*ID*/, MascotInfoEx >::iterator PlayerInfo::findMascotItByTypeid(uint32_t _typeid) {
	
	return std::find_if(mp_mi.begin(), mp_mi.end(), [&](auto& _el) {
		return _el.second._typeid == _typeid;
	});
}

std::multimap< int32_t/*ID*/, MascotInfoEx >::iterator PlayerInfo::findMascotItByTypeidAndId(uint32_t _typeid, int32_t _id) {
	
	auto it = mp_mi.find(_id);

	if (it != mp_mi.end() && it->second._typeid != _typeid) {

		if (mp_mi.count(_id) > 1) {
			auto er = mp_mi.equal_range(_id);

			it = std::find_if(er.first, er.second/*End*/, [&](auto& _el) {
				return _el.second._typeid == _typeid;
			});
		}else
			it = mp_mi.end();
	}

	return it;
}

std::vector< MyRoomItem >::iterator PlayerInfo::findMyRoomItemItById(int32_t _id) {
	
	return std::find_if(v_mri.begin(), v_mri.end(), [&](auto& _el) {
		return _el.id == _id;
	});
}

std::vector< MyRoomItem >::iterator PlayerInfo::findMyRoomItemItByTypeid(uint32_t _typeid) {
	
	return std::find_if(v_mri.begin(), v_mri.end(), [&](auto& _el) {
		return _el._typeid == _typeid;
	});
}

std::vector< CardInfo >::iterator PlayerInfo::findCardItById(int32_t _id) {
	
	return std::find_if(v_card_info.begin(), v_card_info.end(), [&](auto& _el) {
		return _el.id == _id;
	});
}

std::vector< CardInfo >::iterator PlayerInfo::findCardItByTypeid(uint32_t _typeid) {
	
	return std::find_if(v_card_info.begin(), v_card_info.end(), [&](auto& _el) {
		return _el._typeid == _typeid;
	});
}

std::vector< CardEquipInfoEx >::iterator PlayerInfo::findCardEquipedItById(int32_t _id, uint32_t _char_typeid, uint32_t _slot) {
	
	return std::find_if(v_cei.begin(), v_cei.end(), [&](auto& _element) {
		return (_element.id == _id && ((_char_typeid == 0 && _slot == 0)
			|| (_element.parts_typeid == _char_typeid && _element.slot == _slot)));
	});
}

std::vector< CardEquipInfoEx >::iterator PlayerInfo::findCardEquipedItByTypeid(uint32_t _typeid, uint32_t _char_typeid, uint32_t _slot, uint32_t _tipo, uint32_t _efeito) {
	
	return std::find_if(v_cei.begin(), v_cei.end(), [&](auto& _element) {
		return ((_element._typeid == _typeid || (_element.tipo == _tipo && _element.efeito == _efeito))
			&& ((_char_typeid == 0 && _slot == 0) || (_element.parts_typeid == _char_typeid && _element.slot == _slot)));
	});
}

std::vector< TrofelEspecialInfo >::iterator PlayerInfo::findTrofelEspecialItById(int32_t _id) {
	
	return std::find_if(v_tsi_current_season.begin(), v_tsi_current_season.end(), [&](auto& _el) {
		return _el.id == _id;
	});
}

std::vector<  TrofelEspecialInfo >::iterator PlayerInfo::findTrofelEspecialItByTypeid(uint32_t _typeid) {
	
	return std::find_if(v_tsi_current_season.begin(), v_tsi_current_season.end(), [&](auto& _el) {
		return _el._typeid == _typeid;
	});
}

std::vector< TrofelEspecialInfo >::iterator PlayerInfo::findTrofelEspecialItByTypeidAndId(uint32_t _typeid, int32_t _id) {
	
	return std::find_if(v_tsi_current_season.begin(), v_tsi_current_season.end(), [&](auto& _el) {
		return _el._typeid == _typeid && _el.id == _id;
	});
}

std::vector<TrofelEspecialInfo >::iterator PlayerInfo::findTrofelGrandPrixItById(int32_t _id) {
	
	return std::find_if(v_tgp_current_season.begin(), v_tgp_current_season.end(), [&](auto& _el) {
		return _el.id == _id;
	});
}

std::vector< TrofelEspecialInfo >::iterator PlayerInfo::findTrofelGrandPrixItByTypeid(uint32_t _typeid) {
	
	return std::find_if(v_tgp_current_season.begin(), v_tgp_current_season.end(), [&](auto& _el) {
		return _el._typeid == _typeid;
	});
}

std::vector< TrofelEspecialInfo >::iterator PlayerInfo::findTrofelGrandPrixItByTypeidAndId(uint32_t _typeid, int32_t _id) {
	
	return std::find_if(v_tgp_current_season.begin(), v_tgp_current_season.end(), [&](auto& _el) {
		return _el._typeid == _typeid && _el.id == _id;
	});
}

std::vector< WarehouseItemEx* > PlayerInfo::findAllPartNotEquiped(uint32_t _typeid) {

	std::vector< WarehouseItemEx* > v_item;

	std::for_each(mp_wi.begin(), mp_wi.end(), [&](auto& el) {
		auto it = mp_ce.end();

		if (el.second._typeid == _typeid && (el.second.flag & 96) != 96/*Não pode Part Rental*/ && (el.second.flag & 0x20) != 0x20 && (el.second.flag & 0x40) != 0x40 && (it = std::find_if(mp_ce.begin(), mp_ce.end(), [&](auto& el2) {
			return el2.second.isPartEquiped(el.second._typeid, el.second.id);
		})) == mp_ce.end()/*Not Found, então a roupa não está equipada*/) {
			v_item.push_back(&el.second);
		}	// End if
	});

	return v_item;
};

ItemBuffEx* PlayerInfo::findItemBuff(uint32_t _typeid, uint32_t _tipo) {
	auto it = v_ib.end();
	
	it = std::find_if(v_ib.begin(), v_ib.end(), [&](auto& el) {
		return (el._typeid == _typeid || el.tipo == _tipo);
	});

	return (it == v_ib.end() ? nullptr : &(*it));
};

std::multimap< PlayerInfo::stIdentifyKey/*int32_t/*ID*/, UpdateItem >::iterator PlayerInfo::findUpdateItemByTypeid(uint32_t _typeid) {
	
	auto it = mp_ui.end();

	it = std::find_if(mp_ui.begin(), mp_ui.end(), [&](auto& el) {
		return (el.second._typeid == _typeid);
	});

	return it;
};

std::multimap< PlayerInfo::stIdentifyKey/*int32_t/*ID*/, UpdateItem >::iterator PlayerInfo::findUpdateItemByType(UpdateItem::UI_TYPE _type) {
	
	auto it = mp_ui.end();

	it = std::find_if(mp_ui.begin(), mp_ui.end(), [&](auto& el) {
		return (el.second.type == _type);
	});

	return it;
};

std::vector< std::multimap< PlayerInfo::stIdentifyKey/*int32_t/*ID*/, UpdateItem >::iterator > PlayerInfo::findUpdateItemByTypeidAndId(uint32_t _typeid, int32_t _id) {
	
	std::vector< std::multimap< PlayerInfo::stIdentifyKey/*int32_t/*ID*/, UpdateItem >::iterator > v_it;

	auto c = mp_ui.count(PlayerInfo::stIdentifyKey(_typeid, _id));

	if (c > 0) {

		auto range = mp_ui.equal_range(PlayerInfo::stIdentifyKey(_typeid, _id));

		for (auto it = range.first; it != range.second; ++it)
			v_it.push_back(it);
	}

	/*for (auto it = mp_ui.begin(); it != mp_ui.end(); ++it) {
		
		if (it->second._typeid == _typeid && it->second.id == _id)
			v_it.push_back(it);
	};*/

	return v_it;
};

std::multimap< PlayerInfo::stIdentifyKey/*int32_t/*ID*/, UpdateItem >::iterator PlayerInfo::findUpdateItemByIdAndType(int32_t _id, UpdateItem::UI_TYPE _type) {
	
	auto it = mp_ui.end();

	it = std::find_if(mp_ui.begin(), mp_ui.end(), [&](auto& el) {
		return (el.second.id == _id && el.second.type == _type);
	});

	return it;
};

std::multimap< PlayerInfo::stIdentifyKey/*int32_t/*ID*/, UpdateItem >::iterator PlayerInfo::findUpdateItemByTypeidAndType(uint32_t _typeid, UpdateItem::UI_TYPE _type) {
	
	auto it = mp_ui.end();

	it = std::find_if(mp_ui.begin(), mp_ui.end(), [&](auto& el) {
		return (el.second._typeid == _typeid && el.second.type == _type);
	});

	return it;
};

GrandPrixClear* PlayerInfo::findGrandPrixClear(uint32_t _typeid) {
	
	if (_typeid == 0u)
		return nullptr;

	auto it = std::find_if(v_gpc.begin(), v_gpc.end(), [&](auto& _el) {
		return _el._typeid == _typeid;
	});

	return (it != v_gpc.end() ? &(*it) : nullptr);
};

ExtraPower PlayerInfo::getExtraPower(bool _pwr_condition_actived) {

	// Extra Power, Mascot, Ring, Card
	ExtraPower::stPowerQntd power_drive{ 0l }, power_shot{ 0l };

	if (ei.mascot_info != nullptr) {

		auto mascot = sIff::getInstance().findMascot(ei.mascot_info->_typeid);

		if (mascot != nullptr)
			power_drive.m_mascot += mascot->efeito.power_drive; // Power;
	}

	if (ei.char_info != nullptr) {

		auto character = sIff::getInstance().findCharacter(ei.char_info->_typeid);

		if (character != nullptr) {

			std::vector< uint32_t > effect_id_past;

			// Ring
			for (auto i = 0u; i < (sizeof(ei.char_info->auxparts) / sizeof(ei.char_info->auxparts[0])); ++i) {

				if (ei.char_info->auxparts[i] != 0) {

					auto auxpart = sIff::getInstance().findAuxPart(ei.char_info->auxparts[i]);

					if (auxpart != nullptr) {

						auto ab = sIff::getInstance().findAbility(auxpart->_typeid);

						if (ab != nullptr) {

							if ((ab->efeito.type[0] == 11 && ab->efeito.rate[0] == 0) 
								|| (ab->efeito.type[1] == 11 && ab->efeito.rate[1] == 0)
								|| (ab->efeito.type[2] == 11 && ab->efeito.rate[2] == 0)) {
								
								// Efeito type 11 o pangya decide qual é a condição pelo typeid do item, dentro do pangya

								// Aqui só da força se a condição foi ativada, como 3 (Power Gauge) ou sem (Power Gauge)
								if (_pwr_condition_actived)
									power_drive.m_auxpart += auxpart->efeito.power_drive; // Power
							
							}else {

								for (auto j = 0u; j < (sizeof(ab->efeito.type) / sizeof(ab->efeito.type[0])); ++j) {

									if (ab->efeito.type[j] == 13/*SetEffectTable*/) {

										// Continua já verificou esse SetEffectTable
										if (std::find(effect_id_past.begin(), effect_id_past.end(), (uint32_t)ab->efeito.rate[j]) != effect_id_past.end())
											continue;

										effect_id_past.push_back((uint32_t)ab->efeito.rate[j]);
										
										// Find
										auto set = sIff::getInstance().findSetEffectTable((uint32_t)ab->efeito.rate[j]);

										if (set != nullptr) {
											
											auto count = std::count_if(set->item._typeid, LAST_ELEMENT_IN_ARRAY(set->item._typeid), [&](auto _el) {

												if (_el != 0) {
													
													switch (sIff::getInstance().getItemGroupIdentify(_el)) {
													case iff::AUX_PART:
														return ei.char_info->isAuxPartEquiped(_el);
													case iff::PART:
														return ei.char_info->isPartEquiped(_el);
													}
												}

												return _el == 0;
											});

											if (count == 5)
												power_drive.m_auxpart += set->effect_add_power;
										}
									}
								}
							}

						}else
							power_drive.m_auxpart += auxpart->efeito.power_drive; // Power
					}
				}
			}

			//-- Card
			// Character
			for (auto i = 0u; i < (sizeof(ei.char_info->card_character) / sizeof(ei.char_info->card_character[0])); ++i) {

				if (ei.char_info->card_character[i] != 0) {

					auto card = sIff::getInstance().findCard(ei.char_info->card_character[i]);

					if (card != nullptr && sIff::getInstance().getItemSubGroupIdentify22(card->_typeid) == 0/*Character*/
							&& card->efeito.type == 1/*minus(-) distance*/)
						power_drive.m_card -= card->efeito.qntd;
				}
			}

			// Caddie
			for (auto i = 0u; i < (sizeof(ei.char_info->card_caddie) / sizeof(ei.char_info->card_caddie[0])); ++i) {

				if (ei.char_info->card_caddie[i] != 0) {

					auto card = sIff::getInstance().findCard(ei.char_info->card_caddie[i]);

					if (card != nullptr && sIff::getInstance().getItemSubGroupIdentify22(card->_typeid) == 1/*Caddie*/) {

						if (card->efeito.type == 5/*Lolo*/)
							power_shot.m_card += card->efeito.qntd;
						else if (card->efeito.type == 2/*Pippin*/)
							power_drive.m_card += card->efeito.qntd;
					}
				}
			}
		}
	}

	// Card Special
	for (auto it = v_cei.begin(); it != v_cei.end(); ++it) {

		if (it->parts_id == 0 && it->parts_typeid == 0 && sIff::getInstance().getItemSubGroupIdentify22(it->_typeid) == 2/*Special*/
				&& it->efeito == 27)
			power_drive.m_card += it->efeito_qntd;
	}

	return ExtraPower(power_drive, power_shot);
};

uint32_t PlayerInfo::getSlotPower() {

	uint32_t power_slot = 0u;

	// Check CL to Caddie, Mascot, Character, ClubSet, Ring and Card
	if (ei.cad_info != nullptr) {

		auto cad = sIff::getInstance().findCaddie(ei.cad_info->_typeid);

		if (cad != nullptr)
			power_slot += cad->c[0]; // Power
	}

	if (ei.mascot_info != nullptr) {

		auto mascot = sIff::getInstance().findMascot(ei.mascot_info->_typeid);

		if (mascot != nullptr)
			power_slot += mascot->c[0]; // Power;
	}

	if (ei.char_info != nullptr) {

		auto character = sIff::getInstance().findCharacter(ei.char_info->_typeid);

		if (character != nullptr) {

			power_slot += character->c[0]; // Power

			// Parts
			for (auto i = 0u; i < (sizeof(ei.char_info->parts_typeid) / sizeof(ei.char_info->parts_typeid[0])); ++i) {

				if (ei.char_info->parts_typeid[i] != 0) {

					auto parts = sIff::getInstance().findPart(ei.char_info->parts_typeid[i]);

					if (parts != nullptr)
						power_slot += parts->c[0]; // Power

				}
			}

			// Ring
			for (auto i = 0u; i < (sizeof(ei.char_info->auxparts) / sizeof(ei.char_info->auxparts[0])); ++i) {

				if (ei.char_info->auxparts[i] != 0) {

					auto auxpart = sIff::getInstance().findAuxPart(ei.char_info->auxparts[i]);

					if (auxpart != nullptr)
						power_slot += auxpart->c[0]; // Power
				}
			}

			// Card -- Card só da slot

			// Pega o valor máximo de slot de POWER
			int32_t value = getCharacterMaxSlot(CharacterInfo::Stats::S_POWER);

			if (value != -1 && ei.char_info->pcl[0] > value)
				power_slot += value;
			else
				power_slot += ei.char_info->pcl[0];
		}
	}

	// Card Special
	for (auto it = v_cei.begin(); it != v_cei.end(); ++it) {

		if (it->parts_id == 0 && it->parts_typeid == 0 && sIff::getInstance().getItemSubGroupIdentify22(it->_typeid) == 2/*Special*/ && it->efeito == 5)
			power_slot += it->efeito_qntd;
	}

	if (ei.clubset != nullptr && ei.clubset->_typeid == ei.csi._typeid) {
		
		auto clubset = sIff::getInstance().findClubSet(ei.clubset->_typeid);

		// Base
		if (clubset != nullptr)
			power_slot += clubset->c[0];

		// Pega o valor máximo de slot de POWER
		int32_t value = getClubSetMaxSlot(CharacterInfo::Stats::S_POWER);

		if (value != -1 && ei.csi.slot_c[0] > value)
			power_slot += value;
		else
			power_slot += ei.csi.slot_c[0]; // Power
	}

	return power_slot;
};

int32_t PlayerInfo::getCharacterMaxSlot(CharacterInfo::Stats _stats) {

	// pega o número máximo de slot de power do character equipado
	int32_t value = 0;

	if (ei.char_info == nullptr) {

		_smp::message_pool::getInstance().push(new message("[PlayerInfo::getCharacterMaxSlotPower][Error][WARNING] Player[UID=" + std::to_string(uid) 
				+ "] nao tem nenhum character equipado.", CL_FILE_LOG_AND_CONSOLE));

		return -1;
	}

	auto value_part = ei.char_info->getSlotOfStatsFromCharEquipedPartItem(_stats);
	auto value_auxpart = ei.char_info->getSlotOfStatsFromCharEquipedAuxPart(_stats);
	auto value_set_effect_table = ei.char_info->getSlotOfStatsFromSetEffectTable(_stats);
	auto value_card = ei.char_info->getSlotOfStatsFromCharEquipedCard(_stats);

	if (value_part == -1 || value_card == -1 || value_auxpart == -1 || value_set_effect_table == -1) {
		
		_smp::message_pool::getInstance().push(new message("[PlayerInfo::getCharacterMaxSlotPower][Error][WARNING] player[UID="
				+ std::to_string(uid) + "], value of slots stat[value=" + std::to_string(_stats) + "] is invalid. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

		return -1;
	}

	// Slot de Part Equiped
	value += value_part;

	// Slot de AuxPart Equiped
	value += value_auxpart;

	// Slot do Set Effect Table
	value += value_set_effect_table;

	// Slot de Card Equiped
	value += value_card;

	// Level + POWER, cada level da +1 de POWER
	if (_stats == CharacterInfo::Stats::S_POWER)
		value += ((mi.level - 1/*Rookie tem uma letra a+*/) / 5/*Levels*/);

	auto mastery = sIff::getInstance().findCharacterMastery(ei.char_info->_typeid);

	if (mastery.empty()) {
		
		_smp::message_pool::getInstance().push(new message("[PlayerInfo::getSlotPower][Error][WARNING] player[UID=" + std::to_string(uid) 
				+ "] tentou pegar os slots stat[value=" + std::to_string(_stats) + "] do Character[TYPEID=" + std::to_string(ei.char_info->_typeid) + ", ID="
				+ std::to_string(ei.char_info->id) + "], mas nao tem o Character Mastery no IFF_STRUCT do server. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

		return -1;
	}

	if (mastery.size() < (uint32_t)ei.char_info->mastery) {
		
		_smp::message_pool::getInstance().push(new message("[PlayerInfo::getSlotPower][Error][WARNING] player[UID=" + std::to_string(uid) 
				+ "] tentou pegar os slots stat[value=" + std::to_string(_stats) + "] do Character[TYPEID=" + std::to_string(ei.char_info->_typeid) 
				+ ", ID=" + std::to_string(ei.char_info->id) + "], mas o CharacterMastery[value=" + std::to_string(ei.char_info->mastery) 
				+ ", vector_size=" + std::to_string(mastery.size()) + "] do player e invalido. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));

		return -1;
	}

	// Character Mastery
	for (auto i = 0u; i < (uint32_t)ei.char_info->mastery; ++i)
		if ((mastery[i].stats - 1) == _stats)
			value++;

	return value;
};

int32_t PlayerInfo::getClubSetMaxSlot(CharacterInfo::Stats _stats) {

	int32_t value = 0;

	if (ei.clubset == nullptr || ei.clubset->_typeid != ei.csi._typeid) {

		_smp::message_pool::getInstance().push(new message("[PlayerInfo::getClubSetMaxSlotPower][Error][WARNING] Player[UID=" + std::to_string(uid) 
				+ "] nao tem o clubset equipado ou o ClubSet Info nao esta inicializado para o clubset equipado.", CL_FILE_LOG_AND_CONSOLE));

		return -1;
	}

	auto clubset = sIff::getInstance().findClubSet(ei.clubset->_typeid);

	if (clubset == nullptr) {

		_smp::message_pool::getInstance().push(new message("[PlayerInfo::getClubSetMaxSlotPower][Error][WARNING] Player[UID=" + std::to_string(uid) 
				+ "] nao tem o ClubSet[TYPEID=" + std::to_string(ei.clubset->_typeid) + "] no IFF_STRUCT do server.", CL_FILE_LOG_AND_CONSOLE));

		return -1;
	}
		
	value = (clubset->slot[_stats] - clubset->c[_stats]) + ei.clubset->clubset_workshop.c[_stats];

	return value;
};

PlayerRoomInfo::uItemBoost PlayerInfo::checkEquipedItemBoost() {
	
	PlayerRoomInfo::uItemBoost ib{ 0 };

	// Pang
	for (auto& _el : mp_wi) {

		// Pang Boost X2
		// Verifica a quantidade do item para gastar menos processo se ele não tiver a quantidade necessária para ativar a flag
		if (_el.second.STDA_C_ITEM_QNTD > 0 && std::find(passive_item_pang_x2, LAST_ELEMENT_IN_ARRAY(passive_item_pang_x2), _el.second._typeid) != LAST_ELEMENT_IN_ARRAY(passive_item_pang_x2))
			ib.stItemBoost.ucPangMastery = 1u;

		// Pang Boost X4
		// Verifica a quantidade do item para gastar menos processo se ele não tiver a quantidade necessária para ativar a flag
		if (_el.second.STDA_C_ITEM_QNTD > 0 && std::find(passive_item_pang_x4, LAST_ELEMENT_IN_ARRAY(passive_item_pang_x4), _el.second._typeid) != LAST_ELEMENT_IN_ARRAY(passive_item_pang_x4))
			ib.stItemBoost.ucPangNitro = 1u;

		// Tenta não consumir mais processo, quando já estiver as duas flag setada.
		// Tentando verificar outros itens que possa ter ainda no map
		if (ib.stItemBoost.ucPangMastery == 1u && ib.stItemBoost.ucPangNitro == 1u)
			break;
	}

	return ib;
}

bool PlayerInfo::isMasterCourse() {

	unsigned char clear_course[MS_NUM_MAPS]{ 0 };

	unsigned int  i = 0u;

	for (i = 0u; i < MS_NUM_MAPS; ++i)
		clear_course[i] |= a_ms_normal[i].isRecorded();

	for (i = 0u; i < MS_NUM_MAPS; ++i)
		clear_course[i] |= a_ms_natural[i].isRecorded();

	for (i = 0u; i < MS_NUM_MAPS; ++i)
		clear_course[i] |= a_ms_grand_prix[i].isRecorded();

	auto count = std::count_if(clear_course, LAST_ELEMENT_IN_ARRAY(clear_course), [](auto& el) {
		return el == 1u;
	});

	return count == (MS_NUM_MAPS - 2)/*-2 por que tira o map 12 que nunca foi feito e o 17 que é o SSC*/;
}

bool PlayerInfo::checkEquipedItem(uint32_t _typeid) {
	
	if (_typeid == 0u)
		return false;

	bool ret = false;

	switch (sIff::getInstance().getItemGroupIdentify(_typeid)) {
	default:
		
		_smp::message_pool::getInstance().push(new message("[PlayerInfo::checkEquipedItem][WARNING] Player[UID=" + std::to_string(uid) 
				+ "] tentou verificar item[TYPEID=" + std::to_string(_typeid) + "] equipado, mas esse nao eh um item equipado valido. Hacker ou Bug", CL_FILE_LOG_AND_CONSOLE));
		
		ret = false;
		
		break;
	case iff::CHARACTER:
		if (ei.char_info != nullptr && ei.char_info->_typeid == _typeid)
			ret = true;
		break;
	case iff::BALL:
		if (ei.comet != nullptr && ei.comet->_typeid == _typeid)
			ret = true;
		break;
	case iff::CADDIE:
		if (ei.cad_info != nullptr && ei.cad_info->_typeid == _typeid)
			ret = true;
		break;
	case iff::CLUBSET:
		if (ei.clubset != nullptr && ei.clubset->_typeid == _typeid)
			ret = true;
		break;
	case iff::MASCOT:
		if (ei.mascot_info != nullptr && ei.mascot_info->_typeid == _typeid)
			ret = true;
		break;
	}

	return ret;
}

int32_t PlayerInfo::getSumRecordGrandPrix() {

	int32_t grand_prix_record_sum = 0u;

	for (auto& el : a_ms_grand_prix)
		if (el.isRecorded())
			grand_prix_record_sum += el.best_score;

	return grand_prix_record_sum;
}

bool PlayerInfo::isFriend(uint32_t _uid) {

	if (_uid == 0u) {
		_smp::message_pool::getInstance().push(new message("[PlayerInfo::isFriend][Error] _uid is invalid(0)", CL_FILE_LOG_AND_CONSOLE));

		return false;
	}

	auto it = mp_fi.find(_uid);

	return (it != mp_fi.end()) ? true : false;
}

bool PlayerInfo::ownerCaddieItem(uint32_t _typeid) {

	auto cad = findCaddieByTypeid((iff::CADDIE << 26) | sIff::getInstance().getCaddieIdentify(_typeid));

	// Se não tiver o caddie não pode ter o caddie item(parts caddie)
	// Verificar se tem o caddie, o caddie item não precisa
	if (cad == nullptr /*|| cad->parts_typeid != _typeid*/)
		return true;

	return false;
};

bool PlayerInfo::ownerHairStyle(uint32_t _typeid) {
	auto hair = sIff::getInstance().findHairStyle(_typeid);

	if (hair != nullptr) {
		auto character = findCharacterByTypeid((iff::CHARACTER << 26) | hair->character);

		if (character != nullptr && character->default_hair == hair->cor)
			return true;
	}

	return false;
};

bool PlayerInfo::ownerSetItem(uint32_t _typeid) {
	auto set = sIff::getInstance().findSetItem(_typeid);

	if (set != nullptr) {
		for (auto i = 0u; i < (sizeof(set->packege.item_typeid) / sizeof(set->packege.item_typeid[0])); ++i) {
			// Eleminar a verificação do character que ele só inclui se o player não tiver ele
			// se ele tiver não faz diferença não anula o verificação do set
			if (set->packege.item_typeid[i] != 0 && sIff::getInstance().getItemGroupIdentify(set->packege.item_typeid[i]) != iff::CHARACTER)
				if (ownerItem(set->packege.item_typeid[i]))	// se tiver 1 item que seja não pode ganhar o set se não vai duplicar os itens, que ele tem
					return true;
		}
	}

	return false;
};

bool PlayerInfo::ownerItem(uint32_t _typeid, int option) {
	bool ret = false;

	// Verifica se ele tem no Dolfini Locker
	if (df.ownerItem(_typeid))
		return true;

	switch (sIff::getInstance().getItemGroupIdentify(_typeid)) {
	case iff::CHARACTER:
		if (findCharacterByTypeid(_typeid) != nullptr)
			ret = true;
		break;
	case iff::CADDIE:
		if (findCaddieByTypeid(_typeid) != nullptr)
			ret = true;
		break;
	case iff::MASCOT:
		if (findMascotByTypeid(_typeid) != nullptr)
			ret = true;
		break;
	case iff::CARD:
		if (findCardByTypeid(_typeid) != nullptr)
			ret = true;
		break;
	case iff::FURNITURE:
		if (findMyRoomItemByTypeid(_typeid) != nullptr)
			ret = true;
		break;
	case iff::BALL:
	case iff::AUX_PART:
	case iff::CLUBSET:
	case iff::ITEM:
	case iff::PART:
	case iff::SKIN:
		if (findWarehouseItemByTypeid(_typeid) != nullptr)
			ret = true;
		break;
	case iff::SET_ITEM:
		ret = ownerSetItem(_typeid);
		break;
	case iff::HAIR_STYLE:
		ret = ownerHairStyle(_typeid);
		break;
	case iff::CAD_ITEM:		// Esse aqui verifica se já tem, mas não que não pode ter mais. mas sim para aumentar o tempo
		ret = ownerCaddieItem(_typeid);
		break;
	}

	// Player não tem o item no warehouse e nem no Dolfini Locker, Verifica no Mail Box dele
	// Option diferente de 0 não verifica no Mail Box, por que o player está tirando do Mail Box o Item
	if (option == 0 && !ret) {
	
		// Verifica se ele tem no Mail Box
		ret = ownerMailBoxItem(_typeid);
	}

	return ret;
};

bool PlayerInfo::ownerMailBoxItem(unsigned _typeid) {

	CmdFindMailBoxItem cmd_fmbi(uid, _typeid, true);	// Waiter

	snmdb::NormalManagerDB::getInstance().add(0, &cmd_fmbi, nullptr, nullptr);

	cmd_fmbi.waitEvent();

	if (cmd_fmbi.getException().getCodeError() != 0)
		throw cmd_fmbi.getException();

	if (cmd_fmbi.hasFound())
		return true;

	return false;
};

bool PlayerInfo::isPartEquiped(uint32_t _typeid, int32_t _id) {
	
	auto it = std::find_if(mp_ce.begin(), mp_ce.end(), [&](auto& el) {
		return el.second.isPartEquiped(_typeid, _id);
	});

	return it != mp_ce.end();
}

bool PlayerInfo::isAuxPartEquiped(uint32_t _typeid) {
	
	auto it = std::find_if(mp_ce.begin(), mp_ce.end(), [&](auto& el) {
		return el.second.isAuxPartEquiped(_typeid);
	});

	return it != mp_ce.end();
};

void PlayerInfo::consomeMoeda(uint64_t _pang, uint64_t _cookie) {
	
	if (_pang > 0)
		consomePang(_pang);
	
	if (_cookie > 0)
		consomeCookie(_cookie);
};

void PlayerInfo::consomeCookie(uint64_t _cookie) {
	
	if ((int64_t)_cookie <= 0)
		throw exception("[PlayerInfo::consomeCookie][Error] _cookie valor invalido: " + std::to_string((int64_t)_cookie), STDA_MAKE_ERROR(STDA_ERROR_TYPE::PLAYER_INFO, 21, 0));
	
	try {

#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		// Check alteration on cookie of DB
		if (checkAlterationCookieOnDB())
			throw exception("[PlayerInfo::consomeCookie][Error] Player[UID=" + std::to_string(uid) + "] cookie on db is different of server.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PLAYER_INFO, 200, 0));

		if (((int64_t)cookie - _cookie) < 0)
			throw exception("[PlayerInfo::consomeCookie][Error] O Player[UID=" + std::to_string(uid) + "] nao tem cookies suficiente para consumir", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PLAYER_INFO, 20, 0));

		cookie -= _cookie;

		m_update_cookie_db.requestUpdateOnDB();

		snmdb::NormalManagerDB::getInstance().add(2, new CmdUpdateCookie(uid, _cookie, CmdUpdateCookie::DECREASE), PlayerInfo::SQLDBResponse, this);

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

	}catch (exception& e) {

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		_smp::message_pool::getInstance().push(new message("[PlayerInfo::consomeCookie][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		throw;
	}

#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[PlayerInfo::consomeCookie][Log] Player: " + std::to_string(uid) + ", gastou " + std::to_string(_cookie) + " e ficou com " + std::to_string(cookie) + " Cookie(s).", CL_FILE_LOG_AND_CONSOLE));
#else
	_smp::message_pool::getInstance().push(new message("[PlayerInfo::consomeCookie][Log] Player: " + std::to_string(uid) + ", gastou " + std::to_string(_cookie) + " e ficou com " + std::to_string(cookie) + " Cookie(s).", CL_ONLY_FILE_LOG));
#endif
};

void PlayerInfo::consomePang(uint64_t _pang) {
	
	if ((int64_t)_pang <= 0)
		throw exception("[PlayerInfo::consomePang][Error] _pang valor invalido: " + std::to_string((int64_t)_pang), STDA_MAKE_ERROR(STDA_ERROR_TYPE::PLAYER_INFO, 21, 0));

	try {
		
#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		// Check alteration on pang of DB
		if (checkAlterationPangOnDB())
			throw exception("[PlayerInfo::consomePang][Error] Player[UID=" + std::to_string(uid) + "] pang on db is different of server.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PLAYER_INFO, 200, 0));

		if (((int64_t)ui.pang - _pang) < 0)
			throw exception("[PlayerInfo::consomePang][Error] O Player[UID=" + std::to_string(uid) + "] nao tem pangs suficiente para consumir", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PLAYER_INFO, 20, 0));

		ui.pang -= _pang;

		m_update_pang_db.requestUpdateOnDB();

		snmdb::NormalManagerDB::getInstance().add(1, new CmdUpdatePang(uid, _pang, CmdUpdatePang::DECREASE), PlayerInfo::SQLDBResponse, this);

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

	}catch (exception& e) {

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		_smp::message_pool::getInstance().push(new message("[PlayerInfo::consomePang][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		throw;
	}

#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[PlayerInfo::consomePang][Log] Player: " + std::to_string(uid) + ", gastou " + std::to_string(_pang) + " e ficou com " + std::to_string(ui.pang) + " Pang(s).", CL_FILE_LOG_AND_CONSOLE));
#else
	_smp::message_pool::getInstance().push(new message("[PlayerInfo::consomePang][Log] Player: " + std::to_string(uid) + ", gastou " + std::to_string(_pang) + " e ficou com " + std::to_string(ui.pang) + " Pang(s).", CL_ONLY_FILE_LOG));
#endif
};

void PlayerInfo::addMoeda(uint64_t _pang, uint64_t _cookie) {

	if (_pang > 0)
		addPang(_pang);

	if (_cookie > 0)
		addCookie(_cookie);
};

void PlayerInfo::addCookie(uint64_t _cookie) {

	if ((int64_t)_cookie <= 0)
		throw exception("[PlayerInfo::addCookie][Error] _cookie valor invalido: " + std::to_string((int64_t)_cookie), STDA_MAKE_ERROR(STDA_ERROR_TYPE::PLAYER_INFO, 21, 0));

	try {

#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		// Check alteration on cookie of DB 
		if (checkAlterationCookieOnDB())
			throw exception("[PlayerInfo::addCookie][Error] Player[UID=" + std::to_string(uid) + "] cookie on db is different of server.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PLAYER_INFO, 200, 0));

		cookie += _cookie;

		m_update_cookie_db.requestUpdateOnDB();

		snmdb::NormalManagerDB::getInstance().add(2, new CmdUpdateCookie(uid, _cookie, CmdUpdateCookie::INCREASE), PlayerInfo::SQLDBResponse, this);

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

	}catch (exception& e) {

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		_smp::message_pool::getInstance().push(new message("[PlayerInfo::addCookie][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		throw;
	}

#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[PlayerInfo::addCookie][Log] Player: " + std::to_string(uid) + ", ganhou " + std::to_string(_cookie) + " e ficou com " + std::to_string(cookie) + " Cookie(s).", CL_FILE_LOG_AND_CONSOLE));
#else
	_smp::message_pool::getInstance().push(new message("[PlayerInfo::addCookie][Log] Player: " + std::to_string(uid) + ", ganhou " + std::to_string(_cookie) + " e ficou com " + std::to_string(cookie) + " Cookie(s).", CL_ONLY_FILE_LOG));
#endif
};

void PlayerInfo::addPang(uint64_t _pang) {

	if ((int64_t)_pang <= 0)
		throw exception("[PlayerInfo::addPang][Error] _pang valor invalido: " + std::to_string((int64_t)_pang), STDA_MAKE_ERROR(STDA_ERROR_TYPE::PLAYER_INFO, 21, 0));

	try {

#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		// Check alteration on pang of DB 
		if (checkAlterationPangOnDB()) {

			// Pang é diferente atualiza o pang com o valor do banco de daos
			_smp::message_pool::getInstance().push(new message("[PlayerInfo::addPang][Error] Player[UID=" + std::to_string(uid) + "] pang on db is different of server.", CL_FILE_LOG_AND_CONSOLE));

			auto old_pang = ui.pang;

			// Atualiza o valor do pang do server com o do banco de dados
			updatePang();

			// Log
			_smp::message_pool::getInstance().push(new message("[PlayerInfo::addPang][Log] Player[UID=" + std::to_string(uid) 
					+ "] o Pang[DB=" + std::to_string(ui.pang) + ", GS=" + std::to_string(old_pang) 
					+ "] no banco de dados eh diferente do que esta no server, atualiza para o valor do banco de dados.", CL_FILE_LOG_AND_CONSOLE));
		}

		// Add o pang para o player
		ui.pang += _pang;

		m_update_pang_db.requestUpdateOnDB();

		snmdb::NormalManagerDB::getInstance().add(1, new CmdUpdatePang(uid, _pang, CmdUpdatePang::INCREASE), PlayerInfo::SQLDBResponse, this);

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif
	
	}catch (exception& e) {

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		_smp::message_pool::getInstance().push(new message("[PlayerInfo::addPang][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		throw;
	}

#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[PlayerInfo::addPang][Log] Player: " + std::to_string(uid) + ", ganhou " + std::to_string(_pang) + " e ficou com " + std::to_string(ui.pang) + " Pang(s).", CL_FILE_LOG_AND_CONSOLE));
#else
	_smp::message_pool::getInstance().push(new message("[PlayerInfo::addPang][Log] Player: " + std::to_string(uid) + ", ganhou " + std::to_string(_pang) + " e ficou com " + std::to_string(ui.pang) + " Pang(s).", CL_ONLY_FILE_LOG));
#endif
};

void PlayerInfo::updateMoeda() {

	// Update Cookie
	updateCookie();

	// Update Pang
	updatePang();

};

void PlayerInfo::updateCookie() {

	try {

		CmdCookie cmd_cp(uid, true);	// Waiter

		snmdb::NormalManagerDB::getInstance().add(0, &cmd_cp, nullptr, nullptr);

		cmd_cp.waitEvent();

		if (cmd_cp.getException().getCodeError() != 0)
			throw cmd_cp.getException();

		cookie = cmd_cp.getCookie();

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[PlayerInfo::updateCookie][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		// Relanção por que essa função não tem retorno para verifica, então a exception garante que o código não vai continua
		throw;
	}
};

void PlayerInfo::updatePang() {

	try {

		CmdPang cmd_pang(uid, true);	// Waiter

		snmdb::NormalManagerDB::getInstance().add(0, &cmd_pang, nullptr, nullptr);

		cmd_pang.waitEvent();

		if (cmd_pang.getException().getCodeError() != 0)
			throw cmd_pang.getException();

		ui.pang = cmd_pang.getPang();

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[PlayerInfo::updatePang][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		// Relanção por que essa função não tem retorno para verifica, então a exception garante que o código não vai continua
		throw;
	}
};

void PlayerInfo::addPang(uint32_t _uid, uint64_t _pang) {

	if ((int64_t)_pang <= 0)
		throw exception("[PlayerInfo::addPang][Error] _pang valor invalido: " + std::to_string((int64_t)_pang), STDA_MAKE_ERROR(STDA_ERROR_TYPE::PLAYER_INFO, 21, 0));

	snmdb::NormalManagerDB::getInstance().add(1, new CmdUpdatePang(_uid, _pang, CmdUpdatePang::INCREASE), PlayerInfo::SQLDBResponse, nullptr);

#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[PlayerInfo::addPang][Log] Player: " + std::to_string(_uid) + ", ganhou " + std::to_string(_pang) + " Pang(s).", CL_FILE_LOG_AND_CONSOLE));
#else
	_smp::message_pool::getInstance().push(new message("[PlayerInfo::addPang][Log] Player: " + std::to_string(_uid) + ", ganhou " + std::to_string(_pang) + " Pang(s).", CL_ONLY_FILE_LOG));
#endif
};

void PlayerInfo::addCookie(uint32_t _uid, uint64_t _cookie) {

	if ((int64_t)_cookie <= 0)
		throw exception("[PlayerInfo::addCookie][Error] _cookie valor invalido: " + std::to_string((int64_t)_cookie), STDA_MAKE_ERROR(STDA_ERROR_TYPE::PLAYER_INFO, 21, 0));

	snmdb::NormalManagerDB::getInstance().add(2, new CmdUpdateCookie(_uid, _cookie, CmdUpdateCookie::INCREASE), PlayerInfo::SQLDBResponse, nullptr);

#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[PlayerInfo::addCookie][Log] Player: " + std::to_string(_uid) + ", ganhou " + std::to_string(_cookie) + " Cookie Point(s).", CL_FILE_LOG_AND_CONSOLE));
#else
	_smp::message_pool::getInstance().push(new message("[PlayerInfo::addCookie][Log] Player: " + std::to_string(_uid) + ", ganhou " + std::to_string(_cookie) + " Cookie Point(s).", CL_ONLY_FILE_LOG));
#endif
};

void PlayerInfo::addUserInfo(UserInfoEx& _ui, uint64_t _total_pang_win_game) {

	ui.add(_ui, _total_pang_win_game);

	// Update User Info ON DB
	updateUserInfo();

};

void PlayerInfo::updateUserInfo() {

	snmdb::NormalManagerDB::getInstance().add(3, new CmdUpdateUserInfo(uid, ui), PlayerInfo::SQLDBResponse, this);

#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[PlayerInfo::updateUserInfo][Log] Atualizou info do player[UID=" + std::to_string(uid) + "]", CL_FILE_LOG_AND_CONSOLE));
#else
	_smp::message_pool::getInstance().push(new message("[PlayerInfo::updateUserInfo][Log] Atualizou info do player[UID=" + std::to_string(uid) + "]", CL_ONLY_FILE_LOG));
#endif // _DEBUG
	
};

void PlayerInfo::updateUserInfo(uint32_t _uid, UserInfoEx& _ui) {

	if (_uid == 0)
		throw exception("[PlayerInfo::updateUserInfo][Error] _uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PLAYER_INFO, 300, 0));

	snmdb::NormalManagerDB::getInstance().add(3, new CmdUpdateUserInfo(_uid, _ui), PlayerInfo::SQLDBResponse, nullptr);

#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[PlayerInfo::updateUserInfo][Log] Atualizou info do player[UID=" + std::to_string(_uid) + "]", CL_FILE_LOG_AND_CONSOLE));
#else
	_smp::message_pool::getInstance().push(new message("[PlayerInfo::updateUserInfo][Log] Atualizou info do player[UID=" + std::to_string(_uid) + "]", CL_ONLY_FILE_LOG));
#endif // _DEBUG

};

void PlayerInfo::updateTrofelInfo(uint32_t _trofel_typeid, unsigned char _trofel_rank) {

	if (_trofel_typeid == 0u)
		throw exception("[PlayerInfo::updateTrofelInfo][Error] player[UID=" + std::to_string(uid) + "] tentou atualizar um trofel[TYPEID=" 
				+ std::to_string(_trofel_typeid) + ", RANK=" + std::to_string((unsigned short)_trofel_rank) + "] que eh invalido(zero). Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PLAYER_INFO, 200, 0));

	if (_trofel_typeid == TROFEL_GM_EVENT_TYPEID/*GM Event*/)
		throw exception("[PlayerInfo::updateTrofelInfo][Error] player[UID=" + std::to_string(uid) + "] tentou atualizar um trofel[TYPEID=" 
				+ std::to_string(_trofel_typeid) +", RANK=" + std::to_string((unsigned short)_trofel_rank) + "] que nao eh normal, eh um trofel de evento GM. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PLAYER_INFO, 201, 0));

	uint32_t type = sIff::getInstance().getMatchTypeIdentity(_trofel_typeid);

	// Verifica se é o 2C e se o Tipo do Trofel é menor ou igual a 12, que é o Pro 7 o ultimo
	if (sIff::getInstance().getItemSubGroupIdentify24(_trofel_typeid) != 0 && type > 12/*Pro 7*/)
		throw exception("[PlayerInfo::updateTrofelInfo][Error] player[UID=" + std::to_string(uid) + "] tentou atualizar um trofel[TYPEID="
				+ std::to_string(_trofel_typeid) + ", RANK=" + std::to_string((unsigned short)_trofel_rank) + "] que nao eh normal, eh um outro trofel. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PLAYER_INFO, 202, 0));

	if (_trofel_rank == 0u || _trofel_rank > 3)
		throw exception("[PlayerInfo::updateTrofelInfo][Error] player[UID=" + std::to_string(uid) + "] tentou atualizar um trofel[TYPEID="
				+ std::to_string(_trofel_typeid) + ", RANK=" + std::to_string((unsigned short)_trofel_rank) + "] rank eh invalido. Bug,", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PLAYER_INFO, 203, 0));

	// Update Trofel Info Atual (season atual)
	ti_current_season.update(type, _trofel_rank);

	snmdb::NormalManagerDB::getInstance().add(4, new CmdUpdateNormalTrofel(uid, ti_current_season), PlayerInfo::SQLDBResponse, this);
};

void PlayerInfo::updateTrofelInfo(uint32_t _uid, uint32_t _trofel_typeid, unsigned char _trofel_rank) {

	if (_uid == 0u)
		throw exception("[PlayerInfo::updateTrofelInfo][Error] player[UID=" + std::to_string(_uid) + "] tentou atualizar um trofel[TYPEID="
				+ std::to_string(_trofel_typeid) + ", RANK=" + std::to_string((unsigned short)_trofel_rank) + "], mas uid is invalid(zero). Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PLAYER_INFO, 204, 0));

	if (_trofel_typeid == 0u)
		throw exception("[PlayerInfo::updateTrofelInfo][Error] player[UID=" + std::to_string(_uid) + "] tentou atualizar um trofel[TYPEID="
				+ std::to_string(_trofel_typeid) + ", RANK=" + std::to_string((unsigned short)_trofel_rank) + "] que eh invalido(zero). Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PLAYER_INFO, 200, 0));

	if (_trofel_typeid == TROFEL_GM_EVENT_TYPEID/*GM Event*/)
		throw exception("[PlayerInfo::updateTrofelInfo][Error] player[UID=" + std::to_string(_uid) + "] tentou atualizar um trofel[TYPEID="
				+ std::to_string(_trofel_typeid) + ", RANK=" + std::to_string((unsigned short)_trofel_rank) + "] que nao eh normal, eh um trofel de evento GM. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PLAYER_INFO, 201, 0));

	uint32_t type = sIff::getInstance().getMatchTypeIdentity(_trofel_typeid);

	// Verifica se é o 2C e se o Tipo do Trofel é menor ou igual a 12, que é o Pro 7 o ultimo
	if (sIff::getInstance().getItemSubGroupIdentify24(_trofel_typeid) != 0 && type > 12/*Pro 7*/)
		throw exception("[PlayerInfo::updateTrofelInfo][Error] player[UID=" + std::to_string(_uid) + "] tentou atualizar um trofel[TYPEID="
				+ std::to_string(_trofel_typeid) + ", RANK=" + std::to_string((unsigned short)_trofel_rank) + "] que nao eh normal, eh um outro trofel. Bug", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PLAYER_INFO, 202, 0));

	if (_trofel_rank == 0u || _trofel_rank > 3)
		throw exception("[PlayerInfo::updateTrofelInfo][Error] player[UID=" + std::to_string(_uid) + "] tentou atualizar um trofel[TYPEID="
				+ std::to_string(_trofel_typeid) + ", RANK=" + std::to_string((unsigned short)_trofel_rank) + "] rank eh invalido. Bug,", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PLAYER_INFO, 203, 0));

	CmdTrofelInfo cmd_ti(_uid, CmdTrofelInfo::CURRENT, true);	// Waiter

	snmdb::NormalManagerDB::getInstance().add(0, &cmd_ti, nullptr, nullptr);

	cmd_ti.waitEvent();

	if (cmd_ti.getException().getCodeError() != 0)
		throw cmd_ti.getException();

	TrofelInfo ti = cmd_ti.getInfo();

	// Update Trofel Info Atual (season atual)
	ti.update(type, _trofel_rank);

	snmdb::NormalManagerDB::getInstance().add(4, new CmdUpdateNormalTrofel(_uid, ti), PlayerInfo::SQLDBResponse, nullptr);
};

void PlayerInfo::updateMedal(uMedalWin _medal_win) {

	if (_medal_win.ucMedal == 0u)
		throw exception("[PlayerInfo::updateMedal][Error] Player[UID=" + std::to_string(uid) 
				+ "] tentou atualizar medalhas, mas passou nenhuma medalha para atualizar. Hacker ou Bug.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PLAYER_INFO, 600, 0));

	// Update medal info player
	ui.medal.add(_medal_win);

	// Update Info do player na database
	updateUserInfo();
};

void PlayerInfo::updateMedal(uint32_t _uid, uMedalWin _medal_win) {

	if (_uid == 0u)
		throw exception("[PlayerInfo::updateMedal][Error] Player[UID=" + std::to_string(_uid) + "] tentou atualizar medalhas, mas o uid do player eh invalido(zero). Hacker ou Bug.", 
				STDA_MAKE_ERROR(STDA_ERROR_TYPE::PLAYER_INFO, 601, 0));

	if (_medal_win.ucMedal == 0u)
		throw exception("[PlayerInfo::updateMedal][Error] Player[UID=" + std::to_string(_uid)
				+ "] tentou atualizar medalhas, mas passou nenhuma medalha para atualizar. Hacker ou Bug.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PLAYER_INFO, 600, 0));

	// Pega o Info do player para atualizar
	CmdUserInfo cmd_ui(_uid, true);		// Waiter

	snmdb::NormalManagerDB::getInstance().add(0, &cmd_ui, nullptr, nullptr);

	cmd_ui.waitEvent();

	if (cmd_ui.getException().getCodeError() != 0)
		throw cmd_ui.getException();

	auto user_info = cmd_ui.getInfo();

	// Update medal info player
	user_info.medal.add(_medal_win);

	// Update Info do player na database
	PlayerInfo::updateUserInfo(_uid, user_info);

};

int PlayerInfo::addExp(uint32_t _exp) {

	if (_exp == 0)
		throw exception("[PlayerInfo::addExp][Error] _exp is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PLAYER_INFO, 21, 0));

	int ret = -1;

	uint32_t exp = (uint32_t)~0u;

	try {

#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		if ((exp = ExpByLevel[level]) == ~0u)
			_smp::message_pool::getInstance().push(new message("[AddExp][Log] player[UID=" + std::to_string(uid) + "] ja eh infinit legend I, nao precisar mais add exp para ele.", CL_FILE_LOG_AND_CONSOLE));
		else {
			// Att Exp do player
			ui.exp += _exp;

			if (ui.exp >= exp) { // LEVEL UP!
				unsigned char new_level = 0u, ant_level = 0u;
			
				// Atualiza todos os levels das estruturas que o player tem
				ant_level = (unsigned char)level;

				// Check if up n levels
				do {
					new_level = (unsigned char)++level;

					mi.level = new_level;
					ui.level = new_level;

					// Att Exp do player
					ui.exp -= exp;

					// LEVEL UP!
					ret = new_level - ant_level;

				} while ((exp = ExpByLevel[level]) != ~0u && ui.exp > exp);

				_smp::message_pool::getInstance().push(new message("[AddExp][Log] player[UID=" + std::to_string(uid) + "] Upou de Level[FROM=" + std::to_string((unsigned short)ant_level) + ", TO=" 
						+ std::to_string((unsigned short)new_level) + "]", CL_FILE_LOG_AND_CONSOLE));

			}else // Update só a Exp
				ret = 0;

			_smp::message_pool::getInstance().push(new message("[AddExp][Log] player[UID=" + std::to_string(uid) + "] adicionou Experiencia[value=" + std::to_string(_exp) + "] e ficou com [LEVEL=" 
					+ std::to_string(level) + ", EXP=" + std::to_string(ui.exp) + "]", CL_FILE_LOG_AND_CONSOLE));

			// UPDATE ON DB, LEVEL AND EXP
			snmdb::NormalManagerDB::getInstance().add(3, new CmdUpdateLevelAndExp(uid, (unsigned char)level, ui.exp), PlayerInfo::SQLDBResponse, this);
		}

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

	}catch (exception& e) {

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		_smp::message_pool::getInstance().push(new message("[PlayerInfo::addExp][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

		throw;
	}

	return ret;
};

void PlayerInfo::updateLocationDB() {

	try {
		
		m_pl.channel = channel;
		m_pl.lobby = lobby;
		m_pl.room = mi.sala_numero;
		m_pl.place = place;

		// Sincroniza para não ter valores inseridos errados no banco de dados
		m_pl.requestUpdateOnDB();

		snmdb::NormalManagerDB::getInstance().add(5, new CmdUpdatePlayerLocation(uid, m_pl), PlayerInfo::SQLDBResponse, this);

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[PlayerInfo::updateLocationDB][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}

};

bool PlayerInfo::updateGrandPrixClear(uint32_t _typeid, uint32_t _position) {

	if (_typeid == 0u)
		throw exception("[PlayerInfo::updateGrandPrixClear][Error] invliad _typeid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PLAYER_INFO, 100, 0));

	bool upt_client = false;

	auto it = std::find_if(v_gpc.begin(), v_gpc.end(), [&](auto& _el) {
		return _el._typeid == _typeid;
	});

	if (it == v_gpc.end()) {

		// Não tem esse GP o client acaba de completar um novo GP
		it = v_gpc.insert(v_gpc.end(), GrandPrixClear(_typeid, _position));

		// Insere no Banco de dados
		snmdb::NormalManagerDB::getInstance().add(6, new CmdInsertGrandPrixClear(uid, *it), PlayerInfo::SQLDBResponse, this);

		// Update no Cliente
		upt_client = true;
	
	}else {

		// Player já tem esse Grand Prix Clear verifica se ele ficou em um posição melhor
		if (it->position > _position) {

			// Player ficou em uma position melhor atualiza o Grand Prix Clear
			it->position = _position;

			// Update on DB
			snmdb::NormalManagerDB::getInstance().add(7, new CmdUpdateGrandPrixClear(uid, *it), PlayerInfo::SQLDBResponse, this);

			// Update on Cliente
			upt_client = true;
		}
	}

	return upt_client;
};

void PlayerInfo::addGrandZodiacPontos(uint64_t _pontos) {

	if ((int64_t)_pontos < 0ll)
		throw exception("[PlayerInfo::addGrandZodiacPontos][Error] invliad _pontos(" + std::to_string(_pontos) + "), ele eh negativo.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::PLAYER_INFO, 101, 0));

	grand_zodiac_pontos += _pontos;

	// Update no Banco de dados
	snmdb::NormalManagerDB::getInstance().add(8, new CmdGrandZodiacPontos(uid, grand_zodiac_pontos, CmdGrandZodiacPontos::eCMD_GRAND_ZODIAC_TYPE::CGZT_UPDATE), PlayerInfo::SQLDBResponse, this);

	// Log
	_smp::message_pool::getInstance().push(new message("[PlayerInfo::addGrandZodiacPontos][Log] Player[UID=" + std::to_string(uid) 
			+ "] add " + std::to_string(_pontos) + " pontos do grand zodiac e ficou com " + std::to_string(grand_zodiac_pontos), CL_FILE_LOG_AND_CONSOLE));
};

void PlayerInfo::SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg) {

	try {

		if (_arg == nullptr) {
			_smp::message_pool::getInstance().push(new message("[PlayerInfo::SQLDBResponse][WARNING] _arg is nullptr na msg_id = " + std::to_string(_msg_id), CL_FILE_LOG_AND_CONSOLE));
			return;
		}

		auto pi = reinterpret_cast< PlayerInfo* >(_arg);

		// Por Hora só sai, depois faço outro tipo de tratamento se precisar
		if (_pangya_db.getException().getCodeError() != 0) {

			// Trata alguns tipo aqui, que são necessários
			switch (_msg_id) {
				case 1: // Update Pang
				{
					// Error at update on DB
					pi->m_update_pang_db.errorUpdateOnDB();

					break;
				}
				case 2: // Update Cookie
				{
					// Error at update on DB
					pi->m_update_cookie_db.errorUpdateOnDB();

					break;
				}
				case 5: // Update Location Player on DB
				{
					// Error at update on DB
					pi->m_pl.errorUpdateOnDB();

					break;
				}
			}

			_smp::message_pool::getInstance().push(new message("[PlayerInfo::SQLDBResponse][Error] " + _pangya_db.getException().getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));

			return;
		}

		switch (_msg_id) {
		case 1:	// UPDATE pang
		{

			// Success update on DB
			pi->m_update_pang_db.confirmUpdadeOnDB();

			// Não tem retorno então não precisa reinterpretar o pangya_db
			//auto cmd_up = reinterpret_cast< CmdUpdatePang* >(&_pangya_db);
			break;
		}
		case 2:	// UPDATE cookie
		{

			// Success update on DB
			pi->m_update_cookie_db.confirmUpdadeOnDB();

			// Não tem retorno então não precisa reinterpretar o pangya_db
			//auto cmd_uc = reinterpret_cast< CmdUpdateCookie* >(&_pangya_db);
			break;
		}
		case 3:	// UPDATE USER INFO
		{
			// Não tem retorno então não precisa reinterpretar o pangya_db
			// auto cmd_uui = reinterpret_cast< CmdUpdateUserInfo* >(&_pangya_db);
			break;
		}
		case 4:	// Update Normal Trofel Info
		{
			break;
		}
		case 5: // Update Location Player on DB
		{
			// Success update on DB
			pi->m_pl.confirmUpdadeOnDB();

	#ifdef _DEBUG
			auto cmd_upl = reinterpret_cast< CmdUpdatePlayerLocation* >(&_pangya_db);

			_smp::message_pool::getInstance().push(new message("[PlayerInfo::SQLDBResponse][Log] Player[UID=" + std::to_string(cmd_upl->getUID())
					+ "] Atualizou sua Localizacao[CHANNEL=" + std::to_string(cmd_upl->getInfo().channel) + ", LOBBY=" + std::to_string((short)cmd_upl->getInfo().lobby)
					+ ", ROOM=" + std::to_string(cmd_upl->getInfo().room) + ", PLACE=" + std::to_string(cmd_upl->getInfo().place) + "]", CL_FILE_LOG_AND_CONSOLE));
	#endif // _DEBUG
			break;
		}
		case 6: // Insert Grand Prix Clear
		{

			auto cmd_igpc = reinterpret_cast< CmdInsertGrandPrixClear* >(&_pangya_db);

#ifdef _DEBUG
			_smp::message_pool::getInstance().push(new message("[PlayerInfo::SQLDBResponse][Log] Player[UID=" + std::to_string(cmd_igpc->getUID()) 
					+ "] Inseriu Grand Prix Clear[TYPEID=" + std::to_string(cmd_igpc->getInfo()._typeid) 
					+ ", POSITION=" + std::to_string(cmd_igpc->getInfo().position) + "] novo com sucesso.", CL_FILE_LOG_AND_CONSOLE));
#else
			_smp::message_pool::getInstance().push(new message("[PlayerInfo::SQLDBResponse][Log] Player[UID=" + std::to_string(cmd_igpc->getUID())
					+ "] Inseriu Grand Prix Clear[TYPEID=" + std::to_string(cmd_igpc->getInfo()._typeid)
					+ ", POSITION=" + std::to_string(cmd_igpc->getInfo().position) + "] novo com sucesso.", CL_ONLY_FILE_LOG));
#endif // _DEBUG

			break;
		}
		case 7: // Update Grand Prix Clear
		{
			auto cmd_ugpc = reinterpret_cast< CmdUpdateGrandPrixClear* >(&_pangya_db);

#ifdef _DEBUG
			_smp::message_pool::getInstance().push(new message("[PlayerInfo::SQLDBResponse][Log] Player[UID=" + std::to_string(cmd_ugpc->getUID())
					+ "] Atualizou Grand Prix Clear[TYPEID=" + std::to_string(cmd_ugpc->getInfo()._typeid)
					+ ", POSITION=" + std::to_string(cmd_ugpc->getInfo().position) + "] com sucesso.", CL_FILE_LOG_AND_CONSOLE));
#else
			_smp::message_pool::getInstance().push(new message("[PlayerInfo::SQLDBResponse][Log] Player[UID=" + std::to_string(cmd_ugpc->getUID())
					+ "] Atualizou Grand Prix Clear[TYPEID=" + std::to_string(cmd_ugpc->getInfo()._typeid)
					+ ", POSITION=" + std::to_string(cmd_ugpc->getInfo().position) + "] com sucesso.", CL_ONLY_FILE_LOG));
#endif // _DEBUG

			break;
		}
		case 8: // Update Grand Zodiac Pontos
		{
			auto cmd_gzp = reinterpret_cast< CmdGrandZodiacPontos* >(&_pangya_db);

#ifdef _DEBUG
			_smp::message_pool::getInstance().push(new message("[PlayerInfo::SQLDBResponse][Log] Player[UID=" + std::to_string(cmd_gzp->getUID())
					+ "] Atualizou Grand Zodiac Pontos[PONTOS=" + std::to_string(cmd_gzp->getPontos()) + "] com sucesso.", CL_FILE_LOG_AND_CONSOLE));
#else
			_smp::message_pool::getInstance().push(new message("[PlayerInfo::SQLDBResponse][Log] Player[UID=" + std::to_string(cmd_gzp->getUID())
					+ "] Atualizou Grand Zodiac Pontos[PONTOS=" + std::to_string(cmd_gzp->getPontos()) + "] com sucesso.", CL_ONLY_FILE_LOG));
#endif // _DEBUG

			break;
		}
		case 0:
		default:
			break;
		}

	}catch (exception& e) {

		_smp::message_pool::getInstance().push(new message("[PlayerInfo::SQLDBResponse][Error] QUERY_MSG[ID=" + std::to_string(_msg_id)
				+ "]" + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

bool PlayerInfo::checkAlterationCookieOnDB() {

	CmdCookie cmd_cp(uid, true);	// Waiter

	snmdb::NormalManagerDB::getInstance().add(0, &cmd_cp, nullptr, nullptr);

	cmd_cp.waitEvent();

	if (cmd_cp.getException().getCodeError() != 0)
		throw cmd_cp.getException();

	return (cmd_cp.getCookie() != cookie);
};

bool PlayerInfo::checkAlterationPangOnDB() {

	CmdPang cmd_pang(uid, true);	// Waiter

	snmdb::NormalManagerDB::getInstance().add(0, &cmd_pang, nullptr, nullptr);

	cmd_pang.waitEvent();

	if (cmd_pang.getException().getCodeError() != 0)
		throw cmd_pang.getException();

	return (cmd_pang.getPang() != ui.pang);
};

PlayerInfo::stLocation::stLocation() {
	clear();
};

PlayerInfo::stLocation::stLocation(float _x, float _z, float _r) {
	x = _x;
	y = 0.f;
	z = _z;
	r = _r;
};

void PlayerInfo::stLocation::clear() {
	memset(this, 0, sizeof(stLocation));
};

PlayerInfo::stLocation& PlayerInfo::stLocation::operator+=(stLocation& _add_location) {

	x += _add_location.x;
	y += _add_location.y;
	z += _add_location.z;
	r += _add_location.r;

	return *this;
};
