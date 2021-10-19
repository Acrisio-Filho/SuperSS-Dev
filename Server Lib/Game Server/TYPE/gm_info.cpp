// Arquivo gm_info.cpp
// Criado em 28/07/2018 as 16:41 por Acrisio
// Implementa��o da classe GMInfo

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "gm_info.hpp"
#include "../../Projeto IOCP/UTIL/exception.h"
#include "../../Projeto IOCP/TYPE/stda_error.h"
#include "../../Projeto IOCP/UTIL/message_pool.h"

using namespace stdA;

GMInfo::GMInfo() : m_uid(0u), visible(0u), /*Deixa o GM invis�vel, depois ele fica vis�vel se ele quiser com o comando*/ whisper(1u), channel(0u) {
	
#if defined(_WIN32)
	InitializeCriticalSection(&m_cs);
#elif defined(__linux__)
	INIT_PTHREAD_MUTEXATTR_RECURSIVE;
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif
}

GMInfo::~GMInfo() {
	clear();

#if defined(_WIN32)
	DeleteCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_destroy(&m_cs);
#endif
}

void GMInfo::clear() {

	m_uid = 0u;

	visible = 0u;	/*Deixa o GM invis�vel, depois ele fica vis�vel se ele quiser com o comando*/
	whisper = 1u;
	channel = 0u;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	if (!map_open.empty())
		map_open.clear();

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif
}

void GMInfo::openPlayerWhisper(uint32_t _uid) {

	if (_uid == 0)
		throw exception("[GMInfo::openPlayerWhisper][Error] GM[UID=" + std::to_string(m_uid) + "] tentou adicionar player[UID=" 
				+ std::to_string(_uid) + "] a lista de whisper, mas o _uid eh invalido. Hacker ou Bug.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GM_INFO, 1, 0));

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	auto it = map_open.find(_uid);

	if (it == map_open.end())
		map_open[_uid] = true;
	else
		_smp::message_pool::getInstance().push(new message("[GMInfo::openPlayerWhisper][WARNING] GM[UID=" + std::to_string(m_uid) + "] tentou add player[UID=" 
				+ std::to_string(_uid) + "] a lista de whisper abertos, mas ele ja esta na lista", CL_FILE_LOG_AND_CONSOLE));

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif
}

void GMInfo::closePlayerWhisper(uint32_t _uid) {

	if (_uid == 0)
		throw exception("[GMInfo::openPlayerWhisper][Error] GM[UID=" + std::to_string(m_uid) + "] tentou excluir player[UID="
			+ std::to_string(_uid) + "] da lista de whisper, mas o _uid eh invalido. Hacker ou Bug.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GM_INFO, 1, 0));

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	auto it = map_open.find(_uid);

	if (it != map_open.end())
		map_open.erase(it);
	else
		_smp::message_pool::getInstance().push(new message("[[GMInfo::openPlayerWhisper][WARNING] GM[UID=" + std::to_string(m_uid) + "] tentou excluir player[UID="
				+ std::to_string(_uid) + "] da lista de whisper, mas ele nao esta na lista.", CL_FILE_LOG_AND_CONSOLE));

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif
}

bool GMInfo::isOpenPlayerWhisper(uint32_t _uid) {

	bool ret = false;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	for (auto& el : map_open) {
		if (el.first == _uid) {
			ret = true;
			break;
		}
	}

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return ret;
}

void GMInfo::setGMUID(uint32_t _uid) {

	if (_uid == 0)
		throw exception("[GMInfo::setGMUID][Error] GM[UID=" + std::to_string(m_uid) + "] tentou setar o UID do GM para UID[value=" 
				+ std::to_string(_uid) + "], mas o uid eh invalido. Hacker ou Bug.", STDA_MAKE_ERROR(STDA_ERROR_TYPE::GM_INFO, 1, 0));

	m_uid = _uid;
}
