// Arquivo login_server.h
// Criado em 18/12/2017 por Acrisio
// Definição da classe login_server

#pragma once
#ifndef _STDA_LOGIN_SERVER_H
#define _STDA_LOGIN_SERVER_H

#include "../../Projeto IOCP/TYPE/pangya_st.h"
#include "../../Projeto IOCP/Server/server.h"
#include "../SESSION/player_manager.hpp"

#include "../../Projeto IOCP/SOCKET/socket.h"

namespace stdA {
	class login_server : public server {
		public:
			login_server();
			virtual ~login_server();

			bool getAccessFlag();
			bool getCreateUserFlag();

			bool canSameIDLogin();

			// Request Cliente
			void requestDownPlayerOnGameServer(player& _session, packet *_packet);

		public:
			// Auth Server Comandos
			virtual void authCmdShutdown(int32_t _time) override;
			virtual void authCmdBroadcastNotice(std::string _notice) override;
			virtual void authCmdBroadcastTicker(std::string _nickname, std::string _msg) override;
			virtual void authCmdBroadcastCubeWinRare(std::string _msg, uint32_t _option) override;
			virtual void authCmdDisconnectPlayer(uint32_t _req_server_uid, uint32_t _player_uid, unsigned char _force) override;
			virtual void authCmdConfirmDisconnectPlayer(uint32_t _player_uid) override;
			virtual void authCmdNewMailArrivedMailBox(uint32_t _player_uid, uint32_t _mail_id) override;
			virtual void authCmdNewRate(uint32_t _tipo, uint32_t _qntd) override;
			virtual void authCmdReloadGlobalSystem(uint32_t _tipo) override;
			virtual void authCmdConfirmSendInfoPlayerOnline(uint32_t _req_server_uid, AuthServerPlayerInfo _aspi) override;

			virtual void requestLogin(player& _session, packet *_packet);
			virtual void requestReLogin(player& _session, packet *_packet);

		protected:
			virtual void shutdown_time(int32_t _time_sec) override;

		protected:
			virtual void onAcceptCompleted(session *_session) override;
			virtual void onDisconnected(session *_session) override;
			virtual void onHeartBeat() override;
			virtual void onStart() override;

			virtual bool checkCommand(std::stringstream& _command) override;

			virtual bool checkPacket(session& _session, packet *_packet) override;

			virtual void config_init() override;
			virtual void reload_files();

		protected:
			static void SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg);

		protected:
			player_manager m_player_manager;

			bool m_access_flag;
			bool m_create_user_flag;
			bool m_same_id_login_flag;
	};

	namespace sls {
		typedef Singleton< login_server > ls;
	}
}

#endif // !_STDA_LOGIN_SERVER_H

