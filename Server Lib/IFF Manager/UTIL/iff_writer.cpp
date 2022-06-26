// Arquivo iff_writer.cpp
// Criado em 03/06/2019 as 19:48 por Acrisio
// Implementa��o da classe iff_writer

#pragma pack(1)

#include "iff_writer.hpp"
#include "../../Projeto IOCP/ZIP/zip.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"
#include "../../Projeto IOCP/UTIL/exception.h"

#if INTPTR_MAX == INT64_MAX
#define ZIP_GET_REAL_SIZE(_size) (_size)
#elif INTPTR_MAX == INT32_MAX
#define ZIP_GET_REAL_SIZE(_size) (((_size) & 0x00000000FFFFFFFF) << 32 | (((_size) & 0xFFFFFFFF00000000) >> 32))
#else
#error Unknown pointer size or missing size macros!
#endif

#define BEGIN_SAVE_IFF(_type, _method, _iff_name, _num_element)	int error = 0; \
		zip *z = zip_open(PATH_PANGYA_IFF, ZIP_CREATE, &error); \
		if (z == nullptr) { \
\
			_smp::message_pool::getInstance().push(new message("[iff::writer::" + std::string((_method)) + "][Error] nao conseguiu abrir o arquivo zip '"  \
					+ std::string(PATH_PANGYA_IFF) + "' ErrCode: " + std::to_string(error), CL_FILE_LOG_AND_CONSOLE)); \
\
			return; \
		} \
\
		size_t i = 0; \
		uint64_t index, num_entries = zip_get_num_entries(z, ZIP_FL_UNCHANGED); \
\
		zip_stat_t st = { 0 }; \
		zip_stat_init(&st); \
\
		for (index = 0; index < num_entries; ++index) { \
\
			zip_stat_index(z, index, ZIP_FL_UNCHANGED, &st); \
\
			if (strcmp(st.name, (_iff_name)) == 0) \
					break; \
		} \
\
		/* Cabe��rio */ \
		IFF::Head head{ 0 }; \
\
		/* External Attributes */ \
		zip_uint8_t opsys; \
		zip_uint32_t attr; \
\
		/* Encontrou o arquivo ele existe, abre para ler o cabe��rio */ \
		if (index < num_entries) { \
\
			zip_file *file = zip_fopen_index(z, index, ZIP_FL_UNCHANGED); \
\
			if (file != nullptr && ZIP_GET_REAL_SIZE(st.size) >= sizeof(IFF::Head)) { \
\
				if (zip_file_get_external_attributes(z, index, ZIP_FL_UNCHANGED, &opsys, &attr) < 0) \
					_smp::message_pool::getInstance().push(new message("[iff_writer::" + std::string((_method)) + "][Error] zip_file_get_external_attributes, ErrCode: "  \
							+ std::to_string(zip_get_error(z)->zip_err) + " ErrMsg: " + std::string(zip_strerror(z)), CL_FILE_LOG_AND_CONSOLE)); \
\
				_smp::message_pool::getInstance().push(new message("[iff_writer::" + std::string((_method)) + "][Log] " + std::string((_iff_name))  \
						+ " OPERATION SYSTEM: " + std::to_string((unsigned short)opsys) + " ATTR_FILE: " + std::to_string(attr), CL_FILE_LOG_AND_CONSOLE)); \
\
				zip_fread(file, (char*)&head, sizeof(IFF::Head)); \
\
				/* Fecha o file depois que pegou o cabe�ario */ \
				zip_fclose(file); \
\
				/* Verifica a vers�o do cabe�alho */ \
				if (head.version != IFF_VERSION/*13*/ || (head.count_element * sizeof(_type) + sizeof(IFF::Head)) != ZIP_GET_REAL_SIZE(st.size)) { \
\
					_smp::message_pool::getInstance().push(new message("[iff_writer::" + std::string((_method)) + "][Error] " + std::string((_iff_name))  \
							+ " version ou file count element size esta errado.", CL_FILE_LOG_AND_CONSOLE)); \
\
					zip_close(z); \
\
					return; \
				} \
\
			}else if (file != nullptr) { \
\
				zip_fclose(file); \
				zip_close(z); \
\
				_smp::message_pool::getInstance().push(new message("[iff_writer::" + std::string((_method)) + "][Error] nao e um arquivo iff valido.", CL_FILE_LOG_AND_CONSOLE)); \
\
				return; \
\
			}else { \
\
				_smp::message_pool::getInstance().push(new message("[iff_writer::" + std::string((_method)) + "][Error] nao conseguiu abrir zip_fopen_index. ErrCode: "  \
						+ std::to_string(zip_get_error(z)->zip_err) + " ErrMsg: " + std::string(zip_strerror(z)), CL_FILE_LOG_AND_CONSOLE)); \
\
				zip_close(z); \
\
				return; \
			} \
\
		}else {	/* N�o existe cria um cabe�ario e seta o file attributos */ \
\
			_smp::message_pool::getInstance().push(new message("[iff_writer::" + std::string((_method)) + "][Log] nao tinha o arquivo "  \
					+ std::string((_iff_name)) + ", cria um novo.", CL_FILE_LOG_AND_CONSOLE)); \
\
			head.version = IFF_VERSION; \
			head.count_element = (unsigned short)(_num_element); \
\
			opsys = 0; \
			attr = FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM; \
		} \

