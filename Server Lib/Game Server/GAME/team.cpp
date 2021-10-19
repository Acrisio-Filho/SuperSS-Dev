// Arquivo team.cpp
// Criado em 02/11/2018 as 14:37 por Acrisio
// Implementa��o da classe Team

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(_WIN32)
#include <WinSock2.h>
#endif

#include "team.hpp"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"

#include <algorithm>

#define CHECK_SESSION(_method, __session) { \
	if ((__session) == nullptr) \
		throw exception("[Team::" + std::string((_method)) + "][Error] _player is invalid(nullptr)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TEAM, 1, 0)); \
	\
	if (!(__session)->getState() || !(__session)->isConnected()) \
		throw exception("[Team::" + std::string((_method)) + "][Error] _player is not connected.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TEAM, 2, 0)); \
} \

#define CHECK_SESSION_F(_method, __session, __option) { \
	if ((__session) == nullptr) \
		throw exception("[Team::" + std::string((_method)) + "][Error] _player is invalid(nullptr)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TEAM, 1, 0)); \
	\
	if ((!(__session)->getState() || !(__session)->isConnected()) && (__option) != 3/*Force*/) \
		throw exception("[Team::" + std::string((_method)) + "][Error] _player is not connected.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TEAM, 2, 0)); \
} \

using namespace stdA;

Team::Team(const int32_t _id) : m_id(_id), m_players(), m_team_ctx{0}, m_player_turn(nullptr) {

#if defined(_WIN32)
	InitializeCriticalSection(&m_cs);
#elif defined(__linux__)
	INIT_PTHREAD_MUTEXATTR_RECURSIVE;
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif
}

Team::~Team() {

	clear_players();

#if defined(_WIN32)
	DeleteCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_destroy(&m_cs);
#endif
}

void Team::addPlayer(player* _player) {
	CHECK_SESSION("addPlayer", _player);
	
#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	auto it = std::find_if(m_players.begin(), m_players.end(), [&](auto& _el) {
		return _el->m_pi.uid == _player->m_pi.uid;
	});

	if (it == m_players.end())	// Add um playe ao team
		m_players.push_back(_player);
	else
		_smp::message_pool::getInstance().push(new message("[Team::addPlayer][WARNING] player[UID=" + std::to_string(_player->m_pi.uid) + "] ja esta no team.", CL_FILE_LOG_AND_CONSOLE));

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif
}

void Team::deletePlayer(player* _player, int _option) {
	CHECK_SESSION_F("deletePlayer", _player, _option);

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	auto it = std::find_if(m_players.begin(), m_players.end(), [&](auto& _el) {
		return _el->m_pi.uid == _player->m_pi.uid;
	});

	if (it != m_players.end())	// deleta o player do map
		m_players.erase(it);
	else
		_smp::message_pool::getInstance().push(new message("[Team::deletePlayer][WARNING] player[UID=" + std::to_string(_player->m_pi.uid) + "] ja foi deletado o map ou nunca esteve no map.", 
				CL_FILE_LOG_AND_CONSOLE));

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif
}

// finders
player* Team::findPlayerByOID(int32_t _oid) {

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	auto it = std::find_if(m_players.begin(), m_players.end(), [&](auto& _el) {
		return _el->m_oid == _oid;
	});

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return (it != m_players.end()) ? *it : nullptr;
}

player* Team::findPlayerByUID(uint32_t _uid) {

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	auto it = std::find_if(m_players.begin(), m_players.end(), [&](auto& _el) {
		return _el->m_pi.uid == _uid;
	});

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return (it != m_players.end()) ? *it : nullptr;
}

player* Team::findPlayerByNickname(std::string& _nickname) {
	
#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	auto it = std::find_if(m_players.begin(), m_players.end(), [&](auto& _el) {
		return _nickname.compare(_el->m_pi.nickname) == 0;
	});

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return (it != m_players.end()) ? *it : nullptr;
}

std::vector< player* >& Team::getPlayers() {
	return m_players;
}

const uint32_t Team::getNumPlayers() {
	return (const uint32_t)m_players.size();
}

// Gets and Sets
const int32_t Team::getId() {
	return m_id;
}

void Team::setId(const int32_t _id) {
	m_id = _id;
}

const uint32_t Team::getPoint() {
	return m_team_ctx.point;
}

void Team::setPoint(const uint32_t _point) {
	m_team_ctx.point = _point;
}

const unsigned short Team::getDegree() {
	return m_team_ctx.degree;
}

void Team::setDegree(const unsigned short _degree) {
	m_team_ctx.degree = _degree;
}

const Location& Team::getLocation() {
	return m_team_ctx.location;
}

void Team::setLocation(const Location& _location) {
	m_team_ctx.location = _location;
}

const unsigned char Team::getAcertoHole() {
	return m_team_ctx.acerto_hole;
}

