// Arquivo gg_auth_server.hpp
// Criado em 02/02/2021 as 16:47 por Acrisio
// Defini��o da classe gg_auth_server

#pragma once
#ifndef _STDA_GG_AUTH_SERVER_HPP
#define _STDA_GG_AUTH_SERVER_HPP

#include "../../Projeto IOCP/UNIT/unit.hpp"
#include "../../Projeto IOCP/TYPE/singleton.h"
#include "../SESSION/player_manager.hpp"

#include "../../Projeto IOCP/UNIT/unit_auth_server_connect.hpp"
#include "../../Projeto IOCP/UNIT/unit_auth_interface.hpp"

#include "../CSAuth/GGAuth.hpp"

namespace stdA {

	class gg_auth_server : public unit, public IUnitAuthServer {
		public:
			gg_auth_server();
			virtual ~gg_auth_server();

		// Requests
		public:
			virtual void requestLoginFirstStep(player& _session, packet *_packet);
			virtual void requestLoginSecondStep(player& _session, packet *_packet);
			virtual void requestAddPlayerGameGuardCtx(player& _session, packet *_packet);
			virtual void requestDeletePlayerGameGuardCtx(player& _session, packet *_packet);
			virtual void requestGetAuthQueryPlayer(player& _session, packet *_packet);
			virtual void requestCheckAuthAnswerPlayer(player& _session, packet *_packet);

		protected:
			virtual void onAcceptCompleted(session *_session) override;
			virtual void onDisconnected(session *_session) override;

			virtual void onHeartBeat() override;

			virtual void onStart() override;

			virtual bool checkCommand(std::stringstream& _command) override;

			virtual bool checkPacket(session& _session, packet *_packet) override;

			virtual void shutdown_time(int32_t _time_sec) override;

			virtual void config_init() override;
			virtual void reload_files();

		protected:
			player_manager m_player_manager;

			GGAuth *m_game_guard;

		// Auth Server Membros protegidos
		protected:
			unit_auth_server_connect *m_unit_connect;		// Ponteiro Connecta com o Auth Server

			void destroy_unit();

		// Auth Server Comandos
		public:
			virtual void authCmdShutdown(int32_t _time_sec);
			virtual void authCmdBroadcastNotice(std::string _notice);
			virtual void authCmdBroadcastTicker(std::string _nickname, std::string _msg);
			virtual void authCmdBroadcastCubeWinRare(std::string _msg, uint32_t _option);
			virtual void authCmdDisconnectPlayer(uint32_t _req_server_uid, uint32_t _player_uid, unsigned char _force);
			virtual void authCmdConfirmDisconnectPlayer(uint32_t _player_uid);
			virtual void authCmdNewMailArrivedMailBox(uint32_t _player_uid, uint32_t _mail_id);
			virtual void authCmdNewRate(uint32_t _tipo, uint32_t _qntd);
			virtual void authCmdReloadGlobalSystem(uint32_t _tipo);
			virtual void authCmdInfoPlayerOnline(uint32_t _req_server_uid, uint32_t _player_uid);
			virtual void authCmdConfirmSendInfoPlayerOnline(uint32_t _req_server_uid, AuthServerPlayerInfo _aspi);

			// requests Comandos e respostas din�micas
			virtual void authCmdSendCommandToOtherServer(packet& _packet);
			virtual void authCmdSendReplyToOtherServer(packet& _packet);

			// Server envia comandos e resposta para outros server com o Auth Server
			virtual void sendCommandToOtherServerWithAuthServer(packet& _packet, uint32_t _send_server_uid_or_type);
			virtual void sendReplyToOtherServerWithAuthServer(packet& _packet, uint32_t _send_server_uid_or_type);
	};

	typedef Singleton< gg_auth_server > sgg_as;
}

#endif // !_STDA_GG_AUTH_SERVER_HPP