// Arquivo room.h
// Criado em 24/12/2017 por Acrisio
// Definição da classe room

#pragma once
#ifndef _STDA_ROOM_H
#define _STDA_ROOM_H

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#include <pthread.h>
#include <unistd.h>
#endif

#include <vector>
#include "../SESSION/player.hpp"
#include "../TYPE/pangya_game_st.h"
#include "../GAME/personal_shop_manager.hpp"

#include "game.hpp"

#include "team.hpp"
#include "guild_room_manager.hpp"

#include "../../Projeto IOCP/TYPE/smart_calculator_type.hpp"

#include <string>
#include <map>

namespace stdA {
    class room {
        public:
            room(unsigned char _channel_owner, RoomInfoEx _ri);
            virtual ~room();

			void destroy();

			void enter(player& _session);
			int leave(player& _session, int _option);
			int leaveAll(int _option);

			bool isInvited(player& _session);
			InviteChannelInfo addInvited(uint32_t _uid_has_invite, player& _session);
			InviteChannelInfo deleteInvited(player& _session);
			InviteChannelInfo deleteInvited(uint32_t _uid);

			const RoomInfoEx* getInfo();

			// Gets
			const unsigned char getChannelOwenerId();
			const short getNumero();
			const uint32_t getMaster();
			const uint32_t getNumPlayers();
			const uint32_t getPosition(player *_session);

			PlayerRoomInfoEx* getPlayerInfo(player *_session);

			std::vector< player* > getSessions(player *_session = nullptr, bool _with_invited = true);

			uint32_t getRealNumPlayersWithoutInvited();
			bool haveInvited();

			// Sets
			void setNome(std::string _nome);
			void setSenha(std::string _senha);
			void setTipo(unsigned char _tipo);
			void setCourse(unsigned char _course);
			void setQntdHole(unsigned char _qntd_hole);
			void setModo(unsigned char _modo);
			void setTempoVS(uint32_t _tempo);
			void setMaxPlayer(unsigned char _max_player);
			void setTempo30S(uint32_t _tempo);
			void setHoleRepeat(unsigned char _hole_repeat);
			void setFixedHole(uint32_t _fixed_hole);
			void setArtefato(uint32_t _artefato);
			void setNatural(uint32_t _natural);

			void setState(unsigned char _state);
			void setFlag(unsigned char _flag);

			void setStateAFK(unsigned char _state_afk);

			// Checks
			bool checkPass(std::string _pass);

			// Verifica se o Player tem um loja aberta no lounge e se o item está à venda nela
			bool checkPersonalShopItem(player& _session, int32_t _item_id);

			// States
			bool isLocked();
			bool isFull();
			bool isGaming();							// Já começou o jogo
			bool isGamingBefore(uint32_t _uid);	// Já estava no jogo antes
			bool isKickedPlayer(uint32_t _uid);

			virtual bool isAllReady();

			// Updates
			void updatePlayerInfo(player& _session);

			// Finds
			player* findSessionByOID(uint32_t _oid);
			player* findSessionByUID(uint32_t _uid);

			player* findMaster();

			// Bot Tourney, Short Game and Special Shuffle Course
			void makeBot(player& _session);

			// Info Room
			bool requestChangeInfoRoom(player& _session, packet *_packet);

			// Chat Team
			void requestChatTeam(player& _session, packet *_packet);

			// Change Item Equiped of player
			virtual void requestChangePlayerItemRoom(player& _session, ChangePlayerItemRoom& _cpir);

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

			// Room Wait
			void requestToggleAssist(player& _session, packet *_packet);
			void requestChangeTeam(player& _session, packet *_packet);

			// Request Game
			virtual bool requestStartGame(player& _session, packet *_packet);
			void requestInitHole(player& _session, packet *_packet);
			bool requestFinishLoadHole(player& _session, packet *_packet);
			void requestFinishCharIntro(player& _session, packet *_packet);
			void requestFinishHoleData(player& _session, packet *_packet);

			// Server enviou a resposta do InitShot para o cliente
			void requestInitShotSended(player& _session, packet *_packet);

			void requestInitShot(player& _session, packet *_packet);
			void requestSyncShot(player& _session, packet *_packet);
			void requestInitShotArrowSeq(player& _session, packet *_packet);
			void requestShotEndData(player& _session, packet *_packet);
			RetFinishShot requestFinishShot(player& _session, packet *_packet);

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
			bool requestLastPlayerFinishVersus(player& _session, packet *_packet);
			bool requestReplyContinueVersus(player& _session, packet *_packet);

			// Match
			void requestTeamFinishHole(player& _session, packet *_packet);

			void testeDegree();

			// Practice
			void requestLeavePractice(player& _session, packet *_packet);

			// Tourney
			bool requestUseTicketReport(player& _session, packet *_packet);

