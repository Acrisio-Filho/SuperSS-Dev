// Arquivo unit_auth_server_connect.hpp
// Criado em 02/02/2021 as 23:39 por Acrisio
// Definição da classe unit_auth_server_connect

#pragma once
#ifndef _STDA_UNIT_AUTH_SERVER_CONNECT_HPP
#define _STDA_UNIT_AUTH_SERVER_CONNECT_HPP

#include "unit_connect.hpp"
#include "unit_auth_interface.hpp"

namespace stdA {

    class unit_auth_server_connect : public unit_connect_base {
        public:
            unit_auth_server_connect(IUnitAuthServer& _owner_server, ServerInfoEx& _si);
            virtual ~unit_auth_server_connect();

		public:
			virtual void requestFirstPacketKey(UnitPlayer& _session, packet *_packet);
			virtual void requestAskLogin(UnitPlayer& _session, packet *_packet);
			virtual void requestShutdownServer(UnitPlayer& _session, packet *_packet);
			virtual void requestBroadcastNotice(UnitPlayer& _session, packet *_packet);
			virtual void requestBroadcastTicker(UnitPlayer& _session, packet *_packet);
			virtual void requestBroadcastCubeWinRare(UnitPlayer& _session, packet *_packet);
			virtual void requestDisconnectPlayer(UnitPlayer& _session, packet *_packet);
			virtual void requestConfirmDisconnectPlayer(UnitPlayer& _session, packet *_packet);
			virtual void requestNewMailArrivedMailBox(UnitPlayer& _session, packet *_packet);
			virtual void requestNewRate(UnitPlayer& _session, packet *_packet);
			virtual void requestReloadSystem(UnitPlayer& _session, packet *_packet);
			virtual void requestInfoPlayerOnline(UnitPlayer& _session, packet *_packet);
			virtual void requestConfirmSendInfoPlayerOnline(UnitPlayer& _session, packet *_packet);
			virtual void requestSendCommandToOtherServer(UnitPlayer& _session, packet *_packet);
			virtual void requestSendReplyToOtherServer(UnitPlayer& _session, packet *_packet);

			// Request Reply
			virtual void sendConfirmDisconnectPlayer(uint32_t _server_uid, uint32_t _player_uid);
			virtual void sendDisconnectPlayer(uint32_t _server_uid, uint32_t _player_uid);
			virtual void sendInfoPlayerOnline(uint32_t _server_uid, AuthServerPlayerInfo _aspi);
			virtual void getInfoPlayerOnline(uint32_t _server_uid, uint32_t _player_uid);
			virtual void sendCommandToOtherServer(uint32_t _server_uid, packet& _packet);
			virtual void sendReplyToOtherServer(uint32_t _server_uid, packet& _packet);

		protected:
			virtual void onHeartBeat() override;

			virtual void onConnected() override;

			virtual void onDisconnect() override;

			virtual void config_init() override;

		protected:
			IUnitAuthServer& m_owner_server;
    };
}

#endif // !_STDA_UNIT_AUTH_SERVER_CONNECT_HPP