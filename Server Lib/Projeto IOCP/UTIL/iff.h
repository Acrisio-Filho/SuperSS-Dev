// Arquico iff.h
// Criado em 03/10/2017 por Acrisio
// Definição da classe iff

#if defined(_WIN32)
#pragma pack(1)
#endif

#pragma once
#ifndef _STDA_IFF_H
#define _STDA_IFF_H

#include <cstdint>

#if INTPTR_MAX == INT64_MAX
#define PATH_LIBZIP_LIB "../../Projeto IOCP/ZIP/lib/zip-64.lib"
#elif INTPTR_MAX == INT32_MAX
#define PATH_LIBZIP_LIB "../../Projeto IOCP/ZIP/lib/zip.lib"
#else
#error Unknown pointer size or missing size macros!
#endif

#if defined(__linux__)
#include "WinPort.h"
#include <pthread.h>
#endif

#define PATH_PANGYA_IFF "data/pangya_jp.iff"

#define IFF_VERSION 0x0D

#include "../TYPE/data_iff.h"

#include "../TYPE/singleton.h"

#include <string>
#include <map>
#include <vector>

namespace stdA {
    class iff {
		public:
			enum IFF_GROUP_ID: uint32_t {
				CHARACTER = 1,		// 4
				PART,				// 8
				CLUB,				// 12
				CLUBSET,			// 16
				BALL,				// 20
				ITEM,				// 24
				CADDIE,				// 28
				CAD_ITEM,			// 32
				SET_ITEM,			// 36
				COURSE,				// 40
				MATCH,				// 44 Trofel
				ENCHANT = 13,		// 52
				SKIN,				// 56
				HAIR_STYLE,			// 60
				MASCOT,				// 64
				FURNITURE = 18,		// 72
				ACHIEVEMENT,		// 76
				COUNTER_ITEM = 27,	// 108
				AUX_PART,			// 112
				QUEST_STUFF,		// 116
				QUEST_ITEM,			// 120
				CARD,				// 124
			};

        public:
            iff();
            ~iff();

			/*static*/ bool isLoad();

			// Load All
			/*static*/ void load();
			/*static*/ void reset();
			/*static*/ void reload();

