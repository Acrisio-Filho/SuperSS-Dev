// Arquivo auth_server.hpp
// Criado em 02/12/2018 as 13:00 por Acrisio
// Defini��o da classe auth_server

#pragma once
#ifndef _STDA_AUTH_SERVER_HPP
#define _STDA_AUTH_SERVER_HPP

#include "../../Projeto IOCP/TYPE/pangya_st.h"
#include "../../Projeto IOCP/UNIT/unit.hpp"
#include "../SESSION/player_manager.hpp"

#include "../../Projeto IOCP/SOCKET/socket.h"

#include "../TYPE/pangya_auth_st.h"

#include "../PACKET/packet_func_as.h"

namespace stdA {
	class auth_server : public unit {
		public:
			auth_server();
			virtual ~auth_server();

		public:
			// Requests
			void requestDisconnectPlayer(player& _session, packet *_packet);
			void requestConfirmDisconnectPlayer(player& _session, packet *_packet);
			void requestInfoPlayer(player& _session, packet *_packet);
			void requestConfirmSendInfoPlayer(player& _session, packet *_packet);
			void requestSendCommandToOtherServer(player& _session, packet *_packet);
			void requestSendReplyToOtherServer(player& _session, packet *_packet);

			void requestAuthenticPlayer(player& _session, packet *_packet);

		protected:
			virtual void onAcceptCompleted(session *_session) override;
			virtual void onDisconnected(session *_session) override;
			virtual void onHeartBeat() override;
			virtual void onStart() override;

			virtual void translateCmd(std::vector< CommandInfo >& _v_ci);

			virtual bool checkCommand(std::stringstream& _command) override;

			virtual bool checkPacket(session& _session, packet *_packet) override;

			virtual void config_init() override;
			virtual void reload_files();

			virtual void shutdown_time(int32_t _time_sec) override;

		protected:
			static void SQLDBResponse(uint32_t _msg_id, pangya_db& _pangya_db, void* _arg);

		protected:
			player_manager m_player_manager;

			SYSTEMTIME m_guild_ranking_time;
	};

	namespace sas {
		typedef Singleton< auth_server > as;
	}
}

#endif // !_STDA_AUTH_SERVER_HPP
