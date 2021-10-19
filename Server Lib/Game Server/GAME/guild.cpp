// Arquivo guild.cpp
// Criado em 29/12/2019 as 11:51 por Acrisio
// Implementa��o da classe Guild

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "guild.hpp"

using namespace stdA;

Guild::Guild(uint32_t _ul) : v_players(), m_team(eTEAM::RED), m_uid(0u), m_point(0u), m_pang(0ull), m_pang_win(0ull) {

#if defined(_WIN32)
	InitializeCriticalSection(&m_cs);
#elif defined(__linux__)
	INIT_PTHREAD_MUTEXATTR_RECURSIVE;
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif
}

Guild::Guild(uint32_t _uid, eTEAM _team) : v_players(), m_team(_team), m_uid(_uid), m_point(0u), m_pang(0ull), m_pang_win(0ull) {

#if defined(_WIN32)
	InitializeCriticalSection(&m_cs);
#elif defined(__linux__)
	INIT_PTHREAD_MUTEXATTR_RECURSIVE;
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif
}

Guild::~Guild() {

	clear();

#if defined(_WIN32)
	DeleteCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_destroy(&m_cs);
#endif
}

void Guild::clear() {

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	if (!v_players.empty()) {
		v_players.clear();
		v_players.shrink_to_fit();
	}

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	m_team = eTEAM::RED;
	m_uid = 0u;
	m_point = 0u;
	m_pang = 0ull;
	m_pang_win = 0ull;
}

Guild::eTEAM Guild::getTeam() {
	return m_team;
}

uint32_t Guild::getUID() {
	return m_uid;
}

unsigned short Guild::getPoint() {
	return m_point;
}

uint32_t Guild::getPangWin() {
	return m_pang_win;
}

uint64_t Guild::getPang() {
	return m_pang;
}

void Guild::setTeam(eTEAM _team) {
	m_team = _team;
}

void Guild::setUID(uint32_t _uid) {
	m_uid = _uid;
}

void Guild::setPoint(unsigned short _point) {
	m_point = _point;
}

void Guild::setPangWin(uint32_t _pang_win) {
	m_pang_win = _pang_win;
}

void Guild::setPang(uint64_t _pang) {
	m_pang = _pang;
}

void Guild::addPlayer(player& _session) {

	if (findPlayerByUID(_session.m_pi.uid) != nullptr) {

		// Log
		_smp::message_pool::getInstance().push(new message("[Guild::addPlayer][WARNING] tentou adicionar o player[UID=" + std::to_string(_session.m_pi.uid) 
				+ "] na guild, mas ele ja existe na guild. Bug", CL_FILE_LOG_AND_CONSOLE));

		return;
	}

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	v_players.push_back(&_session);

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif
}

void Guild::deletePlayer(player *_session) {

	if (_session == nullptr) {

		_smp::message_pool::getInstance().push(new message("[Guild::deletePlayer][Error] _session is invalid(nullptr). Bug", CL_FILE_LOG_AND_CONSOLE));

		return;
	}

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	auto it = std::find(v_players.begin(), v_players.end(), _session);

	if (it != v_players.end())
		v_players.erase(it);
	else
		_smp::message_pool::getInstance().push(new message("[Guild::deletePlayer][WARNING] Player[UID=" + std::to_string(_session->m_pi.uid) 
				+ "] ja foi deletado do vector. Bug", CL_FILE_LOG_AND_CONSOLE));

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif
}

player* Guild::findPlayerByUID(uint32_t _uid) {
	
	// Invalid _uid
	if (_uid == 0u) {
		
		_smp::message_pool::getInstance().push(new message("[Guild::findPlayerByUID][Error] _uid is invalid(zero). Bug", CL_FILE_LOG_AND_CONSOLE));

		return nullptr;
	}

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif
	
	auto it = std::find_if(v_players.begin(), v_players.end(), [&](auto& _el) {
		return (_el != nullptr && _el->m_pi.uid == _uid);
	});

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return (it != v_players.end() ? *it : nullptr);
}

player* Guild::findPlayerByOID(uint32_t _oid) {
	
	// Invalid _uid
	if (_oid == 0u) {

		_smp::message_pool::getInstance().push(new message("[Guild::findPlayerByUID][Error] _oid is invalid(zero). Bug", CL_FILE_LOG_AND_CONSOLE));

		return nullptr;
	}

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	auto it = std::find_if(v_players.begin(), v_players.end(), [&](auto& _el) {
		return (_el != nullptr && _el->m_oid == _oid);
	});

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return (it != v_players.end() ? *it : nullptr);
}

player* Guild::getPlayerByIndex(uint32_t _index) {

	if (_index > v_players.size()) {

		_smp::message_pool::getInstance().push(new message("[Guild::getPlayerByIndex][Error] index[VALUE=" + std::to_string(_index) 
				+ "] is invalid(out_of_bounds)", CL_FILE_LOG_AND_CONSOLE));

		return nullptr;
	}

	player *p = nullptr;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	p = v_players[_index];

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return p;
}

uint32_t Guild::numPlayers() {
	return (uint32_t)v_players.size();
}