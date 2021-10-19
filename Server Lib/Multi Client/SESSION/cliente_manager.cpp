// Arquivo cliente_manager.cpp
// Criado em 01/04/208 as 23:43 por Acrisio
// Implementação da classe ClienteManager

#pragma pack(1)

#include <WinSock2.h>
#include "cliente_manager.hpp"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"

using namespace stdA;

ClienteManager::ClienteManager(threadpool& _threadpool, unsigned long _max_session, unsigned long _TTL)
	: session_manager(_threadpool, _max_session) {

	if (m_max_session != ~0ul)
		for (auto i = 0ul; i < m_max_session; ++i)
			m_sessions.push_back(new player(*(threadpool_base*)&m_threadpool));

	InitializeCriticalSection(&m_cs);
}

ClienteManager::~ClienteManager() {
	clear();

	DeleteCriticalSection(&m_cs);
}

void ClienteManager::clear() {

	for (auto i = 0ul; i < m_sessions.size(); ++i)
		if (m_sessions[i] != nullptr)
			delete m_sessions[i];

	m_sessions.clear();
	m_sessions.shrink_to_fit();
}

player* ClienteManager::findCliente(unsigned long _uid, bool _oid) {
	
	player *p = nullptr;

	EnterCriticalSection(&m_cs);

	for (auto i = 0ul; i < m_sessions.size(); ++i) {
		if (m_sessions[i]->m_sock != INVALID_SOCKET && ((!_oid) ? m_sessions[i]->getUID() : m_sessions[i]->m_oid) == _uid) {
			p = (player*)m_sessions[i];
			break;
		}
	}

	LeaveCriticalSection(&m_cs);

	return p;
}

std::vector< player* > ClienteManager::getChannelClientes(unsigned char _channel, unsigned char _lobby) {
	
	std::vector< player* > v_p;
	
	for (auto i = 0ul; i < m_sessions.size(); ++i)
		if (m_sessions[i]->m_sock != INVALID_SOCKET && (_channel == (unsigned char)~0 || ((player*)m_sessions[i])->m_ci.m_channel == _channel)
			&& (_lobby == (unsigned char)~0 || ((player*)m_sessions[i])->m_ci.m_lobby == _lobby))
			v_p.push_back((player*)m_sessions[i]);
	
	return v_p;
}
