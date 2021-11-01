// Arquivo player.cpp
// Criado em 02/02/2021 as 17:17 por Acrisio
// Implementa��o da classe player

#pragma pack(1)
#include <WinSock2.h>
#include "player.hpp"

using namespace stdA;

player::player(threadpool_base& _threadpool) 
		: session(_threadpool), m_pi(), m_gg() {
}

player::~player() {
}

bool player::clear() {
	
	bool ret = true;

	if ((ret = session::clear())) {
		
		m_pi.clear();

		// Limpa todos os game guard ctx de todos os player desse server
		if (!m_gg.empty()) {

			for (auto& el : m_gg) {

				if (el.second == nullptr)
					continue;

				el.second->m_auth_reply = false;
				el.second->m_auth_time = 0;
				el.second->m_csa.Close();

				delete el.second;
			}

			m_gg.clear();
		}
	}

	return ret;
}

unsigned char player::getStateLogged() {
	return m_pi.m_state;
}

uint32_t player::getUID() {
	return m_pi.uid;
}

uint32_t player::getCapability() {
	return m_pi.tipo;
}

char* player::getNickname() {
	return m_pi.nickname;
}

char* player::getID() {
	return m_pi.id;
}

void player::addPlayerToGameGuard(unsigned long _uid) {

	if (_uid == ~0ul) {

		_smp::message_pool::getInstance().push(new message("[player::addPlayerToGameGuard][Errror] invalid _uid(~0ul)", CL_FILE_LOG_AND_CONSOLE));

		return;
	}

	// Replace, se existir
	auto old = m_gg.find(_uid);

	if (old != m_gg.end() && old->second != nullptr) {

		old->second->m_auth_reply = false;
		old->second->m_auth_time = 0;
		old->second->m_csa.Close();

		delete old->second;
	}

	// Add new
	m_gg[_uid] = new PlayerGameGuard();

	m_gg[_uid]->m_auth_reply = true;
	m_gg[_uid]->m_auth_time = 0;
	m_gg[_uid]->m_csa.Init();

	// Log
	_smp::message_pool::getInstance().push(new message("[player::addPlayerToGameGuard][Log] Player[SOCKET=" + std::to_string(_uid) + "] add Game Guard Context.", CL_FILE_LOG_AND_CONSOLE));
}

void player::removePlayerToGameGuard(unsigned long _uid) {

	if (_uid == ~0ul) {

		_smp::message_pool::getInstance().push(new message("[player::removePlayerToGameGuard][Errror] invalid _uid(~0ul)", CL_FILE_LOG_AND_CONSOLE));

		return;
	}

	auto it = m_gg.find(_uid);

	if (it != m_gg.end()) {

		it->second->m_auth_reply = false;
		it->second->m_auth_time = 0;
		it->second->m_csa.Close();

		// Remove from map
		m_gg.erase(it);

		// Log
		_smp::message_pool::getInstance().push(new message("[player::removePlayeToGameGuard][Log] Player[SOCKET=" + std::to_string(_uid) + "] removeu Game Guard Context.", CL_FILE_LOG_AND_CONSOLE));
	}
}

PlayerGameGuard* player::getPlayerGameGuard(unsigned long _uid) {
	
	if (_uid == ~0ul) {

		_smp::message_pool::getInstance().push(new message("[player::getPlayerGameGuard][Errror] invalid _uid(~0ul)", CL_FILE_LOG_AND_CONSOLE));

		return (PlayerGameGuard*)nullptr;
	}

	auto it = m_gg.find(_uid);

	return (it != m_gg.end() ? it->second : (PlayerGameGuard*)nullptr);
}
