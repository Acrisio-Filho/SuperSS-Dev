// Arquivo session_manager.cpp
// Criado em 01/11/2018 as 17:52 por Acrisio
// Implementação da classe session_manager

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#elif defined(__linux__)
#include "../UTIL/WinPort.h"
#include <pthread.h>
#endif

#include "session_manager.hpp"
#include "../UTIL/exception.h"
#include "../TYPE/stda_error.h"
#include "../UTIL/message_pool.h"

#include "../UTIL/reader_ini.hpp"

#include "socket.h"

#ifndef INI_PATH
	#if defined(_WIN32)
		#define INI_PATH "\\server.ini"
	#elif defined(__linux__)
		#define INI_PATH "/server.ini"
	#endif
#endif // INI_PATH

using namespace stdA;

uint32_t session_manager::m_count = 0u;
bool session_manager::m_is_init = false;

session_manager::session_manager(threadpool& _threadpool, uint32_t _max_session) : m_threadpool(_threadpool), m_max_session(_max_session), m_TTL(0u) {

	// Carrega as config do arquivo server.ini
	config_init();

#if defined(_WIN32)
	InitializeCriticalSection(&m_cs);
#elif defined(__linux__)
	INIT_PTHREAD_MUTEXATTR_RECURSIVE;
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif

	m_is_init = true;
}

session_manager::~session_manager() {
	
	clear();

#if defined(_WIN32)
	DeleteCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_destroy(&m_cs);
#endif

	m_is_init = false;
}

void session_manager::clear() {

	// Limpa as session para deletar
	if (!m_session_del.empty())
		m_session_del.clear();

	// Não Vazias
	if (!m_sessions.empty()) {
		
		for (auto i = 0u; i < m_sessions.size(); ++i)
			if (m_sessions[i] != nullptr)
				delete m_sessions[i];

		m_sessions.clear();
		m_sessions.shrink_to_fit();
	}
}