			// Find
			/*static*/ IFF::Part *findPart(uint32_t _typeid);
			/*static*/ IFF::AuxPart *findAuxPart(uint32_t _typeid);
			/*static*/ IFF::Ball *findBall(uint32_t _typeid);
			/*static*/ IFF::Caddie *findCaddie(uint32_t _typeid);
			/*static*/ IFF::CaddieItem *findCaddieItem(uint32_t _typeid);
			/*static*/ IFF::CadieMagicBox *findCadieMagicBox(uint32_t _seq);
			/*static*/ IFF::Card *findCard(uint32_t _typeid);
			/*static*/ IFF::Character *findCharacter(uint32_t _typeid);
			/*static*/ IFF::Club *findClub(uint32_t _typeid);
			/*static*/ IFF::ClubSet *findClubSet(uint32_t _typeid);
			/*static*/ IFF::Achievement *findAchievement(uint32_t _typeid);
			/*static*/ IFF::CounterItem *findCounterItem(uint32_t _typeid);
			/*static*/ IFF::Item *findItem(uint32_t _typeid);
			/*static*/ IFF::Mascot *findMascot(uint32_t _typeid);
			/*static*/ IFF::QuestItem *findQuestItem(uint32_t _typeid);
			/*static*/ IFF::QuestStuff *findQuestStuff(uint32_t _typeid);
			/*static*/ IFF::SetItem *findSetItem(uint32_t _typeid);
			/*static*/ IFF::ClubSetWorkShopLevelUpProb *findClubSetWorkShopLevelUpProb(uint32_t _tipo);
			/*static*/ IFF::ClubSetWorkShopRankUpExp *findClubSetWorkShopRankExp(uint32_t _tipo);
			/*static*/ IFF::Course *findCourse(uint32_t _typeid);
			/*static*/ IFF::CutinInfomation *findCutinInfomation(uint32_t _typeid);
			/*static*/ IFF::Enchant *findEnchant(uint32_t _typeid);
			/*static*/ IFF::Furniture *findFurniture(uint32_t _typeid);
			/*static*/ IFF::HairStyle *findHairStyle(uint32_t _typeid);
			/*static*/ IFF::Match *findMatch(uint32_t _typeid);
			/*static*/ IFF::Skin *findSkin(uint32_t _typeid);
			/*static*/ IFF::Ability *findAbility(uint32_t _typeid);
			/*static*/ IFF::Desc *findDesc(uint32_t _typeid);
			/*static*/ IFF::GrandPrixAIOptionalData *findGrandPrixAIOptionalData(uint32_t _id);
			/*static*/ IFF::GrandPrixConditionEquip *findGrandPrixConditionEquip(uint32_t _typeid);
			/*static*/ IFF::GrandPrixData *findGrandPrixData(uint32_t _typeid);
			/*static*/ IFF::MemorialShopCoinItem *findMemorialShopCoinItem(uint32_t _typeid);
			/*static*/ IFF::ArtifactManaInfo *findArtifactManaInfo(uint32_t _typeid);
			/*static*/ IFF::ErrorCodeInfo *findErrorCodeInfo(uint32_t _code);
			/*static*/ IFF::HoleCupDropItem *findHoleCupDropItem(uint32_t _typeid);
			/*static*/ IFF::LevelUpPrizeItem *findLevelUpPrizeItem(uint32_t _level);
			/*static*/ IFF::NonVisibleItemTable *findNonVisibleItemTable(uint32_t _typeid);
			/*static*/ IFF::PointShop *findPointShop(uint32_t _typeid);
			/*static*/ IFF::ShopLimitItem *findShopLimitItem(uint32_t _typeid);
			/*static*/ IFF::SpecialPrizeItem *findSpecialPrizeItem(uint32_t _typeid);
			/*static*/ IFF::SubscriptionItemTable *findSubscriptionItemTable(uint32_t _typeid);
			/*static*/ IFF::SetEffectTable *findSetEffectTable(uint32_t _id);
			/*static*/ IFF::TikiPointTable *findTikiPointTable(uint32_t _id);
			/*static*/ IFF::TikiRecipe *findTikiRecipe(uint32_t _id);
			/*static*/ IFF::TikiSpecialTable *findTikiSpecialTable(uint32_t _id);
			/*static*/ IFF::TimeLimitItem *findTimeLimitItem(uint32_t _typeid);

			/*static*/ std::vector< IFF::AddonPart > findAddonPart(uint32_t _typeid);
			/*static*/ std::vector< IFF::CadieMagicBoxRandom > findCadieMagicBoxRandom(uint32_t _id);
			/*static*/ std::vector< IFF::CharacterMastery > findCharacterMastery(uint32_t _typeid);
			/*static*/ std::vector< IFF::ClubSetWorkShopLevelUpLimit > findClubSetWorkShopLevelUpLimit(uint32_t _tipo);
			/*static*/ std::vector< IFF::GrandPrixRankReward > findGrandPrixRankReward(uint32_t _typeid);
			/*static*/ std::vector< IFF::GrandPrixSpecialHole > findGrandPrixSpecialHole(uint32_t _typeid);
			/*static*/ std::vector< IFF::MemorialShopRareItem > findMemorialShopRareItem(uint32_t _gacha_num);
			/*static*/ std::vector< IFF::CaddieVoiceTable > findCaddieVoiceTable(uint32_t _typeid);
			/*static*/ std::vector< IFF::FurnitureAbility > findFurnitureAbility(uint32_t _typeid);
			/*static*/ std::vector< IFF::TwinsItemTable > findTwinsItemTable(uint32_t _type);

			/*static*/ IFF::SetEffectTable *findFirstItemInSetEffectTable(uint32_t _typeid);
			/*static*/ std::vector< IFF::SetEffectTable > findAllItemInSetEffectTable(uint32_t _typeid);
			
			// find Commom Item
			/*static*/ IFF::Base *findCommomItem(uint32_t _typeid);

			// Checkers

			/*static*/ bool ItemEquipavel(uint32_t _typeid);

			/*static*/ bool IsCanOverlapped(uint32_t _typeid);

			/*static*/ bool IsBuyItem(uint32_t _typeid);

			/*static*/ bool IsGiftItem(uint32_t _typeid);

			/*static*/ bool IsOnlyDisplay(uint32_t _typeid);

