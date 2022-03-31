// Arquivo game_server.h
// Criado em 17/12/2017 por Acrisio
// Definição da classe game_server


#pragma once
#ifndef _STDA_GAME_SERVER_H
#define _STDA_GAME_SERVER_H

#include "../../Projeto IOCP/Server/server.h"
#include "../SESSION/player_manager.hpp"

#include "../GAME/channel.h"

#include <vector>

#include "../../Projeto IOCP/SOCKET/socket.h"

#include "../GAME/login_manager.hpp"

#include "../../Projeto IOCP/TIMER/timer_manager.h"

#include "../../Projeto IOCP/TYPE/singleton.h"

#include "../UTIL/broadcast_list.hpp"

#include <cstdint>
#if INTPTR_MAX == INT64_MAX && MY_GG_SRV_LIB == 0
#include "../UNIT/unit_gg_auth_server_connect.hpp"
#include "../UNIT/unit_gg_interface.hpp"
#elif INTPTR_MAX == INT32_MAX || MY_GG_SRV_LIB == 1
#include "../CSAuth/GGAuth.hpp"
#else
#error Unknown pointer size or missing size macros!
#endif

#include "../../Projeto IOCP/TYPE/smart_calculator_type.hpp"

namespace stdA {
	class game_server : public server
#if INTPTR_MAX == INT64_MAX && MY_GG_SRV_LIB == 0
		, public IUnitGGAuthServer
#endif
	{
		public:
			game_server();
			~game_server();

			void clear();

			channel *enterChannel(player& _session, unsigned char _channel);

			void sendChannelListToSession(player& _session);
			void sendServerListAndChannelListToSession(player& _session);
			void sendDateTimeToSession(player& _session);

			void sendRankServer(player& _session);

			channel* findChannel(uint32_t _channel);

			player* findPlayer(uint32_t _uid, bool _oid = false);

			// find All GM Online
			std::vector< player* > findAllGM();

			void blockOID(uint32_t _oid);
			void unblockOID(uint32_t _oid);

			ServerInfoEx& getInfo() { return m_si; };
			DailyQuestInfo& getDailyQuestInfo() { return m_dqi; };

			LoginManager& getLoginManager();

			virtual void sendSmartCalculatorReplyToPlayer(const uint32_t _uid, std::string _from, std::string _msg) override;
			virtual void sendNoticeGMFromDiscordCmd(std::string& _notice) override;

			// Login
			void requestLogin(player& _session, packet *_packet);

			// Channel
			void requestEnterChannel(player& _session, packet *_packet);

			void requestEnterOtherChannelAndLobby(player& _session, packet* _packet);

			// Change Server
			void requestChangeServer(player& _session, packet *_packet);

			// UCC::Self Design System [Info, Save, Web Key]
			void requestUCCWebKey(player& _session, packet *_packet);
			void requestUCCSystem(player& _session, packet *_packet);

			// Chat
			void requestChat(player& _session, packet *_packet);

			// Chat Macro
			void requestChangeChatMacroUser(player& _session, packet *_packet);

			// Request Player Info
			void requestPlayerInfo(player& _session, packet *_packet);

			// Private Message
			void requestPrivateMessage(player& _session, packet *_packet);
			void requestChangeWhisperState(player& _session, packet *_packet);
			void requestNotifyNotDisplayPrivateMessageNow(player& _session, packet *_packet);

			// Command GM
			void requestCommonCmdGM(player& _session, packet *_packet);
			void requestCommandNoticeGM(player& _session, packet *_packet);

			// Request translate Sub Packet
			void requestTranslateSubPacket(player& _session, packet *_packet);

			// Ticker
			void requestSendTicker(player& _session, packet *_packet);
			void requestQueueTicker(player& _session, packet *_packet);

			// Exception Client Message
			void requestExceptionClientMessage(player& _session, packet *_packet);

			// Game Guard Auth
			void requestCheckGameGuardAuthAnswer(player& _session, packet *_packet);

			// Timer
			timer* makeTime(DWORD _milleseconds, job& _job);
			timer* makeTime(DWORD _milleseconds, job& _job, std::vector< DWORD > _table_interval);
			void unMakeTime(timer* _timer);

			// Set Rate Server
			void setRatePang(uint32_t _pang);
			void setRateExp(uint32_t _exp);
			void setRateClubMastery(uint32_t _club_mastery);

			// Set Event Server
			void setAngelEvent(uint32_t _angel_event);

			// Update Daily Quest Info
			void updateDailyQuest(DailyQuestInfo& _dqi);

			// send Update Room Info, find room nos canais e atualiza o info
			void sendUpdateRoomInfo(room *_r, int _option);

		public:
			// Auth Server Comandos
			virtual void authCmdShutdown(int32_t _time_sec) override;
			virtual void authCmdBroadcastNotice(std::string _notice) override;
			virtual void authCmdBroadcastTicker(std::string _nickname, std::string _msg) override;
			virtual void authCmdBroadcastCubeWinRare(std::string _msg, uint32_t _option) override;
			virtual void authCmdDisconnectPlayer(uint32_t _req_server_uid, uint32_t _player_uid, unsigned char _force) override;
			virtual void authCmdConfirmDisconnectPlayer(uint32_t _player_uid) override;
			virtual void authCmdNewMailArrivedMailBox(uint32_t _player_uid, uint32_t _mail_id) override;
			virtual void authCmdNewRate(uint32_t _tipo, uint32_t _qntd) override;
			virtual void authCmdReloadGlobalSystem(uint32_t _tipo) override;
			virtual void authCmdConfirmSendInfoPlayerOnline(uint32_t _req_server_uid, AuthServerPlayerInfo _aspi) override;

#if INTPTR_MAX == INT64_MAX && MY_GG_SRV_LIB == 0
		public:
			// Game Guard Auth Server
			virtual void ggAuthCmdGetQuery(uint32_t _error, uint32_t _socket_id, GG_AUTH_DATA& _data) override;
			virtual void ggAuthCmdCheckAnswer(uint32_t _error, uint32_t _socket_id) override;

			// Senders
			virtual uint32_t ggAuthCreateUser(uint32_t _socket_id);
			virtual uint32_t ggAuthDeleteUser(uint32_t _socket_id);
			virtual uint32_t ggAuthGetQuery(uint32_t _socket_id);
			virtual uint32_t ggAuthCheckAnswer(uint32_t _socket_id, GG_AUTH_DATA& _data);

			// Reply key by Auth Server
			virtual void authCmdKeyLoginGGAuthServer(packet *_packet);
#endif

		protected:
			player_manager m_player_manager;

			virtual void onAcceptCompleted(session *_session) override;
			virtual void onDisconnected(session *_session) override;

			virtual void onHeartBeat() override;

			virtual void onStart() override;

			virtual bool checkCommand(std::stringstream& _command) override;

			virtual bool checkPacket(session& _session, packet *_packet) override;

			virtual void config_init() override;
			virtual void reload_files();

			virtual void init_systems();
			virtual void reload_systems();
			virtual void reloadGlobalSystem(uint32_t _tipo);

			// Update Rate e Event of Server
			virtual void updateRateAndEvent(uint32_t _tipo, uint32_t _qntd);

			// Shutdown With Time
			virtual void shutdown_time(int32_t _time_sec) override;

			// Check Player Itens
#if defined(_WIN32)
			virtual DWORD check_player();
#elif defined(__linux__)
			virtual void* check_player();
#endif

			// Make Grand Zodiac Event Room
			virtual void makeGrandZodiacEventRoom();

			// Make List of Players to Golden Time Event
			virtual void makeListOfPlayersToGoldenTime();

			// Make Bot GM Event Room
			virtual void makeBotGMEventRoom();

			// Verifica e executa os comandos do Smart Calculator
			virtual bool checkSmartCalculatorCmd(player& _session, std::string& _msg, eTYPE_CALCULATOR_CMD _type);

		public:
			// Destroy room
			virtual void destroyRoom(unsigned char _channel_owner, short _number);

		protected:
			static void SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg);

#if defined(_WIN32)
			static DWORD WINAPI CALLBACK _check_player(LPVOID lpParameter);
#elif defined(__linux__)
			static void* _check_player(LPVOID lpParameter);
#endif

		protected:
			timer_manager m_timer_manager;

		protected:
			BroadcastList m_notice;
			BroadcastList m_ticker;

		private:
			DailyQuestInfo m_dqi;

			LoginManager m_login_manager;

#if INTPTR_MAX == INT64_MAX && MY_GG_SRV_LIB == 0
			unit_gg_auth_server_connect *m_game_guard;
#elif INTPTR_MAX == INT32_MAX || MY_GG_SRV_LIB == 1
			GGAuth *m_game_guard;
#else
#error Unknown pointer size or missing size macros!
#endif

			bool m_GameGuardAuth;	// Habilita ou desabilate o GameGuard Auth dos player

		private:
			std::vector< channel* > v_channel;
	};

	namespace sgs {
		typedef Singleton< game_server > gs;
	}
}

#endif // !_STDA_GAME_SERVER_H

