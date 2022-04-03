// Arquivo iff.cpp
// Criado em 03/10/2017 por Acrisio
// Implementação da classe iff

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "iff.h"
#include "exception.h"

#include <cstdint>

#if defined(_WIN32)
	#if INTPTR_MAX == INT64_MAX
		#include "../ZIP/x64/zip.h"
	#elif INTPTR_MAX == INT32_MAX
		#include "../ZIP/zip.h"
	#else
		#error Unknown pointer size or missing size macros!
	#endif
#elif defined(__linux__)
	#include <zip.h>
#endif

#include "../TYPE/stda_error.h"

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include "WinPort.h"
#endif

#include "../UTIL/message_pool.h"

#if INTPTR_MAX == INT64_MAX
	#define ZIP_GET_REAL_SIZE(_size) (_size)
#elif INTPTR_MAX == INT32_MAX
	#define ZIP_GET_REAL_SIZE(_size) (((_size) & 0x00000000FFFFFFFF) << 32 | (((_size) & 0xFFFFFFFF00000000) >> 32))
#else
	#error Unknown pointer size or missing size macros!
#endif

#define MAKE_UNZIP_VECTOR(type, method, iff_name) std::vector< type > v_iff; \
		\
		int error; \
		\
		zip *z = zip_open(PATH_PANGYA_IFF, 0, &error); \
		if (z != nullptr) { \
			size_t i = 0; \
			uint64_t index, num_entries = zip_get_num_entries(z, ZIP_FL_UNCHANGED); \
			zip_stat_t st = { 0 }; \
			zip_stat_init(&st); \
			\
			for (index = 0; index < num_entries; ++index) { \
				zip_stat_index(z, index, ZIP_FL_UNCHANGED, &st); \
				\
				if (strcmp(st.name, (iff_name)) == 0) { \
					zip_file *file = zip_fopen_index(z, index, ZIP_FL_UNCHANGED); \
					if (file != nullptr && ZIP_GET_REAL_SIZE(st.size) >= sizeof(IFF::Head)) { \
						uint64_t size = 0; \
						IFF::Head head = { 0 }; \
						\
						zip_fread(file, (char*)&head, sizeof(IFF::Head)); \
						\
						if (head.version == IFF_VERSION/*13*/ && (head.count_element * sizeof(type) + sizeof(head)) == ZIP_GET_REAL_SIZE(st.size)) { \
							type _iff; \
							\
							for (i = 0; i < head.count_element; ++i) { \
								_iff.clear(); \
								\
								zip_fread(file, (char*)&_iff, sizeof(type)); \
								\
								v_iff.push_back(_iff); \
							} \
							if (head.count_element != v_iff.size()) \
								_smp::message_pool::getInstance().push(new message("[IFF::loadVector][log] iff name: " + std::string((iff_name)) + ". Nao carregou todos os elementos do IFF, Tem " + std::to_string(head.count_element) + " element(s) e foi carregado " + std::to_string(v_iff.size()), CL_FILE_LOG_AND_CONSOLE)); \
						}else if (head.version != IFF_VERSION) { \
							zip_fclose(file); \
							zip_close(z); \
							throw exception("Error a versao do arquivo IFF e diferente. Accepted " + std::to_string(IFF_VERSION) + " != " + std::to_string(head.version) + " Read from file. " + std::string((method)), STDA_MAKE_ERROR(STDA_ERROR_TYPE::_IFF, 4, 0)); \
						}else { \
							zip_fclose(file); \
							zip_close(z); \
							throw exception("Error o tamanho da estrutua IFF nao bate. size IFF: " + std::to_string(head.count_element * sizeof(type) + 8) + " size FILE ZIP IFF: " \
													+ std::to_string(ZIP_GET_REAL_SIZE(st.size)) + ". " + std::string((method)), STDA_MAKE_ERROR(STDA_ERROR_TYPE::_IFF, 5, 0)); \
						} \
						\
						zip_fclose(file); \
					}else if (file != nullptr) { \
						zip_fclose(file); \
						zip_close(z); \
						throw exception("Error nao e um arquivo IFF valido. " + std::string((method)), STDA_MAKE_ERROR(STDA_ERROR_TYPE::_IFF, 3, 0)); \
					}else { \
						zip_close(z); \
						throw exception("Error ao abrir o zip_fopen_index(). " + std::string((method)) + " Error: " + std::string(zip_strerror(z)), STDA_MAKE_ERROR(STDA_ERROR_TYPE::_IFF, 2, zip_get_error(z)->zip_err)); \
					} \
				} \
			} \
			if (v_iff.empty()) \
				_smp::message_pool::getInstance().push(new message("[E_LOAD_IFF] File " + std::string((iff_name)) + " nao encontrado ou esta vazio.", CL_FILE_LOG_AND_CONSOLE)); \
			\
			zip_close(z); \
		}else \
			throw exception("Error ao abrir o arquivo zip. " + std::string((method)), STDA_MAKE_ERROR(STDA_ERROR_TYPE::_IFF, 1, error)); \
		return v_iff; \

#define MAKE_UNZIP_MAP(key, type, member, method, iff_name) std::map< key, type > map_iff; \
\
		int error; \
\
		zip *z = zip_open(PATH_PANGYA_IFF, 0, &error); \
		if (z != nullptr) { \
			size_t i = 0; \
			uint64_t index, num_entries = zip_get_num_entries(z, ZIP_FL_UNCHANGED); \
			zip_stat_t st = { 0 }; \
			zip_stat_init(&st); \
\
			for (index = 0; index < num_entries; ++index) { \
				zip_stat_index(z, index, ZIP_FL_UNCHANGED, &st); \
\
				if (strcmp(st.name, (iff_name)) == 0) { \
					zip_file *file = zip_fopen_index(z, index, ZIP_FL_UNCHANGED); \
					if (file != nullptr && ZIP_GET_REAL_SIZE(st.size) >= sizeof(IFF::Head)) { \
						uint64_t size = 0; \
						IFF::Head head = { 0 }; \
\
						zip_fread(file, (char*)&head, sizeof(IFF::Head)); \
\
						if (head.version == IFF_VERSION/*13*/ && (head.count_element * sizeof(type) + sizeof(head)) == ZIP_GET_REAL_SIZE(st.size)) { \
							type _iff; \
							\
							for (i = 0; i < head.count_element; ++i) { \
								_iff.clear(); \
								\
								zip_fread(file, (char*)&_iff, sizeof(type)); \
								if (_iff.member > 0 || (_iff.member == 0 && map_iff.find(_iff.member) == map_iff.end())) { \
									if (map_iff.find(_iff.member) != map_iff.end()) \
										_smp::message_pool::getInstance().push(new message("[IFF::loadMap][Log] iff name: " + std::string((iff_name)) + " ja tem esse key[typeid=" + std::to_string(_iff.member) + "] no map." , CL_ONLY_FILE_LOG)); \
									map_iff[_iff.member] = _iff; \
								} \
							} \
							if (head.count_element != map_iff.size()) \
								_smp::message_pool::getInstance().push(new message("[IFF::loadMap][log] iff name: " + std::string((iff_name)) + ". Nao carregou todos os elementos do IFF, Tem " + std::to_string(head.count_element) + " element(s) e foi carregado " + std::to_string(map_iff.size()), CL_FILE_LOG_AND_CONSOLE)); \
						}else if (head.version != IFF_VERSION) { \
							zip_fclose(file); \
							zip_close(z); \
							throw exception("Error a versao do arquivo IFF e diferente. Accepted " + std::to_string(IFF_VERSION) + " != " + std::to_string(head.version) + " Read from file. " + std::string((method)), STDA_MAKE_ERROR(STDA_ERROR_TYPE::_IFF, 4, 0)); \
						}else { \
							zip_fclose(file); \
							zip_close(z); \
							throw exception("Error o tamanho da estrutua IFF nao bate. size IFF: " + std::to_string(head.count_element * sizeof(type) + 8) + " size FILE ZIP IFF: " \
													+ std::to_string(ZIP_GET_REAL_SIZE(st.size)) + ". " + std::string((method)), STDA_MAKE_ERROR(STDA_ERROR_TYPE::_IFF, 5, 0)); \
						} \
						\
						zip_fclose(file); \
					}else if (file != nullptr) { \
						zip_fclose(file); \
						zip_close(z); \
						throw exception("Error nao e um arquivo IFF valido. " + std::string((method)), STDA_MAKE_ERROR(STDA_ERROR_TYPE::_IFF, 3, 0)); \
					}else { \
						zip_close(z); \
						throw exception("Error ao abrir o zip_fopen_index(). " + std::string((method)) + " Error: " + std::string(zip_strerror(z)), STDA_MAKE_ERROR(STDA_ERROR_TYPE::_IFF, 2, zip_get_error(z)->zip_err)); \
					} \
				} \
			} \
			if (map_iff.empty()) \
				_smp::message_pool::getInstance().push(new message("[E_LOAD_IFF] File " + std::string((iff_name)) + " nao encontrado ou esta vazio.", CL_FILE_LOG_AND_CONSOLE)); \
			zip_close(z); \
		}else \
			throw exception("Error ao abrir o arquivo zip. " + std::string((method)), STDA_MAKE_ERROR(STDA_ERROR_TYPE::_IFF, 1, error)); \
		return map_iff; \

