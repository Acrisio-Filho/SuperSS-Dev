// Arquivo timer_manager.cpp
// Criado em 01/01/2018 por Acrisio
// Implementação da classe timer_manager

#if defined(_WIN32)
#pragma pack(1)
#include <WinSock2.h>
#elif defined(__linux__)
#include "../UTIL/WinPort.h"
#endif

#include "timer_manager.h"
#include "../UTIL/exception.h"
#include "../UTIL/message_pool.h"
#include "../UTIL/hex_util.h"

using namespace stdA;

timer_manager::timer_manager() 
	:
#if defined(_WIN32)
	m_hTimerQueue(INVALID_HANDLE_VALUE),
	m_event(INVALID_HANDLE_VALUE)
#elif defined(__linux__)
	m_hTimerQueue(nullptr),
	m_event(nullptr)
#endif
	, m_timers()
{

#if defined(_WIN32)
	if ((m_hTimerQueue = CreateTimerQueue()) == nullptr)
		throw exception("[timer_manager::timer_manager][Error] Ao criar TimerQueue", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TIMER_MANAGER, 1, GetLastError()));

	if ((m_event = CreateEvent(NULL, TRUE, FALSE, NULL)) == INVALID_HANDLE_VALUE)
		throw exception("[timer_manager::timer_manager][Error] Ao criar evento", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TIMER_MANAGER, 5, GetLastError()));

	InitializeCriticalSection(&m_cs);
#elif defined(__linux__)

	if ((m_hTimerQueue = QueueTimer::CreateTimerQueue()) == nullptr)
		throw exception("[timer_manager::timer_manager][Error] Ao criar TimerQueue", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TIMER_MANAGER, 1, errno));

	m_event = new Event(true, 0u);

	if (!m_event->is_good()) {

		delete m_event;

		m_event = nullptr;

		throw exception("[timer_manager::timer_manager][Error] Ao criar evento", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TIMER_MANAGER, 5, errno));
	}

	INIT_PTHREAD_MUTEXATTR_RECURSIVE;
	INIT_PTHREAD_MUTEX_RECURSIVE(&m_cs);
	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif
};

timer_manager::~timer_manager() {

	clear_timers();

#if defined(_WIN32)
	if (m_hTimerQueue != INVALID_HANDLE_VALUE)
		DeleteTimerQueueEx(m_hTimerQueue, m_event);
#elif defined(__linux__)
	if (m_hTimerQueue != nullptr)
		QueueTimer::DeleteTimerQueueEx(m_hTimerQueue, m_event);
#endif

	m_hTimerQueue = nullptr;

	try {

		check_event_end();
	
	}catch (exception& e) {
#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message("[timer_manager::~timer_manager][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
#else
		_smp::message_pool::getInstance().push(new message("[timer_manager::~timer_manager][ErrorSystem] " + e.getFullMessageError(), CL_ONLY_FILE_LOG));
#endif
	}

#if defined(_WIN32)
	if (m_event != INVALID_HANDLE_VALUE)
		CloseHandle(m_event);

	m_event = INVALID_HANDLE_VALUE;

	DeleteCriticalSection(&m_cs);
#elif defined(__linux__)

	if (m_event != nullptr)
		delete m_event;

	m_event = nullptr;

	pthread_mutex_destroy(&m_cs);
#endif
};