			/*static*/ bool IsOnlyPurchase(uint32_t _typeid);

			/*static*/ bool IsOnlyGift(uint32_t _typeid);

			/*static*/ bool IsItemEquipable(uint32_t _typeid);

			/*static*/ bool IsTitle(uint32_t _typeid);

			// ClubSet find ClubSet Original
			/*static*/ std::vector< IFF::ClubSet > findClubSetOriginal(uint32_t _typeid);

			// Getters

			// get Achievements
			/*static*/ std::map< uint32_t, IFF::Achievement >& getAchievement();

			// get QuestItem
			/*static*/ std::map< uint32_t, IFF::QuestItem >& getQuestItem();

			// get CounterItem
			/*static*/ std::map< uint32_t, IFF::CounterItem >& getCounterItem();

			// get Item
			/*static*/ std::map< uint32_t, IFF::Item >& getItem();

			// get Card
			/*static*/ std::map< uint32_t, IFF::Card >& getCard();

			// get Skin
			/*static*/ std::map< uint32_t, IFF::Skin >& getSkin();

			// get AuxPart
			/*static*/ std::map< uint32_t, IFF::AuxPart >& getAuxPart();

			// get Ball
			/*static*/ std::map< uint32_t, IFF::Ball >& getBall();

			// get Character
			/*static*/ std::map< uint32_t, IFF::Character >& getCharacter();

			// get Caddie
			/*static*/ std::map< uint32_t, IFF::Caddie >& getCaddie();

			// get CaddieItem
			/*static*/ std::map< uint32_t, IFF::CaddieItem >& getCaddieItem();

			// get CadieMagicBox
			/*static*/ std::vector< IFF::CadieMagicBox >& getCadieMagicBox();

			// get ClubSet
			/*static*/ std::map< uint32_t, IFF::ClubSet >& getClubSet();

			// get HairStyle
			/*static*/ std::map< uint32_t, IFF::HairStyle >& getHairStyle();

			// get Part
			/*static*/ std::map< uint32_t, IFF::Part >& getPart();

			// get Mascot
			/*static*/ std::map< uint32_t, IFF::Mascot >& getMascot();

			// get SetItem
			/*static*/ std::map< uint32_t, IFF::SetItem >& getSetItem();

			// get Desc
			/*static*/ std::map< uint32_t, IFF::Desc >& getDesc();

			// get LevelUpPrizeItem
			/*static*/ std::map< uint32_t, IFF::LevelUpPrizeItem >& getLevelUpPrizeItem();

			// get MemorialShopCoinItem
			/*static*/ std::map< uint32_t, IFF::MemorialShopCoinItem >& getMemorialShopCoinItem();

			// get MemorialShopRareItem
			/*static*/ std::vector< IFF::MemorialShopRareItem >& getMemorialShopRareItem();

			// get Course
			/*static*/ std::map< uint32_t, IFF::Course >& getCourse();

			// get TimeLimitItem
			/*static*/ std::map< uint32_t, IFF::TimeLimitItem >& getTimeLimitItem();

			// get GrandPrixAIOptionalData
			/*static*/ std::map< uint32_t, IFF::GrandPrixAIOptionalData >& getGrandPrixAIOptionalData();

			// get GrandPrixData
			/*static*/ std::map< uint32_t, IFF::GrandPrixData >& getGrandPrixData();

			// get Ability
			/*static*/ std::map< uint32_t, IFF::Ability >& getAbility();

			// get SetEffectTable
			/*static*/ std::map< uint32_t, IFF::SetEffectTable >& getSetEffectTable();

			// get QuestStuff
			/*static */std::map< uint32_t, IFF::QuestStuff >& getQuestStuff();

			// get Club
			/*static */std::map< uint32_t, IFF::Club >& getClub();

			// get ClubSetWorkShopLevelUpProb
			/*static */std::map< uint32_t, IFF::ClubSetWorkShopLevelUpProb >& getClubSetWorkShopLevelUpProb();

			// get ClubSetWorkShopRankUpExp
			/*static */std::map< uint32_t, IFF::ClubSetWorkShopRankUpExp >& getClubSetWorkShopRankUpExp();

			// get CutinInfomation
			/*static */std::map< uint32_t, IFF::CutinInfomation >& getCutinInfomation();

