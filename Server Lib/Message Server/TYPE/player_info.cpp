// Arquivo player_info.cpp
// Criado em 29/07/2018 as 13:32 por Acrisio
// Implementa��o da classe PlayerInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#endif

#include "player_info.hpp"

using namespace stdA;

PlayerInfo::PlayerInfo() : player_info{0}, m_friend_manager() {
	clear();
}

PlayerInfo::~PlayerInfo() {
	clear();
}

void PlayerInfo::clear() {

	player_info::clear();

	m_state = 0u;

	// Zera
#if defined(_WIN32)
	InterlockedExchange(&m_logout, 0u);
#elif defined(__linux__)
	__atomic_store_n(&m_logout, 0u, __ATOMIC_RELAXED);
#endif
	
	m_cpi.clear();

	m_friend_manager.clear();
}