#pragma comment(lib, PATH_LIBZIP_LIB)

using namespace stdA;

//std::map< uint32_t, IFF::Part > iff::m_part;
//std::map< uint32_t, IFF::AuxPart > iff::m_aux_part;
//std::map< uint32_t, IFF::Ball > iff::m_ball;
//std::map< uint32_t, IFF::Caddie > iff::m_caddie;
//std::map< uint32_t, IFF::CaddieItem > iff::m_caddie_item;
//std::map< uint32_t, IFF::CadieMagicBox > iff::m_cadie_magic_box;
//std::map< uint32_t, IFF::Card > iff::m_card;
//std::map< uint32_t, IFF::Item > iff::m_item;
//std::map< uint32_t, IFF::SetItem > iff::m_set_item;
//std::map< uint32_t, IFF::Mascot > iff::m_mascot;
//std::map< uint32_t, IFF::Achievement > iff::m_achievement;
//std::map< uint32_t, IFF::CounterItem > iff::m_counter_item;
//std::map< uint32_t, IFF::QuestStuff > iff::m_quest_stuff;
//std::map< uint32_t, IFF::QuestItem > iff::m_quest_item;
//std::map< uint32_t, IFF::Character > iff::m_character;
//std::map< uint32_t, IFF::Club > iff::m_club;
//std::map< uint32_t, IFF::ClubSet > iff::m_club_set;
//std::map< uint32_t, IFF::ClubSetWorkShopLevelUpProb > iff::m_club_set_work_shop_level_up_prob;
//std::map< uint32_t, IFF::ClubSetWorkShopRankUpExp > iff::m_club_set_work_shop_rank_exp;
//std::map< uint32_t, IFF::Course > iff::m_course;
//std::map< uint32_t, IFF::CutinInfomation > iff::m_cutin_infomation;
//std::map< uint32_t, IFF::Enchant > iff::m_enchant;
//std::map< uint32_t, IFF::Furniture > iff::m_furniture;
//std::map< uint32_t, IFF::HairStyle > iff::m_hair_style;
//std::map< uint32_t, IFF::Match > iff::m_match;
//std::map< uint32_t, IFF::Skin > iff::m_skin;
//std::map< uint32_t, IFF::Ability > iff::m_ability;
//std::map< uint32_t, IFF::Desc > iff::m_desc;
//std::map< uint32_t, IFF::GrandPrixAIOptionalData > iff::m_grand_prix_ai_optinal_data;
//std::map< uint32_t, IFF::GrandPrixConditionEquip > iff::m_grand_prix_condition_equip;
//std::map< uint32_t, IFF::GrandPrixData > iff::m_grand_prix_data;
//std::map< uint32_t, IFF::MemorialShopCoinItem > iff::m_memorial_shop_coin_item;
//std::map< uint32_t, IFF::ArtifactManaInfo > iff::m_artifact_mana_info;
//std::map< uint32_t, IFF::ErrorCodeInfo > iff::m_error_code_info;
//std::map< uint32_t, IFF::HoleCupDropItem > iff::m_hole_cup_drop_item;
//std::map< uint32_t, IFF::LevelUpPrizeItem > iff::m_level_up_prize_item;
//std::map< uint32_t, IFF::NonVisibleItemTable > iff::m_non_visible_item_table;
//std::map< uint32_t, IFF::PointShop > iff::m_point_shop;
//std::map< uint32_t, IFF::ShopLimitItem > iff::m_shop_limit_item;
//std::map< uint32_t, IFF::SpecialPrizeItem > iff::m_special_prize_item;
//std::map< uint32_t, IFF::SubscriptionItemTable > iff::m_subscription_item_table;
//std::map< uint32_t, IFF::SetEffectTable > iff::m_set_effect_table;
//std::map< uint32_t, IFF::TikiPointTable > iff::m_tiki_point_table;
//std::map< uint32_t, IFF::TikiRecipe > iff::m_tiki_recipe;
//std::map< uint32_t, IFF::TikiSpecialTable > iff::m_tiki_special_table;
//std::map< uint32_t, IFF::TimeLimitItem > iff::m_time_limit_item;
//
//std::vector< IFF::AddonPart > iff::m_addon_part;
//std::vector< IFF::CadieMagicBoxRandom > iff::m_cadie_magic_box_random;
//std::vector< IFF::CharacterMastery > iff::m_character_mastery;
//std::vector< IFF::ClubSetWorkShopLevelUpLimit > iff::m_club_set_work_shop_level_up_limit;
//std::vector< IFF::GrandPrixRankReward > iff::m_grand_prix_rank_reward;
//std::vector< IFF::GrandPrixSpecialHole > iff::m_grand_prix_special_hole;
//std::vector< IFF::MemorialShopRareItem > iff::m_memorial_shop_rare_item;
//std::vector< IFF::CaddieVoiceTable > iff::m_caddie_voice_table;
//std::vector< IFF::FurnitureAbility > iff::m_furniture_ability;
//std::vector< IFF::TwinsItemTable > iff::m_twins_item_table;
//
//bool iff::m_loaded;

iff::iff() : m_loaded(false) {

#if defined(_WIN32)
	InitializeCriticalSection(&m_cs);
#elif defined(__linux__)
	INIT_PTHREAD_MUTEXATTR_RECURSIVE;
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif

	load();
};

iff::~iff() {
	
	reset();

#if defined(_WIN32)
	DeleteCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_destroy(&m_cs);
#endif
};

bool iff::isLoad() {

	bool isLoad = false;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	isLoad = m_loaded;

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return isLoad;
};

std::map< uint32_t, IFF::Achievement > iff::load_achievement() {
	MAKE_UNZIP_MAP(uint32_t, IFF::Achievement, _typeid, "iff::load_achievement()", "Achievement.iff");
};

std::map< uint32_t, IFF::QuestItem > iff::load_quest_item() {
	MAKE_UNZIP_MAP(uint32_t, IFF::QuestItem, _typeid, "iff::load_quest_item", "QuestItem.iff");
};

std::map< uint32_t, IFF::QuestStuff > iff::load_quest_stuff() {
	MAKE_UNZIP_MAP(uint32_t, IFF::QuestStuff, _typeid, "iff::load_quest_stuff", "QuestStuff.iff");
};

std::map< uint32_t, IFF::CounterItem > iff::load_counter_item() {
	MAKE_UNZIP_MAP(uint32_t, IFF::CounterItem, _typeid, "iff::load_counter_item", "CounterItem.iff");
};

std::map< uint32_t, IFF::Item > iff::load_item() {
	MAKE_UNZIP_MAP(uint32_t, IFF::Item, _typeid, "iff::load_item", "Item.iff");
};

std::map< uint32_t, IFF::Part > iff::load_part() {
	MAKE_UNZIP_MAP(uint32_t, IFF::Part, _typeid, "iff::load_part", "Part.iff");
};

std::map< uint32_t, IFF::AuxPart > iff::load_aux_part() {
	MAKE_UNZIP_MAP(uint32_t, IFF::AuxPart, _typeid, "iff::load_aux_part", "AuxPart.iff");
};

std::map< uint32_t, IFF::Ball > iff::load_ball() {
	MAKE_UNZIP_MAP(uint32_t, IFF::Ball, _typeid, "iff::load_ball", "Ball.iff");
};

std::map< uint32_t, IFF::Caddie > iff::load_caddie() {
	MAKE_UNZIP_MAP(uint32_t, IFF::Caddie, _typeid, "iff:load_caddie", "Caddie.iff");
};

std::map< uint32_t, IFF::CaddieItem > iff::load_caddie_item() {
	MAKE_UNZIP_MAP(uint32_t, IFF::CaddieItem, _typeid, "iff::load_caddie_item", "CaddieItem.iff");
};

