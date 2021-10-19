// Arquivo message_pool.cpp
// Criado em 21/05/2017 por Acrisio
// Implementação da classe message_pool

#if defined(_WIN32)
#pragma pack(1)
#endif

#if defined(__linux__)
	#include "WinPort.h"
#endif

#include "message_pool.h"
#include "exception.h"
#include <iostream>
#include "../TYPE/stda_error.h"

using namespace stdA;

//list_fifo_console_asyc< message > _smp::message_pool;

message_pool::message_pool() {
    init();
};

message_pool::~message_pool() {
	destroy();

	if (!m_messages.empty()) {
        
        for (size_t i = 0; i < m_messages.size(); i++)
            if (m_messages[i] != nullptr)
                delete m_messages[i];

        m_messages.clear();
        m_messages.shrink_to_fit();
    }
};

void message_pool::init() {
#if defined(_WIN32)
	InitializeConditionVariable(&cv);
    InitializeCriticalSection(&cs);
    InitializeCriticalSection(&cs_console);
#elif defined(__linux__)
	pthread_cond_init(&cv, nullptr);

	INIT_PTHREAD_MUTEXATTR_RECURSIVE;
	INIT_PTHREAD_MUTEX_RECURSIVE(&cs);
	INIT_PTHREAD_MUTEX_RECURSIVE(&cs_console);
	DESTROY_PTHREAD_MUTEXATTR_RECURSIVE;
#endif
};

void message_pool::destroy() {
#if defined(_WIN32)
	DeleteCriticalSection(&cs);
    DeleteCriticalSection(&cs_console);
#elif defined(__linux__)
	pthread_mutex_destroy(&cs);
	pthread_mutex_destroy(&cs_console);
	pthread_cond_destroy(&cv);
#endif
};

