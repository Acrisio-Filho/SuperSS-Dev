// Arquivo channel.h
// Criado em 24/12/2017 por Acrisio
// Definição da classe channel

#pragma once
#ifndef _STDA_CHANNEL_H
#define _STDA_CHANNEL_H

#include <vector>
#include "../SESSION/player.hpp"
#include "../TYPE/pangya_game_st.h"

//#include "lobby.h"
#include "room_manager.h"
#include <map>

#include "../TYPE/grand_zodiac_type.hpp"

#include "../../Projeto IOCP/PANGYA_DB/pangya_db.h"

#include "../TYPE/golden_time_type.hpp"
#include "../../Projeto IOCP/TYPE/smart_calculator_type.hpp"

namespace stdA {
    class channel {
		public:
			enum ESTADO : unsigned char {
				UNITIALIZED,
				INITIALIZED
			};

			enum LEAVE_ROOM_STATE : char {
				DO_NOTHING = -1,		// Faz nada
				SEND_UPDATE_CLIENT = 0,	// bug arm g++
				ROOM_DESTROYED,
			};

        public:
            channel(ChannelInfo _ci, uProperty _type);
            ~channel();

			void enterChannel(player& _session);
			void leaveChannel(player& _session);

			void checkEnterChannel(player& _session);

			ChannelInfo* getInfo();

			// Gets
			unsigned char getId();

			PlayerCanalInfo* getPlayerInfo(player *_session);

			// Check Invite Time
			void checkInviteTime();

			// stats
			bool isFull();

			// Lobby
			void enterLobby(player& _session, unsigned char _lobby);
			void leaveLobby(player& _session);

			// Lobby Multi player
			void enterLobbyMultiPlayer(player& _session);
			void leaveLobbyMultiPlayer(player& _session);

			// Lobby Grand Prix
			void enterLobbyGrandPrix(player& _session);
			void leaveLobbyGrandPrix(player& _session);

			// retorna todos os players elegíveis a participar do Evento Golden Time
			std::vector< stPlayerReward > getAllEligibleToGoldenTime();
			
			// Send FireWorks para os players que ganharam no Golden Time
			void sendFireWorksWinnerGoldenTime(std::vector< stPlayerReward >& _winners);

			// Room
			LEAVE_ROOM_STATE leaveRoom(player& _session, int _option);

			// Room Lobby Multiplayer
			LEAVE_ROOM_STATE leaveRoomMultiPlayer(player& _session, int _option);

			// Room Lobby Grand Prix
			LEAVE_ROOM_STATE leaveRoomGrandPrix(player& _session, int _option);

			// GM Kick player room
			LEAVE_ROOM_STATE kickPlayerRoom(player& _session, unsigned char force);

			std::vector< player* > getSessions(unsigned char _lobby = ~0);

			// make Grand Zodiac Event Room
			void makeGrandZodiacEventRoom(range_time& _rt);

			// make Bot GM Event Room
			void makeBotGMEventRoom(stRangeTime& _rt, std::vector< stReward > _reward);

			bool execSmartCalculatorCmd(player& _session, std::string& _msg, eTYPE_CALCULATOR_CMD _type);

		public:	// REQUEST FROM CLIENT
			// Lobby
			void requestEnterLobby(player& _session, packet *_packet);
			void requestExitLobby(player& _session, packet *_packet);
			void requestEnterLobbyGrandPrix(player& _session, packet *_packet);
			void requestExitLobbyGrandPrix(player& _session, packet *_packet);

			// Spy (GM) observer
			void requestEnterSpyRoom(player& _session, packet *_packet);

			// Room
			void requestMakeRoom(player& _session, packet *_packet);
			void requestEnterRoom(player& _session, packet* _packet);
			void requestChangeInfoRoom(player& _session, packet* _packet);
			void requestExitRoom(player& _session, packet* _packet);
			void requestShowInfoRoom(player& _session, packet* _packet);
			void requestPlayerLocationRoom(player& _session, packet* _packet);
			void requestChangePlayerStateReadyRoom(player& _session, packet* _packet);
			void requestKickPlayerOfRoom(player& _session, packet* _packet);
			void requestChangePlayerTeamRoom(player& _session, packet* _packet);
			void requestChangePlayerStateAFKRoom(player& _session, packet* _packet);
			void requestPlayerStateCharacterLounge(player& _session, packet* _packet);
			void requestToggleAssist(player& _session, packet *_packet);
			void requestInvite(player& _session, packet *_packet);
			void requestCheckInvite(player& _session, packet *_packet);	// Esse aqui o O Server Original nao retorna nada para o cliente, acho que é só um check
			void requestChatTeam(player& _session, packet *_packet);