			// get Enchant
			/*static */std::map< uint32_t, IFF::Enchant >& getEnchant();

			// get Furniture
			/*static */std::map< uint32_t, IFF::Furniture >& getFurniture();

			// get Match
			/*static */std::map< uint32_t, IFF::Match >& getMatch();

			// get GrandPrixConditionEquip
			/*static */std::map< uint32_t, IFF::GrandPrixConditionEquip >& getGrandPrixConditionEquip();

			// get ArtifactManaInfo
			/*static */std::map< uint32_t, IFF::ArtifactManaInfo >& getArtifactManaInfo();

			// get ErrorCodeInfo
			/*static */std::map< uint32_t, IFF::ErrorCodeInfo >& getErrorCodeInfo();

			// get HoleCupDropItem
			/*static */std::map< uint32_t, IFF::HoleCupDropItem >& getHoleCupDropItem();

			// get NonVisibleItemTable
			/*static */std::map< uint32_t, IFF::NonVisibleItemTable >& getNonVisibleItemTable();

			// get PointShop
			/*static */std::map< uint32_t, IFF::PointShop >& getPointShop();

			// get ShopLimitItem
			/*static */std::map< uint32_t, IFF::ShopLimitItem >& getShopLimitItem();

			// get SpecialPrizeItem
			/*static */std::map< uint32_t, IFF::SpecialPrizeItem >& getSpecialPrizeItem();

			// get SubscriptionItemTable
			/*static */std::map< uint32_t, IFF::SubscriptionItemTable >& getSubscriptionItemTable();

			// get TikiPointTable
			/*static */std::map< uint32_t, IFF::TikiPointTable >& getTikiPointTable();

			// get TikiRecipe
			/*static */std::map< uint32_t, IFF::TikiRecipe >& getTikiRecipe();

			// get TikiSpecialTable
			/*static */std::map< uint32_t, IFF::TikiSpecialTable >& getTikiSpecialTable();

			// get AddonPart
			/*static */std::vector< IFF::AddonPart >& getAddonPart();

			// get CadieMagicBoxRandom
			/*static */std::vector< IFF::CadieMagicBoxRandom >& getCadieMagicBoxRandom();

			// get CharacterMastery
			/*static */std::vector< IFF::CharacterMastery >& getCharacterMastery();

			// get ClubSetWorkShopLevelUpLimit
			/*static */std::vector< IFF::ClubSetWorkShopLevelUpLimit >& getClubSetWorkShopLevelUpLimit();

			// get GrandPrixRankReward
			/*static */std::vector< IFF::GrandPrixRankReward >& getGrandPrixRankReward();

			// get GrandPrixSpecialHole
			/*static */std::vector< IFF::GrandPrixSpecialHole >& getGrandPrixSpecialHole();

			// get CaddieVoiceTable
			/*static */std::vector< IFF::CaddieVoiceTable >& getCaddieVoiceTable();

			// get FurnitureAbility
			/*static */std::vector< IFF::FurnitureAbility >& getFurnitureAbility();

			// get TwinsItemTable
			/*static */std::vector< IFF::TwinsItemTable >& getTwinsItemTable();

		private:
			// Achievement
            /*static*/ std::map< uint32_t, IFF::Achievement > load_achievement();
			/*static*/ std::map< uint32_t, IFF::QuestItem > load_quest_item();
			/*static*/ std::map< uint32_t, IFF::QuestStuff > load_quest_stuff();
			/*static*/ std::map< uint32_t, IFF::CounterItem > load_counter_item();

			// Item
			/*static*/ std::map< uint32_t, IFF::Item > load_item();

			// Part
			/*static*/ std::map< uint32_t, IFF::Part > load_part();
			
			// AuxPart
			/*static*/ std::map< uint32_t, IFF::AuxPart > load_aux_part();

			// Ball
			/*static*/ std::map< uint32_t, IFF::Ball > load_ball();

			// Caddie
			/*static*/ std::map< uint32_t, IFF::Caddie > load_caddie();

			// CaddieItem
			/*static*/ std::map< uint32_t, IFF::CaddieItem > load_caddie_item();

			// CadieMagicBox
			/*static*/ std::vector< IFF::CadieMagicBox > load_cadie_magic_box();

