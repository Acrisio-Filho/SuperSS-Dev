// Arquivo player_manager.cpp
// Criado em 02/12/2018 as 13:21 por Acrisio
// Implementa��o da classe player_manager

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#elif defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#endif

#include "player_manager.hpp"

#include <algorithm>

using namespace stdA;

player_manager::player_manager(threadpool& _threadpool, uint32_t _max_session) : session_manager(_threadpool, _max_session) {

	if (m_max_session != ~0u)
		for (auto i = 0u; i < m_max_session; ++i)
			m_sessions.push_back(new player(*(threadpool_base*)&m_threadpool));
}

player_manager::~player_manager() {
}

std::vector< player* > player_manager::getAllPlayer() {

	std::vector< player* > v_p;

	std::for_each(m_sessions.begin(), m_sessions.end(), [&](auto& el) {
		v_p.push_back((player*)el);
	});

	return v_p;
}

player* player_manager::findPlayer(uint32_t _uid, bool _oid) {
	
	player *_player = nullptr;

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
			_player = (player*)el;
			break;
		}
	}

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return _player;
}

std::vector< player* > player_manager::findPlayerByType(uint32_t _type) {
	
	std::vector< player* > v_p;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	std::for_each(m_sessions.begin(), m_sessions.end(), [&](auto& el) {
		if (el != nullptr && el->getCapability()/*Tipo*/ == _type)
			v_p.push_back((player*)el);
	});

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return v_p;
}

std::vector< player* > player_manager::findPlayerByTypeExcludeUID(uint32_t _type, uint32_t _uid) {
	
	std::vector< player* > v_p;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	std::for_each(m_sessions.begin(), m_sessions.end(), [&](auto& el) {
		if (el != nullptr && el->getCapability()/*Tipo*/ == _type && el->getUID() != _uid)
			v_p.push_back((player*)el);
	});

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return v_p;
}