			// Request Player sai do Web Guild, verifica se tem alguma atualização para passar para o player no server
			void requestExitedFromWebGuild(player& _session, packet* _packet);

			// Request Game
			void requestStartGame(player& _session, packet *_packet);
			void requestInitHole(player& _session, packet *_packet);
			void requestFinishLoadHole(player& _session, packet *_packet);
			void requestFinishCharIntro(player& _session, packet *_packet);
			void requestFinishHoleData(player& _session, packet *_packet);

			// Server enviou a resposta do InitShot para o cliente
			void requestInitShotSended(player& _session, packet *_packet);

			void requestInitShot(player& _session, packet *_packet);
			void requestSyncShot(player& _session, packet *_packet);
			void requestInitShotArrowSeq(player& _session, packet *_packet);
			void requestShotEndData(player& _session, packet *_packet);
			void requestFinishShot(player& _session, packet *_packet);

			void requestChangeMira(player& _session, packet *_packet);
			void requestChangeStateBarSpace(player& _session, packet *_packet);
			void requestActivePowerShot(player& _session, packet *_packet);
			void requestChangeClub(player& _session, packet *_packet);
			void requestUseActiveItem(player& _session, packet *_packet);
			void requestChangeStateTypeing(player& _session, packet *_packet);
			void requestMoveBall(player& _session, packet *_packet);
			void requestChangeStateChatBlock(player& _session, packet *_packet);
			void requestActiveBooster(player& _session, packet *_packet);
			void requestActiveReplay(player& _session, packet *_packet);
			void requestActiveCutin(player& _session, packet *_packet);
			void requestActiveAutoCommand(player& _session, packet *_packet);
			void requestActiveAssistGreen(player& _session, packet *_packet);

			// VersusBase
			void requestLoadGamePercent(player& _session, packet *_packet);
			void requestMarkerOnCourse(player& _session, packet *_packet);
			void requestStartTurnTime(player& _session, packet *_packet);
			void requestUnOrPauseGame(player& _session, packet *_packet);
			void requestLastPlayerFinishVersus(player& _session, packet *_packet);
			void requestReplyContinueVersus(player& _session, packet *_packet);

			// Match
			void requestTeamFinishHole(player& _session, packet *_packet);

			// Practice
			void requestLeavePractice(player& _session, packet *_packet);

			// Tourney
			void requestUseTicketReport(player& _session, packet *_packet);

			// Grand Zodiac
			void requestLeaveChipInPractice(player& _session, packet *_packet);
			void requestStartFirstHoleGrandZodiac(player& _session, packet *_packet);
			void requestReplyInitialValueGrandZodiac(player& _session, packet *_packet);

			// Ability Item
			void requestActiveRing(player& _session, packet *_packet);
			void requestActiveRingGround(player& _session, packet *_packet);
			void requestActiveRingPawsRainbowJP(player& _session, packet *_packet);
			void requestActiveRingPawsRingSetJP(player& _session, packet *_packet);
			void requestActiveRingPowerGagueJP(player& _session, packet *_packet) ;
			void requestActiveRingMiracleSignJP(player& _session, packet *_packet);
			void requestActiveWing(player& _session, packet *_packet);
			void requestActivePaws(player& _session, packet *_packet);
			void requestActiveGlove(player& _session, packet *_packet);
			void requestActiveEarcuff(player& _session, packet *_packet);

			// Request Enter Game After Started
			void requestEnterGameAfterStarted(player& _session, packet *_packet);

			void requestFinishGame(player& _session, packet* _packet);

			void requestChangeWindNextHoleRepeat(player& _session, packet *_packet);

			// Grand Prix
			void requestEnterRoomGrandPrix(player& _session, packet *_packet);
			void requestExitRoomGrandPrix(player& _session, packet *_packet);

			// Player Report Chat Game
			void requestPlayerReportChatGame(player& _session, packet *_packet);

			// Common Command GM
			void requestExecCCGVisible(player& _session, packet *_packet);
			void requestExecCCGChangeWindVersus(player& _session, packet *_packet);
			void requestExecCCGChangeWeather(player& _session, packet *_packet);
			void requestExecCCGGoldenBell(player& _session, packet *_packet);
			void requestExecCCGIdentity(player& _session, packet *_packet);
			void requestExecCCGKick(player& _session, packet *_packet);
			void requestExecCCGDestroy(player& _session, packet *_packet);