#define SAVE_IFF_VECTOR(_type, _method, _iff_name, _vec) BEGIN_SAVE_IFF(_type, (_method), (_iff_name), (_vec).size()); \
		zip_source *s = NULL; \
\
		/* size elements */ \
		head.count_element = (unsigned short)(_vec).size(); \
\
		zip_uint8_t *buff = new zip_uint8_t[(_vec).size() * sizeof(_type) + sizeof(IFF::Head)]; \
\
		size_t index_buff = 0; \
\
		/* Copy to buff head */ \
		memcpy(buff + index_buff, &head, sizeof(head));	index_buff += sizeof(head); \
\
		for (auto& el : (_vec)) { \
			memcpy(buff + index_buff, &el, sizeof(_type));	index_buff += sizeof(_type); \
		} \
\
		/* Cria zip_source_buffer e salva no arquivo iff */ \
		if ((s = zip_source_buffer(z, buff, index_buff, 0)) == NULL || (index = zip_file_add(z, (_iff_name), s, ZIP_FL_UNCHANGED | ZIP_FL_OVERWRITE)) < 0) { \
\
			_smp::message_pool::getInstance().push(new message("[iff_writer::" + std::string((_method)) + "][Error] nao conseguiu adicionar o arquivo " + std::string((_iff_name))  \
					+ " para o arquivo '" + std::string(PATH_PANGYA_IFF) + "'. ErrCode: " + std::to_string(zip_get_error(z)->zip_err) + " ErrMsg: " + std::string(zip_strerror(z)), CL_FILE_LOG_AND_CONSOLE)); \
\
			zip_source_free(s); \
\
		}else \
			_smp::message_pool::getInstance().push(new message("[iff_writer::" + std::string((_method)) + "][Log] salvo o arquivo source " + std::string((_iff_name)) + " com sucesso", CL_FILE_LOG_AND_CONSOLE)); \
\
		/* Set File Attibute */ \
		if (zip_file_set_external_attributes(z, index, ZIP_FL_UNCHANGED, opsys, attr) < 0) \
			_smp::message_pool::getInstance().push(new message("[iff_writer::" + std::string((_method)) + "][Error] zip_file_set_external_attributes, ErrCode: " \
					+ std::to_string(zip_get_error(z)->zip_err) + " ErrMsg: " + std::string(zip_strerror(z)), CL_FILE_LOG_AND_CONSOLE)); \
\
		/* Fecha zip */ \
		zip_close(z); \

