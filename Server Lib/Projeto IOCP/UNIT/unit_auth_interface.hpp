// Arquivo unit_auth_interface.hpp
// Criado em 02/02/2021 as 19:20 por Acrisio
// Definição da Interface Unit Auth Server

#pragma once
#ifndef _STDA_UNIT_INTERFACE_HPP
#define _STDA_UNIT_INTERFACE_HPP

#include <string>
#include "../TYPE/pangya_st.h"
#include "../PACKET/packet.h"

namespace stdA {

    class IUnitAuthServer {
        public:
            virtual void authCmdShutdown(int32_t _time_sec) = 0;
			virtual void authCmdBroadcastNotice(std::string _notice) = 0;
			virtual void authCmdBroadcastTicker(std::string _nickname, std::string _msg) = 0;
			virtual void authCmdBroadcastCubeWinRare(std::string _msg, uint32_t _option) = 0;
			virtual void authCmdDisconnectPlayer(uint32_t _req_server_uid, uint32_t _player_uid, unsigned char _force) = 0;
			virtual void authCmdConfirmDisconnectPlayer(uint32_t _player_uid) = 0;
			virtual void authCmdNewMailArrivedMailBox(uint32_t _player_uid, uint32_t _mail_id) = 0;
			virtual void authCmdNewRate(uint32_t _tipo, uint32_t _qntd) = 0;
			virtual void authCmdReloadGlobalSystem(uint32_t _tipo) = 0;
			virtual void authCmdInfoPlayerOnline(uint32_t _req_server_uid, uint32_t _player_uid) = 0;
			virtual void authCmdConfirmSendInfoPlayerOnline(uint32_t _req_server_uid, AuthServerPlayerInfo _aspi) = 0;

			// requests Comandos e respostas dinâmicas
			virtual void authCmdSendCommandToOtherServer(packet& _packet) = 0;
			virtual void authCmdSendReplyToOtherServer(packet& _packet) = 0;

			// Server envia comandos e resposta para outros server com o Auth Server
			virtual void sendCommandToOtherServerWithAuthServer(packet& _packet, uint32_t _send_server_uid_or_type) = 0;
			virtual void sendReplyToOtherServerWithAuthServer(packet& _packet, uint32_t _send_server_uid_or_type) = 0; 
    };
}

#endif // !_STDA_UNIT_INTERFACE_HPP