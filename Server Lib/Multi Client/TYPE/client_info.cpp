// Arquivo client_info.cpp
// Criado em 01/04/2018 as 23:24
// Implementação da classe ClientInfo

#pragma pack(1)

#include <WinSock2.h>
#include "client_info.h"

#include "../Multi Client/multi_client.h"

using namespace stdA;

ClientInfo::ClientInfo() : client_info() {
	clear();
}

ClientInfo::~ClientInfo() {
	clear();
}

void ClientInfo::clear() {
	
	client_info::clear();

	if (smc::mc != nullptr) {

		if (m_timer_ttl != nullptr)
			smc::mc->removeTimer(m_timer_ttl);

		if (m_timer_msg_lobby != nullptr)
			smc::mc->removeTimer(m_timer_msg_lobby);
	}

	m_login_server.clear();
	m_list_servers.clear();
	m_list_canais.clear();

	m_chat_macro.clear();

	m_channel = ~0;
	m_lobby = ~0;
}
