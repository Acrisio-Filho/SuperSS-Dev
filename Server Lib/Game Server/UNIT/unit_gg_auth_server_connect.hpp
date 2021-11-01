// Arquivo unit_gg_auth_server_connect.hpp
// Criado em 02/02/2021 as 23:44 por Acrisio
// Definição da classe unit_gg_auth_server_connect

#pragma once
#ifndef _STDA_UNIT_GG_AUTH_SERVER_CONNECT_HPP
#define _STDA_UNIT_GG_AUTH_SERVER_CONNECT_HPP

#include "../../Projeto IOCP/UNIT/unit_connect.hpp"
#include "unit_gg_interface.hpp"

namespace stdA {

    class unit_gg_auth_server_connect : public unit_connect_base {
        public:
            unit_gg_auth_server_connect(IUnitGGAuthServer& _owner_server, ServerInfoEx& _si);
            virtual ~unit_gg_auth_server_connect();

		public:
			virtual void requestFirstPacketKey(UnitPlayer& _session, packet *_packet);
			virtual void requestAskLogin(UnitPlayer& _session, packet *_packet);

			virtual void requestGetQuery(UnitPlayer& _session, packet *_packet);
			virtual void requestCheckAnswer(UnitPlayer& _session, packet *_packet);

			virtual void sendHaskKeyLogin(std::string _key);

			virtual void sendCreateUser(uint32_t _socket_id);
			virtual void sendDeleteUser(uint32_t _socket_id);
			virtual void sendGetQuery(uint32_t _socket_id);
			virtual void sendCheckAnswer(uint32_t _socket_id, GG_AUTH_DATA& _data);

		protected:
			virtual void onHeartBeat() override;

			virtual void onConnected() override;

			virtual void onDisconnect() override;

			virtual void config_init() override;

		protected:
			IUnitGGAuthServer& m_owner_server;
    };
}

#endif // !_STDA_UNIT_GG_AUTH_SERVER_CONNECT_HPP