#define SAVE_IFF_MAP(_type, _method, _iff_name, _map) BEGIN_SAVE_IFF(_type, (_method), (_iff_name), (_map).size()); \
		zip_source *s = NULL; \
\
		/* size elements */ \
		head.count_element = (unsigned short)(_map).size(); \
\
		zip_uint8_t *buff = new zip_uint8_t[(_map).size() * sizeof(_type) + sizeof(IFF::Head)]; \
\
		size_t index_buff = 0; \
\
		/* Copy to buff head */ \
		memcpy(buff + index_buff, &head, sizeof(head));	index_buff += sizeof(head); \
\
		for (auto& el : (_map)) { \
			memcpy(buff + index_buff, &el.second, sizeof(_type));	index_buff += sizeof(_type); \
		} \
\
		/* Cria zip_source_buffer e salva no arquivo iff */ \
		if ((s = zip_source_buffer(z, buff, index_buff, 0)) == NULL || (index = zip_file_add(z, (_iff_name), s, ZIP_FL_UNCHANGED | ZIP_FL_OVERWRITE)) < 0) { \
\
			_smp::message_pool::getInstance().push(new message("[iff_writer::" + std::string((_method)) + "][Error] nao conseguiu adicionar o arquivo " + std::string((_iff_name))  \
					+ " para o arquivo '" + std::string(PATH_PANGYA_IFF) + "'. ErrCode: " + std::to_string(zip_get_error(z)->zip_err) + " ErrMsg: " + std::string(zip_strerror(z)), CL_FILE_LOG_AND_CONSOLE)); \
\
			zip_source_free(s); \
\
		}else \
			_smp::message_pool::getInstance().push(new message("[iff_writer::" + std::string((_method)) + "][Log] salvo o arquivo source " + std::string((_iff_name)) + " com sucesso", CL_FILE_LOG_AND_CONSOLE)); \
\
		/* Set File Attibute */ \
		if (zip_file_set_external_attributes(z, index, ZIP_FL_UNCHANGED, opsys, attr) < 0) \
			_smp::message_pool::getInstance().push(new message("[iff_writer::" + std::string((_method)) + "][Error] zip_file_set_external_attributes, ErrCode: " \
					+ std::to_string(zip_get_error(z)->zip_err) + " ErrMsg: " + std::string(zip_strerror(z)), CL_FILE_LOG_AND_CONSOLE)); \
\
		/* Fecha zip */ \
		zip_close(z); \

using namespace stdA;

iff_writer::iff_writer() {
}

iff_writer::~iff_writer() {
}

void iff_writer::saveAll() {

	saveAuxPart();
	saveBall();
	saveCaddie();
	saveCaddieItem();
	saveCard();
	saveCharacter();
	saveClubSet();
	saveHairStyle();
	saveItem();
	saveMascot();
	savePart();
	saveSetItem();
	saveSkin();
	saveCadieMagicBox();
	saveAchievement();
	saveQuestItem();
	saveQuestStuff();
	saveCounterItem();
	saveCadieMagicBoxRandom();
	saveCharacterMastery();
	saveClub();
	saveClubSetWorkShopLevelUpLimit();
	saveClubSetWorkShopLevelUpProb();
	saveClubSetWorkShopRankUpExp();
	saveCourse();
	saveCutinInfomation();
	saveEnchant();
	saveFurniture();
	saveMatch();
	saveAbility();
	saveDesc();
	saveGrandPrixAIOptionalData();
	saveGrandPrixConditionEquip();
	saveGrandPrixData();
	saveGrandPrixRankReward();
	saveGrandPrixSpecialHole();
	saveMemorialShopCoinItem();
	saveMemorialShopRareItem();
	saveAddonPart();
	saveArtifactManaInfo();
	saveCaddieVoiceTable();
	saveErrorCodeInfo();
	saveFurnitureAbility();
	saveHoleCupDropItem();
	saveLevelUpPrizeItem();
	saveNonVisibleItemTable();
	savePointShop();
	saveShopLimitItem();
	saveSpecialPrizeItem();
	saveSubscriptionItemTable();
	saveSetEffectTable();
	saveTikiPointTable();
	saveTikiRecipe();
	saveTikiSpecialTable();
	saveTimeLimitItem();
	saveTwinsItemTable();
}

