// Arquivo pangya_game_db.h
// Criado em 13/08/2017 por Acrisio
// Definição da classe pangya_db para o Game Server

#pragma once
#ifndef _STDA_PANGYA_GAME_DB_H
#define _STDA_PANGYA_GAME_DB_H

#include "../../Projeto IOCP/TYPE/stdAType.h"
#include "../../Projeto IOCP/DATABASE/pangya_db.h"
#include "../TYPE/pangya_game_st.h"
#include <vector>
#include <string>
#include <map>

namespace stdA {
    class pangya_db : public pangya_base_db {
        public:
            pangya_db();
            ~pangya_db();

			static player_info getPlayerInfo(int32_t uid);
			static std::string getAuthKey(int32_t uid);
			// Esse auth Key Login, depois tem que mexer no DB para precisar do UID do server que vai entrar
			// porque o pangya sempre passa o uid do server quando pede ele, "esse deve ser o authkey championship que estava
			// no dump do server 4.9 original"
			static std::string getAuthKeyLogin(int32_t uid);
			static MemberInfoEx getMemberInfo(int32_t uid);
			static UserInfo getUserInfo(int32_t uid);
			static TrofelInfo getTrofelInfo(int32_t uid, int season);
			static UserEquip getUserEquip(int32_t uid);
			static std::vector< MapStatistics > getMapStatistics(int32_t uid, int season);
			static std::vector< MapStatistics > getMapStatisticsAssist(int32_t uid, int season);
			static std::vector< CharacterInfo > getCharacterInfo(int32_t uid);
			static CharacterInfo getCharacterInfoOne(int32_t uid, int32_t character_id);
			static std::vector< CaddieInfo > getCaddieInfo(int32_t uid);
			static ClubSetInfo getClubSetInfo(int32_t uid, int32_t clubset_id);
			static std::vector< MascotInfoEx > getMascotInfo(int32_t uid);
			static std::vector< WarehouseItem > getWarehouseItem(int32_t uid);
			static TutorialInfo getTutorialInfo(int32_t uid);
			static uint64_t getCookie(int32_t uid);
			static CouponGacha getCouponGacha(int32_t uid);
			static std::vector< CourseRewardInfo > getCourseRewardInfo();
			static std::vector< CounterItemInfo > getCounterItemInfo(int32_t uid);
			static std::map< size_t, AchievementInfo > getAchievementInfo(int32_t uid);
			static std::vector< CardInfo > getCardInfo(int32_t uid);
			static std::vector< CardEquipInfo > getCardEquipInfo(int32_t uid);
			static std::vector< YamUsed > getYamUsed(int32_t uid);
			static std::vector< TrofelEspecialInfo > getTrofelEspecialInfo(int32_t uid, int season);
			static std::vector< TrofelEspecialInfo > getTrofelGrandPrix(int32_t uid, int season);

			static MyRoomConfig getMyRoomConfig(int32_t uid);
			static std::vector< MyRoomItem > getMyRoomItem(int32_t uid);
			static uint32_t checkDolfiniLockerPassword(int32_t uid);

			static std::vector< MailBox > getEmailNaoLido(int32_t uid);
			static std::vector< MailBox > getEmailBox(int32_t uid);
			static EmailInfo getEmailInfo(int32_t uid, int32_t email_id);
			static void deleteEmail(int32_t _uid, int32_t* a_email_id, size_t num_email);
			
			static std::vector< stItem > getItemEmailAndAtt(int32_t uid, int32_t email_id);
			static std::vector< CaddieInfo > getCaddieFerias(int32_t uid);
			static std::vector< MsgOffInfo > getMsgOffInfo(int32_t uid);
			static AttendanceRewardInfo getAttendanceRewardInfo(int32_t uid);
			static AttendanceRewardInfo checkAttendanceReward(int32_t uid);
			static Last5PlayersGame getLast5PlayersGame(int32_t uid);
			static KeysOfLogin getKeysOfLogin(int32_t uid);
			static PremiumTicket getPremiumTicket(int32_t uid);
			static DailyQuest getDailyQuest(int32_t uid);
			static GuildInfo getGuildInfo(int32_t uid, int option);
			static std::vector< ServerInfo > getMSNServer();
			static bool checkDailyQuestPlayer(int32_t uid);
			static std::vector< RemoveDailyQuest > getOldDailyQuestPlayer(int32_t uid);
			
			static int32_t addDailyQuest(int32_t uid, std::string quest_name, uint32_t quest_typeid, int tipo, int option, int32_t stuff_typeid, uint32_t count_objective);
			static void setDailyQuest(int32_t uid);
			static void removeDailyQuest(int32_t uid, std::vector< RemoveDailyQuest >& remove);

			static void updateItemEquipSlot(int32_t _uid, int32_t _item_slot[10]);
			static void updateCaddieEquiped(int32_t _uid, int32_t _item_id);
			static void updateBallEquiped(int32_t _uid, int32_t _item_typeid);
			static void updateClubSetEquiped(int32_t _uid, int32_t _item_id);
			static void updateCharacterEquiped(int32_t _uid, int32_t _item_id);
			static void updateMascotEquiped(int32_t _uid, int32_t _item_id);
			static void updateCharacterCutinEquiped(int32_t _uid, CharacterInfo& _ci);
			static void updateSkinEquiped(int32_t _uid, UserEquip& _ue);
			static void updatePosterEquiped(int32_t _uid, UserEquip& _ue);
			static void updateCharacterAllPartEquiped(int32_t _uid, CharacterInfo& _ci);

			static std::string GeraWebKey(int32_t uid);

			static int32_t addItem(int32_t _uid, CharacterInfo& _item, unsigned short _gift_flag, unsigned short _purchase);

			static int32_t addItem(int32_t _uid, CaddieInfo& _item, unsigned short _gift_flag, unsigned short _purchase);

			static int32_t addItem(int32_t _uid, MascotInfoEx& _item, unsigned short _gift_flag, unsigned short _purchase);

			// Furniture
			static int32_t addItem(int32_t _uid, MyRoomItem& _item, unsigned short _gift_flag, unsigned short _purchase);

			static int32_t addItem(int32_t _uid, CounterItemInfo& _item, unsigned short _gift_flag, unsigned short _purchase);

			static int32_t addItem(int32_t _uid, WarehouseItem& _item, unsigned short _flag_iff, unsigned short _gift_flag, unsigned short _purchase);

			static int32_t addItem(int32_t _uid, AchievementInfo& _item, unsigned short _gift_flag, unsigned short _purchase);

			static int32_t addItem(int32_t _uid, CardInfo& _item, unsigned short _gift_flag, unsigned short _purchase);

			static int32_t addHairStyle(int32_t _uid, CharacterInfo& _item);

			static int32_t addCaddieItem(int32_t _uid, CaddieInfo& _item);

			static int32_t addTrofelSpecial(int32_t _uid, uint32_t trofel_typeid);
			static int32_t addTrofelGrandPrix(int32_t _uid, uint32_t trofel_typeid);
    };
}

#endif