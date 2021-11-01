// Arquivo lobby.cpp
// Criado em 24/12/2017 por Acrisio
// Implementação da classe lobby

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "lobby.h"

using namespace stdA;

lobby::lobby() {

};

lobby::~lobby() {
	while (!v_sessions.empty()) {
		v_sessions.erase(v_sessions.begin());
		v_sessions.shrink_to_fit();
	}

	while (!v_rooms.empty()) {
		delete v_rooms.front();

		v_rooms.erase(v_rooms.begin());
		v_rooms.shrink_to_fit();
	}
};