void iff_writer::saveBall() {
	SAVE_IFF_MAP(IFF::Ball, "saveBall", "Ball.iff", m_ball);
}

void iff_writer::saveCaddie() {
	SAVE_IFF_MAP(IFF::Caddie, "saveCaddie", "Caddie.iff", m_caddie);
}

void iff_writer::saveCaddieItem() {
	SAVE_IFF_MAP(IFF::CaddieItem, "saveCaddieItem", "CaddieItem.iff", m_caddie_item);
}

void iff_writer::saveCard(){
	SAVE_IFF_MAP(IFF::Card, "saveCard", "Card.iff", m_card);
}

void iff_writer::saveCharacter() {
	SAVE_IFF_MAP(IFF::Character, "saveCharacter", "Character.iff", m_character);
}

void iff_writer::saveClubSet() {
	SAVE_IFF_MAP(IFF::ClubSet, "saveClubSet", "ClubSet.iff", m_club_set);
}

void iff_writer::saveHairStyle() {
	SAVE_IFF_MAP(IFF::HairStyle, "saveHairStyle", "HairStyle.iff", m_hair_style);
}

void iff_writer::saveItem() {
	SAVE_IFF_MAP(IFF::Item, "saveItem", "Item.iff", m_item);
}

void iff_writer::saveMascot() {
	SAVE_IFF_MAP(IFF::Mascot, "saveMascot", "Mascot.iff", m_mascot);
}

void iff_writer::savePart() {
	SAVE_IFF_MAP(IFF::Part, "savePart", "Part.iff", m_part);
}

void iff_writer::saveAuxPart() {
	SAVE_IFF_MAP(IFF::AuxPart, "saveAuxPart", "AuxPart.iff", m_aux_part);
}

void iff_writer::saveSetItem() {
	SAVE_IFF_MAP(IFF::SetItem, "saveSetItem", "SetItem.iff", m_set_item);
}

void iff_writer::saveSkin() {
	SAVE_IFF_MAP(IFF::Skin, "saveSkin", "Skin.iff", m_skin);
}

void iff_writer::saveCadieMagicBox(){
	SAVE_IFF_VECTOR(IFF::CadieMagicBox, "saveCadieMagicBox", "CadieMagicBox.iff", m_cadie_magic_box);
}

void iff_writer::saveAchievement() {
	SAVE_IFF_MAP(IFF::Achievement, "saveAchievement", "Achievement.iff", m_achievement);
}

void iff_writer::saveQuestItem() {
	SAVE_IFF_MAP(IFF::QuestItem, "saveQuestItem", "QuestItem.iff", m_quest_item);
}

void iff_writer::saveQuestStuff() {
	SAVE_IFF_MAP(IFF::QuestStuff, "saveQuestStuff", "QuestStuff.iff", m_quest_stuff);
}

void iff_writer::saveCounterItem() {
	SAVE_IFF_MAP(IFF::CounterItem, "saveCounterItem", "CounterItem.iff", m_counter_item);
}

void iff_writer::saveCadieMagicBoxRandom() {
	SAVE_IFF_VECTOR(IFF::CadieMagicBoxRandom, "saveCadieMagicBoxRandom", "CadieMagicBoxRandom.iff", m_cadie_magic_box_random);
}

void iff_writer::saveCharacterMastery() {
	SAVE_IFF_VECTOR(IFF::CharacterMastery, "saveCharacterMastery", "CharacterMastery.iff", m_character_mastery);
}

void iff_writer::saveClub() {
	SAVE_IFF_MAP(IFF::Club, "saveClub", "Club.iff", m_club);
}

