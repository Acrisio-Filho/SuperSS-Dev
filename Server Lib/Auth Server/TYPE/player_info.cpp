// Arquivo player_info.cpp
// Criado em 02/12/2018 as 13:14 por Acrisio
// Implementa��o da classe PlayerInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "player_info.hpp"

using namespace stdA;

PlayerInfo::PlayerInfo() {
	clear();
}

PlayerInfo::~PlayerInfo() {
	clear();
}

void PlayerInfo::clear() {

	player_info::clear();

	m_state = 0u;
	m_place = 0u;
	m_server_uid = 0u;
}