void Team::setAcertoHole(const unsigned char _acerto_hole) {
	m_team_ctx.acerto_hole = _acerto_hole;
}

const unsigned char Team::getHole() {
	return m_team_ctx.hole;
}

void Team::setHole(const unsigned char _hole) {
	m_team_ctx.hole = _hole;
}

const char Team::getGiveUp() {
	return m_team_ctx.data.giveup;
}

void Team::setGiveUp(const unsigned char _giveup) {
	m_team_ctx.data.giveup = _giveup;
}

const uint32_t Team::getTimeout() {
	return m_team_ctx.data.time_out;
}

void Team::setTimeout(const uint32_t _timeout) {
	m_team_ctx.data.time_out = _timeout;
}

const uint32_t Team::getTacadaNum() {
	return m_team_ctx.data.tacada_num;
}

void Team::setTacadaNum(const uint32_t _tacada_num) {
	m_team_ctx.data.tacada_num = _tacada_num;
}

const uint32_t Team::getTotalTacadaNum() {
	return m_team_ctx.data.total_tacada_num;
}

void Team::setTotalTacadaNum(const uint32_t _total_tacada_num) {
	m_team_ctx.data.total_tacada_num = _total_tacada_num;
}

const uint64_t Team::getPang() {
	return m_team_ctx.data.pang;
}

void Team::setPang(const uint64_t _pang) {
	m_team_ctx.data.pang = _pang;
}

const uint64_t Team::getBonusPang() {
	return m_team_ctx.data.bonus_pang;
}

void Team::setBonusPang(const uint64_t _bonus_pang) {
	m_team_ctx.data.bonus_pang = _bonus_pang;
}

const uint32_t Team::getBadCondute() {
	return m_team_ctx.data.bad_condute;
}

void Team::setBadCondute(const uint32_t _bad_condute) {
	m_team_ctx.data.bad_condute = _bad_condute;
}

const int32_t Team::getScore() {
	return m_team_ctx.data.score;
}

void Team::setScore(const int32_t _score) {
	m_team_ctx.data.score = _score;
}

const unsigned char Team::getLastWin() {
	return m_team_ctx.win;
}

void Team::setLastWin(const unsigned char _win) {
	m_team_ctx.win = _win;
}

const unsigned char Team::getPlayerStartHole() {
	return m_team_ctx.player_start_hole;
}

void Team::setPlayerStartHole(const unsigned char _player_start_hole) {
	m_team_ctx.player_start_hole = _player_start_hole;
}

const unsigned short Team::getStateFinish() {
	return m_team_ctx.finish;
}

void Team::setStateFinish(const unsigned short _finish) {
	m_team_ctx.finish = _finish;
}

const unsigned char Team::isQuit() {
	return m_team_ctx.quit;
}

void Team::setQuit(const unsigned char _quit) {
	m_team_ctx.quit = _quit;
}

// increment
void Team::incrementTacadaNum(uint32_t _inc) {
	m_team_ctx.data.tacada_num += _inc;
}

void Team::incrementTotalTacadaNum(uint32_t _inc) {
	m_team_ctx.data.total_tacada_num += _inc;
}

void Team::incrementPlayerStartHole(unsigned char _inc) {
	m_team_ctx.player_start_hole += _inc;
}

void Team::incrementPoint(uint32_t _inc) {
	m_team_ctx.point += _inc;
}

void Team::incrementBadCondute(uint32_t _inc) {
	m_team_ctx.data.bad_condute += _inc;
}

void Team::incrementPang(uint64_t _inc) {
	m_team_ctx.data.pang += _inc;
}

void Team::incrementBonusPang(uint64_t _inc) {
	m_team_ctx.data.bonus_pang += _inc;
}

// decrement
void Team::decrementPlayerStartHole(unsigned char _dec) {
	m_team_ctx.player_start_hole -= _dec;
}

const uint32_t Team::getCount() {
	return(const uint32_t)m_players.size();
}

player* Team::requestCalculePlayerTurn(uint32_t _seq_hole) {

	if (_seq_hole > 0)
		_seq_hole--;

	m_player_turn = m_players[((_seq_hole + m_team_ctx.player_start_hole) % m_players.size())];

	return m_player_turn;
}

void Team::sort_player(uint32_t _uid) {

	if (_uid == ~0u)
		throw exception("[Team::sort_player][Error] _uid is invalid(zero)", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TEAM, 4, 0));

	std::sort(m_players.begin(), m_players.end(), [&](auto& _el1, auto& _el2) {
		return _el1->m_pi.uid == _uid && _el2->m_pi.uid != _uid;
	});
}

void Team::clear_players() {

	if (!m_players.empty()) {
		m_players.clear();
		m_players.shrink_to_fit();
	}
}