void iff_writer::saveClubSetWorkShopLevelUpLimit() {
	SAVE_IFF_VECTOR(IFF::ClubSetWorkShopLevelUpLimit, "saveClubSetWorkShopLevelUpLimit", "ClubSetWorkShopLevelUpLimit.iff", m_club_set_work_shop_level_up_limit);
}

void iff_writer::saveClubSetWorkShopLevelUpProb() {
	SAVE_IFF_MAP(IFF::ClubSetWorkShopLevelUpProb, "saveClubSetWorkShopLevelUpProb", "ClubSetWorkShopLevelUpProb.iff", m_club_set_work_shop_level_up_prob);
}

void iff_writer::saveClubSetWorkShopRankUpExp() {
	SAVE_IFF_MAP(IFF::ClubSetWorkShopRankUpExp, "saveClubSetWorkShopRankUpExp", "ClubSetWorkShopRankUpExp.iff", m_club_set_work_shop_rank_exp);
}

void iff_writer::saveCourse() {
	SAVE_IFF_MAP(IFF::Course, "saveCourse", "Course.iff", m_course);
}

void iff_writer::saveCutinInfomation() {
	SAVE_IFF_MAP(IFF::CutinInfomation, "saveCutinInfomation", "CutinInfomation.iff", m_cutin_infomation);
}

void iff_writer::saveEnchant() {
	SAVE_IFF_MAP(IFF::Enchant, "saveEnchant", "Enchant.iff", m_enchant);
}

void iff_writer::saveFurniture() {
	SAVE_IFF_MAP(IFF::Furniture, "saveFurniture", "Furniture.iff", m_furniture);
}

void iff_writer::saveMatch() {
	SAVE_IFF_MAP(IFF::Match, "saveMatch", "Match.iff", m_match);
}

void iff_writer::saveAbility() {
	SAVE_IFF_MAP(IFF::Ability, "saveAbility", "Ability.iff", m_ability);
}

void iff_writer::saveDesc() {
	SAVE_IFF_MAP(IFF::Desc, "saveDesc", "Desc.iff", m_desc);
}

void iff_writer::saveGrandPrixAIOptionalData() {
	SAVE_IFF_MAP(IFF::GrandPrixAIOptionalData, "saveGrandPrixAIOptionalData", "GrandPrixAIOptionalData.sff", m_grand_prix_ai_optinal_data);
}

void iff_writer::saveGrandPrixConditionEquip() {
	SAVE_IFF_MAP(IFF::GrandPrixConditionEquip, "saveGrandPrixConditionEquip", "GrandPrixConditionEquip.iff", m_grand_prix_condition_equip);
}

void iff_writer::saveGrandPrixData() {
	SAVE_IFF_MAP(IFF::GrandPrixData, "saveGrandPrixData", "GrandPrixData.iff", m_grand_prix_data);
}

void iff_writer::saveGrandPrixRankReward() {
	SAVE_IFF_VECTOR(IFF::GrandPrixRankReward, "saveGrandPrixRankReward", "GrandPrixRankReward.iff", m_grand_prix_rank_reward);
}

void iff_writer::saveGrandPrixSpecialHole() {
	SAVE_IFF_VECTOR(IFF::GrandPrixSpecialHole, "saveGrandPrixSpecialHole", "GrandPrixSpecialHole.iff", m_grand_prix_special_hole);
}

void iff_writer::saveMemorialShopCoinItem() {
	SAVE_IFF_MAP(IFF::MemorialShopCoinItem, "saveMemorialShopCoinItem", "MemorialShopCoinItem.sff", m_memorial_shop_coin_item);
}

void iff_writer::saveMemorialShopRareItem() {
	SAVE_IFF_VECTOR(IFF::MemorialShopRareItem, "saveMemorialShopRareItem", "MemorialShopRareItem.iff", m_memorial_shop_rare_item);
}

