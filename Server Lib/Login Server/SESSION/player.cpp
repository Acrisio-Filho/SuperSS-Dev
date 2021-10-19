// Arquivo player.cpp
// Criado em 11/03/2018 as 14:15 por Acrisio
// Implementação da classe player

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#elif defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#endif

#include "player.hpp"

using namespace stdA;

player::player(threadpool_base& _threadpool) : session(_threadpool), m_pi() {
};

player::~player() {

};

bool player::clear() {
	
	bool ret = true;

	if ((ret = session::clear()))
		m_pi.clear();

	return ret;
};

unsigned char player::getStateLogged() {
	return m_pi.m_state;
};

uint32_t player::getUID() {
	return m_pi.uid;
};

uint32_t player::getCapability() {
	return m_pi.m_cap;
};

char* player::getNickname() {
	return m_pi.nickname;
};

char* player::getID() {
	return m_pi.id;
};
