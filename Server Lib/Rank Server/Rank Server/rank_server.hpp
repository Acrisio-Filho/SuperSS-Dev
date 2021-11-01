// Arquivo rank_server.hpp
// Criado em 15/06/2020 as 14:43 por Acrisio
// Defini��o da classe rank_server

#pragma once
#ifndef _STDA_RANK_SERVER_HPP
#define _STDA_RANK_SERVER_HPP

#include "../../Projeto IOCP/Server/server.h"
#include "../SESSION/player_manager.hpp"

#include "../../Projeto IOCP/PACKET/packet.h"
#include "../SESSION/player.hpp"

#include "../UTIL/rank_registry_manager.hpp"
#include "../UTIL/rank_refresh_time.hpp"

namespace stdA {
	class rank_server : public server {
		public:
			rank_server();
			virtual ~rank_server();

			// Request Login
			void requestLogin(player& _session, packet *_packet);

			// Request Player Info
			void requestPlayerInfo(player& _session, packet *_packet);

			// Request search Player in rank
			void requestSearchPlayerInRank(player& _session, packet *_packet);

			void confirmLoginOnOtherServer(player& _session, uint32_t _req_server_uid, AuthServerPlayerInfo& _aspi);

		protected:
			virtual void onAcceptCompleted(session *_session) override;
			virtual void onDisconnected(session *_session) override;

			virtual void onHeartBeat() override;

			virtual void onStart() override;

			virtual bool checkCommand(std::stringstream& _command) override;

			virtual bool checkPacket(session& _session, packet *_packet) override;

			// Shutdown With Time
			virtual void shutdown_time(int32_t _time_sec) override;

			virtual void sendFirstPage(player& _session, int _option);

			// Update Time Refresh
			virtual void updateTimeRefresh(uint32_t _ret, std::string _date);

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

		protected:
			player_manager m_player_manager;

			RankRegistryManager m_rank_registry_manager;

			rank_refresh_time m_refresh_time;

			uint32_t volatile m_sync_update_time_refresh;

			virtual void config_init() override;
			virtual void reload_files();
	};

	namespace srs {
		typedef Singleton< rank_server > rs;
	}
}

#endif // !_STDA_RANK_SERVER_HPP

