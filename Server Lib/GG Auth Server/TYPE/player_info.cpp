// Arquivo player_info.cpp
// Criado em 02/02/2021 as 17:12 por Acrisio
// Implementação da classe PlayerInfo

#pragma pack(1)
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
}
