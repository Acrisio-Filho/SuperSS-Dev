// Arquivo player_manager.cpp
// Criado em 15/06/2020 as 14:58 por Acrisio
// Implementa��o da classe player_manager

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "player_manager.hpp"

#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"

using namespace stdA;

player_manager::player_manager(threadpool& _threadpool, uint32_t _max_session) : session_manager(_threadpool, _max_session) {

	if (m_max_session != ~0u)
		for (auto i = 0u; i < m_max_session; ++i)
			m_sessions.push_back(new player(*(threadpool_base*)&m_threadpool));
}

player_manager::~player_manager() {
}

player* player_manager::findPlayer(uint32_t _uid, bool _oid) {
	
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
