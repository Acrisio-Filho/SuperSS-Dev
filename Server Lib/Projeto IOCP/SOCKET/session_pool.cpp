// Arquivo session_pool.cpp
// Criado em 17/06/2017 por Acrisio
// Definição da classe session_pool

#include <WinSock2.h>
#include "session_pool.h"
#include <memory.h>
#include "../UTIL/exception.h"
#include "../THREAD POOL/threadpool.h"
#include "../TYPE/stda_error.h"

using namespace stdA;

int64_t session_pool::count = 0;

session_pool::session_pool(size_t _TTL) : m_TTL(_TTL) {
	m_max_session = 0;
	m_sessions = nullptr;
	m_session_free = 0;

	InitializeCriticalSection(&cs);
};

session_pool::session_pool(threadpool* _threadpool, size_t _max_session, size_t _TTL) : m_max_session(_max_session), m_TTL(_TTL) {
	init(_threadpool, m_max_session, _TTL);
};

session_pool::~session_pool() {
	DeleteCriticalSection(&cs);

	destroy();
};

void session_pool::init(threadpool* _threadpool, size_t _max_session, size_t _TTL) {
	if (m_sessions != nullptr)
		destroy();

	m_sessions = nullptr;
	m_session_free = 0;
	m_max_session = _max_session;
	m_TTL = _TTL;

	if (m_max_session > 0) {
		m_sessions = new session[m_max_session];

		for (size_t i = 0; i < m_max_session; ++i)
			m_sessions[i].setThreadpool(_threadpool);
	}
};

void session_pool::destroy() {
    m_session_free = 0;
    m_max_session = 0;

	if (m_sessions != nullptr) {
		for (size_t i = 0; i < m_max_session; i++) {
			if (m_sessions[i].m_sock != INVALID_SOCKET) {
				shutdown(m_sessions[i].m_sock, SD_BOTH);
				closesocket(m_sessions[i].m_sock);
			}
		}

		delete[] m_sessions;
	}

    m_sessions = nullptr;
};

session* session_pool::addSession(SOCKET _sock, SOCKADDR_IN _addr, unsigned char _key) {
    session *s = nullptr;

    EnterCriticalSection(&cs);

	do {
		try {
			findNextSessionFree();
		}catch (exception& e) {
			UNREFERENCED_PARAMETER(e);

			// Libera a critical section
			LeaveCriticalSection(&cs);

			throw;	// relança
		}

		s = &m_sessions[m_session_free];
	} while (s->m_sock != INVALID_SOCKET);

	if (s->m_sock != INVALID_SOCKET)
		throw exception("Erro session ja esta sendo usada. session_pool::addSession()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SESSION_POOL, 1, 0));

	// Limpa sessao aqui
	s->lock();

	s->clear();

	s->m_sock = _sock;
	s->m_addr = _addr;
	s->m_key = _key;
	s->m_oid = m_session_free;
	s->m_time_start = std::clock();
	s->m_tick = std::clock();

	s->setState(true);

	count++;

	s->unlock();

    LeaveCriticalSection(&cs);

    return s;
};

void session_pool::deleteSession(session *s) {
	EnterCriticalSection(&cs);

	s->lock();

	s->clear();

	if (s->m_sock == INVALID_SOCKET)
		count--;

	s->unlock();

	LeaveCriticalSection(&cs);
};

void session_pool::checkSessionLive() {
	for (size_t i = 0; i < m_max_session; i++) {
		if (m_sessions[i].m_sock != INVALID_SOCKET) {
			if (!m_sessions[i].isConnected())
				deleteSession(&m_sessions[i]);
			else if (m_TTL > 0 && ((std::clock() - m_sessions[i].m_tick) / (double)CLOCKS_PER_SEC) > ((m_TTL + 200) / 1000.0)) // Close Session not send nothing in time to live)
				deleteSession(&m_sessions[i]);
		}
	}
};

session* session_pool::findSession(uint32_t uid, uint32_t oid) {
	for (size_t i = 0; i < m_max_session; ++i) {
		if (m_sessions[i].m_sock != INVALID_SOCKET) {
			if (oid != ~0) {
				if (m_sessions[i].m_oid == oid)
					return &m_sessions[i];
			}else if (m_sessions[i].m_pi.uid == uid)
				return &m_sessions[i];
		}
	}

	return nullptr;
};

void session_pool::findNextSessionFree() {
	for (m_session_free = 0; m_session_free < m_max_session; m_session_free++)
		if (m_sessions[m_session_free].m_sock == INVALID_SOCKET)
			break;

	if (m_session_free == m_max_session)
		throw exception("Erro chegou ao limite de sessoes. session_pool::findNexSessionFree()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SESSION_POOL, 2, 0));
};

std::vector< session* > session_pool::getChannelSessions(unsigned char _channel, unsigned char _lobby) {
	std::vector< session* > channel_sessions;

	// Option unsigned char _channel == ~0, retorna todas as sessions do server
	for (size_t i = 0; i < m_max_session; ++i)
		if (m_sessions[i].m_sock != INVALID_SOCKET && (_channel == (unsigned char)~0 || m_sessions[i].m_channel == _channel) 
			&& (_lobby == (unsigned char)~0 || m_sessions[i].m_lobby == _lobby))
			channel_sessions.push_back(&m_sessions[i]);

	return channel_sessions;
};