			// CadieMagicBoxRandom
			/*static*/ std::vector< IFF::CadieMagicBoxRandom > load_cadie_magic_box_random();

			// Card
			/*static*/ std::map< uint32_t, IFF::Card > load_card();

			// Character
			/*static*/ std::map< uint32_t, IFF::Character > load_character();

			// CharacterMastery
			/*static*/ std::vector< IFF::CharacterMastery > load_character_mastery();

			// Club
			/*static*/ std::map< uint32_t, IFF::Club > load_club();

			// ClubSet
			/*static*/ std::map< uint32_t, IFF::ClubSet > load_club_set();

			// ClubSetWorkShopLevelUpLimit
			/*static*/ std::vector< IFF::ClubSetWorkShopLevelUpLimit > load_club_set_work_shop_level_up_limit();

			// ClubSetWorkShopLevelUpProb
			/*static*/ std::map< uint32_t, IFF::ClubSetWorkShopLevelUpProb > load_club_set_work_shop_level_up_prob();

			// ClubSetWorkShopRankUpExp
			/*static*/ std::map< uint32_t, IFF::ClubSetWorkShopRankUpExp > load_club_set_work_shop_rank_up_exp();

			// Course
			/*static*/ std::map< uint32_t, IFF::Course > load_course();

			// CutinInfomation
			/*static*/ std::map< uint32_t, IFF::CutinInfomation > load_cutin_infomation();

			// Enchant
			/*static*/ std::map< uint32_t, IFF::Enchant > load_enchant();

			// Furniture
			/*static*/ std::map< uint32_t, IFF::Furniture > load_furniture();

			// HairStyle
			/*static*/ std::map< uint32_t, IFF::HairStyle > load_hair_style();

			// Match
			/*static*/ std::map< uint32_t, IFF::Match > load_match();

			// Skin
			/*static*/ std::map< uint32_t, IFF::Skin > load_skin();

			// Ability
			/*static*/ std::map< uint32_t, IFF::Ability > load_ability();

			// Desc
			/*static*/ std::map< uint32_t, IFF::Desc > load_desc();

			// GrandPrixAIOptionalData
			/*static*/ std::map< uint32_t, IFF::GrandPrixAIOptionalData > load_grand_prix_ai_optional_data();

			// GrandPrixConditionEquip
			/*static*/ std::map< uint32_t, IFF::GrandPrixConditionEquip > load_grand_prix_condition_equip();

			// GrandPrixData
			/*static*/ std::map< uint32_t, IFF::GrandPrixData > load_grand_prix_data();

			// GrandPrixRankReward
			/*static*/ std::vector< IFF::GrandPrixRankReward > load_grand_prix_rank_reward();

			// GrandPrixSpecialHole
			/*static*/ std::vector< IFF::GrandPrixSpecialHole > load_grand_prix_special_hole();

			// MemorialShopCoinItem
			/*static*/ std::map< uint32_t, IFF::MemorialShopCoinItem > load_memorial_shop_coin_item();

			// MemorialShopRareItem
			/*static*/ std::vector< IFF::MemorialShopRareItem > load_memorial_shop_rare_item();

			// AddonPart
			/*static*/ std::vector< IFF::AddonPart > load_addon_part();

			// ArtifactManaInfo
			/*static*/ std::map< uint32_t, IFF::ArtifactManaInfo > load_artifact_mana_info();

			// CaddieVoiceTable
			/*static*/ std::vector< IFF::CaddieVoiceTable > load_caddie_voice_table();

			// ErrorCodeInfo
			/*static*/ std::map< uint32_t, IFF::ErrorCodeInfo > load_error_code_info();

			// FurnitureAbility
			/*static*/ std::vector< IFF::FurnitureAbility > load_furniture_ability();

			// HoleCupDropItem
			/*static*/ std::map< uint32_t, IFF::HoleCupDropItem > load_hole_cup_drop_item();

			// LevelUpPrizeItem
			/*static*/ std::map< uint32_t, IFF::LevelUpPrizeItem > load_level_up_prize_item();

			// NonVisibleItemTable
			/*static*/ std::map< uint32_t, IFF::NonVisibleItemTable > load_non_visible_item_table();

			// PointShop
			/*static*/ std::map< uint32_t, IFF::PointShop > load_point_shop();

