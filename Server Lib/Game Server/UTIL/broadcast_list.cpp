// Arquivo broadcast_list.cpp
// Criado em 03/12/2018 as 23:08 por Acrisio
// Implementa��o da classe BroadcastList

#if defined(_WIN32)
#pragma pack(1)
#endif

#include "broadcast_list.hpp"

#include <ctime>

using namespace stdA;

BroadcastList::BroadcastList(uint32_t _interval_time_second) : m_interval(_interval_time_second), m_last_peek(0u) {

#if defined(_WIN32)
	InitializeCriticalSection(&m_cs);
#elif defined(__linux__)
	INIT_PTHREAD_MUTEXATTR_RECURSIVE;
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif
}

BroadcastList::~BroadcastList() {

	if (!m_list.empty())
		m_list.clear();

#if defined(_WIN32)
	DeleteCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_destroy(&m_cs);
#endif
}

void BroadcastList::push_back(int32_t _time, std::string _notice, TYPE _type) {

	NoticeCtx nc = { (uint32_t)((_time < 0) ? 0 : _time), _notice, _type };

	push_back(nc);
}

void BroadcastList::push_back(int32_t _time, std::string _notice, uint32_t _option, TYPE _type) {

	NoticeCtx nc = { (uint32_t)((_time < 0) ? 0 : _time), _notice, _option, _type };

	push_back(nc);
}

void BroadcastList::push_back(int32_t _time, std::string _nickname, std::string _notice, TYPE _type) {

	NoticeCtx nc = { (uint32_t)((_time < 0) ? 0 : _time), _nickname, _notice, _type };

	push_back(nc);
}

void BroadcastList::push_back(NoticeCtx _nc) {
	
#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	m_list.insert(std::make_pair(_nc.time_second, _nc));

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif
}

BroadcastList::RetNoticeCtx BroadcastList::peek() {

	RetNoticeCtx rnc{ 0 };

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	if (!m_list.empty()) {

		if ((std::time(nullptr) - m_last_peek) >= m_interval) {

			if (m_list.begin()->second.time_second == 0u || m_list.begin()->second.time_second <= std::time(nullptr)) {

				// Peek new Notice
				rnc.nc = m_list.begin()->second;

				// Remove o primeiro da lista
				m_list.erase(m_list.begin());

				rnc.ret = RET_TYPE::OK;

				m_last_peek = (uint32_t)std::time(nullptr);

			}else // Ainda n�o deu o tempo de mostrar o notice no tempo dele
				rnc.ret = RET_TYPE::WAIT;
		
		}else // Ainda n�o deu o intervalo para pegar a pr�xima notice da fila
			rnc.ret = RET_TYPE::WAIT;

	}else // A List est� vazia
		rnc.ret = RET_TYPE::NO_NOTICE;

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return rnc;
}

uint32_t BroadcastList::getSize() {

	size_t count = 0u;

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	count = m_list.size();

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return (uint32_t)count;
}

// NoticeCtx
BroadcastList::NoticeCtx::NoticeCtx(uint32_t _ul) {
	clear();
}

BroadcastList::NoticeCtx::NoticeCtx(uint32_t _time_second, std::string _notice, TYPE _type) : nickname(""), option(0u) {
	
	type = _type;
	time_second = _time_second;

	notice = _notice;
}

BroadcastList::NoticeCtx::NoticeCtx(uint32_t _time_second, std::string _notice, uint32_t _option, TYPE _type) : nickname("") {

	type = _type;
	option = _option;
	time_second = _time_second;

	notice = _notice;
}

BroadcastList::NoticeCtx::NoticeCtx(uint32_t _time_second, std::string _nickname, std::string _notice, TYPE _type) : option(0u) {

	type = _type;
	time_second = _time_second;

	nickname = _nickname;
	notice = _notice;
}

void BroadcastList::NoticeCtx::clear() {
	
	type = TYPE::GM_NOTICE;
	time_second = 0u;

	if (!notice.empty()) {
		notice.clear();
		notice.shrink_to_fit();
	}
}

// RetNotice
BroadcastList::RetNoticeCtx::RetNoticeCtx(uint32_t _ul) {
	clear();
}

void BroadcastList::RetNoticeCtx::clear() {

	ret = RET_TYPE::NO_NOTICE;

	nc.clear();
}