void message_pool::console_log() {
    message* m = getMessage();

    if (m != nullptr) {
#if defined(_WIN32)
        EnterCriticalSection(&cs_console);
#elif defined(__linux__)
		pthread_mutex_lock(&cs_console);
#endif
        
        std::cout << m->get() << std::endl;

#if defined(_WIN32)
        LeaveCriticalSection(&cs_console);
#elif defined(__linux__)
		pthread_mutex_unlock(&cs_console);
#endif

        delete m;
	}else
		throw exception("Message is null. message_pool::console_log()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_POOL, 1, 0));
};

void message_pool::push(message *m) {
	push_back(m);
};

void message_pool::push_front(message *m) {
#if defined(_WIN32)
	EnterCriticalSection(&cs);
#elif defined(__linux__)
	pthread_mutex_lock(&cs);
#endif

	m_messages.push_front(m);

#if defined(_WIN32)
	LeaveCriticalSection(&cs);

	WakeConditionVariable(&cv);
#elif defined(__linux__)
	pthread_mutex_unlock(&cs);

	pthread_cond_signal(&cv);
#endif
};

void message_pool::push_back(message *m) {
#if defined(_WIN32)
	EnterCriticalSection(&cs);
#elif defined(__linux__)
	pthread_mutex_lock(&cs);
#endif

	m_messages.push_back(m);

#if defined(_WIN32)
	LeaveCriticalSection(&cs);

	WakeConditionVariable(&cv);
#elif defined(__linux__)
	pthread_mutex_unlock(&cs);

	pthread_cond_signal(&cv);
#endif
};

message* message_pool::getMessage() {
	return getFirstMessage();
};

message* message_pool::getFirstMessage() {
    message *m = nullptr;

#if defined(_WIN32)
	EnterCriticalSection(&cs);
#elif defined(__linux__)
	pthread_mutex_lock(&cs);
#endif

	while (m_messages.empty()) {
#if defined(_WIN32)
		if (SleepConditionVariableCS(&cv, &cs, INFINITE) == 0 && GetLastError() != ERROR_TIMEOUT) {
			LeaveCriticalSection(&cs);
			throw exception("Erro ao recuperar o signaled event. message_pool::getFirstMessage()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_POOL, 2, 0));
		}
#elif defined(__linux__)

		int32_t error = 0;

		if ((error = pthread_cond_wait(&cv, &cs)) != 0) {
			pthread_mutex_unlock(&cs);
			throw exception("Erro ao recuperar o signaled event. message_pool::getFirstMessage()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_POOL, 2, error));
		}
#endif
	}
	
	m = m_messages.front();
	m_messages.pop_front();

#if defined(_WIN32)
	LeaveCriticalSection(&cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&cs);
#endif

    return m;
};

message* message_pool::getLastMessage() {
    message *m = nullptr;

#if defined(_WIN32)
	EnterCriticalSection(&cs);
#elif defined(__linux__)
	pthread_mutex_lock(&cs);
#endif

	while (m_messages.empty()) {
#if defined(_WIN32)
		if (SleepConditionVariableCS(&cv, &cs, INFINITE) == 0 && GetLastError() != ERROR_TIMEOUT) {
			LeaveCriticalSection(&cs);
			throw exception("Erro ao recuperar o signaled event. message_pool::getLastMessage()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_POOL, 3, 0));
		}
#elif defined(__linux__)

		int32_t error = 0;

		if ((error = pthread_cond_wait(&cv, &cs)) != 0) {
			pthread_mutex_unlock(&cs);
			throw exception("Erro ao recuperar o signaled event. message_pool::getLastMessage()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_POOL, 3, error));
		}
#endif
	}
	
	m = m_messages.back();
	m_messages.pop_back();

#if defined(_WIN32)
	LeaveCriticalSection(&cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&cs);
#endif

    return m;
};

message* message_pool::peekMessage() {
	return peekFirstMessage();
};

message* message_pool::peekFirstMessage() {
    message *m = nullptr;

#if defined(_WIN32)
	EnterCriticalSection(&cs);
#elif defined(__linux__)
	pthread_mutex_lock(&cs);
#endif

	while (m_messages.empty()) {
#if defined(_WIN32)
		if (SleepConditionVariableCS(&cv, &cs, INFINITE) == 0 && GetLastError() != ERROR_TIMEOUT) {
			LeaveCriticalSection(&cs);
			throw exception("Erro ao recuperar o signaled event. message_pool::peekFirstMessage()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_POOL, 4, 0));
		}
#elif defined(__linux__)
		
		int32_t error = 0;

		if ((error = pthread_cond_wait(&cv, &cs)) != 0) {
			pthread_mutex_unlock(&cs);
			throw exception("Erro ao recuperar o signaled event. message_pool::peekFirstMessage()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_POOL, 4, error));
		}
#endif
	}
	
	m = m_messages.front();

#if defined(_WIN32)
	LeaveCriticalSection(&cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&cs);
#endif

    return m;
};

message* message_pool::peekLastMessage() {
    message *m = nullptr;

#if defined(_WIN32)
	EnterCriticalSection(&cs);
#elif defined(__linux__)
	pthread_mutex_lock(&cs);
#endif

	while (m_messages.empty()) {
#if defined(_WIN32)
		if (SleepConditionVariableCS(&cv, &cs, INFINITE) == 0 && GetLastError() != ERROR_TIMEOUT) {
			LeaveCriticalSection(&cs);
			throw exception("Erro ao recuperar o signaled event. message_pool::peekLastMessage()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_POOL, 5, 0));
		}
#elif defined(__linux__)

		int32_t error = 0;

		if ((error = pthread_cond_wait(&cv, &cs)) != 0) {
			pthread_mutex_unlock(&cs);
			throw exception("Erro ao recuperar o signaled event. message_pool::peekLastMessage()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::MESSAGE_POOL, 5, error));
		}
#endif
	}
	
	m = m_messages.back();

#if defined(_WIN32)
	LeaveCriticalSection(&cs);
#elif defined(__linux__)
	pthread_mutex_unlock(&cs);
#endif

    return m;
};