std::vector< IFF::CadieMagicBox > iff::load_cadie_magic_box() {
	MAKE_UNZIP_VECTOR(IFF::CadieMagicBox, "iff::load_cadie_magic_box", "CadieMagicBox.iff");
};

std::vector< IFF::CadieMagicBoxRandom > iff::load_cadie_magic_box_random() {
	MAKE_UNZIP_VECTOR(IFF::CadieMagicBoxRandom, "iff::load_cadie_magic_box_random", "CadieMagicBoxRandom.iff");
};

std::map< uint32_t, IFF::Card > iff::load_card() {
	MAKE_UNZIP_MAP(uint32_t, IFF::Card, _typeid, "iff::load_card", "Card.iff");
};

std::map< uint32_t, IFF::Character > iff::load_character() {
	MAKE_UNZIP_MAP(uint32_t, IFF::Character, _typeid, "iff::load_character", "Character.iff");
};

std::vector< IFF::CharacterMastery > iff::load_character_mastery() {
	MAKE_UNZIP_VECTOR(IFF::CharacterMastery, "iff::load_character_mastery", "CharacterMastery.iff");
};

std::map< uint32_t, IFF::Club > iff::load_club() {
	MAKE_UNZIP_MAP(uint32_t, IFF::Club, _typeid, "iff::load_club", "Club.iff");
};

std::map< uint32_t, IFF::ClubSet > iff::load_club_set() {
	MAKE_UNZIP_MAP(uint32_t, IFF::ClubSet, _typeid, "iff::load_club_set", "ClubSet.iff");
};

std::vector< IFF::ClubSetWorkShopLevelUpLimit > iff::load_club_set_work_shop_level_up_limit() {
	MAKE_UNZIP_VECTOR(IFF::ClubSetWorkShopLevelUpLimit, "iff::load_club_set_work_shop_level_up_limit", "ClubSetWorkShopLevelUpLimit.iff");
};

std::map< uint32_t, IFF::ClubSetWorkShopLevelUpProb > iff::load_club_set_work_shop_level_up_prob() {
	MAKE_UNZIP_MAP(uint32_t, IFF::ClubSetWorkShopLevelUpProb, tipo, "iff::load_club_set_work_shop_level_up_prob", "ClubSetWorkShopLevelUpProb.iff");
};

std::map< uint32_t, IFF::ClubSetWorkShopRankUpExp > iff::load_club_set_work_shop_rank_up_exp() {
	MAKE_UNZIP_MAP(uint32_t, IFF::ClubSetWorkShopRankUpExp, tipo, "iff::load_club_set_work_shop_rank_up_exp", "ClubSetWorkShopRankUpExp.iff");
};

std::map< uint32_t, IFF::Course > iff::load_course() {
	MAKE_UNZIP_MAP(uint32_t, IFF::Course, _typeid, "iff::load_course", "Course.iff");
};

std::map< uint32_t, IFF::CutinInfomation > iff::load_cutin_infomation() {
	MAKE_UNZIP_MAP(uint32_t, IFF::CutinInfomation, _typeid, "iff::load_course", "CutinInfomation.iff");
};

std::map< uint32_t, IFF::Enchant > iff::load_enchant() {
	MAKE_UNZIP_MAP(uint32_t, IFF::Enchant, _typeid, "iff::load_enchant", "Enchant.iff");
};

std::map< uint32_t, IFF::Furniture > iff::load_furniture() {
	MAKE_UNZIP_MAP(uint32_t, IFF::Furniture, _typeid, "iff::load_furniture", "Furniture.iff");
};

std::map< uint32_t, IFF::HairStyle > iff::load_hair_style() {
	MAKE_UNZIP_MAP(uint32_t, IFF::HairStyle, _typeid, "iff::load_hair_style", "HairStyle.iff");
};

std::map< uint32_t, IFF::Match > iff::load_match() {
	MAKE_UNZIP_MAP(uint32_t, IFF::Match, _typeid, "iff::load_match", "Match.iff");
};

std::map< uint32_t, IFF::Skin > iff::load_skin() {
	MAKE_UNZIP_MAP(uint32_t, IFF::Skin, _typeid, "iff::load_skin", "Skin.iff");
};

std::map< uint32_t, IFF::Ability > iff::load_ability() {
	MAKE_UNZIP_MAP(uint32_t, IFF::Ability, _typeid, "iff::load_ability", "Ability.iff");
};

std::map< uint32_t, IFF::Desc > iff::load_desc() {
	MAKE_UNZIP_MAP(uint32_t, IFF::Desc, _typeid, "iff::load_desc", "Desc.iff");
};

std::map< uint32_t, IFF::GrandPrixAIOptionalData > iff::load_grand_prix_ai_optional_data() {
	MAKE_UNZIP_MAP(uint32_t, IFF::GrandPrixAIOptionalData, id, "iff::load_grand_prix_ai_optional_data", "GrandPrixAIOptionalData.sff");
};

std::map< uint32_t, IFF::GrandPrixConditionEquip > iff::load_grand_prix_condition_equip() {
	MAKE_UNZIP_MAP(uint32_t, IFF::GrandPrixConditionEquip, _typeid, "iff::load_grand_prix_condition_equip", "GrandPrixConditionEquip.iff");
};

std::map< uint32_t, IFF::GrandPrixData > iff::load_grand_prix_data() {
	MAKE_UNZIP_MAP(uint32_t, IFF::GrandPrixData, _typeid, "iff::load_grand_prix_data", "GrandPrixData.iff");
};

std::vector< IFF::GrandPrixRankReward > iff::load_grand_prix_rank_reward() {
	MAKE_UNZIP_VECTOR(IFF::GrandPrixRankReward, "iff::load_grand_prix_rank_reward", "GrandPrixRankReward.iff");
};

std::vector< IFF::GrandPrixSpecialHole > iff::load_grand_prix_special_hole() {
	MAKE_UNZIP_VECTOR(IFF::GrandPrixSpecialHole, "iff::load_grand_prix_special_hole", "GrandPrixSpecialHole.iff");
};

std::map< uint32_t, IFF::MemorialShopCoinItem > iff::load_memorial_shop_coin_item() {
	MAKE_UNZIP_MAP(uint32_t, IFF::MemorialShopCoinItem, _typeid, "iff::load_memorial_shop_coin_item", "MemorialShopCoinItem.sff");
};

std::vector< IFF::MemorialShopRareItem > iff::load_memorial_shop_rare_item() {
	MAKE_UNZIP_VECTOR(IFF::MemorialShopRareItem, "iff::load_memorial_shop_rare_item", "MemorialShopRareItem.iff");
};

std::vector< IFF::AddonPart > iff::load_addon_part() {
	MAKE_UNZIP_VECTOR(IFF::AddonPart, "iff::load_addon_part", "AddonPart.iff");
};

std::map< uint32_t, IFF::ArtifactManaInfo > iff::load_artifact_mana_info() {
	MAKE_UNZIP_MAP(uint32_t, IFF::ArtifactManaInfo, artifact_typeid, "iff::load_artifact_mana_info", "ArtifactManaInfo.iff");
};

std::vector< IFF::CaddieVoiceTable > iff::load_caddie_voice_table() {
	MAKE_UNZIP_VECTOR(IFF::CaddieVoiceTable, "iff::load_caddie_voice_table", "CaddieVoiceTable.iff");
};

std::map< uint32_t, IFF::ErrorCodeInfo > iff::load_error_code_info() {
	MAKE_UNZIP_MAP(uint32_t, IFF::ErrorCodeInfo, code, "iff::load_error_code_info", "ErrorCodeInfo.iff");
};

std::vector< IFF::FurnitureAbility > iff::load_furniture_ability() {
	MAKE_UNZIP_VECTOR(IFF::FurnitureAbility, "iff::load_furniture_ability", "FurnitureAbility.iff");
};

std::map< uint32_t, IFF::HoleCupDropItem > iff::load_hole_cup_drop_item() {
	MAKE_UNZIP_MAP(uint32_t, IFF::HoleCupDropItem, _typeid, "iff::hole_cup_drop_item", "HoleCupDropItem.iff");
};

std::map< uint32_t, IFF::LevelUpPrizeItem > iff::load_level_up_prize_item() {
	MAKE_UNZIP_MAP(uint32_t, IFF::LevelUpPrizeItem, level, "iff::level_up_prize_item", "LevelUpPrizeItem.iff");
};

std::map< uint32_t, IFF::NonVisibleItemTable > iff::load_non_visible_item_table() {
	MAKE_UNZIP_MAP(uint32_t, IFF::NonVisibleItemTable, _typeid, "iff::load_non_visible_item_table", "NonVisibleItemTable.iff");
};

