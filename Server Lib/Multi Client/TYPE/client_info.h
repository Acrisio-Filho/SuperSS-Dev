// Arquivo client_info.h
// Criado em 01/04/2018 as 23:19 por Acrisio
// Definição da classe ClientInfo

#pragma once
#ifndef _STDA_CLIENT_INFO_H
#define _STDA_CLIENT_INFO_H

#include "pangya_client_st.h"
#include "../../Projeto IOCP/TIMER/timer.h"

namespace stdA {
	class ClientInfo : public client_info {
		public:
			ClientInfo();
			~ClientInfo();

			void clear();

		public:
			timer *m_timer_ttl;				// Timer para chamar a função TTL
			timer *m_timer_msg_lobby;		// timer for msg to lobby

			ServerInfo m_login_server;		// Login Server Info
			server_list m_list_servers;		// ServerList
			canal_list m_list_canais;		// CanalList

			chat_macro_user m_chat_macro;	// Player Chat Macro

			unsigned char m_channel;		// Channel
			unsigned char m_lobby;			// Lobby
	};
}

#endif // !_STDA_CLIENT_INFO_H