			// ShopLimitItem
			/*static*/ std::map< uint32_t, IFF::ShopLimitItem > load_shop_limit_item();

			// SpecialPrizeItem
			/*static*/ std::map< uint32_t, IFF::SpecialPrizeItem > load_special_prize_item();

			// SubscriptionItemTable
			/*static*/ std::map< uint32_t, IFF::SubscriptionItemTable > load_subscription_item_table();

			// SetEffectTable
			/*static*/ std::map< uint32_t, IFF::SetEffectTable > load_set_effect_table();

			// TikiPonitTable
			/*static*/ std::map< uint32_t, IFF::TikiPointTable > load_tiki_point_table();

			// TikiRecipe
			/*static*/ std::map< uint32_t, IFF::TikiRecipe > load_tiki_recipe();

			// TikiSpecialTable
			/*static*/ std::map< uint32_t, IFF::TikiSpecialTable > load_tiki_special_table();

			// TimeLimitItem
			/*static*/ std::map< uint32_t, IFF::TimeLimitItem > load_time_limit_item();

			// TwinsItemTable
			/*static*/ std::vector< IFF::TwinsItemTable > load_twins_item_table();

			// SetItem
			/*static*/ std::map< uint32_t, IFF::SetItem > load_set_item();

			// Mascot
			/*static*/ std::map< uint32_t, IFF::Mascot > load_mascot();

		public:
			// Metôdos auxíliares de Type ID Item
			// Part Base de Part Num, é o 0x400
			/*static*/ uint32_t getItemGroupIdentify(uint32_t _typeid);
			/*static*/ uint32_t getItemSubGroupIdentify24(uint32_t _typeid);
			/*static*/ uint32_t getItemSubGroupIdentify22(uint32_t _typeid);
			/*static*/ uint32_t getItemSubGroupIdentify21(uint32_t _typeid);	// Usado pelo SetItem separar os itens por secção
			/*static*/ uint32_t getItemCharIdentify(uint32_t _typeid);
			/*static*/ uint32_t getItemCharPartNumber(uint32_t _typeid);
			/*static*/ uint32_t getItemCharTypeNumber(uint32_t _typeid);
			/*static*/ uint32_t getItemIdentify(uint32_t _typeid);
			/*static*/ uint32_t getItemTitleNum(uint32_t _typeid);

			// Trofel Normal Type Identity (0x03FF0000 & _typeid >> 16)
			/*static */uint32_t getMatchTypeIdentity(uint32_t _typeid);

			/*static */uint32_t getCaddieItemType(uint32_t _typeid);
			/*static */uint32_t getCaddieIdentify(uint32_t _typeid);
			
			// Acho que eu fiz para usar no enchant de up stat de taqueira e character
			/*static */uint32_t getEnchantSlotStat(uint32_t _typeid);

			/*static */uint32_t getItemAuxPartNumber(uint32_t _typeid);

			/*static*/ uint32_t getGrandPrixAba(uint32_t _typeid);

			/*static*/ uint32_t getGrandPrixType(uint32_t _typeid);

			/*static*/ bool isGrandPrixEvent(uint32_t _typeid);

			/*static*/ bool isGrandPrixNormal(uint32_t _typeid);