std::map< uint32_t, IFF::PointShop > iff::load_point_shop() {
	MAKE_UNZIP_MAP(uint32_t, IFF::PointShop, _typeid, "iff::load_point_shop", "PointShop.iff");
};

std::map< uint32_t, IFF::ShopLimitItem > iff::load_shop_limit_item() {
	MAKE_UNZIP_MAP(uint32_t, IFF::ShopLimitItem, _typeid, "iff::load_shop_limit_item", "ShopLimitItem.iff");
};

std::map< uint32_t, IFF::SpecialPrizeItem > iff::load_special_prize_item() {
	MAKE_UNZIP_MAP(uint32_t, IFF::SpecialPrizeItem, _typeid, "iff::load_special_prize_item", "SpecialPrizeItem.iff");
};

std::map< uint32_t, IFF::SubscriptionItemTable > iff::load_subscription_item_table() {
	MAKE_UNZIP_MAP(uint32_t, IFF::SubscriptionItemTable, _typeid, "iff::load_subscription_item_table", "SubscriptionItemTable.iff");
};

std::map< uint32_t, IFF::SetEffectTable > iff::load_set_effect_table() {
	MAKE_UNZIP_MAP(uint32_t, IFF::SetEffectTable, id, "iff::load_set_effect_table", "SetEffectTable.iff");
};

std::map< uint32_t, IFF::TikiPointTable > iff::load_tiki_point_table() {
	MAKE_UNZIP_MAP(uint32_t, IFF::TikiPointTable, id, "iff::load_tiki_point_table", "TikiPointTable.iff");
};

std::map< uint32_t, IFF::TikiRecipe > iff::load_tiki_recipe() {
	MAKE_UNZIP_MAP(uint32_t, IFF::TikiRecipe, id, "iff::load_tiki_recipe", "TikiRecipe.iff");
};

std::map< uint32_t, IFF::TikiSpecialTable > iff::load_tiki_special_table() {
	MAKE_UNZIP_MAP(uint32_t, IFF::TikiSpecialTable, id, "iff::load_tiki_special_table", "TikiSpecialTable.iff");
};

std::map< uint32_t, IFF::TimeLimitItem > iff::load_time_limit_item() {
	MAKE_UNZIP_MAP(uint32_t, IFF::TimeLimitItem, _typeid, "iff::load_time_limit_item", "TimeLimitItem.iff");
};

std::vector< IFF::TwinsItemTable > iff::load_twins_item_table() {
	MAKE_UNZIP_VECTOR(IFF::TwinsItemTable, "iff::load_twins_item_table", "TwinsItemTable.iff");
};

std::map< uint32_t, IFF::SetItem > iff::load_set_item() {
	MAKE_UNZIP_MAP(uint32_t, IFF::SetItem, _typeid, "iff::load_set_item", "SetItem.iff");
};

std::map< uint32_t, IFF::Mascot > iff::load_mascot() {
	MAKE_UNZIP_MAP(uint32_t, IFF::Mascot, _typeid, "iff::load_mascot", "Mascot.iff");
};

uint32_t iff::getItemGroupIdentify(uint32_t _typeid) {
	return (uint32_t)((_typeid & 0xFC000000) >> 26);
};

uint32_t iff::getItemSubGroupIdentify24(uint32_t _typeid) {
	return (uint32_t)((_typeid & ~0xFC000000) >> 24);		// aqui é >> 24, mas deixei 25 por causa do item equipável e o passivo, mas posso mudar depois isso
};

uint32_t iff::getItemSubGroupIdentify22(uint32_t _typeid) {
	return (uint32_t)((_typeid & ~0xFC000000) >> 22);		// esse retorno os grupos divididos em 0x40 0x80 0xC0, 0x100, 0x140
};

uint32_t iff::getItemSubGroupIdentify21(uint32_t _typeid) {
	return (uint32_t)((_typeid & ~0xFC000000) >> 21);		// esse retorno os grupos divididos em 0x20 0x40 0x60, 0x80, 0xA0, 0xC0, 0xE0, 0x100
};

uint32_t iff::getItemCharIdentify(uint32_t _typeid) {
	return (uint32_t)((_typeid & 0x03FF0000) >> 18);
};

uint32_t iff::getItemCharPartNumber(uint32_t _typeid) {
	return (uint32_t)((_typeid & 0x0003FF00) >> 13);
};

uint32_t iff::getItemCharTypeNumber(uint32_t _typeid) {
	return (uint32_t)((_typeid & 0x00001FFF) >> 8);
};

uint32_t iff::getItemIdentify(uint32_t _typeid) {
	return (uint32_t)(_typeid & 0x000000FF);
};

uint32_t iff::getItemTitleNum(uint32_t _typeid) {
	return (uint32_t)(_typeid & 0x3FFFFF);
};

uint32_t iff::getMatchTypeIdentity(uint32_t _typeid) {
	return (uint32_t)((_typeid & ~0xFC000000) >> 16);
}

uint32_t iff::getCaddieItemType(uint32_t _typeid) {
	return (uint32_t)((_typeid & 0x0000FF00) >> 13);
};

uint32_t iff::getCaddieIdentify(uint32_t _typeid) {
	return (uint32_t)(((_typeid & 0x0FFF0000) >> 21)/*Caddie Base*/ + ((_typeid & 0x000F0000) >> 16)/*Caddie Type, N, R, S e etc*/);
};

// Acho que eu fiz para usar no enchant de up stat de taqueira e character
uint32_t iff::getEnchantSlotStat(uint32_t _typeid) {
	return (uint32_t)((_typeid & 0x03FF0000) >> 20);
};

uint32_t iff::getItemAuxPartNumber(uint32_t _typeid) {
	return (uint32_t)((_typeid & 0x0003FF00) >> 16);
};

uint32_t iff::getGrandPrixAba(uint32_t _typeid) {
	return (uint32_t)((_typeid & 0x00FFFFFF) >> 19);
};

uint32_t iff::getGrandPrixType(uint32_t _typeid) {
	return (uint32_t)((_typeid & 0x0000FF00) >> 8);
};

bool iff::isGrandPrixEvent(uint32_t _typeid) {
	return (uint32_t)((_typeid & 0x3000000) >> 24) == 3u;
};

bool iff::isGrandPrixNormal(uint32_t _typeid) {
	return (uint32_t)((_typeid & 0x3000000) >> 24) == 0u;
};

void iff::load() {

	try {

#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		if (m_loaded)
			reset();

		m_achievement = load_achievement();
		m_counter_item = load_counter_item();
		m_item = load_item();
		m_mascot = load_mascot();
		m_part = load_part();
		m_aux_part = load_aux_part();
		m_ball = load_ball();
		m_caddie = load_caddie();
		m_caddie_item = load_caddie_item();
		m_cadie_magic_box = load_cadie_magic_box();
		m_cadie_magic_box_random = load_cadie_magic_box_random();
		m_card = load_card();
		m_character = load_character();
		m_character_mastery = load_character_mastery();
		m_club = load_club();
		m_club_set = load_club_set();
		m_club_set_work_shop_level_up_limit = load_club_set_work_shop_level_up_limit();
		m_club_set_work_shop_level_up_prob = load_club_set_work_shop_level_up_prob();
		m_club_set_work_shop_rank_exp = load_club_set_work_shop_rank_up_exp();
		m_course = load_course();
		m_cutin_infomation = load_cutin_infomation();
		m_enchant = load_enchant();
		m_furniture = load_furniture();
		m_hair_style = load_hair_style();
		m_match = load_match();
		m_skin = load_skin();
		m_ability = load_ability();
		m_desc = load_desc();
		m_grand_prix_ai_optinal_data = load_grand_prix_ai_optional_data();
		m_grand_prix_condition_equip = load_grand_prix_condition_equip();
		m_grand_prix_data = load_grand_prix_data();
		m_grand_prix_rank_reward = load_grand_prix_rank_reward();
		m_grand_prix_special_hole = load_grand_prix_special_hole();
		m_memorial_shop_coin_item = load_memorial_shop_coin_item();
		m_memorial_shop_rare_item = load_memorial_shop_rare_item();
		m_addon_part = load_addon_part();
		m_artifact_mana_info = load_artifact_mana_info();
		m_caddie_voice_table = load_caddie_voice_table();
		m_error_code_info = load_error_code_info();
		m_furniture_ability = load_furniture_ability();
		m_hole_cup_drop_item = load_hole_cup_drop_item();
		m_level_up_prize_item = load_level_up_prize_item();
		m_non_visible_item_table = load_non_visible_item_table();
		m_point_shop = load_point_shop();
		m_shop_limit_item = load_shop_limit_item();
		m_special_prize_item = load_special_prize_item();
		m_subscription_item_table = load_subscription_item_table();
		m_set_effect_table = load_set_effect_table();
		m_tiki_point_table = load_tiki_point_table();
		m_tiki_recipe = load_tiki_recipe();
		m_tiki_special_table = load_tiki_special_table();
		m_time_limit_item = load_time_limit_item();
		m_twins_item_table = load_twins_item_table();
		m_quest_item = load_quest_item();
		m_quest_stuff = load_quest_stuff();
		m_set_item = load_set_item();

		m_loaded = true;

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		_smp::message_pool::getInstance().push(new message("[iff::load][Log] IFF carregado com sucesso.", CL_FILE_LOG_AND_CONSOLE));

	}catch (exception& e) {
		
#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		_smp::message_pool::getInstance().push(new message("[iff::load][ErrorSystem] " + e.getFullMessageError()));

		// Relança para o server tomar as providências
		throw;
	}
};

