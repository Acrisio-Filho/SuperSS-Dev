// Aquivo iff_writer.hpp
// Criado em 03/06/2019 as 19:44 por Acrisio
// Defini��o da classe iff_writer

#pragma once
#ifndef _STDA_IFF_WRITER_HPP
#define _STDA_IFF_WRITER_HPP

#include "../../Projeto IOCP/UTIL/iff.h"

namespace stdA {
	class iff_writer : public iff {
		public:
			iff_writer();
			~iff_writer();

			void saveAll();

			void saveBall();
			void saveCaddie();
			void saveCaddieItem();
			void saveCard();
			void saveCharacter();
			void saveClubSet();
			void saveHairStyle();
			void saveItem();
			void saveMascot();
			void savePart();
			void saveAuxPart();
			void saveSetItem();
			void saveSkin();
			void saveCadieMagicBox();
			void saveAchievement();
			void saveQuestItem();
			void saveQuestStuff();
			void saveCounterItem();
			void saveCadieMagicBoxRandom();
			void saveCharacterMastery();
			void saveClub();
			void saveClubSetWorkShopLevelUpLimit();
			void saveClubSetWorkShopLevelUpProb();
			void saveClubSetWorkShopRankUpExp();
			void saveCourse();
			void saveCutinInfomation();
			void saveEnchant();
			void saveFurniture();
			void saveMatch();
			void saveAbility();
			void saveDesc();
			void saveGrandPrixAIOptionalData();
			void saveGrandPrixConditionEquip();
			void saveGrandPrixData();
			void saveGrandPrixRankReward();
			void saveGrandPrixSpecialHole();
			void saveMemorialShopCoinItem();
			void saveMemorialShopRareItem();
			void saveAddonPart();
			void saveArtifactManaInfo();
			void saveCaddieVoiceTable();
			void saveErrorCodeInfo();
			void saveFurnitureAbility();
			void saveHoleCupDropItem();
			void saveLevelUpPrizeItem();
			void saveNonVisibleItemTable();
			void savePointShop();
			void saveShopLimitItem();
			void saveSpecialPrizeItem();
			void saveSubscriptionItemTable();
			void saveSetEffectTable();
			void saveTikiPointTable();
			void saveTikiRecipe();
			void saveTikiSpecialTable();
			void saveTimeLimitItem();
			void saveTwinsItemTable();

	};
}

#endif // !_STDA_IFF_WRITER_HPP
