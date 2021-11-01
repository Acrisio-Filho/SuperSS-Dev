// Arquivo player_manager.cpp
// Criado em 29/07/2018 as 13:50 por Acrisio
// Implementa��o da classe player_manager

#if defined(_WIN32)
#pragma once
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "player_manager.hpp"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"

using namespace stdA;

player_manager::player_manager(threadpool& _threadpool, uint32_t _max_session) : session_manager(_threadpool, _max_session) {

	if (m_max_session != ~0u)
		for (auto i = 0u; i < m_max_session; ++i)
			m_sessions.push_back(new player(*(threadpool_base*)&m_threadpool));
}

player_manager::~player_manager() {
}

player *player_manager::findPlayer(uint32_t _uid, bool _oid) {

	player *p = nullptr;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	for (auto& el : m_sessions) {
		if (
#if defined(_WIN32)
			el->m_sock != INVALID_SOCKET 
#elif defined(__linux__)
			el->m_sock.fd != INVALID_SOCKET 
#endif
		&& ((!_oid) ? el->getUID() : el->m_oid) == _uid) {
			p = (player*)el;
			break;
		}
	}

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return p;
}

std::vector< player* > player_manager::findAllGM() {
	
	std::vector< player* > v_gm;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	for (auto& el : m_sessions)
		if (
#if defined(_WIN32)
			el->m_sock != INVALID_SOCKET 
#elif defined(__linux__)
			el->m_sock.fd != INVALID_SOCKET 
#endif
		&& ((el->getCapability() & 4) || el->getCapability() & 128/*GM Player Normal*/))	// GM
			v_gm.push_back((player*)el);

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return v_gm;
}

std::map< uint32_t, player* > player_manager::findAllFriend(std::vector< FriendInfoEx* > _v_friend) {
	
	std::map< uint32_t, player* > map_fi;

	player *p = nullptr;

	// n�o add o Mesmo Player, no Map
	for (auto& el : _v_friend) {
		if ((p = (player*)findSessionByUID(el->uid)) != nullptr) {

			auto it = map_fi.find(p->m_pi.uid);

			if (it == map_fi.end())
				map_fi[p->m_pi.uid] = p;
		}
	}

	return map_fi;
}

std::map< uint32_t, player* > player_manager::findAllGuildMember(uint32_t _guild_uid) {
	
	std::map< uint32_t, player* > map_gmi;

	player *p = nullptr;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	// n�o add o Mesmo player no Map
	for (auto& el : m_sessions) {

		if ((p = ((player*)el)) != nullptr && p->m_pi.guild_uid > 0 && p->m_pi.guild_uid == _guild_uid) {

			auto it = map_gmi.find(p->m_pi.uid);

			if (it == map_gmi.end())
				map_gmi[p->m_pi.uid] = p;
		}
	}

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif
	
	return map_gmi;
}