			// MyRoom
			void requestChangePlayerItemMyRoom(player& _session, packet *_packet);
			void requestOpenTicketReportScroll(player& _session, packet *_packet);
			void requestChangeMascotMessage(player& _session, packet *_packet);

			// Caddie Extend Days and Set Notice Holyday Caddie
			void requestPayCaddieHolyDay(player& _session, packet *_packet);
			void requestSetNoticeBeginCaddieHolyDay(player& _session, packet *_packet);

			// Shop
			void requestBuyItemShop(player& _session, packet *_packet);
			void requestGiftItemShop(player& _session, packet *_packet);

			// MyRoom Extend or Remove Part Rental
			void requestExtendRental(player& _session, packet *_packet);
			void requestDeleteRental(player& _session, packet *_packet);

			// Attendance reward, Premios por logar no pangya
			void requestCheckAttendanceReward(player& _session, packet *_packet);
			void requestAttendanceRewardLoginCount(player& _session, packet *_packet);

			// Daily Quest
			void requestDailyQuest(player& _session, packet *_packet);
			void requestAcceptDailyQuest(player& _session, packet *_packet);
			void requestTakeRewardDailyQuest(player& _session, packet *_packet);
			void requestLeaveDailyQuest(player& _session, packet *_packet);

			// Cadie's Cauldron
			void requestCadieCauldronExchange(player& _session, packet *_packet);

			// Character Stats
			void requestCharacterStatsUp(player& _session, packet *_packet);
			void requestCharacterStatsDown(player& _session, packet *_packet);
			void requestCharacterMasteryExpand(player& _session, packet *_packet);
			void requestCharacterCardEquip(player& _session, packet *_packet);
			void requestCharacterCardEquipWithPatcher(player& _session, packet *_packet);
			void requestCharacterRemoveCard(player& _session, packet *_packet);

			// ClubSet Enchant
			void requestClubSetStatsUpdate(player& _session, packet *_packet);

			// Tiki's Shop
			void requestTikiShopExchangeItem(player& _session, packet *_packet);

			// Item Equipado
			void requestChangePlayerItemChannel(player& _session, packet *_packet);
			void requestChangePlayerItemRoom(player& _session, packet *_packet);

			// Delete Active Item
			void requestDeleteActiveItem(player& _session, packet *_packet);

			// ClubSet WorkShop
			void requestClubSetWorkShopTransferMasteryPts(player& _session, packet *_packet);
			void requestClubSetWorkShopRecoveryPts(player& _session, packet *_packet);
			void requestClubSetWorkShopUpLevel(player& _session, packet *_packet);
			void requestClubSetWorkShopUpLevelConfirm(player& _session, packet *_packet);
			void requestClubSetWorkShopUpLevelCancel(player& _session, packet *_packet);
			void requestClubSetWorkShopUpRank(player& _session, packet *_packet);
			void requestClubSetWorkShopUpRankTransformConfirm(player& _session, packet *_packet);
			void requestClubSetWorkShopUpRankTransformCancel(player& _session, packet *_packet);

			// ClubSet Reset
			void requestClubSetReset(player& _session, packet *_packet);

			// Tutorial
			void requestMakeTutorial(player& _session, packet *_packet);

			// Web Link
			void requestEnterWebLinkState(player& _session, packet *_packet);

			// Pede o Cookies
			void requestCookie(player& _session, packet *_packet);

			// Pede para atualizar Gacha Coupon(s)
			void requestUpdateGachaCoupon(player& _session, packet *_packet);

			// Box System, Box que envia para o MailBox e a Box que envia direto para o MyRoom
			void requestOpenBoxMail(player& _session, packet *_packet);
			void requestOpenBoxMyRoom(player& _session, packet *_packet);

			// Memorial System
			void requestPlayMemorial(player& _session, packet *_packet);

			// Card System
			void requestOpenCardPack(player& _session, packet *_packet);
			void requestLoloCardCompose(player& _session, packet* _packet);

			// Card Special/ Item Buff
			void requestUseCardSpecial(player& _session, packet *_packet);
			void requestUseItemBuff(player& _session, packet *_packet);

			// Comet Refill
			void requestCometRefill(player& _session, packet *_packet);