		protected:
			/*static */std::map< uint32_t, IFF::Part > m_part;
			/*static */std::map< uint32_t, IFF::Item > m_item;
			/*static */std::map< uint32_t, IFF::SetItem > m_set_item;
			/*static */std::map< uint32_t, IFF::Mascot > m_mascot;
			/*static */std::map< uint32_t, IFF::Achievement > m_achievement;
			/*static */std::map< uint32_t, IFF::CounterItem > m_counter_item;
			/*static */std::map< uint32_t, IFF::QuestStuff > m_quest_stuff;
			/*static */std::map< uint32_t, IFF::QuestItem > m_quest_item;
			/*static */std::map< uint32_t, IFF::AuxPart > m_aux_part;
			/*static */std::map< uint32_t, IFF::Ball > m_ball;
			/*static */std::map< uint32_t, IFF::Caddie > m_caddie;
			/*static */std::map< uint32_t, IFF::CaddieItem > m_caddie_item;
			/*static */std::map< uint32_t, IFF::Card > m_card;
			/*static */std::map< uint32_t, IFF::Character > m_character;
			/*static */std::map< uint32_t, IFF::Club > m_club;
			/*static */std::map< uint32_t, IFF::ClubSet > m_club_set;
			/*static */std::map< uint32_t, IFF::ClubSetWorkShopLevelUpProb > m_club_set_work_shop_level_up_prob;
			/*static */std::map< uint32_t, IFF::ClubSetWorkShopRankUpExp > m_club_set_work_shop_rank_exp;
			/*static */std::map< uint32_t, IFF::Course > m_course;
			/*static */std::map< uint32_t, IFF::CutinInfomation > m_cutin_infomation;
			/*static */std::map< uint32_t, IFF::Enchant > m_enchant;
			/*static */std::map< uint32_t, IFF::Furniture > m_furniture;
			/*static */std::map< uint32_t, IFF::HairStyle > m_hair_style;
			/*static */std::map< uint32_t, IFF::Match > m_match;
			/*static */std::map< uint32_t, IFF::Skin > m_skin;
			/*static */std::map< uint32_t, IFF::Ability > m_ability;
			/*static */std::map< uint32_t, IFF::Desc > m_desc;
			/*static */std::map< uint32_t, IFF::GrandPrixAIOptionalData > m_grand_prix_ai_optinal_data;
			/*static */std::map< uint32_t, IFF::GrandPrixConditionEquip > m_grand_prix_condition_equip;
			/*static */std::map< uint32_t, IFF::GrandPrixData > m_grand_prix_data;
			/*static */std::map< uint32_t, IFF::MemorialShopCoinItem > m_memorial_shop_coin_item;
			/*static */std::map< uint32_t, IFF::ArtifactManaInfo > m_artifact_mana_info;
			/*static */std::map< uint32_t, IFF::ErrorCodeInfo > m_error_code_info;
			/*static */std::map< uint32_t, IFF::HoleCupDropItem > m_hole_cup_drop_item;
			/*static */std::map< uint32_t, IFF::LevelUpPrizeItem > m_level_up_prize_item;
			/*static */std::map< uint32_t, IFF::NonVisibleItemTable > m_non_visible_item_table;
			/*static */std::map< uint32_t, IFF::PointShop > m_point_shop;
			/*static */std::map< uint32_t, IFF::ShopLimitItem > m_shop_limit_item;
			/*static */std::map< uint32_t, IFF::SpecialPrizeItem > m_special_prize_item;
			/*static */std::map< uint32_t, IFF::SubscriptionItemTable > m_subscription_item_table;
			/*static */std::map< uint32_t, IFF::SetEffectTable > m_set_effect_table;
			/*static */std::map< uint32_t, IFF::TikiPointTable > m_tiki_point_table;
			/*static */std::map< uint32_t, IFF::TikiRecipe > m_tiki_recipe;
			/*static */std::map< uint32_t, IFF::TikiSpecialTable > m_tiki_special_table;
			/*static */std::map< uint32_t, IFF::TimeLimitItem > m_time_limit_item;

			/*static */std::vector< IFF::AddonPart > m_addon_part;
			/*static */std::vector< IFF::CadieMagicBox > m_cadie_magic_box;
			/*static */std::vector< IFF::CadieMagicBoxRandom > m_cadie_magic_box_random;
			/*static */std::vector< IFF::CharacterMastery > m_character_mastery;
			/*static */std::vector< IFF::ClubSetWorkShopLevelUpLimit > m_club_set_work_shop_level_up_limit;
			/*static */std::vector< IFF::GrandPrixRankReward > m_grand_prix_rank_reward;
			/*static */std::vector< IFF::GrandPrixSpecialHole > m_grand_prix_special_hole;
			/*static */std::vector< IFF::MemorialShopRareItem > m_memorial_shop_rare_item;
			/*static */std::vector< IFF::CaddieVoiceTable > m_caddie_voice_table;
			/*static */std::vector< IFF::FurnitureAbility > m_furniture_ability;
			/*static */std::vector< IFF::TwinsItemTable > m_twins_item_table;

			/*static */bool m_loaded;

#if defined(_WIN32)
			CRITICAL_SECTION m_cs;
#elif defined(__linux__)
			pthread_mutex_t m_cs;
#endif
    };

	typedef Singleton< iff > sIff;
}

#endif