			// Grand Zodiac
			void requestLeaveChipInPractice(player& _session, packet *_packet);
			void requestStartFirstHoleGrandZodiac(player& _session, packet *_packet);
			void requestReplyInitialValueGrandZodiac(player& _session, packet *_packet);

			// Ability Item
			void requestActiveRing(player& _session, packet *_packet);
			void requestActiveRingGround(player& _session, packet *_packet);
			void requestActiveRingPawsRainbowJP(player& _session, packet *_packet);
			void requestActiveRingPawsRingSetJP(player& _session, packet *_packet);
			void requestActiveRingPowerGagueJP(player& _session, packet *_packet);
			void requestActiveRingMiracleSignJP(player& _session, packet *_packet);
			void requestActiveWing(player& _session, packet *_packet);
			void requestActivePaws(player& _session, packet *_packet);
			void requestActiveGlove(player& _session, packet *_packet);
			void requestActiveEarcuff(player& _session, packet *_packet);

			void requestSendTimeGame(player& _session);
			bool requestEnterGameAfterStarted(player& _session);
			void requestUpdateEnterAfterStartedInfo(player& _session, EnterAfterStartInfo& _easi);

			bool requestFinishGame(player& _session, packet *_packet);

			void requestChangeWindNextHoleRepeat(player& _session, packet *_packet);

			void requestPlayerReportChatGame(player& _session, packet *_packet);

			// Common Command GM
			void requestExecCCGChangeWindVersus(player& _session, packet *_packet);
			void requestExecCCGChangeWeather(player& _session, packet *_packet);
			void requestExecCCGGoldenBell(player& _session, packet *_packet);

			// Smart Calculator Command
			virtual bool execSmartCalculatorCmd(player& _session, std::string& _msg, eTYPE_CALCULATOR_CMD _type);

			// Pede o Hole que o player está, 
			// se eles estiver jogando ou 0 se ele não está jogando
			unsigned char requestPlace(player& _session);

			virtual void startGame(player& _session);

			// Time Tourney After Enter IN GAME
			void requestStartAfterEnter(job& _job);
			void requestEndAfterEnter();

			// senders
			void sendMake(player& _session);
			void sendUpdate();
			void sendCharacter(player& _session, int _option);
			void sendCharacterStateLounge(player& _session);
			void sendWeatherLounge(player& _session);

			// Locker
			void lock();
			bool tryLock();
			void unlock();

			std::vector< InviteChannelInfo >& getAllInvite();

			void setDestroying();

			static void SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg);

		protected:
			size_t findIndexSession(player *_session);
			size_t findIndexSession(uint32_t _uid);

			PlayerRoomInfoEx* makePlayerInfo(player& _session);
			PlayerRoomInfoEx* makePlayerInvitedInfo(player& _session);	// Cria o Info do Player Convidado

			void geraSecurityKey();

			void updatePosition();

			void updateTrofel();
			void updateMaster(player* _session);
			void updateGuild(player& _session);

			// Jogador Chutado da sala
			void clear_player_kicked();
			void addPlayerKicked(uint32_t _uid);

			// Game
			virtual void finish_game();

			// Invite
			void clear_invite();

			// Team
			void init_teans();
			void clear_teans();

			// Add Bot Tourney Visual to Room
			void addBotVisual(player& _session);

			// Para as classes filhas, empedir que exclua a sala dependendo do se tem player ou não na sala
			virtual bool isDropRoom();

			// protected por que é o método unsave(inseguro), sem thread safe
			uint32_t _getRealNumPlayersWithoutInvited();

			// protected por que é o método unsave(inseguro), sem thread safe
			bool _haveInvited();

        protected:
            std::vector< player* > v_sessions;
			std::map< player*, PlayerRoomInfoEx > m_player_info;
			std::map< uint32_t/*UID*/, bool > m_player_kicked;

			PersonalShopManager m_personal_shop;

			std::vector< Team > m_teans;

			GuildRoomManager m_guild_manager;

			std::vector< InviteChannelInfo > v_invite;

#if defined(_WIN32)
			CRITICAL_SECTION m_lock_cs;		// Bloquea a sala
#elif defined(__linux__)
			pthread_mutex_t m_lock_cs;
#endif

			int32_t m_lock_spin_state;			// Estado do spin(count) do bloquea da sala

#if defined(_WIN32)
			CRITICAL_SECTION m_cs;
#elif defined(__linux__)
			pthread_mutex_t m_cs;
#endif

		protected:
			// Room Tipo Lounge
			unsigned char m_weather_lounge;

			void calcRainLounge();

        protected:
            RoomInfoEx m_ri;

			unsigned char m_channel_owner;	// Id do Canal dono da sala

			bool m_bot_tourney;		// Bot para começa o Modo tourney só com 1 jogador

			bool m_destroying;

			Game *m_pGame;
    };
}

#endif