void iff_writer::saveAddonPart() {
	SAVE_IFF_VECTOR(IFF::AddonPart, "saveAddonPart", "AddonPart.iff", m_addon_part);
}

void iff_writer::saveArtifactManaInfo() {
	SAVE_IFF_MAP(IFF::ArtifactManaInfo, "saveArtifactManaInfo", "ArtifactManaInfo.iff", m_artifact_mana_info);
}

void iff_writer::saveCaddieVoiceTable() {
	SAVE_IFF_VECTOR(IFF::CaddieVoiceTable, "saveCaddieVoiceTable", "CaddieVoiceTable.iff", m_caddie_voice_table);
}

void iff_writer::saveErrorCodeInfo() {
	SAVE_IFF_MAP(IFF::ErrorCodeInfo, "saveErrorCodeInfo", "ErrorCodeInfo.iff", m_error_code_info);
}

void iff_writer::saveFurnitureAbility() {
	SAVE_IFF_VECTOR(IFF::FurnitureAbility, "saveFurnitureAbility", "FurnitureAbility.iff", m_furniture_ability);
}

void iff_writer::saveHoleCupDropItem() {
	SAVE_IFF_MAP(IFF::HoleCupDropItem, "saveHoleCupDropItem", "HoleCupDropItem.iff", m_hole_cup_drop_item);
}

void iff_writer::saveLevelUpPrizeItem() {
	SAVE_IFF_MAP(IFF::LevelUpPrizeItem, "saveLevelUpPrizeItem", "LevelUpPrizeItem.iff", m_level_up_prize_item);
}

void iff_writer::saveNonVisibleItemTable() {
	SAVE_IFF_MAP(IFF::NonVisibleItemTable, "saveNonVisibleItemTable", "NonVisibleItemTable.iff", m_non_visible_item_table);
}

void iff_writer::savePointShop() {
	SAVE_IFF_MAP(IFF::PointShop, "savePointShop", "PointShop.iff", m_point_shop);
}

void iff_writer::saveShopLimitItem() {
	SAVE_IFF_MAP(IFF::ShopLimitItem, "saveShopLimitItem", "ShopLimitItem.iff", m_shop_limit_item);
}

void iff_writer::saveSpecialPrizeItem() {
	SAVE_IFF_MAP(IFF::SpecialPrizeItem, "saveSpecialPrizeItem", "SpecialPrizeItem.iff", m_special_prize_item);
}

void iff_writer::saveSubscriptionItemTable() {
	SAVE_IFF_MAP(IFF::SubscriptionItemTable, "saveSubscriptionItemTable", "SubscriptionItemTable.iff", m_subscription_item_table);
}

void iff_writer::saveSetEffectTable() {
	SAVE_IFF_MAP(IFF::SetEffectTable, "saveSetEffectTable", "SetEffectTable.iff", m_set_effect_table);
}

void iff_writer::saveTikiPointTable() {
	SAVE_IFF_MAP(IFF::TikiPointTable, "saveTikiPointTable", "TikiPointTable.iff", m_tiki_point_table);
}

void iff_writer::saveTikiRecipe() {
	SAVE_IFF_MAP(IFF::TikiRecipe, "saveTikiRecipe", "TikiRecipe.iff", m_tiki_recipe);
}

void iff_writer::saveTikiSpecialTable() {
	SAVE_IFF_MAP(IFF::TikiSpecialTable, "saveTikiSpecialTable", "TikiSpecialTable.iff", m_tiki_special_table);
}

void iff_writer::saveTimeLimitItem() {
	SAVE_IFF_MAP(IFF::TimeLimitItem, "saveTimeLimitItem", "TimeLimitItem.iff", m_time_limit_item);
}

void iff_writer::saveTwinsItemTable() {
	SAVE_IFF_VECTOR(IFF::TwinsItemTable, "saveTwinsItemTable", "TwinsItemTable.iff", m_twins_item_table);
}