			// MailBox
			void requestOpenMailBox(player& _session, packet* _packet);
			void requestInfoMail(player& _session, packet *_packet);
			void requestSendMail(player& _session, packet *_packet);
			void requestTakeItemFomMail(player& _session, packet *_packet);
			void requestDeleteMail(player& _session, packet *_packet);

			// Dolfini Locker
			void requestMakePassDolfiniLocker(player& _session, packet *_packet);
			void requestCheckDolfiniLockerPass(player& _session, packet *_packet);
			void requestChangeDolfiniLockerPass(player& _session, packet *_packet);
			void requestChangeDolfiniLockerModeEnter(player& _session, packet *_packet);
			void requestDolfiniLockerItem(player& _session, packet *_packet);
			void requestDolfiniLockerPang(player& _session, packet *_packet);
			void requestUpdateDolfiniLockerPang(player& _session, packet *_packet);
			void requestAddDolfiniLockerItem(player& _session, packet *_packet);
			void requestRemoveDolfiniLockerItem(player& _session, packet *_packet);

			// Legacy Tiki Shop (Point Shop)
			void requestOpenLegacyTikiShop(player& _session, packet* _packet);
			void requestPointLegacyTikiShop(player& _session, packet* _packet);
			void requestExchangeTPByItemLegacyTikiShop(player& _session, packet* _packet);
			void requestExchangeItemByTPLegacyTikiShop(player& _session, packet* _packet);

			// Personal Shop
			void requestOpenEditSaleShop(player& _session, packet *_packet);
			void requestCloseSaleShop(player& _session, packet *_packet);
			void requestChangeNameSaleShop(player& _session, packet *_packet);
			void requestOpenSaleShop(player& _session, packet *_packet);
			void requestVisitCountSaleShop(player& _session, packet *_packet);
			void requestPangSaleShop(player& _session, packet *_packet);
			void requestCancelEditSaleShop(player& _session, packet *_packet);
			void requestViewSaleShop(player& _session, packet *_packet);
			void requestCloseViewSaleShop(player& _session, packet *_packet);
			void requestBuyItemSaleShop(player& _session, packet *_packet);

			// Papel Shop
			void requestOpenPapelShop(player& _session, packet *_packet);
			void requestPlayPapelShop(player& _session, packet *_packet);
			void requestPlayPapelShopBIG(player& _session, packet *_packet);

			// Msg Chat da Sala
			void requestSendMsgChatRoom(player& _session, std::string _msg);

			// senders
			void sendUpdateRoomInfo(RoomInfoEx& _ri, int _option);
			void sendUpdatePlayerInfo(player& _session, int _option);

			// Destroy Room
			void destroyRoom(short _number);

		protected:
			// Tourney Tempo que pode entrar no tourney depois de ter começado acabou troca o info da sala
			// Arg1 Channel ponteiro, Arg2 Numero da Sala
			static int _enter_left_time_is_over(void* _arg1, void* _arg2);

        protected:
            std::vector< player* > v_sessions;
			std::map< player*, PlayerCanalInfo > m_player_info;

			std::vector< InviteChannelInfo > v_invite;

			void addInviteTimeRequest(InviteChannelInfo& _ici);
			void deleteInviteTimeRequest(InviteChannelInfo& _ici);
			void deleteInviteTimeResquestByInvited(player& _session);

			// Tira o request do convidado da sala[Character] o tempo acabou para ele responder ao convite
			bool send_time_out_invite(InviteChannelInfo& _ici);
			void clear_invite_time();

			void removeSession(player *_session);
			void addSession(player *_session);

			player* findSessionByOID(uint32_t _oid);
			player* findSessionByUID(uint32_t _uid);
			player* findSessionByNickname(std::string _nickname);

			size_t findIndexSession(player *_session);
			void makePlayerInfo(player& _session);
			void updatePlayerInfo(player& _session);
			void deletePlayerInfo(player& _session);

		protected:
			static void SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg);

        private:
            ChannelInfo m_ci;
			RoomManager m_rm;

			uProperty m_type;			// Type GrandPrix, Natural, Normal

			unsigned char m_state;

#if defined(_WIN32)
			CRITICAL_SECTION m_cs;
			CRITICAL_SECTION m_cs_invite;	// Critical Section dos Invite Time
#elif defined(__linux__)
			pthread_mutex_t m_cs;
			pthread_mutex_t m_cs_invite;	// Critical Section dos Invite Time
#endif
    };
}

#endif