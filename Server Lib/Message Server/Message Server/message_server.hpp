// Arquivo message_server.hpp
// Criado em 29/07/2018 as 13:13 por Acrisio
// Defini��o da classe message_server

#pragma once
#ifndef _STDA_MESSAGE_SERVER_HPP
#define _STDA_MESSAGE_SERVER_HPP

#include "../../Projeto IOCP/Server/server.h"
#include "../SESSION/player_manager.hpp"

#include "../../Projeto IOCP/PACKET/packet.h"
#include "../SESSION/player.hpp"

#define FRIEND_LIST_LIMIT 50
#define FRIEND_PAG_LIMIT 30

namespace stdA {
	class message_server : public server {
		public:
			message_server();
			virtual ~message_server();

			// Request Login
			void requestLogin(player& _session, packet *_packet);

			void confirmLoginOnOtherServer(player& _session, uint32_t _req_server_uid, AuthServerPlayerInfo& _aspi);

			// Request Friend and Guild Member List
			void requestFriendAndGuildMemberList(player& _session, packet *_packet);

			// Request Update Channel Player Info
			void requestUpdateChannelPlayerInfo(player& _session, packet *_packet);

			// Request Update State Player
			void requestUpdatePlayerState(player& _session, packet *_packet);

			// Request Update Logout Player
			void requestUpdatePlayerLogout(player& _session, packet *_packet);

			// Request Chat
			void requestChatFriend(player& _session, packet *_packet);
			void requestChatGuild(player& _session, packet *_packet);

			// Request Check Nickname
			void requestCheckNickname(player& _session, packet *_packet);

			// Request Assign Apelido
			void requestAssingApelido(player& _session, packet *_packet);

			// Request Block Friend
			void requestBlockFriend(player& _session, packet *_packet);

			// Request Unblock Friend
			void requestUnblockFriend(player& _session, packet *_packet);

			// Request Add Friend
			void requestAddFriend(player& _session, packet *_packet);

			// Request Comfirm Friend
			void requestConfirmFriend(player& _session, packet *_packet);

			// Request Delete Friend
			void requestDeleteFriend(player& _session, packet *_packet);

			// Request Notify Player was invited to room
			void requestNotityPlayerWasInvitedToRoom(player& _session, packet *_packet);

			// Request invit Player to guild battle room
			void requestInvitPlayerToGuildBattleRoom(player& _session, packet *_packet);

			// Command Guild Web Update
			void requestAcceptGuildMember(packet *_packet);
			void requestMemberExitedFromGuild(packet *_packet);
			void requestKickGuildMember(packet *_packet);

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

		protected:
			static void SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg);

			virtual void shutdown_time(int32_t _time_sec) override;

		protected:
			virtual bool sendUpdatePlayerLogoutToFriends(player& _session);

		protected:
			player_manager m_player_manager;

			virtual void onAcceptCompleted(session *_session) override;
			virtual void onDisconnected(session *_session) override;

			virtual void onHeartBeat() override;

			virtual void onStart() override;

			virtual bool checkCommand(std::stringstream& _command) override;

			virtual bool checkPacket(session& _session, packet *_packet) override;

			virtual void init_option_accepted_socket(SOCKET _accepted) override;

			virtual void config_init() override;
			virtual void reload_files();

			virtual void reload_systems();
			virtual void reloadGlobalSystem(uint32_t _tipo);

			// Update Rate e Event of Server
			virtual void updateRateAndEvent(uint32_t _tipo, uint32_t _qntd);
	};

	namespace sms {
		typedef Singleton< message_server > ms;
	}
}

#endif // !_STDA_MESSAGE_SERVER_HPP