session* session_manager::addSession(SOCKET _sock, SOCKADDR_IN _addr, unsigned char _key) {
	
#if defined(_WIN32)
	if (_sock == INVALID_SOCKET)
#elif defined(__linux__)
	if (_sock.fd == INVALID_SOCKET)
#endif
		throw exception("[session_manager::addSession][ERR_SESSION] _sock is invalid.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SESSION_MANAGER, 1, 0));

	session *pSession = nullptr;
	int32_t index;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	if ((index = findSessionFree()) == -1) {
#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif
		throw exception("[session_manager::addSession][ERR_SESSION] already goal limit session estabilized.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SESSION_MANAGER, 3, 0));
	}

	pSession = m_sessions[index];

	pSession->m_sock = _sock;
	pSession->m_addr = _addr;
	pSession->m_key = _key;
	pSession->m_oid = index;
	pSession->m_time_start = pSession->m_tick = std::clock();

	pSession->setState(true);
	pSession->setConnected(true);

	m_count++;

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return pSession;
}

bool session_manager::deleteSession(session* _session) {

	if (_session == nullptr)
		throw exception("[session_manager::deleteSession][ERR_SESSION] _session is nullptr.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SESSION_MANAGER, 1, 0));

	if (!_session->getState() && 
#if defined(_WIN32)
		_session->m_sock == INVALID_SOCKET
#elif defined(__linux__)
		_session->m_sock.fd == INVALID_SOCKET
#endif
	)
		throw exception(std::string("[session_manager::deleteSession][ERR_SESSION] SESSIION[IP=") + _session->getIP() 
				+ ", OID=" + std::to_string(_session->m_oid) + "] _seession not connected.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SESSION_MANAGER, 2, 0));

	bool ret = true;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	if (!_session->getState() && 
#if defined(_WIN32)
		_session->m_sock == INVALID_SOCKET
#elif defined(__linux__)
		_session->m_sock.fd == INVALID_SOCKET
#endif
	) {
		
		// Se não liberar aqui da deadlock
#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif
		
		throw exception(std::string("[session_manager::deleteSession][ERR_SESSION] SESSIION[IP=") + _session->getIP()
				+ ", OID=" + std::to_string(_session->m_oid) + "] _seession not connected.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::SESSION_MANAGER, 2, 0));
	}

	_session->lock();

	if ((ret = _session->clear()))
		m_count--;

	_session->unlock();

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return ret;
}

uint32_t session_manager::getNumSessionOnline() {

	uint32_t curr_online = 0u;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	curr_online = m_count;

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return curr_online;
}

session* session_manager::findSessionByOID(uint32_t _oid) {
	
	session *_session = nullptr;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	for (auto& el : m_sessions) {
#if defined(_WIN32)
		if (el->m_sock != INVALID_SOCKET && el->m_oid == _oid) {
#elif defined(__linux__)
		if (el->m_sock.fd != INVALID_SOCKET && el->m_oid == _oid) {
#endif
			_session = el;
			break;
		}
	}

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return _session;
}

session* session_manager::findSessionByUID(uint32_t _uid) {
	
	session *_session = nullptr;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	for (auto& el : m_sessions) {
#if defined(_WIN32)
		if (el->m_sock != INVALID_SOCKET && el->getUID() == _uid) {
#elif defined(__linux__)
		if (el->m_sock.fd != INVALID_SOCKET && el->getUID() == _uid) {
#endif
			_session = el;
			break;
		}
	}

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return _session;
}

std::vector< session* > session_manager::findAllSessionByUID(uint32_t _uid) {
	
	std::vector< session* > v_s;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	for (auto& el : m_sessions)
#if defined(_WIN32)
		if (el->m_sock != INVALID_SOCKET && el->getUID() == _uid)
#elif defined(__linux__)
		if (el->m_sock.fd != INVALID_SOCKET && el->getUID() == _uid)
#endif
			v_s.push_back(el);

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return v_s;
}

session* session_manager::findSessionByNickname(std::string& _nickname) {
	
	session *s = nullptr;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	for (auto& el : m_sessions) {
#if defined(_WIN32)
		if (el->m_sock != INVALID_SOCKET && _nickname.compare(el->getNickname()) == 0) {
#elif defined(__linux__)
		if (el->m_sock.fd != INVALID_SOCKET && _nickname.compare(el->getNickname()) == 0) {
#endif
			s = el;
			break;
		}
	}

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return s;
}

session* session_manager::getSessionToDelete(DWORD _dwMilliseconds) {

	session *s = nullptr;

	try {

		s = m_session_del.get(_dwMilliseconds);

	}catch (exception& e) {

		if (!STDA_ERROR_CHECK_SOURCE_AND_ERROR(e.getCodeError(), STDA_ERROR_TYPE::LIST_ASYNC, 2/*TIME OUT*/))
			throw;

		// Time out error coloca o s == nullptr, para confirma que deu time out
		if (s != nullptr)
			s = nullptr;
	}

	return s;
}

void session_manager::checkSessionLive() {

	int conn_time = 0;

	for (auto& el : m_sessions) {

#if defined(_WIN32)
		if (el->m_sock != INVALID_SOCKET) {
#elif defined(__linux__)
		if (el->m_sock.fd != INVALID_SOCKET) {
#endif

			if (!el->isCreated()) {

				_smp::message_pool::getInstance().push(new message("[session_manager::checkSessionLive][Log] SESSION{IP=" + std::string(el->getIP()) + " session[UID="
						+ std::to_string(el->getUID()) + "]} esta sendo colocada na lista para ser desconectado, por que a session nao esta criada.", CL_FILE_LOG_AND_CONSOLE));

				//deleteSession(el);
				m_session_del.push_back(el);

			}else if ((conn_time = socket::getConnectTime(el->m_sock)) < 0) {

				_smp::message_pool::getInstance().push(new message("[session_manager::checkSessionLive][Log] SESSION{IP=" + std::string(el->getIP()) + " session[UID="
						+ std::to_string(el->getUID()) + "]} esta sendo colocada na lista para ser desconectado, por que ele nao esta mais connectada com o server. seu tempo de conexao[CONN_TIME=" 
						+ std::to_string(conn_time) + "].", CL_FILE_LOG_AND_CONSOLE));

				//deleteSession(el);
				m_session_del.push_back(el);

			}else if (!el->m_is_authorized && ((std::clock() - el->m_time_start) / (double)CLOCKS_PER_SEC) > (STDA_TIME_LIMIT_NON_AUTHORIZED / 1000.0)) {

				_smp::message_pool::getInstance().push(new message("[session_manager::checkSessionLive][Log] SESSION{IP=" + std::string(el->getIP()) + " session[UID="
						+ std::to_string(el->getUID()) + "]} esta sendo colocada na lista para ser desconectado, por que ele nao logou-se com o server no tempo limite.", CL_FILE_LOG_AND_CONSOLE));

				//deleteSession(el);
				m_session_del.push_back(el);

			}else if (m_TTL > 0 && ((std::clock() - el->m_tick) / (double)CLOCKS_PER_SEC) > (double)(m_TTL / 1000.0)) { // Close Session not send nothing in time to live)

#ifdef _DEBUG
				_smp::message_pool::getInstance().push(new message("[DEBUG MODO][NOT KICK SESSION][session_manager::checkSessionLive][Log] SESSION{IP=" + std::string(el->getIP()) + " session[UID="
						+ std::to_string(el->getUID()) + "]} esta sendo colocada na lista para ser desconectado, por que o server nao recebeu resposta dele no tempo limite. # " + std::to_string((double)(std::clock() - el->m_tick) / (double)CLOCKS_PER_SEC) + " > " + std::to_string(m_TTL / 1000.0), CL_FILE_LOG_AND_CONSOLE));
#else
				_smp::message_pool::getInstance().push(new message("[session_manager::checkSessionLive][Log] SESSION{IP=" + std::string(el->getIP()) + " session[UID="
						+ std::to_string(el->getUID()) + "]} esta sendo colocada na lista para ser desconectado, por que o server nao recebeu resposta dele no tempo limite.", CL_FILE_LOG_AND_CONSOLE));

				//deleteSession(el);
				m_session_del.push_back(el);
#endif // !_DEBUG

			}
		}
	}
}

bool session_manager::isFull() {
	
	bool ret = false;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	auto count = std::count_if(m_sessions.begin(), m_sessions.end(), [](auto& el) {
#if defined(_WIN32)
		return el->m_sock != INVALID_SOCKET;
#elif defined(__linux__)
		return el->m_sock.fd != INVALID_SOCKET;
#endif
	});

	ret = count == m_sessions.size();

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return  ret;
}

uint32_t session_manager::numSessionConnected() {
	return m_count;
}

bool session_manager::isInit() {
	return m_is_init;
}

void session_manager::config_init() {
	
	ReaderIni ri(INI_PATH);

	ri.init();

	m_TTL = ri.readInt("OPTION", "TTL");
}

int32_t session_manager::findSessionFree() {

	for (auto i = 0u; i < m_sessions.size(); ++i)
#if defined(_WIN32)
		if (m_sessions[i]->m_sock == INVALID_SOCKET)
#elif defined(__linux__)
		if (m_sessions[i]->m_sock.fd == INVALID_SOCKET)
#endif
			return (int)i;

	return -1;
}