timer *timer_manager::createTimer(DWORD _time, timer::timer_param *_arg, uint32_t _tipo) {
	
	timer *_timer = nullptr;

	try {

		_timer = new timer(m_hTimerQueue, _time, _arg, _tipo);
	
	}catch (exception& e) {

		if (_timer != nullptr)
			delete _timer;

		_timer = nullptr;

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message(e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
#else
		_smp::message_pool::getInstance().push(new message(e.getFullMessageError(), CL_ONLY_FILE_LOG));
#endif
	}

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	m_timers.push_back(_timer);

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return _timer;
};

timer *timer_manager::createTimer(DWORD _time, timer::timer_param *_arg, std::vector< DWORD > _table_interval, uint32_t _tipo) {
	timer *_timer = nullptr;

	try {

		_timer = new timer(m_hTimerQueue, _time, _arg, _table_interval, _tipo);
	
	}catch (exception& e) {
	
		if (_timer != nullptr)
			delete _timer;

		_timer = nullptr;

#ifdef _DEBUG
		_smp::message_pool::getInstance().push(new message(e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
#else
		_smp::message_pool::getInstance().push(new message(e.getFullMessageError(), CL_ONLY_FILE_LOG));
#endif
	}

#if defined(_WIN32)
	EnterCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_lock(&m_cs);
#endif

	m_timers.push_back(_timer);

#if defined(_WIN32)
	LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&m_cs);
#endif

	return _timer;
};

void timer_manager::deleteTimer(timer *_timer) {


	size_t index = 0u;

	try {

#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		if (_timer == nullptr)
			throw exception("[timer_manager::deleteTimer][Error] timer *_timer is nullptr", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TIMER_MANAGER, 3, 1));

		// Para o Tempo se ele não estiver parado
		if (_timer->getState() != timer::STOPPED)
			_timer->stop();

		// Adiciona a o Deleta Timer, por que antes só estava tirando do vector
		if ((index = findIndexTimer(_timer)) != ~0) {

			if (m_timers[index] != nullptr)
				delete m_timers[index];

			m_timers.erase(m_timers.begin() + index);
			m_timers.shrink_to_fit();

		}else
	#ifdef _DEBUG
			_smp::message_pool::getInstance().push(new message("[timer_manager::deleteTimer][Error] Ao deletar _timer: 0x" + hex_util::lltoaToHex((size_t)_timer) + ", timer no has more in vector. ErrorCode: "
				+ std::to_string(STDA_MAKE_ERROR(STDA_ERROR_TYPE::TIMER_MANAGER, 4, 0)), CL_FILE_LOG_AND_CONSOLE));
	#else
			_smp::message_pool::getInstance().push(new message("[timer_manager::deleteTimer][Error] Ao deletar _timer: 0x" + hex_util::lltoaToHex((size_t)_timer) + ", timer no has more in vector. ErrorCode: "
				+ std::to_string(STDA_MAKE_ERROR(STDA_ERROR_TYPE::TIMER_MANAGER, 4, 0)), CL_ONLY_FILE_LOG));
	#endif

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

	}catch (exception& e) {

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		_smp::message_pool::getInstance().push(new message("[timer_manager::deleteTimer][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

bool timer_manager::isEmpty() {
	return m_timers.empty();
};

void timer_manager::check_event_end() {
	
#if defined(_WIN32)
	if (WaitForSingleObject(m_event, INFINITE) != WAIT_OBJECT_0)
		throw exception("[timer_manager::check_event_end][Error] Nao conseguiu pegar sinal do evento", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TIMER_MANAGER, 2, GetLastError()));

	ResetEvent(m_event);
#elif defined(__linux__)
	if (m_event == nullptr)
		return;

	if (m_event->wait(INFINITE) != WAIT_OBJECT_0)
		throw exception("[timer_manager::check_event_end][Error] Nao conseguiu pegar sinal do evento", STDA_MAKE_ERROR(STDA_ERROR_TYPE::TIMER_MANAGER, 2, errno));

	m_event->reset();
#endif
};

void timer_manager::clear_timers() {
	timer *_timer = nullptr;

	try {

#if defined(_WIN32)
		EnterCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_lock(&m_cs);
#endif

		while (!m_timers.empty()) {

			if ((_timer = m_timers.front()) != nullptr)
				delete _timer;

			m_timers.erase(m_timers.begin());
		}

		m_timers.shrink_to_fit();

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

	}catch (exception& e) {

#if defined(_WIN32)
		LeaveCriticalSection(&m_cs);
#elif defined(__linux__)
		pthread_mutex_unlock(&m_cs);
#endif

		_smp::message_pool::getInstance().push(new message("[timer_manager::clear_timers][ErrorSystem] " + e.getFullMessageError(), CL_FILE_LOG_AND_CONSOLE));
	}
};

size_t timer_manager::findIndexTimer(timer *_timer) {
	
	if (_timer == nullptr)
		return ~0;

	size_t ret = ~0u, i = 0u;

	for (; i < m_timers.size(); ++i)
		if (m_timers[i] == _timer)
			return i;

	return ret;
};
