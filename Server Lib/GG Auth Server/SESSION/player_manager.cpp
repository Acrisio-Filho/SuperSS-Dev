// Arquivo player_manager.cpp
// Criado em 02/02/2021 as 17:26 por Acrisio
// Implemenetação da classe player_manager

#pragma pack(1)
#include <WinSock2.h>
#include "player_manager.hpp"

using namespace stdA;

player_manager::player_manager(threadpool& _threadpool, unsigned long _max_session) : session_manager(_threadpool, _max_session) {

	if (m_max_session != ~0ul)
		for (auto i = 0ul; i < m_max_session; ++i)
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

player* player_manager::findPlayer(unsigned long _uid, bool _oid) {
	
	player *_player = nullptr;

	EnterCriticalSection(&m_cs);

	for (auto& el : m_sessions) {
		if (el->m_sock != INVALID_SOCKET && ((!_oid) ? el->getUID() : el->m_oid) == _uid) {
			_player = (player*)el;
			break;
		}
	}

	LeaveCriticalSection(&m_cs);

	return _player;
}

std::vector< player* > player_manager::findPlayerByType(unsigned long _type) {
	
	std::vector< player* > v_p;

	std::for_each(m_sessions.begin(), m_sessions.end(), [&](auto& el) {
		if (el != nullptr && el->getCapability()/*Tipo*/ == _type)
			v_p.push_back((player*)el);
	});

	return v_p;
}

std::vector< player* > player_manager::findPlayerByTypeExcludeUID(unsigned long _type, unsigned long _uid) {
	
	std::vector< player* > v_p;

	std::for_each(m_sessions.begin(), m_sessions.end(), [&](auto& el) {
		if (el != nullptr && el->getCapability()/*Tipo*/ == _type && el->getUID() != _uid)
			v_p.push_back((player*)el);
	});

	return v_p;
}