void iff::reset() {
#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("[iff::reset][Log] IFF resetando...", CL_FILE_LOG_AND_CONSOLE));
#endif

	try {

#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		m_achievement.clear();
		m_counter_item.clear();
		m_mascot.clear();
		m_item.clear();
		m_part.clear();
		m_aux_part.clear();
		m_ball.clear();
		m_caddie.clear();
		m_caddie_item.clear();
		m_cadie_magic_box.clear();
		m_cadie_magic_box_random.clear();
		m_cadie_magic_box_random.shrink_to_fit();
		m_card.clear();
		m_character.clear();
		m_character_mastery.clear();
		m_character_mastery.shrink_to_fit();
		m_club.clear();
		m_club_set.clear();
		m_club_set_work_shop_level_up_limit.clear();
		m_club_set_work_shop_level_up_limit.shrink_to_fit();
		m_club_set_work_shop_level_up_prob.clear();
		m_club_set_work_shop_rank_exp.clear();
		m_course.clear();
		m_cutin_infomation.clear();
		m_enchant.clear();
		m_furniture.clear();
		m_hair_style.clear();
		m_match.clear();
		m_skin.clear();
		m_ability.clear();
		m_desc.clear();
		m_grand_prix_ai_optinal_data.clear();
		m_grand_prix_condition_equip.clear();
		m_grand_prix_data.clear();
		m_grand_prix_rank_reward.clear();
		m_grand_prix_rank_reward.shrink_to_fit();
		m_grand_prix_special_hole.clear();
		m_grand_prix_special_hole.shrink_to_fit();
		m_memorial_shop_coin_item.clear();
		m_memorial_shop_rare_item.clear();
		m_memorial_shop_rare_item.shrink_to_fit();
		m_addon_part.clear();
		m_artifact_mana_info.clear();
		m_caddie_voice_table.clear();
		m_caddie_voice_table.shrink_to_fit();
		m_error_code_info.clear();
		m_furniture_ability.clear();
		m_furniture_ability.shrink_to_fit();
		m_hole_cup_drop_item.clear();
		m_level_up_prize_item.clear();
		m_non_visible_item_table.clear();
		m_point_shop.clear();
		m_shop_limit_item.clear();
		m_special_prize_item.clear();
		m_subscription_item_table.clear();
		m_set_effect_table.clear();
		m_tiki_point_table.clear();
		m_tiki_recipe.clear();
		m_tiki_special_table.clear();
		m_time_limit_item.clear();
		m_twins_item_table.clear();
		m_twins_item_table.shrink_to_fit();
		m_quest_item.clear();
		m_quest_stuff.clear();
		m_set_item.clear();

		m_loaded = false;

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[iff::reset][Log] IFF resetado com sucesso.", CL_FILE_LOG_AND_CONSOLE));
#endif
	}catch (exception& e) {

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		_smp::message_pool::getInstance().push(new message("[iff::reset][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

void iff::reload() {
	
	if (isLoad())
		reset();

	load();
};

#if defined(_WIN32)
#define MAKE_FIND_MAP_IFF(_type, _iff, _typeid) \
			if (!isLoad()) { \
				_smp::message_pool::getInstance().push(new message("[iff::find" + std::string(#_type) + "][Error] IFF nao carregado", CL_FILE_LOG_AND_CONSOLE)); \
				return nullptr; \
			} \
			_type *ret = nullptr; \
			try { \
				\
				EnterCriticalSection(&m_cs); \
				\
				ret = &(_iff).at((_typeid)); \
				\
				LeaveCriticalSection(&m_cs); \
				\
			}catch (std::out_of_range& e) { \
				UNREFERENCED_PARAMETER(e); \
				\
				LeaveCriticalSection(&m_cs); \
				\
				/*_smp::message_pool::getInstance().push(new message("[iff::find" + std::string(#_type) + "][ErrorSystem] " + e.what(), CL_FILE_LOG_AND_CONSOLE));*/ \
			} \
			return ret;

#define MAKE_FIND_VECTOR_IFF(_type, _iff, member, _value) \
			if (!isLoad()) { \
				_smp::message_pool::getInstance().push(new message("[iff::find" + std::string(#_type) + "][Error] IFF nao carregado", CL_FILE_LOG_AND_CONSOLE)); \
				return std::vector < _type >(); \
			} \
			std::vector< _type > v_cmbr; \
			try { \
				\
				EnterCriticalSection(&m_cs); \
				\
				for (auto i = 0u; i < (_iff).size(); ++i) \
					if ((_iff)[i].member == (_value)) \
						v_cmbr.push_back((_iff)[i]); \
				\
				LeaveCriticalSection(&m_cs); \
				\
			}catch (exception& e) { \
				UNREFERENCED_PARAMETER(e); \
				\
				LeaveCriticalSection(&m_cs); \
				\
				/*_smp::message_pool::getInstance().push(new message("[iff::find" + std::string(#_type) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));*/ \
			} \
			return v_cmbr;
#elif defined(__linux__)
#define MAKE_FIND_MAP_IFF(_type, _iff, _typeid) \
			if (!isLoad()) { \
				_smp::message_pool::getInstance().push(new message("[iff::find" + std::string(#_type) + "][Error] IFF nao carregado", CL_FILE_LOG_AND_CONSOLE)); \
				return nullptr; \
			} \
			_type *ret = nullptr; \
			try { \
				\
				pthread_mutex_lock(&m_cs); \
				\
				ret = &(_iff).at((_typeid)); \
				\
				pthread_mutex_unlock(&m_cs); \
				\
			}catch (std::out_of_range& e) { \
				UNREFERENCED_PARAMETER(e); \
				\
				pthread_mutex_unlock(&m_cs); \
				\
				/*_smp::message_pool::getInstance().push(new message("[iff::find" + std::string(#_type) + "][ErrorSystem] " + e.what(), CL_FILE_LOG_AND_CONSOLE));*/ \
			} \
			return ret;

#define MAKE_FIND_VECTOR_IFF(_type, _iff, member, _value) \
			if (!isLoad()) { \
				_smp::message_pool::getInstance().push(new message("[iff::find" + std::string(#_type) + "][Error] IFF nao carregado", CL_FILE_LOG_AND_CONSOLE)); \
				return std::vector < _type >(); \
			} \
			std::vector< _type > v_cmbr; \
			try { \
				\
				pthread_mutex_lock(&m_cs); \
				\
				for (auto i = 0u; i < (_iff).size(); ++i) \
					if ((_iff)[i].member == (_value)) \
						v_cmbr.push_back((_iff)[i]); \
				\
				pthread_mutex_unlock(&m_cs); \
				\
			}catch (exception& e) { \
				UNREFERENCED_PARAMETER(e); \
				\
				pthread_mutex_unlock(&m_cs); \
				\
				/*_smp::message_pool::getInstance().push(new message("[iff::find" + std::string(#_type) + "][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));*/ \
			} \
			return v_cmbr;
#endif

// Find
IFF::Part *iff::findPart(uint32_t _typeid) {
	MAKE_FIND_MAP_IFF(IFF::Part, m_part, _typeid);
};

IFF::AuxPart *iff::findAuxPart(uint32_t _typeid) {
	MAKE_FIND_MAP_IFF(IFF::AuxPart, m_aux_part, _typeid);
};

IFF::Ball *iff::findBall(uint32_t _typeid) {
	MAKE_FIND_MAP_IFF(IFF::Ball, m_ball, _typeid);
};

IFF::Caddie *iff::findCaddie(uint32_t _typeid) {
	MAKE_FIND_MAP_IFF(IFF::Caddie, m_caddie, _typeid);
};

IFF::CaddieItem *iff::findCaddieItem(uint32_t _typeid) {
	MAKE_FIND_MAP_IFF(IFF::CaddieItem, m_caddie_item, _typeid);
};

IFF::CadieMagicBox *iff::findCadieMagicBox(uint32_t _seq) {
	MAKE_FIND_MAP_IFF(IFF::CadieMagicBox, m_cadie_magic_box, _seq);
};

IFF::Card *iff::findCard(uint32_t _typeid) {
	MAKE_FIND_MAP_IFF(IFF::Card, m_card, _typeid);
};

IFF::Character *iff::findCharacter(uint32_t _typeid) {
	MAKE_FIND_MAP_IFF(IFF::Character, m_character, _typeid);
};

IFF::Club *iff::findClub(uint32_t _typeid) {
	MAKE_FIND_MAP_IFF(IFF::Club, m_club, _typeid);
};

IFF::ClubSet *iff::findClubSet(uint32_t _typeid) {
	MAKE_FIND_MAP_IFF(IFF::ClubSet, m_club_set, _typeid);
};

IFF::Achievement *iff::findAchievement(uint32_t _typeid) {
	MAKE_FIND_MAP_IFF(IFF::Achievement, m_achievement, _typeid);
};

IFF::CounterItem *iff::findCounterItem(uint32_t _typeid) {
	MAKE_FIND_MAP_IFF(IFF::CounterItem, m_counter_item, _typeid);
};

IFF::Item *iff::findItem(uint32_t _typeid) {
	MAKE_FIND_MAP_IFF(IFF::Item, m_item, _typeid);
};

IFF::Mascot *iff::findMascot(uint32_t _typeid) {
	MAKE_FIND_MAP_IFF(IFF::Mascot, m_mascot, _typeid);
};

IFF::QuestItem *iff::findQuestItem(uint32_t _typeid) {
	MAKE_FIND_MAP_IFF(IFF::QuestItem, m_quest_item, _typeid);
};

IFF::QuestStuff *iff::findQuestStuff(uint32_t _typeid) {
	MAKE_FIND_MAP_IFF(IFF::QuestStuff, m_quest_stuff, _typeid);
};

IFF::SetItem *iff::findSetItem(uint32_t _typeid) {
	MAKE_FIND_MAP_IFF(IFF::SetItem, m_set_item, _typeid);
};

IFF::ClubSetWorkShopLevelUpProb *iff::findClubSetWorkShopLevelUpProb(uint32_t _tipo) {
	MAKE_FIND_MAP_IFF(IFF::ClubSetWorkShopLevelUpProb, m_club_set_work_shop_level_up_prob, _tipo);
};

IFF::ClubSetWorkShopRankUpExp *iff::findClubSetWorkShopRankExp(uint32_t _tipo) {
	MAKE_FIND_MAP_IFF(IFF::ClubSetWorkShopRankUpExp, m_club_set_work_shop_rank_exp, _tipo);
};

IFF::Course *iff::findCourse(uint32_t _typeid) {
	MAKE_FIND_MAP_IFF(IFF::Course, m_course, _typeid);
};

IFF::CutinInfomation *iff::findCutinInfomation(uint32_t _typeid) {
	MAKE_FIND_MAP_IFF(IFF::CutinInfomation, m_cutin_infomation, _typeid);
};

IFF::Enchant *iff::findEnchant(uint32_t _typeid) {
	MAKE_FIND_MAP_IFF(IFF::Enchant, m_enchant, _typeid);
};

IFF::Furniture *iff::findFurniture(uint32_t _typeid) {
	MAKE_FIND_MAP_IFF(IFF::Furniture, m_furniture, _typeid);
};

IFF::HairStyle *iff::findHairStyle(uint32_t _typeid) {
	MAKE_FIND_MAP_IFF(IFF::HairStyle, m_hair_style, _typeid);
};

IFF::Match *iff::findMatch(uint32_t _typeid) {
	MAKE_FIND_MAP_IFF(IFF::Match, m_match, _typeid);
};

IFF::Skin *iff::findSkin(uint32_t _typeid) {
	MAKE_FIND_MAP_IFF(IFF::Skin, m_skin, _typeid);
};

IFF::Ability *iff::findAbility(uint32_t _typeid) {
	MAKE_FIND_MAP_IFF(IFF::Ability, m_ability, _typeid);
};

IFF::Desc *iff::findDesc(uint32_t _typeid) {
	MAKE_FIND_MAP_IFF(IFF::Desc, m_desc, _typeid);
};

IFF::GrandPrixAIOptionalData *iff::findGrandPrixAIOptionalData(uint32_t _id) {
	MAKE_FIND_MAP_IFF(IFF::GrandPrixAIOptionalData, m_grand_prix_ai_optinal_data, _id);
};

IFF::GrandPrixConditionEquip *iff::findGrandPrixConditionEquip(uint32_t _typeid) {
	MAKE_FIND_MAP_IFF(IFF::GrandPrixConditionEquip, m_grand_prix_condition_equip, _typeid);
};

IFF::GrandPrixData *iff::findGrandPrixData(uint32_t _typeid) {
	MAKE_FIND_MAP_IFF(IFF::GrandPrixData, m_grand_prix_data, _typeid);
};

IFF::MemorialShopCoinItem *iff::findMemorialShopCoinItem(uint32_t _typeid) {
	MAKE_FIND_MAP_IFF(IFF::MemorialShopCoinItem, m_memorial_shop_coin_item, _typeid);
};

IFF::ArtifactManaInfo *iff::findArtifactManaInfo(uint32_t _typeid) {
	MAKE_FIND_MAP_IFF(IFF::ArtifactManaInfo, m_artifact_mana_info, _typeid);
};

IFF::ErrorCodeInfo *iff::findErrorCodeInfo(uint32_t _code) {
	MAKE_FIND_MAP_IFF(IFF::ErrorCodeInfo, m_error_code_info, _code);
};

IFF::HoleCupDropItem *iff::findHoleCupDropItem(uint32_t _typeid) {
	MAKE_FIND_MAP_IFF(IFF::HoleCupDropItem, m_hole_cup_drop_item, _typeid);
};

IFF::LevelUpPrizeItem *iff::findLevelUpPrizeItem(uint32_t _level) {
	MAKE_FIND_MAP_IFF(IFF::LevelUpPrizeItem, m_level_up_prize_item, _level);
};

IFF::NonVisibleItemTable *iff::findNonVisibleItemTable(uint32_t _typeid) {
	MAKE_FIND_MAP_IFF(IFF::NonVisibleItemTable, m_non_visible_item_table, _typeid);
};

IFF::PointShop *iff::findPointShop(uint32_t _typeid) {
	MAKE_FIND_MAP_IFF(IFF::PointShop, m_point_shop, _typeid);
};

IFF::ShopLimitItem *iff::findShopLimitItem(uint32_t _typeid) {
	MAKE_FIND_MAP_IFF(IFF::ShopLimitItem, m_shop_limit_item, _typeid);
};

IFF::SpecialPrizeItem *iff::findSpecialPrizeItem(uint32_t _typeid) {
	MAKE_FIND_MAP_IFF(IFF::SpecialPrizeItem, m_special_prize_item, _typeid);
};

IFF::SubscriptionItemTable *iff::findSubscriptionItemTable(uint32_t _typeid) {
	MAKE_FIND_MAP_IFF(IFF::SubscriptionItemTable, m_subscription_item_table, _typeid);
};

IFF::SetEffectTable *iff::findSetEffectTable(uint32_t _id) {
	MAKE_FIND_MAP_IFF(IFF::SetEffectTable, m_set_effect_table, _id);
};

IFF::TikiPointTable *iff::findTikiPointTable(uint32_t _id) {
	MAKE_FIND_MAP_IFF(IFF::TikiPointTable, m_tiki_point_table, _id);
};

IFF::TikiRecipe *iff::findTikiRecipe(uint32_t _id) {
	MAKE_FIND_MAP_IFF(IFF::TikiRecipe, m_tiki_recipe, _id);
};

IFF::TikiSpecialTable *iff::findTikiSpecialTable(uint32_t _id) {
	MAKE_FIND_MAP_IFF(IFF::TikiSpecialTable, m_tiki_special_table, _id);
};

IFF::TimeLimitItem *iff::findTimeLimitItem(uint32_t _typeid) {
	MAKE_FIND_MAP_IFF(IFF::TimeLimitItem, m_time_limit_item, _typeid);
};

std::vector< IFF::AddonPart > iff::findAddonPart(uint32_t _typeid) {
	MAKE_FIND_VECTOR_IFF(IFF::AddonPart, m_addon_part, _typeid, _typeid);
};

std::vector< IFF::CadieMagicBoxRandom > iff::findCadieMagicBoxRandom(uint32_t _id) {
	MAKE_FIND_VECTOR_IFF(IFF::CadieMagicBoxRandom, m_cadie_magic_box_random, id, _id);
};

std::vector < IFF::CharacterMastery > iff::findCharacterMastery(uint32_t _typeid) {
	MAKE_FIND_VECTOR_IFF(IFF::CharacterMastery, m_character_mastery, _typeid, _typeid);
};

std::vector< IFF::ClubSetWorkShopLevelUpLimit > iff::findClubSetWorkShopLevelUpLimit(uint32_t _tipo) {
	MAKE_FIND_VECTOR_IFF(IFF::ClubSetWorkShopLevelUpLimit, m_club_set_work_shop_level_up_limit, tipo, _tipo);
};

std::vector< IFF::GrandPrixRankReward > iff::findGrandPrixRankReward(uint32_t _typeid) {
	MAKE_FIND_VECTOR_IFF(IFF::GrandPrixRankReward, m_grand_prix_rank_reward, _typeid, _typeid);
};

std::vector< IFF::GrandPrixSpecialHole > iff::findGrandPrixSpecialHole(uint32_t _typeid) {
	MAKE_FIND_VECTOR_IFF(IFF::GrandPrixSpecialHole, m_grand_prix_special_hole, _typeid, _typeid);
};

std::vector< IFF::MemorialShopRareItem > iff::findMemorialShopRareItem(uint32_t _gacha_num) {
	MAKE_FIND_VECTOR_IFF(IFF::MemorialShopRareItem, m_memorial_shop_rare_item, gacha.number, _gacha_num);
};

std::vector< IFF::CaddieVoiceTable > iff::findCaddieVoiceTable(uint32_t _typeid) {
	MAKE_FIND_VECTOR_IFF(IFF::CaddieVoiceTable, m_caddie_voice_table, _typeid, _typeid);
};

std::vector< IFF::FurnitureAbility > iff::findFurnitureAbility(uint32_t _typeid) {
	MAKE_FIND_VECTOR_IFF(IFF::FurnitureAbility, m_furniture_ability, _typeid, _typeid);
};

std::vector< IFF::TwinsItemTable > iff::findTwinsItemTable(uint32_t _type) {
	MAKE_FIND_VECTOR_IFF(IFF::TwinsItemTable, m_twins_item_table, type, _type);
};

IFF::SetEffectTable *iff::findFirstItemInSetEffectTable(uint32_t _typeid) {

	// Find LAST_ELEMENT_IN_ARRAY é o mesmo é só para não sobrecarregar e da mensagem
#define LAST_ELEMENT_IN_ARRAY_IFF(_element_arr) ((_element_arr) + (sizeof((_element_arr)) / sizeof((_element_arr)[0])))
	
	if (!isLoad()) {
		
		_smp::message_pool::getInstance().push(new message("[iff::findFirstItemInSetEffectTable][Error] IFF nao carregado", CL_FILE_LOG_AND_CONSOLE));

		return nullptr;
	}

	IFF::SetEffectTable *ret = nullptr;

	try {
		
#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif
		
		auto i = std::find_if(m_set_effect_table.begin(), m_set_effect_table.end(), [&](auto& _el) {
			return std::find(_el.second.item._typeid, LAST_ELEMENT_IN_ARRAY_IFF(_el.second.item._typeid), _typeid) != LAST_ELEMENT_IN_ARRAY_IFF(_el.second.item._typeid);
		});

		if (i != m_set_effect_table.end())
			ret = &i->second;
		
#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif
		
	}catch (std::out_of_range& e) { 
		UNREFERENCED_PARAMETER(e); 
		
#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif
		
		/*_smp::message_pool::getInstance().push(new message("[iff::findFirstItemInSetEffectTable][ErrorSystem] " + e.what(), CL_FILE_LOG_AND_CONSOLE));*/ 
	}

	return ret;
};

std::vector< IFF::SetEffectTable > iff::findAllItemInSetEffectTable(uint32_t _typeid) {
	
	// Find LAST_ELEMENT_IN_ARRAY é o mesmo é só para não sobrecarregar e da mensagem
#define LAST_ELEMENT_IN_ARRAY_IFF(_element_arr) ((_element_arr) + (sizeof((_element_arr)) / sizeof((_element_arr)[0])))
	
	if (!isLoad()) {
		
		_smp::message_pool::getInstance().push(new message("[iff::findAllItemInSetEffectTable][Error] IFF nao carregado", CL_FILE_LOG_AND_CONSOLE));

		return std::vector< IFF::SetEffectTable >();
	}

	std::vector< IFF::SetEffectTable > ret;

	try {
		
#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif
		
		std::for_each(m_set_effect_table.begin(), m_set_effect_table.end(), [&](auto& _el) {
			
			if (std::find(_el.second.item._typeid, LAST_ELEMENT_IN_ARRAY_IFF(_el.second.item._typeid), _typeid) != LAST_ELEMENT_IN_ARRAY_IFF(_el.second.item._typeid))
				ret.push_back(_el.second);
		});
		
#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif 
		
	}catch (std::out_of_range& e) { 
		UNREFERENCED_PARAMETER(e); 
		
#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif
		
		/*_smp::message_pool::getInstance().push(new message("[iff::findAllItemInSetEffectTable][ErrorSystem] " + e.what(), CL_FILE_LOG_AND_CONSOLE));*/ 
	}

	return ret;
};

// find Commom Item
IFF::Base *iff::findCommomItem(uint32_t _typeid) {
	IFF::Base* commom = nullptr;

	switch (getItemGroupIdentify(_typeid)) {
	case CHARACTER:
		commom = findCharacter(_typeid);
		break;
	case PART:
		commom = findPart(_typeid);
		break;
	case CLUB:
		commom = findClub(_typeid);
		break;
	case CLUBSET:
		commom = findClubSet(_typeid);
		break;
	case BALL:
		commom = findBall(_typeid);
		break;
	case ITEM:
		commom = findItem(_typeid);
		break;
	case CADDIE:
		commom = findCaddie(_typeid);
		break;
	case CAD_ITEM:
		commom = findCaddieItem(_typeid);
		break;
	case SET_ITEM:
		commom = findSetItem(_typeid);
		break;
	case COURSE:
		commom = findCourse(_typeid);
		break;
	/*case MATCH:	// Não tem a estrutura base
		break;*/
	/*case ENCHANT:	// Não tem a estrutura base
		break;*/
	case SKIN:
		commom = findSkin(_typeid);
		break;
	case HAIR_STYLE:
		commom = findHairStyle(_typeid);
		break;
	case MASCOT:
		commom = findMascot(_typeid);
		break;
	case FURNITURE:
		commom = findFurniture(_typeid);
		break;
	case ACHIEVEMENT:
		commom = findAchievement(_typeid);
		break;
	case COUNTER_ITEM:
		commom = findCounterItem(_typeid);
		break;
	case AUX_PART:
		commom = findAuxPart(_typeid);
		break;
	case QUEST_STUFF:
		commom = findQuestStuff(_typeid);
		break;
	case QUEST_ITEM:
		commom = findQuestItem(_typeid);
		break;
	case CARD:
		commom = findCard(_typeid);
		break;
	}

	return commom;
};

bool iff::ItemEquipavel(uint32_t _typeid) {
	return !((_typeid & 0xFE000000) >> 25 & 3);
};

bool iff::IsCanOverlapped(uint32_t _typeid) {
	
	switch (getItemGroupIdentify(_typeid)) {
	case CHARACTER:
	case CLUB:
	case CLUBSET:
	case FURNITURE:
	case COURSE:
	case MATCH:
	case ENCHANT:
	case SKIN:
	case HAIR_STYLE:
	case ACHIEVEMENT:
	case QUEST_STUFF:
	case QUEST_ITEM:
	default:
		return false;
	case CAD_ITEM:
	{
		auto cadItem = findCaddieItem(_typeid);
		
		if (cadItem != nullptr && cadItem->shop.flag_shop.time_shop.active)
			return true;	// Caddie item pode, se for de tempo para aumentar o tempo dele
		
		break;
	}
	case MASCOT:
	{
		auto mascot = findMascot(_typeid);
		
		if (mascot != nullptr && mascot->shop.flag_shop.time_shop.active)
			return true;
		
		break;
	}
	case PART:
	{
		auto part = findPart(_typeid);

		// Libera os parts para Duplicatas se ele estiver liberado para vender no personal shop
		if (part != nullptr && (part->type_item == IFF::Part::UCC_BLANK || part->type_item == IFF::Part::UCC_COPY 
				|| part->shop.flag_shop.uFlagShop.stFlagShop.can_dup || part->shop.flag_shop.uFlagShop.stFlagShop.can_send_mail_and_personal_shop))
			return true;
		
		break;
	}
	case ITEM:	// Libera todos item para dub se tiver abilitado no shop
	/*{
		auto commom = findCommomItem(_typeid);
		if (commom != nullptr && (commom->shop.flag_shop.uFlagShop.stFlagShop.can_dup || commom->shop.flag_shop.uFlagShop.stFlagShop.can_send_mail_and_personal_shop/*mail*//*))
			return true;
		break;
	}*/
	case BALL:
	case CARD:
		return true;
	case CADDIE:
		if (_typeid == 0x1C000001 || _typeid == 0x1C000002 || _typeid == 0x1C000003 || _typeid == 0x1C000007)
			return true;
		break;
	case SET_ITEM:
	{
		auto tipo_set_item = getItemSubGroupIdentify21(_typeid);
		
		if (tipo_set_item == IFF::SetItem::SUB_TYPE::BALL 
				|| tipo_set_item == IFF::SetItem::SUB_TYPE::CHARACTER_SET_DUP_AND_ITEM_PASSIVE_AND_ACTIVE
				|| tipo_set_item == IFF::SetItem::SUB_TYPE::CARD)
			return true;
		
		break;
	}
	case AUX_PART:
	{
		auto auxPart = findAuxPart(_typeid);
		
		if (auxPart != nullptr && auxPart->cc[0]/*Qntd*/ > 0)
			return (_typeid & ~0x1F0000);
		
		break;
	}	// Fim AuxPart
	}	// Fim Case

	return false;
};

bool iff::IsBuyItem(uint32_t _typeid) {
	
	auto commom = findCommomItem(_typeid);

	if (commom != nullptr)
		return (commom->active == 1 && commom->shop.flag_shop.uFlagShop.stFlagShop.is_saleable == 1);

	return false;
};

bool iff::IsGiftItem(uint32_t _typeid) {
	auto commom = findCommomItem(_typeid);

	// É saleable ou giftable nunca os 2 juntos por que é a flag composta Somente Purchase(compra)
	// então faço o xor nas 2 flag se der o valor de 1 é por que ela é um item que pode presentear
	// Ex: 1 + 1 = 2 Não é
	// Ex: 1 + 0 = 1 OK
	// Ex: 0 + 1 = 1 OK
	// Ex: 0 + 0 = 0 Não é
	if (commom != nullptr)
		return (commom->active == 1 && commom->shop.flag_shop.uFlagShop.stFlagShop.is_cash == 1 
				&& (commom->shop.flag_shop.uFlagShop.stFlagShop.is_saleable ^ commom->shop.flag_shop.uFlagShop.stFlagShop.is_giftable) == 1);

	return false;
};

bool iff::IsOnlyDisplay(uint32_t _typeid) {
	auto commom = findCommomItem(_typeid);

	if (commom != nullptr)
		return (commom->active == 1 && commom->shop.flag_shop.uFlagShop.stFlagShop.only_display == 1);

	return false;
};

bool iff::IsOnlyPurchase(uint32_t _typeid) {
	auto commom = findCommomItem(_typeid);

	if (commom != nullptr)
		return (commom->active == 1 && commom->shop.flag_shop.uFlagShop.stFlagShop.is_saleable == 1 
				&& commom->shop.flag_shop.uFlagShop.stFlagShop.is_giftable == 1);

	return false;
};

bool iff::IsOnlyGift(uint32_t _typeid) {
	auto commom = findCommomItem(_typeid);

	if (commom != nullptr)
		return (commom->active == 1 && commom->shop.flag_shop.uFlagShop.stFlagShop.is_cash
				&& commom->shop.flag_shop.uFlagShop.stFlagShop.is_giftable == 1 && commom->shop.flag_shop.uFlagShop.stFlagShop.is_saleable == 0);

	return false;
};

bool iff::IsItemEquipable(uint32_t _typeid) {
	auto item = findItem(_typeid);
	
	if (item != nullptr)
		return (getItemSubGroupIdentify24(_typeid) >> 1) == 0;	// Equiável, aqui depois tenho que mudar se mudar lá em cima, para (func() >> 1) == 0

	return false;
};

bool iff::IsTitle(uint32_t _typeid) {

	if (getItemGroupIdentify(_typeid) != iff::SKIN)
		return false;	// Não é uma skin(bg, frame, sticker, slot, cutin, title)

	if ((_typeid & 0x3C00000u) != 0x1800000u)
		return false;	// Não é um title

	return true;
};

std::vector< IFF::ClubSet > iff::findClubSetOriginal(uint32_t _typeid) {

	std::vector< IFF::ClubSet > v_clubset;
	IFF::ClubSet *clubset = nullptr;

	// Invalid Typeid
	if (_typeid == 0)
		return v_clubset;

	if ((clubset = findClubSet(_typeid)) != nullptr) {
		
		for (auto& el : m_club_set) {

			// Text pangya é o logo da taqueira, como as especiais tem seu proprio logo
			// então o número do logo vai ser a taqueira base das taqueira que transforma
			if (el.second.text_pangya == clubset->text_pangya)
				v_clubset.push_back(el.second);
		}
	}

	return v_clubset;
};

std::map<uint32_t, IFF::Achievement>& iff::getAchievement() {
	return m_achievement;
};

std::map< uint32_t, IFF::QuestItem >& iff::getQuestItem() {
	return m_quest_item;
};

std::map< uint32_t, IFF::CounterItem >& iff::getCounterItem() {
	return m_counter_item;
};

std::map< uint32_t, IFF::Item >& iff::getItem() {
	return m_item;
};

std::map< uint32_t, IFF::Card >& iff::getCard() {
	return m_card;
};

std::map< uint32_t, IFF::Skin >& iff::getSkin() {
	return m_skin;
};

std::map< uint32_t, IFF::AuxPart >& iff::getAuxPart() {
	return m_aux_part;
};

std::map< uint32_t, IFF::Ball >& iff::getBall() {
	return m_ball;
};

std::map< uint32_t, IFF::Character >& iff::getCharacter() {
	return m_character;
};

std::map< uint32_t, IFF::Caddie >& iff::getCaddie() {
	return m_caddie;
};

std::map< uint32_t, IFF::CaddieItem >& iff::getCaddieItem() {
	return m_caddie_item;
};

std::vector< IFF::CadieMagicBox >& iff::getCadieMagicBox() {
	return m_cadie_magic_box;
};

std::map< uint32_t, IFF::ClubSet >& iff::getClubSet() {
	return m_club_set;
};

std::map< uint32_t, IFF::HairStyle >& iff::getHairStyle() {
	return m_hair_style;
};

std::map< uint32_t, IFF::Part >& iff::getPart() {
	return m_part;
};

std::map< uint32_t, IFF::Mascot >& iff::getMascot() {
	return m_mascot;
};

std::map< uint32_t, IFF::SetItem >& iff::getSetItem() {
	return m_set_item;
};

std::map< uint32_t, IFF::Desc >& iff::getDesc() {
	return m_desc;
};

std::map< uint32_t, IFF::LevelUpPrizeItem >& iff::getLevelUpPrizeItem() {
	return m_level_up_prize_item;
};

std::map< uint32_t, IFF::MemorialShopCoinItem >& iff::getMemorialShopCoinItem() {
	return m_memorial_shop_coin_item;
};

std::vector< IFF::MemorialShopRareItem >& iff::getMemorialShopRareItem() {
	return m_memorial_shop_rare_item;
};

std::map< uint32_t, IFF::Course >& iff::getCourse() {
	return m_course;
};

std::map< uint32_t, IFF::TimeLimitItem >& iff::getTimeLimitItem() {
	return m_time_limit_item;
};

std::map< uint32_t, IFF::GrandPrixAIOptionalData >& iff::getGrandPrixAIOptionalData() {
	return m_grand_prix_ai_optinal_data;
};

std::map<uint32_t, IFF::GrandPrixData>& iff::getGrandPrixData() {
	return m_grand_prix_data;
};

std::map< uint32_t, IFF::Ability >& iff::getAbility() {
	return m_ability;
};

std::map< uint32_t, IFF::SetEffectTable >& iff::getSetEffectTable() {
	return m_set_effect_table;
};
